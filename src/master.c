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

#include "usb.h"
#include "usb.c"

#include "crc8.c"

#include "keycodes.h"
#include "keybase.h"

// for both halfs!
u8 matrix[KEYMAP_LAYER_SIZE] = { 0 };
u8 last_matrix[KEYMAP_LAYER_SIZE] = { 0 };

#define NUM_LAYERS	2
#define KEYBOARD_DATA_LAYERS \
	[0]{																															\
	/* Base Layer: */																											\
		/* Master */																											\
		K(KC_Q), K(KC_W), K(KC_E), K(KC_R), K(KC_T),																			\
		K(KC_A), K(KC_S), K(KC_D), K(KC_F), K(KC_G),																			\
		K(KC_Z), K(KC_X), K(KC_C), K(KC_V), K(KC_B),																			\
		K(KC_Y), 0x00, 0x00, K(KC_O), DK(KEY_DEV_LAYER_INC),																	\
																																\
		/* Slave */																												\
		K(KC_Q), K(KC_W), K(KC_E), KM(KC_8, MOD_RIGHT, MOD_ALT), KM(KC_9, MOD_RIGHT, MOD_ALT),									\
		K(KC_A), K(KC_S), K(KC_D), K(KC_F), K(KC_G),																			\
		K(KC_Z), K(KC_X), K(KC_C), K(KC_V), K(KC_B),																			\
		DK(KEY_DEV_LAYER_DEC), K(KC_U), 0x00, 0x00, K(KC_P)																		\
	},																															\
	[1]{																															\
	/* Alt Layer: */																											\
		/* Master */																											\
		M(KC_MEDIA_BRIGHTNESS_DOWN), M(KC_MEDIA_BRIGHTNESS_UP), M(KC_MEDIA_MUTE), M(KC_MEDIA_VOL_DOWN), M(KC_MEDIA_VOL_UP),		\
		M(KC_MEDIA_BRIGHTNESS_MIN), K(KC_MEDIA_BRIGHTNESS_MAX), 0x00, M(KC_MEDIA_PLAY_PAUSE), 0x00,								\
		M(0xFF), K(KC_F), K(KC_J), K(KC_K), K(KC_L),																			\
		K(KC_Y), 0x00, 0x00, K(KC_O), DK(KEY_DEV_LAYER_INC),																	\
																																\
		/* Slave */																												\
		K(KC_Q), K(KC_W), K(KC_E), KM(KC_8, MOD_RIGHT, MOD_ALT), KM(KC_9, MOD_RIGHT, MOD_ALT),									\
		K(KC_A), K(KC_S), K(KC_D), K(KC_F), K(KC_G),																			\
		K(KC_Z), K(KC_X), K(KC_C), K(KC_V), K(KC_B),																			\
		DK(KEY_DEV_LAYER_DEC), K(KC_U), 0x00, 0x00, K(KC_P)																		\
	}

keyval_t matrix_vals[NUM_LAYERS][KEYMAP_LAYER_SIZE] = {
	KEYBOARD_DATA_LAYERS
};

// todo: rebuild into keyval_t data
typedef struct {
	u8 layer_index;
	u8 key_index;
	u16 repeat_ms;
	u32 last_sent_ms;
} hold_repeat_key_t;

// maybe add a minimum-hold time req. before we start to do the thing!
hold_repeat_key_t hold_repeat_keys[] = {
	{ 1, 3, 250, 0 },
	{ 1, 4, 250, 0 }
};
#define NUM_HOLD_REPEAT_KEYS (sizeof(hold_repeat_keys) / sizeof(hold_repeat_key_t))

hold_repeat_key_t * get_hold_repeat_key(u8 layer, u8 key) {
	for(u8 i = 0; i < NUM_HOLD_REPEAT_KEYS; i++) {
		if(hold_repeat_keys[i].layer_index == layer && hold_repeat_keys[i].key_index == key) {
			return &hold_repeat_keys[i];
		}
	}
	return NULL;
}

typedef struct keyboard_s {
	u8 num_layers;
	keyval_t (*map)[KEYMAP_LAYER_SIZE];

	// state
	u8 layer_index;
} keyboard_t;

