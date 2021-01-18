#ifndef DUELING_DQN
#define DUELING_DQN

#include <torch/torch.h>
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
            std::tuple<torch::Tensor, torch::Tensor> forward(torch::Tensor state);
    };
    TORCH_MODULE(DuelingDeepQNetwork);

    class DuelingDeepQAgent {
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
            std::vector<bool> terminal_memory;
            int replace_target_cnt;
            int learn_step_counter;
        public:
            DuelingDeepQNetwork* q_eval;
            DuelingDeepQNetwork* q_next;

            float epsilon;
            std::mutex mutex;
            std::mutex mutex2;
            bool using_mutex = false;
            bool ready_to_learn = false;

            DuelingDeepQAgent(float gamma, float epsilon, float lr, int input_dims,
                    int batch_size, int n_actions, int max_mem_size=100000,
                    float eps_end=0.01f, float eps_dec=3e-5, int replace=1000);
            void StoreTransition(std::vector<float> state, int action,
                    float reward, std::vector<float> new_state, bool done);
            void ReplaceTargetNetwork();
            int ChooseAction(std::vector<float> observation);
            void Learn();
    };
}

#endif
