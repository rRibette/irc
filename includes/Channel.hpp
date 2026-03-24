/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gougier <gougier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 10:26:13 by gougier           #+#    #+#             */
/*   Updated: 2026/03/24 14:08:07 by rribette         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <vector>
#include <map>
#include "Commands.hpp"
#include "Client.hpp"
#include "Responses.hpp"
#include <sstream>
//#include "Server.hpp"

//#define INV_ONLY 0b00000001
//#define TOP_OP	0b00000010

class Client;

class Channel
{
	private:
	std::string name_;
	std::vector<Client*> clients_;
	std::map<std::string, bool> operators_;
	time_t creation_time_;
	std::string topic_;
	std::string pass_;
	int			user_limit_;
	bool		invite_only_;
	bool		topic_restricted_;

	public:
	Channel();
	Channel(std::string name);
	Channel(std::string name, std::string pass);
	Channel(const Channel &other);
	~Channel();

	//member functions
	int add_client(Client &client);
	void remove_client(std::string nickname_);
	void send_message(Client &source, std::string msg);
	size_t find_client(Client &client);
	int	find_nickname(std::string &nickname);
	bool good_password(std::string &input);
	bool is_operator(Client &client);
	int	make_operator(std::string &target);
	int	change_topic(Client &client, std::string &new_topic);
	void who(Client &client, std::string mask);

	int handle_mode(Client &client, Commands &cmd);
	int read_mode(Client &client, std::vector<std::string> params);
	int	check_signs_modestring(std::string &modestring);
	int is_valid_modestring(std::string &modestring);
	size_t params_needed(std::string &modestring);
	const std::string get_modestring(void) const;
	bool is_full(void);

	//debug
	void print(void) const;
	void list_operators(void) const;
	void debug(void) const;

	//getters and setters
	const std::string				&get_name() const;
	std::string				get_full_name();
	std::vector<Client*>				&get_clients();
	const std::string				&get_pass() const;
	void								set_pass(std::string &pass);
	const std::map<std::string, bool> &get_operators() const;
	size_t							count_operators();
	const int						&get_user_limit() const;
	void								set_user_limit(int limit);
	int								set_user_limit(std::string limit);
	const bool							&get_invite_only() const;
	const bool							&get_topic_restricted() const;
	void							set_invite_only(bool flag);
	void							set_topic_restricted(bool flag);
	const time_t					&get_creation_time() const;

	class ChannelException : public std::exception
	{
		private :
			std::string msg_;
		public :
			ChannelException(const std::string &msg);
			virtual ~ChannelException() throw(){};
			const char *what() const throw();
	};
	std::string 				&get_topic();
	void 						set_topic(std::string topic);
};

#endif
