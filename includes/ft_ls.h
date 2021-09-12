#ifndef FT_LS
# define FT_LS

# include "utils.h"
# define F_l 0
# define F_R 1
# define F_a 2
# define F_r 3
# define F_t 4
# define F_s 5
# define F_debug 6

/*
** LIST
*/
t_path *path_new(char *);
void path_add(t_path **, char *);
int path_size(t_path *);
void childs_add(t_path **, t_path *);
t_file *file_new(char *, char *);
void file_add(t_path **, char *, char *);
void sort_file(t_file **);

/*
** OTHER
*/
int		ft_digitlen(int );
#endif