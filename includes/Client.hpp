/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gougier <gougier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 12:53:27 by gougier           #+#    #+#             */
/*   Updated: 2026/03/24 13:21:28 by rribette         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <deque>
#include "Commands.hpp"
#include "Channel.hpp"

class Server;
class Channel;

class Client
{
	private:
	int 		client_socket_;
	std::string	ip_address_;
	std::string username_;
	std::string nickname_;
	std::string realname_;
	std::string hostname_;
	std::string buffer_;
	std::deque<std::string> channels_;
	std::deque<std::string> invited_list_;
	public:
	Client(int client_socket_fd, std::string ip_address, std::string username, std::string nickname, std::string buffer);
	Client(int client_socket_fd, std::string ip_address);
	Client(const Client &rhs);
	~Client();
	
	Client& operator=(const Client& rhs);
	bool operator==(const Client& rhs);
	//public var
	bool 		is_registered_; //on en a besoin pour envoyer que 1 fois le msg au client
	bool 		pass_validated;
	int			last_ping_;
	bool		last_pong_;
	bool		waiting_pong_;
	bool		answered_pong_;
	void send_message(std::string &msg);
	void user(Commands &cmd);

	//debug
	void debug(void) const;
	void print_hexa(int c);
	void print_non_printable(std::string msg);

	//getters and setters	
	int			get_socket_fd() const;
	void		set_socket_fd(int fd);
	std::string	get_username() const;
	std::string	get_nickname() const;
	std::string	get_buffer() const;
	void 		set_nickname(std::string nickname);
	void 		set_username(std::string username);
	bool 		is_complete();
	void 		append_to_buffer(std::string msg,  Server &server);
	std::deque<std::string> &get_channels(void);
	std::deque<std::string> &get_invited_list(void);
	void		push_channel(std::string chan_name);
	void		leave_all_channels(std::map<std::string, Channel> &channels);
	void		remove_channel(std::string chan_name);

	std::string	get_realname() const;
	std::string	get_hostname() const;
	void		set_realname(std::string realname);
	void		set_hostname(std::string hostname);

};

#endif
