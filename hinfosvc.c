// $NoKeywords: $ipk_project_1 $hinfosvc.c
// $Author:     Tadeas Kachyna <xkachy00@stud.fit.vutbr.cz>
// $Date:       $2022-03-11
//============================================================================//
/**
 * @file        hinfosvc.c
 * @author      Tadeas kachyna
 *
 * @brief       Implementation of  HTTP server
 */

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>

#define CPU_STATS 10
#define OPTIMAL_SIZE 2048
#define CPU_LOAD 64

// this enumeration represents values in /proc/stat, which are used to calculate utilization of CPU
enum cpustat {
    USER,
    NICE,
    SYSTEM,
    IDLE,
    IOWAIT,
    IRQ,
    SOFTIRQ,
    STEAL,
    GUEST,
    GUEST_NICE
};

/**
* @function get_cpu_load
* @brief function counting the CPU utilization
* @return double result 
*/
void get_cpu_load(char *output){
    char *delim;
    char buffer[OPTIMAL_SIZE];
    long long int cpu[CPU_STATS];
    long long int prevnonidle, previdle;
    long long int nonidle, idle;
    long long int total, prevtotal;
    long long int totald, idled;
    
    FILE *fp;
    fp = popen("cat /proc/stat", "r"); // getting CPU statistics
    fgets(buffer, OPTIMAL_SIZE, fp);
    pclose(fp);
    
    int iter = 0;
    delim = strtok(buffer, " ");
    while(iter < CPU_STATS) { // loading them into the array
        
        delim = strtok(NULL, " ");
        cpu[iter] = strtoll(delim, NULL, 10); 
        iter++;
    }
   
    previdle = cpu[IDLE] + cpu[IOWAIT];
    prevnonidle = cpu[USER] + cpu[NICE] + cpu[SYSTEM] + cpu[IRQ] + cpu[SOFTIRQ] + cpu[STEAL];

    sleep(1);   
    
    fp = popen("cat /proc/stat", "r");
    fgets(buffer, OPTIMAL_SIZE, fp);
    pclose(fp);

    iter = 0;
    delim = strtok(buffer, " ");
    while(iter < CPU_STATS) { // loading them into the array
        
        delim = strtok(NULL, " ");
        cpu[iter] = strtoll(delim, NULL, 10); 
        iter++;
    }

    idle = cpu[IDLE] + cpu[IOWAIT];
    nonidle = cpu[USER] + cpu[NICE] + cpu[SYSTEM] + cpu[IRQ] + cpu[SOFTIRQ] + cpu[STEAL];

    prevtotal = previdle + prevnonidle;
    total = nonidle + idle;

    totald = total - prevtotal;
    idled = idle - previdle;

    double result = ((double)totald - (double)idled) / (double)totald * (double)100;

    snprintf(output, 64, "%.2f", result);
    strcat(output, "%\n");
}

/**
* @function get_host_name
*
* @param char string where the outpout should be written
* @brief function getting the host name
*
* @return char hostname
*/
void get_host_name(char *hostname){

    FILE *fp;
    // bash command to extract hostname
    fp = popen("cat /proc/sys/kernel/hostname", "r");
    fgets(hostname, OPTIMAL_SIZE, fp);
}

/**
* @function get_cpu_name
* @param char string, where the outpout should be written
* @brief function getting the name of your CPU
*
* @return char hostname
*/
void get_cpu_name(char *cpuname){

    FILE *fp;
    // bash command to extract cpu name
    fp = popen("cat /proc/cpuinfo | grep -i 'model name' | head -n 1 | awk -F':' '{print substr($2, 2)}'", "r");
    fgets(cpuname, OPTIMAL_SIZE, fp);
}

