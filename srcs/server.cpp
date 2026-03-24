/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gougier <gougier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 13:29:01 by gougier           #+#    #+#             */
/*   Updated: 2026/03/24 14:26:37 by rribette         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

Server::Server()
{
	server_socket_ = -1;
	creation_time = time(NULL);
	// std::cout << "constructor server created" << std::endl;
}

Server::~Server()
{
	//std::cout << "entering server destructor" << std::endl;
	//::close(this->server_socket_);
	// std::cout << "destructor server created" << std::endl;
}

Server::Server(Server const &other)
{
	server_socket_ = other.server_socket_;
	creation_time = other.creation_time;
}

Server&Server::operator=(const Server &rhs)
{
	if (this != &rhs)
	{
		server_socket_ = rhs.server_socket_;
		creation_time = rhs.creation_time;
	}
	return (*this);
}

/* exceptions functions */
Server::Error::Error(const std::string& msg)
{
	msg_ = "Error : " + msg;
}

const char *Server::Error::what() const throw()
{
	return (msg_.c_str());
}


/* init functions */
void Server::init_port(std::string av1)
{
	if (av1.empty())
		throw Error("Port is empty");
	//if (!av1.find_first_not_of("0123456789") != std::string::npos) throw Error("Port is not only digit");
	for (unsigned int i = 0; i < av1.size(); i++)
	{
		if (!std::isdigit(av1[i]))
			throw Error("Port is not only digit");
	}
	//if (av1.size() > std::to_string(INT_MAX).size())  TODO Make something like that but for c++98
	//	throw Error("Port range overflow : (Must be between 1025 and 65535)");
	long port = std::atol(av1.c_str());
	if (port <= 1024 || port > 65535) // a verifier
		throw Error("Port range is not good range (Must be between 1025 and 65535)");
	this->port_ = static_cast <int>(port);
}

void Server::init_password(std::string av2)
{
	if (av2.empty())
		throw Error("Password is empty");
	for (unsigned int i = 0; i < av2.size(); i++)
	{
		if (std::isspace(av2[i]))
			throw Error("Password has spaces");
	}
	this->password_ = (av2);
}

//ouverture dun socket
//lier socket a ip et un port d ecoute
//ecoute de demande de connexion
void Server::init_data()
{
	sockaddr_in myaddr;
	socklen_t __optlen = 1;

	this->server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
	if (this->server_socket_ < 0)
		throw Error("socket");
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(this->port_);
	myaddr.sin_addr.s_addr = INADDR_ANY;
	if(setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &__optlen, sizeof(__optlen)) == -1)
		throw Error("setsockpt");
	if (bind(this->server_socket_, (struct sockaddr*)&myaddr, sizeof(myaddr)) == -1)
		throw Error("bind");
	if (listen(this->server_socket_, 1) == -1)
		throw Error("listen");
	this->init_poll_fd(this->server_socket_);
	last_time_ping_ = time(NULL);
}

//remplissage de la structure de fd pour poll
void Server::init_poll_fd(int socket_fd)
{
	pollfd poll_fd;

	poll_fd.fd = socket_fd;
	poll_fd.events = POLLIN;
	poll_fd.revents = 0;
	this->fds.push_back(poll_fd);
}

//poll sonde les tentatives de connexion
//soit il y a nv client
//soit client deja existant tente de communiauer
void Server::poll_loop()
{
	while (runFlag)
	{
		time_t now = time(NULL);
		if (now - last_time_ping_ > 30)
		{
			check_clients_connection();
			ping_clients();
			last_time_ping_ = now;
		}
		init_sig_handler();
		int nbevents = poll(&fds[0], fds.size(), -1);
		if (nbevents < 0)
			throw Error("CTRL+C");
		for (size_t i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
			{
				if (fds[i].fd == this->server_socket_)
					new_client();
				else
					send_and_recv(i);
			}
		}
	}
	for (size_t i = 0; i < fds.size(); i++)
		close(fds[i].fd);
}

//si connexion accepte
//envoyer le nv client ds un vecteur de clients av son socket fd attribue par accept et son addr ip
void Server::new_client()
{
	socklen_t 	addr_len;
	sockaddr_in myaddr;

	addr_len = sizeof(myaddr);
	int new_socket_fd = accept(server_socket_, (struct sockaddr *)&myaddr, &addr_len);
	std::cout << "new socket : " << new_socket_fd << std::endl;
	if (new_socket_fd < 0)
		throw Error("socket");
	init_poll_fd(new_socket_fd);
	std::cout << "New client : " << new_socket_fd <<  std::endl;
	Client new_client(new_socket_fd, inet_ntoa(myaddr.sin_addr));
	clients.push_back(new_client);
}


