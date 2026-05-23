#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "avr/io.h"

#include "types.h"
#include "usb.h"

volatile u8 keyboard_pressed_keys[6] = {0, 0, 0, 0, 0, 0};
volatile u8 keyboard_modifier = 0;
volatile u16 consumer_key = 0;  // Current consumer key

static u16 keyboard_idle_value = 125;
static u8 current_idle = 0;
static u8 this_interrupt = 0;

volatile u8 usb_config_status = 0;
volatile u8 keyboard_protocol = 0;
	
// Device Descriptor
static const u8 device_descriptor[] PROGMEM = {
	18,			  // bLength
	1,			  // bDescriptorType: Device
	0x00, 0x02,   // bcdUSB: USB 2.0
	0,			  // bDeviceClass
	0,			  // bDeviceSubClass
	0,			  // bDeviceProtocol
	32,			  // bMaxPacketSize0
	(idVendor & 255), ((idVendor >> 8) & 255),
	(idProduct & 255), ((idProduct >> 8) & 255),
	0x00, 0x01,   // bcdDevice 1.0
	0,			  // iManufacturer
	0,			  // iProduct
	0,			  // iSerialNumber
	1			  // bNumConfigurations
};

// Keyboard HID Report Descriptor (page 0x07)
// Standard 6KRO boot-protocol keyboard
static const u8 keyboard_HID_descriptor[] PROGMEM = {
	0x05, 0x01,	// Usage Page: Generic Desktop
	0x09, 0x06,	// Usage: Keyboard
	0xA1, 0x01,	// Collection: Application

	// Modifier byte
	0x05, 0x07,	// Usage Page: Key Codes
	0x19, 0xE0,	// Usage Minimum: Left Ctrl (224)
	0x29, 0xE7,	// Usage Maximum: Right GUI (231)
	0x15, 0x00, // Logical Minimum: 0
	0x25, 0x01,	// Logical Maximum: 1
	0x75, 0x01,	// Report Size: 1 bit
	0x95, 0x08,	// Report Count: 8
	0x81, 0x02,	// Input: Data, Variable, Absolute

	// Reserved byte
	0x95, 0x01,	// Report Count: 1
	0x75, 0x08,	// Report Size: 8 bits
	0x81, 0x01,	// Input: Constant

	// LED output report (5 LEDs + 3 padding)
	0x95, 0x05, // Report Count: 5
	0x75, 0x01, // Report Size: 1 bit
	0x05, 0x08, // Usage Page: LEDs
	0x19, 0x01, // Usage Minimum: Num Lock
	0x29, 0x05, // Usage Maximum: Kana
	0x91, 0x02, // Output: Data, Variable, Absolute
	0x95, 0x01, // Report Count: 1 (padding)
	0x75, 0x03, // Report Size: 3 bits
	0x91, 0x01, // Output: Constant

	// 6 key array
	0x95, 0x06, // Report Count: 6
	0x75, 0x08, // Report Size: 8 bits
	0x15, 0x00, // Logical Minimum: 0
	0x25, 0xE7, // Logical Maximum: 231
	0x05, 0x07, // Usage Page: Key Codes
	0x19, 0x00, // Usage Minimum: 0
	0x29, 0xE7, // Usage Maximum: 231
	0x81, 0x00, // Input: Data, Array
	0xC0		// End Collection
};

// Consumer HID Report Descriptor (page 0x0C)
// Single 16-bit consumer control code
static const u8 consumer_HID_descriptor[] PROGMEM = {
	0x05, 0x0C,			// Usage Page: Consumer
	0x09, 0x01,			// Usage: Consumer Control
	0xA1, 0x01,			// Collection: Application
	0x15, 0x00,			// Logical Minimum: 0
	0x26, 0xFF, 0x03,   // Logical Maximum: 1023
	0x19, 0x00,			// Usage Minimum: 0
	0x2A, 0xFF, 0x03,   // Usage Maximum: 1023
	0x75, 0x10,			// Report Size: 16 bits
	0x95, 0x01,			// Report Count: 1
	0x81, 0x00,			// Input: Data, Array
	0xC0				// End Collection
};