/**
* @function main
* @param int argc number of arguments
* @param char argv contents of arguments
*
* @return int 0
*/
int main(int argc, char const *argv[])
{   
    // argument check
    if(argc != 2) {

        fprintf(stderr, "ERROR > Missing argument(s)!\n");
        return (EXIT_FAILURE);
    }

    char *ret;
    long port_num;
    port_num = strtol(argv[1], &ret, 10);
    if(port_num== '\0') {

        fprintf(stderr, "ERROR > Port has to be a number!\n");
        return (EXIT_FAILURE);
    }
  
    int server_socket, new_socket; 
    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);


    // creating socket file descriptor
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {

        fprintf(stderr, "ERROR > Creating socket failed!\n");
        exit(EXIT_FAILURE);
    }
    
    // setsockopt provides an application program with the means to control socket behavior
    int reuse = 1;
    setsockopt(server_socket, SOL_SOCKET ,SO_REUSEADDR | SO_REUSEPORT, &reuse, sizeof(reuse));

    address.sin_family = AF_INET; // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // any IP
    address.sin_port = htons( port_num ); // port number specified by the user
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    // assign a local socket address address to a socket identified by descriptor socket that has no local socket address assigned
    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {

        fprintf(stderr, "ERROR > Binding Error!\n");
        exit(EXIT_FAILURE);
    }
    
    //indicates a readiness to accept client connection requests
    if (listen(server_socket, 10) < 0) {

        fprintf(stderr, "ERROR > Listening Error!\n");
        exit(EXIT_FAILURE);
    }
    
    char http_200[OPTIMAL_SIZE];
    static bool start = false;

 while(1) {

        // help - it gets print only once
        if(!start) {
            
            printf("====================================================================\n");
            printf("You have successfully launched the server!\nTo close the server - please - press CTRL + C on your keyboard.\n");
            printf("Help: ./hinfosvc [port_num]> & curl http://localhost:[port_num]/[hostame|cpu-name|load]\n");
            printf("You will find more help in the enclosed Readme.md.\n");
            printf("====================================================================\n");
            start = true;
        }

        if ((new_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) <0 ) {

            fprintf(stderr, "ERROR > accept() Error!\n");
            exit(EXIT_FAILURE);
        }

        char buffer[OPTIMAL_SIZE];
        valread = read(new_socket , buffer, OPTIMAL_SIZE);
        if(valread < 0) {
            fprintf(stderr, "ERROR > Valread(): Not enough space!\n");
            exit(EXIT_FAILURE);
        }

        // 200 OK - hostname
        if(strncmp(buffer, "GET /hostname ", 14) == 0) {   

            char hostname[OPTIMAL_SIZE];
            get_host_name(hostname);
            int length = strlen(hostname);
            sprintf(http_200,"HTTP/1.1 200 OK\r\nConnection: close\r\nContent-length: %d\r\nContent-Type: text/plain\r\n\r\n", length);
            write(new_socket , http_200, strlen(http_200));
            write(new_socket , hostname , strlen(hostname));
            fflush(stdout);

        // 200 OK - cpu-name 
        } else if (strncmp(buffer, "GET /cpu-name ", 14) == 0) {   

            char cpu_name[OPTIMAL_SIZE];
            get_cpu_name(cpu_name);
            int length = strlen(cpu_name);
            sprintf(http_200,"HTTP/1.1 200 OK\r\nConnection: close\r\nContent-length: %d\r\nContent-Type: text/plain\r\n\r\n", length);
            write(new_socket, http_200, strlen(http_200));
            write(new_socket , cpu_name , strlen(cpu_name));
            fflush(stdout);

        // 200 OK - load
        } else if (strncmp(buffer, "GET /load ", 10) == 0) {   
           
            char output[CPU_LOAD];
            get_cpu_load(output);
            int length = strlen(output);
            sprintf(http_200,"HTTP/1.1 200 OK\r\nConnection: close\r\nContent-length: %d\r\nContent-Type: text/plain\r\n\r\n", length);
            send(new_socket, http_200, strlen(http_200), 0);
            send(new_socket, output , strlen(output), 0);
            fflush(stdout);

        // 400 Bad Request
        } else if (strncmp(buffer, "GET / ", 6) == 0) {
            
            char http_400[OPTIMAL_SIZE] = "HTTP/1.1 400 Bad Request\r\nConnection: close\r\nContent-length: 16\r\nContent-Type: text/plain\r\n\r\n400 Bad Request\n";
            write(new_socket , http_400, strlen(http_400));
            
        // 404 Not Found
        } else {

            char http_404[OPTIMAL_SIZE] = "HTTP/1.1 404 Not Found\r\nConnection: close\r\nContent-length: 14\r\nContent-Type:   text/plain\r\n\r\n404 Not Found\n";
            write(new_socket , http_404 , strlen(http_404));
                     
        }           
        close(new_socket);
    }
    
    return 0;
}
