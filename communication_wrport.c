
#define __COMMUNICATION_WRPORT_C

#include "communication_wrport.h"

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <string.h>
#include <stdio.h>

int net_detect(char *net_name)
{
	int ret;
	int skfd = 0;
	struct ifreq ifr;
//	struct sockaddr_in *pAddr = NULL;
	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(skfd < 0)
	{
		printf("%s:%d Open socket error!\n", __FILE__, __LINE__);
		return -1;
	}
	strcpy(ifr.ifr_name, net_name);
	if(ioctl(skfd, SIOCGIFFLAGS, &ifr) <0 )
	{
		printf("%s:%d IOCTL error!\n", __FILE__, __LINE__);
		printf("Maybe ethernet inferface %s is not valid!", ifr.ifr_name);
		close(skfd);
		return -1;
	}
	if(ifr.ifr_flags & IFF_RUNNING)
	{
		//printf("%s is running :)\n", ifr.ifr_name);
		ret = 0;
	}
	else
	{
		//printf("%s is not running :(\n", ifr.ifr_name);
		ret = -1;
	}
	if(ioctl(skfd,SIOCGIFADDR,&ifr)<0)
	{
		printf("SIOCGIFADDR IOCTL error!\n");
		close(skfd);
		return -1;
	}
	//pAddr = (struct sockaddr_in *)&(ifr.ifr_addr);
	//printf("ip addr :[%s]\n", inet_ntoa(pAddr->sin_addr));
	if(ioctl(skfd,SIOCGIFHWADDR,&ifr)<0)
	{
		printf("SIOCGIFHWADDR IOCTL error!\n");
		close(skfd);
		return -1;
	}
	/*
	printf("mac addr: %02x:%02x:%02x:%02x:%02x:%02x\n",
		(unsigned char)ifr.ifr_hwaddr.sa_data[0],
		(unsigned char)ifr.ifr_hwaddr.sa_data[1],
		(unsigned char)ifr.ifr_hwaddr.sa_data[2],
		(unsigned char)ifr.ifr_hwaddr.sa_data[3],
		(unsigned char)ifr.ifr_hwaddr.sa_data[4],
		(unsigned char)ifr.ifr_hwaddr.sa_data[5]);
	*/
	close(skfd);
	return ret;
}

