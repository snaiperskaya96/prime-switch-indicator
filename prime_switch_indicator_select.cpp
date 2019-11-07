#include <cstdio>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    if (argc == 2)
    {
        setuid(geteuid());
        char command[128] = {0x0};
        sprintf(command, "sudo prime-select %s", argv[1]);
        return system(command);
    }

    return 1;
}