/*
//========================================================
//TCP quiz
//========================================================
//Developed by: Kamal Panthi 
//Email: kamal210@hotmail.com
//        
//To Compile type:"gcc -g tcpquizclientserver.c -o tcpquizclientserver" 

//or Use Makefile to compile.
--> type make 


//To run server type: "./tcpquizclientserver portnumber"
//To run client type: "./tcpquizclientserver hostname portnumber nickname"



Acknowledgement:

http://www.softlab.ntua.gr/facilities/documentation/unix/unix-socket-faq/unix-socket-faq.html
http://www.hiraeth.com/alan/tutorials/courses/tcpip.html
http://www.redbooks.ibm.com/Redbooks.nsf/RedbookAbstracts/gg243376.html
http://www.tenouk.com/cnlinuxsockettutorials.html
http://www.cprogramming.com/tutorial/random.html

Stevens, W.R.: Unix Network Programming, The Sockets Networking API, Vol. 1, 3rd Ed., Prentice Hall, 2004.
Internetworking with TCP/IP Vol. 3: Client-Server Programming and Application, Linux/POSIX Socket Version (Comer, D.E., Stevens, D.), 2000. 


//======================================================== 

*/



//==================Header Declarations===================
#include <sys/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NICKLEN 10
#define QUESTION 2
#define ANSWER 4
#define HELP 6
#define QUIT 5
#define NOWORK 0

//================Global Variable Declaration=============
char splittedprevious[100], splittedafterwards[100];
int questioncount=0;
int clientcount=0;
int activecsock=0;
char activequestion[100], activeanswer[100];
bool activeqsans=false;

//=============Structure for Questions List==========================
//=======question ==========
typedef struct 
{
	int clientsock;  //client socket accepted value
	char question[100]; //question
	char answer[100];//answer

}QUESTIONS;
QUESTIONS questionslist[100];//instance for QUESTIONS

//=============Structure for Client==========================
//=======client ==========
typedef struct 
{
	char nick[NICKLEN]; 		//username
	int csock;			//usersocket
  
}CLIENTS;
CLIENTS users[10];//instance for Client Users


//========================Command Parser===================
int CommandParser(char* command)
{
	if(!strcmp(command,"QUIT")||!strcmp(command,"Quit")||!strcmp(command,"quit"))
		return QUIT;
	else if(!strcmp(command,"HELP")||!strcmp(command,"Help")||!strcmp(command,"help"))
		return HELP;	
	else if(!strncmp(command,"2",1))
		return QUESTION;
	else if(!strncmp(command,"4",1))
		return ANSWER;	
	else 
		return NOWORK;
	
}    

//==========================SPLIT STRING POSITION============
void StringSplitPosition(char* str, char delim)
{
	bzero(splittedprevious,100);  
	bzero(splittedafterwards,100);  
	int position,i=0,j=0;
	bool found=false;
	
	for(i=0; i<(strlen(str));i++)
	{
		if(str[i]== delim)
		{
			found=true;
			position=i;
			do
			{
				splittedprevious[j]=str[j];
				j++;
			}while(str[j]!=delim);

			
			do
			{		
				splittedafterwards[i-position]=str[i+1];
				i++;
			} while(i< (strlen(str)-1));
			
		}	
	}
	if(found==false)
	{
		strcpy(splittedprevious,str);
	}	
	
}

