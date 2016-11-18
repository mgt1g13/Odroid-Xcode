#include "MultiWiiProtocol.h"
#include "MultiWiiNetwork.h"
//#include <windows.h>
#include <stdio.h>
#define FRAME_SIZE(x) (x+6)

#include <errno.h>
#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>

using namespace std;

#define BAUDRATE B115200 		//B230400
//#define PERIOD 20000 // 20ms --> 50Hz
#define DEVICE "/dev/tty.SLAB_USBtoUART"
//#define DEVICE "/dev/ttyUSB0"
#define FALSE 0
#define TRUE 1


// FILE DESCRIPTOR DA SERIAL
int fd;
uint16_t rc_signals[8] = { 1234 };
uint8_t rc_bytes[16] = { 0 };



void initPort()
{
    
    fd = open(DEVICE, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
        /*
         * Could not open the port.
         */
        
        perror("open_port: Unable to open /dev/ttyf1 - ");
        exit(1);
    }
    else
        fcntl(fd, F_SETFL, 0);
    
    
    struct termios options;
    
    /*
     * Get the current options for the port...
     */
    
    //tcgetattr(fd, &options);
    bzero(&options, sizeof(options));
    
    /*
     * Set the baud rates to 1500...
     */
    
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    
    /*
     * Enable the receiver and set local mode...
     */
    
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CSIZE; /* Mask the character size bits */
    options.c_cflag |= CS8;    /* Select 8 data bits */
    
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    
    
    options.c_cc[VTIME] = 1;  // timeout after .1s that isn't working
    options.c_cc[VMIN] = 0;
    
    
    /*
     * Set the new options for the port...
     */
    
    tcsetattr(fd, TCSANOW, &options);
    
}


//Initialise the connection
//Inicia a conexao
int multiwii_init_serial_port(void)
{
    
    initPort();
    return 1;
}

//Send the given data frame using the serial connection.
//Envia o quadro passado por parametro por porta serial.
int multiwii_send_data(unsigned char* data_frame, unsigned char data_size)
{
    
    int res = (int)write(fd, data_frame, data_size);
    //tcdrain(fd);
    if(res < 0)
    {
        return 0;
    }
    
    return 1;
}

//Receives data from the serial port. Expected size is the maximum number of bytes it is supposed to read
//Recebe dados pela porta serial. Expected_size eh o numero maximo de bytes que devem ser lidos
received_frame_t multiwii_get_data(int expected_size)
{
    
    //sleep(1);
    received_frame_t received_frame;
    received_frame.frame_size = read(fd, received_frame.received_frame, expected_size);
    if( (int) received_frame.frame_size < expected_size )
    {
        printf("Error on read! %s - %d %d\n", strerror(errno), (int)received_frame.frame_size, (int)expected_size);
        tcflush(fd, TCIFLUSH);
//        exit(1);
    }
    
    return received_frame;
    
}

//Close the connection
//Fecha a conexao
void multiwii_close_serial_port()
{
    close(fd);
    
}