#include "actor_critic.h"
#include "../../helpers/distributions/categorical.h"
#include <mutex>

namespace rl {
    torch::Device device(torch::kCPU);

    ActorCriticNetworkImpl::ActorCriticNetworkImpl(float lr, int input_dims,
            int fc1_dims, int fc2_dims, int n_actions) :
        fc1(register_module("fc1", torch::nn::Linear(input_dims, fc1_dims))),
        fc2(register_module("fc2", torch::nn::Linear(fc1_dims, fc2_dims))),
        fc3(register_module("fc3", torch::nn::Linear(fc2_dims, n_actions))),
        optimizer(torch::optim::Adam(this->parameters(),
                    torch::optim::AdamOptions(lr))) {
        this->input_dims = input_dims;
        this->fc1_dims = fc1_dims;
        this->fc2_dims = fc2_dims;
        this->n_actions = n_actions;
        this->lr = lr;

        this->to(device);
    }

    torch::Tensor ActorCriticNetworkImpl::Forward(
            std::vector<float> observation) {
        torch::Tensor state = torch::tensor(observation).to(device);
        torch::Tensor x1 = torch::relu(this->fc1(state));
        torch::Tensor x2 = torch::relu(this->fc2(x1));
        torch::Tensor res = this->fc3(x2);

        return res;
    }

    ActorCriticAgent::ActorCriticAgent(float gamma, float lr,
            int input_dims, int batch_size, int n_actions) :
        actor(ActorCriticNetwork(lr, input_dims, 128, 128, n_actions)),
        critic(ActorCriticNetwork(lr, input_dims, 128, 128, 1)) {
        this->gamma = gamma;
        this->n_actions = n_actions;
    }

    int ActorCriticAgent::ChooseAction(std::vector<float> observation) {
        std::lock_guard<std::mutex> lk(this->network_mutex);

        torch::Tensor probs = torch::softmax(
                this->actor->Forward(observation), 0);

        cpprl::Categorical action_probs = cpprl::Categorical(&probs, nullptr);
        torch::Tensor action = action_probs.sample();

        this->log_probs = action_probs.log_prob(action);

        return action.item<int>();
    }

    void ActorCriticAgent::StoreTransition(std::vector<float> state, int action,
            float reward, std::vector<float> new_state, bool done) {
        std::lock_guard<std::mutex> lk(this->queue_mutex);

        this->experience_queue.push(std::make_tuple(
                    state, new_state, reward, done, this->log_probs));
    }

    void ActorCriticAgent::Learn() {
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

    void ActorCriticAgent::StoreCheckpoint(std::string filename) {
        std::lock_guard<std::mutex> lk(this->network_mutex);
        torch::save(this->actor, filename + "_actor.pt");
        torch::save(this->critic, filename + "_critic.pt");
    }

    void ActorCriticAgent::LoadCheckpoint(std::string filename) {
        std::lock_guard<std::mutex> lk(this->network_mutex);
        torch::load(this->actor, filename + "_actor.pt");
        torch::load(this->critic, filename + "_critic.pt");
    }
}
