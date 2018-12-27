NAME: DIPAYAN KARMAKAR
USC ID: 7890549028
EMAIL ID: DKARMAKA@USC.EDU



This file briefs about the files submitted, insrtuctions on running the project, the way this project is executed in the programs, the format 
of all the masseges exchanged and references used to make this project.

The .zip file for this project dipayan_karmakar.zip consists of 4 file, viz. sender.cpp, receiver.cpp, Makefile and readme.txt.

To run this project, first exatract the files. 
Run the "make" command on the extracted project folder.
In the terminal window, execute "cd Desktop && sudo python project2_env.py".
After this command is done executing, run "xterm sender" and "xterm receiver" on the same terminal window.
This will open two new terminal windows, sender and receiver.
In the sender terminal window, execute "./sender".
In the receiver terminal window, execute "./receiver".
Follow the instruction on the terminal windows to test the programs.
The IP address used for receiver is "10.0.0.2" and port number used is 19028.
For sender, the IP address used is "10.0.0.1".


Description of execution of project in the two c++ files.

#1 RECEIVER:

1. 	The value of PORT is defined in the begining as 19028. The program uses two programmer defined functions, viz, "error" and "SAWUDP_receive".
2.	The error is a function that is called when there is an error in executing any other function. It takes a string input as an error tag.
	It prints the error message along with the tag and uses exit function to stop the program execution. This function is called inside the 
	SAWUDP_receive function which will be elaborated later.
3.	As global variables, there are two(2) integer variables used, viz. "receiver_sock" to store the receiver socket number we will generate
	and "n" to catch any error at any function execution. There are two(2) sockaddr_in structures, viz. "receiver" to store the address of
	the receiver and "sender" to get the address of the sender. There is one char array, "buffer", which is used to add up all the data 
	received over UDP to compile the received message. There is one unsigned int, "clength", used to store the size of a sockaddr_in structure.
4.	A structure named "packet" is created, which corresponds to a data packet sent or received over the connection. This structure contains a
	char named "data" which will contain the character being transmitted, an unsigned long int named "seq" to store the sequence number of the 
	packet being transmitted, and a char named "header" which will contain the header information of the packet.
5.	The SAWUDP_receive function contains the actual socket programming of the receiver. In the following steps, the codes in this function are
	explained.
6.	An integer variable "end" is defined and initialized with zero. This variable will indicate the completion of the message transmission.
7.	To create a receiver communication socket, the socket function is called with AF_INET as domain for IP addressing format, SOCK_DGRAM as 
	communication type for UDP communication and 0 as protocol. The returned socket number is stored in receiver_sock. If the value returned to 
	receiver_sock is less than zero, there is an error in socket creation and hence error function is called.
8.	The receiver variable is filled with data viz., AF_INET in connection family, value of PORT converted into network byte order in receiver
	port number and the IP address "10.0.0.2" in receiver IP address.
9.	The receiver socket receiver_sock is bind with the receiver address defined above. In case of an error in binding, the error function is 
	called.
10.	The user is intimated that the receiver is ready to receive connections. The buffer memory is reset to store a new message. An integer
	variable, "i", is initialized with one(1) and will be used further to indicate the sequence number of the packet being expected to receive.
11.	A while loop is started with the condition that the end variable is zero. Inside the loop two packet structure pointers named "temp" 
	and "ack_temp" are defined and memory is allocated for them. The data part of ack_temp is made NULL as any ACK does not contain any data.
12.	The recvfrom command is run to receive any incoming packet over socket receiver_sock and store it in temp. The address of the sender of 
	the packet is stored in sender. In case of an error in receiving, the error function is called.
13.	An if condition checks if the received packet header indicates that the packet is a SYN packet. If it does, another condition checks
	if the sequence number for the packet is zero(0). If it is, the ack_temp is constructed as ACK for the SYN packet and sent to the sender.
	In case of an error in sending, the error function is called. The memory allocated for temp and ack_temp is freed. The execution is 
	moved to next iteration of the while loop.
14.	An if condition checks if the received packet header indicates that the packet is a FIN packet. If it does, the ack_temp is constructed 
	as ACK for the FIN packet and sent to the sender. In case of an error in sending, the error function is called. A timeval structure 
	variable, "timeout", is defined. The tv_sec and tv_usec of the timeout are made zero(0) and 200000 respectively. Setsockopt is used to set
	a timer of 200ms for the receive operation. In case of an error in setting up timer, the error function is called. A for loop is started
	with 25 iterations in which the recvfrom operation tries to receive a packet. If any new FIN packet is received, and ack is sent 
	corresponding to that. After the for loop is complete the end variable is made one(1) and the memory allocated for temp and ack_temp is 
	freed. The execution is moved to next iteration of the while loop.
15.	An if condition checks if the received packet header indicates that the packet contains data to being transmitted. If it does, another
	condition checks if the sequence number of the received packet matches with i, the sequence number being expected. If it does, the 
	received data is added to the buffer, ACK for that packet is sent to the sender, and the value of i is increase by one(1) to expect the
	next packet. If it does not, the ACK for the last packet added to buffer is resent to the sender. The memory allocated for temp and 
	ack_temp is freed. The execution is moved to next iteration of the while loop.
