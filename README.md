# SDL2 Calculator in C

Uma calculadora de alto desempenho desenvolvida em linguagem C utilizando a biblioteca gráfica **SDL2** e **SDL_ttf**. O projeto foca em eficiência, baixo consumo de memória e uma interface minimalista e funcional.

## ✨ Funcionalidades

- **Operações Básicas**: Adição, subtração, multiplicação e divisão.
- **Funções Especiais**: Cálculo de porcentagem (`%`), inversão de sinal (`+/-`) e limpeza de memória (`C`).
- **Entrada Híbrida**: 
  - Suporte total ao **mouse** (interface visual).
  - Suporte ao **teclado** (números, operadores, Enter, Backspace e Esc).
- **Interface Responsiva**: Feedback visual de botões ao passar o mouse (hover) e ao clicar (pressed).

## 🚀 Diferencial de Desempenho

Diferente de calculadoras modernas feitas em frameworks de alto nível (como Python ou Electron), esta calculadora nativa se destaca pela eficiência:

| Métrica | SDL2 Calculator (C) | Modelo em Python (Pygame) |
| :--- | :--- | :--- |
| **Uso de RAM** | **~8 MB** | ~50 MB |
| **Inicialização** | Instantânea (< 50ms) | ~500ms a 1s |
| **Dependências** | DLLs nativas (~3MB) | Runtime Python (~25MB) |

## 🛠️ Tecnologias Utilizadas

- **Linguagem**: C (Padrão C99/C11)
- **Gráficos**: [SDL2](https://www.libsdl.org/)
- **Renderização de Texto**: [SDL_ttf](https://www.libsdl.org/projects/SDL_ttf/)
- **Compilador**: GCC (via MSYS2/MinGW-w64)

## 📁 Estrutura do Projeto

- `main.c`: Ponto de entrada do programa.
- `calculator.c`: Lógica de UI, eventos e cálculos.
- `calculator.h`: Definições de estruturas e constantes.
- `arial.ttf`: Fonte utilizada na interface (deve estar na pasta do executável).
