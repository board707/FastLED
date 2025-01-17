#ifndef __INC_LED_SYSDEFS_CSKY_W80X_H
#define __INC_LED_SYSDEFS_CSKY_W80X_H

//#include "hardware/sync.h"

#define FASTLED_HLK_w80x


// TODO: PORT SPI TO HW
//#define FASTLED_FORCE_SOFTWARE_SPI

#ifndef FAST_SPI_INTERRUPTS_WRITE_PINS
#define FAST_SPI_INTERRUPTS_WRITE_PINS 1
#endif

#define FASTLED_NO_PINMAP

typedef volatile uint32_t RoReg;
typedef volatile uint32_t RwReg;

// #define F_CPU clock_get_hz(clk_sys) // can't use runtime function call
// is the boot-time value in another var already for any platforms?
// it doesn't seem to be, so hardcode the sdk default of 125 MHz
#ifndef F_CPU
#define F_CPU 240000000
#endif

#ifndef VARIANT_MCK
#define VARIANT_MCK F_CPU
#endif

// 8MHz for PIO
// #define CLOCKLESS_FREQUENCY 8000000
#define CLOCKLESS_FREQUENCY F_CPU

// Default to allowing interrupts
#ifndef FASTLED_ALLOW_INTERRUPTS
#define FASTLED_ALLOW_INTERRUPTS 1
#endif

// not sure if this is wanted? but it probably is
#if FASTLED_ALLOW_INTERRUPTS == 1
#define FASTLED_ACCURATE_CLOCK
#endif

// Default to no PROGMEM
#ifndef FASTLED_USE_PROGMEM
#define FASTLED_USE_PROGMEM 0
#endif

// Default to blocking I2s-based implemnetation
#ifndef FASTLED_W80X_CLOCKLESS_I2S
#define FASTLED_W80X_CLOCKLESS_I2S 1
#endif

// Default to shared interrupt handler for clockless DMA
#ifndef FASTLED_W80X_CLOCKLESS_IRQ_SHARED
#define FASTLED_W80X_CLOCKLESS_IRQ_SHARED 1
#endif

// SPI pin defs for old SDK ver
#ifndef W80X_DEFAULT_SPI
#define W80X_DEFAULT_SPI 0
#endif
#ifndef W80X_DEFAULT_SPI_SCK_PIN
#define W80X_DEFAULT_SPI_SCK_PIN PIN_SPI_SCK
#endif
#ifndef W80X_DEFAULT_SPI_TX_PIN
#define W80X_DEFAULT_SPI_TX_PIN PIN_SPI_MOSI
#endif
#ifndef W80X_DEFAULT_SPI_RX_PIN
#define W80x_DEFAULT_SPI_RX_PIN PIN_SPI_MISO
#endif


//static uint32_t saved_interrupt_status;
//#define cli() (saved_interrupt_status = save_and_disable_interrupts())
//#define sei() (restore_interrupts(saved_interrupt_status))

#endif // __INC_LED_SYSDEFS_ARM_RP2040_H
