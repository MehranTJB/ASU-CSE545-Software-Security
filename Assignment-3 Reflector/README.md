## Reflector

The goal of this project is to create a “reflector” which will relaunch attacks sent to a given IP address and ethernet address to the IP address that sent the attack. This acts as a mirror, such that when an adversary is ports canning a network, they will actually be port scanning themselves. When they launch an exploit at the reflector, the attack will be reflected back at them. 

Reflector traffic -  Which acts as a mirror, such that when an adversary is portscanning a network, 
they will actually be port scanning themselves. When they launch an exploit at the reflector, 
the attack will be reflected back at them. 

This program written in Python3 and use Scapy module which can listen to the given network interface and  
any IP, TCP or UDP packets sent to the given Victim IP address can be sent back to the attacker.