static const u8 configuration_descriptor[] PROGMEM = {
	// --- Configuration Descriptor ---
	9,					  // bLength
	2,					  // bDescriptorType: Configuration
	(CONFIG_SIZE & 0xFF), // wTotalLength LSB
	(CONFIG_SIZE >> 8),  // wTotalLength MSB
	NUM_INTERFACES,		 // bNumInterfaces
	1,					 // bConfigurationValue
	0,					 // iConfiguration
	0xC0,				 // bmAttributes: self-powered
	50,					 // bMaxPower: 100mA

	// --- Interface 0: Keyboard ---
	9,					 // bLength
	4,					 // bDescriptorType: Interface
	KEYBOARD_INTERFACE,  // bInterfaceNumber
	0,					 // bAlternateSetting
	1,					 // bNumEndpoints
	0x03,				 // bInterfaceClass: HID
	0x01,				 // bInterfaceSubClass: Boot
	0x01,				 // bInterfaceProtocol: Keyboard
	0,					 // iInterface

	// HID Descriptor for Keyboard
	9,					 // bLength
	0x21,				 // bDescriptorType: HID
	0x11, 0x01,			 // bcdHID: 1.11
	0,					 // bCountryCode
	1,					 // bNumDescriptors
	0x22,				 // bDescriptorType: Report
	KEYBOARD_HID_DESC_SIZE, // wDescriptorLength LSB
	0,					 // wDescriptorLength MSB

	// Endpoint 3 IN (keyboard)
	7,					 // bLength
	5,					 // bDescriptorType: Endpoint
	KEYBOARD_ENDPOINT_NUM | 0x80, // bEndpointAddress: IN
	0x03,				 // bmAttributes: Interrupt
	KEYBOARD_SIZE, 0,	 // wMaxPacketSize
	1,					 // bInterval: 1ms

	// --- Interface 1: Consumer Control ---
	9,					 // bLength
	4,					 // bDescriptorType: Interface
	CONSUMER_INTERFACE,  // bInterfaceNumber
	0,					 // bAlternateSetting
	1,					 // bNumEndpoints
	0x03,				 // bInterfaceClass: HID
	0x00,				 // bInterfaceSubClass: None
	0x00,				 // bInterfaceProtocol: None
	0,					 // iInterface

	// HID Descriptor for Consumer
	9,					 // bLength
	0x21,				 // bDescriptorType: HID
	0x11, 0x01,			 // bcdHID: 1.11
	0,					 // bCountryCode
	1,					 // bNumDescriptors
	0x22,				 // bDescriptorType: Report
	CONSUMER_HID_DESC_SIZE, // wDescriptorLength LSB
	0,					 // wDescriptorLength MSB

	// Endpoint 4 IN (consumer)
	7,					 // bLength
	5,					 // bDescriptorType: Endpoint
	CONSUMER_ENDPOINT_NUM | 0x80, // bEndpointAddress: IN
	0x03,				 // bmAttributes: Interrupt
	CONSUMER_SIZE, 0,	 // wMaxPacketSize
	4,					 // bInterval: 4ms
};

int usb_init() {
	cli();

	UHWCON |= (1 << UVREGE); // Enable USB pads regulator
	PLLCSR |= 0x12;	// PLL config for 16MHz

	// Wait for PLL lock
	while (!(PLLCSR & (1 << PLOCK)));

	USBCON |= (1 << USBE) | (1 << OTGPADE);
	USBCON &= ~(1 << FRZCLK); // Unfreeze clock

	// Full speed mode
	UDCON &= ~(1 << LSM);
	UDCON &= ~(1 << DETACH);

	UDIEN |= (1 << EORSTE) | (1 << SOFE);

	usb_config_status = 0;
	sei();
	return 0;
}

u8 get_usb_config_status() {
	return usb_config_status != 0;
}

// Send keyboard report
int usb_send() {
	if (!usb_config_status) return -1;

	UENUM = KEYBOARD_ENDPOINT_NUM;

	UEDATX = keyboard_modifier;
	UEDATX = 0; // Reserved
	for (int i = 0; i < 6; i++) {
		UEDATX = keyboard_pressed_keys[i];
	}

	UEINTX = 0b00111010;
	current_idle = 0;
	return 0;
}

int send_keypress(u8 key, u8 mod) {
	keyboard_pressed_keys[0] = key;
	keyboard_modifier = mod;
	if (usb_send() < 0) return -1;

	keyboard_pressed_keys[0] = 0;
	keyboard_modifier = 0;
	if (usb_send() < 0) return -1;

	return 0;
}

