#include "types.h"

u8 calc_crc8(u8 * data, u8 len) {
	u8 crc = 0;
	for(u8 i = 0; i < len; i++) {
		crc ^= data[i];
		for(u8 j = 0; j < 8; j++) {
			if(crc & 0x80) {
				// CRC-8-CCITT poly
				crc = (crc << 1) ^ 0x07;
			} else {
				crc <<= 1;
			}
		}
	}
	return crc;
}