#include "dueling_dqn.h"
#include "ATen/Functions.h"
#include "c10/core/ScalarType.h"
#include "torch/csrc/autograd/generated/variable_factories.h"
#include <random>
#include <algorithm>

namespace rl {
    torch::Device device(torch::kCPU);

    DuelingDeepQNetworkImpl::DuelingDeepQNetworkImpl(float lr, int input_dims, int fc1_dims,
            int fc2_dims, int n_actions):
        fc1(register_module("fc1", torch::nn::Linear(input_dims, fc1_dims))),
        fc2(register_module("fc2", torch::nn::Linear(fc1_dims, fc2_dims))),
        V(register_module("V", torch::nn::Linear(fc2_dims, 1))),
        A(register_module("A", torch::nn::Linear(fc2_dims, n_actions))),
        optimizer(torch::optim::Adam(this->parameters(),
                    torch::optim::AdamOptions(lr))) {
            this->lr = lr;
            this->input_dims = input_dims;
            this->fc1_dims = fc1_dims;
            this->fc2_dims = fc2_dims;
            this->n_actions = n_actions;

            this->to(device);
        }

    std::tuple<torch::Tensor, torch::Tensor> DuelingDeepQNetworkImpl::Forward(
            torch::Tensor state) {
        torch::Tensor x = torch::relu(this->fc1->forward(state));
        x = torch::relu(this->fc2->forward(x));
        torch::Tensor V = this->V->forward(x);
        torch::Tensor A = this->A->forward(x);

        return std::make_tuple(V, A);
    }


    DuelingDeepQAgent::DuelingDeepQAgent(float gamma, float epsilon, float lr, int input_dims,
            int batch_size, int n_actions, int max_mem_size,
            float eps_end, float eps_dec, int replace) :
        replay_buffer(ReplayBuffer(max_mem_size, input_dims)),
        q_eval(DuelingDeepQNetwork(lr, input_dims, 128, 128, n_actions)),
        q_next(DuelingDeepQNetwork(lr, input_dims, 128, 128, n_actions)) {
            this->input_dims = input_dims;

            this->gamma = gamma;
            this->epsilon = epsilon;
            this->eps_min = eps_end;
            this->eps_dec = eps_dec;
            this->lr = lr;

            for (int i = 0; i < n_actions; i++) {
                this->action_space.push_back(i);
            }

            this->replace_target_cnt = replace;
            this->learn_step_counter = 0;

            this->batch_size = batch_size;
        }

    void DuelingDeepQAgent::StoreTransition(std::vector<float> state, int action,
            float reward, std::vector<float> new_state, bool done) {
        std::lock_guard<std::mutex> lk(this->replay_mutex);
        this->replay_buffer.StoreTransition(state, action, reward, new_state, done);
    }

    int DuelingDeepQAgent::ChooseAction(std::vector<float> observation) {
        if ((float) rand() / RAND_MAX > this->epsilon) {
            torch::Tensor state = torch::tensor(observation).to(device);
            {
                std::lock_guard<std::mutex> lock(this->network_mutex);

                auto res = this->q_eval->Forward(state);
                torch::Tensor actions = std::get<1>(res);
                return torch::argmax(actions).item<int>();
            }
        } else {
            return this->action_space[rand() % this->action_space.size()];
        }
    }

    void DuelingDeepQAgent::ReplaceTargetNetwork() {
        if (this->learn_step_counter == this->replace_target_cnt) {
            std::lock_guard<std::mutex> lk(this->network_mutex);
            torch::save(this->q_eval, "tmp.pt");
            torch::load(this->q_next, "tmp.pt");
            this->learn_step_counter = 0;
        }
    }

    void DuelingDeepQAgent::Learn() {
        int cntr;
        {
            std::lock_guard<std::mutex> lk(this->replay_mutex);
            cntr = this->replay_buffer.GetMemCounter();
        }
        if (cntr < this->batch_size)
            return;

        this->q_eval->optimizer.zero_grad();

        this->ReplaceTargetNetwork();

        std::tuple<std::vector<float>, std::vector<int>, std::vector<float>,
            std::vector<float>, std::vector<bool>> sample;

        {
            std::lock_guard<std::mutex> lk(this->replay_mutex);
            sample = this->replay_buffer.SampleBuffer(this->batch_size);
        }

        std::vector<int64_t> _action_batch(
                std::get<1>(sample).begin(), std::get<1>(sample).end());
        std::vector<int> _terminal_batch(
                std::get<4>(sample).begin(), std::get<4>(sample).end());

        torch::Tensor state_batch = torch::from_blob(std::get<0>(sample).data(),
                { batch_size, this->input_dims }, torch::kFloat).to(device);
        torch::Tensor new_state_batch = torch::from_blob(std::get<3>(sample).data(),
                { batch_size, this->input_dims }, torch::kFloat).to(device);
        torch::Tensor reward_batch = torch::from_blob(std::get<2>(sample).data(),
                { batch_size }, torch::kFloat).to(device);
        torch::Tensor terminal_batch = torch::from_blob(_terminal_batch.data(),
                { batch_size }, torch::kBool).to(device);
        torch::Tensor action_batch = torch::from_blob(_action_batch.data(),
                { batch_size }, torch::kInt64).to(device);

        {
            std::lock_guard<std::mutex> lock(this->network_mutex);

            auto res1 = this->q_eval->Forward(state_batch);
            auto res2 = this->q_next->Forward(new_state_batch);

            torch::Tensor V_s = std::get<0>(res1);
            torch::Tensor A_s = std::get<1>(res1);
            torch::Tensor V_s_ = std::get<0>(res2);
            torch::Tensor A_s_ = std::get<1>(res2);

            torch::Tensor q_pred = torch::add(V_s, torch::sub(A_s, A_s.mean(1, true)))
                .gather(1,  action_batch.unsqueeze(-1));
            torch::Tensor q_next = torch::add(V_s_, torch::sub(A_s_, A_s_.mean(1, true)));

            torch::Tensor q_target = reward_batch +
                this->gamma * std::get<0>(torch::max(q_next, 1)).detach();
            q_target.index_put_({ terminal_batch }, 0.0f);

            torch::Tensor loss = torch::mse_loss(q_pred, q_target).to(device);
            loss.backward();
            this->q_eval->optimizer.step();
            this->learn_step_counter += 1;
        }

        if (this->epsilon > this->eps_min) {
            this->epsilon = this->epsilon - this->eps_dec;
        } else {
            this->epsilon = this->eps_min;
        }
    }

    void DuelingDeepQAgent::StoreCheckpoint(std::string filename) {
        std::lock_guard<std::mutex> lk(this->network_mutex);
        torch::save(this->q_eval, filename + "_q_eval.pt");
        torch::save(this->q_next, filename + "_q_next.pt");
    }

    void DuelingDeepQAgent::LoadCheckpoint(std::string filename) {
        std::lock_guard<std::mutex> lk(this->network_mutex);
        torch::load(this->q_eval, filename + "_q_eval.pt");
        torch::load(this->q_next, filename + "_q_next.pt");
    }
}