// Send consumer key (media key)
int send_consumer_key(u16 keycode) {
	if (!usb_config_status) return -1;

	// Send key press
	UENUM = CONSUMER_ENDPOINT_NUM;
	UEDATX = (u8)(keycode & 0xFF);
	UEDATX = (u8)(keycode >> 8);
	UEINTX = 0b00111010;

	_delay_ms(10);

	// Send key release (zeros)
	UENUM = CONSUMER_ENDPOINT_NUM;
	UEDATX = 0;
	UEDATX = 0;
	UEINTX = 0b00111010;

	return 0;
}

// USB_GEN_vect, End of Reset + SOF
ISR(USB_GEN_vect) {
	u8 intbits = UDINT;
	UDINT = 0;

	// End of Reset
	if (intbits & (1 << EORSTI)) {
		UENUM = 0;
		UECONX = (1 << EPEN);
		UECFG0X = 0;
		UECFG1X = 0x22; // 32 bytes, 1 bank
		usb_config_status = 0;
		if (!(UESTA0X & (1 << CFGOK))) return;
		UERST = 1;
		UERST = 0;
		UEIENX = (1 << RXSTPE);
		return;
	}

	// Start of Frame (idle rate for keyboard)
	if ((intbits & (1 << SOFI)) && usb_config_status) {
		this_interrupt++;
		if (keyboard_idle_value && (this_interrupt & 3) == 0) {
			UENUM = KEYBOARD_ENDPOINT_NUM;
			if (UEINTX & (1 << RWAL)) {
				current_idle++;
				if (current_idle == keyboard_idle_value) {
					current_idle = 0;
					UEDATX = keyboard_modifier;
					UEDATX = 0;
					for (int i = 0; i < 6; i++) {
						UEDATX = keyboard_pressed_keys[i];
					}
					UEINTX = 0b00111010;
				}
			}
		}
	}
}

