#include <stdio.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024

void testFunction(int x, float y);
int sum(int a, int b);
char* escapeSequencesTest(char* str);

int main() {
    int x = 10;
    float y = 20.5;
    char ch = 'A';  

    char* str = "Hello, World! \nThis is a string with a tab\tand a newline.\n";
    char* escapeStr = "This is a backspace\bcharacter.\nThis is a carriage return\rnew text.";
    char* backslashStr = "Here is a backslash: \\\nAnd a double quote: \"\nAnd a single quote: \'";

    testFunction(x, y);

    int result = sum(x, 5);
    printf("Result of sum: %d\n", result);

    if (x == y) {
        printf("x equals y\n");
    } else if (x > y) {
        printf("x is greater than y\n");
    } else {
        printf("y is greater than x\n");
    }

    for (int i = 0; i < 5; i++) {
        printf("i: %d\n", i);
    }

    int* ptr = &x;
    *ptr = 15;
    printf("Updated x via pointer: %d\n", x);

    printf("Processed string with escape sequences: %s\n", str);
    printf("Escape sequence test string: %s\n", escapeStr);
    printf("Backslash and quote handling: %s\n", backslashStr);

    return 0;
}

void testFunction(int x, float y) {
    printf("Test function called with x = %d, y = %.2f\n", x, y);
}

int sum(int a, int b) {
    return a + b;
}

char* escapeSequencesTest(char* str) {
    return str;
}
