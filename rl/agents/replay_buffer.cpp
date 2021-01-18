#include "replay_buffer.h"

#include <random>
#include <unordered_set>

namespace rl {
	ReplayBuffer::ReplayBuffer(int max_size, int input_dims) {
		this->mem_size = max_size;
		this->mem_cntr = 0;

		this->state_memory = std::vector<std::vector<float>>(max_size,
				std::vector<float>(input_dims));
		this->new_state_memory = std::vector<std::vector<float>>(max_size,
				std::vector<float>(input_dims, 0.0f));
		this->action_memory = std::vector<int>(max_size, 0);
		this->reward_memory = std::vector<float>(max_size, 0.0f);
		this->terminal_memory = std::vector<bool>(max_size, false);
	}

	void ReplayBuffer::StoreTransition(std::vector<float> state, int action,
			float reward, std::vector<float> new_state, bool done) {
		int index = this->mem_cntr % this->mem_size;

        this->state_memory[index] = state;
        this->action_memory[index] = action;
        this->reward_memory[index] = reward;
        this->new_state_memory[index] = new_state;
        this->terminal_memory[index] = done;

        this->mem_cntr += 1;
	}

	std::tuple<std::vector<float>, std::vector<int>, std::vector<float>,
		std::vector<float>, std::vector<bool>> ReplayBuffer::SampleBuffer(int batch_size) {
		std::vector<float> states;
		std::vector<int> actions;
		std::vector<float> rewards;
		std::vector<float> new_states;
		std::vector<bool> dones;

		auto gen = std::mt19937{std::random_device{}()};
		std::unordered_set<int> elems;
		for (int r = this->mem_size - batch_size; r < this->mem_size; ++r) {
			int v = std::uniform_int_distribution<>(1, r)(gen);
			if (!elems.insert(v).second)
				elems.insert(r);
		}

		for (int i : elems) {
			std::vector<float> state_sample = this->state_memory[i];
			int action_sample = this->action_memory[i];
			float reward_sample = this->reward_memory[i];
			std::vector<float> new_state_sample = this->new_state_memory[i];
			bool done_sample = this->terminal_memory[i];

			states.insert(states.end(),
				state_sample.begin(), state_sample.end());
			actions.push_back(action_sample);
			rewards.push_back(reward_sample);
			new_states.insert(new_states.end(),
				new_state_sample.begin(), new_state_sample.end());
			dones.push_back(done_sample);
		}

		return std::make_tuple(states, actions, rewards, new_states, dones);
	}

    int ReplayBuffer::GetMemCounter() {
        return this->mem_cntr;
    }
}
