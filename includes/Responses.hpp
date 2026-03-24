/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Responses.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gougier <gougier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 16:12:45 by rribette          #+#    #+#             */
/*   Updated: 2026/03/24 10:27:27 by rribette         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSES_HPP
# define RESPONSES_HPP

#include <iostream>
#include <sstream>
#include "Channel.hpp"
#include "Client.hpp"
#include "Commands.hpp"

#define COLOR_BLUE "\e[0;96m"
#define COLOR_DFL "\e[0m"
#define COLOR_RED "\e[0;30m"
#define COLOR_GREEN "\e[0;32m"
#define COLOR_YELLOW "\e[0;33m"


class Channel;
class Client;

namespace Responses
{
	void chan_mode_is(Client &client, Channel &channel);
	void creation_time(Client &client, Channel &channel);
	void no_such_channel(Client &client, std::string chan_name);
	void chan_op_privs_needed(Client &client, Channel &channel);
	void need_more_params(Client &client, Commands &cmd);
	void chan_is_full(Client &client, Channel &channel);
	void who_reply(Client &client, Channel &channel, Client &msg_target);
	void end_of_who(Client &client, std::string &mask);
	void already_registered(Client &client);
};

//MESSAGES
# define RPL_WELCOME 001
# define RPL_YOURHOST 002
# define RPL_CREATED 003
# define RPL_MYINFO 004

//PASS command
# define ERR_NEEDMOREPARAMS 461
# define ERR_ALREADYREGISTERED 462
# define ERR_PASSWDMISMATCH 464

//NICK command
# define ERR_NONICKNAMEGIVEN 431
# define ERR_ERRONEUSNICKNAME 432
# define ERR_NICKNAMEINUSE 433
# define ERR_NICKCOLLISION 436

//PING command
# define ERR_NEEDMOREPARAMS 461
# define ERR_NOORIGIN 409

//OPER command
# define ERR_NOOPERHOST 491
# define RPL_YOUREOPER 381

//JOIN command
# define ERR_NOSUCHCHANNEL 403
# define ERR_TOOMANYCHANNELS 405
# define ERR_BADCHANNELKEY 475
# define ERR_BANNEDFROMCHAN 474
# define ERR_CHANNELISFULL 471
# define ERR_INVITEONLYCHAN 473
# define ERR_BADCHANMASK 476
# define RPL_TOPIC 332
# define RPL_TOPICWHOTIME 333
# define RPL_NAMREPLY 353
# define RPL_ENDOFNAMES 366
# define CHANLIMIT 3

//PART command
# define ERR_NOTONCHANNEL 442

//TOPIC command
# define ERR_CHANOPRIVSNEEDED 482
# define RPL_NOTOPIC 331

//LIST command
# define RPL_LISTSTART 321
# define RPL_LIST 322
# define RPL_LISTEND 323

//INVITE command
# define RPL_INVITING 341
# define ERR_USERONCHANNEL 443

//KICK command
# define ERR_USERNOTINCHANNEL 441

//MOTD command
# define ERR_NOSUCHSERVER 402
# define ERR_NOMOTD 422
# define RPL_MOTDSTART 375
# define RPL_MOTD 372
# define RPL_ENDOFMOTD 376

//VERSION command
# define RPL_ISUPPORT 005
# define RPL_VERSION 351

//ADMIN command
# define PRL_ADMINME 356
# define RPL_ADMINLOC1 257
# define RPL_ADMINLOC2 258
# define RPL_ADMINEMAIL 259

//CONNECT command
# define ERR_NOPRIVILEGES 481
# define ERR_NOPRIVS 723

//LUSERS command
# define RPL_LUSERCLIENT 251
# define RPL_LUSEROP 252
# define RPL_LUSERUNKNOWN 253
# define RPL_LUSERCHANNELS 254
# define RPL_LUSERME 255
# define RPL_LOCALUSERS 265
# define RPL_GLOBALUSERS 266

//PRIVMSG command
# define ERR_NOSUCHNICK 401
# define ERR_CANNOTSENDTOCHAN 404
# define ERR_TOOMANYTARGETS 407
# define ERR_NORECIPIENT 411
# define ERR_NOTEXTTOSEND 412
# define ERR_NOTOPLEVEL 413
# define ERR_WILDTOPLEVEL 414
# define RPL_AWAY 301

//WHO command
# define RPL_WHOREPLY 352
# define RPL_ENDOFWHO 315

//WHOIS command
# define RPL_WHOISCERTFP 276
# define RPL_WHOISUSER 311
# define PRL_WHOISSERVER 312
# define RPL_WHOISOPERATOR 313
# define RPL_WHOISIDLE 317
# define RPL_WHOISCHANNELS 319
# define RPL_WHOISSPECIAL 320
# define RPL_WHOISACCOUNT 330
# define RPL_WHOISACTUALLY 338
# define RPL_WHOISHOST 378
# define RPL_WHOISMODES 379
# define RPL_WHOISSECURE 671


#endif
