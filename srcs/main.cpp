/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dvallien <dvallien@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/04 13:53:40 by dvallien          #+#    #+#             */
/*   Updated: 2022/10/10 17:46:14 by dvallien         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/ircserv.hpp"
#include "../incs/signalManager.hpp"
#include <map>

/* 
	The client-server infrastructure mean a server socket listens for one or more connections from a client socket.
	Two sockets must be of the same type and in the same domain (Unix domain or Internet domain) to enable communication btw hosts.
*/

std::string firstWord(std::string content)
{
	char *words = new char [content.length() + 1];

	std::strcpy(words, content.c_str()); // copy all infos in content (cap, nick, user)

	char *line = strtok(words, " \n");
	while (line)
	{
		std::cout << "Line : " << line << std::endl;
		line = strtok (NULL, " ");
	}
	delete[] words;
	return (line);
}

void	getInfosClient(int socketClient, std::string content) //, std::map<int, User> repertory
{
	(void)socketClient;
	std::cout << "CONTENT : " << content << std::endl;
	if (firstWord(content) == "NICK")
	{
		std::cout << "HERE !!!! " << std::endl;
	}
}

int serverSetup()
{
	int socketServer = socket(AF_INET, SOCK_STREAM, 0);		// listening Socket
	struct sockaddr_in addrServer;							// in : ipv4  in6 : ipv6, contains technique informations of socket
	addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(30000);						// host to network, port to connect
	
	// BIND ADDR IP & PORT TO A SOCKET
	if (bind(socketServer, (const struct sockaddr *)&addrServer, sizeof(addrServer)) == -1)
	{
		std::cerr << "Can't bind" << std::endl;
		close(socketServer);
		return (-1);
	}
	
	// SOCKET WAITING CONNEXIONS, FIX THE WAITING LIST
	if (listen(socketServer, SOMAXCONN) == -1)								
	{
		std::cerr << "Can't listen" << std::endl;
		return (-1);
	}
	return (socketServer);
}

int acceptConnection(int socketServer)
{
	int socketClient;
	struct sockaddr_in addrClient;
	socklen_t csize = sizeof(addrClient);
	
	// ACCEPT CONNEXION DEMAND. CREATE NEW SOCKET AND SEND BACK FD FOR THIS SOCKET : DIALOG SOCKET
	socketClient = accept(socketServer, (struct sockaddr *)&addrClient, &csize);
	if (socketClient == -1)
	{
		std::cerr << "Can't accept" << std::endl;
		return (-1);
	}
	// DISPLAY INFOS ON CONNECTED CLIENT	
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];
	memset(host, 0, NI_MAXHOST);
	memset(service, 0, NI_MAXSERV);
	if (getnameinfo((sockaddr*)&addrClient, csize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
		std::cout << host << " connected on port " << service << std::endl;
	else
		std::cout << "Error" << std::endl;
	
	return (socketClient);
}

void	handleConnection(int socketClient, fd_set *currentSockets, fd_set *writeSockets)
{
	char	buffer[4096];
	memset(buffer, 0, sizeof(buffer));
	int bytesReceived = recv(socketClient, buffer, 4096, 0);
	if (bytesReceived == -1)
	{
		std::cerr << "Error in recv(), Quitting" << std::endl;
		close(socketClient);
		FD_CLR(socketClient, currentSockets);		// remove socket to the set of sockets we are watching
		return ;
	}
	if (bytesReceived == 0)
	{
		std::cout << "Client disconnected" << std::endl;
		close(socketClient);
		FD_CLR(socketClient, currentSockets);		// remove socket to the set of sockets we are watching
		return ;	
	}
	if (buffer[strlen(buffer) - 1] == '\n')
	{
		// SEND RSP_WELCOME 001 msg
		getInfosClient(socketClient, buffer);
		send(socketClient, ":my_irc 001 amarchal\n", sizeof(":my_irc 001 amarchal\n"), 0);
		// ECHO MSG TO ALL CLIENTS
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, writeSockets))
				if (i != socketClient)
					send(i, buffer, bytesReceived + 1, 0);
		}
	}
	return ;
}

int main(int ac, char **av)  // ./ircserv [port] [passwd]
{
	(void)ac;
	(void)av;
	// CREATE A SOCKET
	int socketServer = serverSetup();
	if (socketServer == -1)
		return (1);
	signalOn(socketServer);	

	fd_set currentSockets;
	fd_set readSockets;
	fd_set writeSockets;

	// INITIALIZE CURRENT SET
	FD_ZERO(&currentSockets);				// initialize
	FD_SET(socketServer, &currentSockets);	// add serverSocket to the set of sockets we are watching
	
	while (1)
	{
		readSockets = currentSockets;		// because select is destructive, it keeps only the sockets ready for reading/writing but we want to keep tracks of all sockets we are watching
		writeSockets = currentSockets;
		
		if (select(FD_SETSIZE, &readSockets, &writeSockets, nullptr, nullptr) == -1)
		{
			std::cerr << "select() error" << std::endl;
			return (1);
		}
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &readSockets))
			{
				if (i == socketServer)
				{
					// this is a new connection
					std::cout << "New connection requested" << std::endl;
					int socketClient = acceptConnection(socketServer);
					if (socketClient == -1)
						return (1);
					FD_SET(socketClient, &currentSockets);			// add a new clientSocket to the set of sockets we are watching
					FD_SET(socketClient, &writeSockets);
				}
				else
				{
					std::cout << "something to do with connection " << i << std::endl;
					handleConnection(i, &currentSockets, &writeSockets);		// do what we want to do with this connection
				}
			}
		}
	}
	close(socketServer); // TO DO IN signalHandler
	return (0);
}