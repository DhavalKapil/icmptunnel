# icmptunnel [![Build Status](https://travis-ci.org/DhavalKapil/icmptunnel.svg?branch=master)](https://travis-ci.org/DhavalKapil/icmptunnel)

> Transparently tunnel your IP traffic through ICMP echo and reply packets.

'icmptunnel' works by encapsulating your IP traffic in ICMP echo packets and sending them to your own proxy server. The proxy server decapsulates the packet and forwards the IP traffic. The incoming IP packets which are destined for the client are again encapsulated in ICMP reply packets and sent back to the client. The IP traffic is sent in the 'data' field of ICMP packets.

[RFC 792](http://www.ietf.org/rfc/rfc792.txt), which is IETF's rules governing ICMP packets, allows for an arbitrary data length for any type 0 (echo reply) or 8 (echo message) ICMP packets.

So basically the client machine uses only the ICMP protocol to communicate with the proxy server. Applications running on the client machine are oblivious to this fact and work seamlessly.

## Use Cases

1. **Bypassing Captive Portals**: Many public Wi-Fi use [Captive Portals](https://en.wikipedia.org/wiki/Captive_portal) to authenticate users, i.e. after connecting to the Wi-Fi the user is redirected to a webpage that requires a login. icmptunnel can be used to bypass such authentications in transport/application layers.

2. **Bypassing firewalls**: Firewalls are set up in various networks to block certain type of traffic. icmptunnel can be used to bypass such firewall rules. Obfuscating the data payload can also be helpful to bypass some firewalls.

3. **Encrypted Communication Channel**: Adding sufficient encryption to the data, icmptunnel can be used to establish an encrypted communication channel between two host machines. 

## Requirements

1. A POSIX-compliant host with root access that will be communicating with only ICMP protocol. This will be the client.

2. A POSIX-compliant host with root access with full access to the internet. This will act as our proxy server.

3. The proxy server should be accessible from the client host.

_Note: Although icmptunnel has been successfully tested on Ubuntu 14.04 LTS, it should work on others as well._

## Step-by-step instructions

1. Install `make` on both machines.

2. Clone this repository using this command:

  ```
  git clone https://github.com/DhavalKapil/icmptunnel
  ```

3. Run `make`:

  ```
  make
  ```

4. On the server side run the tunnel with root privileges:

  ```
  [sudo] ./icmptunnel -s 10.0.1.1
  ```

5. On the client side, find out your gateway and the corresponding interface:

  ```
  route -n

  Destination     Gateway         Genmask         Flags Metric Ref    Use Iface

  0.0.0.0         172.25.30.1     0.0.0.0         UG    0      0        0 eth0
  ```

  Edit client.sh and replace \<server\> with the IP address of the proxy server. \<gateway\> with gateway address obtained above and similarly for \<interface\>.

6. Check the DNS server at client side. Make sure it does not use any server not accessible by our proxy server. One suggestion is to use `8.8.8.8`(Google's DNS server) which will be accessible to the proxy server. You would need to edit your DNS settings for this. *You might need to manually delete the route for your local DNS server from your routing table.*

7. Run the tunnel on your client with root privileges:

  ```
  [sudo] ./icmptunnel -c <server>
  ```

The tunnel should run and your client machine should be able to access the internet. All traffic will be tunneled through ICMP.

## Architecture

icmptunnel works by creating a virtual tunnel interface(say `tun0`). All the user traffic on the client host is routed to `tun0`. icmptunnel listens on this interface for IP packets. These packets are encapsulated in an ICMP echo packet(i.e. the payload of the ICMP packet is nothing but the original IP packet). This newly generated ICMP packet is sent outside the client machine, to the proxy server, through the restricted internet connection.

The proxy server receives these ICMP packets and decapsulates the original IP packet. This is retransmitted onto the Internet after implementing IP masquerading. Hence, the target believes that its the proxy server making the request. The target then responds back to the proxy server with an IP packet. This is again captured by icmptunnel, encapsulated in an ICMP reply packet and send back to the client. 

On the client side, the IP packet is retrieved from the payload of the ICMP reply packet and injected in `tun0`. The user applications read from this virtual interface and hence get the proper IP packet.

#### Overall Architecture

```
+--------------+                         +------------+
|              |       ICMP traffic      |            |       IP traffic
|    Client    |  ------------------->   |   Proxy    |   ------------------>
|              |  <-------------------   |   Server   |   <------------------
|              |    through restricted   |            |     proper internet
+--------------+         internet        +------------+
```

#### Client Architecture

```
+--------------+                                    +------------+
|              |  IP traffic  +------+  IP traffic  |            |   ICMP traffic
|     User     |  --------->  | tun0 |  --------->  | icmptunnel | --------------->
| Applications |  <---------  +------+  <---------  |  program   | <---------------
|              |        (Virtual Interface)         |            |    restricted 
+--------------+                                    +------------+     internet
```

#### Proxy Server Architecture

```
                 +------------+
  ICMP traffic   |            |  IP traffic     +------+       NAT/Masquerading
---------------> | icmptunnel | ------------>   | tun0 |    ---------------------> 
<--------------- |  program   | <------------   +------+    <---------------------
   restricted    |            |           (Virtual Interface)   proper internet
    internet     +------------+
```

## Implementation

* ICMP is implemented using raw C sockets.

* The checksum is calculated using the algorithm given in [RFC 1071](https://tools.ietf.org/html/rfc1071).

* [Tun](https://www.kernel.org/doc/Documentation/networking/tuntap.txt) driver is used for creating a virtual interface and binding to user space programs.

* The virtual interface is configured through `ifconfig`.

* `route` is used to change the routing tables of the client so as to route all traffic to the virtual tunnel interface.

* `dd` is used to temporarily change the setting of IP forwarding and replying back to ICMP requests on the side of the proxy server.

* `iptables` is used to set up `nat` on the server side.

## Demo

### Network Setup

Proxy server is connected to `eth0`. This interface provides full internet connection.

Both the client and proxy server are connected to `wlan0`(a WiFi hotspot). This hotspot is configured not to provide any internet connection.

`tun0` will be created in both the client and the proxy server.

The client will make an HTTP request to [dhavalkapil.com](https://dhavalkapil.com).

[Wireshark](https://www.wireshark.org/) is used to capture network traffic at both ends on various interface.

### Screenshots of network traffic

1. `tun0` on client side

  ![tun0 client side](https://i.imgur.com/EnStcDO.png?1)

  The usual HTTP request is visible along with response.

2. `wlan0` on client side

  ![wlan0 client side](https://i.imgur.com/EKEqCGv.png?1)

  All traffic is ICMP. The HTTP/IP packet can be seen as part of the payload of the ICMP packet.

3. `wlan0` on proxy server side

  ![wlan0 proxy server side](https://i.imgur.com/6OhsUyZ.png?1)

  The ICMP packets sent by the client can be seen.

4. `tun0` on proxy server side

  ![tun0 proxy server side](https://i.imgur.com/OCq9aZe.png?1)

  The HTTP/IP packets are decapsulated and sent through `tun0`.

5. `eth0` on proxy server side

  ![eth0 proxy server side](https://i.imgur.com/HQigUea.png?1)
  
  The HTTP/IP packets are forwarded to the internet. Notice how the source IP has been masqueraded because of `nat`.

## Contribution

Feel free to [file issues](https://github.com/DhavalKapil/icmptunnel/issues) and submit [pull requests](https://github.com/DhavalKapil/icmptunnel/pulls) – contributions are welcome.

## License

icmptunnel is licensed under the [MIT license](http://dhaval.mit-license.org/).