void  pthread_udpSend(ST_PORTRCVTXD_TAG *stPort) 
{
	int i,k,n;
	int temp;
	struct sockaddr_in peeraddr;	
    int socklen,sendlen;
	

	socklen = sizeof(peeraddr);
	memset(&peeraddr, 0, socklen);
	peeraddr.sin_family=AF_INET;
	peeraddr.sin_port=htons(7);
	
	peeraddr.sin_addr.s_addr = inet_addr("192.168.2.6");
    if(stPort->socketfd < 0)
		return;
	
    while(1)
    { 
		Wdg_Check(stgContextp,1);
		usleep(1000); // 1ms
        if(stgContextp->CmsUpdata)
        {   
			int ret;
			unsigned int slen=0;
			unsigned char *txd_p;
			unsigned char *dest_p;
            ST_CMS_DATABODY_TAG *stCmsDataBodyp;

			dest_p = malloc(1024);
			txd_p = malloc(1024);
			stCmsDataBodyp = (ST_CMS_DATABODY_TAG *)&txd_p[2];
			
			pthread_mutex_lock(&stgContextp->CmsDataLock);
			for(k=0;k<_CMS_CTRCARD_NUM;k++)
			{
	            temp = stgContextp->CmsData_Len[k] / CMS_DATABODY_LEN_MAX + ((stgContextp->CmsData_Len[k] % CMS_DATABODY_LEN_MAX)>0);
				stCmsDataBodyp->head.TotalPacket = temp;
				stCmsDataBodyp->head.TotalSub = _CMS_CTRCARD_NUM;
                stCmsDataBodyp->head.ColorBits = CMS_COLOR_BITNUM;
				stCmsDataBodyp->head.brightness = Brightness_Control(stgContextp);
				stCmsDataBodyp->head.scanrate = _CMS_CTRCARD_SCAN_RATE;
				stCmsDataBodyp->head.lbith = _CMS_CTRCARD_SCAN_W >> 8;
				stCmsDataBodyp->head.lbitl = _CMS_CTRCARD_SCAN_W;

				for(i=0;i<temp;i++)
				{
	                if((i < (temp-1))||((stgContextp->CmsData_Len[k] % CMS_DATABODY_LEN_MAX)==0))
	                {
	                   sendlen = CMS_DATABODY_LEN_MAX;				   
					}
					else if(stgContextp->CmsData_Len[k] % CMS_DATABODY_LEN_MAX)
					{
	                   sendlen = stgContextp->CmsData_Len[k] % CMS_DATABODY_LEN_MAX;
					}
					txd_p[0] = A006H_SET_CMS_SHOWD >> 8;
					txd_p[1] = A006H_SET_CMS_SHOWD & 0xff;
					stCmsDataBodyp->head.SubId = k;				
					stCmsDataBodyp->head.sizeh = sendlen >> 8;
					stCmsDataBodyp->head.sizel = sendlen & 0xff;
					stCmsDataBodyp->head.PackNo = i+1;
					memcpy(stCmsDataBodyp->buf,&stgContextp->CmsData[k][CMS_DATABODY_LEN_MAX*i],sendlen);
					
                    //sendlen += 10; // 8; 包括命令码
                    sendlen += sizeof(ST_CMS_DATAHEAD_TAG) + 2;
					sendlen = GetSendInfoPacket(dest_p,stgContextp->sendseq++,stgContextp->stSysConfig.address,txd_p,sendlen,stPort);
					slen += sendlen;
					//-------------------debug-------------------			
                    //if((k==(_CMS_CTRCARD_NUM-1))&&(i==(temp-1)))
                    if((k==0)&&(i==0))
                    {
						printf("%s S->:",stPort->portname);
						for(n=0;i<sendlen;n++)
						{
						   if(n>40)
						   {
							   //printf("...len=%d\n",sendlen);
							   break;
						   }
						   else
						   {
							   printf(" %x",dest_p[n]);
						   }
						}
                    }					
					if((k==(_CMS_CTRCARD_NUM-1))&&(i==(temp-1)))
						printf("...len=%d\n",slen);
						//printf(" Total Len =%d,subid=%d,subtotalpack=%d\n",slen,k,stCmsDataBodyp->head.TotalPacket);
					//-------------------------------------

					ret = sendto(stPort->socketfd, dest_p, sendlen, 0, (__CONST_SOCKADDR_ARG )&peeraddr, socklen);
						
	                usleep(400); // 300最低
				}
			}
			
			free(txd_p);
			free(dest_p);
			stgContextp->CmsUpdata = 0;
			
			pthread_mutex_unlock(&stgContextp->CmsDataLock);
		}		
		if(stPort->stSend.len)
		{	
			//-------------------debug-------------------
			printf("%s S->:",stPort->portname);
			for(i=0;i<stPort->stSend.len;i++)
			{
			   if(i>40)
			   {
				   printf("...len=%d",stPort->stSend.len);
				   break;
			   }
			   else
			   {
				   printf(" %x",stPort->stSend.buf[i]);
			   }
			}
			printf("\n");	
			//-------------------------------------

			temp = stPort->stSend.len / 1024 + ((stPort->stSend.len % 1024)>0);
			for(i=0;i<temp;i++)
			{
                if((i < (temp-1))||((stPort->stSend.len % 1024)==0))
                {
                   sendlen = 1024;				   
				}
				else if(stPort->stSend.len % 1024)
				{
                   sendlen = stPort->stSend.len % 1024;
				}

				sendto(stPort->socketfd, &stPort->stSend.buf[i*1024], sendlen, 0, (__CONST_SOCKADDR_ARG )&peeraddr, socklen);			
                usleep(100); 
			}		

			stPort->stSend.len = 0;
		}
    }

}

