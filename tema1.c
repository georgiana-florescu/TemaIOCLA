#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE_SIZE 300

struct Dir;
struct File;

typedef struct Dir {
    char *name;
    struct Dir *parent;
    struct File *head_children_files;
    struct Dir *head_children_dirs;
    struct Dir *next;
} Dir;

typedef struct File {
    char *name;
    struct Dir *parent;
    struct File *next;
} File;

//verifica daca exista un fisier cu numele name in directorul parinte
int findFile(Dir *parent, char *name, File **prev) {
    File *tmp = parent->head_children_files;
    *prev = NULL;
    while (tmp != NULL) {
        if (!strcmp(tmp->name, name)) {
            return 1;
        }
        *prev = tmp;
        tmp = tmp->next;
    }
    return 0;
}

//verifica daca exista un director cu numele name in directorul parinte
int findDir(Dir *parent, char *name, Dir **prev) {
    Dir *tmp = parent->head_children_dirs;
    *prev = NULL;
    while (tmp != NULL) {
        if (!strcmp(tmp->name, name)) {
            return 1;
        }
        *prev = tmp;
        tmp = tmp->next;
    }
    return 0;
}

//elibereaza memoria ocupata de fisierul f
void freeFile(File *f) {
    free(f->name);
    free(f);
}

//elibereaza memoria ocupata de directorul d recursiv
void freeDir(Dir *d) {
    File *prev_file, *tmp_file = d->head_children_files;
    Dir *prev_dir, *tmp_dir = d->head_children_dirs;
    while (tmp_file != NULL) {
        prev_file = tmp_file;
        tmp_file = tmp_file->next;
        freeFile(prev_file);
    }
    while (tmp_dir != NULL) {
        prev_dir = tmp_dir;
        tmp_dir = tmp_dir->next;
        freeDir(prev_dir);
    }
    free(d->name);
    free(d);
}

//creeaza un fisier
void touch(Dir *parent, char *name) {
    File *prev;
    if (findFile(parent, name, &prev) == 1) {
        printf("File already exists\n");
        return;
    }
    File *new_file = (File *) malloc(sizeof(File));
    new_file->name = (char *) malloc(strlen(name) + 1);
    strcpy(new_file->name, name);
    new_file->parent = parent;
    new_file->next = NULL;
    if (parent->head_children_files == NULL) {
        parent->head_children_files = new_file;
    } else {
        prev->next = new_file;
    }
}

//creeaza un nou director
void mkdir(Dir *parent, char *name) {
    File *prev_file;
    Dir *prev_dir;
    //verifica daca numele name este folosit
    if (findFile(parent, name, &prev_file) == 1) {
        printf("Directory already exists\n");
        return;
    }
    if (findDir(parent, name, &prev_dir) == 1) {
        printf("Directory already exists\n");
        return;
    }
    Dir *new_dir = (Dir *) malloc(sizeof(Dir));
    new_dir->name = (char *) malloc(strlen(name) + 1);
    strcpy(new_dir->name, name);
    new_dir->parent = parent;
    new_dir->head_children_files = NULL;
    new_dir->head_children_dirs = NULL;
    new_dir->next = NULL;
    if (parent->head_children_dirs == NULL) {
        parent->head_children_dirs = new_dir;
    } else {
        prev_dir->next = new_dir;
    }
}

//afiseaza continutul directorului parent
void ls(Dir *parent) {
    File *tmp_file = parent->head_children_files;
    Dir *tmp_dir = parent->head_children_dirs;
    while (tmp_dir != NULL) {
        puts(tmp_dir->name);
        tmp_dir = tmp_dir->next;
    }
    while (tmp_file != NULL) {
        puts(tmp_file->name);
        tmp_file = tmp_file->next;
    }
}

//sterge fisierul cu numele name
void rm(Dir *parent, char *name) {
    File *prev, *rm_file;
    if (findFile(parent, name, &prev) == 0) {
        printf("Could not find the file\n");
        return;
    }
    if (prev == NULL) {
        rm_file = parent->head_children_files;
        parent->head_children_files = rm_file->next;
    } else {
        rm_file = prev->next;
        prev->next = prev->next->next;
    }
    freeFile(rm_file);
}

//sterge directorul cu numele name
void rmdir(Dir *parent, char *name) {
    Dir *prev, *rm_dir;
    if (findDir(parent, name, &prev) == 0) {
        printf("Could not find the dir\n");
        return;
    }
    if (prev == NULL) {
        rm_dir = parent->head_children_dirs;
        parent->head_children_dirs = parent->head_children_dirs->next;
    } else {
        rm_dir = prev->next;
        prev->next = prev->next->next;
    }
    freeDir(rm_dir);
}

//schimba directorul curent
void cd(Dir **target, char *name) {
    Dir *d = *target, *tmp;
    if (!strcmp(name, "..")) {
        if (d->parent != NULL) {
            *target = d->parent;
        }
        return;
    }
    tmp = d->head_children_dirs;
    while (tmp != NULL) {
        if (!strcmp(tmp->name, name)) {
            *target = tmp;
            return;
        }
        tmp = tmp->next;
    }
    printf("No directories found!\n");
}

