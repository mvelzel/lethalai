#ifndef BUNT_EVENT_H
#define BUNT_EVENT_H

#include "event.h"

namespace events {
    class BuntEvent : public Event {
        public:
            BuntEvent(int id);
            void Inject() override;
            void Trigger() override;
    };
}

#endif
