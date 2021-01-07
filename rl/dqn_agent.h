#ifndef DQN_AGENT_H
#define DQN_AGENT_H

#include <torch/torch.h>
#include <mutex>

namespace rl {
    class DeepQNetworkImpl : torch::nn::Module {
        private:
            float lr;
            int input_dims;
            int fc1_dims;
            int fc2_dims;
            int n_actions;
        public:
            torch::nn::Linear fc1;
            torch::nn::Linear fc2;
            torch::nn::Linear fc3;
            torch::optim::Adam optimizer;

            DeepQNetworkImpl(float lr, int input_dims, int fc1_dims,
                    int fc2_dims, int n_actions);
            torch::Tensor forward(torch::Tensor state);
    };
    TORCH_MODULE(DeepQNetwork);

    class DeepQAgent {
        private:
            float gamma;
            float eps_min;
            float eps_dec;
            float lr;
            std::vector<int> action_space;
            int mem_size;
            int batch_size;
            int mem_cntr;
            std::vector<std::vector<float>> state_memory;
            std::vector<std::vector<float>> new_state_memory;
            std::vector<int> action_memory;
            std::vector<float> reward_memory;
        public:
            DeepQNetwork* q_eval;

            float epsilon;
            std::mutex mutex;
            std::mutex mutex2;
            bool using_mutex = false;
            bool ready_to_learn = false;

            DeepQAgent(float gamma, float epsilon, float lr, int input_dims,
                    int batch_size, int n_actions, int max_mem_size=10000,
                    float eps_end=0.01f, float eps_dec=1e-4);
            void StoreTransition(std::vector<float> state, int action,
                    float reward, std::vector<float> new_state);
            int ChooseAction(std::vector<float> observation);
            void Learn();
    };
}

#endif
