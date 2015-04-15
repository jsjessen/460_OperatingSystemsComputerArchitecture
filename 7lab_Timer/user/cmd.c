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

// For some reason compiler does like &foo
static const COMMAND cmd_lookup[] = 
{
    { "?"        , show_menu },
    { "help"     , show_menu },
    { "menu"     , show_menu },

    { "ps"       , ps        }, 
    { "chname"   , chname    },     
    { "kmode"    , kmode     },    
    { "switch"   , tswitch   },     
    { "wait"     , wait      },   

    { "fork"     , fork      },   
    { "exec"     , exec      },   

    { "pipe"     , pipe      },  
    { "pfd"      , pfd       }, 
    { "close"    , close     },   
    { "read"     , read      },  
    { "write"    , write     },   
    { "test_pipe", test_pipe },       

    { "exit"     , my_exit   }
};

int (*find_cmd(char* cmd_name))(void)
{
    int i;
    static int cmd_count = sizeof(cmd_lookup) / sizeof(cmd_lookup[0]);

    for(i = 0; i < cmd_count; i++)
    {
        if(strcmp(cmd_name, cmd_lookup[i].name) == 0)
            return cmd_lookup[i].cmd;
    }

    return invalid;
}

int invalid()
{
    printf("invalid command\n");
    show_menu();

    return SUCCESS;
}
