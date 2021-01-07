#include "dqn_agent.h"
#include "ATen/Functions.h"
#include "c10/core/ScalarType.h"
#include "torch/csrc/autograd/generated/variable_factories.h"
#include <random>
#include <algorithm>

namespace rl {
    torch::Device device(torch::kCPU);

    DeepQNetworkImpl::DeepQNetworkImpl(float lr, int input_dims, int fc1_dims,
            int fc2_dims, int n_actions):
        fc1(register_module("fc1", torch::nn::Linear(input_dims, fc1_dims))),
        fc2(register_module("fc2", torch::nn::Linear(fc1_dims, fc2_dims))),
        fc3(register_module("fc3", torch::nn::Linear(fc2_dims, n_actions))),
        optimizer(torch::optim::Adam(this->parameters(),
                   torch::optim::AdamOptions(lr))) {
            this->lr = lr;
            this->input_dims = input_dims;
            this->fc1_dims = fc1_dims;
            this->fc2_dims = fc2_dims;
            this->n_actions = n_actions;

            this->to(device);
        }

    torch::Tensor DeepQNetworkImpl::forward(torch::Tensor state) {
        torch::Tensor x = torch::relu(this->fc1->forward(state));
        x = torch::relu(this->fc2->forward(x));
        torch::Tensor actions = this->fc3->forward(x);

        return actions;
    }

    
    DeepQAgent::DeepQAgent(float gamma, float epsilon, float lr, int input_dims,
            int batch_size, int n_actions, int max_mem_size,
            float eps_end, float eps_dec) {
        this->gamma = gamma;
        this->epsilon = epsilon;
        this->eps_min = eps_end;
        this->eps_dec = eps_dec;
        this->lr = lr;
        for (int i = 0; i < n_actions; i++) {
            this->action_space.push_back(i);
        }
        this->mem_size = max_mem_size;
        this->batch_size = batch_size;
        this->mem_cntr = 0;

        this->q_eval = new DeepQNetwork(this->lr, input_dims, 256, 256, n_actions);

        this->state_memory = std::vector<std::vector<float>>(max_mem_size,
                std::vector<float>(n_actions));
        this->new_state_memory = std::vector<std::vector<float>>(max_mem_size,
                std::vector<float>(n_actions, 0.0f));
        this->action_memory = std::vector<int>(max_mem_size, 0);
        this->reward_memory = std::vector<float>(max_mem_size, 0.0f);
    }

    void DeepQAgent::StoreTransition(std::vector<float> state, int action,
            float reward, std::vector<float> new_state) {
        std::lock_guard<std::mutex> lk(this->mutex2);
        int index = this->mem_cntr % this->mem_size;
        this->state_memory[index] = state;
        this->new_state_memory[index] = new_state;
        this->reward_memory[index] = reward;
        this->action_memory[index] = action;

        this->mem_cntr += 1;
        this->ready_to_learn = true;
    }

    int DeepQAgent::ChooseAction(std::vector<float> observation) {
        if (//!this->using_mutex &&
                (float) rand() / (RAND_MAX) > this->epsilon) {
            torch::Tensor state = torch::tensor(observation).to(device);
            {
                std::lock_guard<std::mutex> lock(this->mutex);
                torch::Tensor actions = (*this->q_eval)->forward(state).to(device);
                return torch::argmax(actions).item<int>();
            }
        } else {
            return this->action_space[rand() % this->action_space.size()];
        }
    }

    void DeepQAgent::Learn() {
        int cntr;
        {
            std::lock_guard<std::mutex> lk(this->mutex2);
            cntr = this->mem_cntr;
        }
        if (cntr < this->batch_size)
            return;
        (*this->q_eval)->optimizer.zero_grad();

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
                { batch_size, size }, at::kFloat).to(device);
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
                { batch_size, size }, at::kFloat).to(device);
        std::vector<float> _reward_batch;
        {
            std::lock_guard<std::mutex> lk(this->mutex2);
            for (int i = 0; i < batch_size; i++) {
                _reward_batch.push_back(this->reward_memory[batch[i]]);
            }
        }
        torch::Tensor reward_batch = torch::tensor(_reward_batch).to(device);
        std::vector<int> action_batch;
        {
            std::lock_guard<std::mutex> lk(this->mutex2);
            for (int i = 0; i < batch_size; i++) {
                action_batch.push_back(this->action_memory[batch[i]]);
            }
        }

        torch::Tensor next;
        torch::Tensor eval;
        {
            std::lock_guard<std::mutex> lock(this->mutex);
            this->using_mutex = true;
            eval = (*this->q_eval)->forward(state_batch).to(device);
            next = (*this->q_eval)->forward(new_state_batch).to(device);
        //    this->using_mutex = false;
        //}
        eval = eval.index({torch::indexing::Slice(0, batch_size),
                    torch::tensor(action_batch)});
                
        torch::Tensor target = reward_batch +
            this->gamma * std::get<0>(torch::max(next, 1));
        //{
        //    std::lock_guard<std::mutex> lock(this->mutex);
            this->using_mutex = true;
            torch::Tensor loss = torch::mse_loss(eval, target).to(device);
            loss.backward();
            (*this->q_eval)->optimizer.step();
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