void  pthread_udpRcv(ST_PORTRCVTXD_TAG *stPort) 
{
	fd_set fds;
	struct timeval tv;
	int n,counter;
	unsigned int tempRcvLen;

	int i;
	int sin_len;
	char message[1024];
	int socket_udp;
	struct sockaddr_in sin;

	bzero(&sin,sizeof(sin));
	sin.sin_family=AF_INET;
	sin.sin_addr.s_addr=htonl(INADDR_ANY);
	sin.sin_port=htons(7);
	sin_len=sizeof(sin);

//----------------------UDP--------------------------
	socket_udp=socket(AF_INET,SOCK_DGRAM,0);
	bind(socket_udp,(struct sockaddr *)&sin,sizeof(sin));
	stPort->socketfd = socket_udp;
//--------------------------------------

    strcpy(stgContextp->Thread_Name[stgContextp->WdtNum],"pthread_udpSend");
	pthread_create(&stgContextp->Thread_Id[stgContextp->WdtNum++], NULL, (void *)pthread_udpSend,(void *)stPort); 

	tempRcvLen	= 0;
	counter = 0;
	while(1)
	{
		Wdg_Check(stgContextp,0);
		usleep(1000); // 1ms

		tv.tv_sec = 0;
		tv.tv_usec= 0;//时间设置为0，非阻塞式，立刻返回
		if(counter < 1000)
			counter++;

		FD_ZERO(&fds);
		FD_SET(socket_udp,&fds);
		select(socket_udp + 1, &fds, NULL, NULL, &tv);
		
		if (FD_ISSET(socket_udp, &fds)) 
		{			
			n = recvfrom(socket_udp,&message[tempRcvLen],sizeof(message)-tempRcvLen,0,(struct sockaddr *)&sin,(socklen_t *)&sin_len);
			if(n>0)
			{
				counter = 0;			
				tempRcvLen += n;
				if(tempRcvLen >= sizeof(stPort->stRcvAnalytice.buf))
				{
					tempRcvLen = 0;
				}
			}			
		} 
		
		if((counter >20)&&(tempRcvLen))//间隔20ms未收到数据
		{
			//-------------------debug-------------------
			printf("%s R<-:",stPort->portname);
			for(i=0;i<tempRcvLen;i++)
			{
			   if(i>40)
			   {
				   printf("...len=%d",tempRcvLen);
				   break;
			   }
			   else
			   {
				   printf(" %x",message[i]);
			   }
			}
			printf("\n");	
			//-------------------------------------
			Wdg_Check(stgContextp,1);
			pthread_mutex_lock(&stPort->stMutexRcv); //debug
			memcpy(stPort->stRcvAnalytice.buf,message,tempRcvLen);
			stPort->stRcvAnalytice.len = tempRcvLen;
			pthread_mutex_unlock(&stPort->stMutexRcv);
			sem_post(&stPort->Sem_RcvBuffNotEmpt);		

			tempRcvLen =0;	 
			counter = 0;
		}
		
	}

	close(socket_udp);
	exit(0);
}

//NET发送线程
void pthread_tcpSend(ST_PORTRCVTXD_TAG *stPort)
{
	fd_set fds;
	int ret,i;
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec= 0;//时间设置为0，非阻塞式，立刻返回
	
    while(1)
	{	
		Wdg_Check(stgContextp,1);
		usleep(1000); // 1ms

		FD_ZERO(&fds);
		if(stPort->fd > 0)
		{
		    FD_SET(stPort->fd,&fds);
			select(stPort->fd + 1, NULL,&fds,  NULL, NULL);	
			/*
			if(FD_ISSET(stPort->fd,&fds))
			{
	           printf("tcp send buffer is empty!\n");
			}
			else
			{
			   printf("tcp send buffer is busy\n!");
			}
			*/
		}
			
		if(stPort->stSend.len && (stPort->fd > 0))
		{			
			ret = send(stPort->fd, stPort->stSend.buf,stPort->stSend.len,0);	
			//-------------------debug-------------------
			/*
            if((stPort->stSend.buf[7]== 0xAF)&&(stPort->stSend.buf[8]== 0xE3))
            {
				test_uploadbitmap(&stPort->stSend.buf[9]);
			}
			*/
			printf("%s S->:",stPort->portname);
			for(i=0;i<stPort->stSend.len;i++)
			{
			   if(i>40)
			   {
				   printf("...len=%d",stPort->stSend.len);
				   break;
			   }
			   else
			   {
				   printf(" %x",stPort->stSend.buf[i]);
			   }
			}
			printf("\n");	
			//-------------------------------------
            stPort->stSend.len = 0;
		}
	}

}

