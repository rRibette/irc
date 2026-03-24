/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   responses.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rribette <rribette@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 13:58:03 by rribette          #+#    #+#             */
/*   Updated: 2026/03/23 11:33:22 by rribette         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Responses.hpp"

void	Responses::chan_mode_is(Client &client, Channel &channel)
{
	std::string nick = client.get_nickname();
	std::string chan = channel.get_full_name();
	std::string modestring = channel.get_modestring();
	std::string total = "324 " + nick + " " + chan + " " + modestring + "\r\n";

	client.send_message(total);
}

void	Responses::creation_time(Client &client, Channel &channel)
{
	std::string nick = client.get_nickname();
	std::string chan = channel.get_full_name();
	std::stringstream ss;
	ss << channel.get_creation_time();
	std::string total = "329 " + nick + " " + chan + " " + ss.str() + "\r\n";

	client.send_message(total);
}

void	Responses::no_such_channel(Client &client, std::string chan_name)
{
	std::string nick = client.get_nickname();
	std::string total = "404 " + nick + " " + chan_name + " :No such channel" + "\r\n";
	
	client.send_message(total);
}

void	Responses::chan_op_privs_needed(Client &client, Channel &channel)
{
	std::string nick = client.get_nickname();
	std::string chan = channel.get_full_name();
	std::string total = "482 " + nick + " " + chan + " :You're not channel operator" + "\r\n";

	client.send_message(total);
}

void	Responses::chan_is_full(Client &client, Channel &channel)
{
	std::string nick = client.get_nickname();
	std::string chan = channel.get_full_name();
	std::string total = "471 " + nick + " " + chan + " :Cannot join channel (+l)\r\n";

	client.send_message(total);
}

void	Responses::need_more_params(Client &client, Commands &cmd)
{
	std::string nick = client.get_nickname();
	std::string command = cmd.cmd;
	for (size_t i = 0; i < cmd.param.size(); i++)
	{
		command.append(" ");
		command.append(cmd.param[i]);
	}
	std::string total = "461 " + nick + " " + command + " :Not enough parameters\r\n";

	client.send_message(total);
}

void	Responses::already_registered(Client &client)
{
	std::string nick = client.get_nickname();
	std::string total = "462 " + nick + " :You may not reregister\r\n";

	client.send_message(total);
}


void	Responses::who_reply(Client &client, Channel &channel, Client &msg_target)
{
	std::string nick = client.get_nickname();
	std::string chan = channel.get_full_name();
	std::string user = client.get_username();
	std::string host = client.get_hostname();
	std::string real = client.get_realname();
	std::string total = "352 " + nick + " " + chan + " " + user + " " + host + " ircserv " + nick + " H :0 " + real + "\r\n";

	msg_target.send_message(total);
}

void	Responses::end_of_who(Client &client, std::string &mask)
{
	std::string nick = client.get_nickname();
	std::string total = "315 " + nick + " " + mask + " :End of WHO list\r\n";

	client.send_message(total);
}
