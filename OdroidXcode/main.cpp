//tutoriais comunicacao porta serial
//http://stackoverflow.com/questions/6068655/what-are-the-functions-i-need-use-for-serial-communication-in-vc


#include "MultiWiiManipulation.h"
#include <stdio.h>

#define MAX_THROTTLE 2000//1850
#define MAX 2000
#define MIN 1000
#define NOMINAL 1500

#define IMU_MODE 0
#define RC_MODE 1

#define TIME_SOBE_LENTAMENTE 5


#include <termios.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>		// inet_aton
#include <pthread.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include <pthread.h>

void arm();
void disarm();
void programa_sobe_lentamente();


int sockfd;
int newsockfd;
int port;

int init_socket(int argc, char *argv[])
{
    int portno = port, clilen;
//     char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    //   int n;
    //int data;
    printf( "using port #%d\n", portno );

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        perror( "ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons( portno );
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
       sizeof(serv_addr)) < 0)
        perror(  "ERROR on binding" );
    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    //--- infinite wait on a connection ---
    while ( 1 )
    {
        printf( "waiting for new client...\n" );
        if ( ( newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, (socklen_t*) &clilen) ) < 0 );
        else
            break;
    }
    return 0;
}



void set_mode(int want_key)
{
    static struct termios old, new_;
    if (!want_key)
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &old);
        return;
    }

    tcgetattr(STDIN_FILENO, &old);
    new_ = old;
    new_.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_);
}

char get_key()
{
//    int c = 0;
//    struct timeval tv;
//    fd_set fs;
//    tv.tv_usec = tv.tv_sec = 0;
//
//    FD_ZERO(&fs);
//    FD_SET(STDIN_FILENO, &fs);
//    select(STDIN_FILENO + 1, &fs, 0, 0, &tv);
//
//    if (FD_ISSET(STDIN_FILENO, &fs))
//    {
//        c = getchar();
//        set_mode(0);
//    }
//    return c;

    int charsToRead = 0;

    ioctl(newsockfd, FIONREAD, &charsToRead);

    if(charsToRead == 0)
        return '\0';

    char ret;
    int n;

    n = (int)recv(newsockfd,&ret,1,0);
    
    printf("%d\n", ret);
    if (n < 0)
    {
        printf("Erro lendo do socket!\n");
        exit(1);
    }
    return ret;

}


int read_int(){
    
    int ret;
    int n;
    
    int charsToRead = 0;
    
    while (charsToRead < sizeof(int)) {
        ioctl(newsockfd, FIONREAD, &charsToRead);
    }
    
    n = (int)recv(newsockfd,&ret,sizeof(ret),0);
    
    printf("%d\n", ret);
    
    if (n < 0)
    {
        printf("Erro lendo do socket!\n");
        exit(1);
    }
    return ret;
   
    
}

void closeEverything(int signal)
{

    control_t send;

    send.throttle = 1000;
    send.roll = 1500;
    send.pitch = 1500;
    send.yaw = 1000;
    send.aux1 = 1000;
    send.aux2 = 1500;
    send.aux3 = 1500;
    send.aux4 = 1500;

    
    for(int i = 0; i < 15 ; i++){
        multiwii_set_control(send);
    }

    multiwii_stop_manipulation();
    close(newsockfd);
    close(sockfd);
    printf("Bye\n");
    exit(1);

}

