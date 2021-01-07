#ifndef HIT_EVENT_H
#define HIT_EVENT_H

#include "event.h"

namespace events {
    class HitEvent : public Event {
        public:
            HitEvent(int id);
            void Inject() override;
            void Trigger() override;
    };
}

#endif
