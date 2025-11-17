#include "cmd.h"
#include "print.h"
#include "slab.h"
#include "mklib.h"

struct cmd_t {
    char* cmd;
    void* entry;
    
    struct cmd_t* next;
};

struct cmd_t* cmd_list_head = 0;

void mk_register_cmd(char *name, void *entry) {
    struct cmd_t* obj = mkmalloc(sizeof(struct cmd_t));
    
    obj->cmd = name;
    obj->entry = entry;
    obj->next = cmd_list_head;

    cmd_list_head = obj;
}

void mk_handle_cmd(char *cmd) {
    struct cmd_t* cur = cmd_list_head;
    
    while (cur != 0) {
        if (strcmp(cur->cmd, cmd) != 0) {
            cur = cur->next;
            continue;
        }
        
        ((func) cur->entry)(cmd + strlen(cur->cmd) + 1);

        return;
    }
    
    int pos = strchr(cmd, ' ');
    
    if (pos != -1)
        cmd[pos] = 0;
    
    print_str("Invalid command ");
    print_str(cmd);
    print_char('\n');
}
