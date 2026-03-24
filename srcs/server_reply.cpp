/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_reply.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gougier <gougier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 13:43:28 by gougier           #+#    #+#             */
/*   Updated: 2026/03/24 10:40:32 by rribette         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Responses.hpp"
#include <sstream>

bool Server::channel_exists(std::string channel_name)
{
	std::map<std::string, Channel>::iterator it = channels.find(channel_name);
	if (it == channels.end())
		return (false);
	else
		return (true);
}

void Server::clear_topic(std::string channel_name)
{
	std::map<std::string, Channel>::iterator it = channels.find(channel_name);
	it->second.get_topic().clear();
}

bool Server::client_is_in_channel(std::string channel_name, Client &client)
{
	std::map<std::string, Channel>::iterator it = channels.find(channel_name);
	std::vector<Client*> &clients = it->second.get_clients();
	for (size_t i = 0; i < clients.size(); i++)
	{
		if (clients[i]->get_nickname() == client.get_nickname())
			return (true);
	}
	return (false);
}

bool Server::user_is_in_channel(std::string channel_name, std::string nickname)
{
	std::map<std::string, Channel>::iterator it = channels.find(channel_name);
	std::vector<Client*> &clients = it->second.get_clients();
	for (size_t i = 0; i < clients.size(); i++)
	{
		if (clients[i]->get_nickname() == nickname)
			return (true);
	}
	return (false);
}

void Server::authentification(Client &client)
{
	std::string result = std::asctime(std::localtime(&creation_time));
	std::string serv_name = "ircserv";
	std::string version = "1.0";
	std::string user_mode = "+ro";
	std::string channel_mode = "+itkol";
	std::string msg1 = ":" + serv_name + " 001 " + client.get_nickname() + " :Welcome " + client.get_nickname() + "\r\n";
	std::string msg2 = ":" + serv_name + " 002 " + client.get_nickname() + " :Your host is " + serv_name + " running version " + version + "\r\n";
	std::string msg3 = ":" + serv_name + " 003 " + client.get_nickname() + " :This server was created the " + result + "\r\n";
	std::string msg4 = ":" + serv_name + " 004 " + client.get_nickname() + " :" + serv_name + " " + version + " " + user_mode + " " + channel_mode + "\r\n";
	send(client.get_socket_fd(), msg1.c_str(), msg1.length(), 0);
	send(client.get_socket_fd(), msg2.c_str(), msg2.length(), 0);
	send(client.get_socket_fd(), msg3.c_str(), msg3.length(), 0);
	send(client.get_socket_fd(), msg4.c_str(), msg4.length(), 0);
}

void Server::play_ping_pong(Client &client)
{
	std::string msg = "PONG :ircserv\r\n";
	send(client.get_socket_fd(), msg.c_str(), msg.length(), 0);
}

//invite doc : 5 cas erreurs
//need more param
//The target channel SHOULD exist (at least one user is on it).
//Only members of the channel are allowed to invite other users.
//SHOULD reject it when the channel has invite-only mode set, and the user is not a channel operator
//If the user is already on the target channel, the server MUST reject
void Server::invite_client(Client &client, Commands &cmd)
{
	// cmd.param[1].erase(0, 1);
	if (cmd.param.size() < 2)
		return server_reply(client, ERR_NEEDMOREPARAMS, cmd.cmd, " :Not enough parameters\r\n");
	if (!(channel_exists(cmd.param[1])))
		return server_reply(client, ERR_NOSUCHCHANNEL, cmd.param[1], " :No such channel\r\n");
	if (!(client_is_in_channel(cmd.param[1], client)))
		return server_reply(client, ERR_NOTONCHANNEL, cmd.param[1], " :You're not on that channel\r\n");
	if ((user_is_in_channel(cmd.param[1], cmd.param[0])))
		return server_reply(client, ERR_USERONCHANNEL, cmd.param[0] + " " + cmd.param[1], " :is already on channel\r\n");
	std::map<std::string, Channel>::iterator it = channels.find(cmd.param[1]);
	if (!(it->second.is_operator(client)))
		return Responses::chan_op_privs_needed(client, it->second);
	Client *target = find_target(cmd.param[0]);
	if (!target)
		return server_reply(client, ERR_NOSUCHNICK, cmd.param[0], " :No such nick\r\n");
	target->get_invited_list().push_back(cmd.param[1]);
	std::string msg_complete = ":" + client.get_nickname() + "!" + client.get_username() + "@" + client.get_hostname() + " INVITE " + cmd.param[0] + " " + cmd.param[1] + "\r\n";
	std::cout << msg_complete << std::endl;
	send(target->get_socket_fd(), msg_complete.c_str(), msg_complete.size(), 0);
	return ;
}

void Server::not_registered_yet(Client &client)
{
	std::string msg = ":ircserv 451 " + client.get_nickname() + " :You have not registered\r\n";
	send(client.get_socket_fd(), msg.c_str(), msg.length(), 0);
}

void Server::kick_client(Client &client, Commands &cmd)
{
	cmd.param[0].erase(0, 1);
	if (cmd.param.size() < 2)
		return server_reply(client, ERR_NEEDMOREPARAMS, cmd.cmd, " :Not enough parameters\r\n");
	if (!(channel_exists(cmd.param[0])))
		return server_reply(client, ERR_NOSUCHCHANNEL, cmd.param[0], " :No such channel\r\n");
	if (!(user_is_in_channel(cmd.param[0], cmd.param[1])))
		return server_reply(client, ERR_USERNOTINCHANNEL, cmd.param[1] + " " + cmd.param[0], " :They aren't on that channel\r\n");
	if (!(client_is_in_channel(cmd.param[0], client)))
		return server_reply(client, ERR_NOTONCHANNEL, cmd.param[0], " :You're not on that channel\r\n");
	std::map<std::string, Channel>::iterator it = channels.find(cmd.param[0]);
	if (!(it->second.is_operator(client)))
		return Responses::chan_op_privs_needed(client, it->second);
	broadcast(cmd.param[0], cmd, client);
	it->second.remove_client(cmd.param[1]);
}

void Server::notify_all_topic_changed(std::string channel_name, std::string msg)
{
	std::map<std::string, Channel>::iterator it = channels.find(channel_name);
	std::cout << "[DEBUG : notify_all_topic_changed]" << std::endl;
	it->second.debug();
	std::vector<Client*> &clients = it->second.get_clients();
	for (size_t i = 0; i < clients.size(); i++)
	{
		//std::cout << "clients size = "<< clients.size() << std::endl;
		//std::cout << "clients n :" << i << " "clients[i]->get_nickname() << std::endl;
		//send(clients[i]->get_socket_fd(), msg.c_str(), msg.length(), 0);
		clients[i]->debug();
		clients[i]->send_message(msg);
	}
}

void Server::server_reply(Client &client, int code, const std::string &middle, const std::string &trailing)
{
	std::stringstream ss;
	ss << code;
	std::string msg_complete = ":ircserv " + ss.str() + " " + client.get_nickname();
	if (!(middle.empty()))
		msg_complete += " " + middle;
	msg_complete += " " + trailing;
	std::cout << COLOR_GREEN << client.get_username() << " : " << msg_complete << COLOR_DFL << std::endl;
	send(client.get_socket_fd(), msg_complete.c_str(), msg_complete.length(), 0);
}

//TODO checker si un changement de topic est permis
void Server::topic(Client &client, Commands &cmd)
{
	std::string chan_name = cmd.param[0];
	if (chan_name[0] == '#')
		chan_name = chan_name.substr(1, std::string::npos);
//	std::cout << COLOR_BLUE << "[DEBUG : topic] : chan_name : " << chan_name << COLOR_DFL << std::endl;
	if (cmd.param.size() < 1)
		return server_reply(client, ERR_NEEDMOREPARAMS, cmd.cmd, " :Not enough parameters\r\n");
	if (!(channel_exists(chan_name)))
		return server_reply(client, ERR_NOSUCHCHANNEL, cmd.param[0], " :No such channel\r\n");
	if (!(client_is_in_channel(chan_name, client)))
		return server_reply(client, ERR_NOTONCHANNEL, cmd.param[0], " :You're not on that channel\r\n");
	std::map<std::string, Channel>::iterator it = channels.find(chan_name);
	if (cmd.param.size() == 1)//If <topic> is not given, either RPL_TOPIC or RPL_NOTOPIC is returned specifying the current channel topic or lack of one.
	{
		if (!(it->second.get_topic().empty()))
			return server_reply(client, RPL_TOPIC, cmd.param[0], " :" + it->second.get_topic() + "\r\n");
		else if (it->second.get_topic().empty())
			return server_reply(client, RPL_NOTOPIC, cmd.param[0], " :No topic is set\r\n");
	}
	if (!(it->second.is_operator(client)))
			return Responses::chan_op_privs_needed(client, it->second);
	if ((cmd.param.size() == 2) && cmd.param[1].empty())//If <topic> is an empty string, the topic for the channel will be cleared.
		return clear_topic(cmd.param[0]);
	std::string newtopic;
	for (size_t i = 1; i < cmd.param.size(); i++)
	{
		newtopic += cmd.param[i];
		if (i + 1 < cmd.param.size())
			newtopic += " ";
	}
	if (it->second.change_topic(client, newtopic) == -2)
		return ;
	std::string msg = ":ircserv 332 " + client.get_nickname() + " " + cmd.param[0] + " " + it->second.get_topic() + "\r\n";
	notify_all_topic_changed(chan_name, msg);
}

void Server::pass(Client &client, Commands &cmd)
{
	if (cmd.param.size() < 1)
		return server_reply(client, ERR_NEEDMOREPARAMS, cmd.cmd, " :Not enough parameters\r\n");
	if (password_ != cmd.param[0])
		return server_reply(client, ERR_PASSWDMISMATCH, "", ":Password incorrect\r\n");
	if (password_ == cmd.param[0])
		client.pass_validated = true;
}

//quand un client quit, envoie un msg a tous les clients des channels ds leauel il etait
void Server::msg_to_shared_channels(Client &client, std::string msg)
{
//	std::cout << COLOR_BLUE << "[DEBUG : msg_to_shared_channels] : " << msg << COLOR_DFL << std::endl;
	std::deque<std::string> &chans = client.get_channels();
	for (size_t i = 0; i < chans.size(); i++)
	{
		std::string chan_name = chans[i];
		std::string new_msg = "QUIT " + this->channels[chan_name].get_full_name() + " " + msg;
//		std::cout << COLOR_BLUE << "[DEBUG : msg_to_shared_channels] : " << chan_name << COLOR_DFL << std::endl;
		(this->channels[chan_name]).send_message(client, new_msg);
	}
}

void Server::quit(Client &client, Commands &cmd)
{
	if (cmd.param.size() >= 1)
	{
		std::string msg = "";
		for (size_t i = 0; i < cmd.param.size(); i++)
		{
			msg.append(cmd.param[i]);
			if (i + 1 < cmd.param.size())
				msg.append(" ");
		}
		msg_to_shared_channels(client, msg);
	}
	disconnect(client.get_socket_fd());
}

void Server::join(Client &client, Commands &cmd)
{
	if (cmd.param.size() < 1)
		return server_reply(client, ERR_NEEDMOREPARAMS, cmd.cmd, " :Not enough parameters\r\n");
	if (cmd.param[0] == "0")
	{
		std::deque <std::string> &channels = client.get_channels();
		std::deque<std::string>::iterator it = channels.begin();
		for (; it != channels.end(); it++)
			part(client, *it);
	}
	std::deque<std::string> channels = split(cmd.param[0],",");
	std::deque<std::string> keys;
	if (cmd.param.size() > 1)
		keys = split(cmd.param[1],",");
	for (size_t i = 0; i < channels.size(); i++)
	{
		std::string channel_name = channels[i];
		if (channel_name[0] == '#')
			channel_name.erase(0, 1);
		std::string key = "";
		if (i < keys.size())
			key = keys[i];
		if (client.get_channels().size() == CHANLIMIT)
			return server_reply(client, ERR_TOOMANYCHANNELS, channel_name, " :You have joined too many channels\r\n");
		if (join_channel(channel_name, client, key))
			join_msg(channel_name, client);
	}
}

std::deque<std::string> Server::split(const std::string &s, const std::string &delimiter)
{
	std::deque<std::string> channels;
	std::string str = s;
	size_t pos = 0;
	std::string token;
	while ((pos = str.find(delimiter)) != std::string::npos)
	{
		token = str.substr(0, pos);
		channels.push_back(token);
		str.erase(0, pos + delimiter.length());
	}
	channels.push_back(str);
	return (channels);
}

void Server::join_msg(std::string channel_name, Client &client)
{
	std::map<std::string, Channel>::iterator it = channels.find(channel_name);
	if (it == channels.end())
		return ;
	std::vector<Client*> &clients = it->second.get_clients();
	std::string msg_complete = ":" + client.get_nickname() + "!" + client.get_username() + "@" + client.get_hostname() + " JOIN #" + channel_name + "\r\n";
	for (size_t i = 0; i < clients.size(); i++)
		send(clients[i]->get_socket_fd(), msg_complete.c_str(), msg_complete.size(), 0);
	if (!(it->second.get_topic().empty()))
	{
		std::string client_msg = "332 " + client.get_nickname() + " " + it->second.get_full_name() + " " + it->second.get_topic() + "\r\n";
		client.send_message(client_msg);
	}
	std::string user_list;
	std::map<std::string, bool> operators = it->second.get_operators();
	for (size_t i = 0; i < clients.size(); i++)
	{
		std::string nick = clients[i]->get_nickname();
		if (operators[nick])
			user_list += '@';
		user_list += nick;
		if (i != clients.size() - 1)
			user_list += " ";
	}
	server_reply(client, RPL_NAMREPLY, "= #" + channel_name, ":" + user_list + "\r\n");
	server_reply(client, RPL_ENDOFNAMES, "#" + channel_name, ":End of /NAMES list\r\n");
}

bool Server::check_invite_list(std::deque<std::string> invited_list_, std::string chan_name)
{
	std::deque<std::string>::iterator it = invited_list_.begin();
	for (; it != invited_list_.end(); it++)
	{
		if (*it == chan_name)
			return (true);
	}
	return (false);
}
