/*
 * main.c
 *
 *  Created on: Dec 15th, 2019
 *      Author: phamsodiep
 */
#include <stdio.h>
#include <string.h>

#include "rawsock.h"
#include "rawip.h"


///////////////////////////////
// PLATFORM HARDCODED MACROES
///////////////////////////////
#define NETDEVNAME  "enp0s3"
#define BUFFER_SIZE (1500)

// Source address
#define IPS0 169
#define IPS1 254
#define IPS2 69
#define IPS3 102

// Destination address
#define DMAC0 0x0A
#define DMAC1 0x00
#define DMAC2 0x27
#define DMAC3 0x00
#define DMAC4 0x00
#define DMAC5 0x08

#define IPD0 169
#define IPD1 254
#define IPD2 69
#define IPD3 101


int serverProcess();
int clientProcess(char* msg);


int main(int argc, char *argv[])
{
  if (argc > 1) {
    return clientProcess(argv[1]);
  }

  return serverProcess();
}


int serverProcess() {
  rsocket_t iSock;
  int nRead = BUFFER_SIZE;
  int nPayload = 0;
  char _buf[BUFFER_SIZE];
  datalink_header_t* pL2 = (datalink_header_t*) _buf;
  ip_header_t* pL3 = (ip_header_t*) (pL2 + 1);
  udp_header_t* pL4 = (udp_header_t*) IP_PAYLOAD_DEC(pL3);
  char* pMsg = (char*)(pL4 + 1);


  if (openInRSocket(NETDEVNAME, &iSock) <= 0) {
    return -1;
  }

  while (1) {
    nRead = datalinkFrameRead(&iSock, pL2, BUFFER_SIZE);
    if (nRead > 0 && ntohs(pL4->dsc_port) == 5005) {
      nPayload = ntohs(pL4->length) - sizeof(udp_header_t);
      pMsg = (char*)(pL4 + 1);
      pMsg[nPayload] = 0;
      printf("%s\n", pMsg);
    }
  }

  return 0;
}


int clientProcess(char* msg) {
  rsocket_t oSock;
  int nRead = BUFFER_SIZE;
  char _buf[BUFFER_SIZE];
  datalink_header_t* pL2 = (datalink_header_t*) _buf;
  ip_header_t* pL3 = (ip_header_t*) (pL2 + 1);
  udp_header_t* pL4 = (udp_header_t*) IP_PAYLOAD_DEC(pL3);
  char* pMsg = (char*)(pL4 + 1);
  int payLoadSize = strlen(msg);
  int pL3Size = payLoadSize + sizeof(udp_header_t);
  int nPayLoad = payLoadSize;

  // 1. Process Payload
  if (nPayLoad <= 0) {
    return -1;
  }
  strncpy(pMsg, msg, nPayLoad);
  nPayLoad = payLoadSize
    + sizeof(datalink_header_t)
    + sizeof(ip_header_t)
    + sizeof(udp_header_t);

  // 2. Fill destination MAC address
  pL2->ether_dhost[0] = DMAC0;
  pL2->ether_dhost[1] = DMAC1;
  pL2->ether_dhost[2] = DMAC2;
  pL2->ether_dhost[3] = DMAC3;
  pL2->ether_dhost[4] = DMAC4;
  pL2->ether_dhost[5] = DMAC5;
  
  pL3->dsc_addr[0] = IPD0;
  pL3->dsc_addr[1] = IPD1;
  pL3->dsc_addr[2] = IPD2;
  pL3->dsc_addr[3] = IPD3;

  // 3. Fill source MAC address
  pL3->src_addr[0] = IPS0;
  pL3->src_addr[1] = IPS1;
  pL3->src_addr[2] = IPS2;
  pL3->src_addr[3] = IPS3;

  // Protocal process
  buildDefaultUDPPacket(pL3, pL3Size, 0x8CC9, 5005);
  // UDP and IP checksume compute
  computeChecksumForUDPPacket(pL3, payLoadSize);

  if (openOutRSocket(NETDEVNAME, &oSock) <= 0) {
    return -1;
  }

  if (datalinkFrameWrite(&oSock, pL2, nPayLoad) > 0) {
    return 0;
  }

  return -1;
}

