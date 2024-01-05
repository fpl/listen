/*
 * listen.c - server that accept connection on an available TCP port.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void 
copydata( int fd,int verbose )
{
  char buf[200];
  int mypid;
  FILE *fp;

  mypid = getpid();
  if ((fp=fdopen(fd,(const char*)"r"))==(FILE*)NULL) {
    fprintf(stderr,"%d: can't fdopen\n",mypid );
    exit(1);
  }
  while (fgets(buf,sizeof(buf),fp)) {
    if (verbose) printf("%d: ",mypid);
    fputs(buf,stdout);
    fflush(stdout);
  }
  exit(0);
}


int
main(int argc,char **argv)
{
  int verbose;
  int lfd,sfd;
  struct sockaddr_in myaddr, claddr;
  int inane;
  int pid;
  
  verbose=0;
  if ( argc>1  && strcmp(argv[1],"-v") == 0 ) {
    verbose=1;
    argc--; argv++;
  }
  if (  (lfd=socket(AF_INET,SOCK_STREAM,0)) <0 ) {
    perror("socket");
    exit(1);
  }
  myaddr.sin_family = AF_INET;
  myaddr.sin_port = 0;
  myaddr.sin_addr.s_addr = INADDR_ANY;
  if ( bind(lfd,(const struct sockaddr*)&myaddr,sizeof(myaddr)) <0 ) {
    perror("bind");
    exit(1);
  }
  inane = sizeof(myaddr);
  if ( getsockname(lfd,(struct sockaddr* restrict)&myaddr,(socklen_t * restrict)&inane) <0 ) {
    perror("getsockname");
    exit(1);
  }
  if ( listen(lfd,5) < 0 ) {
    perror("listen");
    exit(1);
  }
  fprintf(stderr,"listening on %d\n",ntohs(myaddr.sin_port));
  fflush(stdout);

  while ( inane=sizeof(claddr), (sfd=accept(lfd,(struct sockaddr *)&claddr,(socklen_t *restrict)&inane)) >= 0 ) {
    if ((pid=fork())<0) perror("fork");
    else if (pid==0) copydata(sfd,verbose);
    close(sfd);
    if (verbose)
      fprintf(stderr,"accept %s:%d, pid %d\n",
                     inet_ntoa(claddr.sin_addr),ntohs(claddr.sin_port),pid);
  }
  exit(0);
}



