/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gougier <gougier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/02 13:17:43 by gougier           #+#    #+#             */
/*   Updated: 2026/03/24 13:26:50 by rribette         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"
#include "../includes/Server.hpp"

Client::Client(int client_socket_fd, std::string ip_addr, std::string username, std::string nickname, std::string buffer)
{
	this->client_socket_ = client_socket_fd;
	this->ip_address_ = ip_addr;
	this->username_ = username;
	this->nickname_ = nickname;
	this->realname_ = "";
	this->hostname_ = "localhost";
	this->is_registered_ = false;
	this->buffer_ = buffer;
	this->pass_validated = false;
	this->answered_pong_ = true;
	// std::cout << "constructor client created" << std::endl;
}

Client::Client(int client_socket_fd, std::string ip_addr)
{
	this->client_socket_ = client_socket_fd;
	this->ip_address_ = ip_addr;
	this->username_ = "";
	this->nickname_ = "";
	this->realname_ = "";
	this->hostname_ = "localhost";
	this->is_registered_ = false;
	this->buffer_ = "";
	this->pass_validated = false;
	this->answered_pong_ = true;
}

Client::Client(const Client &rhs)
{
	this->client_socket_ = rhs.client_socket_;
	this->ip_address_ = rhs.ip_address_;
	this->username_ = rhs.username_;
	this->nickname_ = rhs.nickname_;
	this->realname_ = rhs.realname_;
	this->hostname_ = rhs.hostname_;
	this->is_registered_ = rhs.is_registered_;
	this->buffer_ = rhs.buffer_;
	this->pass_validated = rhs.pass_validated;
	this->answered_pong_ = rhs.answered_pong_;
}

Client::~Client()
{
//	close(this->client_socket_);
	// std::cout << "destructor client created" << std::endl;
}

Client& Client::operator=(const Client& rhs)
{
	if (this != &rhs)
	{
		this->client_socket_ = rhs.client_socket_;
		this->ip_address_ = rhs.ip_address_;
		this->username_ = rhs.username_;
		this->nickname_ = rhs.nickname_;
		this->realname_ = rhs.realname_;
		this->hostname_ = rhs.hostname_;
		this->is_registered_ = rhs.is_registered_;
		this->buffer_ = rhs.buffer_;
		this->pass_validated = rhs.pass_validated;
	}
	return (*this);
}

bool Client::operator==(const Client &rhs)
{
	return (this->username_ == rhs.username_);
}

void Client::user(Commands &cmd)
{
	std::vector<std::string> param = cmd.param;
	size_t size = param.size();
	if (size < 1 || param[0].size() <= 1)
	{
		Responses::need_more_params(*this, cmd);
		return ;
	}
	this->username_ = param[0];
	size_t i = 1;
	this->hostname_ = "localhost";
	while (i < size)
	{
		if (param[i][0] == ':')
		{
			this->realname_ = param[i].substr(1, std::string::npos);
			for (size_t j = i + 1; j < size; j++)
				this->realname_.append(param[j]);
		}
		i++;
	}
}

void Client::leave_all_channels(std::map<std::string, Channel> &channels)
{
	std::deque<std::string>::iterator it = this->channels_.begin();
	while (it < this->channels_.end())
	{
		channels[*it].remove_client(this->nickname_);
		it++;
	}
}

int Client::get_socket_fd() const
{
	return (client_socket_);
}

void Client::set_socket_fd(int fd)
{
	client_socket_ = fd;
}

std::string Client::get_username() const
{
	return (username_);
}

std::string Client::get_nickname() const
{
	return (nickname_);
}

std::string Client::get_buffer()const
{
	return (buffer_);
}

void Client::set_nickname(std::string nickname)
{
	nickname_ = nickname;
}

void Client::set_username(std::string username)
{
	username_ = username;
}

bool Client::is_complete()
{
	if (username_.empty() || (nickname_.empty()) || !pass_validated)
		return (false);
	return (true);
}

void Client::send_message(std::string &msg)
{
	if (msg.empty())
		return ;
	std::cout << COLOR_GREEN << nickname_ << " : " << msg << COLOR_DFL << std::endl;
	send(client_socket_, msg.c_str(), msg.length(), 0);
}

void Client::print_hexa(int c)
{
	std::string alpha = "0123456789abcdef";
	if (c < 16)
		std::cout << alpha[c];
	else
	{
		print_hexa(c / 16);
		print_hexa(c % 16);
	}
}

void Client::print_non_printable(std::string msg)
{
	for (size_t i = 0; i < msg.length(); i++)
	{
		if (msg[i] >= 32 && msg[i] <= 126)
			std::cout << msg[i];
		else
		{
			std::cout << "0x";
			print_hexa(msg[i]);
		}
	}
}

void Client::append_to_buffer(std::string msg, Server &server)
{
	Commands	cmd;

	buffer_.append(msg);
	size_t found;
	std::string line;
	while ((found = buffer_.find("\n")) != std::string::npos)
	{
		if (buffer_[found - 1] == '\r')
		{
			line = buffer_.substr(0, found - 1);
			buffer_.erase(0, found + 1);
		}
		else
		{
			line = buffer_.substr(0, found);
		buffer_.erase(0, found + 1);
		}
		cmd = server.search_cmd(line);
		server.handle_cmd(cmd, *this);
	}
}

void Client::debug(void) const
{
	std::cout << COLOR_BLUE << "==========[DEBUG CLIENT]==========" << std::endl;
	std::cout << "ADDRESS : " << this << std::endl;
	std::cout << "Username : " << username_ << ", nickname : " << nickname_ << std::endl;
	std::cout << "socket : " << client_socket_ << ", ip : " << ip_address_ << std::endl;
	std::cout << "buffer : " << buffer_ << std::endl;
	std::cout << "channels : " << std::endl;
	if (!channels_.empty())
	{
		for (size_t i = 0; i < channels_.size(); i++)
			std::cout << '\t' << channels_[i] << std::endl;
	}
	std::cout << "==========[FIN DEBUG CLIENT]==========" << COLOR_DFL << std::endl;
}

std::deque<std::string> &Client::get_channels(void)
{
	return (channels_);
}

std::deque<std::string> &Client::get_invited_list(void)
{
	return (invited_list_);
}


void Client::push_channel(std::string chan_name)
{
	channels_.push_back(chan_name);
}

std::string	Client::get_realname() const
{
	return (this->realname_);
}

std::string	Client::get_hostname() const
{
	return (this->hostname_);
}

void		Client::set_realname(std::string realname)
{
	this->realname_ = realname;
}

void		Client::set_hostname(std::string hostname)
{
	this->hostname_ = hostname;
}

void		Client::remove_channel(std::string chan_name)
{
	std::deque<std::string>::iterator it = this->channels_.begin();
	while (it < this->channels_.end())
	{
		if (*it == chan_name)
		{
			this->channels_.erase(it);
			break;
		}
		it++;
	}
}
