#include "calculator.h"

// Protótipos internos
static void calculator_create_buttons(Calculator* calc);
static void calculator_handle_events(Calculator* calc);
static void calculator_render(Calculator* calc);
static void calculator_draw_button(Calculator* calc, Button* btn);
static void calculator_button_click(Calculator* calc, int index);
static int calculator_get_button_at(Calculator* calc, int x, int y);
static double calculator_evaluate(const char* expression);
static void calculator_update_display(Calculator* calc);

Calculator* calculator_create(void) {
    Calculator* calc = (Calculator*)malloc(sizeof(Calculator));
    if (!calc) return NULL;
    
    memset(calc, 0, sizeof(Calculator));
    strcpy(calc->display, "0");
    
    return calc;
}

void calculator_init(Calculator* calc) {
    // Inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL não pôde inicializar! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }
    
    // Inicializar SDL_ttf
    if (TTF_Init() == -1) {
        printf("SDL_ttf não pôde inicializar! TTF_Error: %s\n", TTF_GetError());
        exit(1);
    }
    
    // Criar janela
    calc->window = SDL_CreateWindow(
        "Calculadora SDL2",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    
    if (!calc->window) {
        printf("Janela não pôde ser criada! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }
    
    // Criar renderer
    calc->renderer = SDL_CreateRenderer(calc->window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (!calc->renderer) {
        printf("Renderer não pôde ser criado! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }
    
    // Carregar fontes - MSYS2/Windows paths
    // Tenta primeiro no diretório atual, depois no sistema
    calc->font_display = TTF_OpenFont("arial.ttf", FONT_SIZE);
    calc->font_buttons = TTF_OpenFont("arial.ttf", BUTTON_FONT_SIZE);
    
    // Fallback para fontes do Windows
    if (!calc->font_display) {
        calc->font_display = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", FONT_SIZE);
    }
    if (!calc->font_buttons) {
        calc->font_buttons = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", BUTTON_FONT_SIZE);
    }
    
    // Fallback alternativo: Segoe UI (moderna no Win11)
    if (!calc->font_display) {
        calc->font_display = TTF_OpenFont("C:/Windows/Fonts/segoeui.ttf", FONT_SIZE);
    }
    if (!calc->font_buttons) {
        calc->font_buttons = TTF_OpenFont("C:/Windows/Fonts/segoeui.ttf", BUTTON_FONT_SIZE);
    }
    
    if (!calc->font_display || !calc->font_buttons) {
        printf("Aviso: Não foi possível carregar fontes. Verifique se arial.ttf existe.\n");
    }
    
    calculator_create_buttons(calc);
    calc->running = true;
}

static void calculator_create_buttons(Calculator* calc) {
    calc->button_count = 0;
    
    const char* button_labels[20] = {
        "C", "<-", "%", "/",
        "7", "8", "9", "*",
        "4", "5", "6", "-",
        "1", "2", "3", "+",
        "+-", "0", ".", "="
    };
    
    int button_types[20] = {
        2, 2, 2, 1,
        0, 0, 0, 1,
        0, 0, 0, 1,
        0, 0, 0, 1,
        2, 0, 0, 3
    };
    
    int start_x = BUTTON_MARGIN;
    int start_y = DISPLAY_HEIGHT + BUTTON_MARGIN * 2;
    
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 4; col++) {
            int index = row * 4 + col;
            if (index >= 20) break;
            
            Button btn;
            btn.rect.x = start_x + col * (BUTTON_SIZE + BUTTON_MARGIN);
            btn.rect.y = start_y + row * (BUTTON_SIZE + BUTTON_MARGIN);
            btn.rect.w = BUTTON_SIZE;
            btn.rect.h = BUTTON_SIZE;
            
            strncpy(btn.label, button_labels[index], 3);
            btn.label[3] = '\0';
            btn.type = button_types[index];
            btn.hover = false;
            btn.pressed = false;
            
            calc->buttons[calc->button_count++] = btn;
        }
    }
}

void calculator_run(Calculator* calc) {
    while (calc->running) {
        calculator_handle_events(calc);
        calculator_render(calc);
        SDL_Delay(16);
    }
}

static void calculator_handle_events(Calculator* calc) {
    SDL_Event e;
    
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                calc->running = false;
                break;
                
            case SDL_MOUSEMOTION:
                calc->mouse_x = e.motion.x;
                calc->mouse_y = e.motion.y;
                
                for (int i = 0; i < calc->button_count; i++) {
                    SDL_Point point = {calc->mouse_x, calc->mouse_y};
                    calc->buttons[i].hover = SDL_PointInRect(&point, &calc->buttons[i].rect);
                }
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                if (e.button.button == SDL_BUTTON_LEFT) {
                    int index = calculator_get_button_at(calc, e.button.x, e.button.y);
                    if (index != -1) {
                        calc->buttons[index].pressed = true;
                    }
                }
                break;
                
            case SDL_MOUSEBUTTONUP:
                if (e.button.button == SDL_BUTTON_LEFT) {
                    // Desativa todos os botões primeiro
                    for (int i = 0; i < calc->button_count; i++) {
                        calc->buttons[i].pressed = false;
                    }
                    
                    int index = calculator_get_button_at(calc, e.button.x, e.button.y);
                    if (index != -1) {
                        calculator_button_click(calc, index);
                    }
                }
                break;
                
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                    case SDLK_0: case SDLK_1: case SDLK_2: case SDLK_3:
                    case SDLK_4: case SDLK_5: case SDLK_6: case SDLK_7:
                    case SDLK_8: case SDLK_9:
                        {
                            char num_str[2] = {'0' + (e.key.keysym.sym - SDLK_0), '\0'};
                            if (strcmp(calc->display, "0") == 0 || 
                                strcmp(calc->display, "Erro") == 0) {
                                strcpy(calc->expression, num_str);
                            } else {
                                strcat(calc->expression, num_str);
                            }
                            calculator_update_display(calc);
                        }
                        break;
                        
                    case SDLK_PLUS:
                    case SDLK_KP_PLUS:
                        strcat(calc->expression, "+");
                        calculator_update_display(calc);
                        break;
                        
                    case SDLK_MINUS:
                    case SDLK_KP_MINUS:
                        strcat(calc->expression, "-");
                        calculator_update_display(calc);
                        break;
                        
                    case SDLK_ASTERISK:
                    case SDLK_KP_MULTIPLY:
                        strcat(calc->expression, "*");
                        calculator_update_display(calc);
                        break;
                        
                    case SDLK_SLASH:
                    case SDLK_KP_DIVIDE:
                        strcat(calc->expression, "/");
                        calculator_update_display(calc);
                        break;
                        
                    case SDLK_PERIOD:
                    case SDLK_KP_PERIOD:
                        strcat(calc->expression, ".");
                        calculator_update_display(calc);
                        break;
                        
                    case SDLK_RETURN:
                    case SDLK_KP_ENTER:
                        calculator_button_click(calc, 19);
                        break;
                        
                    case SDLK_ESCAPE:
                        calculator_button_click(calc, 0);
                        break;
                        
                    case SDLK_BACKSPACE:
                        calculator_button_click(calc, 1);
                        break;
                }
                break;
        }
    }
}

static void calculator_render(Calculator* calc) {
    // Fundo
    SDL_SetRenderDrawColor(calc->renderer, 240, 240, 240, 255);
    SDL_RenderClear(calc->renderer);
    
    // Display
    SDL_Rect display_rect = {
        BUTTON_MARGIN,
        BUTTON_MARGIN,
        WINDOW_WIDTH - 2 * BUTTON_MARGIN,
        DISPLAY_HEIGHT
    };
    
    SDL_SetRenderDrawColor(calc->renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(calc->renderer, &display_rect);
    SDL_SetRenderDrawColor(calc->renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(calc->renderer, &display_rect);
    
    // Texto do display
    if (calc->font_display) {
        SDL_Color text_color = {0, 0, 0, 255};
        
        SDL_Surface* text_surface = TTF_RenderText_Blended(
            calc->font_display, calc->display, text_color);
        
        if (text_surface) {
            SDL_Texture* text_texture = SDL_CreateTextureFromSurface(
                calc->renderer, text_surface);
            
            if (text_texture) {
                SDL_Rect text_rect = {
                    display_rect.x + display_rect.w - text_surface->w - 10,
                    display_rect.y + (display_rect.h - text_surface->h) / 2,
                    text_surface->w,
                    text_surface->h
                };
                
                SDL_RenderCopy(calc->renderer, text_texture, NULL, &text_rect);
                SDL_DestroyTexture(text_texture);
            }
            SDL_FreeSurface(text_surface);
        }
    }
    
    // Botões
    for (int i = 0; i < calc->button_count; i++) {
        calculator_draw_button(calc, &calc->buttons[i]);
    }
    
    SDL_RenderPresent(calc->renderer);
}

static void calculator_draw_button(Calculator* calc, Button* btn) {
    Uint8 r, g, b;
    
    // Cores base
    switch (btn->type) {
        case 0: // Número
            r=224; g=224; b=224; 
            break;
        case 1: // Operador
        case 3: // Igual
            r=255; g=149; b=0;
            break;
        case 2: // Especial
            r=212; g=212; b=210;
            break;
        default:
            r=224; g=224; b=224;
    }
    
    // Hover/Pressed
    if (btn->pressed) {
        r = (Uint8)(r * 0.8);
        g = (Uint8)(g * 0.8);
        b = (Uint8)(b * 0.8);
    } else if (btn->hover) {
        r = (Uint8)(r * 0.9);
        g = (Uint8)(g * 0.9);
        b = (Uint8)(b * 0.9);
    }
    
    SDL_SetRenderDrawColor(calc->renderer, r, g, b, 255);
    SDL_RenderFillRect(calc->renderer, &btn->rect);
    
    SDL_SetRenderDrawColor(calc->renderer, 180, 180, 180, 255);
    SDL_RenderDrawRect(calc->renderer, &btn->rect);
    
    // Texto
    if (calc->font_buttons) {
        SDL_Color text_color = {0, 0, 0, 255};
        
        SDL_Surface* text_surface = TTF_RenderText_Blended(
            calc->font_buttons, btn->label, text_color);
        
        if (text_surface) {
            SDL_Texture* text_texture = SDL_CreateTextureFromSurface(
                calc->renderer, text_surface);
            
            if (text_texture) {
                SDL_Rect text_rect = {
                    btn->rect.x + (btn->rect.w - text_surface->w) / 2,
                    btn->rect.y + (btn->rect.h - text_surface->h) / 2,
                    text_surface->w,
                    text_surface->h
                };
                
                SDL_RenderCopy(calc->renderer, text_texture, NULL, &text_rect);
                SDL_DestroyTexture(text_texture);
            }
            SDL_FreeSurface(text_surface);
        }
    }
}

static void calculator_button_click(Calculator* calc, int index) {
    if (index < 0 || index >= calc->button_count) return;
    
    const char* label = calc->buttons[index].label;
    
    if (strcmp(label, "C") == 0) {
        strcpy(calc->expression, "");
        strcpy(calc->display, "0");
    }
    else if (strcmp(label, "<-") == 0) {  // Backspace
        int len = strlen(calc->expression);
        if (len > 0) {
            calc->expression[len - 1] = '\0';
            calculator_update_display(calc);
        }
    }
	else if (strcmp(label, "+-") == 0) {  // Troca sinal
		if (strlen(calc->expression) > 0) {
			if (calc->expression[0] == '-') {
				memmove(calc->expression, calc->expression + 1, strlen(calc->expression));
			} else {
				// Verifica se há espaço para o '-'
				size_t len = strlen(calc->expression);
				if (len < MAX_EXPRESSION - 1) {
				memmove(calc->expression + 1, calc->expression, len + 1);
				calc->expression[0] = '-';
				}
			}
			calculator_update_display(calc);
			}
		}
    else if (strcmp(label, "=") == 0) {
        if (strlen(calc->expression) > 0) {
            double result = calculator_evaluate(calc->expression);
            
            if (isnan(result)) {
                strcpy(calc->display, "Erro");
            } else {
                if (result == (int)result) {
                    snprintf(calc->display, MAX_DISPLAY, "%d", (int)result);
                } else {
                    snprintf(calc->display, MAX_DISPLAY, "%.10g", result);
                }
                strcpy(calc->expression, calc->display);
            }
        }
    }
    else if (strcmp(label, "%") == 0) {
        if (strlen(calc->expression) > 0) {
            double result = calculator_evaluate(calc->expression);
            result /= 100.0;
            
            if (isnan(result)) {
                strcpy(calc->display, "Erro");
            } else {
                snprintf(calc->display, MAX_DISPLAY, "%.10g", result);
                strcpy(calc->expression, calc->display);
            }
        }
    }
    else {
        // Número ou operador
        if (strcmp(calc->display, "0") == 0 || 
            strcmp(calc->display, "Erro") == 0) {
            strcpy(calc->expression, label);
        } else {
            strcat(calc->expression, label);
        }
        calculator_update_display(calc);
    }
}

static void calculator_update_display(Calculator* calc) {
    if (strlen(calc->expression) == 0) {
        strcpy(calc->display, "0");
    } else {
        strncpy(calc->display, calc->expression, MAX_DISPLAY - 1);
        calc->display[MAX_DISPLAY - 1] = '\0';
    }
}

static double calculator_evaluate(const char* expression) {
    // Parser simplificado - usa avaliação left-to-right
    char expr_copy[MAX_EXPRESSION];
    strncpy(expr_copy, expression, MAX_EXPRESSION - 1);
    expr_copy[MAX_EXPRESSION - 1] = '\0';
    
    // Verificar divisão por zero simples
    if (strstr(expr_copy, "/0") != NULL) {
        return NAN;
    }
    
    double result = 0.0;
    char* endptr;
    
    // Tenta converter número inicial
    result = strtod(expr_copy, &endptr);
    
    // Processa operadores restantes
    while (*endptr != '\0') {
        char op = *endptr;
        endptr++;
        
        double next = strtod(endptr, &endptr);
        
        switch (op) {
            case '+': result += next; break;
            case '-': result -= next; break;
            case '*': result *= next; break;
            case '/': 
                if (next == 0.0) return NAN;
                result /= next; 
                break;
        }
    }
    
    return result;
}

static int calculator_get_button_at(Calculator* calc, int x, int y) {
    for (int i = 0; i < calc->button_count; i++) {
        SDL_Point point = {x, y};
        if (SDL_PointInRect(&point, &calc->buttons[i].rect)) {
            return i;
        }
    }
    return -1;
}

void calculator_cleanup(Calculator* calc) {
    if (calc->font_display) TTF_CloseFont(calc->font_display);
    if (calc->font_buttons) TTF_CloseFont(calc->font_buttons);
    
    if (calc->renderer) SDL_DestroyRenderer(calc->renderer);
    if (calc->window) SDL_DestroyWindow(calc->window);
    
    TTF_Quit();
    SDL_Quit();
    
    free(calc);
}