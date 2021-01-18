#ifndef AGENT_H
#define AGENT_H

#include <vector>
#include <string>

namespace rl {
    class Agent {
        public:
            virtual void StoreTransition(std::vector<float> state, int action,
                    float reward, std::vector<float> new_state, bool done) = 0;
            virtual int ChooseAction(std::vector<float> observation) = 0;
            virtual void Learn() = 0;
            virtual void StoreCheckpoint(std::string filename) = 0;
            virtual void LoadCheckpoint(std::string filename) = 0;
    };
}

#endif
