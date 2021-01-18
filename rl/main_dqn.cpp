#include "main_dqn.h"
#include "../helpers/logger.h"
#include "./agents/actor_critic.h"
#include "multi_agent.h"
#include "../gym/lethal_gym.h"
#include <torch/torch.h>
#include <algorithm>
#include <Windows.h>
#include <thread>

namespace rl {
    DWORD WINAPI LearnStub(void *ptr) {
        if (!ptr) return -1;
        Agent* agent = ((Agent*) ptr);
        while (true) {
            agent->Learn();
        }
        return 0;
    }

    void MainDqn::Train() {
        gym::LethalGym* lethal_gym = new gym::LethalGym();
        std::vector<float> scores_p1;
        std::vector<float> scores_p2;
        int n_games = 5000;
        //std::vector<float> eps_history;

        MultiAgent multi_agent = MultiAgent(2000, 10000, 1000, 20, 0.5, true, true);

        helpers::Logger *logger = new helpers::Logger("./lethalai_log.txt");
        logger->WriteLine("Started Training");

        //CreateThread(NULL, 0, LearnStub, agent, 0, NULL);
        for (int i = 0; i < n_games; i++) {
            float score_p1 = 0.0f;
            float score_p2 = 0.0f;

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

                std::vector<float> _reward{ reward[0], reward[1] };

                multi_agent.StoreTransitions(observation, actions, _reward,
                        new_observation, done);
                multi_agent.Learn();

                observation = new_observation;
            }

            scores_p1.push_back(score_p1);
            scores_p2.push_back(score_p2);
            //eps_history.push_back(dqn_agent->epsilon);

            float average_score_p1 = 0.0f;
            float average_score_p2 = 0.0f;
            if (scores_p1.size() <= 100) {
                average_score_p1 = 1.0 * std::accumulate(
                        scores_p1.begin(), scores_p1.end(), 0.0) / scores_p1.size();
                average_score_p2 = 1.0 * std::accumulate(
                        scores_p2.begin(), scores_p2.end(), 0.0) / scores_p2.size();
            } else {
                average_score_p1 = 1.0 * std::accumulate(
                        scores_p1.end() - 100, scores_p1.end(), 0.0) / scores_p1.size();
                average_score_p2 = 1.0 * std::accumulate(
                        scores_p2.end() - 100, scores_p2.end(), 0.0) / scores_p2.size();
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
