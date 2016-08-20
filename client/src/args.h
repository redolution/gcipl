#ifndef INC_ARGS_H
#define INC_ARGS_H

typedef int (*arghook_t)(char c);

struct cmd
{
    char *cmd;
    char *desc;
    struct arg *args;
    arghook_t arghook;
    int (*run)(void);
};

struct arg
{
    char c;
    char *param;
    char *desc;
};

extern struct arg args_global[];

int args_parse(int argc, char **argv, arghook_t hook);
void args_print(struct arg *arglist);

#endif
