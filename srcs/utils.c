#include "utils.h"

char *ft_getcwd(char **env)
{
    int i = -1;
    static char *path = {"PWD"};

    while (env[++i])
    {
        int j = -1;
        while (env[i][++j] == path[j])
        {
            if (!path[j+1])
                return &env[i][j+2];
        }

    }
    return NULL;
}


void error_arg(char *reason, char *arg)
{
    printf("%s: \e[91m%s \e[0m[\e[91m%s\e[0m]\n", EXEC_NAME, reason, arg);
    exit(1);
}

void debug(t_params *param)
{
    printf("l: %d\nR: %d\na: %d\nr: %d\nt: %d\nd: %d\n", param->flags[0],param->flags[1], param->flags[2], param->flags[3], param->flags[4], param->flags[5]);
    t_path *tmp = param->path;

    while (tmp)
    {
        printf("\n%s\n", tmp->name);
        tmp = tmp->next;
    }
}

int *init_flags(int *f)
{
    static int *flags = {NULL};

    if (!flags && f)
        flags = f;
    return flags;
}

int flags(int i)
{
    return init_flags(NULL)[i] == 1;
}