int main(int argc, char **argv)
{

    
//    if(argc < 2 )
//    {
//        printf("Uso ./leitor numero_porta\n");
//        exit(1);
//    }
//    port = atoi(argv[1]);
    port = 5000;

    printf("\n -----------------------------------------------------\n\n");
    printf("\nPrograma para controle de quadcoptero Multiwii por porta Serial");
    printf("\nDesenvolvido por Matheus Garay Trindade\n\n");
    printf("\nUsando porta default: COM2 (Para mudar, va em MultiWiiNetwork.cpp)\n\n");

    signal(SIGINT, closeEverything);

    //set_mode(1);
    init_socket(argc, argv);
    /********************/
    multiwii_init_manipulation();
    /********************/
    



    int throttle = 1500;
    int pitch = 1500;
    int roll = 1500;
    int yaw = 1500;
    int aux1 = NOMINAL;

    control_t send;
    send.throttle = 1500;
    send.pitch = 1500;
    send.roll = 1500;
    send.yaw = 1500;
    send.aux1 = 1500;
    send.aux2 = 1500;
    send.aux3 = 1500;
    send.aux4 = 1500;

    while(1)
    {
//        set_mode(1);
//        char c = get_key();
//
//        switch(c)
//        {
//            case 'o':
//            if(throttle < MAX_THROTTLE) throttle += 50;
//            break;
//            case 'l':
//            if(throttle > MIN) throttle -= 50;
//            break;
//            case 'w':
//            if(pitch < MAX) pitch += 50;
//            break;
//            case 's':
//            if(pitch > MIN) pitch -= 50;
//            break;
//            case 'd':
//            if(yaw < MAX) yaw += 50;
//            break;
//            case 'a':
//            if(yaw > MIN) yaw -= 50;
//            break;
//            case 'e':
//            if(roll < MAX) roll += 50;
//            break;
//            case 'q':
//            if(roll > MIN) roll -= 50;
//            break;
//            case 'b': if(aux1 == NOMINAL) aux1 = MAX;
//            else  aux1 = NOMINAL;
//                break;
//                
//
//        }


        
        send.pitch = read_int();
        send.roll = read_int();
        send.yaw = read_int();
        send.throttle = read_int();
        send.aux1 = read_int();
        send.aux2 = read_int();
        send.aux3 = read_int();
        send.aux4 = read_int();
        
        
//        
//        send.throttle = throttle;
//        send.roll = roll;
//        send.pitch = pitch;
//        send.yaw = yaw;
//        send.aux1 = aux1;
//        send.aux2 = MAX;
//        send.aux3 = 1500;
//        send.aux4 = 1500;
//        
        multiwii_set_control(send);
//
//        control_t ctrl =
//        multiwii_get_control();
//
//        printf("Throttle -> %4d\t", ctrl.throttle);
//        printf("Pitch    -> %4d\t", ctrl.pitch);
//        printf("Roll     -> %4d\t", ctrl.roll);
//        printf("Yaw      -> %4d\t", ctrl.yaw);
//        printf("Aux1      -> %4d\t", ctrl.aux1);
//        printf("\n");
//
//
//        printf("Throttle -> %4d\t", send.throttle);
//        printf("Pitch    -> %4d\t", send.pitch);
//        printf("Roll     -> %4d\t", send.roll);
//        printf("Yaw      -> %4d\t", send.yaw);
//        printf("\n");
//
////        imu_t imu = multiwii_get_imu();
////
////        printf("Acc X    -> %4d ", imu.acc_x);
////        printf("Acc y    -> %4d ", imu.acc_y);
////        printf("Acc z    -> %4d ", imu.acc_z);
////        printf("Gyr X    -> %4d ", imu.gyro_x);
////        printf("Gyr Y    -> %4d ", imu.gyro_y);
////        printf("Gyr Z    -> %4d ", imu.gyro_z);
////        printf("Mag X    -> %4d ", imu.mag_x);
////        printf("Mag Y    -> %4d ", imu.mag_y);
////        printf("Mag Z    -> %4d ", imu.mag_z);
////        printf("\n");
//        usleep(50000);
//        usleep(1000);
//
    }

    multiwii_stop_manipulation();

    return 0;
}