//returneaza un string care contine calea catre directorul curent
char *pwd(Dir *target) {
    int path_len = 1;
    Dir *root = target;
    char *path, *aux;
    while (root != NULL) {
        path_len += strlen(root->name) + 1;
        root = root->parent;
    }
    path = (char *) malloc(path_len);
    aux = (char *) malloc(path_len);
    root = target;
    while (root != NULL) {
        strcpy(aux, path);
        strcpy(path, "/");
        strcat(path, root->name);
        strcat(path, aux);
        root = root->parent;
    }
    free(aux);
    return path;
}

//afiseaza interiorul directorului, forma arborescenta
void tree(Dir *target, int level) {
    File *tmp_f = target->head_children_files;
    Dir *tmp_d = target->head_children_dirs;
    int i;
    while (tmp_d != NULL) {
        for (i = 0; i < level; i++) {
            printf("    ");
        }
        printf("%s\n", tmp_d->name);
        if (tmp_d->head_children_files || tmp_d->head_children_dirs) {
            tree(tmp_d, level + 1);
        }
        tmp_d = tmp_d->next;
    }
    while (tmp_f != NULL) {
        for (i = 0; i < level; i++) {
            printf("    ");
        }
        printf("%s\n", tmp_f->name);
        tmp_f = tmp_f->next;
    }
}

//redenumeste fisier/director
void mv(Dir *parent, char *oldname, char *newname) {
    //2 conditii: oldname exista si newname nu exista
    //verifica daca oldname exista
    int is_dir, is_file, is_newname;
    Dir *pd, *d, *nd;
    File *pf, *f, *nf;
    is_dir = findDir(parent, oldname, &pd);
    is_file = findFile(parent, oldname, &pf);
    if (is_dir == is_file) {
        printf("File/Director not found\n");
        return;
    }
    //verifica daca newname exista
    is_newname = findFile(parent, newname, &nf) | findDir(parent, newname, &nd);
    if (is_newname) {
        printf("File/Director already exists\n");
        return;
    }
    //directorul este mutat pe ultima pozitie sub un nou nume
    if (is_dir) {
        if (pd == NULL) {
            d = parent->head_children_dirs;
            parent->head_children_dirs = d->next;
        } else {
            d = pd->next;
            pd->next = d->next;
        }
        if (parent->head_children_dirs == NULL) {
            parent->head_children_dirs = d;
        } else {
            nd = parent->head_children_dirs;
            while(nd->next != NULL) {
                nd = nd->next;
            }
            nd->next = d;
        }
        free(d->name);
        d->name = strdup(newname);
        d->next = NULL;
    } else {
        if (pf == NULL) {
            f = parent->head_children_files;
            parent->head_children_files = f->next;
        } else {
            f = pf->next;
            pf->next = f->next;
        }
        if (parent->head_children_files == NULL) {
            parent->head_children_files = f;
        } else {
            nf = parent->head_children_files;
            while(nf->next != NULL) {
                nf = nf->next;
            }
            nf->next = f;
        }
        free(f->name);
        f->name = strdup(newname);
        f->next = NULL;
    }
}

// opreste executia aplicatiei
// sterge si elibereaza memoria directorului radacina
void stop(Dir *target) {
    freeDir(target);
}

int main() {
    int continue_execution = 1;
    Dir *current_dir, *root;
    root = (Dir *) malloc(sizeof(Dir));
    root->name = (char *) malloc(5);
    strcpy(root->name, "home");
    root->parent = NULL;
    root->head_children_dirs = NULL;
    root->head_children_files = NULL;
    root->next = NULL;
    current_dir = root;
    do {
        char *comanda, *arg1, *arg2, *str, *aux;
        str = (char *) malloc(MAX_INPUT_LINE_SIZE);
        fgets(str, MAX_INPUT_LINE_SIZE, stdin);
        str[strlen(str) - 1] = 0;
        comanda = strtok(str, " ");
        arg1 = strtok(NULL, " ");
        if (!strcmp(comanda, "ls")) {
            ls(current_dir);
        } else if (!strcmp(comanda, "touch")) {
            touch(current_dir, arg1);
        } else if (!strcmp(comanda, "mkdir")) {
            mkdir(current_dir, arg1);
        } else if (!strcmp(comanda, "rmdir")) {
            rmdir(current_dir, arg1);
        } else if (!strcmp(comanda, "rm")) {
            rm(current_dir, arg1);
        } else if (!strcmp(comanda, "cd")) {
            cd(&current_dir, arg1);
        } else if (!strcmp(comanda, "tree")) {
            tree(current_dir, 0);
        } else if (!strcmp(comanda, "pwd")) {
            aux = pwd(current_dir);
            puts(aux);
            free(aux);
        } else if (!strcmp(comanda, "mv")) {
            arg2 = strtok(NULL, " ");
            mv(current_dir, arg1, arg2);
        } else if (!strcmp(comanda, "stop")) {
            continue_execution = 0;
            stop(root);
        }
        free(str);
    } while(continue_execution);
    return 0;
}