
#include <ctime>

namespace fg {
    namespace diag {
        class LogInterface {
        public:
            virtual void msgError(const char *msg, ...) const = 0;
            virtual void msgWarning(const char *msg, ...) const = 0;
            virtual void msgInfo(const char *msg, ...) const = 0;
        };

        class DefaultLogger : public LogInterface {
        public:
            DefaultLogger(bool fileOutput = false, bool consoleOutput = true);

            void msgError(const char *msg, ...) const override;
            void msgWarning(const char *msg, ...) const override;
            void msgInfo(const char *msg, ...) const override;
        };
    }
}