//recoie data et met ds un buffer 
//puis cherche cmd pour envoyer au parsing
void Server::send_and_recv(int pollidx)
{
	Client 		&client(this->clients[pollidx - 1]);

	int nb_char;
	char buffer[512];
	nb_char = recv(this->fds[pollidx].fd, buffer, 512, 0);
	if (nb_char <= 0)
		return disconnect(this->clients[pollidx - 1].get_socket_fd());
	buffer[nb_char]= 0;
	if (nb_char > 1)
		std::cout << COLOR_YELLOW << this->clients[pollidx - 1].get_socket_fd() << " : " << buffer << COLOR_DFL << std::endl;
	std::string msg(buffer);
	client.append_to_buffer(msg, *this);
}

void sig_handler(int signum)
{
	//std::cout << "sigint has been caught : " << signum << " : " << SIGINT << std::endl;
	if (signum == SIGINT)
		runFlag = 0;
}

int Server::init_sig_handler(void)
{
	struct sigaction new_action;

	new_action.sa_handler = &sig_handler;
	if (sigemptyset(&(new_action.sa_mask)) != 0)
		return (-1);
	new_action.sa_flags = 0;
	//new_action.sa_sigaction = NULL;
	//new_action.sa_restorer = NULL;
	return (sigaction(SIGINT, &new_action, NULL));
}

int Server::init_sig_ignore(void)
{
	struct sigaction new_action;

	new_action.sa_handler = SIG_DFL;
	if (sigemptyset(&(new_action.sa_mask)) != 0)
		return (-1);
	new_action.sa_flags = 0;
	return (sigaction(SIGINT, &new_action, NULL));
}

/* Debug */
void Server::print()
{
	for (size_t i = 0; i < clients.size(); ++i)
	{
		std::cout << "Socket fd du client n" << i + 1 << " = ";
		std::cout << clients[i].get_socket_fd() << std::endl;
	}	
	// std::cout << std::endl;
}

void Server::print_channels()
{
	std::map<std::string, Channel>::iterator it = channels.begin();
	for (; it != channels.end(); it++)
	{
		std::cout << "channel name = " << it->second.get_name() << std::endl;
		std::vector<Client*> clients_in_channel = it->second.get_clients();
		for (size_t i = 0; i < clients_in_channel.size(); i++)
		{
			std::cout << "client inside channel = " << clients_in_channel[i]->get_nickname() << std::endl;
		}
	}
}

void Server::print_fds()
{
	std::cout << "fds size : " << this->fds.size() << std::endl;
	for (size_t i = 0; i < this->fds.size(); i++)
	{
		std::cout << this->fds[i].fd << " : " << this->fds[i].revents << std::endl;
	}
}

/* Getters */
int Server::get_port()
{
	return (this->port_);
}

std::string Server::get_password()
{
	return (this->password_);
}

void Server::set_password(std::string pass)
{
	password_ = pass;
}

//TODO ajout de send quit ou error qd server shutdown
void Server::close_all()
{
	for (size_t i = 0; i < clients.size(); i++)
	{
		std::string msg_complete = ":ircserv QUIT :Server was shutdown\r\n";
		send(clients[i].get_socket_fd(), msg_complete.c_str(), msg_complete.length(), 0);
		close(clients[i].get_socket_fd());
	}
	if (server_socket_ != -1)
		close(server_socket_);
}

void Server::disconnect(int socket_fd)
{
	close(socket_fd);
	std::deque<Client>::iterator it = clients.begin();
	for (; it != clients.end(); it++)
	{
		if (it->get_socket_fd() == socket_fd)
			break ;
	}
	if (it != clients.end())
	{
		it->leave_all_channels(channels);
		clients.erase(it);
	}
	std::vector<pollfd>::iterator it_poll = fds.begin();
	for (; it_poll != fds.end(); it_poll++)
	{
		if (it_poll->fd == socket_fd)
			break ;
	}
	if (it_poll != fds.end())
		fds.erase(it_poll);
	// TODO : mettre fonction QUIT pr quit les channels et le notifer aux clients de ces channels
}

void Server::debug(void) const
{
	std::cout << COLOR_BLUE << "==========[DEBUG SERVER]==========" << std::endl;
	std::cout << "port : " << port_ << ", password : " << password_ << ", socket : " << server_socket_ << std::endl;
	for (size_t i = 0; i < clients.size(); i++)
		clients[i].debug();
	std::cout << COLOR_BLUE;
}

void Server::ping_clients(void)
{
	std::deque<Client>::iterator it = clients.begin();
	for (; it != clients.end(); it ++)
	{
		std::string msg = "PING :ircserv\r\n";
		send (it->get_socket_fd(), msg.c_str(), msg.length(), 0);
		// std::cout << "ping sent" << std::endl;
	}
}
void Server::check_clients_connection(void)
{
	std::deque<Client>::iterator it = clients.begin();
	while (it != clients.end())
	{
		if (!it->answered_pong_)
		{
			Client &client = *it;
			Commands cmd;
			cmd.cmd = "QUIT";
			cmd.param.push_back("Ping timeout");
			it++;
			quit(client, cmd);
			break ;
		}
		else
		{
			it->answered_pong_ = false;
			it++;
		}
	}
}
