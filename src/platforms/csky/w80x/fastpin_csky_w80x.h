#ifndef __FASTPIN_CSKY_W80X_H
#define __FASTPIN_CSKY_W80X_H



FASTLED_NAMESPACE_BEGIN


#if defined(FASTLED_FORCE_SOFTWARE_PINS)
#warning "Software pin support forced, pin access will be sloightly slower."
#define NO_HARDWARE_PIN_SUPPORT
#undef HAS_HARDWARE_PIN_SUPPORT

#else

// warning: set and fastset are not thread-safe! use with caution!
template<uint8_t PIN, uint32_t _MASK, typename GPIO_PORT> class _W80XPIN {
public:
  typedef volatile GPIO_TypeDef* port_ptr_t;
  typedef uint32_t port_t;
  
  
 inline static void setOutput() { pinMode(PIN, OUTPUT); } // TODO: perform MUX config { _PDDR::r() |= _MASK; }
 inline static void setInput() { pinMode(PIN, INPUT); } // TODO: preform MUX config { _PDDR::r() &= ~_MASK; }

  inline static void hi() __attribute__ ((always_inline)) { GPIO_PORT::r()->DATA |= _MASK; }
  inline static void lo() __attribute__ ((always_inline)) { GPIO_PORT::r()->DATA &= ~_MASK; }
  inline static void set(register port_t val) __attribute__ ((always_inline)) { GPIO_PORT::r()->DATA = val; }

  inline static void strobe() __attribute__ ((always_inline)) { toggle(); toggle(); }

  inline static void toggle() __attribute__ ((always_inline)) { if(GPIO_PORT::r()->DATA & _MASK) { lo(); } else { hi(); } }

  inline static void hi(register port_ptr_t port) __attribute__ ((always_inline)) { hi(); }
  inline static void lo(register port_ptr_t port) __attribute__ ((always_inline)) { lo(); }
  inline static void fastset(register port_ptr_t port, register port_t val) __attribute__ ((always_inline)) { set(val); }

  inline static port_t hival() __attribute__ ((always_inline)) { return GPIO_PORT::r()->DATA | _MASK; }
  inline static port_t loval() __attribute__ ((always_inline)) { return GPIO_PORT::r()->DATA & ~_MASK; }
  inline static port_ptr_t port() __attribute__ ((always_inline)) { return GPIO_PORT::r(); }
  inline static port_ptr_t sport() __attribute__ ((always_inline)) { return NULL; }
  inline static port_ptr_t cport() __attribute__ ((always_inline)) { return NULL; }
  inline static port_t mask() __attribute__ ((always_inline)) { return _MASK; }
};

#define _R(T) struct __gen_struct_ ## T
#define _RD32(T) struct __gen_struct_ ## T { static __attribute__((always_inline)) inline volatile GPIO_TypeDef* r() { return T; } };
#define _FL_IO(L) _RD32(GPIO ## L);


#define _FL_DEFPIN(PIN, PORT) template<> class FastPin<PIN> : public _W80XPIN< PIN, digitalPinToBitMask(PIN) ,  _R(GPIO ## PORT)> {};

#define MAX_PIN 29

_FL_IO(A);
_FL_IO(B);

_FL_DEFPIN(PB11, B); 
_FL_DEFPIN(PB14, B);
_FL_DEFPIN(PA0, A);
_FL_DEFPIN(PA10, A);
#ifdef W80X_DEFAULT_SPI_TX_PIN
#define SPI_DATA W80X_DEFAULT_SPI_TX_PIN
#else
#define SPI_DATA PIN_SPI_MOSI
#endif

#ifdef W80X_DEFAULT_SPI_SCK_PIN
#define SPI_CLOCK W80X_DEFAULT_SPI_SCK_PIN
#else
#define SPI_CLOCK PIN_SPI_SCK
#endif

#define HAS_HARDWARE_PIN_SUPPORT

#endif // FASTLED_FORCE_SOFTWARE_PINS


FASTLED_NAMESPACE_END

#endif // __FASTPIN_CSKY_W80X_H
