#include "event_observer.h"

#include "events/death_event.h"
#include "events/bunt_event.h"
#include "events/hit_event.h"

namespace environment {
    EventObserver::EventObserver(bool start_enabled) {
        this->has_observer = false;
        events::Event* death_event = new events::DeathEvent(0);
        events::Event* bunt_event = new events::BuntEvent(1);
        events::Event* hit_event = new events::HitEvent(2);

        this->events[0] = death_event;
        this->events[1] = bunt_event;
        this->events[2] = hit_event;

        if (start_enabled) {
            death_event->Enable();
            bunt_event->Enable();
            hit_event->Enable();
        }
    }

    void EventObserver::EnableEvent(int identifier) {
        this->events[identifier]->Enable();
    }

    void EventObserver::DisableEvent(int identifier) {
        this->events[identifier]->Disable();
    }

    void EventObserver::SetObserver(void (*observer)(events::Event*, unsigned int)) {
        for (auto x : this->events) {
            x.second->SetTriggerFunction(observer);
        }
    }
}
