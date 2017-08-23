#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/signal.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <time.h>

 
#define DEFDATALEN      56
#define MAXIPLEN        60
#define MAXICMPLEN      76
 
static char *hostname = NULL;
const char *host = NULL;
static void ping(const char *host);
FILE *fp;
int pingsock;

static int in_cksum(unsigned short *buf, int sz)
{
  int nleft = sz;
  int sum = 0;
  unsigned short *w = buf;
  unsigned short ans = 0;
   
  while (nleft > 1) {
    sum += *w++;
    nleft -= 2;
  }
   
  if (nleft == 1) {
    *(unsigned char *) (&ans) = *(unsigned char *) w;
    sum += ans;
  }
   
  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  ans = ~sum;
  return (ans);
}

static void startp(char *host)
{
  while(1)
  {
    ping(host);
    sleep(1);
  }
} 
 
static void ping(const char *host)
{
  struct hostent *h;
  struct sockaddr_in pingaddr;
  struct icmp *pkt;
  struct tms *buf;
  int c;
  char packet[DEFDATALEN + MAXIPLEN + MAXICMPLEN];
  struct timespec start, end;
  int count = 0;
  uint64_t ttime;
  struct in_addr ip_addr;
   
  if ((pingsock = socket(AF_INET, SOCK_RAW|SOCK_NONBLOCK, 1)) < 0) {       /* 1 == ICMP */
    perror("ping: creating a raw socket");
    exit(1);
  }
   
  memset(&pingaddr, 0, sizeof(struct sockaddr_in));
  pingaddr.sin_family = AF_INET;

  if (!(h = gethostbyname(host))) {
    fprintf(stderr, "ping: unknown host %s\n", host);
    exit(1);
  }

  ip_addr = *(struct in_addr *)(h->h_addr);//store IP addres
  memcpy(&pingaddr.sin_addr, h->h_addr, sizeof(pingaddr.sin_addr));
  hostname = h->h_name;//store hostname
   
  pkt = (struct icmp *) packet;
  memset(pkt, 0, sizeof(packet));
  pkt->icmp_type = ICMP_ECHO;//set IMCP type
  pkt->icmp_cksum = in_cksum((unsigned short *) pkt, sizeof(packet));
   
  c = sendto(pingsock, packet, sizeof(packet), 0, (struct sockaddr *) &pingaddr, sizeof(struct sockaddr_in));

  if (c < 0 || c != sizeof(packet)) 
  {
    if (c < 0)
      perror("ping: sendto");

    sleep(1);
    close(pingsock);
    startp(host);
  }
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);

  /* listen for replies */
  while (1) {
    struct sockaddr_in from;
    size_t fromlen = sizeof(from);
    
    errno = 0;
    if ((c = recvfrom(pingsock, packet, sizeof(packet), 0, (struct sockaddr *) &from, &fromlen)) <= 0) 
    {
      if (errno == EINTR || errno == EAGAIN) { //packet not received yet
        count++;
        if(count >= 900000) {
          fprintf(stderr,"No response from %s|%s\n", hostname,inet_ntoa(ip_addr));
          close(pingsock);
          clock_gettime(CLOCK_MONOTONIC_RAW, &end);
          ttime = 0;
          fp=fopen("time.txt","a");
          fprintf(fp, "%d\n",ttime);
          fclose(fp);
          sleep(1);
          startp(host);
        }
        continue;
      }
      perror("ping: recvfrom");
      break;
    }

    if (c >= 76) {                   /* ip + icmp */
      struct iphdr *iphdr = (struct iphdr *) packet;
       
      pkt = (struct icmp *) (packet + (iphdr->ihl << 2));      /* skip ip hdr */
      if (pkt->icmp_type == ICMP_ECHOREPLY)
        break;
    }
  }
  clock_gettime(CLOCK_MONOTONIC_RAW, &end);

  ttime = (uint64_t)((end.tv_sec * 1000000000 + end.tv_nsec) - (start.tv_sec * 1000000000 + start.tv_nsec))/1000;
  printf("%s|%s is alive!  ", hostname,inet_ntoa(ip_addr));
  ttime >= 100000 ? printf("\x1b[31m" "%d µsec" "\x1b[0m" "\n", ttime) : printf("\x1b[32m" "%d µsec" "\x1b[0m" "\n", ttime);
  
  fp=fopen("time.txt","a");
  fprintf(fp, "%d\n",ttime);
  fclose(fp);
  close(pingsock);
}

void INThandler()
{
  printf("Print graphic!\n");
  system("python plot.py");
  exit(0);
}

int main (int argc, char* argv [])
{
  signal(SIGINT, INThandler);
  if(argc >= 2)
  {
    system("rm time.txt");
    startp(argv[1]);
  }
  else
    return 0;
}