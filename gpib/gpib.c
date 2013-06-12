#include <stdio.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include <string.h>  
#include <stdlib.h>
  
int main(int argc, char *argv[])  
{ 
    // variables 
    int count;  	// count number of bytes read
    char buffer[2048]; 	// buffer to receive data 
    int inst_id; 	// instrument file descriptor 

    // check the number of parameters  
    if(argc < 3) {
	printf("usage: %s <device> <cmd>\n", argv[0]); 
        return -1;  
    }
 
    // open and write command to the instrument 
    inst_id = open(argv[1], O_RDWR);  
    write(inst_id, argv[2], strlen(argv[2]));  
   
    // check if it will receive some data and read
    if(strchr(argv[2], '?') != NULL )  
    { 
	usleep(5000);       // if absent, next read fails 
        count = read(inst_id, buffer, sizeof(buffer));  
        buffer[count+1] = 0;  
        printf("%s", buffer);  
    }  

    // cleanup and exit  
    close(inst_id);
    exit(EXIT_SUCCESS);  
}  
