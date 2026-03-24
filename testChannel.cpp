/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   testChannel.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rribette <rribette@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 13:31:31 by rribette          #+#    #+#             */
/*   Updated: 2026/02/26 18:23:13 by rribette         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "./includes/Channel.hpp"
#include "./includes/Client.hpp"
#include "./includes/Commands.hpp"


int main(int argc, char *argv[])
{
	std::vector<std::string> params;
	Client alice(3, "127.0.0.0", "alice", "BigBoss", "");
	Client bob(4, "1.1.1.1", "bob", "LittleBoss", "");
	Client charles(5, "localhost", "charles", "TheLastOne", "");
	
	Channel chan("besties");
	Commands	com;

	com.cmd = "test";


	for (int i = 0; i < argc; i++)
		params.push_back(argv[i]);
	com.param = params;
	chan.set_topic_restricted(1);
	try
	{
		chan.add_client(alice);
		//chan.set_user_limit(1);
		chan.add_client(bob);
		//chan.send_message(charles, "Yop");
		//chan.remove_client("TheLastOne");
		//chan.remove_client("LittleBoss");
		chan.add_client(charles);
		int res = chan.handle_mode(alice, com);
		if (res < 0)
			std::cout << "error : " << res << std::endl;
		std::cout << chan.get_modestring() << std::endl;
		chan.debug();
	//	std::cout << chan.is_operator(alice) << " : " << chan.is_operator(charles) << std::endl;
	}
	catch (std::exception &ex)
	{
		std::cout << ex.what() << std::endl;
	}
}

