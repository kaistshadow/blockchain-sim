// source https://gist.github.com/suyash/a19b7f91000b24fde4bc4a015680c611
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main () {
    const char* filename = "CMakeLists.txt";
    int offset = 10;

    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("cannot open %s\n", filename);
        return -1;
    }

    struct stat sbuf;
    if (stat(filename, &sbuf) == -1) {
        printf("cannot stat %s\n", filename);
        return -1;
    }
    int size = sbuf.st_size;

    char* data = (char *)mmap((caddr_t)0, size, PROT_READ, MAP_SHARED, fd, 0);
    if ( data == -1) {
        printf("cannot mmap %s\n", filename);
        return -1;
    }

    printf("byte at offset %d is %c\n", offset, data[offset - 1]);

    int err = munmap(data, size);
    if (err == -1) {
        printf("cannot munmap %s\n", filename);
        return -1;
    }
    printf("test succeded");
    return 0;
}