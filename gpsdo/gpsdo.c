#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/* baudrate settings are defined in <asm/termbits.h>, which is included by <termios.h> */
#define BAUDRATE B4800            
/* change this definition for the correct port */
#define MODEMDEVICE "/dev/ttyUSB0"
#define _POSIX_SOURCE 1 
/* POSIX compliant source */

#define FALSE 0
#define TRUE 1

#define MAX_STRING_LENGTH 255
#define MAX_TOKS 100
#define DELIMITERS "| \t"

volatile int STOP=FALSE; 

int parseString(char* line, char*** argv) {

  char* buffer;
  int argc;

  buffer = (char*) malloc(strlen(line) * sizeof(char));
  strcpy(buffer,line);
  (*argv) = (char**) malloc(MAX_TOKS * sizeof(char**));

  argc = 0;  
  (*argv)[argc++] = strtok(buffer, DELIMITERS);
  while ((((*argv)[argc] = strtok(NULL, DELIMITERS)) != NULL) &&
	 (argc < MAX_TOKS)) ++argc;

  return argc;
}


int main()
{
  int fd,res;
  struct termios oldtio,newtio;
  char buf[255];

  char **A;
  int n;

/* 
  Open modem device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
*/
 fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY ); 
 if (fd <0) {perror(MODEMDEVICE); exit(-1); }

 tcgetattr(fd,&oldtio); /* save current serial port settings */
 bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */

/* 
  BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
  CRTSCTS : output hardware flow control (only used if the cable has
            all necessary lines. See sect. 7 of Serial-HOWTO)
  CS8     : 8n1 (8bit,no parity,1 stopbit)
  CLOCAL  : local connection, no modem contol
  CREAD   : enable receiving characters
*/
 newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
 
/*
  IGNPAR  : ignore bytes with parity errors
  ICRNL   : map CR to NL (otherwise a CR input on the other computer
            will not terminate input)
  otherwise make device raw (no other input processing)
*/
 newtio.c_iflag = IGNPAR | ICRNL;
 
/*
 Raw output.
*/
 newtio.c_oflag = 0;
 
/*
  ICANON  : enable canonical input
  disable all echo functionality, and don't send signals to calling program
*/
 newtio.c_lflag = ICANON;
 
/* 
  initialize all control characters 
  default values can be found in /usr/include/termios.h, and are given
  in the comments, but we don't need them here
*/
 newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
 newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
 newtio.c_cc[VERASE]   = 0;     /* del */
 newtio.c_cc[VKILL]    = 0;     /* @ */
 newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
 newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
 newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
 newtio.c_cc[VSWTC]    = 0;     /* '\0' */
 newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
 newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
 newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
 newtio.c_cc[VEOL]     = 0;     /* '\0' */
 newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
 newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
 newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
 newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
 newtio.c_cc[VEOL2]    = 0;     /* '\0' */

/* 
  now clean the modem line and activate the settings for the port
*/
 tcflush(fd, TCIFLUSH);
 tcsetattr(fd,TCSANOW,&newtio);

/*
  terminal settings done, now handle input
  In this example, inputting a 'z' at the beginning of a line will 
  exit the program.
*/
 /* read blocks program execution until a line terminating character is 
    input, even if more than 255 chars are input. If the number
    of characters read is smaller than the number of chars available,
    subsequent reads will return the remaining chars. res will be set
    to the actual number of characters actually read */

    res = read(fd,buf,255); 
    buf[res]=0;             /* set end of string, so we can printf */
    /* printf(":%s:%d\n", buf, res); */

    /* separate the strings */
    n = parseString(buf, &A);

//  for (i = 0; i < n; ++i)
//     printf("A[%d] = %s\n",i,A[i]);

    if ( n > 0) {
    	/* Output to MRTG http://www.manpagez.com/man/1/mrtg-reference/ */
    	printf("%d\n", (int) strtol(A[2], NULL, 16));   /* DAC Value */
    	printf("12400\n" );   /* Ref Value */
    	printf("%d seconds\n", (int) strtol(A[8], NULL, 16) * 16);   /* Timestamp = uptime ; each sample is 16 seconds*/
    	printf("dac\n");
    }

    /* restore the old port settings */
    tcsetattr(fd,TCSANOW,&oldtio);

    exit(0);
}

