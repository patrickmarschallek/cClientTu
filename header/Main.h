/**
*   Compiler predefined MACROS from:
*   http://sourceforge.net/p/predef/wiki/OperatingSystems/
*/
#ifndef MAIN_H
#define MAIN_H

	#ifdef __gnu_linux__
		//unix libs
		#include <iostream>
		#include <sys/socket.h>
		#include <netinet/in.h>
	#elif __APPLE__ && __MACH__
		//MAC libs
	#elif sun || __sun
		// sun os
	#elif MSDOS ||	__MSDOS__ || _MSDOS || __DOS__ || WIN32 || _WIN32 || _WIN64	|| __WIN32__  || __WINDOWS__
		//Microsoft libs
		#include <iostream>
		#include <winsock2.h>
		#include <WS2tcpip.h>
		#include <stdint.h>

		using namespace std;
	#endif

	#include <stdio.h>
	#include <sys/types.h> 
	#include <string.h>

	void printHelp();
#endif