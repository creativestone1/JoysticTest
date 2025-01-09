// file: logging.c

#include "logging.h"
#include <stdio.h>
#include <time.h>

#define LOG_QUEUE_SIZE 256
#define LOG_ENTRY_SIZE 256

#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN "\033[36m"
#define COLOR_WHITE "\033[37m"

typedef struct {
    int category;
    SDL_LogPriority priority;
    char message[LOG_ENTRY_SIZE];
} LoggingMessage;

LoggingMessage logMessageQueue[LOG_QUEUE_SIZE];
int logQueueStart = 0;
int logQueueEnd = 0;
SDL_mutex* logMutex = NULL;
SDL_cond* logCondition = NULL;

// Get the log category name
static const char* getLogCategoryName(const int category) {
    switch (category) {
        case SDL_LOG_CATEGORY_APPLICATION: return "APPLICATION";
        case SDL_LOG_CATEGORY_ERROR: return "ERROR";
        case SDL_LOG_CATEGORY_SYSTEM: return "SYSTEM";
        case SDL_LOG_CATEGORY_AUDIO: return "AUDIO";
        case SDL_LOG_CATEGORY_VIDEO: return "VIDEO";
        case SDL_LOG_CATEGORY_RENDER: return "RENDER";
        case SDL_LOG_CATEGORY_INPUT: return "INPUT";
        case SDL_LOG_CATEGORY_TEST: return "TEST";
        default: return "UNKNOWN";
    }
}

// Thread function to process logs
static int logThreadFunc(void* arg) {
    while (1) {
        SDL_LockMutex(logMutex);

        while (logQueueStart == logQueueEnd) { // Wait for logs in the queue
            SDL_CondWait(logCondition, logMutex);
        }

        // Process log entry
        LoggingMessage log = logMessageQueue[logQueueStart];
        logQueueStart = (logQueueStart + 1) % LOG_QUEUE_SIZE;

        SDL_UnlockMutex(logMutex);

        // Get the current time
        time_t rawtime;
        struct tm* timeinfo;
        char timeBuffer[24];
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeinfo);

        // Choose color based on priority
        const char* color;
        switch (log.priority) {
            case SDL_LOG_PRIORITY_VERBOSE: color = COLOR_WHITE; break;
            case SDL_LOG_PRIORITY_DEBUG: color = COLOR_CYAN; break;
            case SDL_LOG_PRIORITY_INFO: color = COLOR_GREEN; break;
            case SDL_LOG_PRIORITY_WARN: color = COLOR_YELLOW; break;
            case SDL_LOG_PRIORITY_ERROR: color = COLOR_RED; break;
            case SDL_LOG_PRIORITY_CRITICAL: color = COLOR_MAGENTA; break;
            default: color = COLOR_RESET; break;
        }

        // Print the log message to the console
        fprintf(stdout, "%s[%s] [Priority: %d] [Category: %s] %s%s\n",
                color, timeBuffer, log.priority, getLogCategoryName(log.category), log.message, COLOR_RESET);
    }

    return 0;
}

void loggingAsyncInit() {
    logMutex = SDL_CreateMutex();
    logCondition = SDL_CreateCond();
    SDL_CreateThread(logThreadFunc, "LogThread", NULL);
}

void loggingAsyncLog(const int category, const SDL_LogPriority priority, const char* format, ...) {
    SDL_LockMutex(logMutex);

    if ((logQueueEnd + 1) % LOG_QUEUE_SIZE != logQueueStart) { // Check if the queue is not full
        va_list args;
        va_start(args, format);
        vsnprintf(logMessageQueue[logQueueEnd].message, LOG_ENTRY_SIZE, format, args);
        va_end(args);

        logMessageQueue[logQueueEnd].category = category;
        logMessageQueue[logQueueEnd].priority = priority;
        logQueueEnd = (logQueueEnd + 1) % LOG_QUEUE_SIZE;

        SDL_CondSignal(logCondition); // Notify the logging thread
    }

    SDL_UnlockMutex(logMutex);
}

void loggingAsyncShutdown() {
    SDL_DestroyMutex(logMutex);
    SDL_DestroyCond(logCondition);
}