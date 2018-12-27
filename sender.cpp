/*
This is the sender program. Refer to Readme file for the detailed process 
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


#define PORT 19028	//The port number of receiver for the UDP communication is defined as 19028

using namespace std;	


int sender_socket, n;
struct sockaddr_in receiver_address, from;		//Address variables to store addresses of receiver and the received packet sender
unsigned int length = sizeof(struct sockaddr_in);

struct packet{
char data;	//The data part of packet carrying the character in this project
unsigned long int seq;	//32 bit sequence number space
char header;};		//8 bit header space containing last 3 bits as ACK, SYN and FIN respectively
/*
Function to display error and stop execution in case of an error
reference: https://www.youtube.com/watch?v=V6CohFrRNTo&t=52s
*/
void error(char *msg)
{
perror(msg);		//Display error
exit(0);		//Stop program execution
}

/*
SAWUDP_send function: This function contains the entire operation of UDP,
starting from establishing the socket, sending the data and closing the socket
*/
void SAWUDP_send(char* data){

	struct packet* temp;	//Used to prepare any packet that needs to be sent
	struct packet* ack_temp;	//Used to store any packet received over UDP
	struct timeval timeout;		//Variable to create delay
	timeout.tv_sec=0;		//Delay in seconds 0
	timeout.tv_usec=200000;		//Delay in microseconds 200000
	ack_temp= (struct packet*)malloc(sizeof(struct packet));	//Memory allocation for ack_temp
	temp= (struct packet*)malloc(sizeof(struct packet));	//Memory allocation for temp

	sender_socket = socket(AF_INET, SOCK_DGRAM, 0);	//Creating a UDP sender socket
	if (sender_socket <0){					//Checking if there is a socket creation error
		error("Error opening SOCKET");
	}

	receiver_address.sin_family= AF_INET;		//Defining receiver domain of the communication
	receiver_address.sin_port = htons(PORT);		//Setting the receiver port number for communication
	inet_aton("10.0.0.2", &(receiver_address.sin_addr));	//Using 10.0.0.2 as receiver's IP address
	

	if(setsockopt(sender_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout))<0){		//Setting up timer for UDP receive function
		error("Error Timer Setup");
	}

	/*
	Configuring temp to make a Synchronization packet, to be sent to check UDP connection
	*/
	temp->seq=0;
	temp->header=0x02;
	temp->data=NULL;

	while(1){	//Loop to send SYN packet untill ack for it is received
	
		printf("Establishing a connection to receiver... (sending SYN)\n");
		n=sendto(sender_socket, temp, sizeof(struct packet), 0, (struct sockaddr *)&receiver_address, length);	//Sending SYN packet
		if(n==-1){		//Checking if there is a sending error
		       	error("Sending Error");
		}

		n=recvfrom(sender_socket, ack_temp, sizeof(struct packet), 0, (struct sockaddr *)&from, &length);	//Checking if ack for SYN packet has been received
		if(n<0){		//Checking if there was a receiving timeout
		       	printf("Window timed-out...\n");
		}
		else{
			if((ack_temp->header== 0x06) && (ack_temp->seq==0)){	//Checking if the ack received was ack for SYN packet and had sequence number 0
				printf("ACK Received with SEQ# 0\n");
				break;
			}
			else{
				printf("Header or sequence mismatch");
			}
		}
	}


	int i=1;	//Variable i represent sequence number of packet to be sent
	while(data[i-1]!='\0' && data[i-1]!='\n'){	//Loop to send all the characters untill all of them are acknowledged

		temp->data=data[i-1];	//Storing character to be sent in temp
		temp->seq=i;	//Setting up sequence number
		temp->header=0x00;	//Setting up header

		printf("Sending character \"%c\"\n",data[i-1]);
		n=sendto(sender_socket, temp, sizeof(struct packet), 0, (struct sockaddr *)&receiver_address, length);	//Sending data packet
		if(n==-1){		//Checking if there is a sending error
		       	error("Sending Error");
		}


		n=recvfrom(sender_socket, ack_temp, sizeof(struct packet), 0, (struct sockaddr *)&from, &length);	//Checking if ack for DATA packet has been received
		if(n<0){		//Checking if there was a receiving timeout
		       	printf("Window timed-out...\n");
		}
		else{
			if(ack_temp->header== 0x04){	//Check if the the packet was an ack for data packet 
				i=ack_temp->seq+1;		//Move sequence number pointer to the packet next to the one acknowledged
			printf("ACK Received with SEQ# %d\n",ack_temp->seq);
			}
		}
	}

	/*
	Data transmission is complete. Now we need to send FIN packet and then close the socket.
	*/
	printf("\nTerminating connection... (sending FIN)\n");

	/*
	Configuring temp to make a Finishing packet, to be sent to terminate the UDP connection
	*/
	temp->data=NULL;
	temp->seq=i;
	temp->header=0x01;	//Header will have the FIN bit high

	for(int j=0;j<25;j++){

		n=sendto(sender_socket, temp, sizeof(struct packet), 0, (struct sockaddr *)&receiver_address, length);	//Sending FIN packet
		if(n==-1){		//Checking if there is a sending error
		       	error("Sending Error");
		}


		n=recvfrom(sender_socket, ack_temp, sizeof(struct packet), 0, (struct sockaddr *)&from, &length);	//Checking if ack for FIN packet has been received
		if(n<0){		//Checking if there was a receiving timeout
		       	printf("Window timed-out...\n");
		}
		else{
			if(ack_temp->header == 0x05){	//Checking if the ack received was the ack for FIN packet
			printf("ACK Received with SEQ# %d\n",ack_temp->seq);
			break;
			}
		}
	}

	free(temp);		//Free the memory allocated for temp
	free(ack_temp);		//Free the memory allocated for ack_temp

	printf("Done\n");
	close(sender_socket);		//Close the communication socket
}


int main()
{
	char data[1024];
	memset(data, 0 , sizeof(data));	//Clear buffer memory to store a new input
	printf("Please provide a string with at least 20 characters:\n");

	fgets(data, 1024, stdin);	//Reading user input to buffer

	SAWUDP_send(data);	//Call SAWUDP_send to send the user input string to the receiver


return 0;
}