// USB_COM_vect, EP0 control endpoint
ISR(USB_COM_vect) {
	u8 bmRequestType, bRequest;
	u16 wValue, wIndex, wLength;

	UENUM = 0;
	if (!(UEINTX & (1 << RXSTPI))) return;

	// Read SETUP packet
	bmRequestType = UEDATX;
	bRequest = UEDATX;
	wValue  = UEDATX | ((u16)UEDATX << 8);
	wIndex  = UEDATX | ((u16)UEDATX << 8);
	wLength = UEDATX | ((u16)UEDATX << 8);

	UEINTX &= ~((1 << RXSTPI) | (1 << RXOUTI) | (1 << TXINI));

	if (bRequest == GET_DESCRIPTOR) {
		const u8 *descriptor = 0;
		u16 descriptor_length = 0;

		u8 desc_type  = (wValue >> 8) & 0xFF;
		// u8 desc_index = wValue & 0xFF;

		switch(desc_type) {
			case DESC_DEVICE: {
				descriptor = device_descriptor;
				descriptor_length = 18;
			} break;
			case DESC_CONFIGURATION: {
				descriptor = configuration_descriptor;
				descriptor_length = CONFIG_SIZE;
			} break;
			case DESC_HID_DESCRIPTOR: {
				u8 iface = wIndex & 0xFF;
				if (iface == KEYBOARD_INTERFACE) {
					descriptor = configuration_descriptor + KEYBOARD_HID_OFFSET;
					descriptor_length = 9;
				} else if (iface == CONSUMER_INTERFACE) {
					descriptor = configuration_descriptor + CONSUMER_HID_OFFSET;
					descriptor_length = 9;
				}
			} break;
			case DESC_HID_REPORT_DESCRIPTOR: {
				u8 iface = wIndex & 0xFF;
				if (iface == KEYBOARD_INTERFACE) {
					descriptor = keyboard_HID_descriptor;
					descriptor_length = KEYBOARD_HID_DESC_SIZE;
				} else if (iface == CONSUMER_INTERFACE) {
					descriptor = consumer_HID_descriptor;
					descriptor_length = CONSUMER_HID_DESC_SIZE;
				}
			} break;
		}
		
		if (!descriptor) {
			UECONX |= (1 << STALLRQ) | (1 << EPEN);
			return;
		}

		// Send descriptor
		u16 request_length = (wLength > 255) ? 255 : wLength;
		descriptor_length = (request_length > descriptor_length) ? descriptor_length : request_length;

		while (descriptor_length > 0) {
			while (!(UEINTX & (1 << TXINI)));
			if (UEINTX & (1 << RXOUTI)) return;

			u8 packet_size = (descriptor_length > 32) ? 32 : descriptor_length;
			for (u8 i = 0; i < packet_size; i++) {
				UEDATX = pgm_read_byte(descriptor++);
			}

			descriptor_length -= packet_size;
			UEINTX &= ~(1 << TXINI);
		}
		return;
	}

	if (bRequest == SET_CONFIGURATION && bmRequestType == 0) {
		usb_config_status = wValue;
		UEINTX &= ~(1 << TXINI);

		// Configure keyboard endpoint
		UENUM = KEYBOARD_ENDPOINT_NUM;
		UECONX = 1;
		UECFG0X = 0b11000001; // Interrupt IN
		UECFG1X = 0b00000110; // 8 bytes, double-banked

		// Configure consumer endpoint
		UENUM = CONSUMER_ENDPOINT_NUM;
		UECONX = 1;
		UECFG0X = 0b11000001; // Interrupt IN
		UECFG1X = 0b00000010; // 8 bytes, single-banked (consumer needs 2, but 8 is min)

		// Reset endpoints
		UERST = 0x1E;
		UERST = 0;
		return;
	}

	if (bRequest == SET_ADDRESS) {
		UEINTX &= ~(1 << TXINI);
		while (!(UEINTX & (1 << TXINI)));
		UDADDR = wValue | (1 << ADDEN);
		return;
	}

	if (bRequest == GET_CONFIGURATION && bmRequestType == 0x80) {
		while (!(UEINTX & (1 << TXINI)));
		UEDATX = usb_config_status;
		UEINTX &= ~(1 << TXINI);
		return;
	}

	if (bRequest == GET_STATUS) {
		while (!(UEINTX & (1 << TXINI)));
		UEDATX = 0;
		UEDATX = 0;
		UEINTX &= ~(1 << TXINI);
		return;
	}

	// HID class requests
	u8 iface = wIndex & 0xFF;

	// GET_REPORT
	if (bRequest == GET_REPORT && bmRequestType == 0xA1) {
		while (!(UEINTX & (1 << TXINI)));
		if (iface == KEYBOARD_INTERFACE) {
			UEDATX = keyboard_modifier;
			for (int i = 0; i < 6; i++) {
				UEDATX = keyboard_pressed_keys[i];
			}
		} else if (iface == CONSUMER_INTERFACE) {
			UEDATX = (u8)(consumer_key & 0xFF);
			UEDATX = (u8)(consumer_key >> 8);
		}
		UEINTX &= ~(1 << TXINI);
		return;
	}

	// GET_IDLE (keyboard only)
	if (bRequest == GET_IDLE && bmRequestType == 0xA1 && iface == KEYBOARD_INTERFACE) {
		while (!(UEINTX & (1 << TXINI)));
		UEDATX = keyboard_idle_value;
		UEINTX &= ~(1 << TXINI);
		return;
	}

	// SET_IDLE (keyboard only)
	if (bRequest == SET_IDLE && bmRequestType == 0x21 && iface == KEYBOARD_INTERFACE) {
		keyboard_idle_value = wValue;
		current_idle = 0;
		UEINTX &= ~(1 << TXINI);
		return;
	}

	// GET_PROTOCOL (keyboard only)
	if (bRequest == GET_PROTOCOL && bmRequestType == 0xA1 && iface == KEYBOARD_INTERFACE) {
		while (!(UEINTX & (1 << TXINI)));
		UEDATX = keyboard_protocol;
		UEINTX &= ~(1 << TXINI);
		return;
	}

	// SET_PROTOCOL (keyboard only)
	if (bRequest == SET_PROTOCOL && bmRequestType == 0x21 && iface == KEYBOARD_INTERFACE) {
		keyboard_protocol = wValue >> 8;
		UEINTX &= ~(1 << TXINI);
		return;
	}

	// Unsupported request, stall
	UECONX |= (1 << STALLRQ) | (1 << EPEN);
}