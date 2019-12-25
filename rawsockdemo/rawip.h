/*
 * rawip.h
 *
 *  Created on: Dec 15th, 2019
 *      Author: phamsodiep
 */
#ifndef RAWIP_H_
#define RAWIP_H_


#include <netinet/in.h>
#include "rawsock.h"


//@TODO: these macroes need carefully test
#define IP_OPTION_DEC(v)   ((char*) ((v) + 1))
#define IP_PAYLOAD_DEC(v)  (((v)->ver_ihl & 0x0F) > 5 ? IP_OPTION_DEC(v) + 16 : IP_OPTION_DEC(v))


typedef struct ip_header_tag {
  uint8_t  ver_ihl;
  uint8_t  tso;
  uint16_t totallength;

  uint16_t id;
  uint16_t flags_foffset;

  uint8_t  ttl;
  uint8_t  proto;
  uint16_t checksum;

  uint8_t  src_addr[4];
  uint8_t  dsc_addr[4];
} __attribute__((packed)) ip_header_t;


typedef struct udp_header_tag {
  uint16_t src_port;
  uint16_t dsc_port;
  uint16_t length;
  uint16_t checksumUdp;
} __attribute__((packed)) udp_header_t;


u_int32_t wrapsum(u_int32_t sum);
u_int32_t checksum(unsigned char *buf, unsigned nbytes, u_int32_t sum);
void computeChecksumForUDPPacket(ip_header_t* pL3, int pL4Size);
void buildDefaultUDPPacket(ip_header_t* pL3, int pL3Size, uint16_t srcPort, uint16_t dscPort);

#endif /* RAWIP_H_ */

