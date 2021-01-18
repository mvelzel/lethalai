#pragma once

#include <vector>

namespace rl {
    class ReplayBuffer {
        private:
            int mem_size;
            int mem_cntr;

            std::vector<std::vector<float>> state_memory;
            std::vector<std::vector<float>> new_state_memory;
            std::vector<int> action_memory;
            std::vector<float> reward_memory;
            std::vector<bool> terminal_memory;
        public:
            ReplayBuffer(int max_size, int input_dims);

            void StoreTransition(std::vector<float> state, int action,
                    float reward, std::vector<float> new_state, bool done);
            std::tuple<std::vector<float>, std::vector<int>, std::vector<float>,
                std::vector<float>, std::vector<bool>> SampleBuffer(int batch_size);
            int GetMemCounter();
    };
}
