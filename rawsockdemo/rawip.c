/*
 * rawip.c
 *
 *  Created on: Dec 25th, 2019
 *      Author: phamsodiep
 */
#include "rawip.h"


u_int32_t checksum(unsigned char *buf, unsigned nbytes, u_int32_t sum) {
  uint i;

  // Checksum all the pairs of bytes first...
  for (i = 0; i < (nbytes & ~1U); i += 2) {
    sum += (u_int16_t)ntohs(*((u_int16_t *)(buf + i)));
    if (sum > 0xFFFF)
      sum -= 0xFFFF;
  }

  // If there's a single byte left over, checksum it, too.
  // Network byte order is big-endian, so the remaining byte is
  // the high byte.
  if (i < nbytes) {
    sum += buf[i] << 8;
    if (sum > 0xFFFF)
      sum -= 0xFFFF;
  }

  return (sum);
}


u_int32_t wrapsum(u_int32_t sum) {
  sum = ~sum & 0xFFFF;
  return (htons(sum));
}


void computeChecksumForUDPPacket(ip_header_t* pL3, int pL4Size) {
  udp_header_t* pL4 = (udp_header_t*) IP_PAYLOAD_DEC(pL3);
  uint32_t sum;
  
  // 0. Clear checksum field because calculating will take this field into account
  pL4->checksumUdp = 0;
  // 1. Calculate checksum for Layer 2, datalink
  sum = (uint32_t) pL3->proto + (uint32_t)ntohs(pL4->length);
  // 2. Calculate checksum for Layer 3, ip
  sum = checksum(((unsigned char *) &pL3->src_addr), 8, sum);
  // 3. Calculate checksum for payload UDP (message)
  sum = checksum((unsigned char *)(pL4 + 1), pL4Size, sum);
  // 4. Calculate checksum for UDP header
  sum = checksum((((unsigned char *) &pL4->src_port)), 8, sum);
  pL4->checksumUdp = wrapsum(sum);

  // Calculate checksum of ip
  pL3->checksum = 0;
  pL3->checksum = wrapsum(
    checksum(
      ((unsigned char *) &pL3->ver_ihl),
      sizeof(ip_header_t),
      0
    )
  );
}


void buildDefaultUDPPacket(ip_header_t* pL3, int pL3Size, uint16_t srcPort, uint16_t dscPort) {
  udp_header_t* pL4 = (udp_header_t*) IP_PAYLOAD_DEC(pL3);
  int pL2Size = pL3Size + sizeof(ip_header_t);

  pL3->ver_ihl = 0x45; // no option and version 4
  pL3->tso = 0x00;     // Default for ???
  pL3->totallength = htons(pL2Size);
  pL3->id = 0x37b2;    // ? random
  pL3->flags_foffset = 0x0040; // ?
  pL3->ttl = 0x40;
  pL3->proto = 0x11;   // UDP

  pL4->length = htons(pL3Size);
  pL4->src_port = htons(srcPort);
  pL4->dsc_port = htons(dscPort);
}

