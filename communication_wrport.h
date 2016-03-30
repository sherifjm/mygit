
//file name : communication_wrprot.h

#include <stdio.h>
#include <stdlib.h>
#include <termio.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <linux/rtc.h>
#include <semaphore.h>
#include <dirent.h>
#include <ctype.h>
#include <linux/input.h>
#include <sys/reboot.h>
#include <signal.h>
#include <linux/watchdog.h>
#include "communication_procotol.h"
#include "bmpfile.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#ifndef __COMMUNICATION_WRPORT_H
#define __COMMUNICATION_WRPORT_H

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define SERVER_PORT 5561

#ifndef __COMMUNICATION_WRPORT_C
#define __EXTERN_CW extern
#else
#define __EXTERN_CW
#endif

__EXTERN_CW void  pthread_udpRcv(ST_PORTRCVTXD_TAG *stPort);
__EXTERN_CW void  pthread_tcpRcv(ST_PORTRCVTXD_TAG *stPort);
__EXTERN_CW void  pthread_uart(ST_PORTRCVTXD_TAG *stPort);
__EXTERN_CW void  pthread_tcpAccept(ST_PORTRCVTXD_TAG *stPort);

#ifdef START_CONNECT_TYPE
void pthread_cmsAccept(ST_PORTRCVTXD_TAG *stPort);
#endif

int net_detect(char *net_name);

extern int GetIpport(void);

#endif









