#pragma once

#include "../agent.h"
#include <torch/torch.h>

namespace rl {
    class CriticNetworkImpl : public torch::nn::Module {
        private:
            int input_dims;
            int fc1_dims;
            int fc2_dims;
            int n_actions;
            float lr;
        public:
            torch::nn::Linear fc1;
            torch::nn::Linear fc2;
            torch::nn::Linear q;
            torch::optim::Adam optimizer;

            CriticNetworkImpl(float lr, int input_dims, int fc1_dims,
                    int fc2_dims, int n_actions);
            torch::Tensor Forward(torch::Tensor state, torch::Tensor action);
    };
    TORCH_MODULE(CriticNetwork);

    class ValueNetworkImpl : public torch::nn::Module {
        private:
            int input_dims;
            int fc1_dims;
            int fc2_dims;
            float lr;
        public:
            torch::nn::Linear fc1;
            torch::nn::Linear fc2;
            torch::nn::Linear v;
            torch::optim::Adam optimizer;

            ValueNetworkImpl(float lr, int input_dims, int fc1_dims,
                    int fc2_dims);
            torch::Tensor Forward(torch::Tensor state);
    };
    TORCH_MODULE(ValueNetwork);

    class ActorNetworkImpl : public torch::nn::Module {
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

            ActorNetworkImpl(float lr, int input_dims, int fc1_dims,
                    int fc2_dims, int n_actions);
            torch::Tensor Forward(
                    std::vector<float> observation);
    };
    TORCH_MODULE(ActorNetwork);

    class SoftActorCriticAgent : public Agent {
        private:
            float gamma;
            int n_actions;

            std::mutex network_mutex;
            std::mutex queue_mutex;
        public:
            ValueNetwork value;
            CriticNetwork critic;
            ActorNetwork actor;

            SoftActorCriticAgent(float gamma, float lr,
                    int input_dims, int batch_size, int n_actions);


            void StoreTransition(std::vector<float> state, int action,
                    float reward, std::vector<float> new_state, bool done) override;
            int ChooseAction(std::vector<float> observation) override;
            void Learn() override;
    };
}
