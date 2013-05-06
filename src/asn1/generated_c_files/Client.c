#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <Message.h>
#include <MsgType.h>
#include <stdbool.h>

void printHelp(){
	printf("Usage: client serverip senderport receiverport integer [0|1]\n");
	printf("\t 1 = human readable form; 0 = hexadecimal\n");
}
bool print_tag(int tag){
	printf("\nTag: %d\n", tag);
	if(tag < 64){
		printf("\tUNIVERSAL\n");
	} else if (tag < 128) {
		printf("\tAPPLICATION\n");
	} else if (tag < 192) {
		printf("\tcontext specific\n");
	}else {
		printf("\tPRIVATE\n");
	}

	if((tag & 32) == 32){
		printf("\tcombined coding\n");
	} else {
		printf("\tprimitive type\n");
	}

	if((tag & 31) == 0){
		printf("\tEOC\n");
	} else if((tag & 31) == 1){
		printf("\tBOOLEAN\n");
	} else if((tag & 31) == 2){
		printf("\tINTEGER\n");
	} else if((tag & 31) == 3){
		printf("\tBIT STRING\n");
	} else if((tag & 31) == 4){
		printf("\tOCTET STRING\n");
	} else if((tag & 31) == 5){
		printf("\tNULL\n");
	} else if((tag & 31) == 6){
		printf("\tOBJECT IDENTIFIER\n");
	} else if((tag & 31) == 7){
		printf("\tObject Descriptor\n");
	} else if((tag & 31) == 8){
		printf("\tEXTERNAL\n");
	} else if((tag & 31) == 9){
		printf("\tREAL (float)\n");
	} else if((tag & 31) == 10){
		printf("\tENUMERATED\n");
	} else if((tag & 31) == 11){
		printf("\tEMBEDDED PDV\n");
	} else if((tag & 31) == 12){
		printf("\tUTF8String\n");
	} else if((tag & 31) == 13){
		printf("\tRELATIVE-OID\n");
	} else if((tag & 31) == 14){
		printf("\t(reserved)\n");
	} else if((tag & 31) == 15){
		printf("\t(reserved)\n");
	} else if((tag & 31) == 16){
		printf("\tSEQUENCE and SEQUENCE OF\n");
	} else if((tag & 31) == 17){
		printf("\tSET and SET OF\n");
	} else if((tag & 31) == 18){
		printf("\tNumericString\n");
	} else if((tag & 31) == 19){
		printf("\tPrintableString\n");
	} else if((tag & 31) == 20){
		printf("\tT61String\n");
	} else if((tag & 31) == 21){
		printf("\tVideotexString\n");
	} else if((tag & 31) == 22){
		printf("\tIA5String\n");
	} else if((tag & 31) == 23){
		printf("\tUTCTime\n");
	} else if((tag & 31) == 24){
		printf("\tGeneralizedTime\n");
	} else if((tag & 31) == 25){
		printf("\tGraphicString\n");
	} else if((tag & 31) == 26){
		printf("\tVisibleString\n");
	} else if((tag & 31) == 27){
		printf("\tGeneralString\n");
	} else if((tag & 31) == 28){
		printf("\tUniversalString\n");
	} else if((tag & 31) == 29){
		printf("\tCHARACTER STRING\n");
	} else if((tag & 31) == 30){
		printf("\tBMPString\n");
	} else if((tag & 31) == 31){
		printf("\t(use long-form)\n");
	} 
	return ((tag & 32) == 32);
}
void print_ber(void *buffer, int length, bool hex, int offset);

int print_ber_readable(void *buffer, int offset){
	int tag = 255 & ((int)((char*)buffer)[offset]);
	bool isCombined = print_tag(tag);
	int length = 255 & ((int)((char*)buffer)[offset + 1]);
	printf("Length: \n\t%d \n", length);
	if(isCombined){
		while(offset < length){
			int retVal= print_ber_readable(buffer, offset + 2);
			offset = offset + retVal;
		}
		
	} else {
		print_ber(buffer, length, true, offset + 2);
	}
	return length + 2;
}

void print_ber(void *buffer, int length, bool hex, int offset){
	// printf("\nlength %d offset %d\n", length, offset);
	if (hex) {
		printf("Hexadecimal value:\n\t");
		/* printing out hexadecimal byte values */
		int i;
		for(i = 0; i < length; i++){
			char ch = (int)((char*)buffer)[i + offset];
			printf("%02X ", ch & 0xff);
		}
		printf("\n");
	} else {
		print_ber_readable(buffer, 0);
	}
}
char printBuffer[1024];
int printIndex = 0;
int printLength = 0;
bool humanReadable = true;

 /* Write the encoded output into some FILE stream. */
static int write_out(const void *buffer, size_t size, void *app_key) {
	int32_t* senderSocketP = (int32_t *) (app_key);
	int32_t senderSocket = *senderSocketP;
	int i;
	
	/* printing out hexadecimal byte values */
	for(i = 0; i < size; i++){
		char ch = (int)((char*)buffer)[i];
		printBuffer[printIndex] = ch;
		printIndex++;
	}
	
	int written = 0;
	while(written < size){
		written += write(senderSocket, buffer, size - written, written);
	}
	printLength += size;

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
	message->msgType = msgType;  
	message->number  = integer;  
	message->string = *string;

	printf("\n");
	
	ec = der_encode(&asn_DEF_Message, message, write_out, &senderSocket);
	
	print_ber(printBuffer, printLength, !humanReadable, 0);
	printIndex = 0;
	printLength = 0;

	if(ec.encoded == -1) {
		fprintf(stderr, "Could not encode Rectangle (at %s)\n", ec.failed_type ? ec.failed_type->name : "unknown");
		exit(1);
	} else {
		// fprintf(stderr, "Created with BER encoded Rectangle\n");
   }
   printf("\n");
}

Message_t receiveMessage(int receiverSocket){
	char tag;

	read(receiverSocket, &tag, 1);

	char firstLengthByte;

	read(receiverSocket, &firstLengthByte, 1);

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

	
	asn_dec_rval_t rval; 
	Message_t *message = 0;  
	
	if(!isLengthUndetermined){
		char buf[length];
		
		char recvString[length + 1 + 1];
		recvString[0] = tag;
		recvString[1] = length;

		int receivedBytes = 0;

		while(receivedBytes < length){
			int readBytes = read(receiverSocket, &buf, length - receivedBytes, receivedBytes);
			int j;
			for(j = 0; j < readBytes; j++){
				recvString[j + receivedBytes + 2] = buf[j];
			}
			receivedBytes += readBytes;
		}

		print_ber(recvString, length+2, !humanReadable, 0);

		rval = ber_decode(0, &asn_DEF_Message, (void **)&message, recvString, length + 2);

		if(rval.code != RC_OK) {
			printf("Broken Rectangle encoding at byte");
			exit(1); 
		}

		return *message;
	} else {
		char buf[1024];
		buf[0] = tag;
		buf[1] = length;

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

		print_ber(buf, length+2, !humanReadable, 0);

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
	if(argc < 5){
		printHelp();
		return;
	}

	char* serverIP = argv[1];
	char* senderPortC = argv[2];
	char* receiverPortC = argv[3];
	char* integerToSendC = argv[4];

	if(argc == 6){
		humanReadable = (char)*argv[5] == 49;
	}

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
