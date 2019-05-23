#ifndef __ARP_H__
#define __ARP_H__

#include "base.h"
#include "ether.h"
#include "types.h"

#define ARPHRD_ETHER 1

#define ARPOP_REQUEST 1
#define ARPOP_REPLY 2

struct ether_arp {
    u16 arp_hrd;		/* Format of hardware address.  */
    u16 arp_pro;		/* Format of protocol address.  */
    u8	arp_hln;		/* Length of hardware address.  */  // 以字节为单位，长度为1即8bit
    u8	arp_pln;		/* Length of protocol address.  */  // 4字节，32bit，IPv4地址长度
    u16 arp_op;			/* ARP opcode (command).  */        // 0x01为ARP请求，0x02为ARP应答
	u8	arp_sha[ETH_ALEN];	/* sender hardware address */   // ARP包发送方MAC地址，6字节，48bit
	u32	arp_spa;		/* sender protocol address */		// ARP包发送方IPv4地址
	u8	arp_tha[ETH_ALEN];	/* target hardware address */   // ARP包目的MAC地址
	u32	arp_tpa;		/* target protocol address */		// ARP包目的IPv4地址
} __attribute__ ((packed));

void handle_arp_packet(iface_info_t *info, char *pkt, int len);
void arp_send_request(iface_info_t *iface, u32 dst_ip);
void iface_send_packet_by_arp(iface_info_t *iface, u32 dst_ip, char *pkt, int len);

#endif

/**
 * ARP请求：发送一个ARP包给别的主机（可为广播包），让别人根据自己的IP-MAC地址映射表查询arp_sha(arp sender hardware address)这一MAC地址对应IP地址然后告知自己；
 * ARP应答：将查询结果返回给请求查询的主机；
 * ARP协议流程：1、收到IPv4包，根据IPv4地址查询自己的ARP表，若找到对应的MAC地址，则转发；
 * 			   2、若没有找到对应的MAC地址，则发送ARP请求包（广播包），让别的主机帮忙查找；
 * 			   3、收到别的主机的ARP应答包，找到对应的MAC地址，转发；
 * 			   4、某个包等待1s未收到ARP应答，则重发ARP请求，某个包等待15s未收到ARP应答，则丢弃掉该包；
*/