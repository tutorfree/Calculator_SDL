#include "calculator.h"

int main(int argc, char* argv[]) {
    Calculator* calc = calculator_create();
    if (!calc) return 1;

    calculator_init(calc);
    calculator_run(calc);
    calculator_cleanup(calc);

    return 0;
}