void pthread_tcpRcv(ST_PORTRCVTXD_TAG *stPort)
{
  int i;
  
  fd_set fds;
  struct timeval tv;
  int n,counter;

  ST_TXRX_TAG *rcv_p;
  
  unsigned int tempRcvLen;

  rcv_p = (ST_TXRX_TAG *)malloc(sizeof(ST_TXRX_TAG));
  memset(rcv_p,0x00,sizeof(ST_TXRX_TAG));

  sigset_t setsig;
  __sigemptyset(&setsig);
  __sigaddset(&setsig, SIGALRM);
  sigprocmask(SIG_BLOCK,&setsig,NULL);//屏蔽信号
  tempRcvLen  = 0;
  counter = 0;
  while(1)
  {
    Wdg_Check(stgContextp,1);
    usleep(1000); // 1ms
	
	tv.tv_sec = 0;
	tv.tv_usec= 0;//时间设置为0，非阻塞式，立刻返回
	if(counter < 1000)
		counter++;

	if(stPort->fd < 0)
		continue;
	
	if((counter >20)&&(tempRcvLen))//间隔20ms未收到数据
	{
		//-------------------debug-------------------
		printf("%s R<-:",stPort->portname);
		for(i=0;i<tempRcvLen;i++)
		{
		   if(i>40)
		   {
			   printf("...len=%d",tempRcvLen);
			   break;
		   }
		   else
		   {
			   printf(" %x",rcv_p->buf[i]);
		   }
		}
		printf("\n");	
		//-------------------------------------

        if(!rcv_p->len)
        {
			if(   (rcv_p->buf[0]==0xAA)
				&&((rcv_p->buf[1]==0xBB)||(rcv_p->buf[1]==0xEE)||(rcv_p->buf[1]==0xDD))
			  )	
			{
                memcpy(stPort->stRcvAnalytice.buf,rcv_p->buf,tempRcvLen);
				rcv_p->len = tempRcvLen;
			}
        }
        else 
        {
			if((tempRcvLen + rcv_p->len) < sizeof(stPort->stRcvAnalytice.buf))
			{
				memcpy(&stPort->stRcvAnalytice.buf[rcv_p->len],rcv_p->buf,tempRcvLen);
				rcv_p->len += tempRcvLen;
			}
			else
			{
				rcv_p->len = 0;
			}
		}
        tempRcvLen = stPort->stRcvAnalytice.buf[5];
		tempRcvLen <<= 8;
		tempRcvLen |= stPort->stRcvAnalytice.buf[6];

		if(rcv_p->len >= tempRcvLen)
		{
			stPort->stRcvAnalytice.len = rcv_p->len;			
			sem_post(&stPort->Sem_RcvBuffNotEmpt);
			rcv_p->len = 0;
		}

		
		//stPort->stRcvAnalytice.len = tempRcvLen;			
		//sem_post(&stPort->Sem_RcvBuffNotEmpt);
		
		tempRcvLen =0;	 
		counter = 0;
	}

	FD_ZERO(&fds);
	FD_SET(stPort->fd,&fds);
	select(stPort->fd + 1, &fds, NULL, NULL, &tv);

	if (FD_ISSET(stPort->fd, &fds)) 
	{			
		pthread_mutex_lock(&stPort->stMutexRcv); //debug
		stPort->stRcvAnalytice.len = 0;
		//n = recv(stPort->fd, &stPort->stRcvAnalytice.buf[tempRcvLen], sizeof(stPort->stRcvAnalytice.buf)-tempRcvLen, 0);			
		n = recv(stPort->fd, &rcv_p->buf[tempRcvLen], sizeof(rcv_p->buf)-tempRcvLen, 0);			
		if(n>0)
		{
			counter = 0;			
			tempRcvLen += n;
			if(tempRcvLen >= sizeof(rcv_p->buf))
			{
				tempRcvLen = 0;
			}
		}
		else
		{
			perror("rcv tcp client!");
			stPort->fd = -1;
			//stPort->lineStatue = CONNECT_BREAK;
			printf("Tcp Client Exit!\n");//debug
		}
		pthread_mutex_unlock(&stPort->stMutexRcv);
	}
  }
  
}


