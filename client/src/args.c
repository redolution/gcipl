#include "args.h"

struct arg args_global[] =
{
    {
        .c = 'd',
        .param = "id",
        .desc = "Device to use"
    }
};

int args_parse(int argc, char **argv, arghook_t hook)
{
    // TODO

    return 0;
}

void args_print(struct arg *arglist)
{
    // TODO
}
