#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <inttypes.h>

#include "csumdev.h"

const char *data = "a";

int main()
{
    char buf[16];
    uint32_t val;
    int fd;

    printf("Test data: %s\n", data);

    // write test data
    fd = open("/dev/csumdev", O_RDWR);
    write(fd, data, strlen(data));
    close(fd);

    puts("\ndevice read:");
    fd = open("/dev/csumdev", O_RDWR);
    read(fd, buf, 16);
    printf("checksum: %s\n", buf);
    close(fd);

    puts("\nprocfs read:");
    fd = open("/proc/csumdev", O_RDWR);
    read(fd, buf, 16);
    printf("checksum: %s\n", buf);
    close(fd);

    puts("\nioctl read:");
    fd = open("/dev/csumdev", O_RDWR);
    if (ioctl(fd, CSUM_GET_VALUE, &val) < 0)
            printf("ioctl read error\n");
    else
            printf("checksum: %u\n", val);
    close(fd);

    return 0;
}
