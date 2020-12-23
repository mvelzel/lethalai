#ifndef LOGGER_H
#define LOGGER_H

#include <string>

namespace helpers {
    class Logger {
        public:
            Logger(std::string log_filename);
            void Write(std::string text);
            void WriteLine(std::string text);
            void Clear();
        private:
            std::string log_filename;
    };
}

#endif
