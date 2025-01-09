// file: joystick.c

#include "joystick.h"
#include "logging.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int CIRCLE_RADIUS = 20;

typedef struct {
    int stick1X;
    int stick1Y;
    int stick2X;
    int stick2Y;
} JoystickAxesState;

static JoystickAxesState joystickState = { 0, 0, 0, 0 };
static SDL_Joystick* joystickDevice = NULL;
static SDL_Window* mainWindow = NULL;
static SDL_Renderer* mainRenderer = NULL;

void SDL_RenderDrawCircle(SDL_Renderer* renderer, const int x, const int y, const int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            const int dx = radius - w;
            const int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

int joystickSubsystemInit() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        loggingAsyncLog(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Error initializing SDL: %s", SDL_GetError());
        return -1;
    }

    // Create a window for visualization
    mainWindow = SDL_CreateWindow("Joystick Visualization", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (mainWindow == NULL) {
        loggingAsyncLog(SDL_LOG_CATEGORY_VIDEO, SDL_LOG_PRIORITY_ERROR, "Error creating window: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // Create a renderer for drawing
    mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);
    if (mainRenderer == NULL) {
        loggingAsyncLog(SDL_LOG_CATEGORY_RENDER, SDL_LOG_PRIORITY_ERROR, "Error creating renderer: %s", SDL_GetError());
        SDL_DestroyWindow(mainWindow);
        SDL_Quit();
        return -1;
    }

    // Check for joysticks connected
    if (SDL_NumJoysticks() < 1) {
        loggingAsyncLog(SDL_LOG_CATEGORY_INPUT, SDL_LOG_PRIORITY_WARN, "There are no joysticks connected!");
        SDL_DestroyRenderer(mainRenderer);
        SDL_DestroyWindow(mainWindow);
        SDL_Quit();
        return -1;
    }

    // Open joystick (first one)
    joystickDevice = SDL_JoystickOpen(0);
    if (joystickDevice == NULL) {
        loggingAsyncLog(SDL_LOG_CATEGORY_INPUT, SDL_LOG_PRIORITY_ERROR, "Error opening joystick: %s", SDL_GetError());
        SDL_DestroyRenderer(mainRenderer);
        SDL_DestroyWindow(mainWindow);
        SDL_Quit();
        return -1;
    }

    loggingAsyncLog(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Joystick initialized successfully.");
    return 0;
}

void joystickSubsystemProcess() {
    SDL_Event event;
    int quit = 0;

    while (!quit) {
        // Event handling
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
            else if (event.type == SDL_JOYAXISMOTION) {
                // Read joystick state (axes)
                if (event.jaxis.axis == 0) {
                    joystickState.stick1X = event.jaxis.value;
                }
                else if (event.jaxis.axis == 1) {
                    joystickState.stick1Y = event.jaxis.value;
                }
                else if (event.jaxis.axis == 2) {
                    joystickState.stick2X = event.jaxis.value;
                }
                else if (event.jaxis.axis == 3) {
                    joystickState.stick2Y = event.jaxis.value;
                }

                // Log joystick state asynchronously
                loggingAsyncLog(SDL_LOG_CATEGORY_INPUT, SDL_LOG_PRIORITY_INFO, "Stick 1: X = %d, Y = %d", joystickState.stick1X, joystickState.stick1Y);
                loggingAsyncLog(SDL_LOG_CATEGORY_INPUT, SDL_LOG_PRIORITY_INFO, "Stick 2: X = %d, Y = %d", joystickState.stick2X, joystickState.stick2Y);
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(mainRenderer);

        // Map joystick values (-32768 to 32767) to screen coordinates
        const int stick1_x = (joystickState.stick1X + 32768) * SCREEN_WIDTH / 65536;
        const int stick1_y = (joystickState.stick1Y + 32768) * SCREEN_HEIGHT / 65536;
        const int stick2_x = (joystickState.stick2X + 32768) * SCREEN_WIDTH / 65536;
        const int stick2_y = (joystickState.stick2Y + 32768) * SCREEN_HEIGHT / 65536;

        // Draw first stick as a circle
        SDL_SetRenderDrawColor(mainRenderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawCircle(mainRenderer, stick1_x, stick1_y, CIRCLE_RADIUS);

        // Draw second stick as a circle
        SDL_SetRenderDrawColor(mainRenderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawCircle(mainRenderer, stick2_x, stick2_y, CIRCLE_RADIUS);

        // Update screen
        SDL_RenderPresent(mainRenderer);

        // Delay to limit frame rate
        SDL_Delay(16); // ~60 FPS
    }
}

void joystickSubsystemClose() {
    // Close joystick
    if (joystickDevice != NULL) {
        SDL_JoystickClose(joystickDevice);
        joystickDevice = NULL;
    }

    // Destroy renderer and window
    if (mainRenderer != NULL) {
        SDL_DestroyRenderer(mainRenderer);
        mainRenderer = NULL;
    }
    if (mainWindow != NULL) {
        SDL_DestroyWindow(mainWindow);
        mainWindow = NULL;
    }

    // Quit SDL
    SDL_Quit();
}