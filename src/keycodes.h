#ifndef KEYCODES_H
#define KEYCODES_H

// USB-HID keyboard page 0x07
#define KC_NONE	0x00
#define KC_ERR_ROLLOVER	0x01
#define KC_POST_FAIL	0x02
#define KC_ERR_UNDEF	0x03

#define KC_A	0x04
#define KC_B	0x05
#define KC_C	0x06
#define KC_D	0x07
#define KC_E	0x08
#define KC_F	0x09
#define KC_G	0x0A
#define KC_H	0x0B
#define KC_I	0x0C
#define KC_J	0x0D
#define KC_K	0x0E
#define KC_L	0x0F
#define KC_M	0x10
#define KC_N	0x11
#define KC_O	0x12
#define KC_P	0x13
#define KC_Q	0x14
#define KC_R	0x15
#define KC_S	0x16
#define KC_T	0x17
#define KC_U	0x18
#define KC_V	0x19
#define KC_W	0x1A
#define KC_X	0x1B
#define KC_Y	0x1C
#define KC_Z	0x1D

#define KC_1	0x1E
#define KC_2	0x1F
#define KC_3	0x20
#define KC_4	0x21
#define KC_5	0x22
#define KC_6	0x23
#define KC_7	0x24
#define KC_8	0x25
#define KC_9	0x26
#define KC_0	0x27

#define KC_ENTER	0x28
#define KC_ESC		0x29
#define KC_BACKSPACE	0x2A
#define KC_TAB		0x2B
#define KC_SPACE	0x2C
#define KC_MINUS	0x2D
#define KC_EQUAL	0x2E
#define KC_LBRACE	0x2F
#define KC_RBRACE	0x30
#define KC_BACKSLASH	0x31
#define KC_HASHTILDE	0x32
#define KC_SEMICOLON	0x33
#define KC_APOSTROPHE	0x34
#define KC_GRAVE	0x35
#define KC_COMMA	0x36
#define KC_SLASH	0x38
#define KC_CAPSLOCK	0x39

#define KC_F1	0x3A
#define KC_F2	0x3B
#define KC_F3	0x3D
// ...
#define KC_12	0x45

#define KC_SYSRQ	0x46 // printscreen
#define KC_SCROLLLOCK	0x47
#define KC_INSERT	0x49
#define KC_HOME		0x4A
#define KC_PAGEUP	0x4B
#define KC_DELETE	0x4C
#define KC_END		0x4D
#define KC_PAGEDOWN	0x4E
#define KC_RIGHT	0x4F
#define KC_LEFT		0x50
#define KC_DOWN		0x51
#define KC_UP		0x52

// keypad
#define KC_NUMLOCK	0x53
#define KC_KPSLASH	0x54
#define KC_KPASTERISK	0x55
#define KC_KPMINUS	0x56
#define KC_KPPLUS	0x57
#define KC_KPENTER	0x58
#define KC_KP1		0x59
#define KC_KP2		0x5A
// ...
#define KC_KP0		0x62
#define KC_KPDOT	0x63

#define KC_102ND	0x64 // non-us backslash and |
#define KC_COMPOSE	0x65
#define KC_POWER	0x66
#define KC_KPEQUAL	0x67

#define KC_F13		0x68
// ...
#define KC_F24		0x73

#define KC_OPEN		0x74
#define KC_HELP		0x75
#define KC_PROPS	0x76
#define KC_FRONT	0x77
#define KC_STOP		0x78
#define KC_AGAIN	0x79
#define KC_UNDO		0x7A
#define KC_CUT		0x7B
#define KC_COPY		0x7C
#define KC_PASTE	0x7D
#define KC_FIND		0x7E
#define KC_MUTE		0x7F
#define KC_VOLUMEUP		0x80
#define KC_VOLUMEDOWN		0x81

#define KC_KPCOMMA	0x85
#define KC_RO		0x87
#define KC_KATAKANAHIRAGANA	0x88
#define KC_YEN		0x89
#define KC_HENKAN	0x8A
#define KC_MUHENKAN	0x8B
#define KC_KPJPCOMMA	0x8C

