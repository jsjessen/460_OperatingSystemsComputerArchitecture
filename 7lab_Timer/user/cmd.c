#include "cmd.h"

int show_menu()
{
    printf("********************** Menu ************************\n");
    printf("* ps  chname  kmode switch  wait  exit  fork  exec *\n");
    printf("*  pipe   pfd    close   read   write   test_pipe  *\n");
    printf("****************************************************\n");

    return SUCCESS;
}

typedef struct command
{
    char* name;
    int (*cmd)(void);
} COMMAND;

int (*find_cmd(char *name))()
{
    int i;
    int cmd_count;


    COMMAND cmd_lookup[2];

    cmd_lookup[0].name = "test";
    cmd_lookup[0].cmd = &show_menu;

    cmd_lookup[1].name = "test2";
    cmd_lookup[1].cmd = &show_menu;

    // {
    //     { "?"        , &show_menu },
    //     { "help"     , &show_menu },
    //     { "menu"     , &show_menu },

    //     { "ps"       , &ps        }, 
    //     { "chname"   , &chname    },     
    //     { "kmode"    , &kmode     },    
    //     { "switch"   , &tswitch   },     
    //     { "wait"     , &wait      },   

    //     { "fork"     , &fork      },   
    //     { "exec"     , &exec      },   

    //     { "pipe"     , &pipe      },  
    //     { "pfd"      , &pfd       }, 
    //     { "close"    , &close     },   
    //     { "read"     , &read      },  
    //     { "write"    , &write     },   
    //     { "test_pipe", &test_pipe },       

    //     { "exit"     , &kill      }
    // };

    //int cmd_count = sizeof(cmd_lookup) / sizeof(cmd_lookup[0]);
    cmd_count = 17;

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
