#pragma once

#include <torch/torch.h>
#include "replay_buffer.h"
#include "../agent.h"
#include <mutex>

namespace rl {
    class DuelingDeepQNetworkImpl : public torch::nn::Module {
        private:
            float lr;
            int input_dims;
            int fc1_dims;
            int fc2_dims;
            int n_actions;
        public:
            torch::nn::Linear fc1;
            torch::nn::Linear fc2;
            torch::nn::Linear V;
            torch::nn::Linear A;
            torch::optim::Adam optimizer;

            DuelingDeepQNetworkImpl(float lr, int input_dims, int fc1_dims,
                    int fc2_dims, int n_actions);
            std::tuple<torch::Tensor, torch::Tensor> Forward(torch::Tensor state);
    };
    TORCH_MODULE(DuelingDeepQNetwork);

    class DuelingDeepQAgent : public Agent {
        private:
            int input_dims;

            float gamma;
            float eps_min;
            float eps_dec;
            float lr;
            std::vector<int> action_space;
            int batch_size;
            int replace_target_cnt;
            int learn_step_counter;

            ReplayBuffer replay_buffer;

            std::mutex network_mutex;
            std::mutex replay_mutex;

            void ReplaceTargetNetwork();
        public:
            float epsilon;

            DuelingDeepQNetwork q_eval;
            DuelingDeepQNetwork q_next;

            DuelingDeepQAgent(float gamma, float epsilon, float lr, int input_dims,
                    int batch_size, int n_actions, int max_mem_size=100000,
                    float eps_end=0.01f, float eps_dec=3e-5, int replace=1000);

            void StoreTransition(std::vector<float> state, int action,
                    float reward, std::vector<float> new_state, bool done) override;
            int ChooseAction(std::vector<float> observation) override;
            void Learn() override;
            void StoreCheckpoint(std::string filename) override;
            void LoadCheckpoint(std::string filename) override;
    };
}
