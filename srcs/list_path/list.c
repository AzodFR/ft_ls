#include "ft_ls.h"

int		ft_digitlen(int n)
{
	unsigned int	nb;
	int				i;

	i = 1;
	nb = n;
	if (n < 0)
	{
		i = 2;
		nb = -n;
	}
	while (nb > 9)
	{
		nb /= 10;
		i++;
	}
	return (i);
}

t_path *path_new(char *path)
{
    t_path *new;

    new = malloc(sizeof(t_path));

    new->name = path;
    new->max_len_size = 0;
    new->max_gid_size = 0;
    new->max_link_size = 0;
    new->max_uid_size = 0;
    new->not_found = 0;
    new->next = NULL;
    new->files = NULL;
    new->childs = NULL;
    new->nb_blocks = 0;
    new->color = 94;

    return new;
}

void get_rights(char *s, mode_t stat)
{
    if (stat & S_IRUSR)
        s[0] = 'r';
    if (stat & S_IWUSR)
        s[1] = 'w';
    if (stat & S_ISUID)
        s[2] = 's';
    else if (stat & S_IXUSR)
        s[2] = 'x';
    if (stat & S_IRGRP)
        s[3] = 'r';
    if (stat & S_IWGRP)
        s[4] = 'w';
    if (stat & S_ISGID)
        s[5] = 's';
    else if (stat & S_IXGRP)
        s[5] = 'x';
    if (stat & S_IROTH)
        s[6] = 'r';
    if (stat & S_IWOTH)
        s[7] = 'w';
    if (stat & S_IXOTH)
        s[8] = 'x';

}

char *get_color(mode_t mode)
{
    if (S_ISLNK(mode))
        return "\e[96m";
    else if (S_ISDIR(mode))
        return "\e[94m";
    else if (mode & S_IXUSR)
        return "\e[92m";
    return "\e[0m";

}

t_file *file_new(char *path, char *name)
{
    t_file *new;

    new = malloc(sizeof(t_file));

    new->name = name;
    new->path = path;
    new->linked = NULL;
    new->next = NULL;
    lstat(path, &(new->stats));
    memset(new->rigths, 0, 11);
    memset(new->rigths, '-', 10);
    new->color = 0;
    

    if (S_ISLNK(new->stats.st_mode))
    {
        //lstat(path, &(new->stats));
        char buf[2048];
        memset(buf, 0, 2048);
        readlink(new->name, buf, 2048);
        struct stat bp;
        stat(buf, &bp);

        new->linked = malloc(strlen(buf) + 10);
        memset(new->linked, 0, strlen(buf) + 10);
        strcpy(new->linked, " -> ");
        strcat(new->linked, get_color(bp.st_mode));
        strcat(new->linked, buf);
        new->color = 96;
        new->rigths[0] = 'l';
    }
    else if (S_ISDIR(new->stats.st_mode))
    {
        new->color = 94;
        new->rigths[0] = 'd';
    }
    else if (new->stats.st_mode & S_IXUSR)
        new->color = 92;
    else if (S_ISREG(new->stats.st_mode))
        new->color = 0;

    get_rights(&new->rigths[1], new->stats.st_mode);
    //printf("%s: stats mode == %s\n\n", name, rigth);
    return new;
}

char *tolower_s(char *s)
{
    char *r = malloc (strlen(s) + 1);
    
    int i = -1;
    while (s[++i])
    {
        if (isupper(s[i]))
            r[i] = tolower(s[i]);
        else
            r[i] = s[i];
    }
    r[i] = 0;
    return r;
}

void file_add(t_path **begin, char *path, char *name)
{
   // dprintf(1,"\n\n%s\n", name);
    if (!(*begin)->files)
    {
        (*begin)->files = file_new(path, name);
        if (!flags(F_a) && name[0] == '.')
            return;
        (*begin)->nb_blocks +=  (*begin)->files->stats.st_blocks/2;
        int size_len = ft_digitlen((*begin)->files->stats.st_size);
        if ((*begin)->max_len_size < size_len)
            (*begin)->max_len_size = size_len;
        int size_link = ft_digitlen((*begin)->files->stats.st_nlink);
        if ((*begin)->max_link_size < size_link)
            (*begin)->max_link_size = size_link;
        int size_uid = strlen(getpwuid((*begin)->files->stats.st_uid)->pw_name);
        if ((*begin)->max_uid_size < size_uid)
            (*begin)->max_uid_size = size_uid;
        int size_gid = strlen(getgrgid((*begin)->files->stats.st_gid)->gr_name);
        if ((*begin)->max_gid_size < size_gid)
            (*begin)->max_gid_size = size_gid;
    }
    else
    {
        t_file *new = file_new(path, name);

        t_file *tmp = (*begin)->files;
        if (!strcmp(name, "."))
        {
            new->next = tmp;
            (*begin)->files = new;
        }
        else if (!strcmp(name, ".."))
        {
            if(!strcmp(tmp->name, "."))
            {
                new->next = tmp->next;
                tmp->next = new;
            }
            else
            {
                 new->next = tmp;
                (*begin)->files = new;
            }
        }
        else
        {
            char *name_l;
            int i = 0;
            while (name[i] == '.')
                i++;
            name_l = &name[i];
            t_file *prev;
            char *a = tolower_s(tmp->name);
            char *b = tolower_s(name_l);

            while (tmp && strcmp(a, b) < 0)
            {
                free(a);
                a = NULL;
                prev = tmp;
                tmp = tmp->next;
                if (tmp)
                    a = tolower_s(tmp->name);
            }
            if (a)
                free(a);
            free(b);
            if (!prev)
            {
                new->next = (*begin)->files;
                (*begin)->files = new;
            }
            else
            {
                new->next = prev->next;
                prev->next = new;
            }
        }


        if (!flags(F_a) && name[0] == '.')
            return;
        (*begin)->nb_blocks +=  new->stats.st_blocks/2;
        int size_len = ft_digitlen(new->stats.st_size);
        if ((*begin)->max_len_size < size_len)
            (*begin)->max_len_size = size_len;
        int size_link = ft_digitlen(new->stats.st_nlink);
        if ((*begin)->max_link_size < size_link)
            (*begin)->max_link_size = size_link;
        int size_uid = strlen(getpwuid(new->stats.st_uid)->pw_name);
        if ((*begin)->max_uid_size < size_uid)
            (*begin)->max_uid_size = size_uid;
        int size_gid = strlen(getgrgid(new->stats.st_gid)->gr_name);
        if ((*begin)->max_gid_size < size_gid)
            (*begin)->max_gid_size = size_gid;
    }
}

void path_add(t_path **begin, char *path)
{
    if (!(*begin))
    {
        *begin = path_new(path);
    }
    else
    {
        t_path *tmp = *begin;
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = path_new(path);
    }
}

void childs_add(t_path **begin, t_path *child)
{
    if (!(*begin)->childs)
        (*begin)->childs = child;
    else
    {
        t_path *tmp = (*begin)->childs;
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = child;
    }
}

int path_size(t_path *begin)
{
    t_path *tmp = begin;
    int i = 0;
    while (tmp)
    {
        i++;
        tmp = tmp->next;
    }
    return i;
}

int is_sort(t_file *head)
{
    t_file *tmp = head;
    while(tmp && tmp->next && strcmp(tmp->name , tmp->next->name) < 0)
        tmp = tmp->next;
    if (tmp->next)
        return 0;
    return 1;
}