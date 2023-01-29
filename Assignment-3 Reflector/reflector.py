#!/usr/bin/python3
import sys
import getopt
import traceback
from scapy.all import *
inter=0

def reflectorcallback(packet):
	try:
		if ARP in packet:
			if packet[ARP].pdst == ip_victim:
				arppkt = Ether(src = ether_victim, dst = packet[Ether].src) / ARP(op = 2, hwsrc = ether_victim, psrc = ip_victim, hwdst = packet[ARP].hwsrc, pdst=packet[ARP].psrc)
				sendp(arppkt, iface = inter)
			elif packet[ARP].pdst == ip_reflector:
				arppkt = Ether(src = ether_reflector, dst = packet[Ether].src) / ARP(op = 2, hwsrc = ether_reflector, psrc = ip_reflector, hwdst=packet[ARP].hwsrc, pdst = packet[ARP].psrc)
				sendp(arppkt, iface = inter)
		else:		
			if packet[IP].dst == ip_victim:
				attackerIP = packet[IP].src
				attackerEther = packet[Ether].src
				packet[Ether].dst = attackerEther
				packet[Ether].src = ether_reflector
				packet[IP].dst = attackerIP
				packet[IP].src = ip_reflector
				del packet.chksum
				if packet.haslayer(TCP):
					del packet[TCP].chksum
				elif packet.haslayer(UDP):
					del packet[UDP].chksum
				elif packet.haslayer(ICMP):
					del packet[ICMP].chksum
				packet.show2()
				sendp(packet, iface = inter)
				packet.show2()

			elif packet[IP].dst == ip_reflector:
				attackerIP = packet[IP].src
				attackerEther = packet[Ether].src
				packet[Ether].dst = attackerEther
				packet[Ether].src = ether_victim
				packet[IP].dst = attackerIP
				packet[IP].src = ip_victim
				del packet.chksum
				if packet.haslayer(TCP):
					del packet[TCP].chksum
				elif packet.haslayer(UDP):
					del packet[UDP].chksum
				elif packet.haslayer(ICMP):
					del packet[ICMP].chksum
				packet.show2()
				sendp(packet, iface = inter)
				packet.show2()
	except Exception as e:
		print(traceback.format_exc())
		print (e)			

try:
	opts, args = getopt.getopt(sys.argv[1:], "",['interface=', 'victim-ip=', 'victim-ethernet=', 'reflector-ip=', 'reflector-ethernet='])
except getopt.GetoptError as e:
	print (e)
for param1,param2 in opts :
	if param1 in ("--interface"):
		interface = param2
	if param1 in ("--victim-ip"):
		ip_victim = param2
	if param1 in ("--victim-ethernet"):
		ether_victim = param2
	if param1 in ("--reflector-ip"):
		ip_reflector = param2
	if param1 in ("--reflector-ethernet"):
		ether_reflector = param2
inter = interface
sniff(iface = inter, prn = reflectorcallback)

