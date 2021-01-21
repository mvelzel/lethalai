#pragma once
#include "../environment/input_handler.h"

namespace rl {
    class ActionTranslator {
        public:
            static const int total_action_count = 64;
            static const int individual_action_count = 8;
            static std::vector<environment::InputAction> TranslateAction(
                    int action);
    };
}
