#include "logger.h"
#include <string>
#include <iostream>
#include <fstream>

namespace helpers {
    Logger::Logger(std::string log_filename) {
        this->log_filename = log_filename;
        this->Clear();
    }

    void Logger::Write(std::string text) {
        std::ofstream log_file;
        log_file.open(this->log_filename, std::ios_base::app);
        log_file << text;
        log_file.close();
    }

    void Logger::WriteLine(std::string text) {
        this->Write(text + "\n");
    }

    void Logger::Clear() {
        std::ofstream log_file;
        log_file.open(this->log_filename,
                std::ofstream::out | std::ofstream::trunc);
        log_file.close();
    }
}
