/**
 *  icmp.h
 */

#ifndef icmp_gaurd
#define icmp_gaurd

/**
 * Function to calculate checksum
 */
uint16_t in_cksum(uint16_t *addr, int len);

#endif