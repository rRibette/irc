/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gougier <gougier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 10:28:22 by gougier           #+#    #+#             */
/*   Updated: 2026/03/24 14:29:26 by gougier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"
#include "../includes/Server.hpp"

Channel::Channel()
{
}

Channel::Channel(std::string name)
{
	name_ = name;
	topic_ = "";
	pass_ = "";
	user_limit_ = -1;
	invite_only_ = false;
	topic_restricted_ = false;
	creation_time_ = time(NULL);
}

Channel::Channel(const Channel &other)
{
	name_ = other.name_;
	topic_ = other.topic_;
	pass_ = other.pass_;
	user_limit_ = other.user_limit_;
	invite_only_ = other.invite_only_;
	topic_restricted_ = other.topic_restricted_;
	creation_time_ = other.creation_time_;
}


Channel::Channel(std::string name, std::string pass)
{
	name_ = name;	
	topic_ = "";
	pass_ = pass;
	user_limit_ = -1;
	invite_only_ = false;
	topic_restricted_ = false;
	creation_time_ = time(NULL);
}

Channel::~Channel()
{

}

const std::string& Channel::get_name() const 
{
	return (name_);
}

/*
const std::string &Channel::get_true_name() const
{
	if (name_[0] != '#')
		return (name_);
	return (name_.substr(1, name_.length() - 1));
}
*/
std::vector<Client*>& Channel::get_clients()
{
	return (clients_);
}

const std::string 	&Channel::get_pass(void) const
{
	return  (pass_);
}

const std::map<std::string, bool> &Channel::get_operators(void) const
{
	return (operators_);
}

void	Channel::set_pass(std::string &pass)
{
	pass_ = pass;
}

const int &Channel::get_user_limit(void) const
{
	return (user_limit_);
}

void	Channel::set_user_limit(int limit)
{
	user_limit_ = limit;
}

const std::string Channel::get_modestring(void) const
{
	std::string modestring = "";
	std::string modeparams = "";
	std::string total = "";
	std::stringstream ss;

	if (invite_only_)
		modestring += "i";
	if (topic_restricted_)
		modestring += "t";
	if (!pass_.empty())
	{
		modestring += "k";
		modeparams += pass_;
	}
	if (user_limit_ > -1)
	{
		ss << user_limit_;
		modestring += "l";
		if (!modeparams.empty())
			modeparams += " ";
		modeparams += ss.str();
	}
	
	if (!modestring.empty())
		total += modestring;
	if (!modeparams.empty())
		total += " " + modeparams;
	return (total);
}

void Channel::list_operators(void) const
{
	std::map<std::string, bool>::const_iterator it;

	for (it = operators_.begin(); it != operators_.end(); it++)
		std::cout << it->first << " : " << it->second << std::endl;
}

const time_t &Channel::get_creation_time(void) const
{
	return (creation_time_);
}

int Channel::add_client(Client &client)
{
	size_t index = find_client(client);
	int op = 0;
	if (index != clients_.size())
		return (-1);
	if (user_limit_ != -1 && clients_.size() == static_cast<size_t>(user_limit_))
		return (-2);
	if (clients_.size() == 0)
		op = 1;
	//std::cout << "[DEBUG : add_client]" << std::endl;
	//client.debug();
	//debug();
	clients_.push_back(&client);
	operators_.insert(std::make_pair(client.get_nickname(), op));	
	return (0);
}

void Channel::who(Client &client, std::string mask)
{
	std::vector<Client*>::iterator it = clients_.begin();
	while (it < clients_.end())
	{
		Client &truc = **it;
		Responses::who_reply(truc, *this, client);
		it++;
	}
	Responses::end_of_who(client, mask);
}

//si le nom n existe pas ds notre map de channel alors on en crrer un
//soit on fait un default constr et on ajoute un set_name apres soit ca, a voir?
bool Server::join_channel(std::string chan_name, Client &client, std::string key)
{
	std::map<std::string, Channel>::iterator it = channels.find(chan_name);
	if (it == channels.end())
		it = channels.insert(std::make_pair(chan_name, Channel(chan_name))).first;
	if (it->second.is_full())
	{
		Responses::chan_is_full(client, it->second);
		return (false);
	}
	if (it->second.get_invite_only() && !check_invite_list(client.get_invited_list(), chan_name))
		return server_reply(client, ERR_INVITEONLYCHAN, chan_name, " :Cannot join channel (+i)\r\n"), false;
	if (it->second.good_password(key))
	{
		it->second.add_client(client);
		std::deque<std::string> &channels = client.get_channels();
		channels.push_back(chan_name);
		client.debug();
		it->second.debug();
		return (true);
	}
	else
	{
		server_reply(client, ERR_BADCHANNELKEY, chan_name, " :Cannot join channel (+k)\r\n");
		return (false);
	}
}

bool Channel::is_full(void)
{
	if (user_limit_ == -1)
		return (false);
	int nb = clients_.size();
	return (user_limit_ <= nb);
}

