#ifndef EVENT_H
#define EVENT_H

#include <Windows.h>

namespace events {
    class Event {
        protected:
            int id;
            bool enabled = false;
            void (*trigger_function)(Event*, unsigned int) = NULL;
        public:
            Event(int id);
            int GetId();
            void Enable();
            void Disable();
            void SetTriggerFunction(void (*trigger_function)(Event*, unsigned int));
            virtual void Trigger() = 0;
            virtual void Inject() = 0;
    };
}

#endif
