#include "Connection.h"


Connection::Connection(){

}

Connection::~Connection(){

}

int Connection::createSocket(){
	/* ----------- Creating sockets --------------- */

	// domain = AF_INET (IPv4)
	// type = SOCK_STREAM
	// 		Provides sequenced, reliable, two-way, connection-based byte streams. An out-of-band data transmission mechanism may be supported.
	// using TCP/IP
	this->senderSocket = socket(AF_INET, SOCK_STREAM, 0);
	this->receiverSocket = socket(AF_INET, SOCK_STREAM, 0);

	printf("socket created!\n");

	if(this->senderSocket < 0 || this->receiverSocket < 0) {
		//error when creating socket
		printf("Error when creating sockets\n");
		return 0;
	} 
}

int Connection::startConnection(){
		// creating serveraddress of type sockaddr_in
	struct sockaddr_in serverAddrSender;
	serverAddrSender.sin_family = AF_INET;
	serverAddrSender.sin_port = htons(senderPort);
	inet_pton(AF_INET, serverIP, &serverAddrSender.sin_addr);

	struct sockaddr_in serverAddrReceiver;
	serverAddrReceiver.sin_family = AF_INET;
	serverAddrReceiver.sin_port = htons(receiverPort);
	inet_pton(AF_INET, serverIP, &serverAddrReceiver.sin_addr);

	// printf("establishing conenction...\n");
	int sendConnection = connect(senderSocket, (struct sockaddr*) &serverAddrSender, sizeof serverAddrSender);
	int recvConnection = connect(receiverSocket, (struct sockaddr*) &serverAddrReceiver, sizeof serverAddrReceiver);

	if(sendConnection < 0 || recvConnection < 0) {
		//error when establishing connection
		printf("Error when establishing connection\n");
		return;
	}
	printf("connection established!\n");
}

int Connection::sendInt(){
	/* ----------- Sending integer --------------- */
	
	// integerToSendH = integerToSend in network format
	int32_t integerToSendN = (int) htonl(integerToSend);
	int index;
	int empty = 0;


	printf("sending integer %s\n", integerToSendC);
	if (write(senderSocket, &integerToSendN, sizeof integerToSendN) == -1 ){
		//error while sending data
		printf("Error while sending String\n");
	};
	printf("integer sent!\n");
	return 0;
}

int Connection::receive(){
		/* ----------- Receiving integer & string --------------- */
#ifdef __gnu_linux__
	int32_t intBuffer = 0;
	
	int receivedBytes = 0;
	int32_t receivedInteger = 0;
#else
	const int BUF_SIZE = 4;
	char intBuffer[BUF_SIZE];
	
	int receivedBytes = 0;
	int32_t receivedInteger = 0;
#endif

	while(receivedBytes < 4){
#ifdef __gnu_linux__
		int readBytes = read(
			receiverSocket, &intBuffer2, 
			(sizeof intBuffer2) - receivedBytes2, receivedBytes2);
		receivedInteger += (intBuffer << (8*receivedBytes));
		receivedBytes += readBytes;
#else
		int readBytes = recv(
			receiverSocket, intBuffer, BUF_SIZE,MSG_OOB);
		/* TODO
		receivedInteger2 += (intBuffer2 << receivedBytes2);
		receivedBytes2 += readBytes;*/
#endif
		
	}

    int32_t receivedInt = ntohl(receivedInteger);
    printf("Received int: %d\n", receivedInt);

#ifdef __gnu_linux__
	int32_t intBuffer2 = 0;
	
	int receivedBytes2 = 0;
	int32_t receivedInteger = 0;
#else
	char intBuffer2[BUF_SIZE];
	
	int receivedBytes2 = 0;
	int32_t receivedInteger2 = 0;
#endif

	while(receivedBytes2 < 4){
#ifdef __gnu_linux__
		int readBytes = read(
			receiverSocket, &intBuffer2, 
			(sizeof intBuffer2) - receivedBytes2, receivedBytes2);
		receivedInteger2 += (intBuffer2 << (8*receivedBytes2));
		receivedBytes2 += readBytes;
#else
		int readBytes = recv(
			receiverSocket, intBuffer, BUF_SIZE,MSG_OOB);
		/* TODO
		receivedInteger2 += (intBuffer2 << receivedBytes2);
		receivedBytes2 += readBytes;*/
#endif	
	}

    int32_t receivedCount = ntohl(receivedInteger2);

    if(receivedCount > 20 || receivedCount < 0){
    	printf("Received wrong count\n");
#ifdef __gnu_linux__
			close(senderSocket);
			close(receiverSocket);
#else
			closesocket(senderSocket);
			closesocket(receiverSocket);
#endif

		return;
    }

	char cBuffer[receivedCount];
	char recvString[receivedCount];
	
	receivedBytes = 0;
	while(receivedBytes < receivedCount){
		int readBytes = read(receiverSocket, &cBuffer, (sizeof cBuffer) - receivedBytes, receivedBytes);
		int j;
		for(j = 0; j < readBytes; j++){
			recvString[j + receivedBytes] = cBuffer[j];
		}
		receivedBytes += readBytes;
	}
	recvString[receivedCount] = '\0';

	printf("received String: \"%s\"\n", recvString);

	//  ----------- Changing cases in String --------------- 
	int i;
	for(i = 0; i < sizeof recvString; i++){
		if(recvString[i] >= 65 && recvString[i] <=90){
			recvString[i] += 32;
		} else if (recvString[i] >= 97 && recvString[i] <= 122){
			recvString[i] -= 32;
		}
	}

	printf("converted String: \"%s\" \n", recvString);
}

int Connection::sendString(){
	/* ----------- Sending String --------------- */
	int receivedBytes = 0;
	while(receivedBytes < receivedCount){
		receivedBytes += write(senderSocket, &recvString, (sizeof recvString) - receivedBytes, receivedBytes);
	}
	write(senderSocket, &lineEnd, sizeof lineEnd);
	printf("String sent!\n");


	char cBuffer2[4];
	char recvString2[4];
	
	receivedBytes = 0;
	while(receivedBytes < 4){
		int readBytes = read(receiverSocket, &cBuffer2, 4 - receivedBytes, receivedBytes);
		int j;
		for(j = 0; j < readBytes; j++){
			recvString2[j + receivedBytes] = cBuffer2[j];
		}
		receivedBytes += readBytes;
	}
	recvString2[4] = '\0';

	printf("Received result: %s\n", recvString2);
	
}

int Connection::close(){
#ifdef __gnu_linux__
	close(senderSocket);
	close(receiverSocket);
#else
	closesocket(senderSocket);
	closesocket(receiverSocket);
#endif
}