bool Channel::is_operator(Client &client)
{
	if (find_client(client) == clients_.size())
		return (false);
	return (operators_[client.get_nickname()]);
}

void Channel::remove_client(std::string nickname)
{
	std::vector<Client*>::iterator it = this->clients_.begin();
	while (it < this->clients_.end())
	{
		Client &client = **it;
		if (client.get_nickname() == nickname)
		{
			this->operators_.erase(nickname);
			if (count_operators() == 0)
				(this->operators_.begin())->second = true;
			client.remove_channel(this->name_);
			this->clients_.erase(it);
			break;
		}
		it++;
	}
}

size_t	Channel::count_operators(void)
{
	size_t cnt = 0;
	std::map<std::string, bool>::iterator it = this->operators_.begin();
	std::map<std::string, bool>::iterator end = this->operators_.end();
	while (it != end)
	{
		if (it->second)
			cnt++;
		it++;
	}
	return (cnt);
}

void Channel::print(void) const
{
	std::cout << "Channel name : " << this->name_ << std::endl;
	std::cout << "Clients in channel : " << std::endl;
	for (int i = 0; this->clients_.begin() + i != this->clients_.end(); i++)
		std::cout << "\t" << (*(this->clients_.begin() + i))->get_nickname() << std::endl;
}

void Channel::send_message(Client &source, std::string msg)
{
	size_t client = find_client(source);
	for (size_t i = 0; i < clients_.size(); i++)
	{
		if (i != client)
		{
			std::string full_msg = ":";
			full_msg.append(source.get_nickname());
			full_msg.append("!");
			full_msg.append(source.get_username());
			full_msg.append("@");
			full_msg.append("ircserv ");
			full_msg.append(msg);
			full_msg.append("\r\n");
			//std::cout << COLOR_BLUE << "[DEBUG : send_message] : " << full_msg << COLOR_DFL << std::endl;
			clients_[i]->send_message(full_msg);
		}
	}
}

size_t Channel::find_client(Client &client)
{
	//std::cout << "[DEBUG : find_client] client : " << client.get_nickname() << " : " << client.get_socket_fd() << std::endl;
	for (size_t i = 0; i < clients_.size(); i++)
	{
		if (clients_[i]->get_nickname() == client.get_nickname())
			return (i);
	}
	return (clients_.size());
}

int Channel::find_nickname(std::string &nickname)
{
	for (size_t i = 0; i < clients_.size(); i++)
	{
		if (clients_[i]->get_nickname() == nickname)
			return (i);
	}
	return (-1);
}

int Channel::make_operator(std::string &target)
{
	int idx = find_nickname(target);
	if (idx == -1)
	{
		return (-1);
	}
	operators_[target] = 1;
	return (0);
}

int	Channel::change_topic(Client &client, std::string &new_topic)
{
	size_t idx = find_client(client);
	if (idx == clients_.size())
		return (-1);
	if (topic_restricted_ && !operators_[client.get_nickname()])
	{
		Responses::chan_op_privs_needed(client, *this);
		return (-2);
	}
	topic_ = new_topic;
	return (0);
}

int Channel::handle_mode(Client &client, Commands &cmd)
{
	size_t client_index = find_client(client);
std::cout << COLOR_BLUE << "[DEBUG : handle_mode] : client : " << client_index << 
	", chan : " << cmd.param[0] << ";" << this->get_full_name() << "." <<  COLOR_DFL << std::endl;
	debug();
	if (client_index == clients_.size())
		return (-1);
	if (cmd.param.size() < 2)
	{
		Responses::chan_mode_is(client, *this);
		Responses::creation_time(client, *this);
		return (0);
	}
	if (operators_[client.get_nickname()] == false)
	{
		Responses::chan_op_privs_needed(client, *this);
		return (-2);
	}
	if (is_valid_modestring(cmd.param[1]) == -1)
		return (-3);
	size_t nbparams = params_needed(cmd.param[1]);
	if (cmd.param.size() != nbparams + 2)
		return (-4);
	return (read_mode(client, cmd.param));
}

int Channel::read_mode(Client &client, std::vector<std::string> params)
{
	std::string modestring = params[1];
	int idx = 2;
	int sign_flag = -1;
	(void)client;
	
	for (size_t i = 0; i < modestring.length(); i++)
	{
		if (modestring[i] == '+')
			sign_flag = 1;
		else if (modestring[i] == '-')
			sign_flag = 0;
		else if (modestring[i] == 'i')
			invite_only_ = sign_flag;
		else if (modestring[i] == 't')
			topic_restricted_ = sign_flag;
		else if (modestring[i] == 'o')
		{
			int user_idx = find_nickname(params[idx]);
			if (user_idx != -1)
				operators_[params[idx]] = sign_flag;
			idx++;
		}
		else if (modestring[i] == 'k')
		{
			if (!sign_flag)
				pass_ = "";
			else
			{
				pass_ = params[idx];
				idx++;
			}
		}
		else if (modestring[i] == 'l')
		{
			if (!sign_flag)
				user_limit_ = -1;
			else
				set_user_limit(params[idx++]);
		}
	}
	std::cout << "[DEBUG : read_mode] :" << get_modestring() << std::endl;
	list_operators();
	return (0);
}

