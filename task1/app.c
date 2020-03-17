#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    char buf[100];
    int fd = open("/proc/csumdev", O_RDWR);

    write(fd, "abcddfdfd", 5);

    lseek(fd, 0, SEEK_SET);
    read(fd, buf, 100);
    puts(buf);

    lseek(fd, 0, SEEK_SET);
    write(fd, "abc100", 5);

    lseek(fd, 0, SEEK_SET);
    read(fd, buf, 100);
    puts(buf);
    return 0;
}
