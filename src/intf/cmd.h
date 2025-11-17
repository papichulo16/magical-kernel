#pragma once

typedef void (*func)(char*);

void mk_handle_cmd(char* cmd);
void mk_register_cmd(char* name, char* desc, void* entry, int list);

void cmd_help();