#define KC_HANGEUL	0x90
#define KC_HANJA	0x91
#define KC_KATAKANA	0x92
#define KC_HIRAGANA	0x93
#define KC_ZENKAKUHANKAKU	0x94

#define KC_KPLPAREN	0xB6
#define KC_KPRPAREN	0xB7

#define KC_LCTRL	0xE0
#define KC_LSHIFT	0xE1
#define KC_LALT		0xE2
#define KC_LGUI		0xE3
#define KC_RCTRL	0xE4
#define KC_RSHIFT	0xE5
#define KC_RALT		0xE6
#define KC_RGUI		0xE7

#if 0
// dont know about these at all
// wrong?
#define KC_MEDIA_PLAYPAUSE	0xCD // 0xE8
#define KC_MEDIA_STOPCD		0xE9
#define KC_MEDIA_PREVSONG	0xEA
#define KC_MEDIA_NEXTSONG	0xEB
#define KC_MEDIA_EJECT_CD	0xEC
#define KC_MEDIA_VOLUMEUP	0xED
#define KC_MEDIA_VOLUMEDOWN 0xEE
#define KC_MEDIA_MUTE		0xEF
#define KC_MEDIA_WWW		0xF0
#define KC_MEDIA_BACK		0xF1
#define KC_MEDIA_FORWARD	0xF2
#define KC_MEDIA_STOP		0xF3
#define KC_MEDIA_FIND		0xF4
#define KC_MEDIA_SCROLLUP	0xF5
#define KC_MEDIA_SCROLLDOWN	0xF6
#define KC_MEDIA_EDIT		0xF7
#define KC_MEDIA_SLEEP		0xF8
#define KC_MEDIA_COFFEE		0xF9
#define KC_MEDIA_REFRESH	0xFA
#define KC_MEDIA_CALC		0xFB
#endif

#define KC_MEDIA_NONE           0x00
#define KC_MEDIA_PLAY           0xB0
#define KC_MEDIA_PAUSE          0xB1
#define KC_MEDIA_NEXT_TRACK     0xB5
#define KC_MEDIA_PREV_TRACK     0xB6
#define KC_MEDIA_STOP           0xB7
#define KC_MEDIA_PLAY_PAUSE     0xCD
#define KC_MEDIA_MUTE           0xE2
#define KC_MEDIA_VOL_UP         0xE9
#define KC_MEDIA_VOL_DOWN       0xEA
#define KC_MEDIA_BRIGHTNESS_UP  0x6F
#define KC_MEDIA_BRIGHTNESS_DOWN  0x70
#define KC_MEDIA_KEYBOARD_BRIGHTNESS_INC	0x79
#define KC_MEDIA_KEYBOARD_BRIGHTNESS_DEC	0x7A
#define KC_MEDIA_BRIGHTNESS_MIN	0x73
#define KC_MEDIA_BRIGHTNESS_MAX	0x74

// AL = Application Launch
#define KC_MEDIA_AL_WORDPROC		0x184
#define KC_MEDIA_AL_TEXTEDITOR		0x185
#define KC_MEDIA_AL_SPREADSHEET		0x186
// ...
#define KC_MEDIA_AL_CALC			0x192
#define KC_MEDIA_AL_BROWSER			0x196
#define KC_MEDIA_AL_CONTROL_PANEL	0x19F
#define KC_MEDIA_AL_RUNCMD			0x1A0
#define KC_MEDIA_AL_TASK_MANAGER	0x1A1
// ...
#define KC_MEDIA_AL_FILE_BROSWER	0x1B4
#define KC_MEDIA_AL_IMAGE_BROWSER	0x1B6
// ...
#define KC_MEDIA_AC_NEW		0x201
#define KC_MEDIA_AC_OPEN	0x202
#define KC_MEDIA_AC_CLOSE	0x203
#define KC_MEDIA_AC_EXIT	0x204
#define KC_MEDIA_AC_SAVE	0x207
#define KC_MEDIA_AC_UNDO	0x21A
// ... lots of em (here is 12 bits used (technically 10 bits))

// page 0x0C
#define KC_MEDIA_CC_VOLUME_UP	0xE9
#define KC_MEDIA_CC_VOLUME_DOWN	0xEA
#define KC_MEDIA_CC_MUTE		0xE2

#endif /* KEYCODES_H */