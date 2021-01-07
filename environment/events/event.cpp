#include "event.h"

namespace events {
    Event::Event(int id) {
        this->id = id;
    }

    int Event::GetId() {
        return this->id;
    }
    
    void Event::Enable() {
        this->enabled = true;
    }

    void Event::Disable() {
        this->enabled = false;
    }

    void Event::SetTriggerFunction(void (*trigger_function)(Event *, unsigned int)) {
        this->trigger_function = trigger_function;
    }
}
