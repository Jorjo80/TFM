#ifndef _SYS__ENDIAN_H_
#define _SYS__ENDIAN_H_


#define __FROM_SYS__ENDIAN

#undef __FROM_SYS__ENDIAN

#define _LITTLE_ENDIAN	1234
#define _BIG_ENDIAN	4321
#define _PDP_ENDIAN	3412

/* Note that these macros evaluate their arguments several times.  */

#ifndef BIG_ENDIAN
#define BIG_ENDIAN 4321
#endif
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#endif
#ifndef BYTE_ORDER
#if defined(__IEEE_LITTLE_ENDIAN) || defined(__IEEE_BYTES_LITTLE_ENDIAN)
#define BYTE_ORDER LITTLE_ENDIAN
#else
#define BYTE_ORDER BIG_ENDIAN
#endif
#endif

#define _SIZE_T
#define _SSIZE_T
#define _PTRDIFF_T
typedef unsigned long  size_t;
typedef long           ssize_t;
typedef long           ptrdiff_t;
#endif
/* 7.18.1.1 Exact-width integer types */
typedef	__signed char		__int8_t;
typedef	unsigned char		__uint8_t;
typedef	short			__int16_t;
typedef	unsigned short		__uint16_t;
typedef	int			__int32_t;
typedef	unsigned int		__uint32_t;
/* LONGLONG */
typedef	long long		__int64_t;
/* LONGLONG */
typedef	unsigned long long	__uint64_t;
/* 7.18.1.2 Minimum-width integer types */
typedef	__int8_t		__int_least8_t;
typedef	__uint8_t		__uint_least8_t;
typedef	__int16_t		__int_least16_t;
typedef	__uint16_t		__uint_least16_t;
typedef	__int32_t		__int_least32_t;
typedef	__uint32_t		__uint_least32_t;
typedef	__int64_t		__int_least64_t;
typedef	__uint64_t		__uint_least64_t;
/* 7.18.1.3 Fastest minimum-width integer types */
typedef	__int32_t		__int_fast8_t;
typedef	__uint32_t		__uint_fast8_t;
typedef	__int32_t		__int_fast16_t;
typedef	__uint32_t		__uint_fast16_t;
typedef	__int32_t		__int_fast32_t;
typedef	__uint32_t		__uint_fast32_t;
typedef	__int64_t		__int_fast64_t;
typedef	__uint64_t		__uint_fast64_t;
/* 7.18.1.4 Integer types capable of holding object pointers */
typedef	int 			__intptr_t;
typedef	unsigned int 		__uintptr_t;
/* 7.18.1.5 Greatest-width integer types */
typedef	__int64_t		__intmax_t;
typedef	__uint64_t		__uintmax_t;
/* Register size */
typedef __int32_t		__register_t;
/* VM system types */
typedef unsigned long		__vaddr_t;
typedef unsigned long		__paddr_t;
typedef unsigned long		__vsize_t;
typedef unsigned long		__psize_t;
/* Standard system types */
typedef int			__clock_t;
typedef int			__clockid_t;
typedef long			__ptrdiff_t;
typedef	int			__time_t;
typedef int			__timer_t;
#if defined(__GNUC__) && __GNUC__ >= 3
typedef	__builtin_va_list	__va_list;
#else
typedef	char *			__va_list;
#endif
/* Wide character support types */
#ifndef __cplusplus
typedef	int			__wchar_t;
#endif
typedef int			__wint_t;
typedef	int			__rune_t;
typedef	void *			__wctrans_t;
typedef	void *			__wctype_t;

#define __swap16gen(x)							\
    (__uint16_t)(((__uint16_t)(x) & 0xffU) << 8 | ((__uint16_t)(x) & 0xff00U) >> 8)

#define __swap32gen(x)							\
    (__uint32_t)(((__uint32_t)(x) & 0xff) << 24 |			\
    ((__uint32_t)(x) & 0xff00) << 8 | ((__uint32_t)(x) & 0xff0000) >> 8 |\
    ((__uint32_t)(x) & 0xff000000) >> 24)

#define __swap64gen(x)							\
	(__uint64_t)((((__uint64_t)(x) & 0xff) << 56) |			\
	    ((__uint64_t)(x) & 0xff00ULL) << 40 |			\
	    ((__uint64_t)(x) & 0xff0000ULL) << 24 |			\
	    ((__uint64_t)(x) & 0xff000000ULL) << 8 |			\
	    ((__uint64_t)(x) & 0xff00000000ULL) >> 8 |			\
	    ((__uint64_t)(x) & 0xff0000000000ULL) >> 24 |		\
	    ((__uint64_t)(x) & 0xff000000000000ULL) >> 40 |		\
	    ((__uint64_t)(x) & 0xff00000000000000ULL) >> 56)