keyboard_t keyboard = {
	.num_layers = NUM_LAYERS, // sizeof(matrix_vals)/sizeof(matrix_vals[0]),
	.map = matrix_vals,
	.layer_index = 0
};

// TODO: not active in current version 0.9
typedef struct __attribute__((packed)) {
	u8 magic;
	keyval_t keys[NUM_LAYERS][KEYMAP_LAYER_SIZE];
} keymap_config_t;

keymap_config_t ee_keymap EEMEM = {
	.magic = EEPROM_MAGIC,
	.keys = { KEYBOARD_DATA_LAYERS }
};

#if 0
keymap_config_t rt_keymap = {
	.magic = EEPROM_MAGIC,
	.keys = { KEYBOARD_DATA_LAYERS }
};

void load_keymap(void) {
	eeprom_read_block(&rt_keymap, &ee_keymap, sizeof(keymap_config_t));
	// eeprom_read_block(&rt_keymap, (const void*)EEPROM_ADDR, sizeof(keymap_config_t));

	#if 1
	if(rt_keymap.magic != EEPROM_MAGIC) {
		// TODO: handle both halfs
		memcpy(rt_keymap.keys, matrix_vals, NUM_LAYERS * KEYMAP_LAYER_SIZE);
		rt_keymap.magic = EEPROM_MAGIC;
		eeprom_write_block(&rt_keymap, &ee_keymap, sizeof(keymap_config_t));
		// eeprom_write_block(&rt_keymap, (void*)EEPROM_ADDR, sizeof(keymap_config_t));
	}
	#endif
	
	memcpy(matrix_vals, rt_keymap.keys, NUM_LAYERS * KEYMAP_LAYER_SIZE);
}

#if 0
// TODO: make eeprom writing be per-iteration (partially deffered)
// so write one byte per iteration instead of blocking by writing all at once?
void handle_hid_feature(void) {
	// return;

	if(!vendor_out_ready) { 
		return; 
	} else {
		vendor_out_ready = 0;
	}

	if(vendor_out_buf[0] != EEPROM_MAGIC) {
		return;
	}

	for(u8 i = 0; i < KEYMAP_LAYER_SIZE; i++) {
		rt_keymap.keys[i] = vendor_out_buf[i + 1];
	}

	memcpy(matrix_vals, rt_keymap.keys, KEYMAP_LAYER_SIZE);
	eeprom_write_block(&rt_keymap, &ee_keymap, sizeof(keymap_config_t));
}
#endif
#endif

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

	// UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1); // | (1 << TXCIE1);
	UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1);

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

#define UART_RX_QUEUE_SIZE		(UART_QUEUE_SIZE)
volatile u8 uart_rxq[UART_RX_QUEUE_SIZE] = { 0 };
volatile u8 uart_rx_head = 0;
volatile u8 uart_rx_tail = 0;

static inline int uart_rx_pop(void) {
	if(uart_rx_tail == uart_rx_head) return -1;
	u8 byte = uart_rxq[uart_rx_tail];
	uart_rx_tail = (uart_rx_tail + 1) % UART_RX_QUEUE_SIZE;
	return byte;
}

volatile u8 slave_matrix[NUM_KEYS_PER_HALF] = { 0 };
volatile u8 slave_packet_ready = 0;

// TODO: move into timing.c/h
volatile u32 g_ms = 0;
u32 millis(void) {
	u32 ms;
	cli();
	ms = g_ms;
	sei();
	return ms;
}

void unpack_slave_matrix(u8 * slave_packet, u8 * out) {
	for(u8 i = 0; i < NUM_KEYS_PER_HALF; i++) {
		out[i] = 0;
	}

	for(u8 i = 0; i < NUM_KEYS_PER_HALF; i++) {
		u8 byte = slave_packet[2 + (i >> 3)];
		u8 bit = i & 7;
		out[i] = (byte >> bit) & 1;
	}
}

u8 handled_uart_packet_timer = 0;
u8 handled_uart_packet = 0;
void handle_slave_packet(u8 * packet) {
	// read slave matrix state (3x bits into 4x5 bytes after each master row)
	if(packet[0] == UART_START && packet[1] == UART_MSG_KEYMAP) {
		if(packet[5] != calc_crc8(&packet[2], 3)) {
			return;
		}
		unpack_slave_matrix(packet, (u8*)slave_matrix);
		slave_packet_ready = 1;
	}
}