//==================add client to the structure==========
void AddClient(int csock, char* nick)
{
	users[clientcount].csock = csock;
	strcpy(users[clientcount].nick, nick);	
	clientcount++;
	printf("%s Joined the Quiz.\n",nick);
}
//==================add question and answer to the structure==========
void AddQuestion(int csock, char* question, char* answer)
{
	questionslist[questioncount].clientsock = csock;
	strcpy(questionslist[questioncount].question, question);	
	strcpy(questionslist[questioncount].answer, answer);	
	questioncount++;
	printf("%s(%s) added to the Question answer buffer.\n",question,answer);
}
//==================remove client from the structure==========
void RemoveClient(int clientposition)
{
	printf("%s left the quiz.\n",users[clientposition].nick );
	int i=0;
	if(clientcount==1 || clientposition==clientcount-1)
	{
		bzero(users[clientposition].nick,10); 
		sprintf(users[clientposition].nick,"");
		
	}
	else
	{
		for(i= clientposition; i < clientcount-1; i++)
		{
			bzero(users[i].nick,100);			
			
			users[i].csock=users[i+1].csock;
			strcpy(users[i].nick, users[i+1].nick);
			
			
		}
		bzero(users[clientcount-1].nick,10); 
		sprintf(users[clientcount-1].nick,"");
		
	}
		
	
	clientcount--;
	
}
//==================Remove question and answer from the structure==========
void RemoveQuestion(int questionposition)
{
	int i=0;
	if(questioncount==1 || questionposition==questioncount-1)
	{
		bzero(questionslist[questionposition].question,100); 
		sprintf(questionslist[questionposition].question,"");
		bzero(questionslist[questionposition].answer,100); 
		sprintf(questionslist[questionposition].answer,"");
	}
	else
	{
		for(i= questionposition; i < questioncount-1; i++)
		{
			bzero(questionslist[i].question,100);			
			bzero(questionslist[i].answer,100); 
			questionslist[i].clientsock=questionslist[i+1].clientsock;
			strcpy(questionslist[i].question, questionslist[i+1].question);
			strcpy(questionslist[i].answer, questionslist[i+1].answer);
			
		}
		bzero(questionslist[questioncount-1].question,100); 
		sprintf(questionslist[questioncount-1].question,"");
		bzero(questionslist[questioncount-1].answer,100); 
		sprintf(questionslist[questioncount-1].answer,"");
	}
		
	questioncount--;
	
}
//===========Get active question, answer, csock==========
void ActiveCredentials()
{
	if(questioncount == 0)
	{
		bzero(activequestion,100);
		sprintf(activequestion,"1Enter new question");
		bzero(activeanswer,100);
		sprintf(activeanswer,"");
		activecsock=0;
		activeqsans=false;
		
					
	}
	else if(questioncount == 1)
	{
		bzero(activequestion,100);
		bzero(activeanswer,100);
		sprintf(activequestion,"3%s",questionslist[0].question);		
		strcpy(activeanswer, questionslist[0].answer);
		activecsock= questionslist[0].clientsock;
		RemoveQuestion(0);		
		activeqsans=true;
					
	}
	else
	{
		int randomquestion=0;		
		randomquestion = 1+rand()%(questioncount);
		printf("Randomly selected question number is %d\n",randomquestion);
		bzero(activequestion,100);
		bzero(activeanswer,100);
		sprintf(activequestion,"3%s",questionslist[randomquestion-1].question);		
		strcpy(activeanswer, questionslist[randomquestion-1].answer);
		activecsock= questionslist[randomquestion-1].clientsock;
		RemoveQuestion(randomquestion-1);		
		activeqsans=true;

	}	
}

//=======================Send Message to all=============
void SendMsgToAll(char* message)
{
	int i=0;
	for(i = 0; i <= clientcount; i++)
	{
	
		send(users[i].csock,message,strlen(message),0);	
	
	}
}

//=======================Send Message to all except one=============
void SendMsgToAllExceptOne(char* message, int csock)
{
	int i=0;
	for(i = 0; i <= clientcount; i++)
	{
		if(csock!=users[i].csock)		
			send(users[i].csock,message,strlen(message),0);	
		
	
	}
}

//=======================Error display Function=============
void err(char *msg)
{
	perror(msg);
	exit(0);
}

