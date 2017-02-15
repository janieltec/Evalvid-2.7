#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 66000 //1024

void error(char *msg){
   perror(msg);
   exit(1);
}

int main(int argc, char **argv) {
   int sockfd;
   int portno;
   int clientlen;
   struct sockaddr_in serveraddr;
   struct sockaddr_in clientaddr;
   struct hostent *hostp;
   char buf[BUFSIZE];
   char *hostaddrp;
   int optval;
   int n;
   int cont = 1;
   time_t t = time(0);
   struct timeval now, tv0;
   double ms = 0;
   int bufsize;
   if( argc != 2){
       fprintf(stderr, "Usage: %s <port> \n", argv[0]);
       exit(1);
   }
   portno = atoi(argv[1]);
   sockfd = socket(AF_INET, SOCK_DGRAM, 0);
   if(sockfd < 0){
       error("ERROR opening socket"); 
   }
   optval = 1;
//   setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));
   bufsize=1024*1024*1024;
   while(bufsize >0 ){
      if(setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize)) == 0) {
	break;
     }else
	bufsize -= 1024;
   }
   if(bufsize<=0){
      printf("setsockopt error: rcvbuf\n");
      exit(1);
   }
   printf("Final rcvbuf size= %d\n", bufsize);

   bzero((char *)&serveraddr, sizeof(serveraddr));
   serveraddr.sin_family = AF_INET;
   serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
   serveraddr.sin_port = htons((unsigned short)portno);
  if(bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
       error("ERROR on binding");
   printf("Stand by receiving packets....\n");
   clientlen = sizeof(clientaddr);
   gettimeofday(&tv0, NULL);
   while(1){
       bzero(buf,BUFSIZE);
       n = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *)&clientaddr, &clientlen);
       if(n < 0)
	   error("ERROR in recvfrom");
       else if(n ==0)
             break;
       gettimeofday(&now, NULL);
       ms=(double)((now.tv_sec-tv0.tv_sec)*1000+(now.tv_usec-tv0.tv_usec)/1000)/1000;
        printf("Number packets [%d] - %d \ttimes %.3f\n", cont++, n, ms);

//   hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
//   if(hostp == NULL)
//	error("ERROR on gethostbyaddr");
//   if(hostaddrp == NULL)
//   error("ERROR on inet_ntoa\n");
//printf("server received datagram from %s (%s)\n", hostp->h_name, hostaddrp);
//printf("server received %d/%d bytes: %s\n", strlen(buf), n, buf);

//   n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&clientaddr, clientlen);
//if(n < 0)
//  error("ERROR in sendto");
   }/* While */
   printf("Number total packets = %d \n", --cont);
}
