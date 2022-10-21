/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarchal <amarchal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/13 16:10:21 by dvallien          #+#    #+#             */
/*   Updated: 2022/10/19 13:45:51 by amarchal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/ircserv.hpp"

bool	part(int socketClient, std::string channels, std::vector<std::string> reason, std::map<int, User> &userMap, std::map<std::string, Channel> &channelMap)
{
	User								&current = userMap[socketClient];
	std::vector<std::string>			tabChannels = splitNames(channels);
	std::vector<std::string>::iterator	nameChannel = tabChannels.begin();
	std::vector<std::string>::iterator	it;
	std::string 						buffer;
	
	for( it = reason.begin() + 2; it != reason.end(); it++) //GET REASON FOR LEAVING CHAN
		buffer = buffer + (*it) + " ";
	while(nameChannel != tabChannels.end())
	{
		if((*nameChannel)[0] == '#') //IF FIRST LETTER OF CHAN IS #
		{
			if(channelMap.find(*nameChannel) == channelMap.end())//IF CHANNEL DOESN'T EXIST
				numericReply(ERR_NOSUCHCHANNEL,socketClient, userMap, &(*nameChannel)); //err 403
			else //IF CHANNEL EXIST
			{
				if(channelMap.find(*nameChannel)->second.isInUserSet(socketClient).first == false) //IF USER DOESN'T BELONG TO THIS CHAN
					numericReply(ERR_NOTONCHANNEL, socketClient, userMap, &(*nameChannel)); //err 442
				else
				{
					std::string msg = userSource(current) + " PART " + *nameChannel + " " + buffer;
					informAllUsers(channelMap.find(*nameChannel)->second.getUserSet(), msg);
					// sendMsg(socketClient, msg);
					channelMap.find(*nameChannel)->second.getUserSet().erase(socketClient); //DO PART CMD (ERASE USER OF CHAN)
					if(channelMap.find(*nameChannel)->second.getUserSet().empty()) //IF NO USER LEFT, DELETE CHAN
						channelMap.erase(channelMap.find(*nameChannel));
				}
			} 
		}
		nameChannel++;
	}
	return (0);
}
