#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "game_model/player.h"

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
            bool override_p1;
            bool override_p2;
            bool override_p3;
            bool override_p4;
        public:
            InputHandler(bool enabled);

            static int ActionToKeyPress(InputAction action);
            static int ActionToEvent(int player_number, InputAction action);

            void Inject();
            void DeInject();
            void EnableOverride(int player_number);
            void DisableOverride(int player_number);

            void InputActions(int player_number, InputAction *actions, int action_count);
    };
}

#endif
