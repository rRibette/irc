/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gougier <gougier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 12:16:01 by gougier           #+#    #+#             */
/*   Updated: 2026/03/19 15:49:57 by rribette         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h> //for socket
#include <poll.h> //for poll
#include <netinet/in.h> //for struct sockaddr_in
#include <arpa/inet.h> //for inet_ntoa
#include <netinet/tcp.h>
#include <string.h>
#include <vector>
#include <deque>
#include <algorithm>
#include <map>
#include "Client.hpp"
#include "fcntl.h" //for fcntl
#include "Channel.hpp"
#include "Commands.hpp"
#include "ctime"

static volatile sig_atomic_t runFlag = 1;
void sig_handler(int signum);

class Server
{
	private:
	int 		port_;
	std::string password_;
	int			server_socket_;
	time_t		last_time_ping_;
	time_t		creation_time;
	std::vector<pollfd> fds;
	std::deque<Client> clients;
	std::map<std::string, Channel> channels;

	public:
	Server();
	~Server();
	Server(const Server &rhs);
	Server& operator=(const Server& rhs);
	
	//member functions
	void init_port(std::string av1);
	void init_password(std::string av2);
	void init_data();
	void poll_loop();
	void new_client();
	void init_poll_fd(int socket_fd);
	void send_and_recv(int pollidx);
	bool join_channel(std::string name, Client &client, std::string key);
	Commands search_cmd(std::string &msg);
	void handle_cmd(Commands &cmd, Client &client);
	void who(Client &client, Commands &cmd);

	int init_sig_handler(void);
	int init_sig_ignore(void);
	void send_msg_to_client(Client &client);

	void authentification(Client &client);
	void play_ping_pong(Client &client);
	void invite_client(Client &client, Commands &cmd);
	bool channel_exists(std::string channel_name);
	bool client_is_in_channel(std::string channel_name, Client &client);
	bool user_is_in_channel(std::string channel_name, std::string nickname);
	void not_registered_yet(Client &client);
	void kick_client(Client &client, Commands &cmd);
	int	 handle_mode(Client &client, Commands &cmd);
	void topic(Client &client, Commands &cmd);
	void clear_topic(std::string channel_name);
	void notify_all_topic_changed(std::string channel_name, std::string msg);
	void server_reply(Client &client, int code, const std::string &middle, const std::string &trailing);
	void pass(Client &client, Commands &cmd);
	void nick(Client &client, Commands &cmd);
	bool in_use_on_network(std::string nick);
	void close_all();
	void disconnect(int socket_fd);
	void quit(Client &client, Commands &cmd);
	void msg_to_shared_channels(Client &client, std::string msg);
	void join(Client &client, Commands &cmd);
	void priv_msg(Client &client, Commands &cmd);
	void msg_to_all_in_channel(std::string channel_name, Commands &cmd, Client &client);
	void msg_user_to_user(Client &client, Commands &cmd, Client *target);
	Client *find_target(std::string nick);
	void broadcast(std::string channel_name, Commands &cmd, Client &client);
	void ping_clients(void);
	void check_clients_connection(void);
	void pong(Client &client);
	std::deque<std::string> split(const std::string &s, const std::string &delimiter);
	void part(Client &client, std::string chan_name);
	void join_msg(std::string channel_name, Client &client);
	bool check_invite_list(std::deque<std::string> invited_list_, std::string chan_name);

	//debug
	void print();
	void print_fds();
	void print_channels();
	void debug(void) const;

	//getters setters
	int 		get_port();
	std::string get_password();
	void 		set_password(std::string pass);

	class Error : public std::exception
	{
		private:
		std::string msg_;
		public:
		Error(const std::string& msg);
		virtual ~Error() throw(){};
		const char* what() const throw();
	};
};

#endif
