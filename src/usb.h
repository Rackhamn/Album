#ifndef USB_H
#define USB_H

#include <stdbool.h>
#include <stdint.h>

#include "types.h"

#if 0
extern volatile u8 keyboard_pressed_keys[6];
extern volatile u8 keyboard_modifier;

// Consumer (media keys) state - 16-bit codes
extern volatile u16 consumer_key;  // Current consumer key being sent

extern u8 usb_config_status;
extern u8 keyboard_protocol;
#endif
int usb_init();
u8 get_usb_config_status();

// Keyboard functions
int usb_send();                        // Send current keyboard_pressed_keys state
int send_keypress(u8 key, u8 mod);    // Send press+release

// Consumer functions
int send_consumer_key(u16 keycode);    // Send consumer key press+release

#define idVendor    0x03eb
#define idProduct   0x2ff4

#define NUM_INTERFACES          2

// Interface 0: Keyboard (boot protocol)
#define KEYBOARD_INTERFACE      0
#define KEYBOARD_ENDPOINT_NUM   3
#define KEYBOARD_SIZE           8       // 8 bytes per report

// Interface 1: Consumer control (media keys)
#define CONSUMER_INTERFACE      1
#define CONSUMER_ENDPOINT_NUM   4
#define CONSUMER_SIZE           2       // 2 bytes per report (16-bit code)

// Keyboard HID report descriptor (63 bytes)
#define KEYBOARD_HID_DESC_SIZE  63

// Consumer HID report descriptor (25 bytes)
#define CONSUMER_HID_DESC_SIZE  25

// Configuration descriptor total size:
// 9 (config) + 9 (iface0) + 9 (HID0) + 7 (EP3) + 9 (iface1) + 9 (HID1) + 7 (EP4)
#define CONFIG_SIZE             (9 + 9 + 9 + 7 + 9 + 9 + 7)

// Offsets into config descriptor for HID descriptors
#define KEYBOARD_HID_OFFSET     (9 + 9)
#define CONSUMER_HID_OFFSET     (9 + 9 + 9 + 7 + 9)

#define GET_STATUS          0x00
#define CLEAR_FEATURE       0x01
#define SET_FEATURE         0x03
#define SET_ADDRESS         0x05
#define GET_DESCRIPTOR      0x06
#define GET_CONFIGURATION   0x08
#define SET_CONFIGURATION   0x09
#define GET_INTERFACE       0x0A
#define SET_INTERFACE       0x0B

#define DESC_DEVICE	0x01
#define DESC_CONFIGURATION 0x02
#define DESC_HID_DESCRIPTOR 0x21
#define DESC_HID_REPORT_DESCRIPTOR 0x22

// HID class-specific requests
#define GET_REPORT      0x01
#define GET_IDLE        0x02
#define GET_PROTOCOL    0x03
#define SET_REPORT      0x09
#define SET_IDLE        0x0A
#define SET_PROTOCOL    0x0B

// Consumer key codes (page 0x0C)
#define CONSUMER_NONE           0x0000
#define CONSUMER_PLAY           0x00B0
#define CONSUMER_PAUSE          0x00B1
#define CONSUMER_NEXT_TRACK     0x00B5
#define CONSUMER_PREV_TRACK     0x00B6
#define CONSUMER_STOP           0x00B7
#define CONSUMER_PLAY_PAUSE     0x00CD
#define CONSUMER_MUTE           0x00E2
#define CONSUMER_VOL_UP         0x00E9
#define CONSUMER_VOL_DOWN       0x00EA
#define CONSUMER_BRIGHTNESS_UP  0x006F
#define CONSUMER_BRIGHTNESS_DN  0x0070

#endif /* USB_H */