//======================MAIN==============================
int main(int argc, char *argv[])
{
	fd_set fd_readfd,fd_testfd;//file descripter set for select
	int fd_socket, length,c, n,r,len,check,s,lenm, socknum;// integer variables
	struct sockaddr_in ser_sock, cli_sock;//sockaddress for server and client
	struct hostent *hp;//structure used for the entry in the host database
	char buffer[512],sendb[512],receiveb[512]; //String buffers 
	int PORT; //integer variable for portno
	char NICK[10];// character array to store nick name  
	
	struct timeval timerout;
	timerout.tv_sec = 100000000;//initializaztion of timer...initialized large value for listening first client for longer period
	timerout.tv_usec = 500000;

	if(argc!=2 && argc!=4)
	{
      		fprintf(stderr, "Usage:\n   Server: ./tcpquizclientserver portnumber\n   Client: ./tcpquizclientserver hostname portnumber nickname\n");
      		exit(0);
   	}

	else if(argc==2)//===========SERVER CODE====================
	{
		//========Check error in port=========
		if( (PORT=atoi(argv[1]) ) <=0 )	
			err("\n server: Port Error\n");				
		

		//========creating socket for communication=================================
   		fd_socket= socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
   		if (fd_socket < 0) 
			err("socket");

		//============fill the address buffer========================================
		memset(&ser_sock, 0, sizeof(ser_sock));				
		ser_sock.sin_family = AF_INET;
		ser_sock.sin_addr.s_addr = htonl(INADDR_ANY);
		ser_sock.sin_port = htons(PORT);
		length=sizeof(struct sockaddr_in);

		//===========================Binding socket to the Port====================================
		if (bind(fd_socket, (struct sockaddr *) &ser_sock, sizeof(ser_sock)) < 0)
			err("server: Binding Error\n");
		

		//===========================Listen for the incoming connection==================
		if (listen(fd_socket, 10) < 0)	
			err("server: Failed to Listen\n");
				
		//==================Get Active QS, Ans and Active qs client==============================
		ActiveCredentials();

		//=============================fill the select macros==============================
		FD_ZERO(&fd_readfd);						
		FD_SET(fd_socket,&fd_readfd);
		FD_SET(0,&fd_readfd);
		
		while(1)
		{
			//set the select command
			fd_testfd=fd_readfd;
			s=select(FD_SETSIZE,&fd_testfd,0,0,&timerout);	
			if(s<0)
				err("server: error in select\n");
			else if(s==0)
			{
				printf("Question Timer Exceeded: 120 secs.\n");

				bzero(sendb, 512);
				if(questioncount>0)
				{				
					sprintf(sendb, "120 seconds elapsed since last question '%s'.\nThe correct answer is: %s.\n",activequestion, activeanswer);
				}
				else 
				{
					if(strcmp(activequestion,"1Enter new question")==0)
						sprintf(sendb, "120 seconds elapsed.\n");
					else
						sprintf(sendb, "120 seconds elapsed since last question '%s'.\nThe correct answer is: %s.\n",activequestion, activeanswer);
				}

				//============Send it to all===========================
				SendMsgToAll(sendb);				
				activeqsans=false;

				//============Get another question if avialable=======	
				ActiveCredentials();
				bzero(sendb, 512);
				sprintf(sendb,"%s\n",activequestion);
				SendMsgToAllExceptOne(sendb, activecsock);
				bzero(sendb, 512);
				sprintf(sendb,"Your question '%s' is active now!!",activequestion);
				send(activecsock,sendb,strlen(sendb),0);

				//=======QUESTION is asked NOW RESET TIMER TO 2 MINUTES============
				timerout.tv_sec = 120;
				timerout.tv_usec = 500000;
			}
			else
			{
				//========NEW CONNECTION=======================
				if(FD_ISSET(fd_socket,&fd_testfd))
				{
					
					//===========================Accept Incoming Connections===============================

					if( (socknum=accept(fd_socket,(struct sockaddr *) &cli_sock,&length)) <0)	
						err("server: Accept Error\n");
					
					bzero(receiveb,512);	
					n=recv(socknum,receiveb,512,0);					
					receiveb[n]=0;
					if (n < 0) 
						err("recv");
					printf("Received new connection from Client: '%s'\n",receiveb);

					//========Checking number of clients exists============	
					if(clientcount>=10)//if already 10
					{
						//Send the quiz room is packed message to newly connected client only
						bzero(sendb, 512);
						sprintf(sendb,"Sorry No room avialable!!");
						send(socknum,sendb,strlen(sendb),0);						
						printf("Rooms packed message sent to: %s\n",receiveb);
						
					}
					else//if users less than 10
					{
						AddClient(socknum, receiveb);//Add client to structure						
						
						//Send the welcome message client
						bzero(sendb, 512);
						sprintf(sendb,"0Welcome to the game\n%s",activequestion);
						FD_SET(socknum,&fd_readfd);
						send(socknum,sendb,strlen(sendb),0);					

				
						//Send the new client joined message to other client
						bzero(sendb, 512);
						sprintf(sendb,"%s Joined the Quiz.",receiveb);
						SendMsgToAllExceptOne(sendb, socknum);
						
					}				
					
				}
				else
				{
					int clientcounter=0;
					while(clientcounter<clientcount)
					{
						if(FD_ISSET(users[clientcounter].csock, &fd_testfd))//if something received from a client
						{
							
							bzero(receiveb,512);	
							n=recv(users[clientcounter].csock,receiveb,512,0);					
							receiveb[n]=0;
							if (n <= 0) //client terminated...connection lost from client
							{
								char disconnectednick[10];
								bzero(disconnectednick,10);
								sprintf(disconnectednick,"%s",users[clientcounter].nick);			
								FD_SET(users[clientcounter].csock,&fd_readfd);//removing from select
								//remove client from structure
								RemoveClient(clientcounter);
								bzero(sendb, 512);
								sprintf(sendb,"%s Logged off.",disconnectednick);
								SendMsgToAll(sendb);
								clientcounter--;
							}
							else
							{
								printf("Received from Client: '%s'\n",receiveb);
								c=CommandParser(receiveb);
								if(c==QUESTION)
								{
									char tempqs[512],tempans[512];
									bzero(tempqs,512);				
									int x=0;
									for(x=0; x<(strlen(receiveb));x++)
									{	
										if(receiveb[x]=='2')
										{
											int ttt = x;
											
											do
											{		
												tempqs[x-ttt]=receiveb[x+1];
												x++;
											} while(receiveb[x]!='?');
											x=strlen(receiveb);
					
										}
					
									}

									

									bzero(tempans,512);
									for(x=0; x<(strlen(receiveb));x++)
									{	
										if(receiveb[x]=='(')
										{
											int ttt = x;
											
											do
											{		
												tempans[x-ttt]=receiveb[x+1];
												x++;
											} while(receiveb[x+1]!=')');
											x=strlen(receiveb);
					
										}
					
									}

									//add question to the client
									AddQuestion(users[clientcounter].csock, tempqs, tempans);
									if(questioncount==1 && activeqsans == false)
									{
										//============Get another question if avialable=======	
										ActiveCredentials();
										bzero(sendb, 512);
										sprintf(sendb,"%s\n",activequestion);
										SendMsgToAllExceptOne(sendb, activecsock);
										bzero(sendb, 512);
										sprintf(sendb,"Your question '%s' is active now!!",activequestion);
										send(activecsock,sendb,strlen(sendb),0);

										//=======QUESTION is asked NOW RESET TIMER TO 2 MINUTES============
										timerout.tv_sec = 120;
										timerout.tv_usec = 500000;
									}
							
								}
								else if(c==ANSWER)
								{
									char tempans[512];
									bzero(tempans,512);
									int x=0;
									for(x=0; x<(strlen(receiveb));x++)
									{	
											
										tempans[x]=receiveb[x+1];
												
									}
									bzero(sendb, 512);
									if(strcmp(tempans,activeanswer)==0)//Correct answer received
									{
										sprintf(sendb, "Question: %s\nAnswer: %s\nCorrect Answer Received from: %s\n",activequestion, activeanswer, users[clientcounter].nick);
										SendMsgToAll(sendb);
										//============Get another question if avialable=======	
										ActiveCredentials();
										bzero(sendb, 512);
										sprintf(sendb,"%s\n",activequestion);
										SendMsgToAllExceptOne(sendb, activecsock);
										bzero(sendb, 512);
										sprintf(sendb,"Your question '%s' is active now!!",activequestion);
										send(activecsock,sendb,strlen(sendb),0);

										//=======QUESTION is asked NOW RESET TIMER TO 2 MINUTES============
										timerout.tv_sec = 120;
										timerout.tv_usec = 500000;
										

									}
									else//InCorrect answer received
									{
										sprintf(sendb, "Incorrect Answer Received!!");
										send(users[clientcounter].csock,sendb,strlen(sendb),0);
									}
								}
								else
								{
									//do nothing
								}
								
							
						
							}

					
						}

						clientcounter++;
					}

				}
			}

		}//end of while


	
	}
	else//===========CLIENT CODE====================
	{
		//======Check the host server=========
		hp = gethostbyname(argv[1]);
   		if (hp==0) 
			err("Unknown host");
  		
		//========Check error in port=========
		if( (PORT=atoi(argv[2]) ) <=0 )	
			err("\n Port Error\n");	
			
		//========Check for nick length========
		if(strlen(argv[3])>10) 
			err("\n Please provide nick not more than 10 characters.\n");	 	
		bzero(NICK,10);
		strcpy(NICK,argv[3]);

		//========creating socket for communication=================================
   		fd_socket= socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
   		if (fd_socket < 0) 
			err("socket");

		//============fill the address buffer========================================
		memset(&ser_sock, 0, sizeof(ser_sock));				
		ser_sock.sin_family = AF_INET;
		bcopy((char *)hp->h_addr,(char *)&ser_sock.sin_addr,hp->h_length);	
		//ser_sock.sin_addr = *((struct in_addr *)hp->h_addr);
		//ser_sock.sin_addr.s_addr = htonl(INADDR_ANY);
		ser_sock.sin_port = htons(PORT);
		length=sizeof(struct sockaddr_in);
		
		//=============Connect()=====================================
		if (connect(fd_socket, (struct sockaddr *)&ser_sock, sizeof(struct sockaddr)) < 0) 
			err("Connect");
		
		bzero(sendb, 512);
		sprintf(sendb,"%s",NICK);		
		if(send(fd_socket,sendb,strlen(sendb),0)<0)	
			err("Send Error");

		
		//=============================fill the select macros==============================
		FD_ZERO(&fd_readfd);						
		FD_SET(fd_socket,&fd_readfd);
		FD_SET(0,&fd_readfd);

		//=============================Program Header==============================
		printf("\n ========================================");
		printf("\n 	   TCPQUIZ HELP.");
		printf("\n ========================================");
		printf("\n <quit> To Disconnect.");
		printf("\n <help> To View Help Message.");
		printf("\n <Developed by: Kamal Panthi c0346747 >");
		printf("\n ========================================\n");

		while(1)
		{
			
			fd_testfd=fd_readfd;
			s=select(FD_SETSIZE,&fd_testfd,0,0,&timerout);	
			if(s<0)
				err("error in select\n");

			//check if input came from console keyboard
			if(FD_ISSET(0,&fd_testfd))
			{        	
	   			bzero(buffer,512);   	
	   			fgets(buffer,511,stdin);   	
	    			len=strlen(buffer)-1;
	    			buffer[len]=0;
			
				check=CommandParser(buffer);//check for user commands(function called)


				if(check==QUESTION)
				{
					
					StringSplitPosition(buffer,'?');
					if(strlen(splittedafterwards)!=0)
					{
						StringSplitPosition(buffer,'(');
						if(strlen(splittedafterwards)!=0)
						{
							StringSplitPosition(buffer,')');
							if(strlen(splittedprevious)< strlen(buffer))
							{
							
								

								bzero(sendb,512); 
								sprintf(sendb,"%s",buffer);
								if(send(fd_socket,sendb,strlen(sendb),0)<0)	
									err("Send Error");
							}
							else
							{
								printf("\n Please provide Question and answer in this format <2Question?(Answer)>\n");		
							}
						
						}
						else
						{
							printf("\n Please provide Question and answer in this format <2Question?(Answer)>\n");		
						}
					}
					else
					{
						printf("\n Please provide Question and answer in this format <2Question?(Answer)>\n");		
					}
					
				
				}
				else if(check==ANSWER)
				{

					bzero(sendb,512); 
					sprintf(sendb,"%s",buffer);
					if(send(fd_socket,sendb,strlen(sendb),0)<0)	
						err("Send Error");

					
				}
				else if(check==QUIT)//if the user want to quit
				{
					
					printf("You are disconnected from the TCP QUIZ.\n");
					exit(1);				
				}

				else if(check==HELP)//if the user want help
				{
					printf("\n ========================================");
	   				printf("\n 	   TCPQUIZ HELP.");
					printf("\n ========================================");
					printf("\n <quit> To Disconnect.");
					printf("\n <help> To View Help Message.");
					printf("\n <Developed by: Kamal Panthi c0346747 >");
					printf("\n ========================================");
					printf("\nThis is your help text...sorry i didnt have time to write long text...so please forgive me. \n");
					continue;	
				}
				else
				{
					printf("\n Unrecognized input received from Keyboard! Please try again.\n");	
				}						
							
			}//end of FD_ISSET from console
			
			else if(FD_ISSET(fd_socket, &fd_testfd))//Received something from socket
			{
				bzero(receiveb,512);	
				n=recv(fd_socket,receiveb,512,0);					
				receiveb[n]=0;
				if (n <= 0) 
					err("recv error");
				else
				{
					printf("%s\n", receiveb);
					if(strcmp(receiveb,"Sorry No room avialable!!")==0)
						exit(0);
				}
				  
				   
			}
		
			
			 
		}
	}

	return 0;
        

       
	
	
	

   
}





 
