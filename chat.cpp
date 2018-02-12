/*************************	 Chat Program  - Server Side	 	 	  *******************/
/****************														*****************/
/****************														****************/
/****************			1) Initialize TCP socket					****************/
/****************    		2) wait for a connection, and connect 		***************/
/****************  			3) wait and listen for income message    	***************/
/**************** 		    4) chat with a client       				***************/
/****************														***************/
/*************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <chrono>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512
#define IP_ADDRESS "192.168.125.116"


int main (void)
{
	WSADATA wsaData;
	SOCKET ListenSocket, ClientSocket;
	int iResult, iSendResult, recvbuflen = DEFAULT_BUFLEN, port, len_addr;
	struct addrinfo hints, *res=NULL, *p;
	struct in_addr *ip_addr;
	struct sockaddr_storage local_addr;

	char *ipstr, *sendbuf, ipv_num[] = "ip4";
	char str_dest[64], in_msg[64], *out_msg="welcome to the server!";	
	char recvbuf[DEFAULT_BUFLEN], hostname[64], hostclient[64], serv_name[64];
	bool opt_v = TRUE;
	
	//set of socket descriptors for select and master
    fd_set read_fds;
    fd_set master_fds;
    int fdmax; // maximum file descriptor number
    
	
	//initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(iResult !=0)
	{
		cout << "WSAStartup failed! Error code: " << iResult <<endl;
		WSACleanup();
		return 1; 
		
	}
	
	//successful on initialize window socket
	cout <<"Win socket startup successful...." <<endl;
	
	//clear addrinfo structure, store information
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	
    iResult = getaddrinfo("10.0.0.150", DEFAULT_PORT, &hints, &res);

	if(iResult !=0)
	{
		cout << "getaddrinfo failed! Error code: " << iResult <<endl;
		WSACleanup();
		return 1; 
	}
	
	//successful on initialize window socket
	cout <<"sucessful on calling getaddrinfo()...." <<endl;
	
	p=res;
	struct sockaddr_in *ipv4 = (struct sockaddr_in *) p->ai_addr; //cast to sockaddr_in structure

	
 	ipstr = inet_ntoa(ipv4->sin_addr);
 	if (ipstr ==NULL)
 	{
 		cout <<"Error calling inet_ntoa function " <<endl;
 		WSACleanup();
 		return 1;
	}
	
	cout << "successful on calling 1st inet_ntoa function " <<endl;
	memset(str_dest, 0, sizeof(str_dest));
	 
	strcpy(str_dest, ipstr);
	cout << ipv_num << "address is " << str_dest <<endl;
	
	//call a socket function on the server side
	ListenSocket = INVALID_SOCKET;
	
	ListenSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	cout <<"Server socket descriptor is " << ListenSocket <<endl;
	if(ListenSocket ==INVALID_SOCKET)
	{
		cout <<"Error when creating a server socket! ";
		cout <<"Error code: " << WSAGetLastError() <<endl;
		freeaddrinfo(res);
		WSACleanup();
		return 1;
		
	}
	
	cout << "successful on creating a server socket " << endl;
	
	//set server socket to allow multiple connections , this is just a good habit, it will work without this
    iResult = setsockopt(ListenSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt_v, sizeof(opt_v) );
	if (iResult == SOCKET_ERROR) 
    {
       cout <<"Error code: " << WSAGetLastError() <<endl;
       freeaddrinfo(res);
       closesocket(ListenSocket);
	   WSACleanup();
	   return 1;
    }
	cout << "successful on setting server socket option " << endl;

    //binding a socket
    iResult = bind(ListenSocket, res->ai_addr, (int)res->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
    	cout <<"Error binding a socket! ";
    	cout <<"Error code is " <<WSAGetLastError() <<endl;
    	freeaddrinfo(res);
    	closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
    
    cout << "successful on binding a server socket " << endl;
	freeaddrinfo(res); //free addrinfo after binding, no longer needed
	
    //listen for incoming connection
	iResult = listen(ListenSocket, 2);
	if(iResult == SOCKET_ERROR)
	{
		cout <<"Error listening! ";
    	cout <<"Error code is " <<WSAGetLastError() <<endl;
    	closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	cout << "listening for an incoming connection..... " << endl;
	

	// Accept a client socket
	ClientSocket=INVALID_SOCKET;	
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) 
	{
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
	else{
		
		cout << "Client Connected !! " << endl;
	}
	
	closesocket(ListenSocket); //no longer need server socket, use new connected socket 
	
	// receive until client shuts down the connection
	
	//find out who are on the other side
	len_addr = sizeof(local_addr);
	iResult = getpeername(ClientSocket, (struct sockaddr*)&local_addr, &len_addr );  
	
	if(iResult == SOCKET_ERROR )
	{
		cout <<"Error getpeername()! ";
    	cout <<"Error code is " <<WSAGetLastError() <<endl;
    	closesocket(ClientSocket);
		WSACleanup();
		return 1;		
	} 
	
	// print out the peername information
	ipv4 = (struct sockaddr_in *) &local_addr;  //cast to sockaddr_in struct type
	port = ipv4->sin_port;   // extract port number		
    ipstr = inet_ntoa(ipv4->sin_addr);
 	if (ipstr ==NULL)
 	{
 		cout <<"Error calling inet_ntoa function " <<endl;
 		WSACleanup();
 		return 1;
	}
	
	cout << "successful on calling 2nd inet_ntoa function " <<endl;
	memset(str_dest, 0, sizeof(str_dest));
	 
	strcpy(str_dest, ipstr);
	cout << ipv_num << "peer IP address is " << str_dest <<endl;
	cout << ipv_num << "peer Port is " << port <<endl;	

    //get hostname of the client
	getnameinfo((struct sockaddr*)&local_addr, sizeof(local_addr), hostclient, sizeof(hostclient), serv_name, sizeof(serv_name), 0 );
	cout << "Peer hostname is " <<hostclient<<endl;
	cout << "Peer service name is " <<serv_name<<endl;

    //get hostname
    gethostname(hostname, sizeof(hostname) );
    cout << "Hostname for server machine is  " << hostname <<endl;


// **************************************************************************************
	
	do{
		
		memset(recvbuf, 0, sizeof(recvbuf));
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		
		if(iResult >0)  //if recieve something echo back
		{
		  cout<< iResult << "Bytes Received! " <<'\n';
		  cout <<"Content Received back is: " << recvbuf <<'\n';
		  cout <<"*************************************************"<<endl;
		  
		  if( recvbuf[0] != 'q')
		  {
		  	//iSendResult = send(ClientSocket, recvbuf, iResult, 0);
		  	
		  	cout<<"Server >> ";
            //cin.get(in_msg, 64 );
           // cout << in_msg<<endl; 
            std::cin.getline(in_msg, 64, '\n');
            
           // sendbuf = in_msg; 
            
		  	iSendResult = send(ClientSocket, in_msg, (int)strlen(in_msg), 0); //send input message
		  	
		  	if (iSendResult == SOCKET_ERROR)
		  	{	  	
		  		cout <<"Echo sending failed! Error code: "<< WSAGetLastError() << endl;
    			closesocket(ClientSocket);
				WSACleanup();
				return 1;
		  	}
		  
		  	cout<< iSendResult << "Bytes Sent! " <<'\n';
		  	
		  } //if recieve NOT q
		  
		  //if recieve 'q', do nothing
		} 
		else if (iResult == 0)
		{
			cout <<"client-server connection closed!" <<endl;
			
		}
		else{
			cout <<"Receive Failed! Error code: "<< WSAGetLastError() << endl;	
			closesocket(ClientSocket);
			WSACleanup();
			return 1;			
		}
		
		//system("pause");
		
	} while ( (iResult>0) && (recvbuf[0] != 'q') ); //if recieve nothing, shuts down, or error
	
	
	// shutdown the connection from the sending side since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        cout <<"Shutdonw Failed! Error code: " << WSAGetLastError() <<endl;
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }
	
	closesocket(ClientSocket);
	WSACleanup();	

	system("pause");
	return 0;
	
}
