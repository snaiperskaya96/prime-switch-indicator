#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    if (argc == 2)
    {
        if (strncmp(argv[1], "intel", 6) == 0
            || strncmp(argv[1], "nvidia", 7) == 0)
        {
            setuid(geteuid());
            char command[128] = {0x0};
            sprintf(command, "sudo prime-select %s", argv[1]);
            return system(command);
        }
    }

    return 1;
}