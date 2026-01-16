#include <stdint.h>
#include <stdbool.h>

#include "print.h"
#include "thread.h"
#include "drivers/ps2.h"
#include "cmd.h"
#include "mklib.h"
#include "inlines.c"

void *keyboard_sema;
bool shift = false;

char cmd_buf[256];
uint8_t cmd_pos = 0;

void mk_ps2_keyboard_driver() {

  mk_create_sema(&keyboard_sema, 0);

  while (1) {

    // for now keep the thread going, everything is slow so it will be buggy
    //mk_sema_take(keyboard_sema);

    // status register bit 1 = output  buffer status
    if ((inb(0x64) & 1) == 0)
      continue;

    uint8_t scancode = inb(0x60); // read the data port

    // ignore key releases
    if (scancode & 0x80) {
      scancode &= 0x7F;
      if (scancode == 0x2A || scancode == 0x36)
        shift = false;
    } else {
      if (scancode == 0x2A || scancode == 0x36) {
        shift = true;

        continue;
      }

      char c = translate_scancode_set_1(scancode, shift);

      if (c == '\n') {
        print_char(c);

        mk_handle_cmd((char *)&cmd_buf);
        _memset(&cmd_buf, 0, sizeof(cmd_buf));
        cmd_pos = 0;

        print_str("$ ");
        
        continue;
      }

      if (cmd_pos == 255) {
        continue;
      }

      cmd_buf[cmd_pos++] = c;
      print_char(c);
    }

  }
}


