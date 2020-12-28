#ifndef STATE_OBSERVER_H
#define STATE_OBSERVER_H

#include "game_state.h"

namespace environment {
    class StateObserver {
        private:
            GameState *game_state;
            void (*observer)(GameState*);
        public:
            StateObserver(bool enabled);
            void Enable();
            void Disable();
            void UpdateState();
            void SetObserver(void (*observer)(GameState*));
    };
}

#endif
