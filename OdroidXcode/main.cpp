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


void programa_sobe_lentamente();
void *multiwii_writer(void *args);
void *monitor_abort(void *args);


int sockfd;
int newsockfd;
int port;


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t should_die = PTHREAD_MUTEX_INITIALIZER;

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





float read_float(){
    
    float ret;
    int n;
    
    //    int charsToRead = 0;
    
    //    while (charsToRead < sizeof(int)) {
    //        ioctl(newsockfd, FIONREAD, &charsToRead);
    //    }
    
    
    n = (int)recv(newsockfd,&ret,sizeof(ret),0);
    
    
    
    if (n <= 0)
    {
        printf("Erro lendo do socket!\n");
        exit(1);
    }
    return ret;
    
    
}


int read_int(){
    
    int ret;
    int n;
    
//    int charsToRead = 0;
    
//    while (charsToRead < sizeof(int)) {
//        ioctl(newsockfd, FIONREAD, &charsToRead);
//    }
    
    
    n = (int)recv(newsockfd,&ret,sizeof(ret),0);
    

    
    if (n <= 0)
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

    
    for(int i = 0; i < 100 ; i++){
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

    
    if(argc < 2 )
    {
        printf("Uso ./leitor arquivo_entrada\n");
        exit(1);
    }
//    port = atoi(argv[1]);


    printf("\n -----------------------------------------------------\n\n");
    printf("\nPrograma para controle de quadcoptero Multiwii por porta Serial");
    printf("\nDesenvolvido por Matheus Garay Trindade\n\n");
    printf("\nUsando porta default: COM2 (Para mudar, va em MultiWiiNetwork.cpp)\n\n");

    signal(SIGINT, closeEverything);

    
    FILE *arquivo_entrada = fopen(argv[1], "r");
    
    if(arquivo_entrada == NULL){
        printf("Arquivo %s nao encontrodo\n", argv[1]);
        exit(1);
    }
    else{
        printf("Lendo %s\n", argv[1]);
        
    }

//    init_socket(argc, argv);
    
    /********************/
//    multiwii_init_manipulation();
    /********************/
    


    control_t send, send2;
    send.throttle = 1500;
    send.pitch = 1500;
    send.roll = 1500;
    send.yaw = 1500;
    send.aux1 = 1500;
    send.aux2 = 1500;
    send.aux3 = 2000;
    send.aux4 = 1500;
    
    send2 = send;
    
    pthread_t thread, thread2;
//    
//    pthread_create(&thread, NULL, multiwii_writer, &send2);
//    pthread_create(&thread2, NULL, monitor_abort, &send2);
//    

//    while(1)
//    {
    
        int number_of_iterations;// = read_int();
        
        fscanf(arquivo_entrada, "%d\n", &number_of_iterations);
        printf("Programa: ");
    
        printf("%d iterações\n", number_of_iterations);
        
        int target_throttle[number_of_iterations];
        int number_of_reps[number_of_iterations];
        float reps_duration[number_of_iterations];
        
        for (int i = 0 ; i < number_of_iterations ; i++){
            
            fscanf(arquivo_entrada, "%d %d %f", target_throttle + i, number_of_reps + i, reps_duration + i);
//            target_throttle[i] = read_int();
//            number_of_reps[i] = read_int();
//            reps_duration[i] = read_float();
            
            printf("%d, %d, %f\n",target_throttle[i], number_of_reps[i], reps_duration[i]);
        }
        
        send.aux1 = 2000;
        
        
        
        for(int i = 0 ; i < number_of_iterations ; i++){
            float delta_throttle = (target_throttle[i] - send.throttle)/((float)number_of_reps[i]);
            printf("Iteracao %d\n", i);
            for(int j = 0 ; j < number_of_reps[i] ; j++){
                
                send.throttle = (int) (send.throttle + delta_throttle);
                pthread_mutex_lock(&mutex);
                send2 = send;
                pthread_mutex_unlock(&mutex);
                printf("\tRep: %d\n", j);
                usleep(1000000 * reps_duration[i]);
                
            }
            
        }
        
        
        send.throttle = 1000;
        send.aux1 = 1500;
        pthread_mutex_lock(&mutex);
        send2 = send;
        pthread_mutex_unlock(&mutex);
        printf("Desligando\n");
        usleep(1000000);
//    }

    multiwii_stop_manipulation();

    return 0;
}


/* this function is run by the second thread */
void *multiwii_writer(void *args)
{
    control_t *send = (control_t *) args;
    control_t send2;
    
    while (true) {
        
        pthread_mutex_lock(&mutex);
        send2 = *send;
        pthread_mutex_unlock(&mutex);
        
        
        pthread_mutex_lock(&should_die);
        multiwii_set_control(send2);
        pthread_mutex_unlock(&should_die);
        usleep(2800);
        
        
        
//        control_t ctrl =
//        multiwii_get_control();
//        printf("Throttle -> %4d\t", ctrl.throttle);
//        printf("Pitch    -> %4d\t", ctrl.pitch);
//        printf("Roll     -> %4d\t", ctrl.roll);
//        printf("Yaw      -> %4d\t", ctrl.yaw);
//        printf("Aux1      -> %4d\t", ctrl.aux1);
//        printf("\n");
        
    }
    return NULL;
}


/* this function is run by the second thread */
void *monitor_keyboard(void *args)
{
    
    port = 5050;
    
    init_socket(0, 0);
    printf("I died\n");
    //Termina ultimas escritas, adquire o lock e nao solta mais;
    pthread_mutex_lock(&should_die);
    
    closeEverything(0);
    
    return NULL;
}


/* this function is run by the second thread */
void *monitor_abort(void *args)
{
  
    port = 5050;
    
    init_socket(0, 0);
    printf("I died\n");
    //Termina ultimas escritas, adquire o lock e nao solta mais;
    pthread_mutex_lock(&should_die);
    
    closeEverything(0);
    
    return NULL;
}
