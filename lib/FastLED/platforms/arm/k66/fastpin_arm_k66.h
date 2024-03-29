#ifndef __FASTPIN_ARM_K66_H
#define __FASTPIN_ARM_K66_H

FASTLED_NAMESPACE_BEGIN

#if defined(FASTLED_FORCE_SOFTWARE_PINS)
#warning "Software pin support forced, pin access will be slightly slower."
#define NO_HARDWARE_PIN_SUPPORT
#undef HAS_HARDWARE_PIN_SUPPORT

#else


/// Template definition for teensy 3.0 style ARM pins, providing direct access to the various GPIO registers.  Note that this
/// uses the full port GPIO registers.  In theory, in some way, bit-band register access -should- be faster, however I have found
/// that something about the way gcc does register allocation results in the bit-band code being slower.  It will need more fine tuning.
/// The registers are data output, set output, clear output, toggle output, input, and direction
template<uint8_t PIN, uint32_t _MASK, typename _PDOR, typename _PSOR, typename _PCOR, typename _PTOR, typename _PDIR, typename _PDDR> class _ARMPIN {
public:
	typedef volatile uint32_t * port_ptr_t;
	typedef uint32_t port_t;

	inline static void setOutput() { pinMode(PIN, OUTPUT); } // TODO: perform MUX config { _PDDR::r() |= _MASK; }
	inline static void setInput() { pinMode(PIN, INPUT); } // TODO: preform MUX config { _PDDR::r() &= ~_MASK; }

	inline static void hi() __attribute__ ((always_inline)) { _PSOR::r() = _MASK; }
	inline static void lo() __attribute__ ((always_inline)) { _PCOR::r() = _MASK; }
	inline static void set(register port_t val) __attribute__ ((always_inline)) { _PDOR::r() = val; }

	inline static void strobe() __attribute__ ((always_inline)) { toggle(); toggle(); }

	inline static void toggle() __attribute__ ((always_inline)) { _PTOR::r() = _MASK; }

	inline static void hi(register port_ptr_t port) __attribute__ ((always_inline)) { hi(); }
	inline static void lo(register port_ptr_t port) __attribute__ ((always_inline)) { lo(); }
	inline static void fastset(register port_ptr_t port, register port_t val) __attribute__ ((always_inline)) { *port = val; }

	inline static port_t hival() __attribute__ ((always_inline)) { return _PDOR::r() | _MASK; }
	inline static port_t loval() __attribute__ ((always_inline)) { return _PDOR::r() & ~_MASK; }
	inline static port_ptr_t port() __attribute__ ((always_inline)) { return &_PDOR::r(); }
	inline static port_ptr_t sport() __attribute__ ((always_inline)) { return &_PSOR::r(); }
	inline static port_ptr_t cport() __attribute__ ((always_inline)) { return &_PCOR::r(); }
	inline static port_t mask() __attribute__ ((always_inline)) { return _MASK; }
};

/// Template definition for teensy 3.0 style ARM pins using bit banding, providing direct access to the various GPIO registers.  GCC
/// does a poor job of optimizing around these accesses so they are not being used just yet.
template<uint8_t PIN, int _BIT, typename _PDOR, typename _PSOR, typename _PCOR, typename _PTOR, typename _PDIR, typename _PDDR> class _ARMPIN_BITBAND {
public:
	typedef volatile uint32_t * port_ptr_t;
	typedef uint32_t port_t;

	inline static void setOutput() { pinMode(PIN, OUTPUT); } // TODO: perform MUX config { _PDDR::r() |= _MASK; }
	inline static void setInput() { pinMode(PIN, INPUT); } // TODO: preform MUX config { _PDDR::r() &= ~_MASK; }

	inline static void hi() __attribute__ ((always_inline)) { *_PDOR::template rx<_BIT>() = 1; }
	inline static void lo() __attribute__ ((always_inline)) { *_PDOR::template rx<_BIT>() = 0; }
	inline static void set(register port_t val) __attribute__ ((always_inline)) { *_PDOR::template rx<_BIT>() = val; }

	inline static void strobe() __attribute__ ((always_inline)) { toggle(); toggle(); }

	inline static void toggle() __attribute__ ((always_inline)) { *_PTOR::template rx<_BIT>() = 1; }

	inline static void hi(register port_ptr_t port) __attribute__ ((always_inline)) { hi();  }
	inline static void lo(register port_ptr_t port) __attribute__ ((always_inline)) { lo(); }
	inline static void fastset(register port_ptr_t port, register port_t val) __attribute__ ((always_inline)) { *_PDOR::template rx<_BIT>() = val; }

	inline static port_t hival() __attribute__ ((always_inline)) { return 1; }
	inline static port_t loval() __attribute__ ((always_inline)) { return 0; }
	inline static port_ptr_t port() __attribute__ ((always_inline)) { return _PDOR::template rx<_BIT>(); }
	inline static port_t mask() __attribute__ ((always_inline)) { return 1; }
};

// Macros for k20 pin access/definition
#define GPIO_BITBAND_ADDR(reg, bit) (((uint32_t)&(reg) - 0x40000000) * 32 + (bit) * 4 + 0x42000000)
#define GPIO_BITBAND_PTR(reg, bit) ((uint32_t *)GPIO_BITBAND_ADDR((reg), (bit)))

