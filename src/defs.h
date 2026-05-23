#ifndef DEFS_H
#define DEFS_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define UART_BAUD	115200
// #define UART_BAUD	9600
// #define UART_BAUD	38400

#define SCAN_DELAY	5
#define SHORT_DELAY	100
#define LONG_DELAY	500

// per keyboard half
#define ROWS				4
#define COLS				5

#define NUM_HALFS			2
#define NUM_KEYS_PER_HALF	(ROWS * COLS)
#define KEYMAP_LAYER_SIZE	(NUM_KEYS_PER_HALF * NUM_HALFS)

#define BITS_TO_BYTES(n)	(((n) + 7u) >> 3)
#define BITMAP_BYTES(bits)	((bits + 7) / 8)
#define KEYMAP_HALF_BYTES	BITMAP_BYTES(NUM_KEYS_PER_HALF)

// maybe not here?
#define EEPROM_MAGIC	0x42
#define EEPROM_ADDR		0x00

// TODO: maybe make uart packet COBS or use other format?
// UART_PACKET = [ START, MSG, LEN*, DATA[], CRC ] *LEN not in currently
// maybe also a SEQ number?
#define UART_QUEUE_SIZE		256
#define UART_START			0xAA
#define UART_MSG_KEYMAP		0xBB
#define UART_CRC_SIZE		1
#define UART_PACKET_SIZE	(1 + 1 + KEYMAP_HALF_BYTES + 1)

#define SLEEP_TIME_MS	10

// Pin 13 on Atmega32U4 ItsyBitsy 5V
#define LED_PIN		C, 7
#define EXT_LED_PIN	C, 6
#define EXT_LED2_PIN	F, 0

#define BTN_PIN		B, 5

// UART specifically - data lines are crossed in hardware!
#define TX_PIN		D, 3
#define RX_PIN		D, 2

// maybe reorder for master / slave?
#define COL_0_PIN	E, 6
#define COL_1_PIN	B, 5
#define COL_2_PIN	B, 6
#define COL_3_PIN	B, 7
#define COL_4_PIN	D, 6

#define ROW_0_PIN	F, 7
#define ROW_1_PIN	F, 6
#define ROW_2_PIN	F, 5
#define ROW_3_PIN	F, 4

#endif /* DEFS_H */