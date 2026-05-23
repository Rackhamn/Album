#ifndef KEYBASE_H
#define KEYBASE_H

#include "types.h"

// [ 3, 1, 4, 8 ] := { type, mod l/r bit, mod, value } as (MSB -> LSB)
typedef u16 keyval_t;

#define KEY_TYPE_MASK		0x0007 // 3 bits
#define KEY_PAYLOAD_MASK	0xFFF8 // rest 13 bits
#define KEY_PAYLOAD_SHIFT	3

#define KEY_GET_TYPE(k)		(((k) >> 13) & 0x7)
#define KEY_GET_MODLR(k)	(((k) >> 12) & 0x1)
#define KEY_GET_MOD(k)		(((k) >> 8) & 0xF)
#define KEY_GET_VAL(k)		((k) & 0xFF)

// TOOD: make enum (max 8 ones since we use 3 bits)
#define KEY_TYPE_KEY	0x0	// regular keys
#define KEY_TYPE_MEDIA	0x1 // page 0x0C, val is index into media key list
#define KEY_TYPE_MACRO	0x2	// calling a macro by index or whatever
#define KEY_TYPE_DEV	0x3	// doing device specific functions like layer++

// move to other file pls: also rename
#define KEY_DEV_LAYER_INC	0x0A
#define KEY_DEV_LAYER_DEC	0x0B
#define KEY_DEV_LAYER_HOME	0x0C

#define MOD_CTRL	(1 << 0)
#define MOD_SHIFT	(1 << 1)
#define MOD_ALT		(1 << 2)
#define MOD_GUI		(1 << 3)

#define MOD_LEFT	0
#define MOD_RIGHT	1

// TODO: rename
#define KEY_PACK(type, lrbit, mod, value) \
( \
	((keyval_t)(type) << 13) | \
	((keyval_t)(lrbit) << 12) | \
	((keyval_t)((mod) & 0xF) << 8) | \
	((keyval_t)(value)) \
)

#define K(keycode) \
	KEY_PACK(KEY_TYPE_KEY, 0, 0x00, (keycode))

#define KM(keycode, lrbit, mod) \
	KEY_PACK(KEY_TYPE_KEY, (lrbit), (mod), (keycode))

#define MK(index) \
	KEY_PACK(KEY_TYPE_MACRO, 0, 0x00, (index))

#define DK(index) \
	KEY_PACK(KEY_TYPE_DEV, 0, 0x00, (index))

// todo: media is packing incorrectly rn
#define M(value) \
	KEY_PACK(KEY_TYPE_MEDIA, 0, 0x00, value)

// helpers
#define KM_S(keycode) KM(keycode, MOD_SHIFT)
#define KM_C(keycode) KM(keycode, MOD_CTRL)
#define KM_A(keycode) KM(keycode, MOD_ALT)
#define KM_G(keycode) KM(keycode, MOD_GUI)

#endif /* KEYBASE_H */