16.	The while loop mentioned in step 11 extends till this step.
17.	The receiver socket is closed and the buffer is returned by the SAWUDP_receive function.
18.	The main fucntion is called containing an infinite loop, in which SAWUDP_receive is called to receive a message and that message is 
	displayed on the screen. Because of the infinite loop, the program will keep on running and receive all messages from the sender 
	untill the process is force killed.



#2 SENDER:

1. 	The value of PORT is defined in the begining as 19028. The program uses two programmer defined functions, viz, "error" and "SAWUDP_send".
2.	The error is a function that is called when there is an error in executing any other function. It takes a string input as an error tag. 
	It prints the error message along with the tag and uses exit function to stop the program execution. This function is called inside the 
	SAWUDP_send function which will be elaborated later.
3.	As global variables, there are two(2) integer variables used, viz. "sender_sock" to store the sender socket number we will generate and 
	"n" to catch any error at any function execution. There are two(2) sockaddr_in structures, viz. "receiver_address" to store the address
	of the receiver and "from" to get the address of the terminal sending the packets. There is one unsigned int, "length", used to store 
	the size of a sockaddr_in structure.
4.	A structure named "packet" is created, which corresponds to a data packet sent or received over the connection. This structure contains a
	char named "data" which will contain the character being transmitted, an unsigned long int named "seq" to store the sequence number of the 
	packet being transmitted, and a char named "header" which will contain the header information of the packet.
5.	The SAWUDP_send function contains the actual socket programming of the receiver. It takes an char pointer, "data", as an argument. In the 
	following steps, the codes in this function are explained.
6.	Two packet structure pointers named "temp" and "ack_temp" are defined and memory is allocated for them. A timeval structure variable, 
	"timeout", is defined. The tv_sec and tv_usec of the timeout are made zero(0) and 200000 respectively.
7.	To create a sender communication socket, the socket function is called with AF_INET as domain for IP addressing format, SOCK_DGRAM as 
	communication type for UDP communication and 0 as protocol. The returned socket number is stored in sender_socket. If the value returned to 
	sender_socket is less than zero, there is an error in socket creation and hence error function is called.
8.	The receiver_address variable is filled with data viz., AF_INET in connection family, value of PORT converted into network byte order in 
	receiver_address port number and the IP address "10.0.0.2" in receiver IP address.
9.	Setsockopt is used to set a timer of 200ms for the receive operation. In case of an error in setting up timer, the error function is called.
10.	The variable temp is configured into a SYN packet. An infinite while loop is started. Inside the loop the SYN packet is sent.  In case of 
	an error in sending, the error function is called. The program checks if there is a packet received in next 200ms. If yes, then it checks
	if it is the ACK for the SYN packet with sequence number zero(0). If it is, then the program breaks the infinite loop.
11.	The integer variable "i" is initialized with one(1) which indicates the sequence number of the packet being transmitted.
12.	A while loop is started with the condition that all the characters of data has been sent and acknowledged. Inside the loop, temp is 
	configured in a packet carrying characters of data. This temp is sent to the receiver. Then the program checks if there is any packet 
	received in next 200ms. If yes, then it also checks if it is an ACK of a data packet. If it is, then i is moved to the character next
	to the one acknowledged. 
13. 	The temp is then configured as a FIN packet. A for loop is started with 25 iterations. Inside the loop, the FIN packet is sent to the 
	receiver. Then the program checks if there is any packet received in next 200ms. If yes, then it also checks if it is an ACK of a FIN 
	packet. If it is, the program breaks the for loop.
14.	The memory allocated for temp and ack_temp is freed. The sender_socket is closed. The SAWUDP_send function gets finished here.
15.	The main function is called. In it, a char array, "data", of size 1024 elements, is defined and all the elements of it are made NULL. 
	An user input is taken into data for the message to be transmitted. SAWUDP_send function is called, passing data as the argument. This
	will transmit the message to the receiver. 




Declaration of references used in this project.

1.	"UDP Programming in C" by Vinod Pillai. Link to video: https://www.youtube.com/watch?v=Emuw71lozdA&t=689s
	This video is referred to create all the socket programming codes. The codes are not exactly same but will have a similar arrangement 
	and arguments used in this video.
2.	"TCP/IP Programming in C" by Vinod Pillai. Link to video: https://www.youtube.com/watch?v=V6CohFrRNTo&t=1000s
	This video is referred to for parts of the program like the programmer defined function error etc.
3.	How to set socket timeout in C when making multiple connections?. 
	Link to Website: https://stackoverflow.com/questions/4181784/how-to-set-socket-timeout-in-c-when-making-multiple-connections
	This video is reffered to the 200ms timer for the receive function.



-----------------------------------------------------------------THANK YOU-----------------------------------------------------------------
