
namespace fg {
    namespace diag {
        DefaultLogger::DefaultLogger(bool fileOutput, bool consoleOutput) {

        }

        char _text[2048] = {0};

        void DefaultLogger::msgError(const char *msg, ...) const {
            SYSTEMTIME  stime;
            size_t      cvtRes = 0;
            va_list     ap;

            va_start(ap, msg);
            vsprintf_s(_text, msg, ap);
            va_end(ap);

            GetLocalTime((LPSYSTEMTIME)&stime);
            printf("[Error] %02d:%02d:%02d:%03d > %s\n", stime.wHour, stime.wMinute, stime.wSecond, stime.wMilliseconds, _text);
        }

        void DefaultLogger::msgWarning(const char *msg, ...) const {
            SYSTEMTIME  stime;
            size_t      cvtRes = 0;
            va_list     ap;

            va_start(ap, msg);
            vsprintf_s(_text, msg, ap);
            va_end(ap);

            GetLocalTime((LPSYSTEMTIME)&stime);
            printf("[Warning] %02d:%02d:%02d:%03d > %s\n", stime.wHour, stime.wMinute, stime.wSecond, stime.wMilliseconds, _text);
        }

        void DefaultLogger::msgInfo(const char *msg, ...) const {
            SYSTEMTIME  stime;
            size_t      cvtRes = 0;
            va_list     ap;

            va_start(ap, msg);
            vsprintf_s(_text, msg, ap);
            va_end(ap);

            GetLocalTime((LPSYSTEMTIME)&stime);
            printf("[Info] %02d:%02d:%02d:%03d > %s\n", stime.wHour, stime.wMinute, stime.wSecond, stime.wMilliseconds, _text);
        }
    }
}