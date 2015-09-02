
namespace fg {
    namespace diag {
        DefaultLogger::DefaultLogger(bool fileOutput, bool consoleOutput) {

        }

        char _text[2048] = {0};

        void DefaultLogger::msgError(const char *msg, ...) const {
            std::time_t ttime = std::time(nullptr);
            std::tm localTime = *std::localtime(&ttime);
            std::chrono::time_point <std::chrono::system_clock> tcur = std::chrono::system_clock::now();
            std::chrono::milliseconds ms = std::chrono::duration_cast <std::chrono::milliseconds> (tcur.time_since_epoch());

            va_list ap;
            va_start(ap, msg);
            vsprintf(_text, msg, ap);
            va_end(ap);

            printf("[Error] %02d:%02d:%02d:%03d > %s\n", localTime.tm_hour, localTime.tm_min, localTime.tm_sec, int(ms.count() % 1000), _text);
        }

        void DefaultLogger::msgWarning(const char *msg, ...) const {
            std::time_t ttime = std::time(nullptr);
            std::tm localTime = *std::localtime(&ttime);
            std::chrono::time_point <std::chrono::system_clock> tcur = std::chrono::system_clock::now();
            std::chrono::milliseconds ms = std::chrono::duration_cast <std::chrono::milliseconds> (tcur.time_since_epoch());

            va_list ap;
            va_start(ap, msg);
            vsprintf(_text, msg, ap);
            va_end(ap);

            printf("[Warning] %02d:%02d:%02d:%03d > %s\n", localTime.tm_hour, localTime.tm_min, localTime.tm_sec, int(ms.count() % 1000), _text);
        }

        void DefaultLogger::msgInfo(const char *msg, ...) const {
            std::time_t ttime = std::time(nullptr);
            std::tm localTime = *std::localtime(&ttime);
            std::chrono::time_point <std::chrono::system_clock> tcur = std::chrono::system_clock::now();
            std::chrono::milliseconds ms = std::chrono::duration_cast <std::chrono::milliseconds> (tcur.time_since_epoch());

            va_list ap;
            va_start(ap, msg);
            vsprintf(_text, msg, ap);
            va_end(ap);

            printf("[Info] %02d:%02d:%02d:%03d > %s\n", localTime.tm_hour, localTime.tm_min, localTime.tm_sec, int(ms.count() % 1000), _text);
        }
    }
}