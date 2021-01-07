#ifndef STATE_OBSERVER_H
#define STATE_OBSERVER_H

#include "game_state.h"

namespace environment {
    class StateObserver {
        private:
            GameState *game_state = NULL;
            void (*observer)(GameState*) = NULL;
        public:
            StateObserver(bool enabled);
            void Enable();
            void Disable();
            void UpdateState();
            void SetObserver(void (*observer)(GameState*));
            GameState* GetGameState();
            
            void SpawnBall();
            void SpawnBall(DWORD x, DWORD y);
            void SpawnPlayer(int player_number);
            void SpawnPlayers();
            void HealPlayer(int player_number, int amount);
            void HealPlayers(int amount);
    };
}

#endif
