#pragma once

typedef void (*func)(char*);

void mk_handle_cmd(char* cmd);
void mk_register_cmd(char* name, void* entry);
