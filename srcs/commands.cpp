/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gougier <gougier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 09:42:04 by gougier           #+#    #+#             */
/*   Updated: 2026/03/24 12:21:30 by gougier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Commands.hpp"
#include <sstream>
#include "../includes/Server.hpp"
#include "../includes/Responses.hpp"

void Server::handle_cmd(Commands &cmd, Client &client)
{
	if (!client.is_registered_)
	{
		if (cmd.cmd == "CAP")
			return ;
		if (cmd.cmd == "PASS")
			pass(client, cmd);
		else if (cmd.cmd == "NICK" && client.pass_validated)
			nick(client, cmd);
		else if (cmd.cmd == "USER" && !cmd.param.empty())
			client.user(cmd);
			//client.set_username(cmd.param[0]);
		else
			return (not_registered_yet(client));
		if (client.is_complete())
		{
			authentification(client);
			client.is_registered_ = true;
		}
		return ;
	}
	if (cmd.cmd == "JOIN")	
		join(client, cmd);
	else if (cmd.cmd == "PING" && !cmd.param.empty())
		play_ping_pong(client);
	else if (cmd.cmd == "PONG" && !cmd.param.empty())
		pong(client);
	else if(cmd.cmd == "INVITE")
		invite_client(client, cmd);
	else if(cmd.cmd == "PART" && !cmd.param.empty())
		part(client, cmd.param[0]);
	else if (cmd.cmd == "KICK")
		kick_client(client, cmd);
	else if (cmd.cmd == "TOPIC")
		topic(client, cmd);
	else if (cmd.cmd == "NICK")
		nick(client, cmd);
	else if (cmd.cmd == "QUIT")
		quit(client, cmd);
	else if (cmd.cmd == "MODE")
		handle_mode(client, cmd);
	else if (cmd.cmd == "PRIVMSG")
		priv_msg(client, cmd);
	else if (cmd.cmd == "WHO")
		who(client, cmd);
}

int Server::handle_mode(Client &client, Commands &cmd)
{
	if (cmd.param.empty())
		return (-2);
	std::string chan_name = cmd.param[0];
	if (chan_name[0] != '#')
		return (-1);	
	chan_name = chan_name.substr(1, std::string::npos);
	if (!channel_exists(chan_name))
	{
		Responses::no_such_channel(client, cmd.param[0]);
		return (-1);
	}
	return (channels[chan_name].handle_mode(client, cmd));
}

//TODO chercher cmd du buffer et l extraire et la mettre ds notre struct de cmd ? est ce qu une class est utile ? 
Commands Server::search_cmd(std::string &buffer)
{
	Commands cmd;
	std::string temp;
	std::istringstream ss(buffer);
	ss >> cmd.cmd;
	while (ss >> temp)
		cmd.param.push_back(temp);
	return (cmd);
}

void Server::who(Client &client, Commands &cmd)
{
	if (cmd.param.size() != 1)
		return ;
	std::string chan_name = cmd.param[0];
	if (chan_name[0] != '#')
		return ;
	chan_name = chan_name.substr(1, std::string::npos);
	if (channel_exists(chan_name))
		channels[chan_name].who(client, cmd.param[0]);
}

bool Server::in_use_on_network(std::string nick)
{
	for (size_t i = 0; i < clients.size(); ++i)
	{
		if (clients[i].get_nickname() == nick)
			return (true);
	}
	return (false);
}

bool check_valid_nick(std::string &nick)
{
	char first = nick[0];
	if (std::isdigit(first) || first == '#' || first == ':' || first == '&' || nick == "_")
		return (false);
	return (true);
} 

void Server::nick(Client &client, Commands &cmd)
{
	if (cmd.param.size() < 1)
		return server_reply(client, ERR_NONICKNAMEGIVEN, "", " :No nickname given\r\n");
	Client *target = find_target(cmd.param[0]);
	if (target && target->get_socket_fd() != client.get_socket_fd())
		return server_reply(client, ERR_NICKNAMEINUSE, cmd.param[0], " :Nickname is already in use\r\n");
	if (!check_valid_nick(cmd.param[0]))
		return server_reply(client, ERR_ERRONEUSNICKNAME, cmd.param[0], " :Erroneus nickname\r\n");
	client.set_nickname(cmd.param[0]);
}

void Server::priv_msg(Client &client, Commands &cmd)
{
	if (cmd.param.size() < 1)
		return server_reply(client, ERR_NEEDMOREPARAMS, cmd.cmd, " :Not enough parameters\r\n");
	if (cmd.param.size() == 1)
	{
		if (cmd.param[0] == ":")
			return server_reply(client, ERR_NORECIPIENT, cmd.cmd, " :No recipient given\r\n");
		else
			return server_reply(client, ERR_NOTEXTTOSEND, cmd.cmd, " :No text to send\r\n");
	}
	if (cmd.param[1][0] != ':')
		return server_reply(client, ERR_NOTEXTTOSEND, cmd.cmd, " :No text to send\r\n");
	if (cmd.param[0][0] == '#')
		return msg_to_all_in_channel(cmd.param[0], cmd, client);
	Client *target = find_target(cmd.param[0]);
	if (!target)
		return server_reply(client, ERR_NOSUCHNICK, cmd.param[0], " :No such nick\r\n");
	return msg_user_to_user(client, cmd, target);
}

void Server::msg_to_all_in_channel(std::string channel_name, Commands &cmd, Client &client)
{
	std::string chan_name = channel_name.erase(0, 1);
	std::map<std::string, Channel>::iterator it = channels.find(chan_name);
	if (it == channels.end())
		return ;
	std::vector<Client*> &clients = it->second.get_clients();
	std::string msg;
	for (size_t i = 1; i < cmd.param.size(); i++)
	{
		msg += cmd.param[i];
		if (i + 1 < cmd.param.size())
			msg += " ";
	}
	std::string msg_complete = ":" + client.get_nickname() + "!" + client.get_username() + "@" + client.get_hostname() + " " + cmd.cmd + " " + cmd.param[0] + " " + msg + "\r\n";
	std::cout << msg_complete << std::endl;
	for (size_t i = 0; i < clients.size(); i++)
	{
		if (clients[i]->get_socket_fd() != client.get_socket_fd())
			send(clients[i]->get_socket_fd(), msg_complete.c_str(), msg_complete.size(), 0);
	}
}

void Server::msg_user_to_user(Client &client, Commands &cmd, Client *target)
{
	std::string msg;
	for (size_t i = 1; i < cmd.param.size(); i++)
	{
		msg += cmd.param[i];
		if (i + 1 < cmd.param.size())
			msg += " ";
	}
	std::string msg_complete = ":" + client.get_nickname() + "!" + client.get_username() + "@" + client.get_hostname() + " PRIVMSG " + cmd.param[0] + " " + msg + "\r\n";
	std::cout << msg_complete << std::endl;
	send(target->get_socket_fd(), msg_complete.c_str(), msg_complete.size(), 0);
}

Client *Server::find_target(std::string nick)
{
	for (size_t i = 0; i < clients.size(); ++i)
	{
		if (clients[i].get_nickname() == nick)
			return (&clients[i]);
	}
	return (NULL);
}

//mm fonction que msg_to_all_in_channel mais ou le sender recoit son propre msg (besoin pour kick par ex)
void Server::broadcast(std::string channel_name, Commands &cmd, Client &client)
{
	std::map<std::string, Channel>::iterator it = channels.find(channel_name);
	if (it == channels.end())
		return ;
	std::vector<Client*> &clients = it->second.get_clients();
	std::string msg_complete = ":" + client.get_nickname() + "!" + client.get_username() + "@" + client.get_hostname() + " " + cmd.cmd + " #" + cmd.param[0] + " " + cmd.param[1] + "\r\n";
	std::cout << msg_complete << std::endl;
	for (size_t i = 0; i < clients.size(); i++)
	{
		send(clients[i]->get_socket_fd(), msg_complete.c_str(), msg_complete.size(), 0);
	}
}

void Server::pong(Client &client)
{
	// std::cout << "pong recved" << std::endl;
	client.answered_pong_ = true;
}

void Server::part(Client &client, std::string chan_name)
{
	// cest pas la vrai part function, juste utile
	if (chan_name[0] == '#')
		chan_name.erase(0, 1);
	std::map<std::string, Channel>::iterator it = channels.find(chan_name);
	if (it == channels.end())
		return server_reply(client, ERR_NOSUCHCHANNEL, chan_name, " :No such channel\r\n");
	if (!(client_is_in_channel(chan_name, client)))
		return server_reply(client, ERR_NOTONCHANNEL, chan_name, " :You're not on that channel\r\n");
	std::vector<Client*> &clients = it->second.get_clients();
	std::string msg_complete = ":" + client.get_nickname() + "!" + client.get_username() + "@" + client.get_hostname() + " PART" + " " + it->second.get_full_name() + " " + "\r\n";
	std::cout << msg_complete << std::endl;
	for (size_t i = 0; i < clients.size(); i++)
	{
		send(clients[i]->get_socket_fd(), msg_complete.c_str(), msg_complete.size(), 0);
	}
	it->second.remove_client(client.get_nickname());
}
