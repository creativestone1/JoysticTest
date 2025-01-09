// file: logging.h

#ifndef LOGGING_H
#define LOGGING_H

#include <SDL.h>

// Initialize asynchronous logging
void loggingAsyncInit();

// Log a message asynchronously
void loggingAsyncLog(const int category, const SDL_LogPriority priority, const char* format, ...);

// Shutdown logging system
void loggingAsyncShutdown();

#endif // LOGGING_H