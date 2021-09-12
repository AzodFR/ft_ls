#ifndef UTILS_H
# define UTILS_H

# define EXEC_NAME "ft_ls"
# define ERR_ARG "Unknown argument"

# include <dirent.h>
# include <stdio.h>
# include <stdlib.h>
# include <dirent.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <ctype.h>



typedef struct s_file {
    char *name;
    char *path;
    char *linked;
    char rigths[11];
    struct stat stats;
    int color;
    struct s_file *next;
}               t_file;

typedef struct s_path {
    char *name;
    int not_found;
    int nb_blocks;
    int max_len_size;
    int max_link_size;
    int max_uid_size;
    int max_gid_size;
    int color;
    struct stat stats;
    struct s_file *files;
    struct s_path *next;
    struct s_path *childs;

}             t_path;

typedef struct s_params {
    int flags[6];
    t_path  *path;
}               t_params;

char *ft_getcwd(char **env);
void error_arg(char *reason, char *arg);
void debug(t_params *param);
int *init_flags(int *f);
int flags(int i);

#endif