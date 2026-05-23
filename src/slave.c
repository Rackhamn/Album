#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <string.h> // memcpy

#include "types.h" // todo: replace NintM_t with NM
#include "macros.h"
#include "defs.h"

#include "crc8.c"

volatile u32 ms_counter = 0;

void uart_init(void) {
	ENABLE_INPUT(RX_PIN);
	ENABLE_OUTPUT(TX_PIN);

	#if 1
	// Asynchronous normal mode (U2Xn = 0)
	// unsigned short baud_rate = ((F_CPU / (16UL * UART_BAUD)) - 1);

	UCSR1A = (1 << U2X1); // enable double speed
	unsigned short baud_rate = ((F_CPU / (8UL * UART_BAUD)) - 1);
	
	// baud rate 9600 @ 16MHz
	UBRR1H = (unsigned char)(baud_rate >> 8);
	UBRR1L = (unsigned char)(baud_rate & 0xFF);

	UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1); // | (1 << TXCIE1);

	// 8N1 frame format
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
	#else
	// instead of using the calculated baudrate, use the one that gives the lowest error (0.2% vs 2.1%)
	// UCSR1A = (1 << U2X1); // double speed mode
	// UBBR1 = 16; // 115200 baud @ 16MHz in double speed mode
	// math: F_CPU / (8UL * UART_BAUD) - 1

	UCSR1A = (1 << U2X1); 
	UBRR1 = 16;

	// UDRIEn
	// enable txrx irq
	UCSR1B = (1 << RXEN1) | 
			 (1 << TXEN1) | 
			// (1 << TXCIE1) | 
			 (1 << RXCIE1);

	// set 8-bit data, 0 parity, 1 stop bit
	UCSR1C = (1 << UCSZ10) | (1 << UCSZ11);
	#endif
}

void init_matrix(void) {
	// TODO: make this work from a list of pins (given by user)
	ENABLE_OUTPUT(ROW_0_PIN);
	ENABLE_OUTPUT(ROW_1_PIN);
	ENABLE_OUTPUT(ROW_2_PIN);

		ENABLE_OUTPUT(ROW_3_PIN);

	SET_PIN_HIGH(ROW_0_PIN);
	SET_PIN_HIGH(ROW_1_PIN);
	SET_PIN_HIGH(ROW_2_PIN);

		SET_PIN_HIGH(ROW_3_PIN);

	ENABLE_INPUT(COL_0_PIN);
	ENABLE_INPUT(COL_1_PIN);
	ENABLE_INPUT(COL_2_PIN);

		ENABLE_INPUT(COL_3_PIN);
		ENABLE_INPUT(COL_4_PIN);

	#if 1
	PULLUP_PIN(COL_0_PIN);
	PULLUP_PIN(COL_1_PIN);
	PULLUP_PIN(COL_2_PIN);

		PULLUP_PIN(COL_3_PIN);
		PULLUP_PIN(COL_4_PIN);
	#endif

	// force re-enable pullups
	SET_PIN_HIGH(COL_0_PIN);
	SET_PIN_HIGH(COL_1_PIN);
	SET_PIN_HIGH(COL_2_PIN);

		SET_PIN_HIGH(COL_3_PIN);
		SET_PIN_HIGH(COL_4_PIN);
}

// with 4x5 == 20 keys, this should be 3 bytes
volatile u8 slave_matrix[KEYMAP_HALF_BYTES] = { 0 };

