/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarchal <amarchal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/07 11:45:49 by dvallien          #+#    #+#             */
/*   Updated: 2022/10/19 13:46:13 by amarchal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/User.hpp"

/****************** CANONIC FORM ******************/
User::User(void) : _socket(0), _pass(false), _id(0)
{}

User::User(const User &src) : _socket(src._socket), _passwd(src._passwd), _nickname(src._nickname), _username(src._username), _realname(src._realname), _pass(src._pass), _id(src._id), _command(src._command), _hostname(src._hostname), _channels(src._channels), _privMsg(src._privMsg)
{}

User & User::operator=(const User &src)
{
    this->_socket = src._socket;
    this->_passwd = src._passwd;
	this->_nickname = src._nickname;
	this->_username = src._username;
	this->_realname = src._realname;
	this->_pass = src._pass;
    this->_id = src._id;
    this->_command = src._command;
	this->_hostname = src._hostname;
	this->_channels = src._channels;
	this->_privMsg = src._privMsg;
	return (*this);
}

User::~User(void)
{}

/****************** CONSTRUCTOR BY NAME ******************/
User::User(std::string nickname, std::string username, unsigned int id)
{
	this->_nickname = nickname;
	this->_username = username;
	this->_id = id;
}

/****************** SETTERS ******************/
void	User::setSocket(int socket) {this->_socket = socket;}
void	User::setPasswd(std::string passwd) {this->_passwd = passwd;}
void	User::setNickname(std::string nickname) {this->_nickname = nickname;}
void	User::setUsername(std::string username) {this->_username = username;}
void	User::setRealname(std::string realname) {this->_realname = realname;}
void	User::setId(unsigned int id) {this->_id = id;}
void	User::setHostname(std::string hostname) {this->_hostname = hostname;}
void	User::setPassOk() {_pass = true;}

/****************** GETTERS ******************/
int 						User::getSocket(void) const {return (this->_socket);}
std::string 				User::getPasswd(void) const {return (this->_passwd);}
std::string 				User::getNickname(void) const {return (this->_nickname);}
std::string 				User::getUsername(void) const {return (this->_username);}
std::string 				User::getRealname(void) const {return (this->_realname);}
bool						User::getPass(void) const {return (this->_pass);}
unsigned int 				User::getId(void) const {return (this->_id);}
std::string 				User::getHostname(void) const {return (this->_hostname);}
std::set<std::string>	 	User::getChannels(void) const {return (this->_channels);}
std::set<int> 				User::getPrivMsg(void) const {return (this->_privMsg);}

/****************** MEMBER FUNCTIONS ******************/
void    User::appendCommand(const std::string &str)
{
    _command += str;
}

std::string User::deliverCommand(void)
{
    std::string commandDelivered;
    size_t found = _command.find("\r\n");
    if (found != std::string::npos)
    {
        commandDelivered.assign(_command, 0, found);
        _command.erase(0, found + 2);
    }
    return (commandDelivered);
}

const std::string &User::getCommand(void) const
{
    return (_command);
}

void	User::addChannel(std::string channelName)
{
	this->_channels.insert(channelName);
}

void	User::addPrivMsg(int userSocket)
{
	this->_privMsg.insert(userSocket);
}

void	User::removeChannel(std::string channelName)
{
	std::set<std::string>::iterator it = this->_channels.begin();
	std::set<std::string>::iterator end = this->_channels.end();
	while (it != end)
	{
		if ((*it).compare(channelName) == 0)
			this->_channels.erase(it);
		it++;
	}
}

void	User::removePrivMsg(int userSocket)
{
	std::set<int>::iterator it = this->_privMsg.begin();
	std::set<int>::iterator end = this->_privMsg.end();
	while (it != end)
	{
		if ((*it) == userSocket)
			this->_privMsg.erase(it);
		it++;
	}
}

bool	User::isInPrivMsg(int userSocket)
{
	std::set<int>::iterator it = this->_privMsg.begin();
	std::set<int>::iterator end = this->_privMsg.end();
	while (it != end)
	{
		if ((*it) == userSocket)
			return (true);
		it++;
	}
	return (false);
}

/****************** STREAM OVERLOAD ******************/
std::ostream &operator<<(std::ostream &stream, const User &source)
{
	stream 	<< "~~ User ~~" << std::endl
			<< "Socket\t:" << source.getSocket() << std::endl
			<< "Nickname\t:" << source.getNickname() << std::endl
			<< "Username\t:" << source.getUsername() << std::endl;
	return (stream);
}

/****************** USEFUL USER TOOLS FUNCTIONS *********************/

bool	fullyRegistered(const User &user)
{
	return (!user.getUsername().empty() && !user.getNickname().empty() && !user.getRealname().empty() && !user.getHostname().empty());
}

std::map<int, User>::iterator	findUserByNickName(const std::string &nick, std::map<int, User> &userMap)
{
	std::map<int, User>::iterator 	end = userMap.end();
	for (std::map<int, User>::iterator it = userMap.begin(); it != end; ++it)
	{
		if (!(it->second.getNickname().compare(nick)))
			return (it);
	}
	return (end);
}