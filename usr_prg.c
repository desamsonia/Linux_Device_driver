// reading and writing from character driver, compile this code after sample_character_driver.c

#include<stdio.h>
#include<fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_PATH "/dev/sample_driver"
#define BUFFER_SIZE 100

int main()
{
        int fd;
        char write_buffer[] = "Hello, kernel baby!";
        char read_buffer[BUFFER_SIZE];
        ssize_t bytes_read, bytes_written;

        //open the device
        fd = open(DEVICE_PATH, O_RDWR);

        if(fd == -1)
        {
                perror("Failed to open device");
                return 1;
        }

        printf("open success\n");

        // write to the device
        bytes_written = write(fd,write_buffer,strlen(write_buffer));

        if(bytes_written == -1)
        {
                perror("Failed to write to device");
                close(fd);
                return 1;
        }

        bytes_read = read(fd,read_buffer,BUFFER_SIZE-1);
        if(bytes_read == -1)
        {
                perror("Failed to read from device");
                close(fd);
                return 1;
        }

        read_buffer[bytes_read] = '\0';

        printf("read from device: %s\n",read_buffer);

        //close the device
         close(fd);
         return 0;
}
