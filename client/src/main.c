#include <stdio.h>
#include <string.h>

#include "args.h"
#include "log.h"
#include "reset.h"
#include "usb.h"

static struct cmd *commands[] =
{
    &reset_cmd,
    NULL
};

int main(int argc, char **argv)
{
    log_start(stderr, DEBUG);

    if (argc < 2)
        goto usage;

    int i;

    // Figure out which command to execute
    for (i = 0; commands[i] != NULL; i++)
    {
        if (strcmp(argv[1], commands[i]->cmd) == 0)
            break;
    }

    // Unknown command
    if (commands[i] == NULL)
        goto usage;

    int rc = usb_init();
    if (rc)
        goto exit;

    rc = commands[i]->run();

    usb_deinit();

exit:
    return rc;

usage:
    printf("Usage: %s <command> [arguments]\n"
            "Available commands:\n",
            argv[0]);
    for (i = 0; commands[i] != NULL; i++)
        printf("    %-20s - %s\n", commands[i]->cmd, commands[i]->desc);
    return 0;
}