void pthread_tcpAccept(ST_PORTRCVTXD_TAG *stPort)
{
  int ipport;	
  int s;

  struct sockaddr_in my_addr;
  struct sockaddr_in remote_addr;
  
  int sin_size;
  
  sigset_t setsig;
  __sigemptyset(&setsig);
  __sigaddset(&setsig, SIGALRM);
  
  sigprocmask(SIG_BLOCK,&setsig,NULL);//屏蔽信号
  memset (&my_addr, 0, sizeof (my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_addr.s_addr = INADDR_ANY;

  ipport = (GetIpport()==0) ? SERVER_PORT : GetIpport();
  printf("Set TcpIp Port:%d\n",ipport);
  my_addr.sin_port = htons (ipport);
  
  if ((s = socket (PF_INET, SOCK_STREAM, 0)) < 0)
  {
	perror ("socket");
	pthread_exit (0);	   
  }
  
  if (bind (s, (struct sockaddr *) &my_addr, sizeof (struct sockaddr)) < 0)
  {
	perror ("bind");
	pthread_exit (0);
  }

  listen (s, 1);
  printf("listen.....\n");
  sin_size = sizeof (struct sockaddr_in);

  stPort->fd = -1;
  strcpy(stgContextp->Thread_Name[stgContextp->WdtNum],"pthread_tcpSend");
  pthread_create(&stgContextp->Thread_Id[stgContextp->WdtNum++], NULL, (void *)pthread_tcpSend,(void *)stPort); 

  strcpy(stgContextp->Thread_Name[stgContextp->WdtNum],"pthread_tcpRcv");
  pthread_create(&stgContextp->Thread_Id[stgContextp->WdtNum++], NULL, (void *)pthread_tcpRcv,(void *)stPort); 
  Wdg_Check(stgContextp,0);

  stPort->fd = -1;

  while(1)
  {
	  usleep(1000); // 1ms
	  stPort->fd = accept(s, (struct sockaddr *) &remote_addr, (socklen_t *)&sin_size);
	  printf("accept client %s\n", (char *)inet_ntoa(remote_addr.sin_addr));
  }  
  close (s);
}

void pthread_uart(ST_PORTRCVTXD_TAG *stPort)
{
	int fd;
	struct timeval tv;
	struct termios attr;
	fd_set fds;
	int n,counter,i;
	sigset_t setsig;

    unsigned int tempRcvLen;
		
	__sigemptyset(&setsig);
	__sigaddset(&setsig, SIGALRM);//信号屏蔽
	sigprocmask(SIG_BLOCK,&setsig,NULL);

	fd = open(stPort->devPth,O_RDWR);
	if(fd < 0)
	{
		printf("Load Dev Err:%s\n",stPort->devPth);
		return ;
	}
	else
	{
		printf("Load Dev  OK:%s\n",stPort->devPth);
	}

	tcgetattr(fd,&attr);
	attr.c_cc[VMIN] = 0;//设置最少接收个数为0
	attr.c_iflag = 0;
	attr.c_oflag = 0;
	attr.c_lflag = 0;
	//attr.c_cflag = B9600|CS8|CREAD;
	attr.c_cflag = stPort->uart_baudrate|CS8|CREAD;
	tcsetattr(fd,TCSANOW,&attr);

	tv.tv_sec = 0;
	tv.tv_usec= 0;//时间设置为0，非阻塞式，立刻返回

	counter = 0;
	tempRcvLen = 0;
	
	for(;;)
	{
		Wdg_Check(stgContextp,1);
		
        if(counter < 1000)
			counter++;
		
		FD_ZERO(&fds);
	    FD_SET(fd,&fds);
		if (select(fd + 1, &fds, NULL, NULL, &tv) < 0) 
		{
			printf("Err:in pthread_uart\n");//Error(strerror(errno));
		}

		if((counter >20)&&(tempRcvLen))//间隔20ms未收到数据
		{
			//-------------------debug-------------------
			printf("%s R<-:",stPort->portname);
			for(i=0;i<tempRcvLen;i++)
			{
			   if(i>40)
			   {
				   printf("...len=%d",tempRcvLen);
				   break;
			   }
			   else
			   {
				   printf(" %x",stPort->stRcvAnalytice.buf[i]);
			   }
			}
			printf("\n");	
			//-------------------------------------
			
			//pthread_mutex_lock(&stPort->stMutexRcv); //debug           
			stPort->stRcvAnalytice.len = tempRcvLen;			
			//pthread_mutex_unlock(&stPort->stMutexRcv);
			sem_post(&stPort->Sem_RcvBuffNotEmpt);
			tempRcvLen =0;	 
			counter = 0;
		}

		if (FD_ISSET(fd, &fds)) 
		{			
			pthread_mutex_lock(&stPort->stMutexRcv); //debug
			stPort->stRcvAnalytice.len = 0;
			n = read(fd,&stPort->stRcvAnalytice.buf[tempRcvLen], sizeof(stPort->stRcvAnalytice.buf) - tempRcvLen);			 
            if(n>0)
            {
				counter = 0;
				tempRcvLen += n;
	            if(tempRcvLen >= sizeof(stPort->stRcvAnalytice.buf))
				{
					tempRcvLen = 0;
	            }
            }
			pthread_mutex_unlock(&stPort->stMutexRcv);
		}
 	
		if(stPort->stSend.len)
		{
			fd_set WriteSetFD; 
			
			FD_ZERO(&WriteSetFD); 
			FD_SET(fd, &WriteSetFD);
			select(fd + 1, NULL, &WriteSetFD, NULL, NULL);//待其写完
				
			write(fd, stPort->stSend.buf, stPort->stSend.len);
			//-------------------debug-------------------
			printf("%s S->:",stPort->portname);
			for(i=0;i<stPort->stSend.len;i++)
			{
			   if(i>40)
			   {
				   printf("...len=%d",stPort->stSend.len);
				   break;
			   }
			   else
			   {
				   printf(" %x",stPort->stSend.buf[i]);
			   }
			}
			printf("\n");	
			//-------------------------------------
			stPort->stSend.len = 0;
		}

		usleep(1000); // 1ms
	}

	close(fd);
	pthread_exit(0);
}


