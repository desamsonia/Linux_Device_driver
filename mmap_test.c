#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_PATH "/dev/mmap_driver"
#define MAP_SIZE (4 *4096)
#define PATTERN 0xAA

int main()
{
        int fd = open(DEVICE_PATH, O_RDWR);
        if(fd == -1)
        {
                perror("Failed to open device");
                return 1;
        }

        void *map = mmap(NULL, MAP_SIZE,PROT_READ |PROT_WRITE,MAP_SHARED,fd,0);

        if(map == MAP_FAILED)
        {
                perror("failed to mmap");
                close(fd);
                return 1;
        }

        for(size_t i = 0; i<MAP_SIZE;i++)
        {
                if(((unsigned char *)map)[i] != PATTERN)
                {
                        fprintf(stderr, "Pattern mismatch at offset %zu: expected 0x%X, got 0x%X\n",i,PATTERN, ((unsigned char*)map)[i]);
                        munmap(map,MAP_SIZE);
                        close(fd);
                        return 1;
                }
        }

        printf("Memory mapping verification successful!\n");
        snprintf((char *)map, MAP_SIZE,"Hello from user space!\n");
        printf("Mapped memory content: %s\n",(char *)map);

        munmap(map,MAP_SIZE);
        close(fd);
        return 0;
}
