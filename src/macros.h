#ifndef MACROS_H
#define MACROS_H

#define ENABLE_OUTPUT_X(chan, pin) ((DDR##chan) |= (1 << (pin)))
#define ENABLE_INPUT_X(chan, pin) ((DDR##chan) &= ~(1 << (pin)))
#define TOGGLE_PIN_X(chan, pin) ((PORT##chan) ^= (1 << (pin)))
#define PULLUP_PIN_X(chan, pin) ((PORT##chan) |= (1 << (pin)))

#define SET_PIN_HIGH_X(chan, pin) (((PORT##chan) |= (1 << (pin))))
#define SET_PIN_LOW_X(chan, pin) (((PORT##chan) &= ~(1 << (pin))))
#define READ_PIN_X(chan, pin) ((((PIN##chan) & (1 << (pin)))) ? 1 : 0)

#define ENABLE_OUTPUT(x) ENABLE_OUTPUT_X(x)
#define ENABLE_INPUT(x) ENABLE_INPUT_X(x)
#define PULLUP_PIN(x) PULLUP_PIN_X(x)
#define TOGGLE_PIN(x) TOGGLE_PIN_X(x)
#define SET_PIN_HIGH(x) SET_PIN_HIGH_X(x)
#define SET_PIN_LOW(x) SET_PIN_LOW_X(x)
#define READ_PIN(x) READ_PIN_X(x)

#endif /* MACROS_H */