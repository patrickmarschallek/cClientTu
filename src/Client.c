#include <stdio.h>
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

	/* ----------- Creating socket --------------- */
	
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
	
	/* ----------- Sending integer --------------- */
	
	// integerToSendH = integerToSend in network format
	int integerToSendN = (int) htonl(integerToSend);

	if (write(socketDesc, &integerToSendN, sizeof(integerToSendN)) == -1 ){
		//error while sending data
		printf("Error while sending integer\n");
	};
	printf("integer sent!\n");

	
	/* ----------- Receiving integer & string --------------- */
	
	int32_t intBuffer;
	
	if(recv(socketDesc, &intBuffer, sizeof intBuffer, 0) == -1){
		//error while sending data
		printf("Error while retrieving Integer\n");
	}

    int receivedInt = ntohl(intBuffer);

    printf("Received int: %d\n", receivedInt);

	char cBuffer[40];
	
	if (read(socketDesc, &cBuffer, sizeof cBuffer) == -1){
		//error while sending data
		printf("Error while retrieving String\n");
	}

	//Getting actual length of the String
	//Checking every second value, because Java writes chars in 16 bits
	int lengthOfString = 0;
	int i;
	for(i  = 1; i < sizeof cBuffer; i += 2){
		if(cBuffer[i] == 0){
			break;
		}
	}

	char receivedString[i/2];

	for(i = 0; i < sizeof receivedString; i++){
		receivedString[i] = cBuffer[(i + 1) * 2 - 1];
	}

	printf("received String: \"%s\" \n", receivedString);

	/* ----------- Changing cases in String --------------- */
	
	for(i = 0; i < sizeof receivedString; i++){
		if(receivedString[i] >= 65 && receivedString[i] <=90){
			receivedString[i] += 32;
		} else if (receivedString[i] >= 97 && receivedString[i] <= 122){
			receivedString[i] -= 32;
		}
	}

	printf("converted String: \"%s\" \n", receivedString);
	
	// /* ----------- Sending String --------------- */

	if (write(socketDesc, &receivedString, sizeof(receivedString)) == -1 ){
		//error while sending data
		printf("Error while sending String\n");
	};
	printf("String sent!\n");
	
	close(socketDesc);
}
