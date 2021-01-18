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

    std::tuple<torch::Tensor, torch::Tensor> DuelingDeepQNetworkImpl::forward(
            torch::Tensor state) {
        torch::Tensor x = torch::relu(this->fc1->forward(state));
        x = torch::relu(this->fc2->forward(x));
        torch::Tensor V = this->V->forward(x);
        torch::Tensor A = this->A->forward(x);

        return std::make_tuple(V, A);
    }


    DuelingDeepQAgent::DuelingDeepQAgent(float gamma, float epsilon, float lr, int input_dims,
            int batch_size, int n_actions, int max_mem_size,
            float eps_end, float eps_dec, int replace) {
        this->gamma = gamma;
        this->epsilon = epsilon;
        this->eps_min = eps_end;
        this->eps_dec = eps_dec;
        this->lr = lr;
        for (int i = 0; i < n_actions; i++) {
            this->action_space.push_back(i);
        }
        this->learn_step_counter = 0;
        this->replace_target_cnt = replace;
        this->mem_size = max_mem_size;
        this->batch_size = batch_size;
        this->mem_cntr = 0;

        this->q_eval = new DuelingDeepQNetwork(this->lr, input_dims, 128, 128, n_actions);
        this->q_next = new DuelingDeepQNetwork(this->lr, input_dims, 128, 128, n_actions);

        this->state_memory = std::vector<std::vector<float>>(max_mem_size,
                std::vector<float>(n_actions));
        this->new_state_memory = std::vector<std::vector<float>>(max_mem_size,
                std::vector<float>(n_actions, 0.0f));
        this->action_memory = std::vector<int>(max_mem_size, 0);
        this->reward_memory = std::vector<float>(max_mem_size, 0.0f);
        this->terminal_memory = std::vector<bool>(max_mem_size, false);
    }

    void DuelingDeepQAgent::StoreTransition(std::vector<float> state, int action,
            float reward, std::vector<float> new_state, bool done) {
        std::lock_guard<std::mutex> lk(this->mutex2);
        int index = this->mem_cntr % this->mem_size;
        this->state_memory[index] = state;
        this->new_state_memory[index] = new_state;
        this->reward_memory[index] = reward;
        this->action_memory[index] = action;
        this->terminal_memory[index] = done;

        this->mem_cntr += 1;
        this->ready_to_learn = true;
    }

    int DuelingDeepQAgent::ChooseAction(std::vector<float> observation) {
        if (//!this->using_mutex &&
                (float) rand() / (RAND_MAX) > this->epsilon) {
            torch::Tensor state = torch::tensor(observation).to(device);
            {
                std::lock_guard<std::mutex> lock(this->mutex);
                auto res = (*this->q_eval)->forward(state);
                torch::Tensor actions = std::get<1>(res);
                return torch::argmax(actions).item<int>();
            }
        } else {
            return this->action_space[rand() % this->action_space.size()];
        }
    }

    void DuelingDeepQAgent::ReplaceTargetNetwork() {
        if (this->learn_step_counter % this->replace_target_cnt == 0) {
            std::lock_guard<std::mutex> lk(this->mutex);
            torch::save((*this->q_eval), "tmp.pt");
            torch::load((*this->q_next), "tmp.pt");
        }
    }

    void DuelingDeepQAgent::Learn() {
        int cntr;
        {
            std::lock_guard<std::mutex> lk(this->mutex2);
            cntr = this->mem_cntr;
        }
        if (cntr < this->batch_size)
            return;
        (*this->q_eval)->optimizer.zero_grad();

        this->ReplaceTargetNetwork();

        int max_mem = std::min(cntr, this->mem_size);
        std::vector<int> batch(max_mem);

        std::iota(batch.begin(), batch.end(), 0);
        std::random_shuffle(batch.begin(), batch.end());
        batch.resize(this->batch_size);

        std::vector<float> _state_batch;
        int size;
        {
            std::lock_guard<std::mutex> lk(this->mutex2);
            for (int i = 0; i < batch_size; i++) {
                std::vector<float> mem = this->state_memory[batch[i]];
                if (i == 0)
                    size = mem.size();
                _state_batch.insert(_state_batch.end(), mem.begin(), mem.end());
            }
        }
        torch::Tensor state_batch = torch::from_blob(_state_batch.data(),
                { batch_size, size }, torch::kFloat).to(device);
        std::vector<float> _new_state_batch;
        {
            std::lock_guard<std::mutex> lk(this->mutex2);
            for (int i = 0; i < batch_size; i++) {
                std::vector<float> mem = this->new_state_memory[batch[i]];
                if (i == 0)
                    size = mem.size();
                _new_state_batch.insert(_new_state_batch.end(), mem.begin(), mem.end());
            }
        }
        torch::Tensor new_state_batch = torch::from_blob(_new_state_batch.data(),
                { batch_size, size }, torch::kFloat).to(device);

        std::vector<float> _reward_batch;
        {
            std::lock_guard<std::mutex> lk(this->mutex2);
            for (int i = 0; i < batch_size; i++) {
                _reward_batch.push_back(this->reward_memory[batch[i]]);
            }
        }
        torch::Tensor reward_batch = torch::from_blob(_reward_batch.data(),
                { batch_size }, torch::kFloat).to(device);

        std::vector<int> _terminal_batch;
        {
            std::lock_guard<std::mutex> lk(this->mutex2);
            for (int i = 0; i < batch_size; i++) {
                _terminal_batch.push_back(this->terminal_memory[batch[i]]);
            }
        }
        torch::Tensor terminal_batch = torch::from_blob(_terminal_batch.data(),
                { batch_size }, torch::kBool).to(device);

        std::vector<int64_t> _action_batch;
        {
            std::lock_guard<std::mutex> lk(this->mutex2);
            for (int i = 0; i < batch_size; i++) {
                _action_batch.push_back(this->action_memory[batch[i]]);
            }
        }
        torch::Tensor action_batch = torch::from_blob(_action_batch.data(),
                { batch_size }, torch::kInt64).to(device);

        {
            std::lock_guard<std::mutex> lock(this->mutex);
            this->using_mutex = true;
            auto res1 = (*this->q_eval)->forward(state_batch);
            auto res2 = (*this->q_next)->forward(new_state_batch);

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
            (*this->q_eval)->optimizer.step();
            this->learn_step_counter += 1;
            this->using_mutex = false;
    }

    if (this->epsilon > this->eps_min) {
        this->epsilon = this->epsilon - this->eps_dec;
    } else {
        this->epsilon = this->eps_min;
    }

    {
        std::lock_guard<std::mutex> lk(this->mutex2);
        this->ready_to_learn = false;
    }
    }
}
