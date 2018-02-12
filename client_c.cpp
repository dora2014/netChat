/*************************	 Chat Program  - client Side	 	 	  *******************/
/****************														*****************/
/****************														****************/
/****************			1) Initialize TCP socket					****************/
/****************    		2) send connection request to the server 	***************/
/****************  			3) connection established					***************/
/**************** 		    4) start chatting      						***************/
/****************														***************/
/*************************************************************************************/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <fstream>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

using namespace std;

int main (int argc, char **argv) 
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    char *sendbuf = "Hi, I'm online. ";
    char recvbuf[DEFAULT_BUFLEN], in_msg[64];
    char *ipstr, str_dest[64];
    int iResult, ivalue;
    int recvbuflen = DEFAULT_BUFLEN;
    ofstream outFile;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo("10.0.0.150", DEFAULT_PORT, &hints, &result);
    
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        
        /***************************************************************************************/
        //print out the connecting address
        struct sockaddr_in *ipv4 = (struct sockaddr_in *) ptr->ai_addr;
		ipstr = inet_ntoa(ipv4->sin_addr);
 		if (ipstr ==NULL)
 		{
 			cout <<"Error calling inet_ntoa function " <<endl;
 		//	WSACleanup();
 			return 1;
		}
	
		cout << "successful on calling inet_ntoa function " <<endl;
		memset(str_dest, 0, sizeof(str_dest));
	 
		strcpy(str_dest, ipstr);
		cout << "client connects to IP address: " << str_dest <<endl;
        /*******************************************************************************************/
        
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Send an initial buffer
    iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
       closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);

   int a=1;
    
    do {

		memset(recvbuf, 0, sizeof(recvbuf)); //clear recieve buffer before reading it
		
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		cout <<"Recieved " <<iResult<<endl;
		if(recvbuf[0] == 'q')
			break;
        if ( iResult > 0 )
        {
		
            printf("Bytes received: %d\n", iResult);
            cout <<"Content Received back is: " << recvbuf <<'\n';
            cout <<"*************************************************"<<endl;
            
			//after first echo bytes sent, message is user prompt
			cout<<"Client >> ";
   
            std::cin.getline(in_msg, 64, '\n');

            
            iResult = send( ConnectSocket, in_msg, (int)strlen(in_msg), 0 );
            cout << iResult <<endl;
    		if (iResult == SOCKET_ERROR) 
			{
        		printf("send failed with error: %d\n", WSAGetLastError());
        		closesocket(ConnectSocket);
        		WSACleanup();
        		return 1;
    		}	//end if error
    		
    		a++;
    	}
        else if ( iResult == 0 )
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while( ( iResult > 0 ) && (recvbuf[0] != 'q') );

    cout << "start to shutdown the socket ....." <<endl;
    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
       printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    
    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    system("pause");
    return 0;
}