void process_uart_packets(void) {
	static u8 rx_pos = 0;
	static u8 packet[UART_PACKET_SIZE] = { 0 };
	static u32 last_byte_time = 0;
	// if(slave_packet_ready) return;

	u32 now = millis();
	
	// if we are in mid-packet and havnt rxd byte in 50ms - rest
	if(rx_pos > 0 && (now - last_byte_time) > 50) {
		// timeout or resync
		rx_pos = 0;
	}
	
	int byte;
	while((byte = uart_rx_pop()) >= 0) {
		last_byte_time = millis();

		// restart always if we see START byte for framing fix
		if(byte == UART_START && rx_pos != 0 && rx_pos != 1) {
			rx_pos = 0;
		}

		switch(rx_pos) {
			case 0: {
				if(byte == UART_START) {
					packet[0] = byte;
					rx_pos = 1;
				}
			} break;
			case 1: {
				if(byte == UART_MSG_KEYMAP) {
					packet[1] = byte;
					rx_pos = 2;
				} else {
					// restart / desync
					rx_pos = 0;
				}
			} break;
			default: {
				packet[rx_pos++] = byte;
				if(rx_pos >= UART_PACKET_SIZE) {
					handle_slave_packet(packet);
					rx_pos = 0;
				}
			} break;
		}
	}
}

ISR(USART1_RX_vect) {
	u8 status = UCSR1A;
	// Must read UDR1 to clear error flags
	u8 byte = UDR1;
	
	// Check for errors
	if(status & ((1 << FE1) | (1 << DOR1) | (1 << UPE1))) {
		// Discard corrupted byte, but error flags are now cleared
		SET_PIN_HIGH(EXT_LED2_PIN);
		return;
	}
	
	u8 next_head = (uart_rx_head + 1) % UART_RX_QUEUE_SIZE;
	if(next_head != uart_rx_tail) {
		// not full
		uart_rxq[uart_rx_head] = byte;
		uart_rx_head = next_head;
	} else {
		// drop byte
		SET_PIN_HIGH(EXT_LED2_PIN);
	}
}

