#include "ft_ls.h"
void print_one(t_params *param, t_path *path);

t_params *parse(char **av, char **env)
{
    int i = 0;
    t_params *p;

    p = malloc (sizeof(t_params));
    p->path = NULL;
    char flags[] = {'l', 'R', 'a', 'r', 't', 's', 'd', 0};

    while (av[++i])
    {
        if (av[i][0] != '-')
            path_add(&p->path, strdup(av[i]));
        else
        {
            int j;
            int l = 0;
            while (av[i][++l])
            {
                j = -1;
                while (flags[++j] && flags[j] != av[i][l])
                ;
            if (!flags[j])
            {
                free(p);
                error_arg(ERR_ARG, &av[i][l]);
            }
            else
                p->flags[j] = 1;
            }
        }
    }
    if (!p->path)
        path_add(&p->path, strdup(ft_getcwd(env)));
    return p;
}

void init_dir(t_path **begin)
{
    t_path *tmp = *begin;

    DIR *dir = opendir(tmp->name);
    
    if (!dir)
    {
        tmp->not_found = 1;
        return ;
    }

    struct dirent *tmpd;
    stat(tmp->name, &(tmp->stats));

    while (( tmpd = readdir(dir)))
    {
        char *new_p = malloc(strlen(tmp->name) + strlen(tmpd->d_name) + 2);
        new_p[0] = 0;
        strcat(new_p, tmp->name);
        new_p[strlen(tmp->name)] = '/';
        new_p[strlen(tmp->name) + 1] = 0;
        strcat(new_p, tmpd->d_name);
        if ( flags(F_R) && tmpd->d_name[0] != '.' && tmpd->d_type == DT_DIR)
        {
            t_path *new_dir = path_new(strdup(new_p));
            childs_add(begin, new_dir);
            init_dir(&new_dir);
        }
        file_add(begin, strdup(new_p), strdup(tmpd->d_name));
        free(new_p);
    }
    closedir(dir);
}

void init_all(t_params *param)
{
    t_path *tmp = param->path;
    while (tmp)
    {

        init_dir(&tmp);
        print_one(param, tmp);
        tmp = tmp->next;
    }
}

char *get_paddingsize(int size, int max)
{
    char  *pad = malloc(max - size + 2);
    memset(pad, 0, max - size + 2);
    int i = 0;
    while(size++ <= max)
        pad[i++] = ' ';
    return pad;
}

char **get_pad(t_file *file, t_path *path)
{
    char **pad = malloc(4 * sizeof(char *));
    pad[0] = get_paddingsize(ft_digitlen(file->stats.st_nlink), path->max_link_size);
    pad[3] = get_paddingsize(ft_digitlen(file->stats.st_size), path->max_len_size);
    pad[1] = get_paddingsize(strlen(getpwuid(file->stats.st_uid)->pw_name), path->max_uid_size);
    pad[2] = get_paddingsize(strlen(getgrgid(file->stats.st_gid)->gr_name), path->max_gid_size);
    return pad;
}
void print_one(t_params *param, t_path *path)
{
    t_path *tmp = path;
    if (path_size(param->path) > 1 || flags(F_R))
            printf("\n%s:\n", tmp->name);
    if (flags(F_l))
        printf("total %d\n", tmp->nb_blocks);
    t_file *files = tmp->files;

    while (files)
    {
        if (!flags(F_a) && files->name[0] == '.')
        {
            files = files->next;
            continue;
        }
        if (flags(F_l))
        {
            char **pad = get_pad(files, path);
            char *time_s = ctime(&(files->stats.st_mtime));
            time_s[strlen(time_s) - 1] = 0;
            if (flags(F_s))
                printf("%d %s %s%d %s%s %s%s %s%d %s \e[1;%dm %s\e[0m%s\e[0m\n" ,files->stats.st_blocks/2, files->rigths, pad[0], files->stats.st_nlink, pad[1], getpwuid(files->stats.st_uid)->pw_name , pad[2], getgrgid(files->stats.st_gid)->gr_name, pad[3],files->stats.st_size, time_s,files->color, files->name, files->linked ? files->linked : "");
            else
                printf("%s %s%d %s%s %s%s %s%d %s \e[1;%dm %s\e[0m%s\e[0m\n" ,files->rigths, pad[0], files->stats.st_nlink, pad[1], getpwuid(files->stats.st_uid)->pw_name , pad[2], getgrgid(files->stats.st_gid)->gr_name, pad[3],files->stats.st_size, time_s,files->color, files->name, files->linked ? files->linked : "");
            int i = -1;
            while (++i < 4)
                free(pad[i]);
            free(pad);
        }
        else
        {
            if (flags(F_s))
                printf("%d \e[1;%dm%s\e[0m    ",files->stats.st_blocks/2, files->color, files->name);
            else
                printf("\e[1;%dm%s\e[0m    ", files->color, files->name);
        }
        files = files->next;
    }
    if (!flags(F_R))
        return;
    t_path *child = tmp->childs;
    while (child)
    {
        printf("\n");
        print_one(param, child);
        child = child->next;
    }

}

void print_all(t_params *param)
{
t_path *tmp = param->path;
    while (tmp)
    {
        if (tmp->not_found)
            printf("%s: %s: No such file or directory\n", EXEC_NAME, tmp->name);
        else
         print_one(param, tmp);
        tmp = tmp->next;
    }
}


void free_file(t_file *file)
{
    if (!file)
        return;
    free_file(file->next);
    free(file->name);
    free(file->path);
    if (file->linked)
        free(file->linked);
    free(file);
}

void free_childs(t_path *path)
{
    if (!path)
        return;
    free_childs(path->next);
    free_file(path->files);
    free_childs(path->childs);
    free(path->name);
    free(path);
}

void free_all(t_path *parent)
{
    if (!parent)
        return;
    free_file(parent->files);
    free_childs(parent->childs);
    free_all(parent->next);
    free(parent->name);
    free(parent);
}

int main(int ac, char**av, char **env)
{
    t_params *param  = parse(av, env);

    init_flags(param->flags);

    if (flags(F_debug))
        debug(param);
   init_all(param);
  // print_all(param);
    free_all(param->path);
    free(param);

    return 0;
}