#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "game_model/player.h"
#include <vector>

namespace environment {
    enum InputAction {
        kSwing,
        kBunt,
        kUp,
        kDown,
        kLeft,
        kRight,
        kJump,
        kTaunt
    };

    class InputHandler {
        private:
            bool override_p1 = false;
            bool override_p2 = false;
            bool override_p3 = false;
            bool override_p4 = false;
        public:
            InputHandler(bool enabled);

            static int ActionToKeyPress(InputAction action);
            static int ActionToEvent(int player_number, InputAction action);

            void Inject();
            void DeInject();
            void EnableOverride(int player_number);
            void DisableOverride(int player_number);

            void InputActions(int player_number, std::vector<InputAction> actions);
    };
}

#endif
