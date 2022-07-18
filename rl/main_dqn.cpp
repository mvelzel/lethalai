#include "main_dqn.h"
#include "../helpers/logger.h"
#include "./agents/actor_critic.h"
#include "multi_agent.h"
#include "../gym/lethal_gym.h"
#include <torch/torch.h>
#include <algorithm>
#include <Windows.h>
#include <thread>
#include "../graphics/drawables/graph.h"

namespace rl {
    void MainDqn::Train() {
        gym::LethalGym* lethal_gym = new gym::LethalGym();
        std::vector<float> scores_p1;
        std::vector<float> scores_p2;
        int n_games = 5000;
        //std::vector<float> eps_history;

        MultiAgent multi_agent = MultiAgent(2000, 10000, 1000, 10, 0.5, lethal_gym, false, false);

        graphics::Graph* graph = new graphics::Graph(0, 420, 50.0f, 2500.0f, 500.0f,
                "Average Score");
        lethal_gym->graphics_handler->AddDrawable(graph);

        helpers::Logger *logger = new helpers::Logger("./lethalai_log.txt");
        logger->WriteLine("Started Training");

        for (int i = 0; i < n_games; i++) {
            float score_p1 = 0.0f;
            float score_p2 = 0.0f;
            lethal_gym->info_list->SetEpisode(i);
            lethal_gym->info_list->SetScoreP1(score_p1);
            lethal_gym->info_list->SetScoreP2(score_p2);
            lethal_gym->info_list->SetRecentP1(0.0f);
            lethal_gym->info_list->SetRecentP2(0.0f);

            bool done = false;

            environment::GameState* observation = lethal_gym->Reset();

            while (!done) {
                std::vector<std::vector<environment::InputAction>> actions =
                    multi_agent.ChooseActions(observation);

                auto result = lethal_gym->Step(actions);

                environment::GameState* new_observation = std::get<0>(result);
                float* reward = std::get<1>(result);
                done = std::get<2>(result);

                score_p1 += reward[0];
                score_p2 += reward[1];
                lethal_gym->info_list->SetScoreP1(score_p1);
                lethal_gym->info_list->SetScoreP2(score_p2);

                std::vector<float> _reward{ reward[0], reward[1] };
                multi_agent.StoreTransitions(observation, actions, _reward,
                        new_observation, done);
                multi_agent.Learn();
                observation = new_observation;

            }

            scores_p1.push_back(score_p1);
            scores_p2.push_back(score_p2);

            float average_score_p1 = 0.0f;
            float average_score_p2 = 0.0f;
            if (scores_p1.size() <= 25) {
                average_score_p1 = 1.0 * std::accumulate(
                        scores_p1.begin(), scores_p1.end(), 0.0) / scores_p1.size();
                average_score_p2 = 1.0 * std::accumulate(
                        scores_p2.begin(), scores_p2.end(), 0.0) / scores_p2.size();
            } else {
                average_score_p1 = 1.0 * std::accumulate(
                        scores_p1.end() - 25, scores_p1.end(), 0.0) / scores_p1.size();
                average_score_p2 = 1.0 * std::accumulate(
                        scores_p2.end() - 25, scores_p2.end(), 0.0) / scores_p2.size();
            }
            lethal_gym->info_list->SetAverageP1(average_score_p1);
            lethal_gym->info_list->SetAverageP2(average_score_p2);

            graph->AddData(average_score_p1, 0);
            graph->AddData(average_score_p2, 1);
            if (scores_p1.size() == 1) {
                graph->AddData(average_score_p1, 0);
                graph->AddData(average_score_p2, 1);
            }

            std::ostringstream stream;
            stream << "episode " << i << " score p1 " << score_p1 <<
                " score p2 " << score_p2 << " average score p1 " << average_score_p1 <<
                " average score p2 " << average_score_p2;
            logger->WriteLine(stream.str());
            printf("episode %i score p1 %.2f score p2 %.2f \
average score p1 %.2f average score p2 %.2f\n",
                    i, score_p1, score_p2,
                    average_score_p1, average_score_p2);
        }
    }
}
