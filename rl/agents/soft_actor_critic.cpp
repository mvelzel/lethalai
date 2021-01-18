#include "soft_actor_critic.h"
#include "../../helpers/distributions/categorical.h"

namespace rl {
    //TODO finish implementing
    torch::Device device(torch::kCPU);

    CriticNetworkImpl::CriticNetworkImpl(float lr, int input_dims,
            int fc1_dims, int fc2_dims, int n_actions) :
        fc1(register_module("fc1", torch::nn::Linear(input_dims + n_actions, fc1_dims))),
        fc2(register_module("fc2", torch::nn::Linear(fc1_dims, fc2_dims))),
        q(register_module("q", torch::nn::Linear(fc2_dims, 1))),
        optimizer(torch::optim::Adam(this->parameters(),
                    torch::optim::AdamOptions(lr))) {
        this->input_dims = input_dims;
        this->fc1_dims = fc1_dims;
        this->fc2_dims = fc2_dims;
        this->n_actions = n_actions;
        this->lr = lr;

        this->to(device);
    }

    torch::Tensor CriticNetworkImpl::Forward(
            torch::Tensor state, torch::Tensor action_one_hot) {
        torch::Tensor state_action[2] = { state, action_one_hot };
        torch::Tensor action_value = torch::relu(
                this->fc1(torch::cat(state_action, 1)));
        action_value = torch::relu(this->fc2(action_value));
        torch::Tensor q = this->q(action_value);

        return q;
    }


    ValueNetworkImpl::ValueNetworkImpl(float lr, int input_dims,
            int fc1_dims, int fc2_dims) :
        fc1(register_module("fc1", torch::nn::Linear(input_dims, fc1_dims))),
        fc2(register_module("fc2", torch::nn::Linear(fc1_dims, fc2_dims))),
        v(register_module("v", torch::nn::Linear(fc2_dims, 1))),
        optimizer(torch::optim::Adam(this->parameters(),
                    torch::optim::AdamOptions(lr))) {
        this->input_dims = input_dims;
        this->fc1_dims = fc1_dims;
        this->fc2_dims = fc2_dims;
        this->lr = lr;

        this->to(device);
    }

    torch::Tensor ValueNetworkImpl::Forward(torch::Tensor state) {
        torch::Tensor state_value = torch::relu(this->fc1(state));
        state_value = torch::relu(this->fc2(state_value));
        torch::Tensor v = this->v(state_value);

        return v;
    }

    SoftActorCriticAgent::SoftActorCriticAgent(float gamma, float lr,
            int input_dims, int batch_size, int n_actions) :
        actor(ActorCriticNetwork(lr, input_dims, 128, 128, n_actions)),
        critic(ActorCriticNetwork(lr, input_dims, 128, 128, 1)) {
        this->gamma = gamma;
        this->n_actions = n_actions;
    }

    int SoftActorCriticAgent::ChooseAction(std::vector<float> observation) {
        std::lock_guard<std::mutex> lk(this->network_mutex);

        torch::Tensor probs = torch::softmax(
                this->actor->Forward(observation), 0);

        cpprl::Categorical action_probs = cpprl::Categorical(&probs, nullptr);
        torch::Tensor action = action_probs.sample();

        this->log_probs = action_probs.log_prob(action);

        return action.item<int>();
    }

    void SoftActorCriticAgent::StoreTransition(std::vector<float> state, int action,
            float reward, std::vector<float> new_state, bool done) {
        std::lock_guard<std::mutex> lk(this->queue_mutex);

        this->experience_queue.push(std::make_tuple(
                    state, new_state, reward, done, this->log_probs));
    }

    void SoftActorCriticAgent::Learn() {
        while (true) {
            std::tuple<
                std::vector<float>, std::vector<float>,
                float, bool, torch::Tensor> experience;
            {
                std::lock_guard<std::mutex> lk(this->queue_mutex);

                if (this->experience_queue.empty())
                    break;
                
                experience = this->experience_queue.front();
                this->experience_queue.pop();
            }

            std::vector<float> observation = std::get<0>(experience);
            std::vector<float> new_observation = std::get<1>(experience);
            float reward = std::get<2>(experience);
            bool done = std::get<3>(experience);
            torch::Tensor log_probs = std::get<4>(experience);

            try
            {
                std::lock_guard<std::mutex> lk(this->network_mutex);

                this->actor->optimizer.zero_grad();
                this->critic->optimizer.zero_grad();

                torch::Tensor critic_value = this->critic->Forward(observation);
                torch::Tensor critic_value_new = this->critic->Forward(new_observation);

                torch::Tensor delta = ((reward + this->gamma * critic_value_new *
                            (1 - static_cast<int>(done))) - critic_value);

                torch::Tensor actor_loss = -log_probs * delta;
                torch::Tensor critic_loss = torch::pow(delta, 2);

                (actor_loss + critic_loss).backward();

                this->actor->optimizer.step();
                this->critic->optimizer.step();
            }
            catch (const std::exception& e) {
                std::cout << e.what() << std::endl;
            }
        }
    }
}