#define _R(T) struct __gen_struct_ ## T
#define _RD32(T) struct __gen_struct_ ## T { static __attribute__((always_inline)) inline reg32_t r() { return T; } \
	template<int BIT> static __attribute__((always_inline)) inline ptr_reg32_t rx() { return GPIO_BITBAND_PTR(T, BIT); } };
#define _IO32(L) _RD32(GPIO ## L ## _PDOR); _RD32(GPIO ## L ## _PSOR); _RD32(GPIO ## L ## _PCOR); _RD32(GPIO ## L ## _PTOR); _RD32(GPIO ## L ## _PDIR); _RD32(GPIO ## L ## _PDDR);

#define _DEFPIN_ARM(PIN, BIT, L) template<> class FastPin<PIN> : public _ARMPIN<PIN, 1 << BIT, _R(GPIO ## L ## _PDOR), _R(GPIO ## L ## _PSOR), _R(GPIO ## L ## _PCOR), \
																			_R(GPIO ## L ## _PTOR), _R(GPIO ## L ## _PDIR), _R(GPIO ## L ## _PDDR)> {}; \
									template<> class FastPinBB<PIN> : public _ARMPIN_BITBAND<PIN, BIT, _R(GPIO ## L ## _PDOR), _R(GPIO ## L ## _PSOR), _R(GPIO ## L ## _PCOR), \
 																			_R(GPIO ## L ## _PTOR), _R(GPIO ## L ## _PDIR), _R(GPIO ## L ## _PDDR)> {};

// Actual pin definitions
#if defined(FASTLED_TEENSY3) && defined(CORE_TEENSY)

_IO32(A); _IO32(B); _IO32(C); _IO32(D); _IO32(E);

#define MAX_PIN 63
_DEFPIN_ARM( 0, 16, B); _DEFPIN_ARM( 1, 17, B); _DEFPIN_ARM( 2,  0, D); _DEFPIN_ARM( 3, 12, A);
_DEFPIN_ARM( 4, 13, A); _DEFPIN_ARM( 5,  7, D); _DEFPIN_ARM( 6,  4, D); _DEFPIN_ARM( 7,  2, D);
_DEFPIN_ARM( 8,  3, D); _DEFPIN_ARM( 9,  3, C); _DEFPIN_ARM(10,  4, C); _DEFPIN_ARM(11,  6, C);
_DEFPIN_ARM(12,  7, C); _DEFPIN_ARM(13,  5, C); _DEFPIN_ARM(14,  1, D); _DEFPIN_ARM(15,  0, C);
_DEFPIN_ARM(16,  0, B); _DEFPIN_ARM(17,  1, B); _DEFPIN_ARM(18,  3, B); _DEFPIN_ARM(19,  2, B);
_DEFPIN_ARM(20,  5, D); _DEFPIN_ARM(21,  6, D); _DEFPIN_ARM(22,  1, C); _DEFPIN_ARM(23,  2, C);
_DEFPIN_ARM(24, 26, E); _DEFPIN_ARM(25,  5, A); _DEFPIN_ARM(26, 14, A); _DEFPIN_ARM(27, 15, A);
_DEFPIN_ARM(28, 16, A); _DEFPIN_ARM(29, 18, B); _DEFPIN_ARM(30, 19, B); _DEFPIN_ARM(31, 10, B);
_DEFPIN_ARM(32, 11, B); _DEFPIN_ARM(33, 24, E); _DEFPIN_ARM(34, 25, E); _DEFPIN_ARM(35,  8, C);
_DEFPIN_ARM(36,  9, C); _DEFPIN_ARM(37, 10, C); _DEFPIN_ARM(38, 11, C); _DEFPIN_ARM(39, 17, A);
_DEFPIN_ARM(40, 28, A); _DEFPIN_ARM(41, 29, A); _DEFPIN_ARM(42, 26, A); _DEFPIN_ARM(43, 20, B);
_DEFPIN_ARM(44, 22, B); _DEFPIN_ARM(45, 23, B); _DEFPIN_ARM(46, 21, B); _DEFPIN_ARM(47,  8, D);
_DEFPIN_ARM(48,  9, D); _DEFPIN_ARM(49,  4, B); _DEFPIN_ARM(50,  5, B); _DEFPIN_ARM(51, 14, D);
_DEFPIN_ARM(52, 13, D); _DEFPIN_ARM(53, 12, D); _DEFPIN_ARM(54, 15, D); _DEFPIN_ARM(55, 11, D);
_DEFPIN_ARM(56, 10, E); _DEFPIN_ARM(57, 11, E); _DEFPIN_ARM(58,  0, E); _DEFPIN_ARM(59,  1, E);
_DEFPIN_ARM(60,  2, E); _DEFPIN_ARM(61,  3, E); _DEFPIN_ARM(62,  4, E); _DEFPIN_ARM(63,  5, E);



#define SPI_DATA 11
#define SPI_CLOCK 13

#define SPI2_DATA 7
#define SPI2_CLOCK 14

#define FASTLED_TEENSY3
#define ARM_HARDWARE_SPI
#define HAS_HARDWARE_PIN_SUPPORT
#endif

#endif // FASTLED_FORCE_SOFTWARE_PINS

FASTLED_NAMESPACE_END

#endif // __INC_FASTPIN_ARM_K66