int Channel::is_valid_modestring(std::string &modestring)
{
	int sign_flag = 0;
	std::string modes = "itokl";
	std::string found = "";

	//std::cout << "[DEBUG, is_valid_modestring] : modestring : " << modestring << std::endl;
		
	if (modestring.find_first_not_of("+-itokl", 0) != std::string::npos)
		return (-1);
	if (check_signs_modestring(modestring) == 1)
		return (-1);
	for (size_t i = 0; i < modestring.length(); i++)	
	{
		if (modestring[i] == '+' || modestring[i] == '-')
			sign_flag = 1;
		if (modes.find(modestring[i]) != std::string::npos)
		{
			if (!sign_flag)
				return (-1);
			if (found.find(modestring[i]) == std::string::npos)
				found += modestring[i];
			else
				return (-1);
		}
	}
	return (found.length());	
}

int Channel::set_user_limit(std::string limit)
{
	int new_limit = 0;

	for (size_t i = 0; i < limit.length(); i++)
	{
		if (limit[i] >= '0' && limit[i] <= '9')
		{
			new_limit = new_limit * 10 + (limit[i] - '0');
			if (new_limit >= 128)
			{
				user_limit_ = 128;
				return (user_limit_);
			}
		}
		else
			return (-1);
	}
	//std::cout << "[DEBUG : set_user_limit] string : " << limit << ", int : " << new_limit << std::endl;
	user_limit_ = new_limit;
	return (user_limit_);
}

int Channel::check_signs_modestring(std::string &modestring)
{
	int plus = 0;
	int minus = 0;

	if (modestring[0] != '+' && modestring[0] != '-')
		return (1);
	for (size_t i = 0; i < modestring.length(); i++)
	{
		if (modestring[i] == '+' || modestring[i] == '-')
		{
			if (i > 0 && (modestring[i - 1] == '+' || modestring[i - 1] == '-'))
				return (1);
		}
		if (modestring[i] == '+')
		{
			if (plus == 1)
				return (1);
			plus = 1;
		}
		if (modestring[i] == '-')
		{
			if (minus == 1)
				return (1);
			minus = 1;
		}
	}
	return (0);
}

size_t Channel::params_needed(std::string &modestring)
{
	std::string modes = "kl";
	std::string found = "";
	int	sign_flag = 0;

	for (size_t i = 0; i < modestring.length(); i++)
	{
		if (modestring[i] == '+')
			sign_flag = 1;
		else if (modestring[i] == '-')
			sign_flag = -1;
		if (modes.find(modestring[i]) != std::string::npos)
		{
			if (sign_flag == 1 && found.find(modestring[i]) == std::string::npos)
				found += modestring[i];
		}
		if (modestring[i] == 'o')
		{
			if (found.find('o') == std::string::npos)
				found += modestring[i];
		}
	}
	return (found.length());
}

void Channel::debug(void) const
{
	std::cout << COLOR_BLUE << "==========[DEBUG CHANNEL]==========" << std::endl;
	std::cout << " CHANNEL : " << name_ << " (ADDRESS : " << this << ")" << std::endl;
	//list_operators();
	for (size_t i = 0; i < clients_.size(); i++)
		clients_[i]->debug();
	std::cout << COLOR_BLUE;
	std::cout << "topic : " << topic_ << ", pass : " << pass_ << std::endl;
	std::cout << "user_limit_ : " << user_limit_ << ", invite_only : " << invite_only_ << ", topic_restricted : " << topic_restricted_ << std::endl;
	std::cout << "==========[FIN DEBUG CHANNEL]==========" << COLOR_DFL << std::endl;
}

bool Channel::good_password(std::string &input)
{
	return (pass_.empty() || (pass_ == input));
}

Channel::ChannelException::ChannelException(const std::string& msg)
{
	msg_ = "Channel Error : " + msg;
}

const char *Channel::ChannelException::what() const throw()
{
	return (msg_.c_str());
}

std::string &Channel::get_topic()
{
	return (topic_);
}

void Channel::set_topic(std::string topic)
{
	topic_ = topic;
}

const bool	&Channel::get_invite_only(void) const
{
	return (invite_only_);
}

const bool	&Channel::get_topic_restricted(void) const
{
	return (topic_restricted_);
}

void	Channel::set_invite_only(bool flag)
{
	invite_only_ = flag;
}

void	Channel::set_topic_restricted(bool flag)
{
	topic_restricted_ = flag;
}

std::string Channel::get_full_name(void)
{
	std::string result = "#" + this->name_;
	return (result);
}
