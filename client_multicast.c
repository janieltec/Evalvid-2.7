#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

struct sockaddr_in localsock;
struct ip_mreq group;
int sd;
int datalen;
char databuf[1024];
unsigned int count = 0;
int totalsize = 0;
struct timeval now, tv0;
double ms = 0;
unsigned int nbytes = 0;
int cont = 1;

void error( char *msg){
   printf("%s\n", msg);
   exit(1);
}
int main(int argc, int *argv[]){
//   if(argc !=2){
//      fprintf(stderr, "Usage %s <IP local> \n", arg[0]);
//      exit(1);
//   }

   sd = socket(AF_INET, SOCK_DGRAM, 0);
   if( sd <0) {
      error("ERRO: opening datagrama socket");
    }
   else
      printf("Opening datagrama socket ... OK. \n");

int reuse = 1;
if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0){
   perror("Setting SO_REUSEADDR error");
   close(sd);
   exit(1);
}
else
   printf("Setting SO_REUSEADDR...OK.\n");

memset((char *)&localsock, 0, sizeof(localsock));
localsock.sin_family = AF_INET;
localsock.sin_port = htons(1234);
localsock.sin_addr.s_addr = INADDR_ANY;
if(bind(sd,(struct sockaddr *)&localsock, sizeof(localsock))){
   perror("Binding datagram sock error");
   close(sd);
   exit(1);
}
else
   printf("Bindign datagram socket ... OK. \n");


group.imr_multiaddr.s_addr = inet_addr("239.255.0.1");
group.imr_interface.s_addr = inet_addr(argv[1]);
if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) <0 ){
   perror("ERROR: Adding multicast group");
   close(sd);
   exit(1);
}
else
   printf("Adding multicast group .... OK. \n");

datalen = sizeof(databuf);
gettimeofday(&tv0,NULL);

while(1){
  printf("Recebendo dados .. \n");
  bzero(databuf, datalen);
  nbytes = recvfrom(sd, databuf, datalen, 0, (struct sockaddr *) &localsock, sizeof(localsock));
  if(nbytes < 0)
     error("ERROR: recvfrom");
  else if (nbytes == 0) {
	break;
	}
   gettimeofday(&now, NULL);
   ms = (double)((now.tv_sec-tv0.tv_sec)*1000 + (now.tv_usec - tv0.tv_usec)/1000)/1000;
   printf("Number packets [%d] \t Lenght= %d \t times %.3f\n", cont++, nbytes, ms);
//    totalsize = totalsize+count;
//    printf("Pacotes recebidos %d\n", totalsize);
//   if(read(sd, databuf, datalen) <0 )
//	error("ERROR: Read");
//   else{
//   printf("Reading datagram message ... OK. \n");
//   printf("The message form multicast server is : \"%s \" \n", databuf);  
//   }
}

return 0;
}
