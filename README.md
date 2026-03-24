*This project has been created as part of the 42 curriculum by gougier and rribette.*

# Description
The goal of this project was to recreate an irc server, ready to work for several irc clients. Internet Relay Chat or IRC is a text-based communication protocol on the Internet.
It offers real-time messaging that can be either public or private. Users can exchange direct messages and join group channels.
IRC clients connect to IRC servers in order to join channels.
IRC servers are connected together to form a network.
The implementation was made for irssi as client.
The listed command below were required and implemented :
```bash
`KICK` - Eject a client from the channel
`INVITE` - Invite a client to a channel
`TOPIC` - Change or view the channel topic
`MODE` - Change the channel’s mode:
· i: Set/remove Invite-only channel
· t: Set/remove the restrictions of the TOPIC command to channel
operators
· k: Set/remove the channel key (password)
· o: Give/take channel operator privilege
· l: Set/remove the user limit to channel
`JOIN` - Join a channel
```

# Instructions
To test this project :
```bash
`git clone` <repo_url>
`cd` <repo_name>
`make` at the root of the repo
./ircserv port_nb password (for exemple : `./ircserv 6667 password_test`)
Server should be running, in another terminal either try with netcat :
`nc localhost 6667`
You need to first authenticate with
`PASS password_test`
`NICK nickname_test`
`USER username_test`
or try with irssi :
sudo apt  install irssi  # version 1.2.3-1ubuntu4
enter irssi
/connect localhost port_nb password_test nickname_test
You should be now ready to try the listed commands above
```

# Resources

- https://modern.ircdocs.horse/
- https://datatracker.ietf.org/doc/html/rfc2812#section-5
- https://opensource.com/sites/default/files/gated-content/cheat_sheet_irc_v2.pdf
- https://irssi.org/User-interface/
- https://www.irchelp.org/protocol/rfc/rfc2811.txt
- https://ftp.gnu.org/old-gnu/Manuals/glibc-2.2.3/html_node/libc_toc.html
- AI was used in this project to explain the general structure and some debug.