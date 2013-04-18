#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

void printHelp(){
	printf("Usage: a.out serverip serverport integer\n");
	printf("\tserverip - the server's IPv4 address as a string, example: 10.0.0.1\n");
	printf("\tserverport - the server's port nr as a string\n");
	printf("\tinteger - the integer to send to the server\n");
}

int main(int argc, char *argv[])
{

	if(argc != 4){
		printHelp();
		return;
	}

	char* serverIP = argv[1];
	char* serverPortC = argv[2];
	char* integerToSendC = argv[3];

	int serverPort = atoi(serverPortC);
	int integerToSend = atoi(integerToSendC);

	if(serverPort == 0){
		printHelp();
		return;
	}

	printf("Connecting to %s:%d\nSending integer %d\n", serverIP, serverPort, integerToSend);

	// domain = AF_INET (IPv4)
	// type = SOCK_STREAM
	// 		Provides sequenced, reliable, two-way, connection-based byte streams. An out-of-band data transmission mechanism may be supported.
	// using TCP/IP
	int socketDesc = socket(AF_INET, SOCK_STREAM, 0);

	printf("socket created!\n");

	if(socketDesc < 0) {
		//error when creating socket
		printf("Error when creating socket\n");
		return;
	} 

	// creating serveraddress of type sockaddr_in
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverPort);
	inet_pton(AF_INET, serverIP, &serverAddr.sin_addr);

	printf("establishing conenction...\n");
	int connection = connect(socketDesc, (struct sockaddr*) &serverAddr, sizeof serverAddr);

	if(connection < 0) {
		//error when establishing connection
		printf("Error when establishing connection\n");
		return;
	}
	printf("connection established!\n");

	if (send (socketDesc, &integerToSend, sizeof(integerToSend), 0) == -1 ){
		//error while sending data
		printf("Error while sending integer\n");
	};
	printf("integer sent!\n");

	char buffer[128];

	int recvBytes = recv(socketDesc, buffer, sizeof buffer, 0);

	close(socketDesc);
}
