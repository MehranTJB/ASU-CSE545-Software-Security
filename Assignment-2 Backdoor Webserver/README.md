## Backdoor Webserver

A critical part of establishing persistence on a system is to leave a “backdoor” that allows the hacker access to the system at a later date, without exploiting the same vulnerabilities (they may be fixed in the meantime). In this assignment, we will explore writing a backdoor that pretends to be a web server. A web server makes a great pretense for a backdoor, because web traffic is so prevalent it does not raise alarms and ports 80 and 443 are frequently permitted through firewalls. 

Make a minimal webserver based on RFC-2616 protocol.

Server Side                              Client Side
1: Create Socket                         1: Create Socket 
2: Intialize Socket
3: Bind
4: Listen                                2: Connect (Get Request) 
5: Accept
6: Send/Receive                          3: Send/Receive  
7: CTRL-C -> Exit and Release 
   ports and memories. 


command to execute (Server side): 
./normal_seb_server <port_number>    

Client side:
curl -X GET "http://localhost:1234/exec/ls%20-la"

Source code:
main()
- Create Socket 
- Initialize Socket  
- Bind -> call bind() function
- Listen -> call listen() function
- Accept -> call accept_command() function
- Check input (Only GET request is allowed)
- Process input -> call extract_command() function
- Send response -> call send() function
- Close socket
- Check for CTRL-C -> call SIGINIT_callback_handler() function / 
  clear memories, close socket, release port