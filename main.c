#define SDL_MAIN_HANDLED

#include "logging.h"
#include "joystick.h"

int main(int argc, char* argv[]) {
    loggingAsyncInit();

    if (joystickSubsystemInit() == -1) return -1;

    joystickSubsystemProcess();
    joystickSubsystemClose();

    loggingAsyncShutdown();
    return 0;
}