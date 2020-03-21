#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "csumdev.h"

const char *data = "a";

int main()
{
    char buf[100];
    int fd;
    struct csum_arg_t ret;

    puts("Test data: ");
    puts(data);
    puts("");

    // write test data
    fd = open("/dev/csumdev", O_RDWR);
    write(fd, data, strlen(data));
    close(fd);

    puts("device read:");
    fd = open("/dev/csumdev", O_RDWR);
    read(fd, buf, 100);
    puts(buf);
    close(fd);

    puts("procfs read:");
    fd = open("/proc/csumdev", O_RDWR);
    read(fd, buf, 100);
    puts(buf);
    close(fd);

    puts("ioctl read:");
    fd = open("/dev/csumdev", O_RDWR);
    if (ioctl(fd, CSUM_GET_STRING, &ret) < 0)
            printf("ioctl read error\n");
    else
            puts(ret.str);
    close(fd);

    return 0;
}