#define READ_PIN_WRITE_SLAVE_BIT(index, CX) \
do { \
	u8 mask = 1 << (index & 7); \
	u8 * byte = &slave_matrix[index >> 3]; \
	u8 state = (READ_PIN(COL_##CX##_PIN) == 0); \
	*byte = (*byte & ~mask) | (state << (index & 7)); \
	index += 1; \
} while(0)


// TODO: remake to handle master and slave halves!
u8 p2i(u8 col, u8 row) {
	// return (row * (COLS)) + COLS - col;
	// return (row * COLS) + col;

	return (row * COLS) + (COLS - 1 - col);
}

volatile u8 matrix[ROWS * COLS] = { 0 };

void scan_slave_matrix(void) {
	for(u8 i = 0; i < ROWS * COLS; i++) {
		matrix[i] = 0;
	}

	// TODO: make this work from a list of pins (given by user)
	// or even generated in a pre-pass
	for(u8 row = 0; row < ROWS; row++) {
		// activate
		switch(row) {
			case 0: SET_PIN_LOW(ROW_0_PIN); break;
			case 1: SET_PIN_LOW(ROW_1_PIN); break;
			case 2: SET_PIN_LOW(ROW_2_PIN); break;
			case 3: SET_PIN_LOW(ROW_3_PIN); break;
		}

		_delay_us(SCAN_DELAY);

		// maybe should be half?
		matrix[p2i(0, row)] = (READ_PIN(COL_0_PIN) == 0);
		matrix[p2i(1, row)] = (READ_PIN(COL_1_PIN) == 0);
		matrix[p2i(2, row)] = (READ_PIN(COL_2_PIN) == 0);
		matrix[p2i(3, row)] = (READ_PIN(COL_3_PIN) == 0);
		matrix[p2i(4, row)] = (READ_PIN(COL_4_PIN) == 0);

		// deactivate
		switch(row) {
			case 0: SET_PIN_HIGH(ROW_0_PIN); break;
			case 1: SET_PIN_HIGH(ROW_1_PIN); break;
			case 2: SET_PIN_HIGH(ROW_2_PIN); break;
			case 3: SET_PIN_HIGH(ROW_3_PIN); break;
		}
	}
}

u8 xstate = 0;
void bitbash_slave_matrix(void) {
	// matrix ROWSxCOLS -> slave_matrix 3 bytes

	for(u8 i = 0; i < KEYMAP_HALF_BYTES; i++) {
		slave_matrix[i] = 0;
	}

	// u8 x = NUM_KEYS_PER_HALF;
	for(u8 i = 0; i < ROWS * COLS; i++) {
		u8 byte_index = i >> 3;
		u8 bit_index = i & 7;

		slave_matrix[byte_index] = (slave_matrix[byte_index] & ~(1 << bit_index)) | ((matrix[i] & 1) << bit_index);
	}
}

// TODO: make type?
#define UART_TX_QUEUE_SIZE		(UART_QUEUE_SIZE)
volatile u8 uart_txq[UART_TX_QUEUE_SIZE] = { 0 };
volatile u8 uart_tx_head = 0;
volatile u8 uart_tx_tail = 0;

static inline void uart_tx_push(u8 byte) {
	#if 0
	u8 next_head = (uart_tx_head + 1) % UART_TX_QUEUE_SIZE;
	if(next_head != uart_tx_tail) {
		// not full
		uart_txq[uart_tx_head] = byte;
		uart_tx_head = next_head;
	} else {
		return;
	}

	// re-enable tx if needed
	UCSR1B |= (1 << UDRIE1);
	#else
	u8 next_head;
	do {
		next_head = (uart_tx_head + 1) % UART_TX_QUEUE_SIZE;
		// waits until queue has space - blocking
	} while(next_head == uart_tx_tail);

	uart_txq[uart_tx_head] = byte;
	uart_tx_head = next_head;

	// enable UDRE isr
	UCSR1B |= (1 << UDRIE1);
	#endif
}

static inline u8 uart_tx_push_timeout(u8 byte, u16 timeout_ms) {
	u32 start = ms_counter;
	u8 next_head;

	do {
		next_head = (uart_tx_head + 1) % UART_TX_QUEUE_SIZE;
		if(next_head != uart_tx_tail) break;

		// check timeout
		if((ms_counter - start) > timeout_ms) {
			return 0;
		}
	} while(1);

	uart_txq[uart_tx_head] = byte;
	uart_tx_head = next_head;
	UCSR1B |= (1 << UDRIE1);

	return 1;
}

ISR(USART1_UDRE_vect) {
	if(uart_tx_head != uart_tx_tail) {
		UDR1 = uart_txq[uart_tx_tail];
		uart_tx_tail = (uart_tx_tail + 1) % UART_TX_QUEUE_SIZE;
		SET_PIN_HIGH(EXT_LED2_PIN);
	} else {
		// No more bytes, disable UDRE interrupt
		UCSR1B &= ~(1 << UDRIE1);
	}
}

void push_slave_matrix_packet(void) {
	u8 buf[UART_PACKET_SIZE] = { 0 };

	buf[0] = UART_START;
	buf[1] = UART_MSG_KEYMAP;

	#if 1
	cli();
	buf[2] = slave_matrix[0];
	buf[3] = slave_matrix[1];
	buf[4] = slave_matrix[2];
	sei();
	#else
	// memcpy(&buf[2], &slave_matrix[0], 3);
	#endif

	buf[5] = calc_crc8(&buf[2], 3);

	#if 0
	// try to remove residual bits!
	for(u8 i = 0; i < KEYMAP_HALF_BYTES; i++) {
		slave_matrix[i] &= 0xFF;
	}
	#endif

	for(u8 i = 0; i < UART_PACKET_SIZE; i++) {
		if(!uart_tx_push_timeout(buf[i], 50)) {
			// timeout occurred!
			// toggle led?
			// clear uart queue / packets?
		}
		// uart_tx_push(buf[i]);
	}
}

volatile u8 update_in_progress = 0;
void update_matrix(void) {
	// prevent re-entry
	cli();
	if(update_in_progress) {
		sei();
		return;
	}
	update_in_progress = 1;
	sei();

	scan_slave_matrix();
	bitbash_slave_matrix();
	push_slave_matrix_packet();

	update_in_progress = 0;
}

volatile u32 last_led_toggle = 0;
volatile u16 led_period = 500; // ms
volatile u32 last_package_ms = 0;

ISR(TIMER0_COMPA_vect) {
	ms_counter++;
	if((ms_counter - last_led_toggle) >= led_period) {
		last_led_toggle = ms_counter;
		TOGGLE_PIN(LED_PIN);
	}
}

int main(void) {
	// DDRC |= (1 << LED_PIN);
	ENABLE_OUTPUT(LED_PIN);
	SET_PIN_LOW(LED_PIN);

	ENABLE_OUTPUT(EXT_LED2_PIN);
	SET_PIN_LOW(EXT_LED2_PIN);

	uart_init();
	init_matrix();
	
	// mmm
	slave_matrix[0] = 0;
	slave_matrix[1] = 0;
	slave_matrix[2] = 0;

	for(u8 i = 0; i < (ROWS * COLS); i++) { matrix[i] = 0; }
	
	// enable global interrupts
	// setup timer0 for 1ms ticks (for onboard LED)
	// NOTE: works with the TIMER0_COMPA_vect ISR
	TCCR0A = (1 << WGM01);				// CTC mode
	OCR0A = 249;						// 16MHz / (64 * (249 + 1)) = 1000Hz == 1ms
	TCCR0B = (1 << CS01) | (1 << CS00); // prescaler 64
	TIMSK0 = (1 << OCIE0A);				// enable compare match interrupt
	ms_counter = 0;
	// given that we use the timer0, should we wait until a certain "next max time?" or wait until comm. works and master tells slave to start LED?

	sei(); 

	while (1) {
		// 10ms = 100Hz
		// TODO: make counter better
		if(ms_counter - last_package_ms >= 20) {
			last_package_ms = ms_counter;
			update_matrix();
		}
		_delay_ms(1);
		SET_PIN_LOW(EXT_LED2_PIN);
	}
}
