#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "Message.h"
#include "MsgType.h"

void printHelp(){
	printf("Usage: client serverip senderport receiverport integer\n");
}

 /* Write the encoded output into some FILE stream. */
static int write_out(const void *buffer, size_t size, void *app_key) {
	int32_t* senderSocketP = (int32_t *) (app_key);
	int32_t senderSocket = *senderSocketP;
	int written = 0;
	while(written < size){
		written += write(senderSocket, &buffer, size - written, written);
	}
}

void sendMessage(MsgType_t *msgType, long integer, IA5String_t *string, int senderSocket){
	Message_t *message; /* Type to encode */
	asn_enc_rval_t ec; /* Encoder return value  */
	
	/* Allocate the Message */
	message = calloc(1, sizeof(Message_t)); /* not malloc! */
	if(!message) {
		perror("calloc() failed");
		exit(1); 
	}
	/* Initialize the Rectangle members */
	message->msgType = *msgType;  /* any random value */
	message->number  = integer;  /* any random value */
	message->string = *string;

	/* Encode the Rectangle type as BER (DER) */
	ec = der_encode(&asn_DEF_Message, message, write_out, &senderSocket);
	// ec = der_encode(&asn_DEF_Rectangle, rectangle, write_out, fp);
	// fclose(fp);
	if(ec.encoded == -1) {
		fprintf(stderr, "Could not encode Rectangle (at %s)\n", ec.failed_type ? ec.failed_type->name : "unknown");
		exit(1);
	} else {
		fprintf(stderr, "Created with BER encoded Rectangle\n");
   }
}


int main(int argc, char *argv[])
{

	if(argc != 5){
		printHelp();
		return;
	}

	char* serverIP = argv[1];
	char* senderPortC = argv[2];
	char* receiverPortC = argv[3];
	char* integerToSendC = argv[4];

	const char lineEnd = '\n';

	int senderPort = atoi(senderPortC);
	int receiverPort = atoi(receiverPortC);
	int integerToSend = atoi(integerToSendC);

	if(senderPort == 0){
		printHelp();
		return;
	}

	/* ----------- Creating sockets --------------- */

	// domain = AF_INET (IPv4)
	// type = SOCK_STREAM
	// 		Provides sequenced, reliable, two-way, connection-based byte streams. An out-of-band data transmission mechanism may be supported.
	// using TCP/IP
	int senderSocket = socket(AF_INET, SOCK_STREAM, 0);
	int receiverSocket = socket(AF_INET, SOCK_STREAM, 0);

	printf("socket created!\n");

	if(senderSocket < 0 || receiverSocket < 0) {
		//error when creating socket
		printf("Error when creating sockets\n");
		return;
	} 

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
	
	/* ----------- Sending integer --------------- */
	
	// integerToSendH = integerToSend in network format
	int32_t integerToSendN = (int) htonl(integerToSend);
	int index;
	int empty = 0;

	IA5String_t* string; /* Type to encode */
	
	/* Allocate the Message */
	string = calloc(1, sizeof(IA5String_t)); 

	long integer = 123;
	// integer = 1234;

	sendMessage(MsgType_clientToServerInt, integer, string, senderSocket);


	// printf("sending integer %s\n", integerToSendC);
	// if (write(senderSocket, &integerToSendN, sizeof integerToSendN) == -1 ){
	// 	//error while sending data
	// 	printf("Error while sending String\n");
	// };
	// printf("integer sent!\n");

	
	// /* ----------- Receiving integer & string --------------- */
	
	// int32_t intBuffer = 0;
	
	// int receivedBytes = 0;
	// int32_t receivedInteger = 0;
	// while(receivedBytes < 4){
	// 	int readBytes = read(receiverSocket, &intBuffer, (sizeof intBuffer) - receivedBytes, receivedBytes);
	// 	receivedInteger += (intBuffer << (8*receivedBytes));
	// 	receivedBytes += readBytes;
	// }

 //    int32_t receivedInt = ntohl(receivedInteger);

 //    printf("Received int: %d\n", receivedInt);

 //    int32_t intBuffer2 = 0;
 //    int32_t receivedBytes2 = 0;
	// int32_t receivedInteger2 = 0;
	// while(receivedBytes2 < 4){
	// 	int readBytes = read(receiverSocket, &intBuffer2, (sizeof intBuffer2) - receivedBytes2, receivedBytes2);
	// 	receivedInteger2 += (intBuffer2 << (8*receivedBytes2));
	// 	receivedBytes2 += readBytes;
	// }

 //    int32_t receivedCount = ntohl(receivedInteger2);

 //    if(receivedCount > 20 || receivedCount < 0){
 //    	printf("Received wrong count\n");

	// 	close(senderSocket);
	// 	close(receiverSocket);

	// 	return;
 //    }

	// char cBuffer[receivedCount];
	// char recvString[receivedCount];
	
	// receivedBytes = 0;
	// while(receivedBytes < receivedCount){
	// 	int readBytes = read(receiverSocket, &cBuffer, (sizeof cBuffer) - receivedBytes, receivedBytes);
	// 	int j;
	// 	for(j = 0; j < readBytes; j++){
	// 		recvString[j + receivedBytes] = cBuffer[j];
	// 	}
	// 	receivedBytes += readBytes;
	// }
	// recvString[receivedCount] = '\0';

	// printf("received String: \"%s\"\n", recvString);

	// //  ----------- Changing cases in String --------------- 
	// int i;
	// for(i = 0; i < sizeof recvString; i++){
	// 	if(recvString[i] >= 65 && recvString[i] <=90){
	// 		recvString[i] += 32;
	// 	} else if (recvString[i] >= 97 && recvString[i] <= 122){
	// 		recvString[i] -= 32;
	// 	}
	// }

	// printf("converted String: \"%s\" \n", recvString);
	
	// // // /* ----------- Sending String --------------- */
	// receivedBytes = 0;
	// while(receivedBytes < receivedCount){
	// 	receivedBytes += write(senderSocket, &recvString, (sizeof recvString) - receivedBytes, receivedBytes);
	// }
	// write(senderSocket, &lineEnd, sizeof lineEnd);
	// printf("String sent!\n");


	// char cBuffer2[4];
	// char recvString2[4];
	
	// receivedBytes = 0;
	// while(receivedBytes < 4){
	// 	int readBytes = read(receiverSocket, &cBuffer2, 4 - receivedBytes, receivedBytes);
	// 	int j;
	// 	for(j = 0; j < readBytes; j++){
	// 		recvString2[j + receivedBytes] = cBuffer2[j];
	// 	}
	// 	receivedBytes += readBytes;
	// }
	// recvString2[4] = '\0';

	// printf("Received result: %s\n", recvString2);
	
	close(senderSocket);
	close(receiverSocket);
}
