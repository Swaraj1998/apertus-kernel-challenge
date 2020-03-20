#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

const char *data = "aaa";

int main()
{
    char buf[100];
    int fd;

    puts("Test data: ");
    puts(data);

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

    return 0;
}
