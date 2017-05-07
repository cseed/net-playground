Playground for networking-related projects.

lldump
======

lldump is a simple utility that dumps link layer packets using the
Linux packet socket interface.

lltx
====

lltx transmits a single Ethernet packet.  It sets the protocol to
0x0101 (experimental).  For example:

```
$ sudo ./lltx eth0 12:34:56:78:9a:bc deadcafe
```

sends a packet on eth0 to the Ethernet address 12:34:56:78:9a:bc with
payload deadcafe.  It is captured by lldump as:

```
packet 18
  src_addr
    family 17 packet
    protocol 0101
    ifindex 2
    hatype 1 ether
    pkttype 4 outgoing
    halen 6
    addr xxxxxxxxxxxx
  eth header
    dest addr 123456789abc
    src addr xxxxxxxxxxxx
    size 4
    payload deadcafe
```

echo
====

echo is a primitive echo server and client.
