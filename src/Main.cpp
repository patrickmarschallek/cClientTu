#include "Main.h"

int main (int argc,char** argv){
	
	if(argc != 5){
		printHelp();
		return 0;
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
		return 0;
	}

	return 0;
}

void printHelp(){
	printf("Usage: client serverip senderport receiverport integer\n");
}