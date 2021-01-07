#ifndef DEATH_EVENT_H
#define DEATH_EVENT_H

#include "event.h"

namespace events {
    class DeathEvent : public Event {
        public:
            DeathEvent(int id);
            void Inject() override;
            void Trigger() override;
    };
}

#endif
