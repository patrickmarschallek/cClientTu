#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <Message.h>
#include <MsgType.h>
#include <stdbool.h>

void printHelp(){
	printf("Usage: client serverip senderport receiverport integer\n");
}

 /* Write the encoded output into some FILE stream. */
static int write_out(const void *buffer, size_t size, void *app_key) {
	int32_t* senderSocketP = (int32_t *) (app_key);
	int32_t senderSocket = *senderSocketP;
	int i;
	// /* printing out decimal byte values */
	// printf("writing out: \n");
	// for(i = 0; i < size; i++){
	// 	printf("%d ", (int)((char*)buffer)[i]);
	// }
	// printf("\n");
	
	int written = 0;
	while(written < size){
		written += write(senderSocket, buffer, size - written, written);
	}
	// printf("%d bytes written\n", written);

}

void sendMessage(MsgType_t msgType, long integer, IA5String_t *string, int senderSocket){
	Message_t *message; /* Type to encode */
	asn_enc_rval_t ec; /* Encoder return value  */
	
	// printf("Allocating Message_t\n");
	/* Allocate the Message */
	message = calloc(1, sizeof(Message_t)); /* not malloc! */
	if(!message) {
		perror("calloc() failed");
		exit(1); 
	}
	// printf("Initialize the Rectangle members\n");
	/* Initialize the Rectangle members */
	message->msgType = msgType;  /* any random value */
	// printf("\tmsgType done\n");
	message->number  = integer;  /* any random value */
	// printf("\tnumber done\n");
	message->string = *string;
	// printf("\tstring done\n");

	// printf("message->msgType: %d\n", (int) message->msgType);
	// printf("message->number: %d\n", (int) message->number);
	// printf("message->string: %s\n", message->string.buf);

	// printf("Calling der_encode\n");
	/* Encode the Rectangle type as BER (DER) */
	ec = der_encode(&asn_DEF_Message, message, write_out, &senderSocket);
	// ec = der_encode(&asn_DEF_Rectangle, rectangle, write_out, fp);
	// fclose(fp);
	if(ec.encoded == -1) {
		fprintf(stderr, "Could not encode Rectangle (at %s)\n", ec.failed_type ? ec.failed_type->name : "unknown");
		exit(1);
	} else {
		// fprintf(stderr, "Created with BER encoded Rectangle\n");
   }
}

Message_t receiveMessage(int receiverSocket){
	char tag;

	read(receiverSocket, &tag, 1);

	char firstLengthByte;

	read(receiverSocket, &firstLengthByte, 1);

	// printf("received tag: %d %c", (int) tag, tag);

	int32_t firstLengthByteInt = (int32_t) firstLengthByte;

	int32_t length;

	bool isLengthUndetermined = false;

	if(firstLengthByteInt <= 127){
		length = firstLengthByteInt;
	} else if(firstLengthByteInt == 128){
		// undetermined form
		isLengthUndetermined = true;
	} if(firstLengthByteInt > 128) {
		int32_t octetsForLength = firstLengthByteInt - 128;
		//TODO read length
	}

	// printf("received length: %d", length);

	if(!isLengthUndetermined){
		char buf[length];      /* Temporary buffer      */
		
		char recvString[length + 1 + 1];
		recvString[0] = tag;
		recvString[1] = length;

		asn_dec_rval_t rval; /* Decoder return value  */
		Message_t *message = 0; /* Type to decode. Note this 01! */ 

		int receivedBytes = 0;

		while(receivedBytes < length){
			int readBytes = read(receiverSocket, &buf, length - receivedBytes, receivedBytes);
			int j;
			for(j = 0; j < readBytes; j++){
				recvString[j + receivedBytes + 2] = buf[j];
			}
			receivedBytes += readBytes;
		}
		// recvString[receivedCount] = '\0';

		/* Decode the input buffer as Rectangle type */
		rval = ber_decode(0, &asn_DEF_Message, (void **)&message, recvString, length + 2);


		// printf("message->msgType: %d\n", (int) message->msgType);
		// printf("message->number: %d\n", (int) message->number);
		// printf("message->string: %s\n", message->string.buf);

		if(rval.code != RC_OK) {
			printf("Broken Rectangle encoding at byte");
			exit(1); 
		}

		return *message;
	} else {
		char buf[1024];
		buf[0] = tag;
		buf[1] = length;

		asn_dec_rval_t rval; /* Decoder return value  */
		Message_t *message = 0; /* Type to decode. Note this 01! */ 

		bool endOfContentReceived = false;
		int i = 2;
		while(!endOfContentReceived){
			char cBuf;
			read(receiverSocket, &cBuf, 1);
			buf[i] = cBuf;
			if(i > 0 && (int)buf[i] == 0 && (int)buf[i - 1] == 0){
				endOfContentReceived = true;
			}
			i++;
		}
		// recvString[receivedCount] = '\0';

		/* Decode the input buffer as Rectangle type */
		rval = ber_decode(0, &asn_DEF_Message, (void **)&message, buf, i);

		if(rval.code != RC_OK) {
			printf("Broken Rectangle encoding at byte ");
			exit(1); 
		}

		return *message;
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
	
	IA5String_t* string; /* Type to encode */
	
	/* Allocate the Message */
	string = calloc(1, sizeof(IA5String_t)); 

	long integer = (long) integerToSend;
	
	printf("Client - Sending int: %d \n", integerToSend);

	sendMessage(MsgType_clientToServerInt, integer, string, senderSocket);

	Message_t msg = receiveMessage(receiverSocket);

	printf("Client - Received int: %d and String: %s\n", (int) msg.number, msg.string.buf);

	char* recvString = msg.string.buf;
	
	/* ----------- Changing cases in String --------------- */ 
	int i;
	for(i = 0; true; i++){
		if(recvString[i] == '\0'){
			break;
		}
		if(recvString[i] >= 65 && recvString[i] <=90){
			recvString[i] += 32;
		} else if (recvString[i] >= 97 && recvString[i] <= 122){
			recvString[i] -= 32;
		}
	}

	printf("Client - Sending converted String: %s\n", msg.string.buf);

	sendMessage(MsgType_clientToServerString, 0, &msg.string, senderSocket);

	Message_t msg2 = receiveMessage(receiverSocket);
	printf("Client - received result: %s \n", msg2.string.buf);

	close(senderSocket);
	close(receiverSocket);
}
