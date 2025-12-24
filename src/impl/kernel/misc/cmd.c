#include "cmd.h"
#include "print.h"
#include "slab.h"
#include "mklib.h"


struct cmd_t* cmd_list_head = 0;

void mk_register_cmd(char *name, char* desc, void *entry, int list) {
    struct cmd_t* obj = mkmalloc(sizeof(struct cmd_t));
    
    obj->cmd = name;
    obj->desc = desc;
    obj->entry = entry;
    obj->next = cmd_list_head;
    obj->list = list;

    cmd_list_head = obj;
}

void cmd_help() {
    struct cmd_t* cur = cmd_list_head;
    
    print_str("--- list of available cmds ---\n");
    while (cur != 0) {
        if (!cur->list) {

            //cur = cur->next;
            continue;
	}

        print_str(cur->cmd);
        print_str(" - ");
        print_str(cur->desc);
        print_char('\n');
        
        cur = cur->next;
    }
}

void mk_handle_cmd(char *cmd) {
    struct cmd_t* cur = cmd_list_head;

    if (*cmd == 0) 
        return;
    
    while (cur != 0) {
        if (_strcmp(cur->cmd, cmd) != 0) {
            cur = cur->next;
            continue;
        }
        
        ((func) cur->entry)(cmd + _strlen(cur->cmd) + 1);

        return;
    }
    
    
    int pos = _strchr(cmd, ' ');
    
    if (pos != -1)
        cmd[pos] = 0;
    
    print_str("Invalid command ");
    print_str(cmd);
    print_char('\n');
}
