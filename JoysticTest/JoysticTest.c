#include <SDL.h>
#include <stdio.h>
#include <locale.h>
// Структура для хранения значений стиков джойстика
typedef struct {
    int x1;  // Значение оси X первого стика
    int y1;  // Значение оси Y первого стика
    int x2;  // Значение оси X второго стика
    int y2;  // Значение оси Y второго стика
} JoystickState;
SDL_Joystick* joystick;
JoystickState jsState = { 0, 0, 0, 0 };

int joysticInit() {
    // Инициализация SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        printf("Ошибка инициализации SDL: %s\n", SDL_GetError());
        return -1;
    }

    // Проверяем, подключен ли джойстик
    if (SDL_NumJoysticks() < 1) {
        printf("Нет подключенных джойстиков!\n");
        SDL_Quit();
        return -1;
    }

    // Открываем первый доступный джойстик
    joystick = SDL_JoystickOpen(0);
    if (joystick == NULL) {
        printf("Не удалось открыть джойстик: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    return 0;
}
void joystickProcess() {
    SDL_Event e;
    int quit = 0;

    while (!quit) {
        // Обработка событий
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
            else if (e.type == SDL_JOYAXISMOTION) {
                // Чтение значений осей
                if (e.jaxis.axis == 0) {
                    jsState.x1 = e.jaxis.value;
                }
                else if (e.jaxis.axis == 1) {
                    jsState.y1 = e.jaxis.value;
                }
                else if (e.jaxis.axis == 2) {
                    jsState.x2 = e.jaxis.value;
                }
                else if (e.jaxis.axis == 3) {
                    jsState.y2 = e.jaxis.value;
                }

                // Вывод значений стиков
                printf("Стик 1 - Ось X: %d, Ось Y: %d\n", jsState.x1, jsState.y1);
                printf("Стик 2 - Ось X: %d, Ось Y: %d\n", jsState.x2, jsState.y2);
            }
        }
    }
    return;
}
void joysticClose() {
    // Закрытие джойстика
    SDL_JoystickClose(joystick);
    joystick = NULL;

    // Завершение работы SDL
    SDL_Quit();
    return;
}
int main() {
    setlocale(0, "ru");
    if (joysticInit() == -1)
        return -1;
    joystickProcess();
    joysticClose();
}