// TODO: remake to handle master and slave halves!
u8 p2i(u8 col, u8 row) {
	// return (row * (COLS + COLS)) + col;
	return (row * COLS) + col;
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

void scan_master_matrix(void) {
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

void merge_matrix(void) {
	#if 0
	// merge slave and master matrix data
	for(u8 row = 0; row < ROWS; row++) {
		for(u8 col = 0; col < COLS; col++) {
			u8 sval = slave_matrix[(row * COLS) + col];
			matrix[(row * (COLS * 2)) + COLS + col - 1] = sval;
		}
	}
	#else
	// linear merge
	for (u8 i = 0; i < NUM_KEYS_PER_HALF; i++) {
        matrix[NUM_KEYS_PER_HALF + i] = slave_matrix[i];
    }
	#endif
}

void send_matrix(void) {
	SET_PIN_LOW(EXT_LED2_PIN);
	u32 now = millis();

	// if we want transparent layer keys, this needs to be handled in the loop
	// get_next_layer_index(index, dir);
	keyval_t * layer_map = keyboard.map[keyboard.layer_index];

	keyval_t kv;
	for(u16 i = 0; i < KEYMAP_LAYER_SIZE; i++) {
		// send keypress events
		kv = layer_map[i];
		if(kv != 0) {
			u8 type = KEY_GET_TYPE(kv);
			u8 keycode = KEY_GET_VAL(kv);
			u8 modifiers = KEY_GET_MOD(kv);
			if(KEY_GET_MODLR(kv)) { 
				modifiers <<= 4;
			}

			if((matrix[i] == 1) && (last_matrix[i] == 0)) {
				if(type == KEY_TYPE_KEY) {
					send_keypress(keycode, modifiers);
				} else if (type == KEY_TYPE_MEDIA) {
					// TODO: add all media keys correctly in the keymap bits
					// M(0xFF) -> KC_MEDIA_AL_CALC
					if(keycode == 0xFF) {
						send_consumer_key(0x192);
					}
					else
						send_consumer_key(keycode);
				} else if (type == KEY_TYPE_DEV) {
					// TODO: use minmax or clamp?
					switch(keycode) {
						case KEY_DEV_LAYER_INC: {
							if((keyboard.layer_index + 1) < keyboard.num_layers) {
								keyboard.layer_index++;
							}
						} break;
						case KEY_DEV_LAYER_DEC: {
							if(keyboard.layer_index > 0) {
								keyboard.layer_index--;
							}
						} break;
					}
				}
			} else if((matrix[i] == 1) && (last_matrix[i] == 1)) {
				// dont like this call structure
				hold_repeat_key_t * hrkp = get_hold_repeat_key(keyboard.layer_index, i);

				// this double sends once because we dont set the last_sent_ms in prev.
				if(hrkp) {
					if((now - hrkp->last_sent_ms) >= hrkp->repeat_ms) {
						hrkp->last_sent_ms = now;

						if(type == KEY_TYPE_KEY) {
							send_keypress(keycode, modifiers);
						} else if (type == KEY_TYPE_MEDIA) {
							send_consumer_key(keycode);
						}
					}
				}
			}
		}
	}

	for(u16 i = 0; i < KEYMAP_LAYER_SIZE; i++) {
		last_matrix[i] = matrix[i];
	}
}

volatile u32 ms_counter = 0;
volatile u32 last_led_toggle = 0;
volatile u16 led_period = 500; // ms

ISR(TIMER0_COMPA_vect) {
	g_ms++;
	ms_counter++;
	if((ms_counter - last_led_toggle) >= led_period) {
		last_led_toggle = ms_counter;
		TOGGLE_PIN(LED_PIN);
	}
}

volatile u32 last_scan_ms = 0;
int main(void) {
	ENABLE_OUTPUT(EXT_LED2_PIN);
	SET_PIN_HIGH(EXT_LED2_PIN);

	usb_init();
	uart_init();
	
	// DDRC |= (1 << LED_PIN);
	ENABLE_OUTPUT(LED_PIN);
	SET_PIN_LOW(LED_PIN);
	
	for(u8 i = 0; i < NUM_KEYS_PER_HALF; i++) { slave_matrix[i] = 0; }
	for(u8 i = 0; i < KEYMAP_LAYER_SIZE; i++) { matrix[i] = last_matrix[i] = 0; }
	
	// TODO: init after USB is confirmed?
	init_matrix();
	// load_keymap();

	sei(); // enable global interrupts

	// TODO: use time counter to toggle led while waiting for usb
	while(!get_usb_config_status()) {
	// while(usb_kbd_configured() == 0) {
		TOGGLE_PIN(LED_PIN);
		TOGGLE_PIN(EXT_LED2_PIN);
		_delay_ms(50);
	}
	SET_PIN_LOW(LED_PIN);
	SET_PIN_LOW(EXT_LED2_PIN);

	// setup timer0 for 1ms ticks (for onboard LED)
	// NOTE: works with the TIMER0_COMPA_vect ISR
	TCCR0A = (1 << WGM01);				// CTC mode
	OCR0A = 249;						// 16MHz / (64 * (249 + 1)) = 1000Hz == 1ms
	TCCR0B = (1 << CS01) | (1 << CS00); // prescaler 64
	TIMSK0 = (1 << OCIE0A);				// enable compare match interrupt
	ms_counter = 0;
	// given that we use the timer0, should we wait until a certain "next max time?" or wait until comm. works and master tells slave to start LED?

	while (1) {
		// response from slave
		process_uart_packets();
		
		u32 now = millis();
		// 100hz scan rate
		if((now - last_scan_ms) >= 10) {
			last_scan_ms = now;

			// TODO: matrix scanning rate control
			if(slave_packet_ready) {
				merge_matrix();
				slave_packet_ready = 0;
			}
		}

		scan_master_matrix();
		send_matrix();
		
		// TODO: check / update this on IRQ timer?
		// handle_hid_feature();
		
		// Should this be here? maybe better if its only if usb state is idle?
		#if defined(SLEEP_TIME_MS) 
		#if SLEEP_TIME_MS > 0
		_delay_ms(1);
		#endif
		#endif
	}
}
