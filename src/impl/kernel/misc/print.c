#include "print.h"
#include <stdint.h>

const static size_t NUM_COLS = 80;
const static size_t NUM_ROWS = 25;
const static size_t NUM_MENU_ROWS = 10;

struct Char {
    uint8_t character;
    uint8_t color;
};

// where "video memory" is located
// the rest is to keep track of what to do
struct Char* buffer;
size_t col = 0;
size_t row = 0;
uint8_t color = PRINT_COLOR_WHITE | PRINT_COLOR_BLACK << 4;

char nibbles[] = "0123456789ABCDEF";

void clear_row(size_t row) {
    buffer = (struct Char*) 0xffffffff800b8000;

    struct Char empty = (struct Char) {
        character: ' ',
        color: color,
    };

    for (size_t col = 0; col < NUM_COLS; col++) {
        buffer[col + NUM_COLS * row] = empty;
        
        if (col < 0)
            continue;
    }
}

void print_clear() {
    for (size_t i = 0; i < NUM_ROWS; i++) {
        clear_row(i);
    }
}

void print_newline() {
    col = 0;

    if (row < NUM_ROWS - 1) {
        row++;
        return;
    }

    // if we have exceeded number of rows, move everything up one
    for (size_t row = NUM_MENU_ROWS + 1; row < NUM_ROWS; row++) {
        for (size_t col = 0; col < NUM_COLS; col++) {
            struct Char character = buffer[col + NUM_COLS * row];
            buffer[col + NUM_COLS * (row - 1)] = character;
        }
    }

    clear_row(NUM_ROWS - 1);
}

void print_handle_backspace() {
    if (col > 0) {
        col--;
    } else {
        if (row <= NUM_MENU_ROWS+1)
            return;

        row--;
        col = NUM_COLS;
    }

    buffer[col + NUM_COLS * row] = (struct Char) {
        character: (uint8_t) ' ',
        color: color,
    };
}

void print_char(char character) {
    if (character == '\n') {
        print_newline();
        return;
    }
    
    if (character == '\b') {
        print_handle_backspace();
        return;
    }

    if (col >= NUM_COLS) {
        print_newline();
    }

    buffer[col + NUM_COLS * row] = (struct Char) {
        character: (uint8_t) character,
        color: color,
    };

    col++;
}

void print_str(char* str) {
    for (size_t i = 0; 1; i++) {
        char character = (uint8_t) str[i];

        if (character == '\0') {
            return;
        }

        print_char(character);
    }
}

void print_error(char* str) {
    uint8_t c = color;

    print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_RED);
    print_str(str);
    
    color = c;
}

void print_nibble(uint8_t byte) {
    print_char(nibbles[byte & 0xf]);
}

void print_byte(uint8_t byte) {
    print_str("0x");

    for (int i = 0 ; i < 2; i++)
        print_nibble(byte >> (60 - i * 4));
}

void print_word(uint16_t word) {
    print_str("0x");

    for (int i = 0 ; i < 4; i++)
        print_nibble(word >> (60 - i * 4));
}

void print_dword(uint32_t dword) {
    print_str("0x");

    for (int i = 0 ; i < 8; i++)
        print_nibble(dword >> (60 - i * 4));
}

void print_qword(uint64_t qword) {
    print_str("0x");

    for (int i = 0 ; i < 16; i++)
        print_nibble(qword >> (60 - i * 4));
}

void print_set_color(uint8_t foreground, uint8_t background) {
    color = foreground + (background << 4);
}

void print_menu() {
    print_str(" |\\__/,|   (`\\\n");
    print_str(" |_ _  |.--.) )\n");
    print_str(" ( T   )     /\n");
    print_str("(((^_(((/(((_/\n");
    print_str("Welcome to Magical Kernel Ultra!\n");
    print_str("This is a work in progress, I am pretty busy and lazy so this will be shit and slow paced... but hopefully I get something done!\n\n");

    print_set_color(PRINT_COLOR_BLACK, PRINT_COLOR_LIGHT_GREEN);
    print_str("Author: Luis Abraham\n");
    print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BLACK);
    
    for (int i = 0; i < NUM_COLS; i++)
        print_char('-');
    
    print_char('\n');
}

