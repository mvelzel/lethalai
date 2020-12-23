#ifndef STATE_OBSERVER_H
#define STATE_OBSERVER_H

namespace environment {
    class StateObserver {
        public:
            StateObserver(bool enabled);
            void Enable();
            void Disable();


    };
}

#endif
