# Guia de Compilação e Instalação

Este guia assume que você está utilizando **Windows** com o ambiente **MSYS2 (MinGW-w64)**.

## 1. Pré-requisitos

Abra o terminal do seu MSYS2 (MingW64) e instale o compilador e as bibliotecas necessárias:

```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_ttf
```

## 2. Preparação dos Arquivos

Certifique-se de que os seguintes arquivos estão na mesma pasta:

    main.c

    calculator.c

    calculator.h

    arial.ttf (Copie de C:/Windows/Fonts/arial.ttf para a pasta do projeto)

## 3. Compilação

Modo Console (para testes/debug)

Use este comando para ver erros de printf no terminal:
```bash
gcc main.c calculator.c -o calculadora -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lm
```

Modo GUI (versão final)

Use este comando para gerar o .exe que não abre o terminal ao fundo:
```bash
gcc main.c calculator.c -o calculadora -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lm -mwindows
```

## 4. Configuração das DLLs

Para que o calculadora.exe funcione fora do terminal do MSYS2, você deve copiar as seguintes DLLs da pasta C:\msys64\mingw64\bin\ para a pasta onde está o seu executável:

    SDL2.dll

    SDL2_ttf.dll

    libfreetype-6.dll

    zlib1.dll

    libbz2-1.dll

    libpng16-16.dll

    libharfbuzz-0.dll

    libglib-2.0-0.dll

    libintl-8.dll

    libiconv-2.dll

## 5. Execução

Basta clicar duas vezes em calculadora.exe. Certifique-se de que o arquivo arial.ttf esteja presente, caso contrário, os textos não serão exibidos.