void arm(){
    
    printf("arming\n");
    
    control_t send;

    send.throttle = MIN;
    send.roll = NOMINAL;
    send.pitch = NOMINAL;
    send.yaw = NOMINAL;
    send.aux1 = MAX;
    send.aux2 = NOMINAL;
    send.aux3 = NOMINAL;
    send.aux4 = NOMINAL;
    

    timeval t;
    timeval current_val;
    gettimeofday(&t, NULL);
    unsigned int start = t.tv_usec/1000;
    
    
    gettimeofday(&current_val, NULL);
    unsigned int end = current_val.tv_usec/1000;

    
    while (end-start < 1) {
        multiwii_set_control(send);
        gettimeofday(&current_val, NULL);
        end = current_val.tv_usec/1000;
        usleep(100000);
    }
}

void disarm(){
    
    control_t send;
    
    send.throttle = MIN;
    send.roll = NOMINAL;
    send.pitch = NOMINAL;
    send.yaw = NOMINAL;
    send.aux1 = NOMINAL;
    send.aux2 = NOMINAL;
    send.aux3 = NOMINAL;
    send.aux4 = NOMINAL;
    
    
    timeval t;
    timeval current_val;
    gettimeofday(&t, NULL);
    unsigned int start = t.tv_usec/1000;
    
    
    gettimeofday(&current_val, NULL);
    unsigned int end = current_val.tv_usec/1000;
    

    multiwii_set_control(send);

    
    
    while (end-start < 10) {

        multiwii_set_control(send);
        
        gettimeofday(&current_val, NULL);
        end = current_val.tv_usec/1000;
    }
    
}

void programa_sobe_lentamente(){

    arm();
    
    timeval t_inicio;
    gettimeofday(&t_inicio, NULL);
    timeval t_now;
    gettimeofday(&t_now, NULL);
    
    
    control_t send;
    
    send.throttle = 1550;
    send.roll = NOMINAL;
    send.pitch = NOMINAL;
    send.yaw = NOMINAL;
    send.aux1 = MAX;
    send.aux2 = NOMINAL;
    send.aux3 = NOMINAL;
    send.aux4 = NOMINAL;

    
    while (t_now.tv_sec - t_inicio.tv_sec < TIME_SOBE_LENTAMENTE) {
        printf("%ld\n", t_now.tv_sec - t_inicio.tv_sec);
        multiwii_set_control(send);
        gettimeofday(&t_now, NULL);
        
        usleep(100000);
        control_t ctrl =
        multiwii_get_control();
        
        printf("Throttle -> %4d\t", ctrl.throttle);
        printf("Pitch    -> %4d\t", ctrl.pitch);
        printf("Roll     -> %4d\t", ctrl.roll);
        printf("Yaw      -> %4d\t", ctrl.yaw);
        printf("Aux1      -> %4d\t", ctrl.aux1);
        printf("\n");
    }
    
    
    send.throttle = 1450;
    send.roll = NOMINAL;
    send.pitch = NOMINAL;
    send.yaw = NOMINAL;
    send.aux1 = MAX;
    send.aux2 = NOMINAL;
    send.aux3 = NOMINAL;
    send.aux4 = NOMINAL;
    
    
    while (t_now.tv_sec - t_inicio.tv_sec < TIME_SOBE_LENTAMENTE + 4) {
        printf("%ld\n", t_now.tv_sec - t_inicio.tv_sec);
        multiwii_set_control(send);
        gettimeofday(&t_now, NULL);
        
        usleep(100000);
        control_t ctrl =
        multiwii_get_control();
        
        printf("Throttle -> %4d\t", ctrl.throttle);
        printf("Pitch    -> %4d\t", ctrl.pitch);
        printf("Roll     -> %4d\t", ctrl.roll);
        printf("Yaw      -> %4d\t", ctrl.yaw);
        printf("Aux1      -> %4d\t", ctrl.aux1);
        printf("\n");
    }
    disarm();
}



/* this function is run by the second thread */
void *multiwii_writer(void *args)
{
    control_t *send = (control_t *) args;
    
    while (true) {
        multiwii_set_control(*send);
    }
    return NULL;
    
}
