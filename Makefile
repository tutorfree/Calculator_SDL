# Variáveis
CC = gcc
CFLAGS = -Wall -Wextra -std=c99

# Ordem correta das bibliotecas para evitar o erro "undefined reference to WinMain"
LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lm

# Flags de Ligação:
# Use -mwindows para esconder o terminal (Produção)
# Use -mconsole para exibir o terminal (Debug/Teste)
LDFLAGS = -mwindows

# Nome do executável
TARGET = calculadora.exe

# Regra principal (executada ao digitar apenas 'make')
all: $(TARGET)

# Compilação e Ligação
$(TARGET): main.c calculator.c calculator.h
	$(CC) $(CFLAGS) main.c calculator.c -o $(TARGET) $(LIBS) $(LDFLAGS)

# Limpeza dos arquivos gerados
clean:
	rm -f $(TARGET)