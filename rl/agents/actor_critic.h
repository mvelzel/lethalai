#ifndef ACTOR_CRITIC_H
#define ACTOR_CRITIC_H

#include "../agent.h"
#include <torch/torch.h>

namespace rl {
    class ActorCriticNetworkImpl : public torch::nn::Module {
        private:
            int input_dims;
            int fc1_dims;
            int fc2_dims;
            int n_actions;
            float lr;
        public:
            torch::nn::Linear fc1;
            torch::nn::Linear fc2;
            torch::nn::Linear fc3;
            torch::optim::Adam optimizer;

            ActorCriticNetworkImpl(float lr, int input_dims, int fc1_dims,
                    int fc2_dims, int n_actions);
            torch::Tensor Forward(
                    std::vector<float> observation);
    };
    TORCH_MODULE(ActorCriticNetwork);

    class ActorCriticAgent : public Agent {
        private:
            float gamma;
            int n_actions;
            torch::Tensor log_probs;

            std::queue<std::tuple<
                std::vector<float>, std::vector<float>,
                float, bool, torch::Tensor>> experience_queue;

            std::mutex network_mutex;
            std::mutex queue_mutex;
        public:
            ActorCriticNetwork actor;
            ActorCriticNetwork critic;

            ActorCriticAgent(float gamma, float lr,
                    int input_dims, int batch_size, int n_actions);


            void StoreTransition(std::vector<float> state, int action,
                    float reward, std::vector<float> new_state, bool done) override;
            int ChooseAction(std::vector<float> observation) override;
            void Learn() override;
            void StoreCheckpoint(std::string filename) override;
            void LoadCheckpoint(std::string filename) override;
    };
}

#endif
