#ifndef CONNECTION_H
#define CONNECTION_H

#include "Main.h"

class Connection{
	 public:                           
		Connection();                      // der Default-Konstruktor
		~Connection();                     // der Destruktor
 
	private:                             
		int createSocket();
		int startConnection();
		int sendInt();
		int sendString();
		int receive();
		int close();

		int senderPort;
		int receiverPort;
		int integerToSend;

		char* serverIP;
		char* senderPortC;
		char* receiverPortC;
		char* integerToSendC;

#ifdef __gnu_linux__
		int senderSocket;
		int receiverSocket;
#else
		SOCKET senderSocket;
		SOCKET receiverSocket;
#endif
};

#endif