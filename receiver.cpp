/*
This is the receiver program. Refer to Readme file for the detailed process 
*/

// List of header files

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <arpa/inet.h>

#define PORT 19028	//The port number for the UDP communication is defined as 19028

using namespace std;


int receiver_sock, n;
struct sockaddr_in receiver, sender;		//Address variables to store addresses of receiver and the received packet sender
char buffer[1024];		//String to store the Data received over UDP
unsigned int clength=sizeof(struct sockaddr_in);


struct packet{
char data;	//The data part of packet carrying the character in this project
unsigned long int seq;	//32 bit sequence number space
char header;};		//8 bit header space containing last 3 bits as ACK, SYN and FIN respectively


/*
Function to display error and stop execution in case of an error
reference: https://www.youtube.com/watch?v=V6CohFrRNTo&t=52s
*/
void error(char *msg){
	perror(msg);		//Display error
	exit(0);		//Stop program execution
}


/*
SAWUDP_receive function: This function contains the entire operation of UDP,
starting from establishing the socket, receiving the data, closing the socket and returning the data to main
*/
char* SAWUDP_receive(){

	int end=0;	//Variable used to indicate end of the data transmission

	receiver_sock= socket(AF_INET, SOCK_DGRAM, 0);	//Creating a UDP receiver socket
	if(receiver_sock<0){				//Checking if there is a socket creation error
		error("Socket Creation error");
	}



	receiver.sin_family= AF_INET;		//Defining receiver domain of the communication
	receiver.sin_port= htons(PORT);		//Setting the receiver port number for communication
	inet_aton("10.0.0.2", &(receiver.sin_addr));	//Using 10.0.0.2 as receiver's IP address


	n= bind(receiver_sock, (struct sockaddr *)&receiver, sizeof(receiver));	//Bind receiver socket with receiver address
	if(n==-1){		//Checking if there is a binding error
        	error("Binding Error");
	}


	printf("Receiver is running and ready to receive connections on port %d:\n",PORT);

	memset(buffer, 0 , sizeof(buffer));	//Clear buffer memory to store a new input

	int i=1;	//Start the sequence from first packet
	while(end==0){		//Run loop until the transmission is terminated by user
		
		struct packet* temp;	//Used to store any packet received over UDP
		struct packet* ack_temp;	//Used to prepare any packet that needs to be sent
		temp= (struct packet*)malloc(sizeof(struct packet));	//Memory allocation for temp
		ack_temp= (struct packet*)malloc(sizeof(struct packet));	//Memory allocation for ack_temp
		ack_temp->data=NULL;

		n=recvfrom(receiver_sock, temp, sizeof(struct packet), 0, (struct sockaddr *)&sender, &clength);	//Receive any packet sent by the sender
		if(n==-1){		//Checking if there is a receiving error
	        	error("Receiving Error");
		}

		if(temp->header == 0x02){	//Check if the received packet was a SYN packet
			if(temp->seq==0){
				printf("Connection request received from <IP Address: %s, Port # %d>\n", inet_ntoa(sender.sin_addr), ntohs(sender.sin_port));
				printf("Sending ACK with SEQ # 0, expecting SEQ # 1\n");
				/*
				Configuring ack_temp to make a acknowledgement packet for the received SYN packet
				*/
				ack_temp->seq=0;
				ack_temp->header=0x06;	//Setting up the SYN bit and ACK bit high, and the FIN bit low
				n=sendto(receiver_sock, ack_temp, sizeof(struct packet), 0, (struct sockaddr *)&sender, clength);	//Sending SYN ACK to sender
				if(n==-1){		//Checking if there is a sending error
        				error("Sending Error");
				}
			}
			else{
				printf("Sequence mismatch\n");
			}
			//Free the memory allocated for temp and ack_temp
			free(temp);
			free(ack_temp);
			continue;	//Ignore other if conditions and go for receiving next packet is required
		}

		if(temp->header == 0x01){	//Check if the received packet was a FIN packet
			printf("\nSender is terminating with FIN...\n");
			/*
			Configuring ack_temp to make a acknowledgement packet for the received FIN packet
			*/
			ack_temp->seq = temp->seq;
			ack_temp->header=0x05;		//Setting up the FIN and ACK bit high and the SYN bit low
			ack_temp->data=NULL;
			printf("Sending ACK with SEQ# %d\n", ack_temp->seq);
			n=sendto(receiver_sock, ack_temp, sizeof(struct packet), 0, (struct sockaddr *)&sender, clength);	//Sending ACK for the FIN bit
			if(n==-1){		//Checking if there is a sending error
        			error("Sending Error");
			}

			struct timeval timeout;	//Variable to create delay
			timeout.tv_sec=0;		//Delay in seconds 0
			timeout.tv_usec=200000;		//Delay in microseconds 200000
			if(setsockopt(receiver_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout))<0){		//Setting up timer for UDP receive function
				error("Error Timer Setup");
			}

			for(int j=0; j<25; j++){
				n=recvfrom(receiver_sock, temp, sizeof(struct packet), 0, (struct sockaddr *)&sender, &clength);	//Wait for 5 seconds and try to receive any packet sent
				if(n>0 && temp->header==0x01){		//Checking if a packet is received and if it is a FIN packet
					printf("Sending ACK with SEQ# %d\n", ack_temp->seq);
					n=sendto(receiver_sock, ack_temp, sizeof(struct packet), 0, (struct sockaddr *)&sender, clength);	//Send ack for the received FIN bit
					if(n==-1){		//Checking if there is a sending error
        					error("Sending Error");
					}
				}
			}
			end=1;		//Toggle the end variable to stop further reception of data
			//Free the memory allocated for temp and ack_temp
			free(temp);
			free(ack_temp);
			continue;	//Ignore other if conditions and go for receiving next packet is required
		}

		if(temp->header == 0x00){	//Check if the received packet was a data packet
			if(temp->seq==i){	//Check if the data for the currently required sequence number was sent
				buffer[i-1]=temp->data;		//Add the received character to the buffer
				printf("Received character \"%c\"\n", temp->data);
				/*
				Configuring ack_temp to make a acknowledgement for the data packet received
				*/
				ack_temp->seq=temp->seq;	//Used received packet sequence number as seq for the ack packet
				ack_temp->header=0x04;		//Set ACK high and FIN and SYN low
				ack_temp->data=NULL;
				printf("Sending ACK with SEQ# %d, expecting SEQ # %d\n", ack_temp->seq, ack_temp->seq+1);
				n=sendto(receiver_sock, ack_temp, sizeof(struct packet), 0, (struct sockaddr *)&sender, clength);	//Send the ACK for the received data packet
				if(n==-1){		//Checking if there is a sending error
        				error("Sending Error");
				}
				i++;	//Move to next character
			}
			else{	//Wrong packet received
				/*
				Configuring ack_temp to acknowledge the last matching data packet received
				*/
				ack_temp->seq=i-1;		//Set sequence to previous window number
				ack_temp->header=0x04;		//Set ACK high and FIN and SYN low
				ack_temp->data=NULL;
				printf("Sending ACK with SEQ# %d, expecting SEQ # %d\n", ack_temp->seq, ack_temp->seq+1);
				n=sendto(receiver_sock, ack_temp, sizeof(struct packet), 0, (struct sockaddr *)&sender, clength);	//Send the ACK for the last matching data packet received
				if(n==-1){		//Checking if there is a sending error
        				error("Sending Error");
				}
			}
			//Free the memory allocated for temp and ack_temp
			free(temp);
			free(ack_temp);
			continue;	//Ignore other if conditions and go for receiving next packet is required
		}
	}

	close(receiver_sock);	//Close the receiver socket
	return buffer;		//Returned the received data to the main function
}

int main()
{
	while(1){
		char* str = SAWUDP_receive();		//Call SAWUDP_receive to get the string sent
		cout<<"Reception Complete:\n\""<<str<<"\"\n\n";
	}
return 0;
    
}

