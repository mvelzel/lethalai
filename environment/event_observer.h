#ifndef EVENT_OBSERVER_H
#define EVENT_OBSERVER_H

#include <unordered_map>
#include "events/event.h"

namespace environment {
    class EventObserver {
        private:
            std::unordered_map<int, events::Event*> events;
            bool has_observer = false;
        public:
            EventObserver(bool start_enabled);
            void EnableEvent(int identifier);
            void DisableEvent(int identifier);
            void SetObserver(void (*observer)(events::Event*, unsigned int));
    };
}

#endif
