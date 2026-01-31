#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 480
#define DISPLAY_HEIGHT 80
#define BUTTON_SIZE 60
#define BUTTON_MARGIN 10
#define FONT_SIZE 32
#define BUTTON_FONT_SIZE 24
#define MAX_EXPRESSION 256
#define MAX_DISPLAY 32

typedef struct {
    SDL_Rect rect;
    char label[8];
    int type; // 0: num, 1: op, 2: special, 3: equal
    bool hover;
    bool pressed;
} Button;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font_display;
    TTF_Font* font_buttons;
    
    char expression[MAX_EXPRESSION];
    char display[MAX_DISPLAY];
    
    Button buttons[20];
    int button_count;
    
    int mouse_x, mouse_y;
    bool running;
} Calculator;

Calculator* calculator_create(void);
void calculator_init(Calculator* calc);
void calculator_run(Calculator* calc);
void calculator_cleanup(Calculator* calc);

#endif