#include "action_translator.h"

namespace rl {
    std::vector<environment::InputAction> ActionTranslator::TranslateAction(int action) {
        int action1 = action / ActionTranslator::individual_action_count;
        int action2 = action % ActionTranslator::individual_action_count;
        std::vector<environment::InputAction> res;
        res.push_back(static_cast<environment::InputAction>(action1));
        res.push_back(static_cast<environment::InputAction>(action2));
        return res;
    }
}