#ifndef __HAVE_MD_SWAP
static __inline __uint16_t
__swap16md(__uint16_t x)
{
	return (__swap16gen(x));
}

static __inline __uint32_t
__swap32md(__uint32_t x)
{
	return (__swap32gen(x));
}

static __inline __uint64_t
__swap64md(__uint64_t x)
{
	return (__swap64gen(x));
}
#endif

#define __swap16(x)							\
	(__uint16_t)(__builtin_constant_p(x) ? __swap16gen(x) : __swap16md(x))
#define __swap32(x)							\
	(__uint32_t)(__builtin_constant_p(x) ? __swap32gen(x) : __swap32md(x))
#define __swap64(x)							\
	(__uint64_t)(__builtin_constant_p(x) ? __swap64gen(x) : __swap64md(x))

#if _BYTE_ORDER == _LITTLE_ENDIAN

#define _QUAD_HIGHWORD 1
#define _QUAD_LOWWORD 0

#define __htobe16	__swap16
#define __htobe32	__swap32
#define __htobe64	__swap64
#define __htole16(x)	((__uint16_t)(x))
#define __htole32(x)	((__uint32_t)(x))
#define __htole64(x)	((__uint64_t)(x))

#ifdef _KERNEL
#ifdef __HAVE_MD_SWAPIO

#define __bemtoh16(_x) __mswap16(_x)
#define __bemtoh32(_x) __mswap32(_x)
#define __bemtoh64(_x) __mswap64(_x)

#define __htobem16(_x, _v) __swapm16((_x), (_v))
#define __htobem32(_x, _v) __swapm32((_x), (_v))
#define __htobem64(_x, _v) __swapm64((_x), (_v))

#endif /* __HAVE_MD_SWAPIO */
#endif /* _KERNEL */
#endif /* _BYTE_ORDER == _LITTLE_ENDIAN */

#if _BYTE_ORDER == _BIG_ENDIAN

#define _QUAD_HIGHWORD 0
#define _QUAD_LOWWORD 1

#define __htobe16(x)	((__uint16_t)(x))
#define __htobe32(x)	((__uint32_t)(x))
#define __htobe64(x)	((__uint64_t)(x))
#define __htole16	__swap16
#define __htole32	__swap32
#define __htole64	__swap64

#ifdef _KERNEL
#ifdef __HAVE_MD_SWAPIO

#define __lemtoh16(_x) __mswap16(_x)
#define __lemtoh32(_x) __mswap32(_x)
#define __lemtoh64(_x) __mswap64(_x)

#define __htolem16(_x, _v) __swapm16((_x), (_v))
#define __htolem32(_x, _v) __swapm32((_x), (_v))
#define __htolem64(_x, _v) __swapm64((_x), (_v))

#endif /* __HAVE_MD_SWAPIO */
#endif /* _KERNEL */
#endif /* _BYTE_ORDER == _BIG_ENDIAN */


#ifdef _KERNEL
/*
 * Fill in the __hto[bl]em{16,32,64} and __[bl]emtoh{16,32,64} macros
 * that haven't been defined yet
 */

#ifndef __bemtoh16
#define __bemtoh16(_x)		__htobe16(*(__uint16_t *)(_x))
#define __bemtoh32(_x)		__htobe32(*(__uint32_t *)(_x))
#define __bemtoh64(_x)		__htobe64(*(__uint64_t *)(_x))
#endif

#ifndef __htobem16
#define __htobem16(_x, _v)	(*(__uint16_t *)(_x) = __htobe16(_v))
#define __htobem32(_x, _v)	(*(__uint32_t *)(_x) = __htobe32(_v))
#define __htobem64(_x, _v)	(*(__uint64_t *)(_x) = __htobe64(_v))
#endif

#ifndef __lemtoh16
#define __lemtoh16(_x)		__htole16(*(__uint16_t *)(_x))
#define __lemtoh32(_x)		__htole32(*(__uint32_t *)(_x))
#define __lemtoh64(_x)		__htole64(*(__uint64_t *)(_x))
#endif

#ifndef __htolem16
#define __htolem16(_x, _v)	(*(__uint16_t *)(_x) = __htole16(_v))
#define __htolem32(_x, _v)	(*(__uint32_t *)(_x) = __htole32(_v))
#define __htolem64(_x, _v)	(*(__uint64_t *)(_x) = __htole64(_v))
#endif
#endif /* _KERNEL */

#endif /* _SYS__ENDIAN_H_ */
