/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gougier <gougier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 12:13:18 by gougier           #+#    #+#             */
/*   Updated: 2026/02/26 09:54:42 by rribette         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

#include <signal.h>

//#include <cerrno>
//#include <cstring>

int main(int ac, char **av)
{
	if (ac != 3)
		return (std::cerr << "Program argument must be ./ircserv <port> <password>" << std::endl, 1);
	Server server;
	try
	{
		server.init_port(av[1]);
		server.init_password(av[2]);
		server.init_data();
		server.poll_loop();
	}
	catch(const std::exception& e)
	{
		server.close_all();
		std::cerr << e.what() << '\n';
		return (1);
	}
}
