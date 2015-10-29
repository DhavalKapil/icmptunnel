#!/bin/sh

# Assigining an IP address and mask to 'tun0' interface
ifconfig tun0 mtu 1500 up 10.0.1.2 netmask 255.255.255.0

# Modifying IP routing tables
route del default
route add -host <server> gw <gateway> dev <device>
route add default gw 10.0.1.1 tun0
