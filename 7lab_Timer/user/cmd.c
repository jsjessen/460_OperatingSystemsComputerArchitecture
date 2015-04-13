#include "cmd.h"
#include "syscall.h"

int color;

typedef struct command
{
    char* name;
    int (*cmd)();
} COMMAND;

static const COMMAND cmd_lookup[] =
{
    { "?"        , &show_menu },
    { "help"     , &show_menu },
    { "menu"     , &show_menu },
                               
    { "ps"       , &ps        }, 
    { "chname"   , &chname    },     
    { "kmode"    , &kmode     },    
    { "switch"   , &tswitch   },     
    { "wait"     , &wait      },   
                            
    { "fork"     , &fork      },   
    { "exec"     , &exec      },   

    { "pipe"     , &pipe      },  
    { "pfd"      , &pfd       }, 
    { "close"    , &close     },   
    { "read"     , &read      },  
    { "write"    , &write     },   
    { "test_pipe", &test_pipe },       

    { "exit"     , &kill      }
};

int show_menu()
{
    printf("********************** Menu ************************\n");
    printf("* ps  chname  kmode switch  wait  exit  fork  exec *\n");
    printf("*  pipe   pfd    close   read   write   test_pipe  *\n");
    printf("****************************************************\n");

    return SUCCESS;
}

int (*find_cmd(char *name))()
{
    static int cmd_count = sizeof(cmd_lookup) / sizeof(cmd_lookup[0]);
    int i;

    for(i = 0; i < cmd_count; i++)
    {
        if(strcmp(name, cmd_lookup[i].name) == 0)
            return cmd_lookup[i].cmd;
    }

    return &invalid;
}

int invalid(char *name)
{
    printf("Command '%s' does not exist\n", name);
    show_menu();

    return SUCCESS;
}
