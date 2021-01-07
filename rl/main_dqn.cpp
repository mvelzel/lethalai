#include "main_dqn.h"
#include "../helpers/logger.h"
#include "./dqn_agent.h"
#include "../gym/lethal_gym.h"
#include <torch/torch.h>
#include <algorithm>
#include <Windows.h>
#include <thread>

namespace rl {
    DWORD WINAPI LearnStub(void *ptr) {
        if (!ptr) return -1;
        DeepQAgent* agent = ((DeepQAgent*) ptr);
        int interval = 5000;
        int i = 0;
        while (true) {
            i++;
            if (i == interval) {
                if (agent->ready_to_learn) {
                    ((DeepQAgent*)ptr)->Learn();
                }
                i = 0;
            }
        }
        return 0;
    }

    void MainDqn::Train() {
        gym::LethalGym* lethal_gym = new gym::LethalGym();
        DeepQAgent* dqn_agent = new DeepQAgent(0.99f, 1.0f, 0.003f, 148, 64, 7);
        std::vector<float> scores;
        int n_games = 500;
        std::vector<float> eps_history;

        helpers::Logger *logger = new helpers::Logger("./lethalai_log.txt");
        logger->WriteLine("Started Training");

        bool init = false;
        for (int i = 0; i < n_games; i++) {
            float score = 0.0f;
            bool done = false;
            std::cout << "Resetting..." << std::endl;
            logger->WriteLine("Resetting...");
            std::vector<float> observation = lethal_gym->Reset()->NormalizeFloats(0);
            std::cout << "Done Resetting" << std::endl;
            logger->WriteLine("Done Resetting");

            while (!done) {
                std::vector<environment::InputAction> p1action;
                std::vector<environment::InputAction> p2action;

                p1action.push_back(static_cast<environment::InputAction>(
                            dqn_agent->ChooseAction(observation)));
                std::vector<std::vector<environment::InputAction>> actions{ p1action, p2action };
                auto result = lethal_gym->Step(actions);

                //std::cout << "Normalizing Floats" << std::endl;
                std::vector<float> new_observation = std::get<0>(result)->NormalizeFloats(0);
                //std::cout << "Done Normalizing Floats" << std::endl;
                float reward = std::get<1>(result);
                done = std::get<2>(result);

                score += reward;

                dqn_agent->StoreTransition(observation, (int) p1action[0], reward,
                        new_observation);
                if (!init) {
                    CreateThread(NULL, 0, LearnStub, dqn_agent, 0, NULL);
                    init = true;
                }

                observation = new_observation;
            }

            scores.push_back(score);
            eps_history.push_back(dqn_agent->epsilon);

            float average_score = 0.0f;
            if (scores.size() <= 100)
                average_score = 1.0 * std::accumulate(
                        scores.begin(), scores.end(), 0.0) / scores.size();
            else
                average_score = 1.0 * std::accumulate(
                        scores.end() - 100, scores.end(), 0.0) / scores.size();

            std::ostringstream stream;
            stream << "episode " << i << " score " << score << " average score " <<
                average_score << " epsilon " << dqn_agent->epsilon;
            logger->WriteLine(stream.str());
            printf("episode %i score %.2f average score %.2f epsilon %.2f\n",
                    i, score, average_score, dqn_agent->epsilon);
        }
    }
}
