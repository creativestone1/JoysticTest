// file: joystick.h

#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <SDL.h>

// Initialize the joystick subsystem
int joystickSubsystemInit();

// Process joystick events
void joystickSubsystemProcess();

// Close the joystick subsystem
void joystickSubsystemClose();

#endif // JOYSTICK_H