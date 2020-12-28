//
// Created by robin on 19.12.2020.
//

#ifndef POCKETLZMA_LZMA_C_H
#define POCKETLZMA_LZMA_C_H

//Notes - LZMA-files
/*!
 * #include "Precomp.h"
 * #include "Lzma2Dec.h"
 * #include "Lzma2Dec.c"
 * #include "Lzma2DecMt.h"
 * #include "Alloc.h"
 * #include "MtDec.h"
 *
 *
 *
 */

namespace plz
{
	namespace c
	{
		//H
		//#include "liblzma/api/lzma.h"

		// C O M M O N
		// -------------------
		//#include "liblzma/common/lzma.h"

		//H

/*** Start of inlined file: alone_decoder.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       alone_decoder.h
/// \brief      Decoder for LZMA_Alone files
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LZMA_ALONE_DECODER_H
#define LZMA_ALONE_DECODER_H


/*** Start of inlined file: common.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       common.h
/// \brief      Definitions common to the whole liblzma library
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LZMA_COMMON_H
#define LZMA_COMMON_H

//#include "../tuklib/sysdefs.h"

/*** Start of inlined file: mythread.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       mythread.h
/// \brief      Some threading related helper macros and functions
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MYTHREAD_H
#define MYTHREAD_H


/*** Start of inlined file: sysdefs.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       sysdefs.h
/// \brief      Common includes, definitions, system-specific things etc.
///
/// This file is used also by the lzma command line tool, that's why this
/// file is separate from common.h.
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LZMA_SYSDEFS_H
#define LZMA_SYSDEFS_H

//////////////
// Includes //
//////////////

//RBP: Defined this, as this is intended to be used with modern c++
#define HAVE_STDBOOL_H

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

// Get standard-compliant stdio functions under MinGW and MinGW-w64.
#ifdef __MINGW32__
#	define __USE_MINGW_ANSI_STDIO 1
#endif

// size_t and NULL
#include <stddef.h>

#ifdef HAVE_INTTYPES_H
#	include <inttypes.h>
#endif

// C99 says that inttypes.h always includes stdint.h, but some systems
// don't do that, and require including stdint.h separately.
#ifdef HAVE_STDINT_H
#	include <stdint.h>
#endif

// Some pre-C99 systems have SIZE_MAX in limits.h instead of stdint.h. The
// limits are also used to figure out some macros missing from pre-C99 systems.
#include <limits.h>

// Be more compatible with systems that have non-conforming inttypes.h.
// We assume that int is 32-bit and that long is either 32-bit or 64-bit.
// Full Autoconf test could be more correct, but this should work well enough.
// Note that this duplicates some code from lzma.h, but this is better since
// we can work without inttypes.h thanks to Autoconf tests.
#ifndef UINT32_C
#	if UINT_MAX != 4294967295U
#		error UINT32_C is not defined and unsigned int is not 32-bit.
#	endif
#	define UINT32_C(n) n ## U
#endif
#ifndef UINT32_MAX
#	define UINT32_MAX UINT32_C(4294967295)
#endif
#ifndef PRIu32
#	define PRIu32 "u"
#endif
#ifndef PRIx32
#	define PRIx32 "x"
#endif
#ifndef PRIX32
#	define PRIX32 "X"
#endif

#if ULONG_MAX == 4294967295UL
#	ifndef UINT64_C
#		define UINT64_C(n) n ## ULL
#	endif
#	ifndef PRIu64
#		define PRIu64 "llu"
#	endif
#	ifndef PRIx64
#		define PRIx64 "llx"
#	endif
#	ifndef PRIX64
#		define PRIX64 "llX"
#	endif
#else
#	ifndef UINT64_C
#		define UINT64_C(n) n ## UL
#	endif
#	ifndef PRIu64
#		define PRIu64 "lu"
#	endif
#	ifndef PRIx64
#		define PRIx64 "lx"
#	endif
#	ifndef PRIX64
#		define PRIX64 "lX"
#	endif
#endif
#ifndef UINT64_MAX
#	define UINT64_MAX UINT64_C(18446744073709551615)
#endif

// Incorrect(?) SIZE_MAX:
//   - Interix headers typedef size_t to unsigned long,
//     but a few lines later define SIZE_MAX to INT32_MAX.
//   - SCO OpenServer (x86) headers typedef size_t to unsigned int
//     but define SIZE_MAX to INT32_MAX.
#if defined(__INTERIX) || defined(_SCO_DS)
#	undef SIZE_MAX
#endif

// The code currently assumes that size_t is either 32-bit or 64-bit.
//RBP: commented out
//#ifndef SIZE_MAX
//#	if SIZEOF_SIZE_T == 4
//#		define SIZE_MAX UINT32_MAX
//#	elif SIZEOF_SIZE_T == 8
//#		define SIZE_MAX UINT64_MAX
//#	else
//#		error size_t is not 32-bit or 64-bit
//#	endif
//#endif

//RBP: commented out
//#if SIZE_MAX != UINT32_MAX && SIZE_MAX != UINT64_MAX
//#	error size_t is not 32-bit or 64-bit
//#endif

#include <stdlib.h>
#include <assert.h>

// Pre-C99 systems lack stdbool.h. All the code in LZMA Utils must be written
// so that it works with fake bool type, for example:
//
//    bool foo = (flags & 0x100) != 0;
//    bool bar = !!(flags & 0x100);
//
// This works with the real C99 bool but breaks with fake bool:
//
//    bool baz = (flags & 0x100);
//

#ifdef HAVE_STDBOOL_H
#	include <stdbool.h>
#else
#	if ! HAVE__BOOL
typedef unsigned char _Bool;
#	endif
#	define bool _Bool
#	define false 0
#	define true 1
#	define __bool_true_false_are_defined 1
#endif

// string.h should be enough but let's include strings.h and memory.h too if
// they exists, since that shouldn't do any harm, but may improve portability.
#include <string.h>

#ifdef HAVE_STRINGS_H
#	include <strings.h>
#endif

#ifdef HAVE_MEMORY_H
#	include <memory.h>
#endif

// As of MSVC 2013, inline and restrict are supported with
// non-standard keywords.
#if defined(_WIN32) && defined(_MSC_VER)
#	ifndef inline
#		define inline __inline
#	endif
#	ifndef restrict
#		define restrict __restrict
#	endif
#endif

////////////
// Macros //
////////////

#undef memzero
#define memzero(s, n) memset(s, 0, n)

// NOTE: Avoid using MIN() and MAX(), because even conditionally defining
// those macros can cause some portability trouble, since on some systems
// the system headers insist defining their own versions.
#define my_min(x, y) ((x) < (y) ? (x) : (y))
#define my_max(x, y) ((x) > (y) ? (x) : (y))

#ifndef ARRAY_SIZE
#	define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#endif

#if defined(__GNUC__) \
		&& ((__GNUC__ == 4 && __GNUC_MINOR__ >= 3) || __GNUC__ > 4)
#	define lzma_attr_alloc_size(x) __attribute__((__alloc_size__(x)))
#else
#	define lzma_attr_alloc_size(x)
#endif

#endif

/*** End of inlined file: sysdefs.h ***/

// If any type of threading is enabled, #define MYTHREAD_ENABLED.
#if defined(MYTHREAD_POSIX) || defined(MYTHREAD_WIN95) \
		|| defined(MYTHREAD_VISTA)
#	define MYTHREAD_ENABLED 1
#endif

#ifdef MYTHREAD_ENABLED

////////////////////////////////////////
// Shared between all threading types //
////////////////////////////////////////

// Locks a mutex for a duration of a block.
//
// Perform mythread_mutex_lock(&mutex) in the beginning of a block
// and mythread_mutex_unlock(&mutex) at the end of the block. "break"
// may be used to unlock the mutex and jump out of the block.
// mythread_sync blocks may be nested.
//
// Example:
//
//     mythread_sync(mutex) {
//         foo();
//         if (some_error)
//             break; // Skips bar()
//         bar();
//     }
//
// At least GCC optimizes the loops completely away so it doesn't slow
// things down at all compared to plain mythread_mutex_lock(&mutex)
// and mythread_mutex_unlock(&mutex) calls.
//
#define mythread_sync(mutex) mythread_sync_helper1(mutex, __LINE__)
#define mythread_sync_helper1(mutex, line) mythread_sync_helper2(mutex, line)
#define mythread_sync_helper2(mutex, line) \
	for (unsigned int mythread_i_ ## line = 0; \
			mythread_i_ ## line \
				? (mythread_mutex_unlock(&(mutex)), 0) \
				: (mythread_mutex_lock(&(mutex)), 1); \
			mythread_i_ ## line = 1) \
		for (unsigned int mythread_j_ ## line = 0; \
				!mythread_j_ ## line; \
				mythread_j_ ## line = 1)
#endif

#if !defined(MYTHREAD_ENABLED)

//////////////////
// No threading //
//////////////////

// Calls the given function once. This isn't thread safe.
#define mythread_once(func) \
do { \
	static bool once_ = false; \
	if (!once_) { \
		func(); \
		once_ = true; \
	} \
} while (0)

#if !(defined(_WIN32) && !defined(__CYGWIN__))
// Use sigprocmask() to set the signal mask in single-threaded programs.
#include <signal.h>

static inline void
mythread_sigmask(int how, const sigset_t *restrict set,
		sigset_t *restrict oset)
{
	int ret = sigprocmask(how, set, oset);
	assert(ret == 0);
	(void)ret;
}
#endif

#elif defined(MYTHREAD_POSIX)

////////////////////
// Using pthreads //
////////////////////

#include <sys/time.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

#define MYTHREAD_RET_TYPE void *
#define MYTHREAD_RET_VALUE NULL

typedef pthread_t mythread;
typedef pthread_mutex_t mythread_mutex;

typedef struct {
	pthread_cond_t cond;
#ifdef HAVE_CLOCK_GETTIME
	// Clock ID (CLOCK_REALTIME or CLOCK_MONOTONIC) associated with
	// the condition variable.
	clockid_t clk_id;
#endif
} mythread_cond;

typedef struct timespec mythread_condtime;

// Calls the given function once in a thread-safe way.
#define mythread_once(func) \
	do { \
		static pthread_once_t once_ = PTHREAD_ONCE_INIT; \
		pthread_once(&once_, &func); \
	} while (0)

// Use pthread_sigmask() to set the signal mask in multi-threaded programs.
// Do nothing on OpenVMS since it lacks pthread_sigmask().
static inline void
mythread_sigmask(int how, const sigset_t *restrict set,
		sigset_t *restrict oset)
{
#ifdef __VMS
	(void)how;
	(void)set;
	(void)oset;
#else
	int ret = pthread_sigmask(how, set, oset);
	assert(ret == 0);
	(void)ret;
#endif
}

// Creates a new thread with all signals blocked. Returns zero on success
// and non-zero on error.
static inline int
mythread_create(mythread *thread, void *(*func)(void *arg), void *arg)
{
	sigset_t old;
	sigset_t all;
	sigfillset(&all);

	mythread_sigmask(SIG_SETMASK, &all, &old);
	const int ret = pthread_create(thread, NULL, func, arg);
	mythread_sigmask(SIG_SETMASK, &old, NULL);

	return ret;
}

// Joins a thread. Returns zero on success and non-zero on error.
static inline int
mythread_join(mythread thread)
{
	return pthread_join(thread, NULL);
}

// Initiatlizes a mutex. Returns zero on success and non-zero on error.
static inline int
mythread_mutex_init(mythread_mutex *mutex)
{
	return pthread_mutex_init(mutex, NULL);
}

static inline void
mythread_mutex_destroy(mythread_mutex *mutex)
{
	int ret = pthread_mutex_destroy(mutex);
	assert(ret == 0);
	(void)ret;
}

static inline void
mythread_mutex_lock(mythread_mutex *mutex)
{
	int ret = pthread_mutex_lock(mutex);
	assert(ret == 0);
	(void)ret;
}

static inline void
mythread_mutex_unlock(mythread_mutex *mutex)
{
	int ret = pthread_mutex_unlock(mutex);
	assert(ret == 0);
	(void)ret;
}

// Initializes a condition variable.
//
// Using CLOCK_MONOTONIC instead of the default CLOCK_REALTIME makes the
// timeout in pthread_cond_timedwait() work correctly also if system time
// is suddenly changed. Unfortunately CLOCK_MONOTONIC isn't available
// everywhere while the default CLOCK_REALTIME is, so the default is
// used if CLOCK_MONOTONIC isn't available.
//
// If clock_gettime() isn't available at all, gettimeofday() will be used.
static inline int
mythread_cond_init(mythread_cond *mycond)
{
#ifdef HAVE_CLOCK_GETTIME
	// NOTE: HAVE_DECL_CLOCK_MONOTONIC is always defined to 0 or 1.
#	if defined(HAVE_PTHREAD_CONDATTR_SETCLOCK) && HAVE_DECL_CLOCK_MONOTONIC
	struct timespec ts;
	pthread_condattr_t condattr;

	// POSIX doesn't seem to *require* that pthread_condattr_setclock()
	// will fail if given an unsupported clock ID. Test that
	// CLOCK_MONOTONIC really is supported using clock_gettime().
	if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0
			&& pthread_condattr_init(&condattr) == 0) {
		int ret = pthread_condattr_setclock(
				&condattr, CLOCK_MONOTONIC);
		if (ret == 0)
			ret = pthread_cond_init(&mycond->cond, &condattr);

		pthread_condattr_destroy(&condattr);

		if (ret == 0) {
			mycond->clk_id = CLOCK_MONOTONIC;
			return 0;
		}
	}

	// If anything above fails, fall back to the default CLOCK_REALTIME.
	// POSIX requires that all implementations of clock_gettime() must
	// support at least CLOCK_REALTIME.
#	endif

	mycond->clk_id = CLOCK_REALTIME;
#endif

	return pthread_cond_init(&mycond->cond, NULL);
}

static inline void
mythread_cond_destroy(mythread_cond *cond)
{
	int ret = pthread_cond_destroy(&cond->cond);
	assert(ret == 0);
	(void)ret;
}

static inline void
mythread_cond_signal(mythread_cond *cond)
{
	int ret = pthread_cond_signal(&cond->cond);
	assert(ret == 0);
	(void)ret;
}

static inline void
mythread_cond_wait(mythread_cond *cond, mythread_mutex *mutex)
{
	int ret = pthread_cond_wait(&cond->cond, mutex);
	assert(ret == 0);
	(void)ret;
}

// Waits on a condition or until a timeout expires. If the timeout expires,
// non-zero is returned, otherwise zero is returned.
static inline int
mythread_cond_timedwait(mythread_cond *cond, mythread_mutex *mutex,
		const mythread_condtime *condtime)
{
	int ret = pthread_cond_timedwait(&cond->cond, mutex, condtime);
	assert(ret == 0 || ret == ETIMEDOUT);
	return ret;
}

// Sets condtime to the absolute time that is timeout_ms milliseconds
// in the future. The type of the clock to use is taken from cond.
static inline void
mythread_condtime_set(mythread_condtime *condtime, const mythread_cond *cond,
		uint32_t timeout_ms)
{
	condtime->tv_sec = timeout_ms / 1000;
	condtime->tv_nsec = (timeout_ms % 1000) * 1000000;

#ifdef HAVE_CLOCK_GETTIME
	struct timespec now;
	int ret = clock_gettime(cond->clk_id, &now);
	assert(ret == 0);
	(void)ret;

	condtime->tv_sec += now.tv_sec;
	condtime->tv_nsec += now.tv_nsec;
#else
	(void)cond;

	struct timeval now;
	gettimeofday(&now, NULL);

	condtime->tv_sec += now.tv_sec;
	condtime->tv_nsec += now.tv_usec * 1000L;
#endif

	// tv_nsec must stay in the range [0, 999_999_999].
	if (condtime->tv_nsec >= 1000000000L) {
		condtime->tv_nsec -= 1000000000L;
		++condtime->tv_sec;
	}
}

#elif defined(MYTHREAD_WIN95) || defined(MYTHREAD_VISTA)

/////////////////////
// Windows threads //
/////////////////////

#define WIN32_LEAN_AND_MEAN
#ifdef MYTHREAD_VISTA
#	undef _WIN32_WINNT
#	define _WIN32_WINNT 0x0600
#endif
#include <windows.h>
#include <process.h>

#define MYTHREAD_RET_TYPE unsigned int __stdcall
#define MYTHREAD_RET_VALUE 0

typedef HANDLE mythread;
typedef CRITICAL_SECTION mythread_mutex;

#ifdef MYTHREAD_WIN95
typedef HANDLE mythread_cond;
#else
typedef CONDITION_VARIABLE mythread_cond;
#endif

typedef struct {
	// Tick count (milliseconds) in the beginning of the timeout.
	// NOTE: This is 32 bits so it wraps around after 49.7 days.
	// Multi-day timeouts may not work as expected.
	DWORD start;

	// Length of the timeout in milliseconds. The timeout expires
	// when the current tick count minus "start" is equal or greater
	// than "timeout".
	DWORD timeout;
} mythread_condtime;

// mythread_once() is only available with Vista threads.
#ifdef MYTHREAD_VISTA
#define mythread_once(func) \
	do { \
		static INIT_ONCE once_ = INIT_ONCE_STATIC_INIT; \
		BOOL pending_; \
		if (!InitOnceBeginInitialize(&once_, 0, &pending_, NULL)) \
			abort(); \
		if (pending_) \
			func(); \
		if (!InitOnceComplete(&once, 0, NULL)) \
			abort(); \
	} while (0)
#endif

// mythread_sigmask() isn't available on Windows. Even a dummy version would
// make no sense because the other POSIX signal functions are missing anyway.

static inline int
mythread_create(mythread *thread,
		unsigned int (__stdcall *func)(void *arg), void *arg)
{
	uintptr_t ret = _beginthreadex(NULL, 0, func, arg, 0, NULL);
	if (ret == 0)
		return -1;

	*thread = (HANDLE)ret;
	return 0;
}

static inline int
mythread_join(mythread thread)
{
	int ret = 0;

	if (WaitForSingleObject(thread, INFINITE) != WAIT_OBJECT_0)
		ret = -1;

	if (!CloseHandle(thread))
		ret = -1;

	return ret;
}

static inline int
mythread_mutex_init(mythread_mutex *mutex)
{
	InitializeCriticalSection(mutex);
	return 0;
}

static inline void
mythread_mutex_destroy(mythread_mutex *mutex)
{
	DeleteCriticalSection(mutex);
}

static inline void
mythread_mutex_lock(mythread_mutex *mutex)
{
	EnterCriticalSection(mutex);
}

static inline void
mythread_mutex_unlock(mythread_mutex *mutex)
{
	LeaveCriticalSection(mutex);
}

static inline int
mythread_cond_init(mythread_cond *cond)
{
#ifdef MYTHREAD_WIN95
	*cond = CreateEvent(NULL, FALSE, FALSE, NULL);
	return *cond == NULL ? -1 : 0;
#else
	InitializeConditionVariable(cond);
	return 0;
#endif
}

static inline void
mythread_cond_destroy(mythread_cond *cond)
{
#ifdef MYTHREAD_WIN95
	CloseHandle(*cond);
#else
	(void)cond;
#endif
}

static inline void
mythread_cond_signal(mythread_cond *cond)
{
#ifdef MYTHREAD_WIN95
	SetEvent(*cond);
#else
	WakeConditionVariable(cond);
#endif
}

static inline void
mythread_cond_wait(mythread_cond *cond, mythread_mutex *mutex)
{
#ifdef MYTHREAD_WIN95
	LeaveCriticalSection(mutex);
	WaitForSingleObject(*cond, INFINITE);
	EnterCriticalSection(mutex);
#else
	BOOL ret = SleepConditionVariableCS(cond, mutex, INFINITE);
	assert(ret);
	(void)ret;
#endif
}

static inline int
mythread_cond_timedwait(mythread_cond *cond, mythread_mutex *mutex,
		const mythread_condtime *condtime)
{
#ifdef MYTHREAD_WIN95
	LeaveCriticalSection(mutex);
#endif

	DWORD elapsed = GetTickCount() - condtime->start;
	DWORD timeout = elapsed >= condtime->timeout
			? 0 : condtime->timeout - elapsed;

#ifdef MYTHREAD_WIN95
	DWORD ret = WaitForSingleObject(*cond, timeout);
	assert(ret == WAIT_OBJECT_0 || ret == WAIT_TIMEOUT);

	EnterCriticalSection(mutex);

	return ret == WAIT_TIMEOUT;
#else
	BOOL ret = SleepConditionVariableCS(cond, mutex, timeout);
	assert(ret || GetLastError() == ERROR_TIMEOUT);
	return !ret;
#endif
}

static inline void
mythread_condtime_set(mythread_condtime *condtime, const mythread_cond *cond,
		uint32_t timeout)
{
	(void)cond;
	condtime->start = GetTickCount();
	condtime->timeout = timeout;
}

#endif

#endif

/*** End of inlined file: mythread.h ***/



/*** Start of inlined file: tuklib_integer.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       tuklib_integer.h
/// \brief      Various integer and bit operations
///
/// This file provides macros or functions to do some basic integer and bit
/// operations.
///
/// Native endian inline functions (XX = 16, 32, or 64):
///   - Unaligned native endian reads: readXXne(ptr)
///   - Unaligned native endian writes: writeXXne(ptr, num)
///   - Aligned native endian reads: aligned_readXXne(ptr)
///   - Aligned native endian writes: aligned_writeXXne(ptr, num)
///
/// Endianness-converting integer operations (these can be macros!)
/// (XX = 16, 32, or 64; Y = b or l):
///   - Byte swapping: bswapXX(num)
///   - Byte order conversions to/from native (byteswaps if Y isn't
///     the native endianness): convXXYe(num)
///   - Unaligned reads (16/32-bit only): readXXYe(ptr)
///   - Unaligned writes (16/32-bit only): writeXXYe(ptr, num)
///   - Aligned reads: aligned_readXXYe(ptr)
///   - Aligned writes: aligned_writeXXYe(ptr, num)
///
/// Since the above can macros, the arguments should have no side effects
/// because they may be evaluated more than once.
///
/// Bit scan operations for non-zero 32-bit integers (inline functions):
///   - Bit scan reverse (find highest non-zero bit): bsr32(num)
///   - Count leading zeros: clz32(num)
///   - Count trailing zeros: ctz32(num)
///   - Bit scan forward (simply an alias for ctz32()): bsf32(num)
///
/// The above bit scan operations return 0-31. If num is zero,
/// the result is undefined.
//
//  Authors:    Lasse Collin
//              Joachim Henke
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef TUKLIB_INTEGER_H
#define TUKLIB_INTEGER_H


/*** Start of inlined file: tuklib_common.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       tuklib_common.h
/// \brief      Common definitions for tuklib modules
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef TUKLIB_COMMON_H
#define TUKLIB_COMMON_H

// The config file may be replaced by a package-specific file.
// It should include at least stddef.h, inttypes.h, and limits.h.

/*** Start of inlined file: tuklib_config.h ***/
#ifdef HAVE_CONFIG_H

#else
#	include <stddef.h>
#	include <inttypes.h>
#	include <limits.h>
#endif

/*** End of inlined file: tuklib_config.h ***/


// TUKLIB_SYMBOL_PREFIX is prefixed to all symbols exported by
// the tuklib modules. If you use a tuklib module in a library,
// you should use TUKLIB_SYMBOL_PREFIX to make sure that there
// are no symbol conflicts in case someone links your library
// into application that also uses the same tuklib module.
#ifndef TUKLIB_SYMBOL_PREFIX
#	define TUKLIB_SYMBOL_PREFIX
#endif

#define TUKLIB_CAT_X(a, b) a ## b
#define TUKLIB_CAT(a, b) TUKLIB_CAT_X(a, b)

#ifndef TUKLIB_SYMBOL
#	define TUKLIB_SYMBOL(sym) TUKLIB_CAT(TUKLIB_SYMBOL_PREFIX, sym)
#endif

#ifndef TUKLIB_DECLS_BEGIN
#	ifdef __cplusplus
#		define TUKLIB_DECLS_BEGIN extern "C" {
#	else
#		define TUKLIB_DECLS_BEGIN
#	endif
#endif

#ifndef TUKLIB_DECLS_END
#	ifdef __cplusplus
#		define TUKLIB_DECLS_END }
#	else
#		define TUKLIB_DECLS_END
#	endif
#endif

#if defined(__GNUC__) && defined(__GNUC_MINOR__)
#	define TUKLIB_GNUC_REQ(major, minor) \
		((__GNUC__ == (major) && __GNUC_MINOR__ >= (minor)) \
			|| __GNUC__ > (major))
#else
#	define TUKLIB_GNUC_REQ(major, minor) 0
#endif

#if TUKLIB_GNUC_REQ(2, 5)
#	define tuklib_attr_noreturn __attribute__((__noreturn__))
#else
#	define tuklib_attr_noreturn
#endif

#if (defined(_WIN32) && !defined(__CYGWIN__)) \
		|| defined(__OS2__) || defined(__MSDOS__)
#	define TUKLIB_DOSLIKE 1
#endif

#endif

/*** End of inlined file: tuklib_common.h ***/

#include <string.h>

// Newer Intel C compilers require immintrin.h for _bit_scan_reverse()
// and such functions.
#if defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 1500)
#	include <immintrin.h>
#endif

///////////////////
// Byte swapping //
///////////////////

#if defined(HAVE___BUILTIN_BSWAPXX)
	// GCC >= 4.8 and Clang
#	define bswap16(n) __builtin_bswap16(n)
#	define bswap32(n) __builtin_bswap32(n)
#	define bswap64(n) __builtin_bswap64(n)

#elif defined(HAVE_BYTESWAP_H)
	// glibc, uClibc, dietlibc
#	include <byteswap.h>
#	ifdef HAVE_BSWAP_16
#		define bswap16(num) bswap_16(num)
#	endif
#	ifdef HAVE_BSWAP_32
#		define bswap32(num) bswap_32(num)
#	endif
#	ifdef HAVE_BSWAP_64
#		define bswap64(num) bswap_64(num)
#	endif

#elif defined(HAVE_SYS_ENDIAN_H)
	// *BSDs and Darwin
#	include <sys/endian.h>

#elif defined(HAVE_SYS_BYTEORDER_H)
	// Solaris
#	include <sys/byteorder.h>
#	ifdef BSWAP_16
#		define bswap16(num) BSWAP_16(num)
#	endif
#	ifdef BSWAP_32
#		define bswap32(num) BSWAP_32(num)
#	endif
#	ifdef BSWAP_64
#		define bswap64(num) BSWAP_64(num)
#	endif
#	ifdef BE_16
#		define conv16be(num) BE_16(num)
#	endif
#	ifdef BE_32
#		define conv32be(num) BE_32(num)
#	endif
#	ifdef BE_64
#		define conv64be(num) BE_64(num)
#	endif
#	ifdef LE_16
#		define conv16le(num) LE_16(num)
#	endif
#	ifdef LE_32
#		define conv32le(num) LE_32(num)
#	endif
#	ifdef LE_64
#		define conv64le(num) LE_64(num)
#	endif
#endif

#ifndef bswap16
#	define bswap16(n) (uint16_t)( \
		  (((n) & 0x00FFU) << 8) \
		| (((n) & 0xFF00U) >> 8) \
	)
#endif

#ifndef bswap32
#	define bswap32(n) (uint32_t)( \
		  (((n) & UINT32_C(0x000000FF)) << 24) \
		| (((n) & UINT32_C(0x0000FF00)) << 8) \
		| (((n) & UINT32_C(0x00FF0000)) >> 8) \
		| (((n) & UINT32_C(0xFF000000)) >> 24) \
	)
#endif

#ifndef bswap64
#	define bswap64(n) (uint64_t)( \
		  (((n) & UINT64_C(0x00000000000000FF)) << 56) \
		| (((n) & UINT64_C(0x000000000000FF00)) << 40) \
		| (((n) & UINT64_C(0x0000000000FF0000)) << 24) \
		| (((n) & UINT64_C(0x00000000FF000000)) << 8) \
		| (((n) & UINT64_C(0x000000FF00000000)) >> 8) \
		| (((n) & UINT64_C(0x0000FF0000000000)) >> 24) \
		| (((n) & UINT64_C(0x00FF000000000000)) >> 40) \
		| (((n) & UINT64_C(0xFF00000000000000)) >> 56) \
	)
#endif

// Define conversion macros using the basic byte swapping macros.
#ifdef WORDS_BIGENDIAN
#	ifndef conv16be
#		define conv16be(num) ((uint16_t)(num))
#	endif
#	ifndef conv32be
#		define conv32be(num) ((uint32_t)(num))
#	endif
#	ifndef conv64be
#		define conv64be(num) ((uint64_t)(num))
#	endif
#	ifndef conv16le
#		define conv16le(num) bswap16(num)
#	endif
#	ifndef conv32le
#		define conv32le(num) bswap32(num)
#	endif
#	ifndef conv64le
#		define conv64le(num) bswap64(num)
#	endif
#else
#	ifndef conv16be
#		define conv16be(num) bswap16(num)
#	endif
#	ifndef conv32be
#		define conv32be(num) bswap32(num)
#	endif
#	ifndef conv64be
#		define conv64be(num) bswap64(num)
#	endif
#	ifndef conv16le
#		define conv16le(num) ((uint16_t)(num))
#	endif
#	ifndef conv32le
#		define conv32le(num) ((uint32_t)(num))
#	endif
#	ifndef conv64le
#		define conv64le(num) ((uint64_t)(num))
#	endif
#endif

////////////////////////////////
// Unaligned reads and writes //
////////////////////////////////

// The traditional way of casting e.g. *(const uint16_t *)uint8_pointer
// is bad even if the uint8_pointer is properly aligned because this kind
// of casts break strict aliasing rules and result in undefined behavior.
// With unaligned pointers it's even worse: compilers may emit vector
// instructions that require aligned pointers even if non-vector
// instructions work with unaligned pointers.
//
// Using memcpy() is the standard compliant way to do unaligned access.
// Many modern compilers inline it so there is no function call overhead.
// For those compilers that don't handle the memcpy() method well, the
// old casting method (that violates strict aliasing) can be requested at
// build time. A third method, casting to a packed struct, would also be
// an option but isn't provided to keep things simpler (it's already a mess).
// Hopefully this is flexible enough in practice.

static inline uint16_t
read16ne(const uint8_t *buf)
{
#if defined(TUKLIB_FAST_UNALIGNED_ACCESS) \
		&& defined(TUKLIB_USE_UNSAFE_TYPE_PUNNING)
	return *(const uint16_t *)buf;
#else
	uint16_t num;
	memcpy(&num, buf, sizeof(num));
	return num;
#endif
}

static inline uint32_t
read32ne(const uint8_t *buf)
{
#if defined(TUKLIB_FAST_UNALIGNED_ACCESS) \
		&& defined(TUKLIB_USE_UNSAFE_TYPE_PUNNING)
	return *(const uint32_t *)buf;
#else
	uint32_t num;
	memcpy(&num, buf, sizeof(num));
	return num;
#endif
}

static inline uint64_t
read64ne(const uint8_t *buf)
{
#if defined(TUKLIB_FAST_UNALIGNED_ACCESS) \
		&& defined(TUKLIB_USE_UNSAFE_TYPE_PUNNING)
	return *(const uint64_t *)buf;
#else
	uint64_t num;
	memcpy(&num, buf, sizeof(num));
	return num;
#endif
}

static inline void
write16ne(uint8_t *buf, uint16_t num)
{
#if defined(TUKLIB_FAST_UNALIGNED_ACCESS) \
		&& defined(TUKLIB_USE_UNSAFE_TYPE_PUNNING)
	*(uint16_t *)buf = num;
#else
	memcpy(buf, &num, sizeof(num));
#endif
	return;
}

static inline void
write32ne(uint8_t *buf, uint32_t num)
{
#if defined(TUKLIB_FAST_UNALIGNED_ACCESS) \
		&& defined(TUKLIB_USE_UNSAFE_TYPE_PUNNING)
	*(uint32_t *)buf = num;
#else
	memcpy(buf, &num, sizeof(num));
#endif
	return;
}

static inline void
write64ne(uint8_t *buf, uint64_t num)
{
#if defined(TUKLIB_FAST_UNALIGNED_ACCESS) \
		&& defined(TUKLIB_USE_UNSAFE_TYPE_PUNNING)
	*(uint64_t *)buf = num;
#else
	memcpy(buf, &num, sizeof(num));
#endif
	return;
}

static inline uint16_t
read16be(const uint8_t *buf)
{
#if defined(WORDS_BIGENDIAN) || defined(TUKLIB_FAST_UNALIGNED_ACCESS)
	uint16_t num = read16ne(buf);
	return conv16be(num);
#else
	uint16_t num = ((uint16_t)buf[0] << 8) | (uint16_t)buf[1];
	return num;
#endif
}

static inline uint16_t
read16le(const uint8_t *buf)
{
#if !defined(WORDS_BIGENDIAN) || defined(TUKLIB_FAST_UNALIGNED_ACCESS)
	uint16_t num = read16ne(buf);
	return conv16le(num);
#else
	uint16_t num = ((uint16_t)buf[0]) | ((uint16_t)buf[1] << 8);
	return num;
#endif
}

static inline uint32_t
read32be(const uint8_t *buf)
{
#if defined(WORDS_BIGENDIAN) || defined(TUKLIB_FAST_UNALIGNED_ACCESS)
	uint32_t num = read32ne(buf);
	return conv32be(num);
#else
	uint32_t num = (uint32_t)buf[0] << 24;
	num |= (uint32_t)buf[1] << 16;
	num |= (uint32_t)buf[2] << 8;
	num |= (uint32_t)buf[3];
	return num;
#endif
}

static inline uint32_t
read32le(const uint8_t *buf)
{
#if !defined(WORDS_BIGENDIAN) || defined(TUKLIB_FAST_UNALIGNED_ACCESS)
	uint32_t num = read32ne(buf);
	return conv32le(num);
#else
	uint32_t num = (uint32_t)buf[0];
	num |= (uint32_t)buf[1] << 8;
	num |= (uint32_t)buf[2] << 16;
	num |= (uint32_t)buf[3] << 24;
	return num;
#endif
}

// NOTE: Possible byte swapping must be done in a macro to allow the compiler
// to optimize byte swapping of constants when using glibc's or *BSD's
// byte swapping macros. The actual write is done in an inline function
// to make type checking of the buf pointer possible.
#if defined(WORDS_BIGENDIAN) || defined(TUKLIB_FAST_UNALIGNED_ACCESS)
#	define write16be(buf, num) write16ne(buf, conv16be(num))
#	define write32be(buf, num) write32ne(buf, conv32be(num))
#endif

#if !defined(WORDS_BIGENDIAN) || defined(TUKLIB_FAST_UNALIGNED_ACCESS)
#	define write16le(buf, num) write16ne(buf, conv16le(num))
#	define write32le(buf, num) write32ne(buf, conv32le(num))
#endif

#ifndef write16be
static inline void
write16be(uint8_t *buf, uint16_t num)
{
	buf[0] = (uint8_t)(num >> 8);
	buf[1] = (uint8_t)num;
	return;
}
#endif

#ifndef write16le
static inline void
write16le(uint8_t *buf, uint16_t num)
{
	buf[0] = (uint8_t)num;
	buf[1] = (uint8_t)(num >> 8);
	return;
}
#endif

#ifndef write32be
static inline void
write32be(uint8_t *buf, uint32_t num)
{
	buf[0] = (uint8_t)(num >> 24);
	buf[1] = (uint8_t)(num >> 16);
	buf[2] = (uint8_t)(num >> 8);
	buf[3] = (uint8_t)num;
	return;
}
#endif

#ifndef write32le
static inline void
write32le(uint8_t *buf, uint32_t num)
{
	buf[0] = (uint8_t)num;
	buf[1] = (uint8_t)(num >> 8);
	buf[2] = (uint8_t)(num >> 16);
	buf[3] = (uint8_t)(num >> 24);
	return;
}
#endif

//////////////////////////////
// Aligned reads and writes //
//////////////////////////////

// Separate functions for aligned reads and writes are provided since on
// strict-align archs aligned access is much faster than unaligned access.
//
// Just like in the unaligned case, memcpy() is needed to avoid
// strict aliasing violations. However, on archs that don't support
// unaligned access the compiler cannot know that the pointers given
// to memcpy() are aligned which results in slow code. As of C11 there is
// no standard way to tell the compiler that we know that the address is
// aligned but some compilers have language extensions to do that. With
// such language extensions the memcpy() method gives excellent results.
//
// What to do on a strict-align system when no known language extentensions
// are available? Falling back to byte-by-byte access would be safe but ruin
// optimizations that have been made specifically with aligned access in mind.
// As a compromise, aligned reads will fall back to non-compliant type punning
// but aligned writes will be byte-by-byte, that is, fast reads are preferred
// over fast writes. This obviously isn't great but hopefully it's a working
// compromise for now.
//
// __builtin_assume_aligned is support by GCC >= 4.7 and clang >= 3.6.
#ifdef HAVE___BUILTIN_ASSUME_ALIGNED
#	define tuklib_memcpy_aligned(dest, src, size) \
		memcpy(dest, __builtin_assume_aligned(src, size), size)
#else
#	define tuklib_memcpy_aligned(dest, src, size) \
		memcpy(dest, src, size)
#	ifndef TUKLIB_FAST_UNALIGNED_ACCESS
#		define TUKLIB_USE_UNSAFE_ALIGNED_READS 1
#	endif
#endif

static inline uint16_t
aligned_read16ne(const uint8_t *buf)
{
#if defined(TUKLIB_USE_UNSAFE_TYPE_PUNNING) \
		|| defined(TUKLIB_USE_UNSAFE_ALIGNED_READS)
	return *(const uint16_t *)buf;
#else
	uint16_t num;
	tuklib_memcpy_aligned(&num, buf, sizeof(num));
	return num;
#endif
}

static inline uint32_t
aligned_read32ne(const uint8_t *buf)
{
#if defined(TUKLIB_USE_UNSAFE_TYPE_PUNNING) \
		|| defined(TUKLIB_USE_UNSAFE_ALIGNED_READS)
	return *(const uint32_t *)buf;
#else
	uint32_t num;
	tuklib_memcpy_aligned(&num, buf, sizeof(num));
	return num;
#endif
}

static inline uint64_t
aligned_read64ne(const uint8_t *buf)
{
#if defined(TUKLIB_USE_UNSAFE_TYPE_PUNNING) \
		|| defined(TUKLIB_USE_UNSAFE_ALIGNED_READS)
	return *(const uint64_t *)buf;
#else
	uint64_t num;
	tuklib_memcpy_aligned(&num, buf, sizeof(num));
	return num;
#endif
}

static inline void
aligned_write16ne(uint8_t *buf, uint16_t num)
{
#ifdef TUKLIB_USE_UNSAFE_TYPE_PUNNING
	*(uint16_t *)buf = num;
#else
	tuklib_memcpy_aligned(buf, &num, sizeof(num));
#endif
	return;
}

static inline void
aligned_write32ne(uint8_t *buf, uint32_t num)
{
#ifdef TUKLIB_USE_UNSAFE_TYPE_PUNNING
	*(uint32_t *)buf = num;
#else
	tuklib_memcpy_aligned(buf, &num, sizeof(num));
#endif
	return;
}

static inline void
aligned_write64ne(uint8_t *buf, uint64_t num)
{
#ifdef TUKLIB_USE_UNSAFE_TYPE_PUNNING
	*(uint64_t *)buf = num;
#else
	tuklib_memcpy_aligned(buf, &num, sizeof(num));
#endif
	return;
}

static inline uint16_t
aligned_read16be(const uint8_t *buf)
{
	uint16_t num = aligned_read16ne(buf);
	return conv16be(num);
}

static inline uint16_t
aligned_read16le(const uint8_t *buf)
{
	uint16_t num = aligned_read16ne(buf);
	return conv16le(num);
}

static inline uint32_t
aligned_read32be(const uint8_t *buf)
{
	uint32_t num = aligned_read32ne(buf);
	return conv32be(num);
}

static inline uint32_t
aligned_read32le(const uint8_t *buf)
{
	uint32_t num = aligned_read32ne(buf);
	return conv32le(num);
}

static inline uint64_t
aligned_read64be(const uint8_t *buf)
{
	uint64_t num = aligned_read64ne(buf);
	return conv64be(num);
}

static inline uint64_t
aligned_read64le(const uint8_t *buf)
{
	uint64_t num = aligned_read64ne(buf);
	return conv64le(num);
}

// These need to be macros like in the unaligned case.
#define aligned_write16be(buf, num) aligned_write16ne((buf), conv16be(num))
#define aligned_write16le(buf, num) aligned_write16ne((buf), conv16le(num))
#define aligned_write32be(buf, num) aligned_write32ne((buf), conv32be(num))
#define aligned_write32le(buf, num) aligned_write32ne((buf), conv32le(num))
#define aligned_write64be(buf, num) aligned_write64ne((buf), conv64be(num))
#define aligned_write64le(buf, num) aligned_write64ne((buf), conv64le(num))

////////////////////
// Bit operations //
////////////////////

static inline uint32_t
bsr32(uint32_t n)
{
	// Check for ICC first, since it tends to define __GNUC__ too.
#if defined(__INTEL_COMPILER)
	return _bit_scan_reverse(n);

#elif TUKLIB_GNUC_REQ(3, 4) && UINT_MAX == UINT32_MAX
	// GCC >= 3.4 has __builtin_clz(), which gives good results on
	// multiple architectures. On x86, __builtin_clz() ^ 31U becomes
	// either plain BSR (so the XOR gets optimized away) or LZCNT and
	// XOR (if -march indicates that SSE4a instructions are supported).
	return (uint32_t)__builtin_clz(n) ^ 31U;

#elif defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
	uint32_t i;
	__asm__("bsrl %1, %0" : "=r" (i) : "rm" (n));
	return i;

#elif defined(_MSC_VER)
	unsigned long i;
	_BitScanReverse(&i, n);
	return i;

#else
	uint32_t i = 31;

	if ((n & 0xFFFF0000) == 0) {
		n <<= 16;
		i = 15;
	}

	if ((n & 0xFF000000) == 0) {
		n <<= 8;
		i -= 8;
	}

	if ((n & 0xF0000000) == 0) {
		n <<= 4;
		i -= 4;
	}

	if ((n & 0xC0000000) == 0) {
		n <<= 2;
		i -= 2;
	}

	if ((n & 0x80000000) == 0)
		--i;

	return i;
#endif
}

static inline uint32_t
clz32(uint32_t n)
{
#if defined(__INTEL_COMPILER)
	return _bit_scan_reverse(n) ^ 31U;

#elif TUKLIB_GNUC_REQ(3, 4) && UINT_MAX == UINT32_MAX
	return (uint32_t)__builtin_clz(n);

#elif defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
	uint32_t i;
	__asm__("bsrl %1, %0\n\t"
		"xorl $31, %0"
		: "=r" (i) : "rm" (n));
	return i;

#elif defined(_MSC_VER)
	unsigned long i;
	_BitScanReverse(&i, n);
	return i ^ 31U;

#else
	uint32_t i = 0;

	if ((n & 0xFFFF0000) == 0) {
		n <<= 16;
		i = 16;
	}

	if ((n & 0xFF000000) == 0) {
		n <<= 8;
		i += 8;
	}

	if ((n & 0xF0000000) == 0) {
		n <<= 4;
		i += 4;
	}

	if ((n & 0xC0000000) == 0) {
		n <<= 2;
		i += 2;
	}

	if ((n & 0x80000000) == 0)
		++i;

	return i;
#endif
}

static inline uint32_t
ctz32(uint32_t n)
{
#if defined(__INTEL_COMPILER)
	return _bit_scan_forward(n);

#elif TUKLIB_GNUC_REQ(3, 4) && UINT_MAX >= UINT32_MAX
	return (uint32_t)__builtin_ctz(n);

#elif defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
	uint32_t i;
	__asm__("bsfl %1, %0" : "=r" (i) : "rm" (n));
	return i;

#elif defined(_MSC_VER)
	unsigned long i;
	_BitScanForward(&i, n);
	return i;

#else
	uint32_t i = 0;

	if ((n & 0x0000FFFF) == 0) {
		n >>= 16;
		i = 16;
	}

	if ((n & 0x000000FF) == 0) {
		n >>= 8;
		i += 8;
	}

	if ((n & 0x0000000F) == 0) {
		n >>= 4;
		i += 4;
	}

	if ((n & 0x00000003) == 0) {
		n >>= 2;
		i += 2;
	}

	if ((n & 0x00000001) == 0)
		++i;

	return i;
#endif
}

#define bsf32 ctz32

#endif

/*** End of inlined file: tuklib_integer.h ***/

#if defined(_WIN32) || defined(__CYGWIN__)
#	ifdef DLL_EXPORT
#		define LZMA_API_EXPORT __declspec(dllexport)
#	else
#		define LZMA_API_EXPORT
#	endif
// Don't use ifdef or defined() below.
#elif HAVE_VISIBILITY
#	define LZMA_API_EXPORT __attribute__((__visibility__("default")))
#else
#	define LZMA_API_EXPORT
#endif

#define LZMA_API(type) LZMA_API_EXPORT type LZMA_API_CALL

#include "lzma.h"

// These allow helping the compiler in some often-executed branches, whose
// result is almost always the same.
#ifdef __GNUC__
#	define likely(expr) __builtin_expect(expr, true)
#	define unlikely(expr) __builtin_expect(expr, false)
#else
#	define likely(expr) (expr)
#	define unlikely(expr) (expr)
#endif

/// Size of temporary buffers needed in some filters
#define LZMA_BUFFER_SIZE 4096

/// Maximum number of worker threads within one multithreaded component.
/// The limit exists solely to make it simpler to prevent integer overflows
/// when allocating structures etc. This should be big enough for now...
/// the code won't scale anywhere close to this number anyway.
#define LZMA_THREADS_MAX 16384

/// Starting value for memory usage estimates. Instead of calculating size
/// of _every_ structure and taking into account malloc() overhead etc., we
/// add a base size to all memory usage estimates. It's not very accurate
/// but should be easily good enough.
#define LZMA_MEMUSAGE_BASE (UINT64_C(1) << 15)

/// Start of internal Filter ID space. These IDs must never be used
/// in Streams.
#define LZMA_FILTER_RESERVED_START (LZMA_VLI_C(1) << 62)

/// Supported flags that can be passed to lzma_stream_decoder()
/// or lzma_auto_decoder().
#define LZMA_SUPPORTED_FLAGS \
	( LZMA_TELL_NO_CHECK \
	| LZMA_TELL_UNSUPPORTED_CHECK \
	| LZMA_TELL_ANY_CHECK \
	| LZMA_IGNORE_CHECK \
	| LZMA_CONCATENATED )

/// Largest valid lzma_action value as unsigned integer.
#define LZMA_ACTION_MAX ((unsigned int)(LZMA_FULL_BARRIER))

/// Special return value (lzma_ret) to indicate that a timeout was reached
/// and lzma_code() must not return LZMA_BUF_ERROR. This is converted to
/// LZMA_OK in lzma_code(). This is not in the lzma_ret enumeration because
/// there's no need to have it in the public API.
#define LZMA_TIMED_OUT 32

typedef struct lzma_next_coder_s lzma_next_coder;

typedef struct lzma_filter_info_s lzma_filter_info;

/// Type of a function used to initialize a filter encoder or decoder
typedef lzma_ret (*lzma_init_function)(
		lzma_next_coder *next, const lzma_allocator *allocator,
		const lzma_filter_info *filters);

/// Type of a function to do some kind of coding work (filters, Stream,
/// Block encoders/decoders etc.). Some special coders use don't use both
/// input and output buffers, but for simplicity they still use this same
/// function prototype.
typedef lzma_ret (*lzma_code_function)(
		void *coder, const lzma_allocator *allocator,
		const uint8_t *restrict in, size_t *restrict in_pos,
		size_t in_size, uint8_t *restrict out,
		size_t *restrict out_pos, size_t out_size,
		lzma_action action);

/// Type of a function to free the memory allocated for the coder
typedef void (*lzma_end_function)(
		void *coder, const lzma_allocator *allocator);

/// Raw coder validates and converts an array of lzma_filter structures to
/// an array of lzma_filter_info structures. This array is used with
/// lzma_next_filter_init to initialize the filter chain.
struct lzma_filter_info_s {
	/// Filter ID. This is used only by the encoder
	/// with lzma_filters_update().
	lzma_vli id;

	/// Pointer to function used to initialize the filter.
	/// This is NULL to indicate end of array.
	lzma_init_function init;

	/// Pointer to filter's options structure
	void *options;
};

/// Hold data and function pointers of the next filter in the chain.
struct lzma_next_coder_s {
	/// Pointer to coder-specific data
	void *coder;

	/// Filter ID. This is LZMA_VLI_UNKNOWN when this structure doesn't
	/// point to a filter coder.
	lzma_vli id;

	/// "Pointer" to init function. This is never called here.
	/// We need only to detect if we are initializing a coder
	/// that was allocated earlier. See lzma_next_coder_init and
	/// lzma_next_strm_init macros in this file.
	uintptr_t init;

	/// Pointer to function to do the actual coding
	lzma_code_function code;

	/// Pointer to function to free lzma_next_coder.coder. This can
	/// be NULL; in that case, lzma_free is called to free
	/// lzma_next_coder.coder.
	lzma_end_function end;

	/// Pointer to a function to get progress information. If this is NULL,
	/// lzma_stream.total_in and .total_out are used instead.
	void (*get_progress)(void *coder,
			uint64_t *progress_in, uint64_t *progress_out);

	/// Pointer to function to return the type of the integrity check.
	/// Most coders won't support this.
	lzma_check (*get_check)(const void *coder);

	/// Pointer to function to get and/or change the memory usage limit.
	/// If new_memlimit == 0, the limit is not changed.
	lzma_ret (*memconfig)(void *coder, uint64_t *memusage,
			uint64_t *old_memlimit, uint64_t new_memlimit);

	/// Update the filter-specific options or the whole filter chain
	/// in the encoder.
	lzma_ret (*update)(void *coder, const lzma_allocator *allocator,
			const lzma_filter *filters,
			const lzma_filter *reversed_filters);
};

/// Macro to initialize lzma_next_coder structure
#define LZMA_NEXT_CODER_INIT \
	(lzma_next_coder){ \
		.coder = NULL, \
		.init = (uintptr_t)(NULL), \
		.id = LZMA_VLI_UNKNOWN, \
		.code = NULL, \
		.end = NULL, \
		.get_progress = NULL, \
		.get_check = NULL, \
		.memconfig = NULL, \
		.update = NULL, \
	}

/// Internal data for lzma_strm_init, lzma_code, and lzma_end. A pointer to
/// this is stored in lzma_stream.
struct lzma_internal_s {
	/// The actual coder that should do something useful
	lzma_next_coder next;

	/// Track the state of the coder. This is used to validate arguments
	/// so that the actual coders can rely on e.g. that LZMA_SYNC_FLUSH
	/// is used on every call to lzma_code until next.code has returned
	/// LZMA_STREAM_END.
	enum {
		ISEQ_RUN,
		ISEQ_SYNC_FLUSH,
		ISEQ_FULL_FLUSH,
		ISEQ_FINISH,
		ISEQ_FULL_BARRIER,
		ISEQ_END,
		ISEQ_ERROR,
	} sequence;

	/// A copy of lzma_stream avail_in. This is used to verify that the
	/// amount of input doesn't change once e.g. LZMA_FINISH has been
	/// used.
	size_t avail_in;

	/// Indicates which lzma_action values are allowed by next.code.
	bool supported_actions[LZMA_ACTION_MAX + 1];

	/// If true, lzma_code will return LZMA_BUF_ERROR if no progress was
	/// made (no input consumed and no output produced by next.code).
	bool allow_buf_error;
};

/// Allocates memory
extern void *lzma_alloc(size_t size, const lzma_allocator *allocator)
		lzma_attribute((__malloc__)) lzma_attr_alloc_size(1);

/// Allocates memory and zeroes it (like calloc()). This can be faster
/// than lzma_alloc() + memzero() while being backward compatible with
/// custom allocators.
extern void * lzma_attribute((__malloc__)) lzma_attr_alloc_size(1)
		lzma_alloc_zero(size_t size, const lzma_allocator *allocator);

/// Frees memory
extern void lzma_free(void *ptr, const lzma_allocator *allocator);

/// Allocates strm->internal if it is NULL, and initializes *strm and
/// strm->internal. This function is only called via lzma_next_strm_init macro.
extern lzma_ret lzma_strm_init(lzma_stream *strm);

/// Initializes the next filter in the chain, if any. This takes care of
/// freeing the memory of previously initialized filter if it is different
/// than the filter being initialized now. This way the actual filter
/// initialization functions don't need to use lzma_next_coder_init macro.
extern lzma_ret lzma_next_filter_init(lzma_next_coder *next,
		const lzma_allocator *allocator,
		const lzma_filter_info *filters);

/// Update the next filter in the chain, if any. This checks that
/// the application is not trying to change the Filter IDs.
extern lzma_ret lzma_next_filter_update(
		lzma_next_coder *next, const lzma_allocator *allocator,
		const lzma_filter *reversed_filters);

/// Frees the memory allocated for next->coder either using next->end or,
/// if next->end is NULL, using lzma_free.
extern void lzma_next_end(lzma_next_coder *next,
		const lzma_allocator *allocator);

/// Copy as much data as possible from in[] to out[] and update *in_pos
/// and *out_pos accordingly. Returns the number of bytes copied.
extern size_t lzma_bufcpy(const uint8_t *restrict in, size_t *restrict in_pos,
		size_t in_size, uint8_t *restrict out,
		size_t *restrict out_pos, size_t out_size);

/// \brief      Return if expression doesn't evaluate to LZMA_OK
///
/// There are several situations where we want to return immediately
/// with the value of expr if it isn't LZMA_OK. This macro shortens
/// the code a little.
#define return_if_error(expr) \
do { \
	const lzma_ret ret_ = (expr); \
	if (ret_ != LZMA_OK) \
		return ret_; \
} while (0)

/// If next isn't already initialized, free the previous coder. Then mark
/// that next is _possibly_ initialized for the coder using this macro.
/// "Possibly" means that if e.g. allocation of next->coder fails, the
/// structure isn't actually initialized for this coder, but leaving
/// next->init to func is still OK.
#define lzma_next_coder_init(func, next, allocator) \
do { \
	if ((uintptr_t)(func) != (next)->init) \
		lzma_next_end(next, allocator); \
	(next)->init = (uintptr_t)(func); \
} while (0)

/// Initializes lzma_strm and calls func() to initialize strm->internal->next.
/// (The function being called will use lzma_next_coder_init()). If
/// initialization fails, memory that wasn't freed by func() is freed
/// along strm->internal.
#define lzma_next_strm_init(func, strm, ...) \
do { \
	return_if_error(lzma_strm_init(strm)); \
	const lzma_ret ret_ = func(&(strm)->internal->next, \
			(strm)->allocator, __VA_ARGS__); \
	if (ret_ != LZMA_OK) { \
		lzma_end(strm); \
		return ret_; \
	} \
} while (0)

#endif

/*** End of inlined file: common.h ***/

extern lzma_ret lzma_alone_decoder_init(
		lzma_next_coder *next, const lzma_allocator *allocator,
		uint64_t memlimit, bool picky);

#endif

/*** End of inlined file: alone_decoder.h ***/



/*** Start of inlined file: block_buffer_encoder.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       block_buffer_encoder.h
/// \brief      Single-call .xz Block encoder
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LZMA_BLOCK_BUFFER_ENCODER_H
#define LZMA_BLOCK_BUFFER_ENCODER_H

/// uint64_t version of lzma_block_buffer_bound(). It is used by
/// stream_encoder_mt.c. Probably the original lzma_block_buffer_bound()
/// should have been 64-bit, but fixing it would break the ABI.
extern uint64_t lzma_block_buffer_bound64(uint64_t uncompressed_size);

#endif

/*** End of inlined file: block_buffer_encoder.h ***/


/*** Start of inlined file: block_decoder.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       block_decoder.h
/// \brief      Decodes .xz Blocks
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LZMA_BLOCK_DECODER_H
#define LZMA_BLOCK_DECODER_H

extern lzma_ret lzma_block_decoder_init(lzma_next_coder *next,
		const lzma_allocator *allocator, lzma_block *block);

#endif

/*** End of inlined file: block_decoder.h ***/


/*** Start of inlined file: block_encoder.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       block_encoder.h
/// \brief      Encodes .xz Blocks
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LZMA_BLOCK_ENCODER_H
#define LZMA_BLOCK_ENCODER_H

/// \brief      Biggest Compressed Size value that the Block encoder supports
///
/// The maximum size of a single Block is limited by the maximum size of
/// a Stream, which in theory is 2^63 - 3 bytes (i.e. LZMA_VLI_MAX - 3).
/// While the size is really big and no one should hit it in practice, we
/// take it into account in some places anyway to catch some errors e.g. if
/// application passes insanely big value to some function.
///
/// We could take into account the headers etc. to determine the exact
/// maximum size of the Compressed Data field, but the complexity would give
/// us nothing useful. Instead, limit the size of Compressed Data so that
/// even with biggest possible Block Header and Check fields the total
/// encoded size of the Block stays as a valid VLI. This doesn't guarantee
/// that the size of the Stream doesn't grow too big, but that problem is
/// taken care outside the Block handling code.
///
/// ~LZMA_VLI_C(3) is to guarantee that if we need padding at the end of
/// the Compressed Data field, it will still stay in the proper limit.
///
/// This constant is in this file because it is needed in both
/// block_encoder.c and block_buffer_encoder.c.
#define COMPRESSED_SIZE_MAX ((LZMA_VLI_MAX - LZMA_BLOCK_HEADER_SIZE_MAX \
		- LZMA_CHECK_SIZE_MAX) & ~LZMA_VLI_C(3))

extern lzma_ret lzma_block_encoder_init(lzma_next_coder *next,
		const lzma_allocator *allocator, lzma_block *block);

#endif

/*** End of inlined file: block_encoder.h ***/


/*** Start of inlined file: easy_preset.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       easy_preset.h
/// \brief      Preset handling for easy encoder and decoder
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

typedef struct {
	/// We need to keep the filters array available in case
	/// LZMA_FULL_FLUSH is used.
	lzma_filter filters[LZMA_FILTERS_MAX + 1];

	/// Options for LZMA2
	lzma_options_lzma opt_lzma;

	// Options for more filters can be added later, so this struct
	// is not ready to be put into the public API.

} lzma_options_easy;

/// Set *easy to the settings given by the preset. Returns true on error,
/// false on success.
extern bool lzma_easy_preset(lzma_options_easy *easy, uint32_t preset);

/*** End of inlined file: easy_preset.h ***/


/*** Start of inlined file: filter_common.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       filter_common.h
/// \brief      Filter-specific stuff common for both encoder and decoder
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LZMA_FILTER_COMMON_H
#define LZMA_FILTER_COMMON_H

/// Both lzma_filter_encoder and lzma_filter_decoder begin with these members.
typedef struct {
	/// Filter ID
	lzma_vli id;

	/// Initializes the filter encoder and calls lzma_next_filter_init()
	/// for filters + 1.
	lzma_init_function init;

	/// Calculates memory usage of the encoder. If the options are
	/// invalid, UINT64_MAX is returned.
	uint64_t (*memusage)(const void *options);

} lzma_filter_coder;

typedef const lzma_filter_coder *(*lzma_filter_find)(lzma_vli id);

extern lzma_ret lzma_raw_coder_init(
		lzma_next_coder *next, const lzma_allocator *allocator,
		const lzma_filter *filters,
		lzma_filter_find coder_find, bool is_encoder);

extern uint64_t lzma_raw_coder_memusage(lzma_filter_find coder_find,
		const lzma_filter *filters);

#endif

/*** End of inlined file: filter_common.h ***/


/*** Start of inlined file: filter_decoder.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       filter_decoder.h
/// \brief      Filter ID mapping to filter-specific functions
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LZMA_FILTER_DECODER_H
#define LZMA_FILTER_DECODER_H

extern lzma_ret lzma_raw_decoder_init(
		lzma_next_coder *next, const lzma_allocator *allocator,
		const lzma_filter *options);

#endif

/*** End of inlined file: filter_decoder.h ***/


/*** Start of inlined file: filter_encoder.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       filter_encoder.c
/// \brief      Filter ID mapping to filter-specific functions
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LZMA_FILTER_ENCODER_H
#define LZMA_FILTER_ENCODER_H

// FIXME: Might become a part of the public API.
extern uint64_t lzma_mt_block_size(const lzma_filter *filters);

extern lzma_ret lzma_raw_encoder_init(
		lzma_next_coder *next, const lzma_allocator *allocator,
		const lzma_filter *filters);

#endif

/*** End of inlined file: filter_encoder.h ***/


/*** Start of inlined file: index_encoder.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       index_encoder.h
/// \brief      Encodes the Index field
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LZMA_INDEX_ENCODER_H
#define LZMA_INDEX_ENCODER_H

extern lzma_ret lzma_index_encoder_init(lzma_next_coder *next,
		const lzma_allocator *allocator, const lzma_index *i);

#endif

/*** End of inlined file: index_encoder.h ***/


/*** Start of inlined file: index.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       index.h
/// \brief      Handling of Index
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LZMA_INDEX_H
#define LZMA_INDEX_H

/// Minimum Unpadded Size
#define UNPADDED_SIZE_MIN LZMA_VLI_C(5)

/// Maximum Unpadded Size
#define UNPADDED_SIZE_MAX (LZMA_VLI_MAX & ~LZMA_VLI_C(3))

/// Get the size of the Index Padding field. This is needed by Index encoder
/// and decoder, but applications should have no use for this.
extern uint32_t lzma_index_padding_size(const lzma_index *i);

/// Set for how many Records to allocate memory the next time
/// lzma_index_append() needs to allocate space for a new Record.
/// This is used only by the Index decoder.
extern void lzma_index_prealloc(lzma_index *i, lzma_vli records);

/// Round the variable-length integer to the next multiple of four.
static inline lzma_vli
vli_ceil4(lzma_vli vli)
{
	assert(vli <= LZMA_VLI_MAX);
	return (vli + 3) & ~LZMA_VLI_C(3);
}

/// Calculate the size of the Index field excluding Index Padding
static inline lzma_vli
index_size_unpadded(lzma_vli count, lzma_vli index_list_size)
{
	// Index Indicator + Number of Records + List of Records + CRC32
	return 1 + lzma_vli_size(count) + index_list_size + 4;
}

/// Calculate the size of the Index field including Index Padding
static inline lzma_vli
index_size(lzma_vli count, lzma_vli index_list_size)
{
	return vli_ceil4(index_size_unpadded(count, index_list_size));
}

/// Calculate the total size of the Stream
static inline lzma_vli
index_stream_size(lzma_vli blocks_size,
		lzma_vli count, lzma_vli index_list_size)
{
	return LZMA_STREAM_HEADER_SIZE + blocks_size
			+ index_size(count, index_list_size)
			+ LZMA_STREAM_HEADER_SIZE;
}

#endif

/*** End of inlined file: index.h ***/


/*** Start of inlined file: memcmplen.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       memcmplen.h
/// \brief      Optimized comparison of two buffers
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LZMA_MEMCMPLEN_H
#define LZMA_MEMCMPLEN_H

#ifdef HAVE_IMMINTRIN_H
#	include <immintrin.h>
#endif

/// Find out how many equal bytes the two buffers have.
///
/// \param      buf1    First buffer
/// \param      buf2    Second buffer
/// \param      len     How many bytes have already been compared and will
///                     be assumed to match
/// \param      limit   How many bytes to compare at most, including the
///                     already-compared bytes. This must be significantly
///                     smaller than UINT32_MAX to avoid integer overflows.
///                     Up to LZMA_MEMCMPLEN_EXTRA bytes may be read past
///                     the specified limit from both buf1 and buf2.
///
/// \return     Number of equal bytes in the buffers is returned.
///             This is always at least len and at most limit.
///
/// \note       LZMA_MEMCMPLEN_EXTRA defines how many extra bytes may be read.
///             It's rounded up to 2^n. This extra amount needs to be
///             allocated in the buffers being used. It needs to be
///             initialized too to keep Valgrind quiet.
static inline uint32_t lzma_attribute((__always_inline__))
lzma_memcmplen(const uint8_t *buf1, const uint8_t *buf2,
		uint32_t len, uint32_t limit)
{
	assert(len <= limit);
	assert(limit <= UINT32_MAX / 2);

#if defined(TUKLIB_FAST_UNALIGNED_ACCESS) \
		&& ((TUKLIB_GNUC_REQ(3, 4) && defined(__x86_64__)) \
			|| (defined(__INTEL_COMPILER) && defined(__x86_64__)) \
			|| (defined(__INTEL_COMPILER) && defined(_M_X64)) \
			|| (defined(_MSC_VER) && defined(_M_X64)))
	// NOTE: This will use 64-bit unaligned access which
	// TUKLIB_FAST_UNALIGNED_ACCESS wasn't meant to permit, but
	// it's convenient here at least as long as it's x86-64 only.
	//
	// I keep this x86-64 only for now since that's where I know this
	// to be a good method. This may be fine on other 64-bit CPUs too.
	// On big endian one should use xor instead of subtraction and switch
	// to __builtin_clzll().
#define LZMA_MEMCMPLEN_EXTRA 8
	while (len < limit) {
		const uint64_t x = read64ne(buf1 + len) - read64ne(buf2 + len);
		if (x != 0) {
#	if defined(_M_X64) // MSVC or Intel C compiler on Windows
			unsigned long tmp;
			_BitScanForward64(&tmp, x);
			len += (uint32_t)tmp >> 3;
#	else // GCC, clang, or Intel C compiler
			len += (uint32_t)__builtin_ctzll(x) >> 3;
#	endif
			return my_min(len, limit);
		}

		len += 8;
	}

	return limit;

#elif defined(TUKLIB_FAST_UNALIGNED_ACCESS) \
		&& defined(HAVE__MM_MOVEMASK_EPI8) \
		&& ((defined(__GNUC__) && defined(__SSE2_MATH__)) \
			|| (defined(__INTEL_COMPILER) && defined(__SSE2__)) \
			|| (defined(_MSC_VER) && defined(_M_IX86_FP) \
				&& _M_IX86_FP >= 2))
	// NOTE: Like above, this will use 128-bit unaligned access which
	// TUKLIB_FAST_UNALIGNED_ACCESS wasn't meant to permit.
	//
	// SSE2 version for 32-bit and 64-bit x86. On x86-64 the above
	// version is sometimes significantly faster and sometimes
	// slightly slower than this SSE2 version, so this SSE2
	// version isn't used on x86-64.
#	define LZMA_MEMCMPLEN_EXTRA 16
	while (len < limit) {
		const uint32_t x = 0xFFFF ^ _mm_movemask_epi8(_mm_cmpeq_epi8(
			_mm_loadu_si128((const __m128i *)(buf1 + len)),
			_mm_loadu_si128((const __m128i *)(buf2 + len))));

		if (x != 0) {
			len += ctz32(x);
			return my_min(len, limit);
		}

		len += 16;
	}

	return limit;

#elif defined(TUKLIB_FAST_UNALIGNED_ACCESS) && !defined(WORDS_BIGENDIAN)
	// Generic 32-bit little endian method
#	define LZMA_MEMCMPLEN_EXTRA 4
	while (len < limit) {
		uint32_t x = read32ne(buf1 + len) - read32ne(buf2 + len);
		if (x != 0) {
			if ((x & 0xFFFF) == 0) {
				len += 2;
				x >>= 16;
			}

			if ((x & 0xFF) == 0)
				++len;

			return my_min(len, limit);
		}

		len += 4;
	}

	return limit;

#elif defined(TUKLIB_FAST_UNALIGNED_ACCESS) && defined(WORDS_BIGENDIAN)
	// Generic 32-bit big endian method
#	define LZMA_MEMCMPLEN_EXTRA 4
	while (len < limit) {
		uint32_t x = read32ne(buf1 + len) ^ read32ne(buf2 + len);
		if (x != 0) {
			if ((x & 0xFFFF0000) == 0) {
				len += 2;
				x <<= 16;
			}

			if ((x & 0xFF000000) == 0)
				++len;

			return my_min(len, limit);
		}

		len += 4;
	}

	return limit;

#else
	// Simple portable version that doesn't use unaligned access.
#	define LZMA_MEMCMPLEN_EXTRA 0
	while (len < limit && buf1[len] == buf2[len])
		++len;

	return len;
#endif
}

#endif

/*** End of inlined file: memcmplen.h ***/


/*** Start of inlined file: outqueue.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       outqueue.h
/// \brief      Output queue handling in multithreaded coding
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

/// Output buffer for a single thread
typedef struct {
	/// Pointer to the output buffer of lzma_outq.buf_size_max bytes
	uint8_t *buf;

	/// Amount of data written to buf
	size_t size;

	/// Additional size information
	lzma_vli unpadded_size;
	lzma_vli uncompressed_size;

	/// True when no more data will be written into this buffer.
	///
	/// \note       This is read by another thread and thus access
	///             to this variable needs a mutex.
	bool finished;

} lzma_outbuf;

typedef struct {
	/// Array of buffers that are used cyclically.
	lzma_outbuf *bufs;

	/// Memory allocated for all the buffers
	uint8_t *bufs_mem;

	/// Amount of buffer space available in each buffer
	size_t buf_size_max;

	/// Number of buffers allocated
	uint32_t bufs_allocated;

	/// Position in the bufs array. The next buffer to be taken
	/// into use is bufs[bufs_pos].
	uint32_t bufs_pos;

	/// Number of buffers in use
	uint32_t bufs_used;

	/// Position in the buffer in lzma_outq_read()
	size_t read_pos;

} lzma_outq;

/**
 * \brief       Calculate the memory usage of an output queue
 *
 * \return      Approximate memory usage in bytes or UINT64_MAX on error.
 */
extern uint64_t lzma_outq_memusage(uint64_t buf_size_max, uint32_t threads);

/// \brief      Initialize an output queue
///
/// \param      outq            Pointer to an output queue. Before calling
///                             this function the first time, *outq should
///                             have been zeroed with memzero() so that this
///                             function knows that there are no previous
///                             allocations to free.
/// \param      allocator       Pointer to allocator or NULL
/// \param      buf_size_max    Maximum amount of data that a single buffer
///                             in the queue may need to store.
/// \param      threads         Number of buffers that may be in use
///                             concurrently. Note that more than this number
///                             of buffers will actually get allocated to
///                             improve performance when buffers finish
///                             out of order.
///
/// \return     - LZMA_OK
///             - LZMA_MEM_ERROR
///
extern lzma_ret lzma_outq_init(
		lzma_outq *outq, const lzma_allocator *allocator,
		uint64_t buf_size_max, uint32_t threads);

/// \brief      Free the memory associated with the output queue
extern void lzma_outq_end(lzma_outq *outq, const lzma_allocator *allocator);

/// \brief      Get a new buffer
///
/// lzma_outq_has_buf() must be used to check that there is a buffer
/// available before calling lzma_outq_get_buf().
///
extern lzma_outbuf *lzma_outq_get_buf(lzma_outq *outq);

/// \brief      Test if there is data ready to be read
///
/// Call to this function must be protected with the same mutex that
/// is used to protect lzma_outbuf.finished.
///
extern bool lzma_outq_is_readable(const lzma_outq *outq);

/// \brief      Read finished data
///
/// \param      outq            Pointer to an output queue
/// \param      out             Beginning of the output buffer
/// \param      out_pos         The next byte will be written to
///                             out[*out_pos].
/// \param      out_size        Size of the out buffer; the first byte into
///                             which no data is written to is out[out_size].
/// \param      unpadded_size   Unpadded Size from the Block encoder
/// \param      uncompressed_size Uncompressed Size from the Block encoder
///
/// \return     - LZMA: All OK. Either no data was available or the buffer
///               being read didn't become empty yet.
///             - LZMA_STREAM_END: The buffer being read was finished.
///               *unpadded_size and *uncompressed_size were set.
///
/// \note       This reads lzma_outbuf.finished variables and thus call
///             to this function needs to be protected with a mutex.
///
extern lzma_ret lzma_outq_read(lzma_outq *restrict outq,
		uint8_t *restrict out, size_t *restrict out_pos,
		size_t out_size, lzma_vli *restrict unpadded_size,
		lzma_vli *restrict uncompressed_size);

/// \brief      Test if there is at least one buffer free
///
/// This must be used before getting a new buffer with lzma_outq_get_buf().
///
static inline bool
lzma_outq_has_buf(const lzma_outq *outq)
{
	return outq->bufs_used < outq->bufs_allocated;
}

/// \brief      Test if the queue is completely empty
static inline bool
lzma_outq_is_empty(const lzma_outq *outq)
{
	return outq->bufs_used == 0;
}

/*** End of inlined file: outqueue.h ***/


/*** Start of inlined file: stream_decoder.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       stream_decoder.h
/// \brief      Decodes .xz Streams
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LZMA_STREAM_DECODER_H
#define LZMA_STREAM_DECODER_H

extern lzma_ret lzma_stream_decoder_init(
		lzma_next_coder *next, const lzma_allocator *allocator,
		uint64_t memlimit, uint32_t flags);

#endif

/*** End of inlined file: stream_decoder.h ***/


/*** Start of inlined file: stream_flags_common.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       stream_flags_common.h
/// \brief      Common stuff for Stream flags coders
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LZMA_STREAM_FLAGS_COMMON_H
#define LZMA_STREAM_FLAGS_COMMON_H

/// Size of the Stream Flags field
#define LZMA_STREAM_FLAGS_SIZE 2

extern const uint8_t lzma_header_magic[6];
extern const uint8_t lzma_footer_magic[2];

static inline bool
is_backward_size_valid(const lzma_stream_flags *options)
{
	return options->backward_size >= LZMA_BACKWARD_SIZE_MIN
			&& options->backward_size <= LZMA_BACKWARD_SIZE_MAX
			&& (options->backward_size & 3) == 0;
}

#endif

/*** End of inlined file: stream_flags_common.h ***/

		//C

/*** Start of inlined file: alone_decoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       alone_decoder.c
/// \brief      Decoder for LZMA_Alone files
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "lzma_decoder.h"
#include "lz_decoder.h"

typedef struct {
	lzma_next_coder next;

	enum {
		SEQ_PROPERTIES,
		SEQ_DICTIONARY_SIZE,
		SEQ_UNCOMPRESSED_SIZE,
		SEQ_CODER_INIT,
		SEQ_CODE,
	} sequence;

	/// If true, reject files that are unlikely to be .lzma files.
	/// If false, more non-.lzma files get accepted and will give
	/// LZMA_DATA_ERROR either immediately or after a few output bytes.
	bool picky;

	/// Position in the header fields
	size_t pos;

	/// Uncompressed size decoded from the header
	lzma_vli uncompressed_size;

	/// Memory usage limit
	uint64_t memlimit;

	/// Amount of memory actually needed (only an estimate)
	uint64_t memusage;

	/// Options decoded from the header needed to initialize
	/// the LZMA decoder
	lzma_options_lzma options;
} lzma_alone_coder;

static lzma_ret
alone_decode(void *coder_ptr, const lzma_allocator *allocator,
		const uint8_t *restrict in, size_t *restrict in_pos,
		size_t in_size, uint8_t *restrict out,
		size_t *restrict out_pos, size_t out_size,
		lzma_action action)
{
	lzma_alone_coder *coder = coder_ptr;

	while (*out_pos < out_size
			&& (coder->sequence == SEQ_CODE || *in_pos < in_size))
	switch (coder->sequence) {
	case SEQ_PROPERTIES:
		if (lzma_lzma_lclppb_decode(&coder->options, in[*in_pos]))
			return LZMA_FORMAT_ERROR;

		coder->sequence = SEQ_DICTIONARY_SIZE;
		++*in_pos;
		break;

	case SEQ_DICTIONARY_SIZE:
		coder->options.dict_size
				|= (size_t)(in[*in_pos]) << (coder->pos * 8);

		if (++coder->pos == 4) {
			if (coder->picky && coder->options.dict_size
					!= UINT32_MAX) {
				// A hack to ditch tons of false positives:
				// We allow only dictionary sizes that are
				// 2^n or 2^n + 2^(n-1). LZMA_Alone created
				// only files with 2^n, but accepts any
				// dictionary size.
				uint32_t d = coder->options.dict_size - 1;
				d |= d >> 2;
				d |= d >> 3;
				d |= d >> 4;
				d |= d >> 8;
				d |= d >> 16;
				++d;

				if (d != coder->options.dict_size)
					return LZMA_FORMAT_ERROR;
			}

			coder->pos = 0;
			coder->sequence = SEQ_UNCOMPRESSED_SIZE;
		}

		++*in_pos;
		break;

	case SEQ_UNCOMPRESSED_SIZE:
		coder->uncompressed_size
				|= (lzma_vli)(in[*in_pos]) << (coder->pos * 8);
		++*in_pos;
		if (++coder->pos < 8)
			break;

		// Another hack to ditch false positives: Assume that
		// if the uncompressed size is known, it must be less
		// than 256 GiB.
		if (coder->picky
				&& coder->uncompressed_size != LZMA_VLI_UNKNOWN
				&& coder->uncompressed_size
					>= (LZMA_VLI_C(1) << 38))
			return LZMA_FORMAT_ERROR;

		// Calculate the memory usage so that it is ready
		// for SEQ_CODER_INIT.
		coder->memusage = lzma_lzma_decoder_memusage(&coder->options)
				+ LZMA_MEMUSAGE_BASE;

		coder->pos = 0;
		coder->sequence = SEQ_CODER_INIT;

	// Fall through

	case SEQ_CODER_INIT: {
		if (coder->memusage > coder->memlimit)
			return LZMA_MEMLIMIT_ERROR;

		lzma_filter_info filters[2] = {
			{
				.init = &lzma_lzma_decoder_init,
				.options = &coder->options,
			}, {
				.init = NULL,
			}
		};

		const lzma_ret ret = lzma_next_filter_init(&coder->next,
				allocator, filters);
		if (ret != LZMA_OK)
			return ret;

		// Use a hack to set the uncompressed size.
		lzma_lz_decoder_uncompressed(coder->next.coder,
				coder->uncompressed_size);

		coder->sequence = SEQ_CODE;
		break;
	}

	case SEQ_CODE: {
		return coder->next.code(coder->next.coder,
				allocator, in, in_pos, in_size,
				out, out_pos, out_size, action);
	}

	default:
		return LZMA_PROG_ERROR;
	}

	return LZMA_OK;
}

static void
alone_decoder_end(void *coder_ptr, const lzma_allocator *allocator)
{
	lzma_alone_coder *coder = coder_ptr;
	lzma_next_end(&coder->next, allocator);
	lzma_free(coder, allocator);
	return;
}

static lzma_ret
alone_decoder_memconfig(void *coder_ptr, uint64_t *memusage,
		uint64_t *old_memlimit, uint64_t new_memlimit)
{
	lzma_alone_coder *coder = coder_ptr;

	*memusage = coder->memusage;
	*old_memlimit = coder->memlimit;

	if (new_memlimit != 0) {
		if (new_memlimit < coder->memusage)
			return LZMA_MEMLIMIT_ERROR;

		coder->memlimit = new_memlimit;
	}

	return LZMA_OK;
}

extern lzma_ret
lzma_alone_decoder_init(lzma_next_coder *next, const lzma_allocator *allocator,
		uint64_t memlimit, bool picky)
{
	lzma_next_coder_init(&lzma_alone_decoder_init, next, allocator);

	lzma_alone_coder *coder = next->coder;

	if (coder == NULL) {
		coder = lzma_alloc(sizeof(lzma_alone_coder), allocator);
		if (coder == NULL)
			return LZMA_MEM_ERROR;

		next->coder = coder;
		next->code = &alone_decode;
		next->end = &alone_decoder_end;
		next->memconfig = &alone_decoder_memconfig;
		coder->next = LZMA_NEXT_CODER_INIT;
	}

	coder->sequence = SEQ_PROPERTIES;
	coder->picky = picky;
	coder->pos = 0;
	coder->options.dict_size = 0;
	coder->options.preset_dict = NULL;
	coder->options.preset_dict_size = 0;
	coder->uncompressed_size = 0;
	coder->memlimit = my_max(1, memlimit);
	coder->memusage = LZMA_MEMUSAGE_BASE;

	return LZMA_OK;
}

extern LZMA_API(lzma_ret)
lzma_alone_decoder(lzma_stream *strm, uint64_t memlimit)
{
	lzma_next_strm_init(lzma_alone_decoder_init, strm, memlimit, false);

	strm->internal->supported_actions[LZMA_RUN] = true;
	strm->internal->supported_actions[LZMA_FINISH] = true;

	return LZMA_OK;
}

/*** End of inlined file: alone_decoder.c ***/



/*** Start of inlined file: alone_encoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       alone_encoder.c
/// \brief      Encoder for LZMA_Alone files
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////


/*** Start of inlined file: lzma_encoder.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       lzma_encoder.h
/// \brief      LZMA encoder API
///
//  Authors:    Igor Pavlov
//              Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LZMA_LZMA_ENCODER_H
#define LZMA_LZMA_ENCODER_H

#include "common.h"

typedef struct lzma_lzma1_encoder_s lzma_lzma1_encoder;

extern lzma_ret lzma_lzma_encoder_init(lzma_next_coder *next,
		const lzma_allocator *allocator,
		const lzma_filter_info *filters);

extern uint64_t lzma_lzma_encoder_memusage(const void *options);

extern lzma_ret lzma_lzma_props_encode(const void *options, uint8_t *out);

/// Encodes lc/lp/pb into one byte. Returns false on success and true on error.
extern bool lzma_lzma_lclppb_encode(
		const lzma_options_lzma *options, uint8_t *byte);

#ifdef LZMA_LZ_ENCODER_H

/// Initializes raw LZMA encoder; this is used by LZMA2.
extern lzma_ret lzma_lzma_encoder_create(
		void **coder_ptr, const lzma_allocator *allocator,
		const lzma_options_lzma *options, lzma_lz_options *lz_options);

/// Resets an already initialized LZMA encoder; this is used by LZMA2.
extern lzma_ret lzma_lzma_encoder_reset(
		lzma_lzma1_encoder *coder, const lzma_options_lzma *options);

extern lzma_ret lzma_lzma_encode(lzma_lzma1_encoder *restrict coder,
		lzma_mf *restrict mf, uint8_t *restrict out,
		size_t *restrict out_pos, size_t out_size,
		uint32_t read_limit);

#endif

#endif

/*** End of inlined file: lzma_encoder.h ***/

#define ALONE_HEADER_SIZE (1 + 4 + 8)

typedef struct {
	lzma_next_coder next;

	enum {
		SEQ_HEADER,
		SEQ_CODE,
	} sequence;

	size_t header_pos;
	uint8_t header[ALONE_HEADER_SIZE];
} lzma_alone_coder;

static lzma_ret
alone_encode(void *coder_ptr, const lzma_allocator *allocator,
		const uint8_t *restrict in, size_t *restrict in_pos,
		size_t in_size, uint8_t *restrict out,
		size_t *restrict out_pos, size_t out_size,
		lzma_action action)
{
	lzma_alone_coder *coder = coder_ptr;

	while (*out_pos < out_size)
	switch (coder->sequence) {
	case SEQ_HEADER:
		lzma_bufcpy(coder->header, &coder->header_pos,
				ALONE_HEADER_SIZE,
				out, out_pos, out_size);
		if (coder->header_pos < ALONE_HEADER_SIZE)
			return LZMA_OK;

		coder->sequence = SEQ_CODE;
		break;

	case SEQ_CODE:
		return coder->next.code(coder->next.coder,
				allocator, in, in_pos, in_size,
				out, out_pos, out_size, action);

	default:
		assert(0);
		return LZMA_PROG_ERROR;
	}

	return LZMA_OK;
}

static void
alone_encoder_end(void *coder_ptr, const lzma_allocator *allocator)
{
	lzma_alone_coder *coder = coder_ptr;
	lzma_next_end(&coder->next, allocator);
	lzma_free(coder, allocator);
	return;
}

// At least for now, this is not used by any internal function.
static lzma_ret
alone_encoder_init(lzma_next_coder *next, const lzma_allocator *allocator,
		const lzma_options_lzma *options)
{
	lzma_next_coder_init(&alone_encoder_init, next, allocator);

	lzma_alone_coder *coder = next->coder;

	if (coder == NULL) {
		coder = lzma_alloc(sizeof(lzma_alone_coder), allocator);
		if (coder == NULL)
			return LZMA_MEM_ERROR;

		next->coder = coder;
		next->code = &alone_encode;
		next->end = &alone_encoder_end;
		coder->next = LZMA_NEXT_CODER_INIT;
	}

	// Basic initializations
	coder->sequence = SEQ_HEADER;
	coder->header_pos = 0;

	// Encode the header:
	// - Properties (1 byte)
	if (lzma_lzma_lclppb_encode(options, coder->header))
		return LZMA_OPTIONS_ERROR;

	// - Dictionary size (4 bytes)
	if (options->dict_size < LZMA_DICT_SIZE_MIN)
		return LZMA_OPTIONS_ERROR;

	// Round up to the next 2^n or 2^n + 2^(n - 1) depending on which
	// one is the next unless it is UINT32_MAX. While the header would
	// allow any 32-bit integer, we do this to keep the decoder of liblzma
	// accepting the resulting files.
	uint32_t d = options->dict_size - 1;
	d |= d >> 2;
	d |= d >> 3;
	d |= d >> 4;
	d |= d >> 8;
	d |= d >> 16;
	if (d != UINT32_MAX)
		++d;

	write32le(coder->header + 1, d);

	// - Uncompressed size (always unknown and using EOPM)
	memset(coder->header + 1 + 4, 0xFF, 8);

	// Initialize the LZMA encoder.
	const lzma_filter_info filters[2] = {
		{
			.init = &lzma_lzma_encoder_init,
			.options = (void *)(options),
		}, {
			.init = NULL,
		}
	};

	return lzma_next_filter_init(&coder->next, allocator, filters);
}

/*
extern lzma_ret
lzma_alone_encoder_init(lzma_next_coder *next, const lzma_allocator *allocator,
		const lzma_options_alone *options)
{
	lzma_next_coder_init(&alone_encoder_init, next, allocator, options);
}
*/

extern LZMA_API(lzma_ret)
lzma_alone_encoder(lzma_stream *strm, const lzma_options_lzma *options)
{
	lzma_next_strm_init(alone_encoder_init, strm, options);

	strm->internal->supported_actions[LZMA_RUN] = true;
	strm->internal->supported_actions[LZMA_FINISH] = true;

	return LZMA_OK;
}

/*** End of inlined file: alone_encoder.c ***/


/*** Start of inlined file: auto_decoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       auto_decoder.c
/// \brief      Autodetect between .xz Stream and .lzma (LZMA_Alone) formats
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

typedef struct {
	/// Stream decoder or LZMA_Alone decoder
	lzma_next_coder next;

	uint64_t memlimit;
	uint32_t flags;

	enum {
		SEQ_INIT,
		SEQ_CODE,
		SEQ_FINISH,
	} sequence;
} lzma_auto_coder;

static lzma_ret
auto_decode(void *coder_ptr, const lzma_allocator *allocator,
		const uint8_t *restrict in, size_t *restrict in_pos,
		size_t in_size, uint8_t *restrict out,
		size_t *restrict out_pos, size_t out_size, lzma_action action)
{
	lzma_auto_coder *coder = coder_ptr;

	switch (coder->sequence) {
	case SEQ_INIT:
		if (*in_pos >= in_size)
			return LZMA_OK;

		// Update the sequence now, because we want to continue from
		// SEQ_CODE even if we return some LZMA_*_CHECK.
		coder->sequence = SEQ_CODE;

		// Detect the file format. For now this is simple, since if
		// it doesn't start with 0xFD (the first magic byte of the
		// new format), it has to be LZMA_Alone, or something that
		// we don't support at all.
		if (in[*in_pos] == 0xFD) {
			return_if_error(lzma_stream_decoder_init(
					&coder->next, allocator,
					coder->memlimit, coder->flags));
		} else {
			return_if_error(lzma_alone_decoder_init(&coder->next,
					allocator, coder->memlimit, true));

			// If the application wants to know about missing
			// integrity check or about the check in general, we
			// need to handle it here, because LZMA_Alone decoder
			// doesn't accept any flags.
			if (coder->flags & LZMA_TELL_NO_CHECK)
				return LZMA_NO_CHECK;

			if (coder->flags & LZMA_TELL_ANY_CHECK)
				return LZMA_GET_CHECK;
		}

	// Fall through

	case SEQ_CODE: {
		const lzma_ret ret = coder->next.code(
				coder->next.coder, allocator,
				in, in_pos, in_size,
				out, out_pos, out_size, action);
		if (ret != LZMA_STREAM_END
				|| (coder->flags & LZMA_CONCATENATED) == 0)
			return ret;

		coder->sequence = SEQ_FINISH;
	}

	// Fall through

	case SEQ_FINISH:
		// When LZMA_DECODE_CONCATENATED was used and we were decoding
		// LZMA_Alone file, we need to check check that there is no
		// trailing garbage and wait for LZMA_FINISH.
		if (*in_pos < in_size)
			return LZMA_DATA_ERROR;

		return action == LZMA_FINISH ? LZMA_STREAM_END : LZMA_OK;

	default:
		assert(0);
		return LZMA_PROG_ERROR;
	}
}

static void
auto_decoder_end(void *coder_ptr, const lzma_allocator *allocator)
{
	lzma_auto_coder *coder = coder_ptr;
	lzma_next_end(&coder->next, allocator);
	lzma_free(coder, allocator);
	return;
}

static lzma_check
auto_decoder_get_check(const void *coder_ptr)
{
	const lzma_auto_coder *coder = coder_ptr;

	// It is LZMA_Alone if get_check is NULL.
	return coder->next.get_check == NULL ? LZMA_CHECK_NONE
			: coder->next.get_check(coder->next.coder);
}

static lzma_ret
auto_decoder_memconfig(void *coder_ptr, uint64_t *memusage,
		uint64_t *old_memlimit, uint64_t new_memlimit)
{
	lzma_auto_coder *coder = coder_ptr;

	lzma_ret ret;

	if (coder->next.memconfig != NULL) {
		ret = coder->next.memconfig(coder->next.coder,
				memusage, old_memlimit, new_memlimit);
		assert(*old_memlimit == coder->memlimit);
	} else {
		// No coder is configured yet. Use the base value as
		// the current memory usage.
		*memusage = LZMA_MEMUSAGE_BASE;
		*old_memlimit = coder->memlimit;

		ret = LZMA_OK;
		if (new_memlimit != 0 && new_memlimit < *memusage)
			ret = LZMA_MEMLIMIT_ERROR;
	}

	if (ret == LZMA_OK && new_memlimit != 0)
		coder->memlimit = new_memlimit;

	return ret;
}

static lzma_ret
auto_decoder_init(lzma_next_coder *next, const lzma_allocator *allocator,
		uint64_t memlimit, uint32_t flags)
{
	lzma_next_coder_init(&auto_decoder_init, next, allocator);

	if (flags & ~LZMA_SUPPORTED_FLAGS)
		return LZMA_OPTIONS_ERROR;

	lzma_auto_coder *coder = next->coder;
	if (coder == NULL) {
		coder = lzma_alloc(sizeof(lzma_auto_coder), allocator);
		if (coder == NULL)
			return LZMA_MEM_ERROR;

		next->coder = coder;
		next->code = &auto_decode;
		next->end = &auto_decoder_end;
		next->get_check = &auto_decoder_get_check;
		next->memconfig = &auto_decoder_memconfig;
		coder->next = LZMA_NEXT_CODER_INIT;
	}

	coder->memlimit = my_max(1, memlimit);
	coder->flags = flags;
	coder->sequence = SEQ_INIT;

	return LZMA_OK;
}

extern LZMA_API(lzma_ret)
lzma_auto_decoder(lzma_stream *strm, uint64_t memlimit, uint32_t flags)
{
	lzma_next_strm_init(auto_decoder_init, strm, memlimit, flags);

	strm->internal->supported_actions[LZMA_RUN] = true;
	strm->internal->supported_actions[LZMA_FINISH] = true;

	return LZMA_OK;
}

/*** End of inlined file: auto_decoder.c ***/


/*** Start of inlined file: block_buffer_decoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       block_buffer_decoder.c
/// \brief      Single-call .xz Block decoder
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

extern LZMA_API(lzma_ret)
lzma_block_buffer_decode(lzma_block *block, const lzma_allocator *allocator,
		const uint8_t *in, size_t *in_pos, size_t in_size,
		uint8_t *out, size_t *out_pos, size_t out_size)
{
	if (in_pos == NULL || (in == NULL && *in_pos != in_size)
			|| *in_pos > in_size || out_pos == NULL
			|| (out == NULL && *out_pos != out_size)
			|| *out_pos > out_size)
		return LZMA_PROG_ERROR;

	// Initialize the Block decoder.
	lzma_next_coder block_decoder = LZMA_NEXT_CODER_INIT;
	lzma_ret ret = lzma_block_decoder_init(
			&block_decoder, allocator, block);

	if (ret == LZMA_OK) {
		// Save the positions so that we can restore them in case
		// an error occurs.
		const size_t in_start = *in_pos;
		const size_t out_start = *out_pos;

		// Do the actual decoding.
		ret = block_decoder.code(block_decoder.coder, allocator,
				in, in_pos, in_size, out, out_pos, out_size,
				LZMA_FINISH);

		if (ret == LZMA_STREAM_END) {
			ret = LZMA_OK;
		} else {
			if (ret == LZMA_OK) {
				// Either the input was truncated or the
				// output buffer was too small.
				assert(*in_pos == in_size
						|| *out_pos == out_size);

				// If all the input was consumed, then the
				// input is truncated, even if the output
				// buffer is also full. This is because
				// processing the last byte of the Block
				// never produces output.
				//
				// NOTE: This assumption may break when new
				// filters are added, if the end marker of
				// the filter doesn't consume at least one
				// complete byte.
				if (*in_pos == in_size)
					ret = LZMA_DATA_ERROR;
				else
					ret = LZMA_BUF_ERROR;
			}

			// Restore the positions.
			*in_pos = in_start;
			*out_pos = out_start;
		}
	}

	// Free the decoder memory. This needs to be done even if
	// initialization fails, because the internal API doesn't
	// require the initialization function to free its memory on error.
	lzma_next_end(&block_decoder, allocator);

	return ret;
}

/*** End of inlined file: block_buffer_decoder.c ***/


/*** Start of inlined file: block_buffer_encoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       block_buffer_encoder.c
/// \brief      Single-call .xz Block encoder
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "lzma2_encoder.h"
#include "check.h"

/// Estimate the maximum size of the Block Header and Check fields for
/// a Block that uses LZMA2 uncompressed chunks. We could use
/// lzma_block_header_size() but this is simpler.
///
/// Block Header Size + Block Flags + Compressed Size
/// + Uncompressed Size + Filter Flags for LZMA2 + CRC32 + Check
/// and round up to the next multiple of four to take Header Padding
/// into account.
#define HEADERS_BOUND ((1 + 1 + 2 * LZMA_VLI_BYTES_MAX + 3 + 4 \
		+ LZMA_CHECK_SIZE_MAX + 3) & ~3)

static uint64_t
lzma2_bound(uint64_t uncompressed_size)
{
	// Prevent integer overflow in overhead calculation.
	if (uncompressed_size > COMPRESSED_SIZE_MAX)
		return 0;

	// Calculate the exact overhead of the LZMA2 headers: Round
	// uncompressed_size up to the next multiple of LZMA2_CHUNK_MAX,
	// multiply by the size of per-chunk header, and add one byte for
	// the end marker.
	const uint64_t overhead = ((uncompressed_size + LZMA2_CHUNK_MAX - 1)
				/ LZMA2_CHUNK_MAX)
			* LZMA2_HEADER_UNCOMPRESSED + 1;

	// Catch the possible integer overflow.
	if (COMPRESSED_SIZE_MAX - overhead < uncompressed_size)
		return 0;

	return uncompressed_size + overhead;
}

extern uint64_t
lzma_block_buffer_bound64(uint64_t uncompressed_size)
{
	// If the data doesn't compress, we always use uncompressed
	// LZMA2 chunks.
	uint64_t lzma2_size = lzma2_bound(uncompressed_size);
	if (lzma2_size == 0)
		return 0;

	// Take Block Padding into account.
	lzma2_size = (lzma2_size + 3) & ~UINT64_C(3);

	// No risk of integer overflow because lzma2_bound() already takes
	// into account the size of the headers in the Block.
	return HEADERS_BOUND + lzma2_size;
}

extern LZMA_API(size_t)
lzma_block_buffer_bound(size_t uncompressed_size)
{
	uint64_t ret = lzma_block_buffer_bound64(uncompressed_size);

#if SIZE_MAX < UINT64_MAX
	// Catch the possible integer overflow on 32-bit systems.
	if (ret > SIZE_MAX)
		return 0;
#endif

	return ret;
}

static lzma_ret
block_encode_uncompressed(lzma_block *block, const uint8_t *in, size_t in_size,
		uint8_t *out, size_t *out_pos, size_t out_size)
{
	// Use LZMA2 uncompressed chunks. We wouldn't need a dictionary at
	// all, but LZMA2 always requires a dictionary, so use the minimum
	// value to minimize memory usage of the decoder.
	lzma_options_lzma lzma2 = {
		.dict_size = LZMA_DICT_SIZE_MIN,
	};

	lzma_filter filters[2];
	filters[0].id = LZMA_FILTER_LZMA2;
	filters[0].options = &lzma2;
	filters[1].id = LZMA_VLI_UNKNOWN;

	// Set the above filter options to *block temporarily so that we can
	// encode the Block Header.
	lzma_filter *filters_orig = block->filters;
	block->filters = filters;

	if (lzma_block_header_size(block) != LZMA_OK) {
		block->filters = filters_orig;
		return LZMA_PROG_ERROR;
	}

	// Check that there's enough output space. The caller has already
	// set block->compressed_size to what lzma2_bound() has returned,
	// so we can reuse that value. We know that compressed_size is a
	// known valid VLI and header_size is a small value so their sum
	// will never overflow.
	assert(block->compressed_size == lzma2_bound(in_size));
	if (out_size - *out_pos
			< block->header_size + block->compressed_size) {
		block->filters = filters_orig;
		return LZMA_BUF_ERROR;
	}

	if (lzma_block_header_encode(block, out + *out_pos) != LZMA_OK) {
		block->filters = filters_orig;
		return LZMA_PROG_ERROR;
	}

	block->filters = filters_orig;
	*out_pos += block->header_size;

	// Encode the data using LZMA2 uncompressed chunks.
	size_t in_pos = 0;
	uint8_t control = 0x01; // Dictionary reset

	while (in_pos < in_size) {
		// Control byte: Indicate uncompressed chunk, of which
		// the first resets the dictionary.
		out[(*out_pos)++] = control;
		control = 0x02; // No dictionary reset

		// Size of the uncompressed chunk
		const size_t copy_size
				= my_min(in_size - in_pos, LZMA2_CHUNK_MAX);
		out[(*out_pos)++] = (copy_size - 1) >> 8;
		out[(*out_pos)++] = (copy_size - 1) & 0xFF;

		// The actual data
		assert(*out_pos + copy_size <= out_size);
		memcpy(out + *out_pos, in + in_pos, copy_size);

		in_pos += copy_size;
		*out_pos += copy_size;
	}

	// End marker
	out[(*out_pos)++] = 0x00;
	assert(*out_pos <= out_size);

	return LZMA_OK;
}

static lzma_ret
block_encode_normal(lzma_block *block, const lzma_allocator *allocator,
		const uint8_t *in, size_t in_size,
		uint8_t *out, size_t *out_pos, size_t out_size)
{
	// Find out the size of the Block Header.
	return_if_error(lzma_block_header_size(block));

	// Reserve space for the Block Header and skip it for now.
	if (out_size - *out_pos <= block->header_size)
		return LZMA_BUF_ERROR;

	const size_t out_start = *out_pos;
	*out_pos += block->header_size;

	// Limit out_size so that we stop encoding if the output would grow
	// bigger than what uncompressed Block would be.
	if (out_size - *out_pos > block->compressed_size)
		out_size = *out_pos + block->compressed_size;

	// TODO: In many common cases this could be optimized to use
	// significantly less memory.
	lzma_next_coder raw_encoder = LZMA_NEXT_CODER_INIT;
	lzma_ret ret = lzma_raw_encoder_init(
			&raw_encoder, allocator, block->filters);

	if (ret == LZMA_OK) {
		size_t in_pos = 0;
		ret = raw_encoder.code(raw_encoder.coder, allocator,
				in, &in_pos, in_size, out, out_pos, out_size,
				LZMA_FINISH);
	}

	// NOTE: This needs to be run even if lzma_raw_encoder_init() failed.
	lzma_next_end(&raw_encoder, allocator);

	if (ret == LZMA_STREAM_END) {
		// Compression was successful. Write the Block Header.
		block->compressed_size
				= *out_pos - (out_start + block->header_size);
		ret = lzma_block_header_encode(block, out + out_start);
		if (ret != LZMA_OK)
			ret = LZMA_PROG_ERROR;

	} else if (ret == LZMA_OK) {
		// Output buffer became full.
		ret = LZMA_BUF_ERROR;
	}

	// Reset *out_pos if something went wrong.
	if (ret != LZMA_OK)
		*out_pos = out_start;

	return ret;
}

static lzma_ret
block_buffer_encode(lzma_block *block, const lzma_allocator *allocator,
		const uint8_t *in, size_t in_size,
		uint8_t *out, size_t *out_pos, size_t out_size,
		bool try_to_compress)
{
	// Validate the arguments.
	if (block == NULL || (in == NULL && in_size != 0) || out == NULL
			|| out_pos == NULL || *out_pos > out_size)
		return LZMA_PROG_ERROR;

	// The contents of the structure may depend on the version so
	// check the version before validating the contents of *block.
	if (block->version > 1)
		return LZMA_OPTIONS_ERROR;

	if ((unsigned int)(block->check) > LZMA_CHECK_ID_MAX
			|| (try_to_compress && block->filters == NULL))
		return LZMA_PROG_ERROR;

	if (!lzma_check_is_supported(block->check))
		return LZMA_UNSUPPORTED_CHECK;

	// Size of a Block has to be a multiple of four, so limit the size
	// here already. This way we don't need to check it again when adding
	// Block Padding.
	out_size -= (out_size - *out_pos) & 3;

	// Get the size of the Check field.
	const size_t check_size = lzma_check_size(block->check);
	assert(check_size != UINT32_MAX);

	// Reserve space for the Check field.
	if (out_size - *out_pos <= check_size)
		return LZMA_BUF_ERROR;

	out_size -= check_size;

	// Initialize block->uncompressed_size and calculate the worst-case
	// value for block->compressed_size.
	block->uncompressed_size = in_size;
	block->compressed_size = lzma2_bound(in_size);
	if (block->compressed_size == 0)
		return LZMA_DATA_ERROR;

	// Do the actual compression.
	lzma_ret ret = LZMA_BUF_ERROR;
	if (try_to_compress)
		ret = block_encode_normal(block, allocator,
				in, in_size, out, out_pos, out_size);

	if (ret != LZMA_OK) {
		// If the error was something else than output buffer
		// becoming full, return the error now.
		if (ret != LZMA_BUF_ERROR)
			return ret;

		// The data was uncompressible (at least with the options
		// given to us) or the output buffer was too small. Use the
		// uncompressed chunks of LZMA2 to wrap the data into a valid
		// Block. If we haven't been given enough output space, even
		// this may fail.
		return_if_error(block_encode_uncompressed(block, in, in_size,
				out, out_pos, out_size));
	}

	assert(*out_pos <= out_size);

	// Block Padding. No buffer overflow here, because we already adjusted
	// out_size so that (out_size - out_start) is a multiple of four.
	// Thus, if the buffer is full, the loop body can never run.
	for (size_t i = (size_t)(block->compressed_size); i & 3; ++i) {
		assert(*out_pos < out_size);
		out[(*out_pos)++] = 0x00;
	}

	// If there's no Check field, we are done now.
	if (check_size > 0) {
		// Calculate the integrity check. We reserved space for
		// the Check field earlier so we don't need to check for
		// available output space here.
		lzma_check_state check;
		lzma_check_init(&check, block->check);
		lzma_check_update(&check, block->check, in, in_size);
		lzma_check_finish(&check, block->check);

		memcpy(block->raw_check, check.buffer.u8, check_size);
		memcpy(out + *out_pos, check.buffer.u8, check_size);
		*out_pos += check_size;
	}

	return LZMA_OK;
}

extern LZMA_API(lzma_ret)
lzma_block_buffer_encode(lzma_block *block, const lzma_allocator *allocator,
		const uint8_t *in, size_t in_size,
		uint8_t *out, size_t *out_pos, size_t out_size)
{
	return block_buffer_encode(block, allocator,
			in, in_size, out, out_pos, out_size, true);
}

extern LZMA_API(lzma_ret)
lzma_block_uncomp_encode(lzma_block *block,
		const uint8_t *in, size_t in_size,
		uint8_t *out, size_t *out_pos, size_t out_size)
{
	// It won't allocate any memory from heap so no need
	// for lzma_allocator.
	return block_buffer_encode(block, NULL,
			in, in_size, out, out_pos, out_size, false);
}

/*** End of inlined file: block_buffer_encoder.c ***/


/*** Start of inlined file: block_decoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       block_decoder.c
/// \brief      Decodes .xz Blocks
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "check.h"

typedef struct {
	enum {
		SEQ_CODE,
		SEQ_PADDING,
		SEQ_CHECK,
	} sequence;

	/// The filters in the chain; initialized with lzma_raw_decoder_init().
	lzma_next_coder next;

	/// Decoding options; we also write Compressed Size and Uncompressed
	/// Size back to this structure when the decoding has been finished.
	lzma_block *block;

	/// Compressed Size calculated while decoding
	lzma_vli compressed_size;

	/// Uncompressed Size calculated while decoding
	lzma_vli uncompressed_size;

	/// Maximum allowed Compressed Size; this takes into account the
	/// size of the Block Header and Check fields when Compressed Size
	/// is unknown.
	lzma_vli compressed_limit;

	/// Position when reading the Check field
	size_t check_pos;

	/// Check of the uncompressed data
	lzma_check_state check;

	/// True if the integrity check won't be calculated and verified.
	bool ignore_check;
} lzma_block_coder;

static inline bool
update_size(lzma_vli *size, lzma_vli add, lzma_vli limit)
{
	if (limit > LZMA_VLI_MAX)
		limit = LZMA_VLI_MAX;

	if (limit < *size || limit - *size < add)
		return true;

	*size += add;

	return false;
}

static inline bool
is_size_valid(lzma_vli size, lzma_vli reference)
{
	return reference == LZMA_VLI_UNKNOWN || reference == size;
}

static lzma_ret
block_decode(void *coder_ptr, const lzma_allocator *allocator,
		const uint8_t *restrict in, size_t *restrict in_pos,
		size_t in_size, uint8_t *restrict out,
		size_t *restrict out_pos, size_t out_size, lzma_action action)
{
	lzma_block_coder *coder = coder_ptr;

	switch (coder->sequence) {
	case SEQ_CODE: {
		const size_t in_start = *in_pos;
		const size_t out_start = *out_pos;

		const lzma_ret ret = coder->next.code(coder->next.coder,
				allocator, in, in_pos, in_size,
				out, out_pos, out_size, action);

		const size_t in_used = *in_pos - in_start;
		const size_t out_used = *out_pos - out_start;

		// NOTE: We compare to compressed_limit here, which prevents
		// the total size of the Block growing past LZMA_VLI_MAX.
		if (update_size(&coder->compressed_size, in_used,
					coder->compressed_limit)
				|| update_size(&coder->uncompressed_size,
					out_used,
					coder->block->uncompressed_size))
			return LZMA_DATA_ERROR;

		if (!coder->ignore_check)
			lzma_check_update(&coder->check, coder->block->check,
					out + out_start, out_used);

		if (ret != LZMA_STREAM_END)
			return ret;

		// Compressed and Uncompressed Sizes are now at their final
		// values. Verify that they match the values given to us.
		if (!is_size_valid(coder->compressed_size,
					coder->block->compressed_size)
				|| !is_size_valid(coder->uncompressed_size,
					coder->block->uncompressed_size))
			return LZMA_DATA_ERROR;

		// Copy the values into coder->block. The caller
		// may use this information to construct Index.
		coder->block->compressed_size = coder->compressed_size;
		coder->block->uncompressed_size = coder->uncompressed_size;

		coder->sequence = SEQ_PADDING;
	}

	// Fall through

	case SEQ_PADDING:
		// Compressed Data is padded to a multiple of four bytes.
		while (coder->compressed_size & 3) {
			if (*in_pos >= in_size)
				return LZMA_OK;

			// We use compressed_size here just get the Padding
			// right. The actual Compressed Size was stored to
			// coder->block already, and won't be modified by
			// us anymore.
			++coder->compressed_size;

			if (in[(*in_pos)++] != 0x00)
				return LZMA_DATA_ERROR;
		}

		if (coder->block->check == LZMA_CHECK_NONE)
			return LZMA_STREAM_END;

		if (!coder->ignore_check)
			lzma_check_finish(&coder->check, coder->block->check);

		coder->sequence = SEQ_CHECK;

	// Fall through

	case SEQ_CHECK: {
		const size_t check_size = lzma_check_size(coder->block->check);
		lzma_bufcpy(in, in_pos, in_size, coder->block->raw_check,
				&coder->check_pos, check_size);
		if (coder->check_pos < check_size)
			return LZMA_OK;

		// Validate the Check only if we support it.
		// coder->check.buffer may be uninitialized
		// when the Check ID is not supported.
		if (!coder->ignore_check
				&& lzma_check_is_supported(coder->block->check)
				&& memcmp(coder->block->raw_check,
					coder->check.buffer.u8,
					check_size) != 0)
			return LZMA_DATA_ERROR;

		return LZMA_STREAM_END;
	}
	}

	return LZMA_PROG_ERROR;
}

static void
block_decoder_end(void *coder_ptr, const lzma_allocator *allocator)
{
	lzma_block_coder *coder = coder_ptr;
	lzma_next_end(&coder->next, allocator);
	lzma_free(coder, allocator);
	return;
}

extern lzma_ret
lzma_block_decoder_init(lzma_next_coder *next, const lzma_allocator *allocator,
		lzma_block *block)
{
	lzma_next_coder_init(&lzma_block_decoder_init, next, allocator);

	// Validate the options. lzma_block_unpadded_size() does that for us
	// except for Uncompressed Size and filters. Filters are validated
	// by the raw decoder.
	if (lzma_block_unpadded_size(block) == 0
			|| !lzma_vli_is_valid(block->uncompressed_size))
		return LZMA_PROG_ERROR;

	// Allocate *next->coder if needed.
	lzma_block_coder *coder = next->coder;
	if (coder == NULL) {
		coder = lzma_alloc(sizeof(lzma_block_coder), allocator);
		if (coder == NULL)
			return LZMA_MEM_ERROR;

		next->coder = coder;
		next->code = &block_decode;
		next->end = &block_decoder_end;
		coder->next = LZMA_NEXT_CODER_INIT;
	}

	// Basic initializations
	coder->sequence = SEQ_CODE;
	coder->block = block;
	coder->compressed_size = 0;
	coder->uncompressed_size = 0;

	// If Compressed Size is not known, we calculate the maximum allowed
	// value so that encoded size of the Block (including Block Padding)
	// is still a valid VLI and a multiple of four.
	coder->compressed_limit
			= block->compressed_size == LZMA_VLI_UNKNOWN
				? (LZMA_VLI_MAX & ~LZMA_VLI_C(3))
					- block->header_size
					- lzma_check_size(block->check)
				: block->compressed_size;

	// Initialize the check. It's caller's problem if the Check ID is not
	// supported, and the Block decoder cannot verify the Check field.
	// Caller can test lzma_check_is_supported(block->check).
	coder->check_pos = 0;
	lzma_check_init(&coder->check, block->check);

	coder->ignore_check = block->version >= 1
			? block->ignore_check : false;

	// Initialize the filter chain.
	return lzma_raw_decoder_init(&coder->next, allocator,
			block->filters);
}

extern LZMA_API(lzma_ret)
lzma_block_decoder(lzma_stream *strm, lzma_block *block)
{
	lzma_next_strm_init(lzma_block_decoder_init, strm, block);

	strm->internal->supported_actions[LZMA_RUN] = true;
	strm->internal->supported_actions[LZMA_FINISH] = true;

	return LZMA_OK;
}

/*** End of inlined file: block_decoder.c ***/


/*** Start of inlined file: block_encoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       block_encoder.c
/// \brief      Encodes .xz Blocks
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "check.h"

typedef struct {
	/// The filters in the chain; initialized with lzma_raw_decoder_init().
	lzma_next_coder next;

	/// Encoding options; we also write Unpadded Size, Compressed Size,
	/// and Uncompressed Size back to this structure when the encoding
	/// has been finished.
	lzma_block *block;

	enum {
		SEQ_CODE,
		SEQ_PADDING,
		SEQ_CHECK,
	} sequence;

	/// Compressed Size calculated while encoding
	lzma_vli compressed_size;

	/// Uncompressed Size calculated while encoding
	lzma_vli uncompressed_size;

	/// Position in the Check field
	size_t pos;

	/// Check of the uncompressed data
	lzma_check_state check;
} lzma_block_coder;

static lzma_ret
block_encode(void *coder_ptr, const lzma_allocator *allocator,
		const uint8_t *restrict in, size_t *restrict in_pos,
		size_t in_size, uint8_t *restrict out,
		size_t *restrict out_pos, size_t out_size, lzma_action action)
{
	lzma_block_coder *coder = coder_ptr;

	// Check that our amount of input stays in proper limits.
	if (LZMA_VLI_MAX - coder->uncompressed_size < in_size - *in_pos)
		return LZMA_DATA_ERROR;

	switch (coder->sequence) {
	case SEQ_CODE: {
		const size_t in_start = *in_pos;
		const size_t out_start = *out_pos;

		const lzma_ret ret = coder->next.code(coder->next.coder,
				allocator, in, in_pos, in_size,
				out, out_pos, out_size, action);

		const size_t in_used = *in_pos - in_start;
		const size_t out_used = *out_pos - out_start;

		if (COMPRESSED_SIZE_MAX - coder->compressed_size < out_used)
			return LZMA_DATA_ERROR;

		coder->compressed_size += out_used;

		// No need to check for overflow because we have already
		// checked it at the beginning of this function.
		coder->uncompressed_size += in_used;

		lzma_check_update(&coder->check, coder->block->check,
				in + in_start, in_used);

		if (ret != LZMA_STREAM_END || action == LZMA_SYNC_FLUSH)
			return ret;

		assert(*in_pos == in_size);
		assert(action == LZMA_FINISH);

		// Copy the values into coder->block. The caller
		// may use this information to construct Index.
		coder->block->compressed_size = coder->compressed_size;
		coder->block->uncompressed_size = coder->uncompressed_size;

		coder->sequence = SEQ_PADDING;
	}

	// Fall through

	case SEQ_PADDING:
		// Pad Compressed Data to a multiple of four bytes. We can
		// use coder->compressed_size for this since we don't need
		// it for anything else anymore.
		while (coder->compressed_size & 3) {
			if (*out_pos >= out_size)
				return LZMA_OK;

			out[*out_pos] = 0x00;
			++*out_pos;
			++coder->compressed_size;
		}

		if (coder->block->check == LZMA_CHECK_NONE)
			return LZMA_STREAM_END;

		lzma_check_finish(&coder->check, coder->block->check);

		coder->sequence = SEQ_CHECK;

	// Fall through

	case SEQ_CHECK: {
		const size_t check_size = lzma_check_size(coder->block->check);
		lzma_bufcpy(coder->check.buffer.u8, &coder->pos, check_size,
				out, out_pos, out_size);
		if (coder->pos < check_size)
			return LZMA_OK;

		memcpy(coder->block->raw_check, coder->check.buffer.u8,
				check_size);
		return LZMA_STREAM_END;
	}
	}

	return LZMA_PROG_ERROR;
}

static void
block_encoder_end(void *coder_ptr, const lzma_allocator *allocator)
{
	lzma_block_coder *coder = coder_ptr;
	lzma_next_end(&coder->next, allocator);
	lzma_free(coder, allocator);
	return;
}

static lzma_ret
block_encoder_update(void *coder_ptr, const lzma_allocator *allocator,
		const lzma_filter *filters lzma_attribute((__unused__)),
		const lzma_filter *reversed_filters)
{
	lzma_block_coder *coder = coder_ptr;

	if (coder->sequence != SEQ_CODE)
		return LZMA_PROG_ERROR;

	return lzma_next_filter_update(
			&coder->next, allocator, reversed_filters);
}

extern lzma_ret
lzma_block_encoder_init(lzma_next_coder *next, const lzma_allocator *allocator,
		lzma_block *block)
{
	lzma_next_coder_init(&lzma_block_encoder_init, next, allocator);

	if (block == NULL)
		return LZMA_PROG_ERROR;

	// The contents of the structure may depend on the version so
	// check the version first.
	if (block->version > 1)
		return LZMA_OPTIONS_ERROR;

	// If the Check ID is not supported, we cannot calculate the check and
	// thus not create a proper Block.
	if ((unsigned int)(block->check) > LZMA_CHECK_ID_MAX)
		return LZMA_PROG_ERROR;

	if (!lzma_check_is_supported(block->check))
		return LZMA_UNSUPPORTED_CHECK;

	// Allocate and initialize *next->coder if needed.
	lzma_block_coder *coder = next->coder;
	if (coder == NULL) {
		coder = lzma_alloc(sizeof(lzma_block_coder), allocator);
		if (coder == NULL)
			return LZMA_MEM_ERROR;

		next->coder = coder;
		next->code = &block_encode;
		next->end = &block_encoder_end;
		next->update = &block_encoder_update;
		coder->next = LZMA_NEXT_CODER_INIT;
	}

	// Basic initializations
	coder->sequence = SEQ_CODE;
	coder->block = block;
	coder->compressed_size = 0;
	coder->uncompressed_size = 0;
	coder->pos = 0;

	// Initialize the check
	lzma_check_init(&coder->check, block->check);

	// Initialize the requested filters.
	return lzma_raw_encoder_init(&coder->next, allocator, block->filters);
}

extern LZMA_API(lzma_ret)
lzma_block_encoder(lzma_stream *strm, lzma_block *block)
{
	lzma_next_strm_init(lzma_block_encoder_init, strm, block);

	strm->internal->supported_actions[LZMA_RUN] = true;
	strm->internal->supported_actions[LZMA_FINISH] = true;

	return LZMA_OK;
}

/*** End of inlined file: block_encoder.c ***/


/*** Start of inlined file: block_header_decoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       block_header_decoder.c
/// \brief      Decodes Block Header from .xz files
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "check.h"

static void
free_properties(lzma_block *block, const lzma_allocator *allocator)
{
	// Free allocated filter options. The last array member is not
	// touched after the initialization in the beginning of
	// lzma_block_header_decode(), so we don't need to touch that here.
	for (size_t i = 0; i < LZMA_FILTERS_MAX; ++i) {
		lzma_free(block->filters[i].options, allocator);
		block->filters[i].id = LZMA_VLI_UNKNOWN;
		block->filters[i].options = NULL;
	}

	return;
}

extern LZMA_API(lzma_ret)
lzma_block_header_decode(lzma_block *block,
		const lzma_allocator *allocator, const uint8_t *in)
{
	// NOTE: We consider the header to be corrupt not only when the
	// CRC32 doesn't match, but also when variable-length integers
	// are invalid or over 63 bits, or if the header is too small
	// to contain the claimed information.

	// Initialize the filter options array. This way the caller can
	// safely free() the options even if an error occurs in this function.
	for (size_t i = 0; i <= LZMA_FILTERS_MAX; ++i) {
		block->filters[i].id = LZMA_VLI_UNKNOWN;
		block->filters[i].options = NULL;
	}

	// Versions 0 and 1 are supported. If a newer version was specified,
	// we need to downgrade it.
	if (block->version > 1)
		block->version = 1;

	// This isn't a Block Header option, but since the decompressor will
	// read it if version >= 1, it's better to initialize it here than
	// to expect the caller to do it since in almost all cases this
	// should be false.
	block->ignore_check = false;

	// Validate Block Header Size and Check type. The caller must have
	// already set these, so it is a programming error if this test fails.
	if (lzma_block_header_size_decode(in[0]) != block->header_size
			|| (unsigned int)(block->check) > LZMA_CHECK_ID_MAX)
		return LZMA_PROG_ERROR;

	// Exclude the CRC32 field.
	const size_t in_size = block->header_size - 4;

	// Verify CRC32
	if (lzma_crc32(in, in_size, 0) != read32le(in + in_size))
		return LZMA_DATA_ERROR;

	// Check for unsupported flags.
	if (in[1] & 0x3C)
		return LZMA_OPTIONS_ERROR;

	// Start after the Block Header Size and Block Flags fields.
	size_t in_pos = 2;

	// Compressed Size
	if (in[1] & 0x40) {
		return_if_error(lzma_vli_decode(&block->compressed_size,
				NULL, in, &in_pos, in_size));

		// Validate Compressed Size. This checks that it isn't zero
		// and that the total size of the Block is a valid VLI.
		if (lzma_block_unpadded_size(block) == 0)
			return LZMA_DATA_ERROR;
	} else {
		block->compressed_size = LZMA_VLI_UNKNOWN;
	}

	// Uncompressed Size
	if (in[1] & 0x80)
		return_if_error(lzma_vli_decode(&block->uncompressed_size,
				NULL, in, &in_pos, in_size));
	else
		block->uncompressed_size = LZMA_VLI_UNKNOWN;

	// Filter Flags
	const size_t filter_count = (in[1] & 3U) + 1;
	for (size_t i = 0; i < filter_count; ++i) {
		const lzma_ret ret = lzma_filter_flags_decode(
				&block->filters[i], allocator,
				in, &in_pos, in_size);
		if (ret != LZMA_OK) {
			free_properties(block, allocator);
			return ret;
		}
	}

	// Padding
	while (in_pos < in_size) {
		if (in[in_pos++] != 0x00) {
			free_properties(block, allocator);

			// Possibly some new field present so use
			// LZMA_OPTIONS_ERROR instead of LZMA_DATA_ERROR.
			return LZMA_OPTIONS_ERROR;
		}
	}

	return LZMA_OK;
}

/*** End of inlined file: block_header_decoder.c ***/


/*** Start of inlined file: block_header_encoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       block_header_encoder.c
/// \brief      Encodes Block Header for .xz files
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "check.h"

extern LZMA_API(lzma_ret)
lzma_block_header_size(lzma_block *block)
{
	if (block->version > 1)
		return LZMA_OPTIONS_ERROR;

	// Block Header Size + Block Flags + CRC32.
	uint32_t size = 1 + 1 + 4;

	// Compressed Size
	if (block->compressed_size != LZMA_VLI_UNKNOWN) {
		const uint32_t add = lzma_vli_size(block->compressed_size);
		if (add == 0 || block->compressed_size == 0)
			return LZMA_PROG_ERROR;

		size += add;
	}

	// Uncompressed Size
	if (block->uncompressed_size != LZMA_VLI_UNKNOWN) {
		const uint32_t add = lzma_vli_size(block->uncompressed_size);
		if (add == 0)
			return LZMA_PROG_ERROR;

		size += add;
	}

	// List of Filter Flags
	if (block->filters == NULL || block->filters[0].id == LZMA_VLI_UNKNOWN)
		return LZMA_PROG_ERROR;

	for (size_t i = 0; block->filters[i].id != LZMA_VLI_UNKNOWN; ++i) {
		// Don't allow too many filters.
		if (i == LZMA_FILTERS_MAX)
			return LZMA_PROG_ERROR;

		uint32_t add;
		return_if_error(lzma_filter_flags_size(&add,
				block->filters + i));

		size += add;
	}

	// Pad to a multiple of four bytes.
	block->header_size = (size + 3) & ~UINT32_C(3);

	// NOTE: We don't verify that the encoded size of the Block stays
	// within limits. This is because it is possible that we are called
	// with exaggerated Compressed Size (e.g. LZMA_VLI_MAX) to reserve
	// space for Block Header, and later called again with lower,
	// real values.

	return LZMA_OK;
}

extern LZMA_API(lzma_ret)
lzma_block_header_encode(const lzma_block *block, uint8_t *out)
{
	// Validate everything but filters.
	if (lzma_block_unpadded_size(block) == 0
			|| !lzma_vli_is_valid(block->uncompressed_size))
		return LZMA_PROG_ERROR;

	// Indicate the size of the buffer _excluding_ the CRC32 field.
	const size_t out_size = block->header_size - 4;

	// Store the Block Header Size.
	out[0] = out_size / 4;

	// We write Block Flags in pieces.
	out[1] = 0x00;
	size_t out_pos = 2;

	// Compressed Size
	if (block->compressed_size != LZMA_VLI_UNKNOWN) {
		return_if_error(lzma_vli_encode(block->compressed_size, NULL,
				out, &out_pos, out_size));

		out[1] |= 0x40;
	}

	// Uncompressed Size
	if (block->uncompressed_size != LZMA_VLI_UNKNOWN) {
		return_if_error(lzma_vli_encode(block->uncompressed_size, NULL,
				out, &out_pos, out_size));

		out[1] |= 0x80;
	}

	// Filter Flags
	if (block->filters == NULL || block->filters[0].id == LZMA_VLI_UNKNOWN)
		return LZMA_PROG_ERROR;

	size_t filter_count = 0;
	do {
		// There can be a maximum of four filters.
		if (filter_count == LZMA_FILTERS_MAX)
			return LZMA_PROG_ERROR;

		return_if_error(lzma_filter_flags_encode(
				block->filters + filter_count,
				out, &out_pos, out_size));

	} while (block->filters[++filter_count].id != LZMA_VLI_UNKNOWN);

	out[1] |= filter_count - 1;

	// Padding
	memzero(out + out_pos, out_size - out_pos);

	// CRC32
	write32le(out + out_size, lzma_crc32(out, out_size, 0));

	return LZMA_OK;
}

/*** End of inlined file: block_header_encoder.c ***/


/*** Start of inlined file: block_util.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       block_util.c
/// \brief      Utility functions to handle lzma_block
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

extern LZMA_API(lzma_ret)
lzma_block_compressed_size(lzma_block *block, lzma_vli unpadded_size)
{
	// Validate everything but Uncompressed Size and filters.
	if (lzma_block_unpadded_size(block) == 0)
		return LZMA_PROG_ERROR;

	const uint32_t container_size = block->header_size
			+ lzma_check_size(block->check);

	// Validate that Compressed Size will be greater than zero.
	if (unpadded_size <= container_size)
		return LZMA_DATA_ERROR;

	// Calculate what Compressed Size is supposed to be.
	// If Compressed Size was present in Block Header,
	// compare that the new value matches it.
	const lzma_vli compressed_size = unpadded_size - container_size;
	if (block->compressed_size != LZMA_VLI_UNKNOWN
			&& block->compressed_size != compressed_size)
		return LZMA_DATA_ERROR;

	block->compressed_size = compressed_size;

	return LZMA_OK;
}

extern LZMA_API(lzma_vli)
lzma_block_unpadded_size(const lzma_block *block)
{
	// Validate the values that we are interested in i.e. all but
	// Uncompressed Size and the filters.
	//
	// NOTE: This function is used for validation too, so it is
	// essential that these checks are always done even if
	// Compressed Size is unknown.
	if (block == NULL || block->version > 1
			|| block->header_size < LZMA_BLOCK_HEADER_SIZE_MIN
			|| block->header_size > LZMA_BLOCK_HEADER_SIZE_MAX
			|| (block->header_size & 3)
			|| !lzma_vli_is_valid(block->compressed_size)
			|| block->compressed_size == 0
			|| (unsigned int)(block->check) > LZMA_CHECK_ID_MAX)
		return 0;

	// If Compressed Size is unknown, return that we cannot know
	// size of the Block either.
	if (block->compressed_size == LZMA_VLI_UNKNOWN)
		return LZMA_VLI_UNKNOWN;

	// Calculate Unpadded Size and validate it.
	const lzma_vli unpadded_size = block->compressed_size
				+ block->header_size
				+ lzma_check_size(block->check);

	assert(unpadded_size >= UNPADDED_SIZE_MIN);
	if (unpadded_size > UNPADDED_SIZE_MAX)
		return 0;

	return unpadded_size;
}

extern LZMA_API(lzma_vli)
lzma_block_total_size(const lzma_block *block)
{
	lzma_vli unpadded_size = lzma_block_unpadded_size(block);

	if (unpadded_size != LZMA_VLI_UNKNOWN)
		unpadded_size = vli_ceil4(unpadded_size);

	return unpadded_size;
}

/*** End of inlined file: block_util.c ***/


/*** Start of inlined file: common.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       common.c
/// \brief      Common functions needed in many places in liblzma
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

/////////////
// Version //
/////////////

extern LZMA_API(uint32_t)
lzma_version_number(void)
{
	return LZMA_VERSION;
}

extern LZMA_API(const char *)
lzma_version_string(void)
{
	return LZMA_VERSION_STRING;
}

///////////////////////
// Memory allocation //
///////////////////////

extern void * lzma_attribute((__malloc__)) lzma_attr_alloc_size(1)
lzma_alloc(size_t size, const lzma_allocator *allocator)
{
	// Some malloc() variants return NULL if called with size == 0.
	if (size == 0)
		size = 1;

	void *ptr;

	if (allocator != NULL && allocator->alloc != NULL)
		ptr = allocator->alloc(allocator->opaque, 1, size);
	else
		ptr = malloc(size);

	return ptr;
}

extern void * lzma_attribute((__malloc__)) lzma_attr_alloc_size(1)
lzma_alloc_zero(size_t size, const lzma_allocator *allocator)
{
	// Some calloc() variants return NULL if called with size == 0.
	if (size == 0)
		size = 1;

	void *ptr;

	if (allocator != NULL && allocator->alloc != NULL) {
		ptr = allocator->alloc(allocator->opaque, 1, size);
		if (ptr != NULL)
			memzero(ptr, size);
	} else {
		ptr = calloc(1, size);
	}

	return ptr;
}

extern void
lzma_free(void *ptr, const lzma_allocator *allocator)
{
	if (allocator != NULL && allocator->free != NULL)
		allocator->free(allocator->opaque, ptr);
	else
		free(ptr);

	return;
}

//////////
// Misc //
//////////

extern size_t
lzma_bufcpy(const uint8_t *restrict in, size_t *restrict in_pos,
		size_t in_size, uint8_t *restrict out,
		size_t *restrict out_pos, size_t out_size)
{
	const size_t in_avail = in_size - *in_pos;
	const size_t out_avail = out_size - *out_pos;
	const size_t copy_size = my_min(in_avail, out_avail);

	// Call memcpy() only if there is something to copy. If there is
	// nothing to copy, in or out might be NULL and then the memcpy()
	// call would trigger undefined behavior.
	if (copy_size > 0)
		memcpy(out + *out_pos, in + *in_pos, copy_size);

	*in_pos += copy_size;
	*out_pos += copy_size;

	return copy_size;
}

extern lzma_ret
lzma_next_filter_init(lzma_next_coder *next, const lzma_allocator *allocator,
		const lzma_filter_info *filters)
{
	lzma_next_coder_init(filters[0].init, next, allocator);
	next->id = filters[0].id;
	return filters[0].init == NULL
			? LZMA_OK : filters[0].init(next, allocator, filters);
}

extern lzma_ret
lzma_next_filter_update(lzma_next_coder *next, const lzma_allocator *allocator,
		const lzma_filter *reversed_filters)
{
	// Check that the application isn't trying to change the Filter ID.
	// End of filters is indicated with LZMA_VLI_UNKNOWN in both
	// reversed_filters[0].id and next->id.
	if (reversed_filters[0].id != next->id)
		return LZMA_PROG_ERROR;

	if (reversed_filters[0].id == LZMA_VLI_UNKNOWN)
		return LZMA_OK;

	assert(next->update != NULL);
	return next->update(next->coder, allocator, NULL, reversed_filters);
}

extern void
lzma_next_end(lzma_next_coder *next, const lzma_allocator *allocator)
{
	if (next->init != (uintptr_t)(NULL)) {
		// To avoid tiny end functions that simply call
		// lzma_free(coder, allocator), we allow leaving next->end
		// NULL and call lzma_free() here.
		if (next->end != NULL)
			next->end(next->coder, allocator);
		else
			lzma_free(next->coder, allocator);

		// Reset the variables so the we don't accidentally think
		// that it is an already initialized coder.
		*next = LZMA_NEXT_CODER_INIT;
	}

	return;
}

//////////////////////////////////////
// External to internal API wrapper //
//////////////////////////////////////

extern lzma_ret
lzma_strm_init(lzma_stream *strm)
{
	if (strm == NULL)
		return LZMA_PROG_ERROR;

	if (strm->internal == NULL) {
		strm->internal = lzma_alloc(sizeof(lzma_internal),
				strm->allocator);
		if (strm->internal == NULL)
			return LZMA_MEM_ERROR;

		strm->internal->next = LZMA_NEXT_CODER_INIT;
	}

	memzero(strm->internal->supported_actions,
			sizeof(strm->internal->supported_actions));
	strm->internal->sequence = ISEQ_RUN;
	strm->internal->allow_buf_error = false;

	strm->total_in = 0;
	strm->total_out = 0;

	return LZMA_OK;
}

extern LZMA_API(lzma_ret)
lzma_code(lzma_stream *strm, lzma_action action)
{
	// Sanity checks
	if ((strm->next_in == NULL && strm->avail_in != 0)
			|| (strm->next_out == NULL && strm->avail_out != 0)
			|| strm->internal == NULL
			|| strm->internal->next.code == NULL
			|| (unsigned int)(action) > LZMA_ACTION_MAX
			|| !strm->internal->supported_actions[action])
		return LZMA_PROG_ERROR;

	// Check if unsupported members have been set to non-zero or non-NULL,
	// which would indicate that some new feature is wanted.
	if (strm->reserved_ptr1 != NULL
			|| strm->reserved_ptr2 != NULL
			|| strm->reserved_ptr3 != NULL
			|| strm->reserved_ptr4 != NULL
			|| strm->reserved_int1 != 0
			|| strm->reserved_int2 != 0
			|| strm->reserved_int3 != 0
			|| strm->reserved_int4 != 0
			|| strm->reserved_enum1 != LZMA_RESERVED_ENUM
			|| strm->reserved_enum2 != LZMA_RESERVED_ENUM)
		return LZMA_OPTIONS_ERROR;

	switch (strm->internal->sequence) {
	case ISEQ_RUN:
		switch (action) {
		case LZMA_RUN:
			break;

		case LZMA_SYNC_FLUSH:
			strm->internal->sequence = ISEQ_SYNC_FLUSH;
			break;

		case LZMA_FULL_FLUSH:
			strm->internal->sequence = ISEQ_FULL_FLUSH;
			break;

		case LZMA_FINISH:
			strm->internal->sequence = ISEQ_FINISH;
			break;

		case LZMA_FULL_BARRIER:
			strm->internal->sequence = ISEQ_FULL_BARRIER;
			break;
		}

		break;

	case ISEQ_SYNC_FLUSH:
		// The same action must be used until we return
		// LZMA_STREAM_END, and the amount of input must not change.
		if (action != LZMA_SYNC_FLUSH
				|| strm->internal->avail_in != strm->avail_in)
			return LZMA_PROG_ERROR;

		break;

	case ISEQ_FULL_FLUSH:
		if (action != LZMA_FULL_FLUSH
				|| strm->internal->avail_in != strm->avail_in)
			return LZMA_PROG_ERROR;

		break;

	case ISEQ_FINISH:
		if (action != LZMA_FINISH
				|| strm->internal->avail_in != strm->avail_in)
			return LZMA_PROG_ERROR;

		break;

	case ISEQ_FULL_BARRIER:
		if (action != LZMA_FULL_BARRIER
				|| strm->internal->avail_in != strm->avail_in)
			return LZMA_PROG_ERROR;

		break;

	case ISEQ_END:
		return LZMA_STREAM_END;

	case ISEQ_ERROR:
	default:
		return LZMA_PROG_ERROR;
	}

	size_t in_pos = 0;
	size_t out_pos = 0;
	lzma_ret ret = strm->internal->next.code(
			strm->internal->next.coder, strm->allocator,
			strm->next_in, &in_pos, strm->avail_in,
			strm->next_out, &out_pos, strm->avail_out, action);

	strm->next_in += in_pos;
	strm->avail_in -= in_pos;
	strm->total_in += in_pos;

	strm->next_out += out_pos;
	strm->avail_out -= out_pos;
	strm->total_out += out_pos;

	strm->internal->avail_in = strm->avail_in;

	// Cast is needed to silence a warning about LZMA_TIMED_OUT, which
	// isn't part of lzma_ret enumeration.
	switch ((unsigned int)(ret)) {
	case LZMA_OK:
		// Don't return LZMA_BUF_ERROR when it happens the first time.
		// This is to avoid returning LZMA_BUF_ERROR when avail_out
		// was zero but still there was no more data left to written
		// to next_out.
		if (out_pos == 0 && in_pos == 0) {
			if (strm->internal->allow_buf_error)
				ret = LZMA_BUF_ERROR;
			else
				strm->internal->allow_buf_error = true;
		} else {
			strm->internal->allow_buf_error = false;
		}
		break;

	case LZMA_TIMED_OUT:
		strm->internal->allow_buf_error = false;
		ret = LZMA_OK;
		break;

	case LZMA_STREAM_END:
		if (strm->internal->sequence == ISEQ_SYNC_FLUSH
				|| strm->internal->sequence == ISEQ_FULL_FLUSH
				|| strm->internal->sequence
					== ISEQ_FULL_BARRIER)
			strm->internal->sequence = ISEQ_RUN;
		else
			strm->internal->sequence = ISEQ_END;

	// Fall through

	case LZMA_NO_CHECK:
	case LZMA_UNSUPPORTED_CHECK:
	case LZMA_GET_CHECK:
	case LZMA_MEMLIMIT_ERROR:
		// Something else than LZMA_OK, but not a fatal error,
		// that is, coding may be continued (except if ISEQ_END).
		strm->internal->allow_buf_error = false;
		break;

	default:
		// All the other errors are fatal; coding cannot be continued.
		assert(ret != LZMA_BUF_ERROR);
		strm->internal->sequence = ISEQ_ERROR;
		break;
	}

	return ret;
}

extern LZMA_API(void)
lzma_end(lzma_stream *strm)
{
	if (strm != NULL && strm->internal != NULL) {
		lzma_next_end(&strm->internal->next, strm->allocator);
		lzma_free(strm->internal, strm->allocator);
		strm->internal = NULL;
	}

	return;
}

extern LZMA_API(void)
lzma_get_progress(lzma_stream *strm,
		uint64_t *progress_in, uint64_t *progress_out)
{
	if (strm->internal->next.get_progress != NULL) {
		strm->internal->next.get_progress(strm->internal->next.coder,
				progress_in, progress_out);
	} else {
		*progress_in = strm->total_in;
		*progress_out = strm->total_out;
	}

	return;
}

extern LZMA_API(lzma_check)
lzma_get_check(const lzma_stream *strm)
{
	// Return LZMA_CHECK_NONE if we cannot know the check type.
	// It's a bug in the application if this happens.
	if (strm->internal->next.get_check == NULL)
		return LZMA_CHECK_NONE;

	return strm->internal->next.get_check(strm->internal->next.coder);
}

extern LZMA_API(uint64_t)
lzma_memusage(const lzma_stream *strm)
{
	uint64_t memusage;
	uint64_t old_memlimit;

	if (strm == NULL || strm->internal == NULL
			|| strm->internal->next.memconfig == NULL
			|| strm->internal->next.memconfig(
				strm->internal->next.coder,
				&memusage, &old_memlimit, 0) != LZMA_OK)
		return 0;

	return memusage;
}

extern LZMA_API(uint64_t)
lzma_memlimit_get(const lzma_stream *strm)
{
	uint64_t old_memlimit;
	uint64_t memusage;

	if (strm == NULL || strm->internal == NULL
			|| strm->internal->next.memconfig == NULL
			|| strm->internal->next.memconfig(
				strm->internal->next.coder,
				&memusage, &old_memlimit, 0) != LZMA_OK)
		return 0;

	return old_memlimit;
}

extern LZMA_API(lzma_ret)
lzma_memlimit_set(lzma_stream *strm, uint64_t new_memlimit)
{
	// Dummy variables to simplify memconfig functions
	uint64_t old_memlimit;
	uint64_t memusage;

	if (strm == NULL || strm->internal == NULL
			|| strm->internal->next.memconfig == NULL)
		return LZMA_PROG_ERROR;

	// Zero is a special value that cannot be used as an actual limit.
	// If 0 was specified, use 1 instead.
	if (new_memlimit == 0)
		new_memlimit = 1;

	return strm->internal->next.memconfig(strm->internal->next.coder,
			&memusage, &old_memlimit, new_memlimit);
}

/*** End of inlined file: common.c ***/


/*** Start of inlined file: easy_buffer_encoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       easy_buffer_encoder.c
/// \brief      Easy single-call .xz Stream encoder
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////


/*** Start of inlined file: easy_preset.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       easy_preset.h
/// \brief      Preset handling for easy encoder and decoder
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

typedef struct {
	/// We need to keep the filters array available in case
	/// LZMA_FULL_FLUSH is used.
	lzma_filter filters[LZMA_FILTERS_MAX + 1];

	/// Options for LZMA2
	lzma_options_lzma opt_lzma;

	// Options for more filters can be added later, so this struct
	// is not ready to be put into the public API.

} lzma_options_easy;

/// Set *easy to the settings given by the preset. Returns true on error,
/// false on success.
extern bool lzma_easy_preset(lzma_options_easy *easy, uint32_t preset);

/*** End of inlined file: easy_preset.h ***/

extern LZMA_API(lzma_ret)
lzma_easy_buffer_encode(uint32_t preset, lzma_check check,
		const lzma_allocator *allocator, const uint8_t *in,
		size_t in_size, uint8_t *out, size_t *out_pos, size_t out_size)
{
	lzma_options_easy opt_easy;
	if (lzma_easy_preset(&opt_easy, preset))
		return LZMA_OPTIONS_ERROR;

	return lzma_stream_buffer_encode(opt_easy.filters, check,
			allocator, in, in_size, out, out_pos, out_size);
}

/*** End of inlined file: easy_buffer_encoder.c ***/


/*** Start of inlined file: easy_decoder_memusage.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       easy_decoder_memusage.c
/// \brief      Decoder memory usage calculation to match easy encoder presets
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////


/*** Start of inlined file: easy_preset.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       easy_preset.h
/// \brief      Preset handling for easy encoder and decoder
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

typedef struct {
	/// We need to keep the filters array available in case
	/// LZMA_FULL_FLUSH is used.
	lzma_filter filters[LZMA_FILTERS_MAX + 1];

	/// Options for LZMA2
	lzma_options_lzma opt_lzma;

	// Options for more filters can be added later, so this struct
	// is not ready to be put into the public API.

} lzma_options_easy;

/// Set *easy to the settings given by the preset. Returns true on error,
/// false on success.
extern bool lzma_easy_preset(lzma_options_easy *easy, uint32_t preset);

/*** End of inlined file: easy_preset.h ***/

extern LZMA_API(uint64_t)
lzma_easy_decoder_memusage(uint32_t preset)
{
	lzma_options_easy opt_easy;
	if (lzma_easy_preset(&opt_easy, preset))
		return UINT32_MAX;

	return lzma_raw_decoder_memusage(opt_easy.filters);
}

/*** End of inlined file: easy_decoder_memusage.c ***/


/*** Start of inlined file: easy_encoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       easy_encoder.c
/// \brief      Easy .xz Stream encoder initialization
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////


/*** Start of inlined file: easy_preset.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       easy_preset.h
/// \brief      Preset handling for easy encoder and decoder
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

typedef struct {
	/// We need to keep the filters array available in case
	/// LZMA_FULL_FLUSH is used.
	lzma_filter filters[LZMA_FILTERS_MAX + 1];

	/// Options for LZMA2
	lzma_options_lzma opt_lzma;

	// Options for more filters can be added later, so this struct
	// is not ready to be put into the public API.

} lzma_options_easy;

/// Set *easy to the settings given by the preset. Returns true on error,
/// false on success.
extern bool lzma_easy_preset(lzma_options_easy *easy, uint32_t preset);

/*** End of inlined file: easy_preset.h ***/

extern LZMA_API(lzma_ret)
lzma_easy_encoder(lzma_stream *strm, uint32_t preset, lzma_check check)
{
	lzma_options_easy opt_easy;
	if (lzma_easy_preset(&opt_easy, preset))
		return LZMA_OPTIONS_ERROR;

	return lzma_stream_encoder(strm, opt_easy.filters, check);
}

/*** End of inlined file: easy_encoder.c ***/


/*** Start of inlined file: easy_encoder_memusage.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       easy_encoder_memusage.c
/// \brief      Easy .xz Stream encoder memory usage calculation
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////


/*** Start of inlined file: easy_preset.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       easy_preset.h
/// \brief      Preset handling for easy encoder and decoder
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

typedef struct {
	/// We need to keep the filters array available in case
	/// LZMA_FULL_FLUSH is used.
	lzma_filter filters[LZMA_FILTERS_MAX + 1];

	/// Options for LZMA2
	lzma_options_lzma opt_lzma;

	// Options for more filters can be added later, so this struct
	// is not ready to be put into the public API.

} lzma_options_easy;

/// Set *easy to the settings given by the preset. Returns true on error,
/// false on success.
extern bool lzma_easy_preset(lzma_options_easy *easy, uint32_t preset);

/*** End of inlined file: easy_preset.h ***/

extern LZMA_API(uint64_t)
lzma_easy_encoder_memusage(uint32_t preset)
{
	lzma_options_easy opt_easy;
	if (lzma_easy_preset(&opt_easy, preset))
		return UINT32_MAX;

	return lzma_raw_encoder_memusage(opt_easy.filters);
}

/*** End of inlined file: easy_encoder_memusage.c ***/


/*** Start of inlined file: easy_preset.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       easy_preset.c
/// \brief      Preset handling for easy encoder and decoder
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////


/*** Start of inlined file: easy_preset.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       easy_preset.h
/// \brief      Preset handling for easy encoder and decoder
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

typedef struct {
	/// We need to keep the filters array available in case
	/// LZMA_FULL_FLUSH is used.
	lzma_filter filters[LZMA_FILTERS_MAX + 1];

	/// Options for LZMA2
	lzma_options_lzma opt_lzma;

	// Options for more filters can be added later, so this struct
	// is not ready to be put into the public API.

} lzma_options_easy;

/// Set *easy to the settings given by the preset. Returns true on error,
/// false on success.
extern bool lzma_easy_preset(lzma_options_easy *easy, uint32_t preset);

/*** End of inlined file: easy_preset.h ***/

extern bool
lzma_easy_preset(lzma_options_easy *opt_easy, uint32_t preset)
{
	if (lzma_lzma_preset(&opt_easy->opt_lzma, preset))
		return true;

	opt_easy->filters[0].id = LZMA_FILTER_LZMA2;
	opt_easy->filters[0].options = &opt_easy->opt_lzma;
	opt_easy->filters[1].id = LZMA_VLI_UNKNOWN;

	return false;
}

/*** End of inlined file: easy_preset.c ***/


/*** Start of inlined file: filter_buffer_decoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       filter_buffer_decoder.c
/// \brief      Single-call raw decoding
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

extern LZMA_API(lzma_ret)
lzma_raw_buffer_decode(
		const lzma_filter *filters, const lzma_allocator *allocator,
		const uint8_t *in, size_t *in_pos, size_t in_size,
		uint8_t *out, size_t *out_pos, size_t out_size)
{
	// Validate what isn't validated later in filter_common.c.
	if (in == NULL || in_pos == NULL || *in_pos > in_size || out == NULL
			|| out_pos == NULL || *out_pos > out_size)
		return LZMA_PROG_ERROR;

	// Initialize the decoer.
	lzma_next_coder next = LZMA_NEXT_CODER_INIT;
	return_if_error(lzma_raw_decoder_init(&next, allocator, filters));

	// Store the positions so that we can restore them if something
	// goes wrong.
	const size_t in_start = *in_pos;
	const size_t out_start = *out_pos;

	// Do the actual decoding and free decoder's memory.
	lzma_ret ret = next.code(next.coder, allocator, in, in_pos, in_size,
			out, out_pos, out_size, LZMA_FINISH);

	if (ret == LZMA_STREAM_END) {
		ret = LZMA_OK;
	} else {
		if (ret == LZMA_OK) {
			// Either the input was truncated or the
			// output buffer was too small.
			assert(*in_pos == in_size || *out_pos == out_size);

			if (*in_pos != in_size) {
				// Since input wasn't consumed completely,
				// the output buffer became full and is
				// too small.
				ret = LZMA_BUF_ERROR;

			} else if (*out_pos != out_size) {
				// Since output didn't became full, the input
				// has to be truncated.
				ret = LZMA_DATA_ERROR;

			} else {
				// All the input was consumed and output
				// buffer is full. Now we don't immediately
				// know the reason for the error. Try
				// decoding one more byte. If it succeeds,
				// then the output buffer was too small. If
				// we cannot get a new output byte, the input
				// is truncated.
				uint8_t tmp[1];
				size_t tmp_pos = 0;
				(void)next.code(next.coder, allocator,
						in, in_pos, in_size,
						tmp, &tmp_pos, 1, LZMA_FINISH);

				if (tmp_pos == 1)
					ret = LZMA_BUF_ERROR;
				else
					ret = LZMA_DATA_ERROR;
			}
		}

		// Restore the positions.
		*in_pos = in_start;
		*out_pos = out_start;
	}

	lzma_next_end(&next, allocator);

	return ret;
}

/*** End of inlined file: filter_buffer_decoder.c ***/


/*** Start of inlined file: filter_buffer_encoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       filter_buffer_encoder.c
/// \brief      Single-call raw encoding
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

extern LZMA_API(lzma_ret)
lzma_raw_buffer_encode(
		const lzma_filter *filters, const lzma_allocator *allocator,
		const uint8_t *in, size_t in_size,
		uint8_t *out, size_t *out_pos, size_t out_size)
{
	// Validate what isn't validated later in filter_common.c.
	if ((in == NULL && in_size != 0) || out == NULL
			|| out_pos == NULL || *out_pos > out_size)
		return LZMA_PROG_ERROR;

	// Initialize the encoder
	lzma_next_coder next = LZMA_NEXT_CODER_INIT;
	return_if_error(lzma_raw_encoder_init(&next, allocator, filters));

	// Store the output position so that we can restore it if
	// something goes wrong.
	const size_t out_start = *out_pos;

	// Do the actual encoding and free coder's memory.
	size_t in_pos = 0;
	lzma_ret ret = next.code(next.coder, allocator, in, &in_pos, in_size,
			out, out_pos, out_size, LZMA_FINISH);
	lzma_next_end(&next, allocator);

	if (ret == LZMA_STREAM_END) {
		ret = LZMA_OK;
	} else {
		if (ret == LZMA_OK) {
			// Output buffer was too small.
			assert(*out_pos == out_size);
			ret = LZMA_BUF_ERROR;
		}

		// Restore the output position.
		*out_pos = out_start;
	}

	return ret;
}

/*** End of inlined file: filter_buffer_encoder.c ***/


/*** Start of inlined file: filter_common.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       filter_common.c
/// \brief      Filter-specific stuff common for both encoder and decoder
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

static const struct {
	/// Filter ID
	lzma_vli id;

	/// Size of the filter-specific options structure
	size_t options_size;

	/// True if it is OK to use this filter as non-last filter in
	/// the chain.
	bool non_last_ok;

	/// True if it is OK to use this filter as the last filter in
	/// the chain.
	bool last_ok;

	/// True if the filter may change the size of the data (that is, the
	/// amount of encoded output can be different than the amount of
	/// uncompressed input).
	bool changes_size;

} features[] = {
#if defined (HAVE_ENCODER_LZMA1) || defined(HAVE_DECODER_LZMA1)
	{
		.id = LZMA_FILTER_LZMA1,
		.options_size = sizeof(lzma_options_lzma),
		.non_last_ok = false,
		.last_ok = true,
		.changes_size = true,
	},
#endif
#if defined(HAVE_ENCODER_LZMA2) || defined(HAVE_DECODER_LZMA2)
	{
		.id = LZMA_FILTER_LZMA2,
		.options_size = sizeof(lzma_options_lzma),
		.non_last_ok = false,
		.last_ok = true,
		.changes_size = true,
	},
#endif
#if defined(HAVE_ENCODER_X86) || defined(HAVE_DECODER_X86)
	{
		.id = LZMA_FILTER_X86,
		.options_size = sizeof(lzma_options_bcj),
		.non_last_ok = true,
		.last_ok = false,
		.changes_size = false,
	},
#endif
#if defined(HAVE_ENCODER_POWERPC) || defined(HAVE_DECODER_POWERPC)
	{
		.id = LZMA_FILTER_POWERPC,
		.options_size = sizeof(lzma_options_bcj),
		.non_last_ok = true,
		.last_ok = false,
		.changes_size = false,
	},
#endif
#if defined(HAVE_ENCODER_IA64) || defined(HAVE_DECODER_IA64)
	{
		.id = LZMA_FILTER_IA64,
		.options_size = sizeof(lzma_options_bcj),
		.non_last_ok = true,
		.last_ok = false,
		.changes_size = false,
	},
#endif
#if defined(HAVE_ENCODER_ARM) || defined(HAVE_DECODER_ARM)
	{
		.id = LZMA_FILTER_ARM,
		.options_size = sizeof(lzma_options_bcj),
		.non_last_ok = true,
		.last_ok = false,
		.changes_size = false,
	},
#endif
#if defined(HAVE_ENCODER_ARMTHUMB) || defined(HAVE_DECODER_ARMTHUMB)
	{
		.id = LZMA_FILTER_ARMTHUMB,
		.options_size = sizeof(lzma_options_bcj),
		.non_last_ok = true,
		.last_ok = false,
		.changes_size = false,
	},
#endif
#if defined(HAVE_ENCODER_SPARC) || defined(HAVE_DECODER_SPARC)
	{
		.id = LZMA_FILTER_SPARC,
		.options_size = sizeof(lzma_options_bcj),
		.non_last_ok = true,
		.last_ok = false,
		.changes_size = false,
	},
#endif
#if defined(HAVE_ENCODER_DELTA) || defined(HAVE_DECODER_DELTA)
	{
		.id = LZMA_FILTER_DELTA,
		.options_size = sizeof(lzma_options_delta),
		.non_last_ok = true,
		.last_ok = false,
		.changes_size = false,
	},
#endif
	{
		.id = LZMA_VLI_UNKNOWN
	}
};

extern LZMA_API(lzma_ret)
lzma_filters_copy(const lzma_filter *src, lzma_filter *dest,
		const lzma_allocator *allocator)
{
	if (src == NULL || dest == NULL)
		return LZMA_PROG_ERROR;

	lzma_ret ret;
	size_t i;
	for (i = 0; src[i].id != LZMA_VLI_UNKNOWN; ++i) {
		// There must be a maximum of four filters plus
		// the array terminator.
		if (i == LZMA_FILTERS_MAX) {
			ret = LZMA_OPTIONS_ERROR;
			goto error;
		}

		dest[i].id = src[i].id;

		if (src[i].options == NULL) {
			dest[i].options = NULL;
		} else {
			// See if the filter is supported only when the
			// options is not NULL. This might be convenient
			// sometimes if the app is actually copying only
			// a partial filter chain with a place holder ID.
			//
			// When options is not NULL, the Filter ID must be
			// supported by us, because otherwise we don't know
			// how big the options are.
			size_t j;
			for (j = 0; src[i].id != features[j].id; ++j) {
				if (features[j].id == LZMA_VLI_UNKNOWN) {
					ret = LZMA_OPTIONS_ERROR;
					goto error;
				}
			}

			// Allocate and copy the options.
			dest[i].options = lzma_alloc(features[j].options_size,
					allocator);
			if (dest[i].options == NULL) {
				ret = LZMA_MEM_ERROR;
				goto error;
			}

			memcpy(dest[i].options, src[i].options,
					features[j].options_size);
		}
	}

	// Terminate the filter array.
	assert(i <= LZMA_FILTERS_MAX + 1);
	dest[i].id = LZMA_VLI_UNKNOWN;
	dest[i].options = NULL;

	return LZMA_OK;

error:
	// Free the options which we have already allocated.
	while (i-- > 0) {
		lzma_free(dest[i].options, allocator);
		dest[i].options = NULL;
	}

	return ret;
}

static lzma_ret
validate_chain(const lzma_filter *filters, size_t *count)
{
	// There must be at least one filter.
	if (filters == NULL || filters[0].id == LZMA_VLI_UNKNOWN)
		return LZMA_PROG_ERROR;

	// Number of non-last filters that may change the size of the data
	// significantly (that is, more than 1-2 % or so).
	size_t changes_size_count = 0;

	// True if it is OK to add a new filter after the current filter.
	bool non_last_ok = true;

	// True if the last filter in the given chain is actually usable as
	// the last filter. Only filters that support embedding End of Payload
	// Marker can be used as the last filter in the chain.
	bool last_ok = false;

	size_t i = 0;
	do {
		size_t j;
		for (j = 0; filters[i].id != features[j].id; ++j)
			if (features[j].id == LZMA_VLI_UNKNOWN)
				return LZMA_OPTIONS_ERROR;

		// If the previous filter in the chain cannot be a non-last
		// filter, the chain is invalid.
		if (!non_last_ok)
			return LZMA_OPTIONS_ERROR;

		non_last_ok = features[j].non_last_ok;
		last_ok = features[j].last_ok;
		changes_size_count += features[j].changes_size;

	} while (filters[++i].id != LZMA_VLI_UNKNOWN);

	// There must be 1-4 filters. The last filter must be usable as
	// the last filter in the chain. A maximum of three filters are
	// allowed to change the size of the data.
	if (i > LZMA_FILTERS_MAX || !last_ok || changes_size_count > 3)
		return LZMA_OPTIONS_ERROR;

	*count = i;
	return LZMA_OK;
}

extern lzma_ret
lzma_raw_coder_init(lzma_next_coder *next, const lzma_allocator *allocator,
		const lzma_filter *options,
		lzma_filter_find coder_find, bool is_encoder)
{
	// Do some basic validation and get the number of filters.
	size_t count;
	return_if_error(validate_chain(options, &count));

	// Set the filter functions and copy the options pointer.
	lzma_filter_info filters[LZMA_FILTERS_MAX + 1];
	if (is_encoder) {
		for (size_t i = 0; i < count; ++i) {
			// The order of the filters is reversed in the
			// encoder. It allows more efficient handling
			// of the uncompressed data.
			const size_t j = count - i - 1;

			const lzma_filter_coder *const fc
					= coder_find(options[i].id);
			if (fc == NULL || fc->init == NULL)
				return LZMA_OPTIONS_ERROR;

			filters[j].id = options[i].id;
			filters[j].init = fc->init;
			filters[j].options = options[i].options;
		}
	} else {
		for (size_t i = 0; i < count; ++i) {
			const lzma_filter_coder *const fc
					= coder_find(options[i].id);
			if (fc == NULL || fc->init == NULL)
				return LZMA_OPTIONS_ERROR;

			filters[i].id = options[i].id;
			filters[i].init = fc->init;
			filters[i].options = options[i].options;
		}
	}

	// Terminate the array.
	filters[count].id = LZMA_VLI_UNKNOWN;
	filters[count].init = NULL;

	// Initialize the filters.
	const lzma_ret ret = lzma_next_filter_init(next, allocator, filters);
	if (ret != LZMA_OK)
		lzma_next_end(next, allocator);

	return ret;
}

extern uint64_t
lzma_raw_coder_memusage(lzma_filter_find coder_find,
		const lzma_filter *filters)
{
	// The chain has to have at least one filter.
	{
		size_t tmp;
		if (validate_chain(filters, &tmp) != LZMA_OK)
			return UINT64_MAX;
	}

	uint64_t total = 0;
	size_t i = 0;

	do {
		const lzma_filter_coder *const fc
				 = coder_find(filters[i].id);
		if (fc == NULL)
			return UINT64_MAX; // Unsupported Filter ID

		if (fc->memusage == NULL) {
			// This filter doesn't have a function to calculate
			// the memory usage and validate the options. Such
			// filters need only little memory, so we use 1 KiB
			// as a good estimate. They also accept all possible
			// options, so there's no need to worry about lack
			// of validation.
			total += 1024;
		} else {
			// Call the filter-specific memory usage calculation
			// function.
			const uint64_t usage
					= fc->memusage(filters[i].options);
			if (usage == UINT64_MAX)
				return UINT64_MAX; // Invalid options

			total += usage;
		}
	} while (filters[++i].id != LZMA_VLI_UNKNOWN);

	// Add some fixed amount of extra. It's to compensate memory usage
	// of Stream, Block etc. coders, malloc() overhead, stack etc.
	return total + LZMA_MEMUSAGE_BASE;
}

/*** End of inlined file: filter_common.c ***/


/*** Start of inlined file: filter_decoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       filter_decoder.c
/// \brief      Filter ID mapping to filter-specific functions
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "lzma_decoder.h"
#include "lzma2_decoder.h"
#include "simple_decoder.h"
#include "delta_decoder.h"

typedef struct {
	/// Filter ID
	lzma_vli id;

	/// Initializes the filter encoder and calls lzma_next_filter_init()
	/// for filters + 1.
	lzma_init_function init;

	/// Calculates memory usage of the encoder. If the options are
	/// invalid, UINT64_MAX is returned.
	uint64_t (*memusage)(const void *options);

	/// Decodes Filter Properties.
	///
	/// \return     - LZMA_OK: Properties decoded successfully.
	///             - LZMA_OPTIONS_ERROR: Unsupported properties
	///             - LZMA_MEM_ERROR: Memory allocation failed.
	lzma_ret (*props_decode)(
			void **options, const lzma_allocator *allocator,
			const uint8_t *props, size_t props_size);

} lzma_filter_decoder;

static const lzma_filter_decoder decoders[] = {
#ifdef HAVE_DECODER_LZMA1
	{
		.id = LZMA_FILTER_LZMA1,
		.init = &lzma_lzma_decoder_init,
		.memusage = &lzma_lzma_decoder_memusage,
		.props_decode = &lzma_lzma_props_decode,
	},
#endif
#ifdef HAVE_DECODER_LZMA2
	{
		.id = LZMA_FILTER_LZMA2,
		.init = &lzma_lzma2_decoder_init,
		.memusage = &lzma_lzma2_decoder_memusage,
		.props_decode = &lzma_lzma2_props_decode,
	},
#endif
#ifdef HAVE_DECODER_X86
	{
		.id = LZMA_FILTER_X86,
		.init = &lzma_simple_x86_decoder_init,
		.memusage = NULL,
		.props_decode = &lzma_simple_props_decode,
	},
#endif
#ifdef HAVE_DECODER_POWERPC
	{
		.id = LZMA_FILTER_POWERPC,
		.init = &lzma_simple_powerpc_decoder_init,
		.memusage = NULL,
		.props_decode = &lzma_simple_props_decode,
	},
#endif
#ifdef HAVE_DECODER_IA64
	{
		.id = LZMA_FILTER_IA64,
		.init = &lzma_simple_ia64_decoder_init,
		.memusage = NULL,
		.props_decode = &lzma_simple_props_decode,
	},
#endif
#ifdef HAVE_DECODER_ARM
	{
		.id = LZMA_FILTER_ARM,
		.init = &lzma_simple_arm_decoder_init,
		.memusage = NULL,
		.props_decode = &lzma_simple_props_decode,
	},
#endif
#ifdef HAVE_DECODER_ARMTHUMB
	{
		.id = LZMA_FILTER_ARMTHUMB,
		.init = &lzma_simple_armthumb_decoder_init,
		.memusage = NULL,
		.props_decode = &lzma_simple_props_decode,
	},
#endif
#ifdef HAVE_DECODER_SPARC
	{
		.id = LZMA_FILTER_SPARC,
		.init = &lzma_simple_sparc_decoder_init,
		.memusage = NULL,
		.props_decode = &lzma_simple_props_decode,
	},
#endif
#ifdef HAVE_DECODER_DELTA
	{
		.id = LZMA_FILTER_DELTA,
		.init = &lzma_delta_decoder_init,
		.memusage = &lzma_delta_coder_memusage,
		.props_decode = &lzma_delta_props_decode,
	},
#endif
};

static const lzma_filter_decoder *
decoder_find(lzma_vli id)
{
	for (size_t i = 0; i < ARRAY_SIZE(decoders); ++i)
		if (decoders[i].id == id)
			return decoders + i;

	return NULL;
}

extern LZMA_API(lzma_bool)
lzma_filter_decoder_is_supported(lzma_vli id)
{
	return decoder_find(id) != NULL;
}

extern lzma_ret
lzma_raw_decoder_init(lzma_next_coder *next, const lzma_allocator *allocator,
		const lzma_filter *options)
{
	return lzma_raw_coder_init(next, allocator,
			options, (lzma_filter_find)(&decoder_find), false);
}

extern LZMA_API(lzma_ret)
lzma_raw_decoder(lzma_stream *strm, const lzma_filter *options)
{
	lzma_next_strm_init(lzma_raw_decoder_init, strm, options);

	strm->internal->supported_actions[LZMA_RUN] = true;
	strm->internal->supported_actions[LZMA_FINISH] = true;

	return LZMA_OK;
}

extern LZMA_API(uint64_t)
lzma_raw_decoder_memusage(const lzma_filter *filters)
{
	return lzma_raw_coder_memusage(
			(lzma_filter_find)(&decoder_find), filters);
}

extern LZMA_API(lzma_ret)
lzma_properties_decode(lzma_filter *filter, const lzma_allocator *allocator,
		const uint8_t *props, size_t props_size)
{
	// Make it always NULL so that the caller can always safely free() it.
	filter->options = NULL;

	const lzma_filter_decoder *const fd = decoder_find(filter->id);
	if (fd == NULL)
		return LZMA_OPTIONS_ERROR;

	if (fd->props_decode == NULL)
		return props_size == 0 ? LZMA_OK : LZMA_OPTIONS_ERROR;

	return fd->props_decode(
			&filter->options, allocator, props, props_size);
}

/*** End of inlined file: filter_decoder.c ***/


/*** Start of inlined file: filter_encoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       filter_decoder.c
/// \brief      Filter ID mapping to filter-specific functions
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "lzma_encoder.h"
#include "lzma2_encoder.h"
#include "simple_encoder.h"
#include "delta_encoder.h"

typedef struct {
	/// Filter ID
	lzma_vli id;

	/// Initializes the filter encoder and calls lzma_next_filter_init()
	/// for filters + 1.
	lzma_init_function init;

	/// Calculates memory usage of the encoder. If the options are
	/// invalid, UINT64_MAX is returned.
	uint64_t (*memusage)(const void *options);

	/// Calculates the recommended Uncompressed Size for .xz Blocks to
	/// which the input data can be split to make multithreaded
	/// encoding possible. If this is NULL, it is assumed that
	/// the encoder is fast enough with single thread.
	uint64_t (*block_size)(const void *options);

	/// Tells the size of the Filter Properties field. If options are
	/// invalid, UINT32_MAX is returned. If this is NULL, props_size_fixed
	/// is used.
	lzma_ret (*props_size_get)(uint32_t *size, const void *options);
	uint32_t props_size_fixed;

	/// Encodes Filter Properties.
	///
	/// \return     - LZMA_OK: Properties encoded successfully.
	///             - LZMA_OPTIONS_ERROR: Unsupported options
	///             - LZMA_PROG_ERROR: Invalid options or not enough
	///               output space
	lzma_ret (*props_encode)(const void *options, uint8_t *out);

} lzma_filter_encoder;

static const lzma_filter_encoder encoders[] = {
#ifdef HAVE_ENCODER_LZMA1
	{
		.id = LZMA_FILTER_LZMA1,
		.init = &lzma_lzma_encoder_init,
		.memusage = &lzma_lzma_encoder_memusage,
		.block_size = NULL, // FIXME
		.props_size_get = NULL,
		.props_size_fixed = 5,
		.props_encode = &lzma_lzma_props_encode,
	},
#endif
#ifdef HAVE_ENCODER_LZMA2
	{
		.id = LZMA_FILTER_LZMA2,
		.init = &lzma_lzma2_encoder_init,
		.memusage = &lzma_lzma2_encoder_memusage,
		.block_size = &lzma_lzma2_block_size, // FIXME
		.props_size_get = NULL,
		.props_size_fixed = 1,
		.props_encode = &lzma_lzma2_props_encode,
	},
#endif
#ifdef HAVE_ENCODER_X86
	{
		.id = LZMA_FILTER_X86,
		.init = &lzma_simple_x86_encoder_init,
		.memusage = NULL,
		.block_size = NULL,
		.props_size_get = &lzma_simple_props_size,
		.props_encode = &lzma_simple_props_encode,
	},
#endif
#ifdef HAVE_ENCODER_POWERPC
	{
		.id = LZMA_FILTER_POWERPC,
		.init = &lzma_simple_powerpc_encoder_init,
		.memusage = NULL,
		.block_size = NULL,
		.props_size_get = &lzma_simple_props_size,
		.props_encode = &lzma_simple_props_encode,
	},
#endif
#ifdef HAVE_ENCODER_IA64
	{
		.id = LZMA_FILTER_IA64,
		.init = &lzma_simple_ia64_encoder_init,
		.memusage = NULL,
		.block_size = NULL,
		.props_size_get = &lzma_simple_props_size,
		.props_encode = &lzma_simple_props_encode,
	},
#endif
#ifdef HAVE_ENCODER_ARM
	{
		.id = LZMA_FILTER_ARM,
		.init = &lzma_simple_arm_encoder_init,
		.memusage = NULL,
		.block_size = NULL,
		.props_size_get = &lzma_simple_props_size,
		.props_encode = &lzma_simple_props_encode,
	},
#endif
#ifdef HAVE_ENCODER_ARMTHUMB
	{
		.id = LZMA_FILTER_ARMTHUMB,
		.init = &lzma_simple_armthumb_encoder_init,
		.memusage = NULL,
		.block_size = NULL,
		.props_size_get = &lzma_simple_props_size,
		.props_encode = &lzma_simple_props_encode,
	},
#endif
#ifdef HAVE_ENCODER_SPARC
	{
		.id = LZMA_FILTER_SPARC,
		.init = &lzma_simple_sparc_encoder_init,
		.memusage = NULL,
		.block_size = NULL,
		.props_size_get = &lzma_simple_props_size,
		.props_encode = &lzma_simple_props_encode,
	},
#endif
#ifdef HAVE_ENCODER_DELTA
	{
		.id = LZMA_FILTER_DELTA,
		.init = &lzma_delta_encoder_init,
		.memusage = &lzma_delta_coder_memusage,
		.block_size = NULL,
		.props_size_get = NULL,
		.props_size_fixed = 1,
		.props_encode = &lzma_delta_props_encode,
	},
#endif
};

static const lzma_filter_encoder *
encoder_find(lzma_vli id)
{
	for (size_t i = 0; i < ARRAY_SIZE(encoders); ++i)
		if (encoders[i].id == id)
			return encoders + i;

	return NULL;
}

extern LZMA_API(lzma_bool)
lzma_filter_encoder_is_supported(lzma_vli id)
{
	return encoder_find(id) != NULL;
}

extern LZMA_API(lzma_ret)
lzma_filters_update(lzma_stream *strm, const lzma_filter *filters)
{
	if (strm->internal->next.update == NULL)
		return LZMA_PROG_ERROR;

	// Validate the filter chain.
	if (lzma_raw_encoder_memusage(filters) == UINT64_MAX)
		return LZMA_OPTIONS_ERROR;

	// The actual filter chain in the encoder is reversed. Some things
	// still want the normal order chain, so we provide both.
	size_t count = 1;
	while (filters[count].id != LZMA_VLI_UNKNOWN)
		++count;

	lzma_filter reversed_filters[LZMA_FILTERS_MAX + 1];
	for (size_t i = 0; i < count; ++i)
		reversed_filters[count - i - 1] = filters[i];

	reversed_filters[count].id = LZMA_VLI_UNKNOWN;

	return strm->internal->next.update(strm->internal->next.coder,
			strm->allocator, filters, reversed_filters);
}

extern lzma_ret
lzma_raw_encoder_init(lzma_next_coder *next, const lzma_allocator *allocator,
		const lzma_filter *options)
{
	return lzma_raw_coder_init(next, allocator,
			options, (lzma_filter_find)(&encoder_find), true);
}

extern LZMA_API(lzma_ret)
lzma_raw_encoder(lzma_stream *strm, const lzma_filter *options)
{
	lzma_next_strm_init(lzma_raw_coder_init, strm, options,
			(lzma_filter_find)(&encoder_find), true);

	strm->internal->supported_actions[LZMA_RUN] = true;
	strm->internal->supported_actions[LZMA_SYNC_FLUSH] = true;
	strm->internal->supported_actions[LZMA_FINISH] = true;

	return LZMA_OK;
}

extern LZMA_API(uint64_t)
lzma_raw_encoder_memusage(const lzma_filter *filters)
{
	return lzma_raw_coder_memusage(
			(lzma_filter_find)(&encoder_find), filters);
}

extern uint64_t
lzma_mt_block_size(const lzma_filter *filters)
{
	uint64_t max = 0;

	for (size_t i = 0; filters[i].id != LZMA_VLI_UNKNOWN; ++i) {
		const lzma_filter_encoder *const fe
				= encoder_find(filters[i].id);
		if (fe->block_size != NULL) {
			const uint64_t size
					= fe->block_size(filters[i].options);
			if (size == 0)
				return 0;

			if (size > max)
				max = size;
		}
	}

	return max;
}

extern LZMA_API(lzma_ret)
lzma_properties_size(uint32_t *size, const lzma_filter *filter)
{
	const lzma_filter_encoder *const fe = encoder_find(filter->id);
	if (fe == NULL) {
		// Unknown filter - if the Filter ID is a proper VLI,
		// return LZMA_OPTIONS_ERROR instead of LZMA_PROG_ERROR,
		// because it's possible that we just don't have support
		// compiled in for the requested filter.
		return filter->id <= LZMA_VLI_MAX
				? LZMA_OPTIONS_ERROR : LZMA_PROG_ERROR;
	}

	if (fe->props_size_get == NULL) {
		// No props_size_get() function, use props_size_fixed.
		*size = fe->props_size_fixed;
		return LZMA_OK;
	}

	return fe->props_size_get(size, filter->options);
}

extern LZMA_API(lzma_ret)
lzma_properties_encode(const lzma_filter *filter, uint8_t *props)
{
	const lzma_filter_encoder *const fe = encoder_find(filter->id);
	if (fe == NULL)
		return LZMA_PROG_ERROR;

	if (fe->props_encode == NULL)
		return LZMA_OK;

	return fe->props_encode(filter->options, props);
}

/*** End of inlined file: filter_encoder.c ***/


/*** Start of inlined file: filter_flags_decoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       filter_flags_decoder.c
/// \brief      Decodes a Filter Flags field
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

extern LZMA_API(lzma_ret)
lzma_filter_flags_decode(
		lzma_filter *filter, const lzma_allocator *allocator,
		const uint8_t *in, size_t *in_pos, size_t in_size)
{
	// Set the pointer to NULL so the caller can always safely free it.
	filter->options = NULL;

	// Filter ID
	return_if_error(lzma_vli_decode(&filter->id, NULL,
			in, in_pos, in_size));

	if (filter->id >= LZMA_FILTER_RESERVED_START)
		return LZMA_DATA_ERROR;

	// Size of Properties
	lzma_vli props_size;
	return_if_error(lzma_vli_decode(&props_size, NULL,
			in, in_pos, in_size));

	// Filter Properties
	if (in_size - *in_pos < props_size)
		return LZMA_DATA_ERROR;

	const lzma_ret ret = lzma_properties_decode(
			filter, allocator, in + *in_pos, props_size);

	*in_pos += props_size;

	return ret;
}

/*** End of inlined file: filter_flags_decoder.c ***/


/*** Start of inlined file: filter_flags_encoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       filter_flags_encoder.c
/// \brief      Encodes a Filter Flags field
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

extern LZMA_API(lzma_ret)
lzma_filter_flags_size(uint32_t *size, const lzma_filter *filter)
{
	if (filter->id >= LZMA_FILTER_RESERVED_START)
		return LZMA_PROG_ERROR;

	return_if_error(lzma_properties_size(size, filter));

	*size += lzma_vli_size(filter->id) + lzma_vli_size(*size);

	return LZMA_OK;
}

extern LZMA_API(lzma_ret)
lzma_filter_flags_encode(const lzma_filter *filter,
		uint8_t *out, size_t *out_pos, size_t out_size)
{
	// Filter ID
	if (filter->id >= LZMA_FILTER_RESERVED_START)
		return LZMA_PROG_ERROR;

	return_if_error(lzma_vli_encode(filter->id, NULL,
			out, out_pos, out_size));

	// Size of Properties
	uint32_t props_size;
	return_if_error(lzma_properties_size(&props_size, filter));
	return_if_error(lzma_vli_encode(props_size, NULL,
			out, out_pos, out_size));

	// Filter Properties
	if (out_size - *out_pos < props_size)
		return LZMA_PROG_ERROR;

	return_if_error(lzma_properties_encode(filter, out + *out_pos));

	*out_pos += props_size;

	return LZMA_OK;
}

/*** End of inlined file: filter_flags_encoder.c ***/


/*** Start of inlined file: hardware_cputhreads.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       hardware_cputhreads.c
/// \brief      Get the number of CPU threads or cores
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "tuklib_cpucores.h"

extern LZMA_API(uint32_t)
lzma_cputhreads(void)
{
	return tuklib_cpucores();
}

/*** End of inlined file: hardware_cputhreads.c ***/


/*** Start of inlined file: hardware_physmem.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       hardware_physmem.c
/// \brief      Get the total amount of physical memory (RAM)
//
//  Author:     Jonathan Nieder
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "tuklib_physmem.h"

extern LZMA_API(uint64_t)
lzma_physmem(void)
{
	// It is simpler to make lzma_physmem() a wrapper for
	// tuklib_physmem() than to hack appropriate symbol visibility
	// support for the tuklib modules.
	return tuklib_physmem();
}

/*** End of inlined file: hardware_physmem.c ***/


/*** Start of inlined file: index.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       index.c
/// \brief      Handling of .xz Indexes and some other Stream information
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

/// \brief      How many Records to allocate at once
///
/// This should be big enough to avoid making lots of tiny allocations
/// but small enough to avoid too much unused memory at once.
#define INDEX_GROUP_SIZE 512

/// \brief      How many Records can be allocated at once at maximum
#define PREALLOC_MAX ((SIZE_MAX - sizeof(index_group)) / sizeof(index_record))

/// \brief      Base structure for index_stream and index_group structures
typedef struct index_tree_node_s index_tree_node;
struct index_tree_node_s {
	/// Uncompressed start offset of this Stream (relative to the
	/// beginning of the file) or Block (relative to the beginning
	/// of the Stream)
	lzma_vli uncompressed_base;

	/// Compressed start offset of this Stream or Block
	lzma_vli compressed_base;

	index_tree_node *parent;
	index_tree_node *left;
	index_tree_node *right;
};

/// \brief      AVL tree to hold index_stream or index_group structures
typedef struct {
	/// Root node
	index_tree_node *root;

	/// Leftmost node. Since the tree will be filled sequentially,
	/// this won't change after the first node has been added to
	/// the tree.
	index_tree_node *leftmost;

	/// The rightmost node in the tree. Since the tree is filled
	/// sequentially, this is always the node where to add the new data.
	index_tree_node *rightmost;

	/// Number of nodes in the tree
	uint32_t count;

} index_tree;

typedef struct {
	lzma_vli uncompressed_sum;
	lzma_vli unpadded_sum;
} index_record;

typedef struct {
	/// Every Record group is part of index_stream.groups tree.
	index_tree_node node;

	/// Number of Blocks in this Stream before this group.
	lzma_vli number_base;

	/// Number of Records that can be put in records[].
	size_t allocated;

	/// Index of the last Record in use.
	size_t last;

	/// The sizes in this array are stored as cumulative sums relative
	/// to the beginning of the Stream. This makes it possible to
	/// use binary search in lzma_index_locate().
	///
	/// Note that the cumulative summing is done specially for
	/// unpadded_sum: The previous value is rounded up to the next
	/// multiple of four before adding the Unpadded Size of the new
	/// Block. The total encoded size of the Blocks in the Stream
	/// is records[last].unpadded_sum in the last Record group of
	/// the Stream.
	///
	/// For example, if the Unpadded Sizes are 39, 57, and 81, the
	/// stored values are 39, 97 (40 + 57), and 181 (100 + 181).
	/// The total encoded size of these Blocks is 184.
	///
	/// This is a flexible array, because it makes easy to optimize
	/// memory usage in case someone concatenates many Streams that
	/// have only one or few Blocks.
	index_record records[];

} index_group;

typedef struct {
	/// Every index_stream is a node in the tree of Streams.
	index_tree_node node;

	/// Number of this Stream (first one is 1)
	uint32_t number;

	/// Total number of Blocks before this Stream
	lzma_vli block_number_base;

	/// Record groups of this Stream are stored in a tree.
	/// It's a T-tree with AVL-tree balancing. There are
	/// INDEX_GROUP_SIZE Records per node by default.
	/// This keeps the number of memory allocations reasonable
	/// and finding a Record is fast.
	index_tree groups;

	/// Number of Records in this Stream
	lzma_vli record_count;

	/// Size of the List of Records field in this Stream. This is used
	/// together with record_count to calculate the size of the Index
	/// field and thus the total size of the Stream.
	lzma_vli index_list_size;

	/// Stream Flags of this Stream. This is meaningful only if
	/// the Stream Flags have been told us with lzma_index_stream_flags().
	/// Initially stream_flags.version is set to UINT32_MAX to indicate
	/// that the Stream Flags are unknown.
	lzma_stream_flags stream_flags;

	/// Amount of Stream Padding after this Stream. This defaults to
	/// zero and can be set with lzma_index_stream_padding().
	lzma_vli stream_padding;

} index_stream;

struct lzma_index_s {
	/// AVL-tree containing the Stream(s). Often there is just one
	/// Stream, but using a tree keeps lookups fast even when there
	/// are many concatenated Streams.
	index_tree streams;

	/// Uncompressed size of all the Blocks in the Stream(s)
	lzma_vli uncompressed_size;

	/// Total size of all the Blocks in the Stream(s)
	lzma_vli total_size;

	/// Total number of Records in all Streams in this lzma_index
	lzma_vli record_count;

	/// Size of the List of Records field if all the Streams in this
	/// lzma_index were packed into a single Stream (makes it simpler to
	/// take many .xz files and combine them into a single Stream).
	///
	/// This value together with record_count is needed to calculate
	/// Backward Size that is stored into Stream Footer.
	lzma_vli index_list_size;

	/// How many Records to allocate at once in lzma_index_append().
	/// This defaults to INDEX_GROUP_SIZE but can be overridden with
	/// lzma_index_prealloc().
	size_t prealloc;

	/// Bitmask indicating what integrity check types have been used
	/// as set by lzma_index_stream_flags(). The bit of the last Stream
	/// is not included here, since it is possible to change it by
	/// calling lzma_index_stream_flags() again.
	uint32_t checks;
};

static void
index_tree_init(index_tree *tree)
{
	tree->root = NULL;
	tree->leftmost = NULL;
	tree->rightmost = NULL;
	tree->count = 0;
	return;
}

/// Helper for index_tree_end()
static void
index_tree_node_end(index_tree_node *node, const lzma_allocator *allocator,
		void (*free_func)(void *node, const lzma_allocator *allocator))
{
	// The tree won't ever be very huge, so recursion should be fine.
	// 20 levels in the tree is likely quite a lot already in practice.
	if (node->left != NULL)
		index_tree_node_end(node->left, allocator, free_func);

	if (node->right != NULL)
		index_tree_node_end(node->right, allocator, free_func);

	free_func(node, allocator);
	return;
}

/// Free the memory allocated for a tree. Each node is freed using the
/// given free_func which is either &lzma_free or &index_stream_end.
/// The latter is used to free the Record groups from each index_stream
/// before freeing the index_stream itself.
static void
index_tree_end(index_tree *tree, const lzma_allocator *allocator,
		void (*free_func)(void *node, const lzma_allocator *allocator))
{
	assert(free_func != NULL);

	if (tree->root != NULL)
		index_tree_node_end(tree->root, allocator, free_func);

	return;
}

/// Add a new node to the tree. node->uncompressed_base and
/// node->compressed_base must have been set by the caller already.
static void
index_tree_append(index_tree *tree, index_tree_node *node)
{
	node->parent = tree->rightmost;
	node->left = NULL;
	node->right = NULL;

	++tree->count;

	// Handle the special case of adding the first node.
	if (tree->root == NULL) {
		tree->root = node;
		tree->leftmost = node;
		tree->rightmost = node;
		return;
	}

	// The tree is always filled sequentially.
	assert(tree->rightmost->uncompressed_base <= node->uncompressed_base);
	assert(tree->rightmost->compressed_base < node->compressed_base);

	// Add the new node after the rightmost node. It's the correct
	// place due to the reason above.
	tree->rightmost->right = node;
	tree->rightmost = node;

	// Balance the AVL-tree if needed. We don't need to keep the balance
	// factors in nodes, because we always fill the tree sequentially,
	// and thus know the state of the tree just by looking at the node
	// count. From the node count we can calculate how many steps to go
	// up in the tree to find the rotation root.
	uint32_t up = tree->count ^ (UINT32_C(1) << bsr32(tree->count));
	if (up != 0) {
		// Locate the root node for the rotation.
		up = ctz32(tree->count) + 2;
		do {
			node = node->parent;
		} while (--up > 0);

		// Rotate left using node as the rotation root.
		index_tree_node *pivot = node->right;

		if (node->parent == NULL) {
			tree->root = pivot;
		} else {
			assert(node->parent->right == node);
			node->parent->right = pivot;
		}

		pivot->parent = node->parent;

		node->right = pivot->left;
		if (node->right != NULL)
			node->right->parent = node;

		pivot->left = node;
		node->parent = pivot;
	}

	return;
}

/// Get the next node in the tree. Return NULL if there are no more nodes.
static void *
index_tree_next(const index_tree_node *node)
{
	if (node->right != NULL) {
		node = node->right;
		while (node->left != NULL)
			node = node->left;

		return (void *)(node);
	}

	while (node->parent != NULL && node->parent->right == node)
		node = node->parent;

	return (void *)(node->parent);
}

/// Locate a node that contains the given uncompressed offset. It is
/// caller's job to check that target is not bigger than the uncompressed
/// size of the tree (the last node would be returned in that case still).
static void *
index_tree_locate(const index_tree *tree, lzma_vli target)
{
	const index_tree_node *result = NULL;
	const index_tree_node *node = tree->root;

	assert(tree->leftmost == NULL
			|| tree->leftmost->uncompressed_base == 0);

	// Consecutive nodes may have the same uncompressed_base.
	// We must pick the rightmost one.
	while (node != NULL) {
		if (node->uncompressed_base > target) {
			node = node->left;
		} else {
			result = node;
			node = node->right;
		}
	}

	return (void *)(result);
}

/// Allocate and initialize a new Stream using the given base offsets.
static index_stream *
index_stream_init(lzma_vli compressed_base, lzma_vli uncompressed_base,
		uint32_t stream_number, lzma_vli block_number_base,
		const lzma_allocator *allocator)
{
	index_stream *s = lzma_alloc(sizeof(index_stream), allocator);
	if (s == NULL)
		return NULL;

	s->node.uncompressed_base = uncompressed_base;
	s->node.compressed_base = compressed_base;
	s->node.parent = NULL;
	s->node.left = NULL;
	s->node.right = NULL;

	s->number = stream_number;
	s->block_number_base = block_number_base;

	index_tree_init(&s->groups);

	s->record_count = 0;
	s->index_list_size = 0;
	s->stream_flags.version = UINT32_MAX;
	s->stream_padding = 0;

	return s;
}

/// Free the memory allocated for a Stream and its Record groups.
static void
index_stream_end(void *node, const lzma_allocator *allocator)
{
	index_stream *s = node;
	index_tree_end(&s->groups, allocator, &lzma_free);
	lzma_free(s, allocator);
	return;
}

static lzma_index *
index_init_plain(const lzma_allocator *allocator)
{
	lzma_index *i = lzma_alloc(sizeof(lzma_index), allocator);
	if (i != NULL) {
		index_tree_init(&i->streams);
		i->uncompressed_size = 0;
		i->total_size = 0;
		i->record_count = 0;
		i->index_list_size = 0;
		i->prealloc = INDEX_GROUP_SIZE;
		i->checks = 0;
	}

	return i;
}

extern LZMA_API(lzma_index *)
lzma_index_init(const lzma_allocator *allocator)
{
	lzma_index *i = index_init_plain(allocator);
	if (i == NULL)
		return NULL;

	index_stream *s = index_stream_init(0, 0, 1, 0, allocator);
	if (s == NULL) {
		lzma_free(i, allocator);
		return NULL;
	}

	index_tree_append(&i->streams, &s->node);

	return i;
}

extern LZMA_API(void)
lzma_index_end(lzma_index *i, const lzma_allocator *allocator)
{
	// NOTE: If you modify this function, check also the bottom
	// of lzma_index_cat().
	if (i != NULL) {
		index_tree_end(&i->streams, allocator, &index_stream_end);
		lzma_free(i, allocator);
	}

	return;
}

extern void
lzma_index_prealloc(lzma_index *i, lzma_vli records)
{
	if (records > PREALLOC_MAX)
		records = PREALLOC_MAX;

	i->prealloc = (size_t)(records);
	return;
}

extern LZMA_API(uint64_t)
lzma_index_memusage(lzma_vli streams, lzma_vli blocks)
{
	// This calculates an upper bound that is only a little bit
	// bigger than the exact maximum memory usage with the given
	// parameters.

	// Typical malloc() overhead is 2 * sizeof(void *) but we take
	// a little bit extra just in case. Using LZMA_MEMUSAGE_BASE
	// instead would give too inaccurate estimate.
	const size_t alloc_overhead = 4 * sizeof(void *);

	// Amount of memory needed for each Stream base structures.
	// We assume that every Stream has at least one Block and
	// thus at least one group.
	const size_t stream_base = sizeof(index_stream)
			+ sizeof(index_group) + 2 * alloc_overhead;

	// Amount of memory needed per group.
	const size_t group_base = sizeof(index_group)
			+ INDEX_GROUP_SIZE * sizeof(index_record)
			+ alloc_overhead;

	// Number of groups. There may actually be more, but that overhead
	// has been taken into account in stream_base already.
	const lzma_vli groups
			= (blocks + INDEX_GROUP_SIZE - 1) / INDEX_GROUP_SIZE;

	// Memory used by index_stream and index_group structures.
	const uint64_t streams_mem = streams * stream_base;
	const uint64_t groups_mem = groups * group_base;

	// Memory used by the base structure.
	const uint64_t index_base = sizeof(lzma_index) + alloc_overhead;

	// Validate the arguments and catch integer overflows.
	// Maximum number of Streams is "only" UINT32_MAX, because
	// that limit is used by the tree containing the Streams.
	const uint64_t limit = UINT64_MAX - index_base;
	if (streams == 0 || streams > UINT32_MAX || blocks > LZMA_VLI_MAX
			|| streams > limit / stream_base
			|| groups > limit / group_base
			|| limit - streams_mem < groups_mem)
		return UINT64_MAX;

	return index_base + streams_mem + groups_mem;
}

extern LZMA_API(uint64_t)
lzma_index_memused(const lzma_index *i)
{
	return lzma_index_memusage(i->streams.count, i->record_count);
}

extern LZMA_API(lzma_vli)
lzma_index_block_count(const lzma_index *i)
{
	return i->record_count;
}

extern LZMA_API(lzma_vli)
lzma_index_stream_count(const lzma_index *i)
{
	return i->streams.count;
}

extern LZMA_API(lzma_vli)
lzma_index_size(const lzma_index *i)
{
	return index_size(i->record_count, i->index_list_size);
}

extern LZMA_API(lzma_vli)
lzma_index_total_size(const lzma_index *i)
{
	return i->total_size;
}

extern LZMA_API(lzma_vli)
lzma_index_stream_size(const lzma_index *i)
{
	// Stream Header + Blocks + Index + Stream Footer
	return LZMA_STREAM_HEADER_SIZE + i->total_size
			+ index_size(i->record_count, i->index_list_size)
			+ LZMA_STREAM_HEADER_SIZE;
}

static lzma_vli
index_file_size(lzma_vli compressed_base, lzma_vli unpadded_sum,
		lzma_vli record_count, lzma_vli index_list_size,
		lzma_vli stream_padding)
{
	// Earlier Streams and Stream Paddings + Stream Header
	// + Blocks + Index + Stream Footer + Stream Padding
	//
	// This might go over LZMA_VLI_MAX due to too big unpadded_sum
	// when this function is used in lzma_index_append().
	lzma_vli file_size = compressed_base + 2 * LZMA_STREAM_HEADER_SIZE
			+ stream_padding + vli_ceil4(unpadded_sum);
	if (file_size > LZMA_VLI_MAX)
		return LZMA_VLI_UNKNOWN;

	// The same applies here.
	file_size += index_size(record_count, index_list_size);
	if (file_size > LZMA_VLI_MAX)
		return LZMA_VLI_UNKNOWN;

	return file_size;
}

extern LZMA_API(lzma_vli)
lzma_index_file_size(const lzma_index *i)
{
	const index_stream *s = (const index_stream *)(i->streams.rightmost);
	const index_group *g = (const index_group *)(s->groups.rightmost);
	return index_file_size(s->node.compressed_base,
			g == NULL ? 0 : g->records[g->last].unpadded_sum,
			s->record_count, s->index_list_size,
			s->stream_padding);
}

extern LZMA_API(lzma_vli)
lzma_index_uncompressed_size(const lzma_index *i)
{
	return i->uncompressed_size;
}

extern LZMA_API(uint32_t)
lzma_index_checks(const lzma_index *i)
{
	uint32_t checks = i->checks;

	// Get the type of the Check of the last Stream too.
	const index_stream *s = (const index_stream *)(i->streams.rightmost);
	if (s->stream_flags.version != UINT32_MAX)
		checks |= UINT32_C(1) << s->stream_flags.check;

	return checks;
}

extern uint32_t
lzma_index_padding_size(const lzma_index *i)
{
	return (LZMA_VLI_C(4) - index_size_unpadded(
			i->record_count, i->index_list_size)) & 3;
}

extern LZMA_API(lzma_ret)
lzma_index_stream_flags(lzma_index *i, const lzma_stream_flags *stream_flags)
{
	if (i == NULL || stream_flags == NULL)
		return LZMA_PROG_ERROR;

	// Validate the Stream Flags.
	return_if_error(lzma_stream_flags_compare(
			stream_flags, stream_flags));

	index_stream *s = (index_stream *)(i->streams.rightmost);
	s->stream_flags = *stream_flags;

	return LZMA_OK;
}

extern LZMA_API(lzma_ret)
lzma_index_stream_padding(lzma_index *i, lzma_vli stream_padding)
{
	if (i == NULL || stream_padding > LZMA_VLI_MAX
			|| (stream_padding & 3) != 0)
		return LZMA_PROG_ERROR;

	index_stream *s = (index_stream *)(i->streams.rightmost);

	// Check that the new value won't make the file grow too big.
	const lzma_vli old_stream_padding = s->stream_padding;
	s->stream_padding = 0;
	if (lzma_index_file_size(i) + stream_padding > LZMA_VLI_MAX) {
		s->stream_padding = old_stream_padding;
		return LZMA_DATA_ERROR;
	}

	s->stream_padding = stream_padding;
	return LZMA_OK;
}

extern LZMA_API(lzma_ret)
lzma_index_append(lzma_index *i, const lzma_allocator *allocator,
		lzma_vli unpadded_size, lzma_vli uncompressed_size)
{
	// Validate.
	if (i == NULL || unpadded_size < UNPADDED_SIZE_MIN
			|| unpadded_size > UNPADDED_SIZE_MAX
			|| uncompressed_size > LZMA_VLI_MAX)
		return LZMA_PROG_ERROR;

	index_stream *s = (index_stream *)(i->streams.rightmost);
	index_group *g = (index_group *)(s->groups.rightmost);

	const lzma_vli compressed_base = g == NULL ? 0
			: vli_ceil4(g->records[g->last].unpadded_sum);
	const lzma_vli uncompressed_base = g == NULL ? 0
			: g->records[g->last].uncompressed_sum;
	const uint32_t index_list_size_add = lzma_vli_size(unpadded_size)
			+ lzma_vli_size(uncompressed_size);

	// Check that the file size will stay within limits.
	if (index_file_size(s->node.compressed_base,
			compressed_base + unpadded_size, s->record_count + 1,
			s->index_list_size + index_list_size_add,
			s->stream_padding) == LZMA_VLI_UNKNOWN)
		return LZMA_DATA_ERROR;

	// The size of the Index field must not exceed the maximum value
	// that can be stored in the Backward Size field.
	if (index_size(i->record_count + 1,
			i->index_list_size + index_list_size_add)
			> LZMA_BACKWARD_SIZE_MAX)
		return LZMA_DATA_ERROR;

	if (g != NULL && g->last + 1 < g->allocated) {
		// There is space in the last group at least for one Record.
		++g->last;
	} else {
		// We need to allocate a new group.
		g = lzma_alloc(sizeof(index_group)
				+ i->prealloc * sizeof(index_record),
				allocator);
		if (g == NULL)
			return LZMA_MEM_ERROR;

		g->last = 0;
		g->allocated = i->prealloc;

		// Reset prealloc so that if the application happens to
		// add new Records, the allocation size will be sane.
		i->prealloc = INDEX_GROUP_SIZE;

		// Set the start offsets of this group.
		g->node.uncompressed_base = uncompressed_base;
		g->node.compressed_base = compressed_base;
		g->number_base = s->record_count + 1;

		// Add the new group to the Stream.
		index_tree_append(&s->groups, &g->node);
	}

	// Add the new Record to the group.
	g->records[g->last].uncompressed_sum
			= uncompressed_base + uncompressed_size;
	g->records[g->last].unpadded_sum
			= compressed_base + unpadded_size;

	// Update the totals.
	++s->record_count;
	s->index_list_size += index_list_size_add;

	i->total_size += vli_ceil4(unpadded_size);
	i->uncompressed_size += uncompressed_size;
	++i->record_count;
	i->index_list_size += index_list_size_add;

	return LZMA_OK;
}

/// Structure to pass info to index_cat_helper()
typedef struct {
	/// Uncompressed size of the destination
	lzma_vli uncompressed_size;

	/// Compressed file size of the destination
	lzma_vli file_size;

	/// Same as above but for Block numbers
	lzma_vli block_number_add;

	/// Number of Streams that were in the destination index before we
	/// started appending new Streams from the source index. This is
	/// used to fix the Stream numbering.
	uint32_t stream_number_add;

	/// Destination index' Stream tree
	index_tree *streams;

} index_cat_info;

/// Add the Stream nodes from the source index to dest using recursion.
/// Simplest iterative traversal of the source tree wouldn't work, because
/// we update the pointers in nodes when moving them to the destination tree.
static void
index_cat_helper(const index_cat_info *info, index_stream *this)
{
	index_stream *left = (index_stream *)(this->node.left);
	index_stream *right = (index_stream *)(this->node.right);

	if (left != NULL)
		index_cat_helper(info, left);

	this->node.uncompressed_base += info->uncompressed_size;
	this->node.compressed_base += info->file_size;
	this->number += info->stream_number_add;
	this->block_number_base += info->block_number_add;
	index_tree_append(info->streams, &this->node);

	if (right != NULL)
		index_cat_helper(info, right);

	return;
}

extern LZMA_API(lzma_ret)
lzma_index_cat(lzma_index *restrict dest, lzma_index *restrict src,
		const lzma_allocator *allocator)
{
	const lzma_vli dest_file_size = lzma_index_file_size(dest);

	// Check that we don't exceed the file size limits.
	if (dest_file_size + lzma_index_file_size(src) > LZMA_VLI_MAX
			|| dest->uncompressed_size + src->uncompressed_size
				> LZMA_VLI_MAX)
		return LZMA_DATA_ERROR;

	// Check that the encoded size of the combined lzma_indexes stays
	// within limits. In theory, this should be done only if we know
	// that the user plans to actually combine the Streams and thus
	// construct a single Index (probably rare). However, exceeding
	// this limit is quite theoretical, so we do this check always
	// to simplify things elsewhere.
	{
		const lzma_vli dest_size = index_size_unpadded(
				dest->record_count, dest->index_list_size);
		const lzma_vli src_size = index_size_unpadded(
				src->record_count, src->index_list_size);
		if (vli_ceil4(dest_size + src_size) > LZMA_BACKWARD_SIZE_MAX)
			return LZMA_DATA_ERROR;
	}

	// Optimize the last group to minimize memory usage. Allocation has
	// to be done before modifying dest or src.
	{
		index_stream *s = (index_stream *)(dest->streams.rightmost);
		index_group *g = (index_group *)(s->groups.rightmost);
		if (g != NULL && g->last + 1 < g->allocated) {
			assert(g->node.left == NULL);
			assert(g->node.right == NULL);

			index_group *newg = lzma_alloc(sizeof(index_group)
					+ (g->last + 1)
					* sizeof(index_record),
					allocator);
			if (newg == NULL)
				return LZMA_MEM_ERROR;

			newg->node = g->node;
			newg->allocated = g->last + 1;
			newg->last = g->last;
			newg->number_base = g->number_base;

			memcpy(newg->records, g->records, newg->allocated
					* sizeof(index_record));

			if (g->node.parent != NULL) {
				assert(g->node.parent->right == &g->node);
				g->node.parent->right = &newg->node;
			}

			if (s->groups.leftmost == &g->node) {
				assert(s->groups.root == &g->node);
				s->groups.leftmost = &newg->node;
				s->groups.root = &newg->node;
			}

			assert(s->groups.rightmost == &g->node);
			s->groups.rightmost = &newg->node;

			lzma_free(g, allocator);

			// NOTE: newg isn't leaked here because
			// newg == (void *)&newg->node.
		}
	}

	// Add all the Streams from src to dest. Update the base offsets
	// of each Stream from src.
	const index_cat_info info = {
		.uncompressed_size = dest->uncompressed_size,
		.file_size = dest_file_size,
		.stream_number_add = dest->streams.count,
		.block_number_add = dest->record_count,
		.streams = &dest->streams,
	};
	index_cat_helper(&info, (index_stream *)(src->streams.root));

	// Update info about all the combined Streams.
	dest->uncompressed_size += src->uncompressed_size;
	dest->total_size += src->total_size;
	dest->record_count += src->record_count;
	dest->index_list_size += src->index_list_size;
	dest->checks = lzma_index_checks(dest) | src->checks;

	// There's nothing else left in src than the base structure.
	lzma_free(src, allocator);

	return LZMA_OK;
}

/// Duplicate an index_stream.
static index_stream *
index_dup_stream(const index_stream *src, const lzma_allocator *allocator)
{
	// Catch a somewhat theoretical integer overflow.
	if (src->record_count > PREALLOC_MAX)
		return NULL;

	// Allocate and initialize a new Stream.
	index_stream *dest = index_stream_init(src->node.compressed_base,
			src->node.uncompressed_base, src->number,
			src->block_number_base, allocator);
	if (dest == NULL)
		return NULL;

	// Copy the overall information.
	dest->record_count = src->record_count;
	dest->index_list_size = src->index_list_size;
	dest->stream_flags = src->stream_flags;
	dest->stream_padding = src->stream_padding;

	// Return if there are no groups to duplicate.
	if (src->groups.leftmost == NULL)
		return dest;

	// Allocate memory for the Records. We put all the Records into
	// a single group. It's simplest and also tends to make
	// lzma_index_locate() a little bit faster with very big Indexes.
	index_group *destg = lzma_alloc(sizeof(index_group)
			+ src->record_count * sizeof(index_record),
			allocator);
	if (destg == NULL) {
		index_stream_end(dest, allocator);
		return NULL;
	}

	// Initialize destg.
	destg->node.uncompressed_base = 0;
	destg->node.compressed_base = 0;
	destg->number_base = 1;
	destg->allocated = src->record_count;
	destg->last = src->record_count - 1;

	// Go through all the groups in src and copy the Records into destg.
	const index_group *srcg = (const index_group *)(src->groups.leftmost);
	size_t i = 0;
	do {
		memcpy(destg->records + i, srcg->records,
				(srcg->last + 1) * sizeof(index_record));
		i += srcg->last + 1;
		srcg = index_tree_next(&srcg->node);
	} while (srcg != NULL);

	assert(i == destg->allocated);

	// Add the group to the new Stream.
	index_tree_append(&dest->groups, &destg->node);

	return dest;
}

extern LZMA_API(lzma_index *)
lzma_index_dup(const lzma_index *src, const lzma_allocator *allocator)
{
	// Allocate the base structure (no initial Stream).
	lzma_index *dest = index_init_plain(allocator);
	if (dest == NULL)
		return NULL;

	// Copy the totals.
	dest->uncompressed_size = src->uncompressed_size;
	dest->total_size = src->total_size;
	dest->record_count = src->record_count;
	dest->index_list_size = src->index_list_size;

	// Copy the Streams and the groups in them.
	const index_stream *srcstream
			= (const index_stream *)(src->streams.leftmost);
	do {
		index_stream *deststream = index_dup_stream(
				srcstream, allocator);
		if (deststream == NULL) {
			lzma_index_end(dest, allocator);
			return NULL;
		}

		index_tree_append(&dest->streams, &deststream->node);

		srcstream = index_tree_next(&srcstream->node);
	} while (srcstream != NULL);

	return dest;
}

/// Indexing for lzma_index_iter.internal[]
enum {
	ITER_INDEX,
	ITER_STREAM,
	ITER_GROUP,
	ITER_RECORD,
	ITER_METHOD,
};

/// Values for lzma_index_iter.internal[ITER_METHOD].s
enum {
	ITER_METHOD_NORMAL,
	ITER_METHOD_NEXT,
	ITER_METHOD_LEFTMOST,
};

static void
iter_set_info(lzma_index_iter *iter)
{
	const lzma_index *i = iter->internal[ITER_INDEX].p;
	const index_stream *stream = iter->internal[ITER_STREAM].p;
	const index_group *group = iter->internal[ITER_GROUP].p;
	const size_t record = iter->internal[ITER_RECORD].s;

	// lzma_index_iter.internal must not contain a pointer to the last
	// group in the index, because that may be reallocated by
	// lzma_index_cat().
	if (group == NULL) {
		// There are no groups.
		assert(stream->groups.root == NULL);
		iter->internal[ITER_METHOD].s = ITER_METHOD_LEFTMOST;

	} else if (i->streams.rightmost != &stream->node
			|| stream->groups.rightmost != &group->node) {
		// The group is not not the last group in the index.
		iter->internal[ITER_METHOD].s = ITER_METHOD_NORMAL;

	} else if (stream->groups.leftmost != &group->node) {
		// The group isn't the only group in the Stream, thus we
		// know that it must have a parent group i.e. it's not
		// the root node.
		assert(stream->groups.root != &group->node);
		assert(group->node.parent->right == &group->node);
		iter->internal[ITER_METHOD].s = ITER_METHOD_NEXT;
		iter->internal[ITER_GROUP].p = group->node.parent;

	} else {
		// The Stream has only one group.
		assert(stream->groups.root == &group->node);
		assert(group->node.parent == NULL);
		iter->internal[ITER_METHOD].s = ITER_METHOD_LEFTMOST;
		iter->internal[ITER_GROUP].p = NULL;
	}

	// NOTE: lzma_index_iter.stream.number is lzma_vli but we use uint32_t
	// internally.
	iter->stream.number = stream->number;
	iter->stream.block_count = stream->record_count;
	iter->stream.compressed_offset = stream->node.compressed_base;
	iter->stream.uncompressed_offset = stream->node.uncompressed_base;

	// iter->stream.flags will be NULL if the Stream Flags haven't been
	// set with lzma_index_stream_flags().
	iter->stream.flags = stream->stream_flags.version == UINT32_MAX
			? NULL : &stream->stream_flags;
	iter->stream.padding = stream->stream_padding;

	if (stream->groups.rightmost == NULL) {
		// Stream has no Blocks.
		iter->stream.compressed_size = index_size(0, 0)
				+ 2 * LZMA_STREAM_HEADER_SIZE;
		iter->stream.uncompressed_size = 0;
	} else {
		const index_group *g = (const index_group *)(
				stream->groups.rightmost);

		// Stream Header + Stream Footer + Index + Blocks
		iter->stream.compressed_size = 2 * LZMA_STREAM_HEADER_SIZE
				+ index_size(stream->record_count,
					stream->index_list_size)
				+ vli_ceil4(g->records[g->last].unpadded_sum);
		iter->stream.uncompressed_size
				= g->records[g->last].uncompressed_sum;
	}

	if (group != NULL) {
		iter->block.number_in_stream = group->number_base + record;
		iter->block.number_in_file = iter->block.number_in_stream
				+ stream->block_number_base;

		iter->block.compressed_stream_offset
				= record == 0 ? group->node.compressed_base
				: vli_ceil4(group->records[
					record - 1].unpadded_sum);
		iter->block.uncompressed_stream_offset
				= record == 0 ? group->node.uncompressed_base
				: group->records[record - 1].uncompressed_sum;

		iter->block.uncompressed_size
				= group->records[record].uncompressed_sum
				- iter->block.uncompressed_stream_offset;
		iter->block.unpadded_size
				= group->records[record].unpadded_sum
				- iter->block.compressed_stream_offset;
		iter->block.total_size = vli_ceil4(iter->block.unpadded_size);

		iter->block.compressed_stream_offset
				+= LZMA_STREAM_HEADER_SIZE;

		iter->block.compressed_file_offset
				= iter->block.compressed_stream_offset
				+ iter->stream.compressed_offset;
		iter->block.uncompressed_file_offset
				= iter->block.uncompressed_stream_offset
				+ iter->stream.uncompressed_offset;
	}

	return;
}

extern LZMA_API(void)
lzma_index_iter_init(lzma_index_iter *iter, const lzma_index *i)
{
	iter->internal[ITER_INDEX].p = i;
	lzma_index_iter_rewind(iter);
	return;
}

extern LZMA_API(void)
lzma_index_iter_rewind(lzma_index_iter *iter)
{
	iter->internal[ITER_STREAM].p = NULL;
	iter->internal[ITER_GROUP].p = NULL;
	iter->internal[ITER_RECORD].s = 0;
	iter->internal[ITER_METHOD].s = ITER_METHOD_NORMAL;
	return;
}

extern LZMA_API(lzma_bool)
lzma_index_iter_next(lzma_index_iter *iter, lzma_index_iter_mode mode)
{
	// Catch unsupported mode values.
	if ((unsigned int)(mode) > LZMA_INDEX_ITER_NONEMPTY_BLOCK)
		return true;

	const lzma_index *i = iter->internal[ITER_INDEX].p;
	const index_stream *stream = iter->internal[ITER_STREAM].p;
	const index_group *group = NULL;
	size_t record = iter->internal[ITER_RECORD].s;

	// If we are being asked for the next Stream, leave group to NULL
	// so that the rest of the this function thinks that this Stream
	// has no groups and will thus go to the next Stream.
	if (mode != LZMA_INDEX_ITER_STREAM) {
		// Get the pointer to the current group. See iter_set_inf()
		// for explanation.
		switch (iter->internal[ITER_METHOD].s) {
		case ITER_METHOD_NORMAL:
			group = iter->internal[ITER_GROUP].p;
			break;

		case ITER_METHOD_NEXT:
			group = index_tree_next(iter->internal[ITER_GROUP].p);
			break;

		case ITER_METHOD_LEFTMOST:
			group = (const index_group *)(
					stream->groups.leftmost);
			break;
		}
	}

again:
	if (stream == NULL) {
		// We at the beginning of the lzma_index.
		// Locate the first Stream.
		stream = (const index_stream *)(i->streams.leftmost);
		if (mode >= LZMA_INDEX_ITER_BLOCK) {
			// Since we are being asked to return information
			// about the first a Block, skip Streams that have
			// no Blocks.
			while (stream->groups.leftmost == NULL) {
				stream = index_tree_next(&stream->node);
				if (stream == NULL)
					return true;
			}
		}

		// Start from the first Record in the Stream.
		group = (const index_group *)(stream->groups.leftmost);
		record = 0;

	} else if (group != NULL && record < group->last) {
		// The next Record is in the same group.
		++record;

	} else {
		// This group has no more Records or this Stream has
		// no Blocks at all.
		record = 0;

		// If group is not NULL, this Stream has at least one Block
		// and thus at least one group. Find the next group.
		if (group != NULL)
			group = index_tree_next(&group->node);

		if (group == NULL) {
			// This Stream has no more Records. Find the next
			// Stream. If we are being asked to return information
			// about a Block, we skip empty Streams.
			do {
				stream = index_tree_next(&stream->node);
				if (stream == NULL)
					return true;
			} while (mode >= LZMA_INDEX_ITER_BLOCK
					&& stream->groups.leftmost == NULL);

			group = (const index_group *)(
					stream->groups.leftmost);
		}
	}

	if (mode == LZMA_INDEX_ITER_NONEMPTY_BLOCK) {
		// We need to look for the next Block again if this Block
		// is empty.
		if (record == 0) {
			if (group->node.uncompressed_base
					== group->records[0].uncompressed_sum)
				goto again;
		} else if (group->records[record - 1].uncompressed_sum
				== group->records[record].uncompressed_sum) {
			goto again;
		}
	}

	iter->internal[ITER_STREAM].p = stream;
	iter->internal[ITER_GROUP].p = group;
	iter->internal[ITER_RECORD].s = record;

	iter_set_info(iter);

	return false;
}

extern LZMA_API(lzma_bool)
lzma_index_iter_locate(lzma_index_iter *iter, lzma_vli target)
{
	const lzma_index *i = iter->internal[ITER_INDEX].p;

	// If the target is past the end of the file, return immediately.
	if (i->uncompressed_size <= target)
		return true;

	// Locate the Stream containing the target offset.
	const index_stream *stream = index_tree_locate(&i->streams, target);
	assert(stream != NULL);
	target -= stream->node.uncompressed_base;

	// Locate the group containing the target offset.
	const index_group *group = index_tree_locate(&stream->groups, target);
	assert(group != NULL);

	// Use binary search to locate the exact Record. It is the first
	// Record whose uncompressed_sum is greater than target.
	// This is because we want the rightmost Record that fullfills the
	// search criterion. It is possible that there are empty Blocks;
	// we don't want to return them.
	size_t left = 0;
	size_t right = group->last;

	while (left < right) {
		const size_t pos = left + (right - left) / 2;
		if (group->records[pos].uncompressed_sum <= target)
			left = pos + 1;
		else
			right = pos;
	}

	iter->internal[ITER_STREAM].p = stream;
	iter->internal[ITER_GROUP].p = group;
	iter->internal[ITER_RECORD].s = left;

	iter_set_info(iter);

	return false;
}

/*** End of inlined file: index.c ***/


/*** Start of inlined file: index_decoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       index_decoder.c
/// \brief      Decodes the Index field
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "check.h"

typedef struct {
	enum {
		SEQ_INDICATOR,
		SEQ_COUNT,
		SEQ_MEMUSAGE,
		SEQ_UNPADDED,
		SEQ_UNCOMPRESSED,
		SEQ_PADDING_INIT,
		SEQ_PADDING,
		SEQ_CRC32,
	} sequence;

	/// Memory usage limit
	uint64_t memlimit;

	/// Target Index
	lzma_index *index;

	/// Pointer give by the application, which is set after
	/// successful decoding.
	lzma_index **index_ptr;

	/// Number of Records left to decode.
	lzma_vli count;

	/// The most recent Unpadded Size field
	lzma_vli unpadded_size;

	/// The most recent Uncompressed Size field
	lzma_vli uncompressed_size;

	/// Position in integers
	size_t pos;

	/// CRC32 of the List of Records field
	uint32_t crc32;
} lzma_index_coder;

static lzma_ret
index_decode(void *coder_ptr, const lzma_allocator *allocator,
		const uint8_t *restrict in, size_t *restrict in_pos,
		size_t in_size,
		uint8_t *restrict out lzma_attribute((__unused__)),
		size_t *restrict out_pos lzma_attribute((__unused__)),
		size_t out_size lzma_attribute((__unused__)),
		lzma_action action lzma_attribute((__unused__)))
{
	lzma_index_coder *coder = coder_ptr;

	// Similar optimization as in index_encoder.c
	const size_t in_start = *in_pos;
	lzma_ret ret = LZMA_OK;

	while (*in_pos < in_size)
	switch (coder->sequence) {
	case SEQ_INDICATOR:
		// Return LZMA_DATA_ERROR instead of e.g. LZMA_PROG_ERROR or
		// LZMA_FORMAT_ERROR, because a typical usage case for Index
		// decoder is when parsing the Stream backwards. If seeking
		// backward from the Stream Footer gives us something that
		// doesn't begin with Index Indicator, the file is considered
		// corrupt, not "programming error" or "unrecognized file
		// format". One could argue that the application should
		// verify the Index Indicator before trying to decode the
		// Index, but well, I suppose it is simpler this way.
		if (in[(*in_pos)++] != 0x00)
			return LZMA_DATA_ERROR;

		coder->sequence = SEQ_COUNT;
		break;

	case SEQ_COUNT:
		ret = lzma_vli_decode(&coder->count, &coder->pos,
				in, in_pos, in_size);
		if (ret != LZMA_STREAM_END)
			goto out;

		coder->pos = 0;
		coder->sequence = SEQ_MEMUSAGE;

	// Fall through

	case SEQ_MEMUSAGE:
		if (lzma_index_memusage(1, coder->count) > coder->memlimit) {
			ret = LZMA_MEMLIMIT_ERROR;
			goto out;
		}

		// Tell the Index handling code how many Records this
		// Index has to allow it to allocate memory more efficiently.
		lzma_index_prealloc(coder->index, coder->count);

		ret = LZMA_OK;
		coder->sequence = coder->count == 0
				? SEQ_PADDING_INIT : SEQ_UNPADDED;
		break;

	case SEQ_UNPADDED:
	case SEQ_UNCOMPRESSED: {
		lzma_vli *size = coder->sequence == SEQ_UNPADDED
				? &coder->unpadded_size
				: &coder->uncompressed_size;

		ret = lzma_vli_decode(size, &coder->pos,
				in, in_pos, in_size);
		if (ret != LZMA_STREAM_END)
			goto out;

		ret = LZMA_OK;
		coder->pos = 0;

		if (coder->sequence == SEQ_UNPADDED) {
			// Validate that encoded Unpadded Size isn't too small
			// or too big.
			if (coder->unpadded_size < UNPADDED_SIZE_MIN
					|| coder->unpadded_size
						> UNPADDED_SIZE_MAX)
				return LZMA_DATA_ERROR;

			coder->sequence = SEQ_UNCOMPRESSED;
		} else {
			// Add the decoded Record to the Index.
			return_if_error(lzma_index_append(
					coder->index, allocator,
					coder->unpadded_size,
					coder->uncompressed_size));

			// Check if this was the last Record.
			coder->sequence = --coder->count == 0
					? SEQ_PADDING_INIT
					: SEQ_UNPADDED;
		}

		break;
	}

	case SEQ_PADDING_INIT:
		coder->pos = lzma_index_padding_size(coder->index);
		coder->sequence = SEQ_PADDING;

	// Fall through

	case SEQ_PADDING:
		if (coder->pos > 0) {
			--coder->pos;
			if (in[(*in_pos)++] != 0x00)
				return LZMA_DATA_ERROR;

			break;
		}

		// Finish the CRC32 calculation.
		coder->crc32 = lzma_crc32(in + in_start,
				*in_pos - in_start, coder->crc32);

		coder->sequence = SEQ_CRC32;

	// Fall through

	case SEQ_CRC32:
		do {
			if (*in_pos == in_size)
				return LZMA_OK;

			if (((coder->crc32 >> (coder->pos * 8)) & 0xFF)
					!= in[(*in_pos)++])
				return LZMA_DATA_ERROR;

		} while (++coder->pos < 4);

		// Decoding was successful, now we can let the application
		// see the decoded Index.
		*coder->index_ptr = coder->index;

		// Make index NULL so we don't free it unintentionally.
		coder->index = NULL;

		return LZMA_STREAM_END;

	default:
		assert(0);
		return LZMA_PROG_ERROR;
	}

out:
	// Update the CRC32,
	coder->crc32 = lzma_crc32(in + in_start,
			*in_pos - in_start, coder->crc32);

	return ret;
}

static void
index_decoder_end(void *coder_ptr, const lzma_allocator *allocator)
{
	lzma_index_coder *coder = coder_ptr;
	lzma_index_end(coder->index, allocator);
	lzma_free(coder, allocator);
	return;
}

static lzma_ret
index_decoder_memconfig(void *coder_ptr, uint64_t *memusage,
		uint64_t *old_memlimit, uint64_t new_memlimit)
{
	lzma_index_coder *coder = coder_ptr;

	*memusage = lzma_index_memusage(1, coder->count);
	*old_memlimit = coder->memlimit;

	if (new_memlimit != 0) {
		if (new_memlimit < *memusage)
			return LZMA_MEMLIMIT_ERROR;

		coder->memlimit = new_memlimit;
	}

	return LZMA_OK;
}

static lzma_ret
index_decoder_reset(lzma_index_coder *coder, const lzma_allocator *allocator,
		lzma_index **i, uint64_t memlimit)
{
	// Remember the pointer given by the application. We will set it
	// to point to the decoded Index only if decoding is successful.
	// Before that, keep it NULL so that applications can always safely
	// pass it to lzma_index_end() no matter did decoding succeed or not.
	coder->index_ptr = i;
	*i = NULL;

	// We always allocate a new lzma_index.
	coder->index = lzma_index_init(allocator);
	if (coder->index == NULL)
		return LZMA_MEM_ERROR;

	// Initialize the rest.
	coder->sequence = SEQ_INDICATOR;
	coder->memlimit = my_max(1, memlimit);
	coder->count = 0; // Needs to be initialized due to _memconfig().
	coder->pos = 0;
	coder->crc32 = 0;

	return LZMA_OK;
}

static lzma_ret
index_decoder_init(lzma_next_coder *next, const lzma_allocator *allocator,
		lzma_index **i, uint64_t memlimit)
{
	lzma_next_coder_init(&index_decoder_init, next, allocator);

	if (i == NULL)
		return LZMA_PROG_ERROR;

	lzma_index_coder *coder = next->coder;
	if (coder == NULL) {
		coder = lzma_alloc(sizeof(lzma_index_coder), allocator);
		if (coder == NULL)
			return LZMA_MEM_ERROR;

		next->coder = coder;
		next->code = &index_decode;
		next->end = &index_decoder_end;
		next->memconfig = &index_decoder_memconfig;
		coder->index = NULL;
	} else {
		lzma_index_end(coder->index, allocator);
	}

	return index_decoder_reset(coder, allocator, i, memlimit);
}

extern LZMA_API(lzma_ret)
lzma_index_decoder(lzma_stream *strm, lzma_index **i, uint64_t memlimit)
{
	lzma_next_strm_init(index_decoder_init, strm, i, memlimit);

	strm->internal->supported_actions[LZMA_RUN] = true;
	strm->internal->supported_actions[LZMA_FINISH] = true;

	return LZMA_OK;
}

extern LZMA_API(lzma_ret)
lzma_index_buffer_decode(lzma_index **i, uint64_t *memlimit,
		const lzma_allocator *allocator,
		const uint8_t *in, size_t *in_pos, size_t in_size)
{
	// Sanity checks
	if (i == NULL || memlimit == NULL
			|| in == NULL || in_pos == NULL || *in_pos > in_size)
		return LZMA_PROG_ERROR;

	// Initialize the decoder.
	lzma_index_coder coder;
	return_if_error(index_decoder_reset(&coder, allocator, i, *memlimit));

	// Store the input start position so that we can restore it in case
	// of an error.
	const size_t in_start = *in_pos;

	// Do the actual decoding.
	lzma_ret ret = index_decode(&coder, allocator, in, in_pos, in_size,
			NULL, NULL, 0, LZMA_RUN);

	if (ret == LZMA_STREAM_END) {
		ret = LZMA_OK;
	} else {
		// Something went wrong, free the Index structure and restore
		// the input position.
		lzma_index_end(coder.index, allocator);
		*in_pos = in_start;

		if (ret == LZMA_OK) {
			// The input is truncated or otherwise corrupt.
			// Use LZMA_DATA_ERROR instead of LZMA_BUF_ERROR
			// like lzma_vli_decode() does in single-call mode.
			ret = LZMA_DATA_ERROR;

		} else if (ret == LZMA_MEMLIMIT_ERROR) {
			// Tell the caller how much memory would have
			// been needed.
			*memlimit = lzma_index_memusage(1, coder.count);
		}
	}

	return ret;
}

/*** End of inlined file: index_decoder.c ***/


/*** Start of inlined file: index_encoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       index_encoder.c
/// \brief      Encodes the Index field
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "check.h"

typedef struct {
	enum {
		SEQ_INDICATOR,
		SEQ_COUNT,
		SEQ_UNPADDED,
		SEQ_UNCOMPRESSED,
		SEQ_NEXT,
		SEQ_PADDING,
		SEQ_CRC32,
	} sequence;

	/// Index being encoded
	const lzma_index *index;

	/// Iterator for the Index being encoded
	lzma_index_iter iter;

	/// Position in integers
	size_t pos;

	/// CRC32 of the List of Records field
	uint32_t crc32;
} lzma_index_coder;

static lzma_ret
index_encode(void *coder_ptr,
		const lzma_allocator *allocator lzma_attribute((__unused__)),
		const uint8_t *restrict in lzma_attribute((__unused__)),
		size_t *restrict in_pos lzma_attribute((__unused__)),
		size_t in_size lzma_attribute((__unused__)),
		uint8_t *restrict out, size_t *restrict out_pos,
		size_t out_size,
		lzma_action action lzma_attribute((__unused__)))
{
	lzma_index_coder *coder = coder_ptr;

	// Position where to start calculating CRC32. The idea is that we
	// need to call lzma_crc32() only once per call to index_encode().
	const size_t out_start = *out_pos;

	// Return value to use if we return at the end of this function.
	// We use "goto out" to jump out of the while-switch construct
	// instead of returning directly, because that way we don't need
	// to copypaste the lzma_crc32() call to many places.
	lzma_ret ret = LZMA_OK;

	while (*out_pos < out_size)
	switch (coder->sequence) {
	case SEQ_INDICATOR:
		out[*out_pos] = 0x00;
		++*out_pos;
		coder->sequence = SEQ_COUNT;
		break;

	case SEQ_COUNT: {
		const lzma_vli count = lzma_index_block_count(coder->index);
		ret = lzma_vli_encode(count, &coder->pos,
				out, out_pos, out_size);
		if (ret != LZMA_STREAM_END)
			goto out;

		ret = LZMA_OK;
		coder->pos = 0;
		coder->sequence = SEQ_NEXT;
		break;
	}

	case SEQ_NEXT:
		if (lzma_index_iter_next(
				&coder->iter, LZMA_INDEX_ITER_BLOCK)) {
			// Get the size of the Index Padding field.
			coder->pos = lzma_index_padding_size(coder->index);
			assert(coder->pos <= 3);
			coder->sequence = SEQ_PADDING;
			break;
		}

		coder->sequence = SEQ_UNPADDED;

	// Fall through

	case SEQ_UNPADDED:
	case SEQ_UNCOMPRESSED: {
		const lzma_vli size = coder->sequence == SEQ_UNPADDED
				? coder->iter.block.unpadded_size
				: coder->iter.block.uncompressed_size;

		ret = lzma_vli_encode(size, &coder->pos,
				out, out_pos, out_size);
		if (ret != LZMA_STREAM_END)
			goto out;

		ret = LZMA_OK;
		coder->pos = 0;

		// Advance to SEQ_UNCOMPRESSED or SEQ_NEXT.
		++coder->sequence;
		break;
	}

	case SEQ_PADDING:
		if (coder->pos > 0) {
			--coder->pos;
			out[(*out_pos)++] = 0x00;
			break;
		}

		// Finish the CRC32 calculation.
		coder->crc32 = lzma_crc32(out + out_start,
				*out_pos - out_start, coder->crc32);

		coder->sequence = SEQ_CRC32;

	// Fall through

	case SEQ_CRC32:
		// We don't use the main loop, because we don't want
		// coder->crc32 to be touched anymore.
		do {
			if (*out_pos == out_size)
				return LZMA_OK;

			out[*out_pos] = (coder->crc32 >> (coder->pos * 8))
					& 0xFF;
			++*out_pos;

		} while (++coder->pos < 4);

		return LZMA_STREAM_END;

	default:
		assert(0);
		return LZMA_PROG_ERROR;
	}

out:
	// Update the CRC32.
	coder->crc32 = lzma_crc32(out + out_start,
			*out_pos - out_start, coder->crc32);

	return ret;
}

static void
index_encoder_end(void *coder, const lzma_allocator *allocator)
{
	lzma_free(coder, allocator);
	return;
}

static void
index_encoder_reset(lzma_index_coder *coder, const lzma_index *i)
{
	lzma_index_iter_init(&coder->iter, i);

	coder->sequence = SEQ_INDICATOR;
	coder->index = i;
	coder->pos = 0;
	coder->crc32 = 0;

	return;
}

extern lzma_ret
lzma_index_encoder_init(lzma_next_coder *next, const lzma_allocator *allocator,
		const lzma_index *i)
{
	lzma_next_coder_init(&lzma_index_encoder_init, next, allocator);

	if (i == NULL)
		return LZMA_PROG_ERROR;

	if (next->coder == NULL) {
		next->coder = lzma_alloc(sizeof(lzma_index_coder), allocator);
		if (next->coder == NULL)
			return LZMA_MEM_ERROR;

		next->code = &index_encode;
		next->end = &index_encoder_end;
	}

	index_encoder_reset(next->coder, i);

	return LZMA_OK;
}

extern LZMA_API(lzma_ret)
lzma_index_encoder(lzma_stream *strm, const lzma_index *i)
{
	lzma_next_strm_init(lzma_index_encoder_init, strm, i);

	strm->internal->supported_actions[LZMA_RUN] = true;
	strm->internal->supported_actions[LZMA_FINISH] = true;

	return LZMA_OK;
}

extern LZMA_API(lzma_ret)
lzma_index_buffer_encode(const lzma_index *i,
		uint8_t *out, size_t *out_pos, size_t out_size)
{
	// Validate the arguments.
	if (i == NULL || out == NULL || out_pos == NULL || *out_pos > out_size)
		return LZMA_PROG_ERROR;

	// Don't try to encode if there's not enough output space.
	if (out_size - *out_pos < lzma_index_size(i))
		return LZMA_BUF_ERROR;

	// The Index encoder needs just one small data structure so we can
	// allocate it on stack.
	lzma_index_coder coder;
	index_encoder_reset(&coder, i);

	// Do the actual encoding. This should never fail, but store
	// the original *out_pos just in case.
	const size_t out_start = *out_pos;
	lzma_ret ret = index_encode(&coder, NULL, NULL, NULL, 0,
			out, out_pos, out_size, LZMA_RUN);

	if (ret == LZMA_STREAM_END) {
		ret = LZMA_OK;
	} else {
		// We should never get here, but just in case, restore the
		// output position and set the error accordingly if something
		// goes wrong and debugging isn't enabled.
		assert(0);
		*out_pos = out_start;
		ret = LZMA_PROG_ERROR;
	}

	return ret;
}

/*** End of inlined file: index_encoder.c ***/


/*** Start of inlined file: index_hash.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       index_hash.c
/// \brief      Validates Index by using a hash function
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "check.h"

typedef struct {
	/// Sum of the Block sizes (including Block Padding)
	lzma_vli blocks_size;

	/// Sum of the Uncompressed Size fields
	lzma_vli uncompressed_size;

	/// Number of Records
	lzma_vli count;

	/// Size of the List of Index Records as bytes
	lzma_vli index_list_size;

	/// Check calculated from Unpadded Sizes and Uncompressed Sizes.
	lzma_check_state check;

} lzma_index_hash_info;

struct lzma_index_hash_s {
	enum {
		SEQ_BLOCK,
		SEQ_COUNT,
		SEQ_UNPADDED,
		SEQ_UNCOMPRESSED,
		SEQ_PADDING_INIT,
		SEQ_PADDING,
		SEQ_CRC32,
	} sequence;

	/// Information collected while decoding the actual Blocks.
	lzma_index_hash_info blocks;

	/// Information collected from the Index field.
	lzma_index_hash_info records;

	/// Number of Records not fully decoded
	lzma_vli remaining;

	/// Unpadded Size currently being read from an Index Record.
	lzma_vli unpadded_size;

	/// Uncompressed Size currently being read from an Index Record.
	lzma_vli uncompressed_size;

	/// Position in variable-length integers when decoding them from
	/// the List of Records.
	size_t pos;

	/// CRC32 of the Index
	uint32_t crc32;
};

extern LZMA_API(lzma_index_hash *)
lzma_index_hash_init(lzma_index_hash *index_hash,
		const lzma_allocator *allocator)
{
	if (index_hash == NULL) {
		index_hash = lzma_alloc(sizeof(lzma_index_hash), allocator);
		if (index_hash == NULL)
			return NULL;
	}

	index_hash->sequence = SEQ_BLOCK;
	index_hash->blocks.blocks_size = 0;
	index_hash->blocks.uncompressed_size = 0;
	index_hash->blocks.count = 0;
	index_hash->blocks.index_list_size = 0;
	index_hash->records.blocks_size = 0;
	index_hash->records.uncompressed_size = 0;
	index_hash->records.count = 0;
	index_hash->records.index_list_size = 0;
	index_hash->unpadded_size = 0;
	index_hash->uncompressed_size = 0;
	index_hash->pos = 0;
	index_hash->crc32 = 0;

	// These cannot fail because LZMA_CHECK_BEST is known to be supported.
	(void)lzma_check_init(&index_hash->blocks.check, LZMA_CHECK_BEST);
	(void)lzma_check_init(&index_hash->records.check, LZMA_CHECK_BEST);

	return index_hash;
}

extern LZMA_API(void)
lzma_index_hash_end(lzma_index_hash *index_hash,
		const lzma_allocator *allocator)
{
	lzma_free(index_hash, allocator);
	return;
}

extern LZMA_API(lzma_vli)
lzma_index_hash_size(const lzma_index_hash *index_hash)
{
	// Get the size of the Index from ->blocks instead of ->records for
	// cases where application wants to know the Index Size before
	// decoding the Index.
	return index_size(index_hash->blocks.count,
			index_hash->blocks.index_list_size);
}

/// Updates the sizes and the hash without any validation.
static lzma_ret
hash_append(lzma_index_hash_info *info, lzma_vli unpadded_size,
		lzma_vli uncompressed_size)
{
	info->blocks_size += vli_ceil4(unpadded_size);
	info->uncompressed_size += uncompressed_size;
	info->index_list_size += lzma_vli_size(unpadded_size)
			+ lzma_vli_size(uncompressed_size);
	++info->count;

	const lzma_vli sizes[2] = { unpadded_size, uncompressed_size };
	lzma_check_update(&info->check, LZMA_CHECK_BEST,
			(const uint8_t *)(sizes), sizeof(sizes));

	return LZMA_OK;
}

extern LZMA_API(lzma_ret)
lzma_index_hash_append(lzma_index_hash *index_hash, lzma_vli unpadded_size,
		lzma_vli uncompressed_size)
{
	// Validate the arguments.
	if (index_hash->sequence != SEQ_BLOCK
			|| unpadded_size < UNPADDED_SIZE_MIN
			|| unpadded_size > UNPADDED_SIZE_MAX
			|| uncompressed_size > LZMA_VLI_MAX)
		return LZMA_PROG_ERROR;

	// Update the hash.
	return_if_error(hash_append(&index_hash->blocks,
			unpadded_size, uncompressed_size));

	// Validate the properties of *info are still in allowed limits.
	if (index_hash->blocks.blocks_size > LZMA_VLI_MAX
			|| index_hash->blocks.uncompressed_size > LZMA_VLI_MAX
			|| index_size(index_hash->blocks.count,
					index_hash->blocks.index_list_size)
				> LZMA_BACKWARD_SIZE_MAX
			|| index_stream_size(index_hash->blocks.blocks_size,
					index_hash->blocks.count,
					index_hash->blocks.index_list_size)
				> LZMA_VLI_MAX)
		return LZMA_DATA_ERROR;

	return LZMA_OK;
}

extern LZMA_API(lzma_ret)
lzma_index_hash_decode(lzma_index_hash *index_hash, const uint8_t *in,
		size_t *in_pos, size_t in_size)
{
	// Catch zero input buffer here, because in contrast to Index encoder
	// and decoder functions, applications call this function directly
	// instead of via lzma_code(), which does the buffer checking.
	if (*in_pos >= in_size)
		return LZMA_BUF_ERROR;

	// NOTE: This function has many similarities to index_encode() and
	// index_decode() functions found from index_encoder.c and
	// index_decoder.c. See the comments especially in index_encoder.c.
	const size_t in_start = *in_pos;
	lzma_ret ret = LZMA_OK;

	while (*in_pos < in_size)
	switch (index_hash->sequence) {
	case SEQ_BLOCK:
		// Check the Index Indicator is present.
		if (in[(*in_pos)++] != 0x00)
			return LZMA_DATA_ERROR;

		index_hash->sequence = SEQ_COUNT;
		break;

	case SEQ_COUNT: {
		ret = lzma_vli_decode(&index_hash->remaining,
				&index_hash->pos, in, in_pos, in_size);
		if (ret != LZMA_STREAM_END)
			goto out;

		// The count must match the count of the Blocks decoded.
		if (index_hash->remaining != index_hash->blocks.count)
			return LZMA_DATA_ERROR;

		ret = LZMA_OK;
		index_hash->pos = 0;

		// Handle the special case when there are no Blocks.
		index_hash->sequence = index_hash->remaining == 0
				? SEQ_PADDING_INIT : SEQ_UNPADDED;
		break;
	}

	case SEQ_UNPADDED:
	case SEQ_UNCOMPRESSED: {
		lzma_vli *size = index_hash->sequence == SEQ_UNPADDED
				? &index_hash->unpadded_size
				: &index_hash->uncompressed_size;

		ret = lzma_vli_decode(size, &index_hash->pos,
				in, in_pos, in_size);
		if (ret != LZMA_STREAM_END)
			goto out;

		ret = LZMA_OK;
		index_hash->pos = 0;

		if (index_hash->sequence == SEQ_UNPADDED) {
			if (index_hash->unpadded_size < UNPADDED_SIZE_MIN
					|| index_hash->unpadded_size
						> UNPADDED_SIZE_MAX)
				return LZMA_DATA_ERROR;

			index_hash->sequence = SEQ_UNCOMPRESSED;
		} else {
			// Update the hash.
			return_if_error(hash_append(&index_hash->records,
					index_hash->unpadded_size,
					index_hash->uncompressed_size));

			// Verify that we don't go over the known sizes. Note
			// that this validation is simpler than the one used
			// in lzma_index_hash_append(), because here we know
			// that values in index_hash->blocks are already
			// validated and we are fine as long as we don't
			// exceed them in index_hash->records.
			if (index_hash->blocks.blocks_size
					< index_hash->records.blocks_size
					|| index_hash->blocks.uncompressed_size
					< index_hash->records.uncompressed_size
					|| index_hash->blocks.index_list_size
					< index_hash->records.index_list_size)
				return LZMA_DATA_ERROR;

			// Check if this was the last Record.
			index_hash->sequence = --index_hash->remaining == 0
					? SEQ_PADDING_INIT : SEQ_UNPADDED;
		}

		break;
	}

	case SEQ_PADDING_INIT:
		index_hash->pos = (LZMA_VLI_C(4) - index_size_unpadded(
				index_hash->records.count,
				index_hash->records.index_list_size)) & 3;
		index_hash->sequence = SEQ_PADDING;

	// Fall through

	case SEQ_PADDING:
		if (index_hash->pos > 0) {
			--index_hash->pos;
			if (in[(*in_pos)++] != 0x00)
				return LZMA_DATA_ERROR;

			break;
		}

		// Compare the sizes.
		if (index_hash->blocks.blocks_size
				!= index_hash->records.blocks_size
				|| index_hash->blocks.uncompressed_size
				!= index_hash->records.uncompressed_size
				|| index_hash->blocks.index_list_size
				!= index_hash->records.index_list_size)
			return LZMA_DATA_ERROR;

		// Finish the hashes and compare them.
		lzma_check_finish(&index_hash->blocks.check, LZMA_CHECK_BEST);
		lzma_check_finish(&index_hash->records.check, LZMA_CHECK_BEST);
		if (memcmp(index_hash->blocks.check.buffer.u8,
				index_hash->records.check.buffer.u8,
				lzma_check_size(LZMA_CHECK_BEST)) != 0)
			return LZMA_DATA_ERROR;

		// Finish the CRC32 calculation.
		index_hash->crc32 = lzma_crc32(in + in_start,
				*in_pos - in_start, index_hash->crc32);

		index_hash->sequence = SEQ_CRC32;

	// Fall through

	case SEQ_CRC32:
		do {
			if (*in_pos == in_size)
				return LZMA_OK;

			if (((index_hash->crc32 >> (index_hash->pos * 8))
					& 0xFF) != in[(*in_pos)++])
				return LZMA_DATA_ERROR;

		} while (++index_hash->pos < 4);

		return LZMA_STREAM_END;

	default:
		assert(0);
		return LZMA_PROG_ERROR;
	}

out:
	// Update the CRC32,
	index_hash->crc32 = lzma_crc32(in + in_start,
			*in_pos - in_start, index_hash->crc32);

	return ret;
}

/*** End of inlined file: index_hash.c ***/


/*** Start of inlined file: outqueue.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       outqueue.c
/// \brief      Output queue handling in multithreaded coding
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////


/*** Start of inlined file: outqueue.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       outqueue.h
/// \brief      Output queue handling in multithreaded coding
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

/// Output buffer for a single thread
typedef struct {
	/// Pointer to the output buffer of lzma_outq.buf_size_max bytes
	uint8_t *buf;

	/// Amount of data written to buf
	size_t size;

	/// Additional size information
	lzma_vli unpadded_size;
	lzma_vli uncompressed_size;

	/// True when no more data will be written into this buffer.
	///
	/// \note       This is read by another thread and thus access
	///             to this variable needs a mutex.
	bool finished;

} lzma_outbuf;

typedef struct {
	/// Array of buffers that are used cyclically.
	lzma_outbuf *bufs;

	/// Memory allocated for all the buffers
	uint8_t *bufs_mem;

	/// Amount of buffer space available in each buffer
	size_t buf_size_max;

	/// Number of buffers allocated
	uint32_t bufs_allocated;

	/// Position in the bufs array. The next buffer to be taken
	/// into use is bufs[bufs_pos].
	uint32_t bufs_pos;

	/// Number of buffers in use
	uint32_t bufs_used;

	/// Position in the buffer in lzma_outq_read()
	size_t read_pos;

} lzma_outq;

/**
 * \brief       Calculate the memory usage of an output queue
 *
 * \return      Approximate memory usage in bytes or UINT64_MAX on error.
 */
extern uint64_t lzma_outq_memusage(uint64_t buf_size_max, uint32_t threads);

/// \brief      Initialize an output queue
///
/// \param      outq            Pointer to an output queue. Before calling
///                             this function the first time, *outq should
///                             have been zeroed with memzero() so that this
///                             function knows that there are no previous
///                             allocations to free.
/// \param      allocator       Pointer to allocator or NULL
/// \param      buf_size_max    Maximum amount of data that a single buffer
///                             in the queue may need to store.
/// \param      threads         Number of buffers that may be in use
///                             concurrently. Note that more than this number
///                             of buffers will actually get allocated to
///                             improve performance when buffers finish
///                             out of order.
///
/// \return     - LZMA_OK
///             - LZMA_MEM_ERROR
///
extern lzma_ret lzma_outq_init(
		lzma_outq *outq, const lzma_allocator *allocator,
		uint64_t buf_size_max, uint32_t threads);

/// \brief      Free the memory associated with the output queue
extern void lzma_outq_end(lzma_outq *outq, const lzma_allocator *allocator);

/// \brief      Get a new buffer
///
/// lzma_outq_has_buf() must be used to check that there is a buffer
/// available before calling lzma_outq_get_buf().
///
extern lzma_outbuf *lzma_outq_get_buf(lzma_outq *outq);

/// \brief      Test if there is data ready to be read
///
/// Call to this function must be protected with the same mutex that
/// is used to protect lzma_outbuf.finished.
///
extern bool lzma_outq_is_readable(const lzma_outq *outq);

/// \brief      Read finished data
///
/// \param      outq            Pointer to an output queue
/// \param      out             Beginning of the output buffer
/// \param      out_pos         The next byte will be written to
///                             out[*out_pos].
/// \param      out_size        Size of the out buffer; the first byte into
///                             which no data is written to is out[out_size].
/// \param      unpadded_size   Unpadded Size from the Block encoder
/// \param      uncompressed_size Uncompressed Size from the Block encoder
///
/// \return     - LZMA: All OK. Either no data was available or the buffer
///               being read didn't become empty yet.
///             - LZMA_STREAM_END: The buffer being read was finished.
///               *unpadded_size and *uncompressed_size were set.
///
/// \note       This reads lzma_outbuf.finished variables and thus call
///             to this function needs to be protected with a mutex.
///
extern lzma_ret lzma_outq_read(lzma_outq *restrict outq,
		uint8_t *restrict out, size_t *restrict out_pos,
		size_t out_size, lzma_vli *restrict unpadded_size,
		lzma_vli *restrict uncompressed_size);

/// \brief      Test if there is at least one buffer free
///
/// This must be used before getting a new buffer with lzma_outq_get_buf().
///
static inline bool
lzma_outq_has_buf(const lzma_outq *outq)
{
	return outq->bufs_used < outq->bufs_allocated;
}

/// \brief      Test if the queue is completely empty
static inline bool
lzma_outq_is_empty(const lzma_outq *outq)
{
	return outq->bufs_used == 0;
}

/*** End of inlined file: outqueue.h ***/

/// This is to ease integer overflow checking: We may allocate up to
/// 2 * LZMA_THREADS_MAX buffers and we need some extra memory for other
/// data structures (that's the second /2).
#define BUF_SIZE_MAX (UINT64_MAX / LZMA_THREADS_MAX / 2 / 2)

static lzma_ret
get_options(uint64_t *bufs_alloc_size, uint32_t *bufs_count,
		uint64_t buf_size_max, uint32_t threads)
{
	if (threads > LZMA_THREADS_MAX || buf_size_max > BUF_SIZE_MAX)
		return LZMA_OPTIONS_ERROR;

	// The number of buffers is twice the number of threads.
	// This wastes RAM but keeps the threads busy when buffers
	// finish out of order.
	//
	// NOTE: If this is changed, update BUF_SIZE_MAX too.
	*bufs_count = threads * 2;
	*bufs_alloc_size = *bufs_count * buf_size_max;

	return LZMA_OK;
}

extern uint64_t
lzma_outq_memusage(uint64_t buf_size_max, uint32_t threads)
{
	uint64_t bufs_alloc_size;
	uint32_t bufs_count;

	if (get_options(&bufs_alloc_size, &bufs_count, buf_size_max, threads)
			!= LZMA_OK)
		return UINT64_MAX;

	return sizeof(lzma_outq) + bufs_count * sizeof(lzma_outbuf)
			+ bufs_alloc_size;
}

extern lzma_ret
lzma_outq_init(lzma_outq *outq, const lzma_allocator *allocator,
		uint64_t buf_size_max, uint32_t threads)
{
	uint64_t bufs_alloc_size;
	uint32_t bufs_count;

	// Set bufs_count and bufs_alloc_size.
	return_if_error(get_options(&bufs_alloc_size, &bufs_count,
			buf_size_max, threads));

	// Allocate memory if needed.
	if (outq->buf_size_max != buf_size_max
			|| outq->bufs_allocated != bufs_count) {
		lzma_outq_end(outq, allocator);

#if SIZE_MAX < UINT64_MAX
		if (bufs_alloc_size > SIZE_MAX)
			return LZMA_MEM_ERROR;
#endif

		outq->bufs = lzma_alloc(bufs_count * sizeof(lzma_outbuf),
				allocator);
		outq->bufs_mem = lzma_alloc((size_t)(bufs_alloc_size),
				allocator);

		if (outq->bufs == NULL || outq->bufs_mem == NULL) {
			lzma_outq_end(outq, allocator);
			return LZMA_MEM_ERROR;
		}
	}

	// Initialize the rest of the main structure. Initialization of
	// outq->bufs[] is done when they are actually needed.
	outq->buf_size_max = (size_t)(buf_size_max);
	outq->bufs_allocated = bufs_count;
	outq->bufs_pos = 0;
	outq->bufs_used = 0;
	outq->read_pos = 0;

	return LZMA_OK;
}

extern void
lzma_outq_end(lzma_outq *outq, const lzma_allocator *allocator)
{
	lzma_free(outq->bufs, allocator);
	outq->bufs = NULL;

	lzma_free(outq->bufs_mem, allocator);
	outq->bufs_mem = NULL;

	return;
}

extern lzma_outbuf *
lzma_outq_get_buf(lzma_outq *outq)
{
	// Caller must have checked it with lzma_outq_has_buf().
	assert(outq->bufs_used < outq->bufs_allocated);

	// Initialize the new buffer.
	lzma_outbuf *buf = &outq->bufs[outq->bufs_pos];
	buf->buf = outq->bufs_mem + outq->bufs_pos * outq->buf_size_max;
	buf->size = 0;
	buf->finished = false;

	// Update the queue state.
	if (++outq->bufs_pos == outq->bufs_allocated)
		outq->bufs_pos = 0;

	++outq->bufs_used;

	return buf;
}

extern bool
lzma_outq_is_readable(const lzma_outq *outq)
{
	uint32_t i = outq->bufs_pos - outq->bufs_used;
	if (outq->bufs_pos < outq->bufs_used)
		i += outq->bufs_allocated;

	return outq->bufs[i].finished;
}

extern lzma_ret
lzma_outq_read(lzma_outq *restrict outq, uint8_t *restrict out,
		size_t *restrict out_pos, size_t out_size,
		lzma_vli *restrict unpadded_size,
		lzma_vli *restrict uncompressed_size)
{
	// There must be at least one buffer from which to read.
	if (outq->bufs_used == 0)
		return LZMA_OK;

	// Get the buffer.
	uint32_t i = outq->bufs_pos - outq->bufs_used;
	if (outq->bufs_pos < outq->bufs_used)
		i += outq->bufs_allocated;

	lzma_outbuf *buf = &outq->bufs[i];

	// If it isn't finished yet, we cannot read from it.
	if (!buf->finished)
		return LZMA_OK;

	// Copy from the buffer to output.
	lzma_bufcpy(buf->buf, &outq->read_pos, buf->size,
			out, out_pos, out_size);

	// Return if we didn't get all the data from the buffer.
	if (outq->read_pos < buf->size)
		return LZMA_OK;

	// The buffer was finished. Tell the caller its size information.
	*unpadded_size = buf->unpadded_size;
	*uncompressed_size = buf->uncompressed_size;

	// Free this buffer for further use.
	--outq->bufs_used;
	outq->read_pos = 0;

	return LZMA_STREAM_END;
}

/*** End of inlined file: outqueue.c ***/


/*** Start of inlined file: stream_buffer_decoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       stream_buffer_decoder.c
/// \brief      Single-call .xz Stream decoder
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

extern LZMA_API(lzma_ret)
lzma_stream_buffer_decode(uint64_t *memlimit, uint32_t flags,
		const lzma_allocator *allocator,
		const uint8_t *in, size_t *in_pos, size_t in_size,
		uint8_t *out, size_t *out_pos, size_t out_size)
{
	// Sanity checks
	if (in_pos == NULL || (in == NULL && *in_pos != in_size)
			|| *in_pos > in_size || out_pos == NULL
			|| (out == NULL && *out_pos != out_size)
			|| *out_pos > out_size)
		return LZMA_PROG_ERROR;

	// Catch flags that are not allowed in buffer-to-buffer decoding.
	if (flags & LZMA_TELL_ANY_CHECK)
		return LZMA_PROG_ERROR;

	// Initialize the Stream decoder.
	// TODO: We need something to tell the decoder that it can use the
	// output buffer as workspace, and thus save significant amount of RAM.
	lzma_next_coder stream_decoder = LZMA_NEXT_CODER_INIT;
	lzma_ret ret = lzma_stream_decoder_init(
			&stream_decoder, allocator, *memlimit, flags);

	if (ret == LZMA_OK) {
		// Save the positions so that we can restore them in case
		// an error occurs.
		const size_t in_start = *in_pos;
		const size_t out_start = *out_pos;

		// Do the actual decoding.
		ret = stream_decoder.code(stream_decoder.coder, allocator,
				in, in_pos, in_size, out, out_pos, out_size,
				LZMA_FINISH);

		if (ret == LZMA_STREAM_END) {
			ret = LZMA_OK;
		} else {
			// Something went wrong, restore the positions.
			*in_pos = in_start;
			*out_pos = out_start;

			if (ret == LZMA_OK) {
				// Either the input was truncated or the
				// output buffer was too small.
				assert(*in_pos == in_size
						|| *out_pos == out_size);

				// If all the input was consumed, then the
				// input is truncated, even if the output
				// buffer is also full. This is because
				// processing the last byte of the Stream
				// never produces output.
				if (*in_pos == in_size)
					ret = LZMA_DATA_ERROR;
				else
					ret = LZMA_BUF_ERROR;

			} else if (ret == LZMA_MEMLIMIT_ERROR) {
				// Let the caller know how much memory would
				// have been needed.
				uint64_t memusage;
				(void)stream_decoder.memconfig(
						stream_decoder.coder,
						memlimit, &memusage, 0);
			}
		}
	}

	// Free the decoder memory. This needs to be done even if
	// initialization fails, because the internal API doesn't
	// require the initialization function to free its memory on error.
	lzma_next_end(&stream_decoder, allocator);

	return ret;
}

/*** End of inlined file: stream_buffer_decoder.c ***/


/*** Start of inlined file: stream_buffer_encoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       stream_buffer_encoder.c
/// \brief      Single-call .xz Stream encoder
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

/// Maximum size of Index that has exactly one Record.
/// Index Indicator + Number of Records + Record + CRC32 rounded up to
/// the next multiple of four.
#define INDEX_BOUND ((1 + 1 + 2 * LZMA_VLI_BYTES_MAX + 4 + 3) & ~3)

/// Stream Header, Stream Footer, and Index
#define HEADERS_BOUND (2 * LZMA_STREAM_HEADER_SIZE + INDEX_BOUND)

extern LZMA_API(size_t)
lzma_stream_buffer_bound(size_t uncompressed_size)
{
	// Get the maximum possible size of a Block.
	const size_t block_bound = lzma_block_buffer_bound(uncompressed_size);
	if (block_bound == 0)
		return 0;

	// Catch the possible integer overflow and also prevent the size of
	// the Stream exceeding LZMA_VLI_MAX (theoretically possible on
	// 64-bit systems).
	if (my_min(SIZE_MAX, LZMA_VLI_MAX) - block_bound < HEADERS_BOUND)
		return 0;

	return block_bound + HEADERS_BOUND;
}

extern LZMA_API(lzma_ret)
lzma_stream_buffer_encode(lzma_filter *filters, lzma_check check,
		const lzma_allocator *allocator,
		const uint8_t *in, size_t in_size,
		uint8_t *out, size_t *out_pos_ptr, size_t out_size)
{
	// Sanity checks
	if (filters == NULL || (unsigned int)(check) > LZMA_CHECK_ID_MAX
			|| (in == NULL && in_size != 0) || out == NULL
			|| out_pos_ptr == NULL || *out_pos_ptr > out_size)
		return LZMA_PROG_ERROR;

	if (!lzma_check_is_supported(check))
		return LZMA_UNSUPPORTED_CHECK;

	// Note for the paranoids: Index encoder prevents the Stream from
	// getting too big and still being accepted with LZMA_OK, and Block
	// encoder catches if the input is too big. So we don't need to
	// separately check if the buffers are too big.

	// Use a local copy. We update *out_pos_ptr only if everything
	// succeeds.
	size_t out_pos = *out_pos_ptr;

	// Check that there's enough space for both Stream Header and
	// Stream Footer.
	if (out_size - out_pos <= 2 * LZMA_STREAM_HEADER_SIZE)
		return LZMA_BUF_ERROR;

	// Reserve space for Stream Footer so we don't need to check for
	// available space again before encoding Stream Footer.
	out_size -= LZMA_STREAM_HEADER_SIZE;

	// Encode the Stream Header.
	lzma_stream_flags stream_flags = {
		.version = 0,
		.check = check,
	};

	if (lzma_stream_header_encode(&stream_flags, out + out_pos)
			!= LZMA_OK)
		return LZMA_PROG_ERROR;

	out_pos += LZMA_STREAM_HEADER_SIZE;

	// Encode a Block but only if there is at least one byte of input.
	lzma_block block = {
		.version = 0,
		.check = check,
		.filters = filters,
	};

	if (in_size > 0)
		return_if_error(lzma_block_buffer_encode(&block, allocator,
				in, in_size, out, &out_pos, out_size));

	// Index
	{
		// Create an Index. It will have one Record if there was
		// at least one byte of input to encode. Otherwise the
		// Index will be empty.
		lzma_index *i = lzma_index_init(allocator);
		if (i == NULL)
			return LZMA_MEM_ERROR;

		lzma_ret ret = LZMA_OK;

		if (in_size > 0)
			ret = lzma_index_append(i, allocator,
					lzma_block_unpadded_size(&block),
					block.uncompressed_size);

		// If adding the Record was successful, encode the Index
		// and get its size which will be stored into Stream Footer.
		if (ret == LZMA_OK) {
			ret = lzma_index_buffer_encode(
					i, out, &out_pos, out_size);

			stream_flags.backward_size = lzma_index_size(i);
		}

		lzma_index_end(i, allocator);

		if (ret != LZMA_OK)
			return ret;
	}

	// Stream Footer. We have already reserved space for this.
	if (lzma_stream_footer_encode(&stream_flags, out + out_pos)
			!= LZMA_OK)
		return LZMA_PROG_ERROR;

	out_pos += LZMA_STREAM_HEADER_SIZE;

	// Everything went fine, make the new output position available
	// to the application.
	*out_pos_ptr = out_pos;
	return LZMA_OK;
}

/*** End of inlined file: stream_buffer_encoder.c ***/


/*** Start of inlined file: stream_decoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       stream_decoder.c
/// \brief      Decodes .xz Streams
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

typedef struct {
	enum {
		SEQ_STREAM_HEADER,
		SEQ_BLOCK_HEADER,
		SEQ_BLOCK,
		SEQ_INDEX,
		SEQ_STREAM_FOOTER,
		SEQ_STREAM_PADDING,
	} sequence;

	/// Block or Metadata decoder. This takes little memory and the same
	/// data structure can be used to decode every Block Header, so it's
	/// a good idea to have a separate lzma_next_coder structure for it.
	lzma_next_coder block_decoder;

	/// Block options decoded by the Block Header decoder and used by
	/// the Block decoder.
	lzma_block block_options;

	/// Stream Flags from Stream Header
	lzma_stream_flags stream_flags;

	/// Index is hashed so that it can be compared to the sizes of Blocks
	/// with O(1) memory usage.
	lzma_index_hash *index_hash;

	/// Memory usage limit
	uint64_t memlimit;

	/// Amount of memory actually needed (only an estimate)
	uint64_t memusage;

	/// If true, LZMA_NO_CHECK is returned if the Stream has
	/// no integrity check.
	bool tell_no_check;

	/// If true, LZMA_UNSUPPORTED_CHECK is returned if the Stream has
	/// an integrity check that isn't supported by this liblzma build.
	bool tell_unsupported_check;

	/// If true, LZMA_GET_CHECK is returned after decoding Stream Header.
	bool tell_any_check;

	/// If true, we will tell the Block decoder to skip calculating
	/// and verifying the integrity check.
	bool ignore_check;

	/// If true, we will decode concatenated Streams that possibly have
	/// Stream Padding between or after them. LZMA_STREAM_END is returned
	/// once the application isn't giving us any new input, and we aren't
	/// in the middle of a Stream, and possible Stream Padding is a
	/// multiple of four bytes.
	bool concatenated;

	/// When decoding concatenated Streams, this is true as long as we
	/// are decoding the first Stream. This is needed to avoid misleading
	/// LZMA_FORMAT_ERROR in case the later Streams don't have valid magic
	/// bytes.
	bool first_stream;

	/// Write position in buffer[] and position in Stream Padding
	size_t pos;

	/// Buffer to hold Stream Header, Block Header, and Stream Footer.
	/// Block Header has biggest maximum size.
	uint8_t buffer[LZMA_BLOCK_HEADER_SIZE_MAX];
} lzma_stream_coder;

static lzma_ret
stream_decoder_reset(lzma_stream_coder *coder, const lzma_allocator *allocator)
{
	// Initialize the Index hash used to verify the Index.
	coder->index_hash = lzma_index_hash_init(coder->index_hash, allocator);
	if (coder->index_hash == NULL)
		return LZMA_MEM_ERROR;

	// Reset the rest of the variables.
	coder->sequence = SEQ_STREAM_HEADER;
	coder->pos = 0;

	return LZMA_OK;
}

static lzma_ret
stream_decode(void *coder_ptr, const lzma_allocator *allocator,
		const uint8_t *restrict in, size_t *restrict in_pos,
		size_t in_size, uint8_t *restrict out,
		size_t *restrict out_pos, size_t out_size, lzma_action action)
{
	lzma_stream_coder *coder = coder_ptr;

	// When decoding the actual Block, it may be able to produce more
	// output even if we don't give it any new input.
	while (true)
	switch (coder->sequence) {
	case SEQ_STREAM_HEADER: {
		// Copy the Stream Header to the internal buffer.
		lzma_bufcpy(in, in_pos, in_size, coder->buffer, &coder->pos,
				LZMA_STREAM_HEADER_SIZE);

		// Return if we didn't get the whole Stream Header yet.
		if (coder->pos < LZMA_STREAM_HEADER_SIZE)
			return LZMA_OK;

		coder->pos = 0;

		// Decode the Stream Header.
		const lzma_ret ret = lzma_stream_header_decode(
				&coder->stream_flags, coder->buffer);
		if (ret != LZMA_OK)
			return ret == LZMA_FORMAT_ERROR && !coder->first_stream
					? LZMA_DATA_ERROR : ret;

		// If we are decoding concatenated Streams, and the later
		// Streams have invalid Header Magic Bytes, we give
		// LZMA_DATA_ERROR instead of LZMA_FORMAT_ERROR.
		coder->first_stream = false;

		// Copy the type of the Check so that Block Header and Block
		// decoders see it.
		coder->block_options.check = coder->stream_flags.check;

		// Even if we return LZMA_*_CHECK below, we want
		// to continue from Block Header decoding.
		coder->sequence = SEQ_BLOCK_HEADER;

		// Detect if there's no integrity check or if it is
		// unsupported if those were requested by the application.
		if (coder->tell_no_check && coder->stream_flags.check
				== LZMA_CHECK_NONE)
			return LZMA_NO_CHECK;

		if (coder->tell_unsupported_check
				&& !lzma_check_is_supported(
					coder->stream_flags.check))
			return LZMA_UNSUPPORTED_CHECK;

		if (coder->tell_any_check)
			return LZMA_GET_CHECK;
	}

	// Fall through

	case SEQ_BLOCK_HEADER: {
		if (*in_pos >= in_size)
			return LZMA_OK;

		if (coder->pos == 0) {
			// Detect if it's Index.
			if (in[*in_pos] == 0x00) {
				coder->sequence = SEQ_INDEX;
				break;
			}

			// Calculate the size of the Block Header. Note that
			// Block Header decoder wants to see this byte too
			// so don't advance *in_pos.
			coder->block_options.header_size
					= lzma_block_header_size_decode(
						in[*in_pos]);
		}

		// Copy the Block Header to the internal buffer.
		lzma_bufcpy(in, in_pos, in_size, coder->buffer, &coder->pos,
				coder->block_options.header_size);

		// Return if we didn't get the whole Block Header yet.
		if (coder->pos < coder->block_options.header_size)
			return LZMA_OK;

		coder->pos = 0;

		// Version 1 is needed to support the .ignore_check option.
		coder->block_options.version = 1;

		// Set up a buffer to hold the filter chain. Block Header
		// decoder will initialize all members of this array so
		// we don't need to do it here.
		lzma_filter filters[LZMA_FILTERS_MAX + 1];
		coder->block_options.filters = filters;

		// Decode the Block Header.
		return_if_error(lzma_block_header_decode(&coder->block_options,
				allocator, coder->buffer));

		// If LZMA_IGNORE_CHECK was used, this flag needs to be set.
		// It has to be set after lzma_block_header_decode() because
		// it always resets this to false.
		coder->block_options.ignore_check = coder->ignore_check;

		// Check the memory usage limit.
		const uint64_t memusage = lzma_raw_decoder_memusage(filters);
		lzma_ret ret;

		if (memusage == UINT64_MAX) {
			// One or more unknown Filter IDs.
			ret = LZMA_OPTIONS_ERROR;
		} else {
			// Now we can set coder->memusage since we know that
			// the filter chain is valid. We don't want
			// lzma_memusage() to return UINT64_MAX in case of
			// invalid filter chain.
			coder->memusage = memusage;

			if (memusage > coder->memlimit) {
				// The chain would need too much memory.
				ret = LZMA_MEMLIMIT_ERROR;
			} else {
				// Memory usage is OK.
				// Initialize the Block decoder.
				ret = lzma_block_decoder_init(
						&coder->block_decoder,
						allocator,
						&coder->block_options);
			}
		}

		// Free the allocated filter options since they are needed
		// only to initialize the Block decoder.
		for (size_t i = 0; i < LZMA_FILTERS_MAX; ++i)
			lzma_free(filters[i].options, allocator);

		coder->block_options.filters = NULL;

		// Check if memory usage calculation and Block enocoder
		// initialization succeeded.
		if (ret != LZMA_OK)
			return ret;

		coder->sequence = SEQ_BLOCK;
	}

	// Fall through

	case SEQ_BLOCK: {
		const lzma_ret ret = coder->block_decoder.code(
				coder->block_decoder.coder, allocator,
				in, in_pos, in_size, out, out_pos, out_size,
				action);

		if (ret != LZMA_STREAM_END)
			return ret;

		// Block decoded successfully. Add the new size pair to
		// the Index hash.
		return_if_error(lzma_index_hash_append(coder->index_hash,
				lzma_block_unpadded_size(
					&coder->block_options),
				coder->block_options.uncompressed_size));

		coder->sequence = SEQ_BLOCK_HEADER;
		break;
	}

	case SEQ_INDEX: {
		// If we don't have any input, don't call
		// lzma_index_hash_decode() since it would return
		// LZMA_BUF_ERROR, which we must not do here.
		if (*in_pos >= in_size)
			return LZMA_OK;

		// Decode the Index and compare it to the hash calculated
		// from the sizes of the Blocks (if any).
		const lzma_ret ret = lzma_index_hash_decode(coder->index_hash,
				in, in_pos, in_size);
		if (ret != LZMA_STREAM_END)
			return ret;

		coder->sequence = SEQ_STREAM_FOOTER;
	}

	// Fall through

	case SEQ_STREAM_FOOTER: {
		// Copy the Stream Footer to the internal buffer.
		lzma_bufcpy(in, in_pos, in_size, coder->buffer, &coder->pos,
				LZMA_STREAM_HEADER_SIZE);

		// Return if we didn't get the whole Stream Footer yet.
		if (coder->pos < LZMA_STREAM_HEADER_SIZE)
			return LZMA_OK;

		coder->pos = 0;

		// Decode the Stream Footer. The decoder gives
		// LZMA_FORMAT_ERROR if the magic bytes don't match,
		// so convert that return code to LZMA_DATA_ERROR.
		lzma_stream_flags footer_flags;
		const lzma_ret ret = lzma_stream_footer_decode(
				&footer_flags, coder->buffer);
		if (ret != LZMA_OK)
			return ret == LZMA_FORMAT_ERROR
					? LZMA_DATA_ERROR : ret;

		// Check that Index Size stored in the Stream Footer matches
		// the real size of the Index field.
		if (lzma_index_hash_size(coder->index_hash)
				!= footer_flags.backward_size)
			return LZMA_DATA_ERROR;

		// Compare that the Stream Flags fields are identical in
		// both Stream Header and Stream Footer.
		return_if_error(lzma_stream_flags_compare(
				&coder->stream_flags, &footer_flags));

		if (!coder->concatenated)
			return LZMA_STREAM_END;

		coder->sequence = SEQ_STREAM_PADDING;
	}

	// Fall through

	case SEQ_STREAM_PADDING:
		assert(coder->concatenated);

		// Skip over possible Stream Padding.
		while (true) {
			if (*in_pos >= in_size) {
				// Unless LZMA_FINISH was used, we cannot
				// know if there's more input coming later.
				if (action != LZMA_FINISH)
					return LZMA_OK;

				// Stream Padding must be a multiple of
				// four bytes.
				return coder->pos == 0
						? LZMA_STREAM_END
						: LZMA_DATA_ERROR;
			}

			// If the byte is not zero, it probably indicates
			// beginning of a new Stream (or the file is corrupt).
			if (in[*in_pos] != 0x00)
				break;

			++*in_pos;
			coder->pos = (coder->pos + 1) & 3;
		}

		// Stream Padding must be a multiple of four bytes (empty
		// Stream Padding is OK).
		if (coder->pos != 0) {
			++*in_pos;
			return LZMA_DATA_ERROR;
		}

		// Prepare to decode the next Stream.
		return_if_error(stream_decoder_reset(coder, allocator));
		break;

	default:
		assert(0);
		return LZMA_PROG_ERROR;
	}

	// Never reached
}

static void
stream_decoder_end(void *coder_ptr, const lzma_allocator *allocator)
{
	lzma_stream_coder *coder = coder_ptr;
	lzma_next_end(&coder->block_decoder, allocator);
	lzma_index_hash_end(coder->index_hash, allocator);
	lzma_free(coder, allocator);
	return;
}

static lzma_check
stream_decoder_get_check(const void *coder_ptr)
{
	const lzma_stream_coder *coder = coder_ptr;
	return coder->stream_flags.check;
}

static lzma_ret
stream_decoder_memconfig(void *coder_ptr, uint64_t *memusage,
		uint64_t *old_memlimit, uint64_t new_memlimit)
{
	lzma_stream_coder *coder = coder_ptr;

	*memusage = coder->memusage;
	*old_memlimit = coder->memlimit;

	if (new_memlimit != 0) {
		if (new_memlimit < coder->memusage)
			return LZMA_MEMLIMIT_ERROR;

		coder->memlimit = new_memlimit;
	}

	return LZMA_OK;
}

extern lzma_ret
lzma_stream_decoder_init(
		lzma_next_coder *next, const lzma_allocator *allocator,
		uint64_t memlimit, uint32_t flags)
{
	lzma_next_coder_init(&lzma_stream_decoder_init, next, allocator);

	if (flags & ~LZMA_SUPPORTED_FLAGS)
		return LZMA_OPTIONS_ERROR;

	lzma_stream_coder *coder = next->coder;
	if (coder == NULL) {
		coder = lzma_alloc(sizeof(lzma_stream_coder), allocator);
		if (coder == NULL)
			return LZMA_MEM_ERROR;

		next->coder = coder;
		next->code = &stream_decode;
		next->end = &stream_decoder_end;
		next->get_check = &stream_decoder_get_check;
		next->memconfig = &stream_decoder_memconfig;

		coder->block_decoder = LZMA_NEXT_CODER_INIT;
		coder->index_hash = NULL;
	}

	coder->memlimit = my_max(1, memlimit);
	coder->memusage = LZMA_MEMUSAGE_BASE;
	coder->tell_no_check = (flags & LZMA_TELL_NO_CHECK) != 0;
	coder->tell_unsupported_check
			= (flags & LZMA_TELL_UNSUPPORTED_CHECK) != 0;
	coder->tell_any_check = (flags & LZMA_TELL_ANY_CHECK) != 0;
	coder->ignore_check = (flags & LZMA_IGNORE_CHECK) != 0;
	coder->concatenated = (flags & LZMA_CONCATENATED) != 0;
	coder->first_stream = true;

	return stream_decoder_reset(coder, allocator);
}

extern LZMA_API(lzma_ret)
lzma_stream_decoder(lzma_stream *strm, uint64_t memlimit, uint32_t flags)
{
	lzma_next_strm_init(lzma_stream_decoder_init, strm, memlimit, flags);

	strm->internal->supported_actions[LZMA_RUN] = true;
	strm->internal->supported_actions[LZMA_FINISH] = true;

	return LZMA_OK;
}

/*** End of inlined file: stream_decoder.c ***/


/*** Start of inlined file: stream_encoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       stream_encoder.c
/// \brief      Encodes .xz Streams
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

typedef struct {
	enum {
		SEQ_STREAM_HEADER,
		SEQ_BLOCK_INIT,
		SEQ_BLOCK_HEADER,
		SEQ_BLOCK_ENCODE,
		SEQ_INDEX_ENCODE,
		SEQ_STREAM_FOOTER,
	} sequence;

	/// True if Block encoder has been initialized by
	/// stream_encoder_init() or stream_encoder_update()
	/// and thus doesn't need to be initialized in stream_encode().
	bool block_encoder_is_initialized;

	/// Block
	lzma_next_coder block_encoder;

	/// Options for the Block encoder
	lzma_block block_options;

	/// The filter chain currently in use
	lzma_filter filters[LZMA_FILTERS_MAX + 1];

	/// Index encoder. This is separate from Block encoder, because this
	/// doesn't take much memory, and when encoding multiple Streams
	/// with the same encoding options we avoid reallocating memory.
	lzma_next_coder index_encoder;

	/// Index to hold sizes of the Blocks
	lzma_index *index;

	/// Read position in buffer[]
	size_t buffer_pos;

	/// Total number of bytes in buffer[]
	size_t buffer_size;

	/// Buffer to hold Stream Header, Block Header, and Stream Footer.
	/// Block Header has biggest maximum size.
	uint8_t buffer[LZMA_BLOCK_HEADER_SIZE_MAX];
} lzma_stream_coder;

static lzma_ret
block_encoder_init(lzma_stream_coder *coder, const lzma_allocator *allocator)
{
	// Prepare the Block options. Even though Block encoder doesn't need
	// compressed_size, uncompressed_size, and header_size to be
	// initialized, it is a good idea to do it here, because this way
	// we catch if someone gave us Filter ID that cannot be used in
	// Blocks/Streams.
	coder->block_options.compressed_size = LZMA_VLI_UNKNOWN;
	coder->block_options.uncompressed_size = LZMA_VLI_UNKNOWN;

	return_if_error(lzma_block_header_size(&coder->block_options));

	// Initialize the actual Block encoder.
	return lzma_block_encoder_init(&coder->block_encoder, allocator,
			&coder->block_options);
}

static lzma_ret
stream_encode(void *coder_ptr, const lzma_allocator *allocator,
		const uint8_t *restrict in, size_t *restrict in_pos,
		size_t in_size, uint8_t *restrict out,
		size_t *restrict out_pos, size_t out_size, lzma_action action)
{
	lzma_stream_coder *coder = coder_ptr;

	// Main loop
	while (*out_pos < out_size)
	switch (coder->sequence) {
	case SEQ_STREAM_HEADER:
	case SEQ_BLOCK_HEADER:
	case SEQ_STREAM_FOOTER:
		lzma_bufcpy(coder->buffer, &coder->buffer_pos,
				coder->buffer_size, out, out_pos, out_size);
		if (coder->buffer_pos < coder->buffer_size)
			return LZMA_OK;

		if (coder->sequence == SEQ_STREAM_FOOTER)
			return LZMA_STREAM_END;

		coder->buffer_pos = 0;
		++coder->sequence;
		break;

	case SEQ_BLOCK_INIT: {
		if (*in_pos == in_size) {
			// If we are requested to flush or finish the current
			// Block, return LZMA_STREAM_END immediately since
			// there's nothing to do.
			if (action != LZMA_FINISH)
				return action == LZMA_RUN
						? LZMA_OK : LZMA_STREAM_END;

			// The application had used LZMA_FULL_FLUSH to finish
			// the previous Block, but now wants to finish without
			// encoding new data, or it is simply creating an
			// empty Stream with no Blocks.
			//
			// Initialize the Index encoder, and continue to
			// actually encoding the Index.
			return_if_error(lzma_index_encoder_init(
					&coder->index_encoder, allocator,
					coder->index));
			coder->sequence = SEQ_INDEX_ENCODE;
			break;
		}

		// Initialize the Block encoder unless it was already
		// initialized by stream_encoder_init() or
		// stream_encoder_update().
		if (!coder->block_encoder_is_initialized)
			return_if_error(block_encoder_init(coder, allocator));

		// Make it false so that we don't skip the initialization
		// with the next Block.
		coder->block_encoder_is_initialized = false;

		// Encode the Block Header. This shouldn't fail since we have
		// already initialized the Block encoder.
		if (lzma_block_header_encode(&coder->block_options,
				coder->buffer) != LZMA_OK)
			return LZMA_PROG_ERROR;

		coder->buffer_size = coder->block_options.header_size;
		coder->sequence = SEQ_BLOCK_HEADER;
		break;
	}

	case SEQ_BLOCK_ENCODE: {
		static const lzma_action convert[LZMA_ACTION_MAX + 1] = {
			LZMA_RUN,
			LZMA_SYNC_FLUSH,
			LZMA_FINISH,
			LZMA_FINISH,
			LZMA_FINISH,
		};

		const lzma_ret ret = coder->block_encoder.code(
				coder->block_encoder.coder, allocator,
				in, in_pos, in_size,
				out, out_pos, out_size, convert[action]);
		if (ret != LZMA_STREAM_END || action == LZMA_SYNC_FLUSH)
			return ret;

		// Add a new Index Record.
		const lzma_vli unpadded_size = lzma_block_unpadded_size(
				&coder->block_options);
		assert(unpadded_size != 0);
		return_if_error(lzma_index_append(coder->index, allocator,
				unpadded_size,
				coder->block_options.uncompressed_size));

		coder->sequence = SEQ_BLOCK_INIT;
		break;
	}

	case SEQ_INDEX_ENCODE: {
		// Call the Index encoder. It doesn't take any input, so
		// those pointers can be NULL.
		const lzma_ret ret = coder->index_encoder.code(
				coder->index_encoder.coder, allocator,
				NULL, NULL, 0,
				out, out_pos, out_size, LZMA_RUN);
		if (ret != LZMA_STREAM_END)
			return ret;

		// Encode the Stream Footer into coder->buffer.
		const lzma_stream_flags stream_flags = {
			.version = 0,
			.backward_size = lzma_index_size(coder->index),
			.check = coder->block_options.check,
		};

		if (lzma_stream_footer_encode(&stream_flags, coder->buffer)
				!= LZMA_OK)
			return LZMA_PROG_ERROR;

		coder->buffer_size = LZMA_STREAM_HEADER_SIZE;
		coder->sequence = SEQ_STREAM_FOOTER;
		break;
	}

	default:
		assert(0);
		return LZMA_PROG_ERROR;
	}

	return LZMA_OK;
}

static void
stream_encoder_end(void *coder_ptr, const lzma_allocator *allocator)
{
	lzma_stream_coder *coder = coder_ptr;

	lzma_next_end(&coder->block_encoder, allocator);
	lzma_next_end(&coder->index_encoder, allocator);
	lzma_index_end(coder->index, allocator);

	for (size_t i = 0; coder->filters[i].id != LZMA_VLI_UNKNOWN; ++i)
		lzma_free(coder->filters[i].options, allocator);

	lzma_free(coder, allocator);
	return;
}

static lzma_ret
stream_encoder_update(void *coder_ptr, const lzma_allocator *allocator,
		const lzma_filter *filters,
		const lzma_filter *reversed_filters)
{
	lzma_stream_coder *coder = coder_ptr;

	if (coder->sequence <= SEQ_BLOCK_INIT) {
		// There is no incomplete Block waiting to be finished,
		// thus we can change the whole filter chain. Start by
		// trying to initialize the Block encoder with the new
		// chain. This way we detect if the chain is valid.
		coder->block_encoder_is_initialized = false;
		coder->block_options.filters = (lzma_filter *)(filters);
		const lzma_ret ret = block_encoder_init(coder, allocator);
		coder->block_options.filters = coder->filters;
		if (ret != LZMA_OK)
			return ret;

		coder->block_encoder_is_initialized = true;

	} else if (coder->sequence <= SEQ_BLOCK_ENCODE) {
		// We are in the middle of a Block. Try to update only
		// the filter-specific options.
		return_if_error(coder->block_encoder.update(
				coder->block_encoder.coder, allocator,
				filters, reversed_filters));
	} else {
		// Trying to update the filter chain when we are already
		// encoding Index or Stream Footer.
		return LZMA_PROG_ERROR;
	}

	// Free the copy of the old chain and make a copy of the new chain.
	for (size_t i = 0; coder->filters[i].id != LZMA_VLI_UNKNOWN; ++i)
		lzma_free(coder->filters[i].options, allocator);

	return lzma_filters_copy(filters, coder->filters, allocator);
}

static lzma_ret
stream_encoder_init(lzma_next_coder *next, const lzma_allocator *allocator,
		const lzma_filter *filters, lzma_check check)
{
	lzma_next_coder_init(&stream_encoder_init, next, allocator);

	if (filters == NULL)
		return LZMA_PROG_ERROR;

	lzma_stream_coder *coder = next->coder;

	if (coder == NULL) {
		coder = lzma_alloc(sizeof(lzma_stream_coder), allocator);
		if (coder == NULL)
			return LZMA_MEM_ERROR;

		next->coder = coder;
		next->code = &stream_encode;
		next->end = &stream_encoder_end;
		next->update = &stream_encoder_update;

		coder->filters[0].id = LZMA_VLI_UNKNOWN;
		coder->block_encoder = LZMA_NEXT_CODER_INIT;
		coder->index_encoder = LZMA_NEXT_CODER_INIT;
		coder->index = NULL;
	}

	// Basic initializations
	coder->sequence = SEQ_STREAM_HEADER;
	coder->block_options.version = 0;
	coder->block_options.check = check;

	// Initialize the Index
	lzma_index_end(coder->index, allocator);
	coder->index = lzma_index_init(allocator);
	if (coder->index == NULL)
		return LZMA_MEM_ERROR;

	// Encode the Stream Header
	lzma_stream_flags stream_flags = {
		.version = 0,
		.check = check,
	};
	return_if_error(lzma_stream_header_encode(
			&stream_flags, coder->buffer));

	coder->buffer_pos = 0;
	coder->buffer_size = LZMA_STREAM_HEADER_SIZE;

	// Initialize the Block encoder. This way we detect unsupported
	// filter chains when initializing the Stream encoder instead of
	// giving an error after Stream Header has already written out.
	return stream_encoder_update(coder, allocator, filters, NULL);
}

extern LZMA_API(lzma_ret)
lzma_stream_encoder(lzma_stream *strm,
		const lzma_filter *filters, lzma_check check)
{
	lzma_next_strm_init(stream_encoder_init, strm, filters, check);

	strm->internal->supported_actions[LZMA_RUN] = true;
	strm->internal->supported_actions[LZMA_SYNC_FLUSH] = true;
	strm->internal->supported_actions[LZMA_FULL_FLUSH] = true;
	strm->internal->supported_actions[LZMA_FULL_BARRIER] = true;
	strm->internal->supported_actions[LZMA_FINISH] = true;

	return LZMA_OK;
}

/*** End of inlined file: stream_encoder.c ***/


/*** Start of inlined file: stream_encoder_mt.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       stream_encoder_mt.c
/// \brief      Multithreaded .xz Stream encoder
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////


/*** Start of inlined file: easy_preset.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       easy_preset.h
/// \brief      Preset handling for easy encoder and decoder
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

typedef struct {
	/// We need to keep the filters array available in case
	/// LZMA_FULL_FLUSH is used.
	lzma_filter filters[LZMA_FILTERS_MAX + 1];

	/// Options for LZMA2
	lzma_options_lzma opt_lzma;

	// Options for more filters can be added later, so this struct
	// is not ready to be put into the public API.

} lzma_options_easy;

/// Set *easy to the settings given by the preset. Returns true on error,
/// false on success.
extern bool lzma_easy_preset(lzma_options_easy *easy, uint32_t preset);

/*** End of inlined file: easy_preset.h ***/


/*** Start of inlined file: outqueue.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       outqueue.h
/// \brief      Output queue handling in multithreaded coding
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

/// Output buffer for a single thread
typedef struct {
	/// Pointer to the output buffer of lzma_outq.buf_size_max bytes
	uint8_t *buf;

	/// Amount of data written to buf
	size_t size;

	/// Additional size information
	lzma_vli unpadded_size;
	lzma_vli uncompressed_size;

	/// True when no more data will be written into this buffer.
	///
	/// \note       This is read by another thread and thus access
	///             to this variable needs a mutex.
	bool finished;

} lzma_outbuf;

typedef struct {
	/// Array of buffers that are used cyclically.
	lzma_outbuf *bufs;

	/// Memory allocated for all the buffers
	uint8_t *bufs_mem;

	/// Amount of buffer space available in each buffer
	size_t buf_size_max;

	/// Number of buffers allocated
	uint32_t bufs_allocated;

	/// Position in the bufs array. The next buffer to be taken
	/// into use is bufs[bufs_pos].
	uint32_t bufs_pos;

	/// Number of buffers in use
	uint32_t bufs_used;

	/// Position in the buffer in lzma_outq_read()
	size_t read_pos;

} lzma_outq;

/**
 * \brief       Calculate the memory usage of an output queue
 *
 * \return      Approximate memory usage in bytes or UINT64_MAX on error.
 */
extern uint64_t lzma_outq_memusage(uint64_t buf_size_max, uint32_t threads);

/// \brief      Initialize an output queue
///
/// \param      outq            Pointer to an output queue. Before calling
///                             this function the first time, *outq should
///                             have been zeroed with memzero() so that this
///                             function knows that there are no previous
///                             allocations to free.
/// \param      allocator       Pointer to allocator or NULL
/// \param      buf_size_max    Maximum amount of data that a single buffer
///                             in the queue may need to store.
/// \param      threads         Number of buffers that may be in use
///                             concurrently. Note that more than this number
///                             of buffers will actually get allocated to
///                             improve performance when buffers finish
///                             out of order.
///
/// \return     - LZMA_OK
///             - LZMA_MEM_ERROR
///
extern lzma_ret lzma_outq_init(
		lzma_outq *outq, const lzma_allocator *allocator,
		uint64_t buf_size_max, uint32_t threads);

/// \brief      Free the memory associated with the output queue
extern void lzma_outq_end(lzma_outq *outq, const lzma_allocator *allocator);

/// \brief      Get a new buffer
///
/// lzma_outq_has_buf() must be used to check that there is a buffer
/// available before calling lzma_outq_get_buf().
///
extern lzma_outbuf *lzma_outq_get_buf(lzma_outq *outq);

/// \brief      Test if there is data ready to be read
///
/// Call to this function must be protected with the same mutex that
/// is used to protect lzma_outbuf.finished.
///
extern bool lzma_outq_is_readable(const lzma_outq *outq);

/// \brief      Read finished data
///
/// \param      outq            Pointer to an output queue
/// \param      out             Beginning of the output buffer
/// \param      out_pos         The next byte will be written to
///                             out[*out_pos].
/// \param      out_size        Size of the out buffer; the first byte into
///                             which no data is written to is out[out_size].
/// \param      unpadded_size   Unpadded Size from the Block encoder
/// \param      uncompressed_size Uncompressed Size from the Block encoder
///
/// \return     - LZMA: All OK. Either no data was available or the buffer
///               being read didn't become empty yet.
///             - LZMA_STREAM_END: The buffer being read was finished.
///               *unpadded_size and *uncompressed_size were set.
///
/// \note       This reads lzma_outbuf.finished variables and thus call
///             to this function needs to be protected with a mutex.
///
extern lzma_ret lzma_outq_read(lzma_outq *restrict outq,
		uint8_t *restrict out, size_t *restrict out_pos,
		size_t out_size, lzma_vli *restrict unpadded_size,
		lzma_vli *restrict uncompressed_size);

/// \brief      Test if there is at least one buffer free
///
/// This must be used before getting a new buffer with lzma_outq_get_buf().
///
static inline bool
lzma_outq_has_buf(const lzma_outq *outq)
{
	return outq->bufs_used < outq->bufs_allocated;
}

/// \brief      Test if the queue is completely empty
static inline bool
lzma_outq_is_empty(const lzma_outq *outq)
{
	return outq->bufs_used == 0;
}

/*** End of inlined file: outqueue.h ***/

/// Maximum supported block size. This makes it simpler to prevent integer
/// overflows if we are given unusually large block size.
#define BLOCK_SIZE_MAX (UINT64_MAX / LZMA_THREADS_MAX)

typedef enum {
	/// Waiting for work.
	THR_IDLE,

	/// Encoding is in progress.
	THR_RUN,

	/// Encoding is in progress but no more input data will
	/// be read.
	THR_FINISH,

	/// The main thread wants the thread to stop whatever it was doing
	/// but not exit.
	THR_STOP,

	/// The main thread wants the thread to exit. We could use
	/// cancellation but since there's stopped anyway, this is lazier.
	THR_EXIT,

} worker_state;

typedef struct lzma_stream_coder_s lzma_stream_coder;

typedef struct worker_thread_s worker_thread;
struct worker_thread_s {
	worker_state state;

	/// Input buffer of coder->block_size bytes. The main thread will
	/// put new input into this and update in_size accordingly. Once
	/// no more input is coming, state will be set to THR_FINISH.
	uint8_t *in;

	/// Amount of data available in the input buffer. This is modified
	/// only by the main thread.
	size_t in_size;

	/// Output buffer for this thread. This is set by the main
	/// thread every time a new Block is started with this thread
	/// structure.
	lzma_outbuf *outbuf;

	/// Pointer to the main structure is needed when putting this
	/// thread back to the stack of free threads.
	lzma_stream_coder *coder;

	/// The allocator is set by the main thread. Since a copy of the
	/// pointer is kept here, the application must not change the
	/// allocator before calling lzma_end().
	const lzma_allocator *allocator;

	/// Amount of uncompressed data that has already been compressed.
	uint64_t progress_in;

	/// Amount of compressed data that is ready.
	uint64_t progress_out;

	/// Block encoder
	lzma_next_coder block_encoder;

	/// Compression options for this Block
	lzma_block block_options;

	/// Next structure in the stack of free worker threads.
	worker_thread *next;

	mythread_mutex mutex;
	mythread_cond cond;

	/// The ID of this thread is used to join the thread
	/// when it's not needed anymore.
	mythread thread_id;
};

struct lzma_stream_coder_s {
	enum {
		SEQ_STREAM_HEADER,
		SEQ_BLOCK,
		SEQ_INDEX,
		SEQ_STREAM_FOOTER,
	} sequence;

	/// Start a new Block every block_size bytes of input unless
	/// LZMA_FULL_FLUSH or LZMA_FULL_BARRIER is used earlier.
	size_t block_size;

	/// The filter chain currently in use
	lzma_filter filters[LZMA_FILTERS_MAX + 1];

	/// Index to hold sizes of the Blocks
	lzma_index *index;

	/// Index encoder
	lzma_next_coder index_encoder;

	/// Stream Flags for encoding the Stream Header and Stream Footer.
	lzma_stream_flags stream_flags;

	/// Buffer to hold Stream Header and Stream Footer.
	uint8_t header[LZMA_STREAM_HEADER_SIZE];

	/// Read position in header[]
	size_t header_pos;

	/// Output buffer queue for compressed data
	lzma_outq outq;

	/// Maximum wait time if cannot use all the input and cannot
	/// fill the output buffer. This is in milliseconds.
	uint32_t timeout;

	/// Error code from a worker thread
	lzma_ret thread_error;

	/// Array of allocated thread-specific structures
	worker_thread *threads;

	/// Number of structures in "threads" above. This is also the
	/// number of threads that will be created at maximum.
	uint32_t threads_max;

	/// Number of thread structures that have been initialized, and
	/// thus the number of worker threads actually created so far.
	uint32_t threads_initialized;

	/// Stack of free threads. When a thread finishes, it puts itself
	/// back into this stack. This starts as empty because threads
	/// are created only when actually needed.
	worker_thread *threads_free;

	/// The most recent worker thread to which the main thread writes
	/// the new input from the application.
	worker_thread *thr;

	/// Amount of uncompressed data in Blocks that have already
	/// been finished.
	uint64_t progress_in;

	/// Amount of compressed data in Stream Header + Blocks that
	/// have already been finished.
	uint64_t progress_out;

	mythread_mutex mutex;
	mythread_cond cond;
};

/// Tell the main thread that something has gone wrong.
static void
worker_error(worker_thread *thr, lzma_ret ret)
{
	assert(ret != LZMA_OK);
	assert(ret != LZMA_STREAM_END);

	mythread_sync(thr->coder->mutex) {
		if (thr->coder->thread_error == LZMA_OK)
			thr->coder->thread_error = ret;

		mythread_cond_signal(&thr->coder->cond);
	}

	return;
}

static worker_state
worker_encode(worker_thread *thr, worker_state state)
{
	assert(thr->progress_in == 0);
	assert(thr->progress_out == 0);

	// Set the Block options.
	thr->block_options = (lzma_block){
		.version = 0,
		.check = thr->coder->stream_flags.check,
		.compressed_size = thr->coder->outq.buf_size_max,
		.uncompressed_size = thr->coder->block_size,

		// TODO: To allow changing the filter chain, the filters
		// array must be copied to each worker_thread.
		.filters = thr->coder->filters,
	};

	// Calculate maximum size of the Block Header. This amount is
	// reserved in the beginning of the buffer so that Block Header
	// along with Compressed Size and Uncompressed Size can be
	// written there.
	lzma_ret ret = lzma_block_header_size(&thr->block_options);
	if (ret != LZMA_OK) {
		worker_error(thr, ret);
		return THR_STOP;
	}

	// Initialize the Block encoder.
	ret = lzma_block_encoder_init(&thr->block_encoder,
			thr->allocator, &thr->block_options);
	if (ret != LZMA_OK) {
		worker_error(thr, ret);
		return THR_STOP;
	}

	size_t in_pos = 0;
	size_t in_size = 0;

	thr->outbuf->size = thr->block_options.header_size;
	const size_t out_size = thr->coder->outq.buf_size_max;

	do {
		mythread_sync(thr->mutex) {
			// Store in_pos and out_pos into *thr so that
			// an application may read them via
			// lzma_get_progress() to get progress information.
			//
			// NOTE: These aren't updated when the encoding
			// finishes. Instead, the final values are taken
			// later from thr->outbuf.
			thr->progress_in = in_pos;
			thr->progress_out = thr->outbuf->size;

			while (in_size == thr->in_size
					&& thr->state == THR_RUN)
				mythread_cond_wait(&thr->cond, &thr->mutex);

			state = thr->state;
			in_size = thr->in_size;
		}

		// Return if we were asked to stop or exit.
		if (state >= THR_STOP)
			return state;

		lzma_action action = state == THR_FINISH
				? LZMA_FINISH : LZMA_RUN;

		// Limit the amount of input given to the Block encoder
		// at once. This way this thread can react fairly quickly
		// if the main thread wants us to stop or exit.
		static const size_t in_chunk_max = 16384;
		size_t in_limit = in_size;
		if (in_size - in_pos > in_chunk_max) {
			in_limit = in_pos + in_chunk_max;
			action = LZMA_RUN;
		}

		ret = thr->block_encoder.code(
				thr->block_encoder.coder, thr->allocator,
				thr->in, &in_pos, in_limit, thr->outbuf->buf,
				&thr->outbuf->size, out_size, action);
	} while (ret == LZMA_OK && thr->outbuf->size < out_size);

	switch (ret) {
	case LZMA_STREAM_END:
		assert(state == THR_FINISH);

		// Encode the Block Header. By doing it after
		// the compression, we can store the Compressed Size
		// and Uncompressed Size fields.
		ret = lzma_block_header_encode(&thr->block_options,
				thr->outbuf->buf);
		if (ret != LZMA_OK) {
			worker_error(thr, ret);
			return THR_STOP;
		}

		break;

	case LZMA_OK:
		// The data was incompressible. Encode it using uncompressed
		// LZMA2 chunks.
		//
		// First wait that we have gotten all the input.
		mythread_sync(thr->mutex) {
			while (thr->state == THR_RUN)
				mythread_cond_wait(&thr->cond, &thr->mutex);

			state = thr->state;
			in_size = thr->in_size;
		}

		if (state >= THR_STOP)
			return state;

		// Do the encoding. This takes care of the Block Header too.
		thr->outbuf->size = 0;
		ret = lzma_block_uncomp_encode(&thr->block_options,
				thr->in, in_size, thr->outbuf->buf,
				&thr->outbuf->size, out_size);

		// It shouldn't fail.
		if (ret != LZMA_OK) {
			worker_error(thr, LZMA_PROG_ERROR);
			return THR_STOP;
		}

		break;

	default:
		worker_error(thr, ret);
		return THR_STOP;
	}

	// Set the size information that will be read by the main thread
	// to write the Index field.
	thr->outbuf->unpadded_size
			= lzma_block_unpadded_size(&thr->block_options);
	assert(thr->outbuf->unpadded_size != 0);
	thr->outbuf->uncompressed_size = thr->block_options.uncompressed_size;

	return THR_FINISH;
}

static MYTHREAD_RET_TYPE
worker_start(void *thr_ptr)
{
	worker_thread *thr = thr_ptr;
	worker_state state = THR_IDLE; // Init to silence a warning

	while (true) {
		// Wait for work.
		mythread_sync(thr->mutex) {
			while (true) {
				// The thread is already idle so if we are
				// requested to stop, just set the state.
				if (thr->state == THR_STOP) {
					thr->state = THR_IDLE;
					mythread_cond_signal(&thr->cond);
				}

				state = thr->state;
				if (state != THR_IDLE)
					break;

				mythread_cond_wait(&thr->cond, &thr->mutex);
			}
		}

		assert(state != THR_IDLE);
		assert(state != THR_STOP);

		if (state <= THR_FINISH)
			state = worker_encode(thr, state);

		if (state == THR_EXIT)
			break;

		// Mark the thread as idle unless the main thread has
		// told us to exit. Signal is needed for the case
		// where the main thread is waiting for the threads to stop.
		mythread_sync(thr->mutex) {
			if (thr->state != THR_EXIT) {
				thr->state = THR_IDLE;
				mythread_cond_signal(&thr->cond);
			}
		}

		mythread_sync(thr->coder->mutex) {
			// Mark the output buffer as finished if
			// no errors occurred.
			thr->outbuf->finished = state == THR_FINISH;

			// Update the main progress info.
			thr->coder->progress_in
					+= thr->outbuf->uncompressed_size;
			thr->coder->progress_out += thr->outbuf->size;
			thr->progress_in = 0;
			thr->progress_out = 0;

			// Return this thread to the stack of free threads.
			thr->next = thr->coder->threads_free;
			thr->coder->threads_free = thr;

			mythread_cond_signal(&thr->coder->cond);
		}
	}

	// Exiting, free the resources.
	mythread_mutex_destroy(&thr->mutex);
	mythread_cond_destroy(&thr->cond);

	lzma_next_end(&thr->block_encoder, thr->allocator);
	lzma_free(thr->in, thr->allocator);
	return MYTHREAD_RET_VALUE;
}

/// Make the threads stop but not exit. Optionally wait for them to stop.
static void
threads_stop(lzma_stream_coder *coder, bool wait_for_threads)
{
	// Tell the threads to stop.
	for (uint32_t i = 0; i < coder->threads_initialized; ++i) {
		mythread_sync(coder->threads[i].mutex) {
			coder->threads[i].state = THR_STOP;
			mythread_cond_signal(&coder->threads[i].cond);
		}
	}

	if (!wait_for_threads)
		return;

	// Wait for the threads to settle in the idle state.
	for (uint32_t i = 0; i < coder->threads_initialized; ++i) {
		mythread_sync(coder->threads[i].mutex) {
			while (coder->threads[i].state != THR_IDLE)
				mythread_cond_wait(&coder->threads[i].cond,
						&coder->threads[i].mutex);
		}
	}

	return;
}

/// Stop the threads and free the resources associated with them.
/// Wait until the threads have exited.
static void
threads_end(lzma_stream_coder *coder, const lzma_allocator *allocator)
{
	for (uint32_t i = 0; i < coder->threads_initialized; ++i) {
		mythread_sync(coder->threads[i].mutex) {
			coder->threads[i].state = THR_EXIT;
			mythread_cond_signal(&coder->threads[i].cond);
		}
	}

	for (uint32_t i = 0; i < coder->threads_initialized; ++i) {
		int ret = mythread_join(coder->threads[i].thread_id);
		assert(ret == 0);
		(void)ret;
	}

	lzma_free(coder->threads, allocator);
	return;
}

/// Initialize a new worker_thread structure and create a new thread.
static lzma_ret
initialize_new_thread(lzma_stream_coder *coder,
		const lzma_allocator *allocator)
{
	worker_thread *thr = &coder->threads[coder->threads_initialized];

	thr->in = lzma_alloc(coder->block_size, allocator);
	if (thr->in == NULL)
		return LZMA_MEM_ERROR;

	if (mythread_mutex_init(&thr->mutex))
		goto error_mutex;

	if (mythread_cond_init(&thr->cond))
		goto error_cond;

	thr->state = THR_IDLE;
	thr->allocator = allocator;
	thr->coder = coder;
	thr->progress_in = 0;
	thr->progress_out = 0;
	thr->block_encoder = LZMA_NEXT_CODER_INIT;

	if (mythread_create(&thr->thread_id, &worker_start, thr))
		goto error_thread;

	++coder->threads_initialized;
	coder->thr = thr;

	return LZMA_OK;

error_thread:
	mythread_cond_destroy(&thr->cond);

error_cond:
	mythread_mutex_destroy(&thr->mutex);

error_mutex:
	lzma_free(thr->in, allocator);
	return LZMA_MEM_ERROR;
}

static lzma_ret
get_thread(lzma_stream_coder *coder, const lzma_allocator *allocator)
{
	// If there are no free output subqueues, there is no
	// point to try getting a thread.
	if (!lzma_outq_has_buf(&coder->outq))
		return LZMA_OK;

	// If there is a free structure on the stack, use it.
	mythread_sync(coder->mutex) {
		if (coder->threads_free != NULL) {
			coder->thr = coder->threads_free;
			coder->threads_free = coder->threads_free->next;
		}
	}

	if (coder->thr == NULL) {
		// If there are no uninitialized structures left, return.
		if (coder->threads_initialized == coder->threads_max)
			return LZMA_OK;

		// Initialize a new thread.
		return_if_error(initialize_new_thread(coder, allocator));
	}

	// Reset the parts of the thread state that have to be done
	// in the main thread.
	mythread_sync(coder->thr->mutex) {
		coder->thr->state = THR_RUN;
		coder->thr->in_size = 0;
		coder->thr->outbuf = lzma_outq_get_buf(&coder->outq);
		mythread_cond_signal(&coder->thr->cond);
	}

	return LZMA_OK;
}

static lzma_ret
stream_encode_in(lzma_stream_coder *coder, const lzma_allocator *allocator,
		const uint8_t *restrict in, size_t *restrict in_pos,
		size_t in_size, lzma_action action)
{
	while (*in_pos < in_size
			|| (coder->thr != NULL && action != LZMA_RUN)) {
		if (coder->thr == NULL) {
			// Get a new thread.
			const lzma_ret ret = get_thread(coder, allocator);
			if (coder->thr == NULL)
				return ret;
		}

		// Copy the input data to thread's buffer.
		size_t thr_in_size = coder->thr->in_size;
		lzma_bufcpy(in, in_pos, in_size, coder->thr->in,
				&thr_in_size, coder->block_size);

		// Tell the Block encoder to finish if
		//  - it has got block_size bytes of input; or
		//  - all input was used and LZMA_FINISH, LZMA_FULL_FLUSH,
		//    or LZMA_FULL_BARRIER was used.
		//
		// TODO: LZMA_SYNC_FLUSH and LZMA_SYNC_BARRIER.
		const bool finish = thr_in_size == coder->block_size
				|| (*in_pos == in_size && action != LZMA_RUN);

		bool block_error = false;

		mythread_sync(coder->thr->mutex) {
			if (coder->thr->state == THR_IDLE) {
				// Something has gone wrong with the Block
				// encoder. It has set coder->thread_error
				// which we will read a few lines later.
				block_error = true;
			} else {
				// Tell the Block encoder its new amount
				// of input and update the state if needed.
				coder->thr->in_size = thr_in_size;

				if (finish)
					coder->thr->state = THR_FINISH;

				mythread_cond_signal(&coder->thr->cond);
			}
		}

		if (block_error) {
			lzma_ret ret;

			mythread_sync(coder->mutex) {
				ret = coder->thread_error;
			}

			return ret;
		}

		if (finish)
			coder->thr = NULL;
	}

	return LZMA_OK;
}

/// Wait until more input can be consumed, more output can be read, or
/// an optional timeout is reached.
static bool
wait_for_work(lzma_stream_coder *coder, mythread_condtime *wait_abs,
		bool *has_blocked, bool has_input)
{
	if (coder->timeout != 0 && !*has_blocked) {
		// Every time when stream_encode_mt() is called via
		// lzma_code(), *has_blocked starts as false. We set it
		// to true here and calculate the absolute time when
		// we must return if there's nothing to do.
		//
		// The idea of *has_blocked is to avoid unneeded calls
		// to mythread_condtime_set(), which may do a syscall
		// depending on the operating system.
		*has_blocked = true;
		mythread_condtime_set(wait_abs, &coder->cond, coder->timeout);
	}

	bool timed_out = false;

	mythread_sync(coder->mutex) {
		// There are four things that we wait. If one of them
		// becomes possible, we return.
		//  - If there is input left, we need to get a free
		//    worker thread and an output buffer for it.
		//  - Data ready to be read from the output queue.
		//  - A worker thread indicates an error.
		//  - Time out occurs.
		while ((!has_input || coder->threads_free == NULL
					|| !lzma_outq_has_buf(&coder->outq))
				&& !lzma_outq_is_readable(&coder->outq)
				&& coder->thread_error == LZMA_OK
				&& !timed_out) {
			if (coder->timeout != 0)
				timed_out = mythread_cond_timedwait(
						&coder->cond, &coder->mutex,
						wait_abs) != 0;
			else
				mythread_cond_wait(&coder->cond,
						&coder->mutex);
		}
	}

	return timed_out;
}

static lzma_ret
stream_encode_mt(void *coder_ptr, const lzma_allocator *allocator,
		const uint8_t *restrict in, size_t *restrict in_pos,
		size_t in_size, uint8_t *restrict out,
		size_t *restrict out_pos, size_t out_size, lzma_action action)
{
	lzma_stream_coder *coder = coder_ptr;

	switch (coder->sequence) {
	case SEQ_STREAM_HEADER:
		lzma_bufcpy(coder->header, &coder->header_pos,
				sizeof(coder->header),
				out, out_pos, out_size);
		if (coder->header_pos < sizeof(coder->header))
			return LZMA_OK;

		coder->header_pos = 0;
		coder->sequence = SEQ_BLOCK;

	// Fall through

	case SEQ_BLOCK: {
		// Initialized to silence warnings.
		lzma_vli unpadded_size = 0;
		lzma_vli uncompressed_size = 0;
		lzma_ret ret = LZMA_OK;

		// These are for wait_for_work().
		bool has_blocked = false;
		mythread_condtime wait_abs;

		while (true) {
			mythread_sync(coder->mutex) {
				// Check for Block encoder errors.
				ret = coder->thread_error;
				if (ret != LZMA_OK) {
					assert(ret != LZMA_STREAM_END);
					break; // Break out of mythread_sync.
				}

				// Try to read compressed data to out[].
				ret = lzma_outq_read(&coder->outq,
						out, out_pos, out_size,
						&unpadded_size,
						&uncompressed_size);
			}

			if (ret == LZMA_STREAM_END) {
				// End of Block. Add it to the Index.
				ret = lzma_index_append(coder->index,
						allocator, unpadded_size,
						uncompressed_size);

				// If we didn't fill the output buffer yet,
				// try to read more data. Maybe the next
				// outbuf has been finished already too.
				if (*out_pos < out_size)
					continue;
			}

			if (ret != LZMA_OK) {
				// coder->thread_error was set or
				// lzma_index_append() failed.
				threads_stop(coder, false);
				return ret;
			}

			// Try to give uncompressed data to a worker thread.
			ret = stream_encode_in(coder, allocator,
					in, in_pos, in_size, action);
			if (ret != LZMA_OK) {
				threads_stop(coder, false);
				return ret;
			}

			// See if we should wait or return.
			//
			// TODO: LZMA_SYNC_FLUSH and LZMA_SYNC_BARRIER.
			if (*in_pos == in_size) {
				// LZMA_RUN: More data is probably coming
				// so return to let the caller fill the
				// input buffer.
				if (action == LZMA_RUN)
					return LZMA_OK;

				// LZMA_FULL_BARRIER: The same as with
				// LZMA_RUN but tell the caller that the
				// barrier was completed.
				if (action == LZMA_FULL_BARRIER)
					return LZMA_STREAM_END;

				// Finishing or flushing isn't completed until
				// all input data has been encoded and copied
				// to the output buffer.
				if (lzma_outq_is_empty(&coder->outq)) {
					// LZMA_FINISH: Continue to encode
					// the Index field.
					if (action == LZMA_FINISH)
						break;

					// LZMA_FULL_FLUSH: Return to tell
					// the caller that flushing was
					// completed.
					if (action == LZMA_FULL_FLUSH)
						return LZMA_STREAM_END;
				}
			}

			// Return if there is no output space left.
			// This check must be done after testing the input
			// buffer, because we might want to use a different
			// return code.
			if (*out_pos == out_size)
				return LZMA_OK;

			// Neither in nor out has been used completely.
			// Wait until there's something we can do.
			if (wait_for_work(coder, &wait_abs, &has_blocked,
					*in_pos < in_size))
				return LZMA_TIMED_OUT;
		}

		// All Blocks have been encoded and the threads have stopped.
		// Prepare to encode the Index field.
		return_if_error(lzma_index_encoder_init(
				&coder->index_encoder, allocator,
				coder->index));
		coder->sequence = SEQ_INDEX;

		// Update the progress info to take the Index and
		// Stream Footer into account. Those are very fast to encode
		// so in terms of progress information they can be thought
		// to be ready to be copied out.
		coder->progress_out += lzma_index_size(coder->index)
				+ LZMA_STREAM_HEADER_SIZE;
	}

	// Fall through

	case SEQ_INDEX: {
		// Call the Index encoder. It doesn't take any input, so
		// those pointers can be NULL.
		const lzma_ret ret = coder->index_encoder.code(
				coder->index_encoder.coder, allocator,
				NULL, NULL, 0,
				out, out_pos, out_size, LZMA_RUN);
		if (ret != LZMA_STREAM_END)
			return ret;

		// Encode the Stream Footer into coder->buffer.
		coder->stream_flags.backward_size
				= lzma_index_size(coder->index);
		if (lzma_stream_footer_encode(&coder->stream_flags,
				coder->header) != LZMA_OK)
			return LZMA_PROG_ERROR;

		coder->sequence = SEQ_STREAM_FOOTER;
	}

	// Fall through

	case SEQ_STREAM_FOOTER:
		lzma_bufcpy(coder->header, &coder->header_pos,
				sizeof(coder->header),
				out, out_pos, out_size);
		return coder->header_pos < sizeof(coder->header)
				? LZMA_OK : LZMA_STREAM_END;
	}

	assert(0);
	return LZMA_PROG_ERROR;
}

static void
stream_encoder_mt_end(void *coder_ptr, const lzma_allocator *allocator)
{
	lzma_stream_coder *coder = coder_ptr;

	// Threads must be killed before the output queue can be freed.
	threads_end(coder, allocator);
	lzma_outq_end(&coder->outq, allocator);

	for (size_t i = 0; coder->filters[i].id != LZMA_VLI_UNKNOWN; ++i)
		lzma_free(coder->filters[i].options, allocator);

	lzma_next_end(&coder->index_encoder, allocator);
	lzma_index_end(coder->index, allocator);

	mythread_cond_destroy(&coder->cond);
	mythread_mutex_destroy(&coder->mutex);

	lzma_free(coder, allocator);
	return;
}

/// Options handling for lzma_stream_encoder_mt_init() and
/// lzma_stream_encoder_mt_memusage()
static lzma_ret
get_options(const lzma_mt *options, lzma_options_easy *opt_easy,
		const lzma_filter **filters, uint64_t *block_size,
		uint64_t *outbuf_size_max)
{
	// Validate some of the options.
	if (options == NULL)
		return LZMA_PROG_ERROR;

	if (options->flags != 0 || options->threads == 0
			|| options->threads > LZMA_THREADS_MAX)
		return LZMA_OPTIONS_ERROR;

	if (options->filters != NULL) {
		// Filter chain was given, use it as is.
		*filters = options->filters;
	} else {
		// Use a preset.
		if (lzma_easy_preset(opt_easy, options->preset))
			return LZMA_OPTIONS_ERROR;

		*filters = opt_easy->filters;
	}

	// Block size
	if (options->block_size > 0) {
		if (options->block_size > BLOCK_SIZE_MAX)
			return LZMA_OPTIONS_ERROR;

		*block_size = options->block_size;
	} else {
		// Determine the Block size from the filter chain.
		*block_size = lzma_mt_block_size(*filters);
		if (*block_size == 0)
			return LZMA_OPTIONS_ERROR;

		assert(*block_size <= BLOCK_SIZE_MAX);
	}

	// Calculate the maximum amount output that a single output buffer
	// may need to hold. This is the same as the maximum total size of
	// a Block.
	*outbuf_size_max = lzma_block_buffer_bound64(*block_size);
	if (*outbuf_size_max == 0)
		return LZMA_MEM_ERROR;

	return LZMA_OK;
}

static void
get_progress(void *coder_ptr, uint64_t *progress_in, uint64_t *progress_out)
{
	lzma_stream_coder *coder = coder_ptr;

	// Lock coder->mutex to prevent finishing threads from moving their
	// progress info from the worker_thread structure to lzma_stream_coder.
	mythread_sync(coder->mutex) {
		*progress_in = coder->progress_in;
		*progress_out = coder->progress_out;

		for (size_t i = 0; i < coder->threads_initialized; ++i) {
			mythread_sync(coder->threads[i].mutex) {
				*progress_in += coder->threads[i].progress_in;
				*progress_out += coder->threads[i]
						.progress_out;
			}
		}
	}

	return;
}

static lzma_ret
stream_encoder_mt_init(lzma_next_coder *next, const lzma_allocator *allocator,
		const lzma_mt *options)
{
	lzma_next_coder_init(&stream_encoder_mt_init, next, allocator);

	// Get the filter chain.
	lzma_options_easy easy;
	const lzma_filter *filters;
	uint64_t block_size;
	uint64_t outbuf_size_max;
	return_if_error(get_options(options, &easy, &filters,
			&block_size, &outbuf_size_max));

#if SIZE_MAX < UINT64_MAX
	if (block_size > SIZE_MAX)
		return LZMA_MEM_ERROR;
#endif

	// Validate the filter chain so that we can give an error in this
	// function instead of delaying it to the first call to lzma_code().
	// The memory usage calculation verifies the filter chain as
	// a side effect so we take advantage of that.
	if (lzma_raw_encoder_memusage(filters) == UINT64_MAX)
		return LZMA_OPTIONS_ERROR;

	// Validate the Check ID.
	if ((unsigned int)(options->check) > LZMA_CHECK_ID_MAX)
		return LZMA_PROG_ERROR;

	if (!lzma_check_is_supported(options->check))
		return LZMA_UNSUPPORTED_CHECK;

	// Allocate and initialize the base structure if needed.
	lzma_stream_coder *coder = next->coder;
	if (coder == NULL) {
		coder = lzma_alloc(sizeof(lzma_stream_coder), allocator);
		if (coder == NULL)
			return LZMA_MEM_ERROR;

		next->coder = coder;

		// For the mutex and condition variable initializations
		// the error handling has to be done here because
		// stream_encoder_mt_end() doesn't know if they have
		// already been initialized or not.
		if (mythread_mutex_init(&coder->mutex)) {
			lzma_free(coder, allocator);
			next->coder = NULL;
			return LZMA_MEM_ERROR;
		}

		if (mythread_cond_init(&coder->cond)) {
			mythread_mutex_destroy(&coder->mutex);
			lzma_free(coder, allocator);
			next->coder = NULL;
			return LZMA_MEM_ERROR;
		}

		next->code = &stream_encode_mt;
		next->end = &stream_encoder_mt_end;
		next->get_progress = &get_progress;
// 		next->update = &stream_encoder_mt_update;

		coder->filters[0].id = LZMA_VLI_UNKNOWN;
		coder->index_encoder = LZMA_NEXT_CODER_INIT;
		coder->index = NULL;
		memzero(&coder->outq, sizeof(coder->outq));
		coder->threads = NULL;
		coder->threads_max = 0;
		coder->threads_initialized = 0;
	}

	// Basic initializations
	coder->sequence = SEQ_STREAM_HEADER;
	coder->block_size = (size_t)(block_size);
	coder->thread_error = LZMA_OK;
	coder->thr = NULL;

	// Allocate the thread-specific base structures.
	assert(options->threads > 0);
	if (coder->threads_max != options->threads) {
		threads_end(coder, allocator);

		coder->threads = NULL;
		coder->threads_max = 0;

		coder->threads_initialized = 0;
		coder->threads_free = NULL;

		coder->threads = lzma_alloc(
				options->threads * sizeof(worker_thread),
				allocator);
		if (coder->threads == NULL)
			return LZMA_MEM_ERROR;

		coder->threads_max = options->threads;
	} else {
		// Reuse the old structures and threads. Tell the running
		// threads to stop and wait until they have stopped.
		threads_stop(coder, true);
	}

	// Output queue
	return_if_error(lzma_outq_init(&coder->outq, allocator,
			outbuf_size_max, options->threads));

	// Timeout
	coder->timeout = options->timeout;

	// Free the old filter chain and copy the new one.
	for (size_t i = 0; coder->filters[i].id != LZMA_VLI_UNKNOWN; ++i)
		lzma_free(coder->filters[i].options, allocator);

	return_if_error(lzma_filters_copy(
			filters, coder->filters, allocator));

	// Index
	lzma_index_end(coder->index, allocator);
	coder->index = lzma_index_init(allocator);
	if (coder->index == NULL)
		return LZMA_MEM_ERROR;

	// Stream Header
	coder->stream_flags.version = 0;
	coder->stream_flags.check = options->check;
	return_if_error(lzma_stream_header_encode(
			&coder->stream_flags, coder->header));

	coder->header_pos = 0;

	// Progress info
	coder->progress_in = 0;
	coder->progress_out = LZMA_STREAM_HEADER_SIZE;

	return LZMA_OK;
}

extern LZMA_API(lzma_ret)
lzma_stream_encoder_mt(lzma_stream *strm, const lzma_mt *options)
{
	lzma_next_strm_init(stream_encoder_mt_init, strm, options);

	strm->internal->supported_actions[LZMA_RUN] = true;
// 	strm->internal->supported_actions[LZMA_SYNC_FLUSH] = true;
	strm->internal->supported_actions[LZMA_FULL_FLUSH] = true;
	strm->internal->supported_actions[LZMA_FULL_BARRIER] = true;
	strm->internal->supported_actions[LZMA_FINISH] = true;

	return LZMA_OK;
}

// This function name is a monster but it's consistent with the older
// monster names. :-( 31 chars is the max that C99 requires so in that
// sense it's not too long. ;-)
extern LZMA_API(uint64_t)
lzma_stream_encoder_mt_memusage(const lzma_mt *options)
{
	lzma_options_easy easy;
	const lzma_filter *filters;
	uint64_t block_size;
	uint64_t outbuf_size_max;

	if (get_options(options, &easy, &filters, &block_size,
			&outbuf_size_max) != LZMA_OK)
		return UINT64_MAX;

	// Memory usage of the input buffers
	const uint64_t inbuf_memusage = options->threads * block_size;

	// Memory usage of the filter encoders
	uint64_t filters_memusage = lzma_raw_encoder_memusage(filters);
	if (filters_memusage == UINT64_MAX)
		return UINT64_MAX;

	filters_memusage *= options->threads;

	// Memory usage of the output queue
	const uint64_t outq_memusage = lzma_outq_memusage(
			outbuf_size_max, options->threads);
	if (outq_memusage == UINT64_MAX)
		return UINT64_MAX;

	// Sum them with overflow checking.
	uint64_t total_memusage = LZMA_MEMUSAGE_BASE
			+ sizeof(lzma_stream_coder)
			+ options->threads * sizeof(worker_thread);

	if (UINT64_MAX - total_memusage < inbuf_memusage)
		return UINT64_MAX;

	total_memusage += inbuf_memusage;

	if (UINT64_MAX - total_memusage < filters_memusage)
		return UINT64_MAX;

	total_memusage += filters_memusage;

	if (UINT64_MAX - total_memusage < outq_memusage)
		return UINT64_MAX;

	return total_memusage + outq_memusage;
}

/*** End of inlined file: stream_encoder_mt.c ***/


/*** Start of inlined file: stream_flags_common.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       stream_flags_common.c
/// \brief      Common stuff for Stream flags coders
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

const uint8_t lzma_header_magic[6] = { 0xFD, 0x37, 0x7A, 0x58, 0x5A, 0x00 };
const uint8_t lzma_footer_magic[2] = { 0x59, 0x5A };

extern LZMA_API(lzma_ret)
lzma_stream_flags_compare(
		const lzma_stream_flags *a, const lzma_stream_flags *b)
{
	// We can compare only version 0 structures.
	if (a->version != 0 || b->version != 0)
		return LZMA_OPTIONS_ERROR;

	// Check type
	if ((unsigned int)(a->check) > LZMA_CHECK_ID_MAX
			|| (unsigned int)(b->check) > LZMA_CHECK_ID_MAX)
		return LZMA_PROG_ERROR;

	if (a->check != b->check)
		return LZMA_DATA_ERROR;

	// Backward Sizes are compared only if they are known in both.
	if (a->backward_size != LZMA_VLI_UNKNOWN
			&& b->backward_size != LZMA_VLI_UNKNOWN) {
		if (!is_backward_size_valid(a) || !is_backward_size_valid(b))
			return LZMA_PROG_ERROR;

		if (a->backward_size != b->backward_size)
			return LZMA_DATA_ERROR;
	}

	return LZMA_OK;
}

/*** End of inlined file: stream_flags_common.c ***/


/*** Start of inlined file: stream_flags_decoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       stream_flags_decoder.c
/// \brief      Decodes Stream Header and Stream Footer from .xz files
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

static bool
stream_flags_decode(lzma_stream_flags *options, const uint8_t *in)
{
	// Reserved bits must be unset.
	if (in[0] != 0x00 || (in[1] & 0xF0))
		return true;

	options->version = 0;
	options->check = in[1] & 0x0F;

	return false;
}

extern LZMA_API(lzma_ret)
lzma_stream_header_decode(lzma_stream_flags *options, const uint8_t *in)
{
	// Magic
	if (memcmp(in, lzma_header_magic, sizeof(lzma_header_magic)) != 0)
		return LZMA_FORMAT_ERROR;

	// Verify the CRC32 so we can distinguish between corrupt
	// and unsupported files.
	const uint32_t crc = lzma_crc32(in + sizeof(lzma_header_magic),
			LZMA_STREAM_FLAGS_SIZE, 0);
	if (crc != read32le(in + sizeof(lzma_header_magic)
			+ LZMA_STREAM_FLAGS_SIZE))
		return LZMA_DATA_ERROR;

	// Stream Flags
	if (stream_flags_decode(options, in + sizeof(lzma_header_magic)))
		return LZMA_OPTIONS_ERROR;

	// Set Backward Size to indicate unknown value. That way
	// lzma_stream_flags_compare() can be used to compare Stream Header
	// and Stream Footer while keeping it useful also for comparing
	// two Stream Footers.
	options->backward_size = LZMA_VLI_UNKNOWN;

	return LZMA_OK;
}

extern LZMA_API(lzma_ret)
lzma_stream_footer_decode(lzma_stream_flags *options, const uint8_t *in)
{
	// Magic
	if (memcmp(in + sizeof(uint32_t) * 2 + LZMA_STREAM_FLAGS_SIZE,
			lzma_footer_magic, sizeof(lzma_footer_magic)) != 0)
		return LZMA_FORMAT_ERROR;

	// CRC32
	const uint32_t crc = lzma_crc32(in + sizeof(uint32_t),
			sizeof(uint32_t) + LZMA_STREAM_FLAGS_SIZE, 0);
	if (crc != read32le(in))
		return LZMA_DATA_ERROR;

	// Stream Flags
	if (stream_flags_decode(options, in + sizeof(uint32_t) * 2))
		return LZMA_OPTIONS_ERROR;

	// Backward Size
	options->backward_size = read32le(in + sizeof(uint32_t));
	options->backward_size = (options->backward_size + 1) * 4;

	return LZMA_OK;
}

/*** End of inlined file: stream_flags_decoder.c ***/


/*** Start of inlined file: stream_flags_encoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       stream_flags_encoder.c
/// \brief      Encodes Stream Header and Stream Footer for .xz files
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

static bool
stream_flags_encode(const lzma_stream_flags *options, uint8_t *out)
{
	if ((unsigned int)(options->check) > LZMA_CHECK_ID_MAX)
		return true;

	out[0] = 0x00;
	out[1] = options->check;

	return false;
}

extern LZMA_API(lzma_ret)
lzma_stream_header_encode(const lzma_stream_flags *options, uint8_t *out)
{
	assert(sizeof(lzma_header_magic) + LZMA_STREAM_FLAGS_SIZE
			+ 4 == LZMA_STREAM_HEADER_SIZE);

	if (options->version != 0)
		return LZMA_OPTIONS_ERROR;

	// Magic
	memcpy(out, lzma_header_magic, sizeof(lzma_header_magic));

	// Stream Flags
	if (stream_flags_encode(options, out + sizeof(lzma_header_magic)))
		return LZMA_PROG_ERROR;

	// CRC32 of the Stream Header
	const uint32_t crc = lzma_crc32(out + sizeof(lzma_header_magic),
			LZMA_STREAM_FLAGS_SIZE, 0);

	write32le(out + sizeof(lzma_header_magic) + LZMA_STREAM_FLAGS_SIZE,
			crc);

	return LZMA_OK;
}

extern LZMA_API(lzma_ret)
lzma_stream_footer_encode(const lzma_stream_flags *options, uint8_t *out)
{
	assert(2 * 4 + LZMA_STREAM_FLAGS_SIZE + sizeof(lzma_footer_magic)
			== LZMA_STREAM_HEADER_SIZE);

	if (options->version != 0)
		return LZMA_OPTIONS_ERROR;

	// Backward Size
	if (!is_backward_size_valid(options))
		return LZMA_PROG_ERROR;

	write32le(out + 4, options->backward_size / 4 - 1);

	// Stream Flags
	if (stream_flags_encode(options, out + 2 * 4))
		return LZMA_PROG_ERROR;

	// CRC32
	const uint32_t crc = lzma_crc32(
			out + 4, 4 + LZMA_STREAM_FLAGS_SIZE, 0);

	write32le(out, crc);

	// Magic
	memcpy(out + 2 * 4 + LZMA_STREAM_FLAGS_SIZE,
			lzma_footer_magic, sizeof(lzma_footer_magic));

	return LZMA_OK;
}

/*** End of inlined file: stream_flags_encoder.c ***/


/*** Start of inlined file: vli_decoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       vli_decoder.c
/// \brief      Decodes variable-length integers
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

extern LZMA_API(lzma_ret)
lzma_vli_decode(lzma_vli *restrict vli, size_t *vli_pos,
		const uint8_t *restrict in, size_t *restrict in_pos,
		size_t in_size)
{
	// If we haven't been given vli_pos, work in single-call mode.
	size_t vli_pos_internal = 0;
	if (vli_pos == NULL) {
		vli_pos = &vli_pos_internal;
		*vli = 0;

		// If there's no input, use LZMA_DATA_ERROR. This way it is
		// easy to decode VLIs from buffers that have known size,
		// and get the correct error code in case the buffer is
		// too short.
		if (*in_pos >= in_size)
			return LZMA_DATA_ERROR;

	} else {
		// Initialize *vli when starting to decode a new integer.
		if (*vli_pos == 0)
			*vli = 0;

		// Validate the arguments.
		if (*vli_pos >= LZMA_VLI_BYTES_MAX
				|| (*vli >> (*vli_pos * 7)) != 0)
			return LZMA_PROG_ERROR;;

		if (*in_pos >= in_size)
			return LZMA_BUF_ERROR;
	}

	do {
		// Read the next byte. Use a temporary variable so that we
		// can update *in_pos immediately.
		const uint8_t byte = in[*in_pos];
		++*in_pos;

		// Add the newly read byte to *vli.
		*vli += (lzma_vli)(byte & 0x7F) << (*vli_pos * 7);
		++*vli_pos;

		// Check if this is the last byte of a multibyte integer.
		if ((byte & 0x80) == 0) {
			// We don't allow using variable-length integers as
			// padding i.e. the encoding must use the most the
			// compact form.
			if (byte == 0x00 && *vli_pos > 1)
				return LZMA_DATA_ERROR;

			return vli_pos == &vli_pos_internal
					? LZMA_OK : LZMA_STREAM_END;
		}

		// There is at least one more byte coming. If we have already
		// read maximum number of bytes, the integer is considered
		// corrupt.
		//
		// If we need bigger integers in future, old versions liblzma
		// will confusingly indicate the file being corrupt instead of
		// unsupported. I suppose it's still better this way, because
		// in the foreseeable future (writing this in 2008) the only
		// reason why files would appear having over 63-bit integers
		// is that the files are simply corrupt.
		if (*vli_pos == LZMA_VLI_BYTES_MAX)
			return LZMA_DATA_ERROR;

	} while (*in_pos < in_size);

	return vli_pos == &vli_pos_internal ? LZMA_DATA_ERROR : LZMA_OK;
}

/*** End of inlined file: vli_decoder.c ***/


/*** Start of inlined file: vli_encoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       vli_encoder.c
/// \brief      Encodes variable-length integers
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

extern LZMA_API(lzma_ret)
lzma_vli_encode(lzma_vli vli, size_t *vli_pos,
		uint8_t *restrict out, size_t *restrict out_pos,
		size_t out_size)
{
	// If we haven't been given vli_pos, work in single-call mode.
	size_t vli_pos_internal = 0;
	if (vli_pos == NULL) {
		vli_pos = &vli_pos_internal;

		// In single-call mode, we expect that the caller has
		// reserved enough output space.
		if (*out_pos >= out_size)
			return LZMA_PROG_ERROR;
	} else {
		// This never happens when we are called by liblzma, but
		// may happen if called directly from an application.
		if (*out_pos >= out_size)
			return LZMA_BUF_ERROR;
	}

	// Validate the arguments.
	if (*vli_pos >= LZMA_VLI_BYTES_MAX || vli > LZMA_VLI_MAX)
		return LZMA_PROG_ERROR;

	// Shift vli so that the next bits to encode are the lowest. In
	// single-call mode this never changes vli since *vli_pos is zero.
	vli >>= *vli_pos * 7;

	// Write the non-last bytes in a loop.
	while (vli >= 0x80) {
		// We don't need *vli_pos during this function call anymore,
		// but update it here so that it is ready if we need to
		// return before the whole integer has been decoded.
		++*vli_pos;
		assert(*vli_pos < LZMA_VLI_BYTES_MAX);

		// Write the next byte.
		out[*out_pos] = (uint8_t)(vli) | 0x80;
		vli >>= 7;

		if (++*out_pos == out_size)
			return vli_pos == &vli_pos_internal
					? LZMA_PROG_ERROR : LZMA_OK;
	}

	// Write the last byte.
	out[*out_pos] = (uint8_t)(vli);
	++*out_pos;
	++*vli_pos;

	return vli_pos == &vli_pos_internal ? LZMA_OK : LZMA_STREAM_END;

}

/*** End of inlined file: vli_encoder.c ***/


/*** Start of inlined file: vli_size.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       vli_size.c
/// \brief      Calculates the encoded size of a variable-length integer
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

extern LZMA_API(uint32_t)
lzma_vli_size(lzma_vli vli)
{
	if (vli > LZMA_VLI_MAX)
		return 0;

	uint32_t i = 0;
	do {
		vli >>= 7;
		++i;
	} while (vli != 0);

	assert(i <= LZMA_VLI_BYTES_MAX);
	return i;
}

/*** End of inlined file: vli_size.c ***/

		// L Z M A
		// ----------------

		//H

/*** Start of inlined file: fastpos.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       fastpos.h
/// \brief      Kind of two-bit version of bit scan reverse
///
//  Authors:    Igor Pavlov
//              Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LZMA_FASTPOS_H
#define LZMA_FASTPOS_H

// LZMA encodes match distances by storing the highest two bits using
// a six-bit value [0, 63], and then the missing lower bits.
// Dictionary size is also stored using this encoding in the .xz
// file format header.
//
// fastpos.h provides a way to quickly find out the correct six-bit
// values. The following table gives some examples of this encoding:
//
//     dist   return
//       0       0
//       1       1
//       2       2
//       3       3
//       4       4
//       5       4
//       6       5
//       7       5
//       8       6
//      11       6
//      12       7
//     ...      ...
//      15       7
//      16       8
//      17       8
//     ...      ...
//      23       8
//      24       9
//      25       9
//     ...      ...
//
//
// Provided functions or macros
// ----------------------------
//
// get_dist_slot(dist) is the basic version. get_dist_slot_2(dist)
// assumes that dist >= FULL_DISTANCES, thus the result is at least
// FULL_DISTANCES_BITS * 2. Using get_dist_slot(dist) instead of
// get_dist_slot_2(dist) would give the same result, but get_dist_slot_2(dist)
// should be tiny bit faster due to the assumption being made.
//
//
// Size vs. speed
// --------------
//
// With some CPUs that have fast BSR (bit scan reverse) instruction, the
// size optimized version is slightly faster than the bigger table based
// approach. Such CPUs include Intel Pentium Pro, Pentium II, Pentium III
// and Core 2 (possibly others). AMD K7 seems to have slower BSR, but that
// would still have speed roughly comparable to the table version. Older
// x86 CPUs like the original Pentium have very slow BSR; on those systems
// the table version is a lot faster.
//
// On some CPUs, the table version is a lot faster when using position
// dependent code, but with position independent code the size optimized
// version is slightly faster. This occurs at least on 32-bit SPARC (no
// ASM optimizations).
//
// I'm making the table version the default, because that has good speed
// on all systems I have tried. The size optimized version is sometimes
// slightly faster, but sometimes it is a lot slower.

#ifdef HAVE_SMALL
#	define get_dist_slot(dist) \
		((dist) <= 4 ? (dist) : get_dist_slot_2(dist))

static inline uint32_t
get_dist_slot_2(uint32_t dist)
{
	const uint32_t i = bsr32(dist);
	return (i + i) + ((dist >> (i - 1)) & 1);
}

#else

#define FASTPOS_BITS 13

extern const uint8_t lzma_fastpos[1 << FASTPOS_BITS];

#define fastpos_shift(extra, n) \
	((extra) + (n) * (FASTPOS_BITS - 1))

#define fastpos_limit(extra, n) \
	(UINT32_C(1) << (FASTPOS_BITS + fastpos_shift(extra, n)))

#define fastpos_result(dist, extra, n) \
	(uint32_t)(lzma_fastpos[(dist) >> fastpos_shift(extra, n)]) \
			+ 2 * fastpos_shift(extra, n)

static inline uint32_t
get_dist_slot(uint32_t dist)
{
	// If it is small enough, we can pick the result directly from
	// the precalculated table.
	if (dist < fastpos_limit(0, 0))
		return lzma_fastpos[dist];

	if (dist < fastpos_limit(0, 1))
		return fastpos_result(dist, 0, 1);

	return fastpos_result(dist, 0, 2);
}

#ifdef FULL_DISTANCES_BITS
static inline uint32_t
get_dist_slot_2(uint32_t dist)
{
	assert(dist >= FULL_DISTANCES);

	if (dist < fastpos_limit(FULL_DISTANCES_BITS - 1, 0))
		return fastpos_result(dist, FULL_DISTANCES_BITS - 1, 0);

	if (dist < fastpos_limit(FULL_DISTANCES_BITS - 1, 1))
		return fastpos_result(dist, FULL_DISTANCES_BITS - 1, 1);

	return fastpos_result(dist, FULL_DISTANCES_BITS - 1, 2);
}
#endif

#endif

#endif

/*** End of inlined file: fastpos.h ***/



/*** Start of inlined file: lzma2_decoder.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       lzma2_decoder.h
/// \brief      LZMA2 decoder
///
//  Authors:    Igor Pavlov
//              Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LZMA_LZMA2_DECODER_H
#define LZMA_LZMA2_DECODER_H

#include "common.h"

extern lzma_ret lzma_lzma2_decoder_init(lzma_next_coder *next,
		const lzma_allocator *allocator,
		const lzma_filter_info *filters);

extern uint64_t lzma_lzma2_decoder_memusage(const void *options);

extern lzma_ret lzma_lzma2_props_decode(
		void **options, const lzma_allocator *allocator,
		const uint8_t *props, size_t props_size);

#endif

/*** End of inlined file: lzma2_decoder.h ***/


/*** Start of inlined file: lzma2_encoder.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       lzma2_encoder.h
/// \brief      LZMA2 encoder
///
//  Authors:    Igor Pavlov
//              Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LZMA_LZMA2_ENCODER_H
#define LZMA_LZMA2_ENCODER_H

#include "common.h"

/// Maximum number of bytes of actual data per chunk (no headers)
#define LZMA2_CHUNK_MAX (UINT32_C(1) << 16)

/// Maximum uncompressed size of LZMA chunk (no headers)
#define LZMA2_UNCOMPRESSED_MAX (UINT32_C(1) << 21)

/// Maximum size of LZMA2 headers
#define LZMA2_HEADER_MAX 6

/// Size of a header for uncompressed chunk
#define LZMA2_HEADER_UNCOMPRESSED 3

extern lzma_ret lzma_lzma2_encoder_init(
		lzma_next_coder *next, const lzma_allocator *allocator,
		const lzma_filter_info *filters);

extern uint64_t lzma_lzma2_encoder_memusage(const void *options);

extern lzma_ret lzma_lzma2_props_encode(const void *options, uint8_t *out);

extern uint64_t lzma_lzma2_block_size(const void *options);

#endif

/*** End of inlined file: lzma2_encoder.h ***/


/*** Start of inlined file: lzma_common.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       lzma_common.h
/// \brief      Private definitions common to LZMA encoder and decoder
///
//  Authors:    Igor Pavlov
//              Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LZMA_LZMA_COMMON_H
#define LZMA_LZMA_COMMON_H

#include "common.h"
#include "range_common.h"

///////////////////
// Miscellaneous //
///////////////////

/// Maximum number of position states. A position state is the lowest pos bits
/// number of bits of the current uncompressed offset. In some places there
/// are different sets of probabilities for different pos states.
#define POS_STATES_MAX (1 << LZMA_PB_MAX)

/// Validates lc, lp, and pb.
static inline bool
is_lclppb_valid(const lzma_options_lzma *options)
{
	return options->lc <= LZMA_LCLP_MAX && options->lp <= LZMA_LCLP_MAX
			&& options->lc + options->lp <= LZMA_LCLP_MAX
			&& options->pb <= LZMA_PB_MAX;
}

///////////
// State //
///////////

/// This enum is used to track which events have occurred most recently and
/// in which order. This information is used to predict the next event.
///
/// Events:
///  - Literal: One 8-bit byte
///  - Match: Repeat a chunk of data at some distance
///  - Long repeat: Multi-byte match at a recently seen distance
///  - Short repeat: One-byte repeat at a recently seen distance
///
/// The event names are in from STATE_oldest_older_previous. REP means
/// either short or long repeated match, and NONLIT means any non-literal.
typedef enum {
	STATE_LIT_LIT,
	STATE_MATCH_LIT_LIT,
	STATE_REP_LIT_LIT,
	STATE_SHORTREP_LIT_LIT,
	STATE_MATCH_LIT,
	STATE_REP_LIT,
	STATE_SHORTREP_LIT,
	STATE_LIT_MATCH,
	STATE_LIT_LONGREP,
	STATE_LIT_SHORTREP,
	STATE_NONLIT_MATCH,
	STATE_NONLIT_REP,
} lzma_lzma_state;

/// Total number of states
#define STATES 12

/// The lowest 7 states indicate that the previous state was a literal.
#define LIT_STATES 7

/// Indicate that the latest state was a literal.
#define update_literal(state) \
	state = ((state) <= STATE_SHORTREP_LIT_LIT \
			? STATE_LIT_LIT \
			: ((state) <= STATE_LIT_SHORTREP \
				? (state) - 3 \
				: (state) - 6))

/// Indicate that the latest state was a match.
#define update_match(state) \
	state = ((state) < LIT_STATES ? STATE_LIT_MATCH : STATE_NONLIT_MATCH)

/// Indicate that the latest state was a long repeated match.
#define update_long_rep(state) \
	state = ((state) < LIT_STATES ? STATE_LIT_LONGREP : STATE_NONLIT_REP)

/// Indicate that the latest state was a short match.
#define update_short_rep(state) \
	state = ((state) < LIT_STATES ? STATE_LIT_SHORTREP : STATE_NONLIT_REP)

/// Test if the previous state was a literal.
#define is_literal_state(state) \
	((state) < LIT_STATES)

/////////////
// Literal //
/////////////

/// Each literal coder is divided in three sections:
///   - 0x001-0x0FF: Without match byte
///   - 0x101-0x1FF: With match byte; match bit is 0
///   - 0x201-0x2FF: With match byte; match bit is 1
///
/// Match byte is used when the previous LZMA symbol was something else than
/// a literal (that is, it was some kind of match).
#define LITERAL_CODER_SIZE 0x300

/// Maximum number of literal coders
#define LITERAL_CODERS_MAX (1 << LZMA_LCLP_MAX)

/// Locate the literal coder for the next literal byte. The choice depends on
///   - the lowest literal_pos_bits bits of the position of the current
///     byte; and
///   - the highest literal_context_bits bits of the previous byte.
#define literal_subcoder(probs, lc, lp_mask, pos, prev_byte) \
	((probs)[(((pos) & (lp_mask)) << (lc)) \
			+ ((uint32_t)(prev_byte) >> (8U - (lc)))])

static inline void
literal_init(probability (*probs)[LITERAL_CODER_SIZE],
		uint32_t lc, uint32_t lp)
{
	assert(lc + lp <= LZMA_LCLP_MAX);

	const uint32_t coders = 1U << (lc + lp);

	for (uint32_t i = 0; i < coders; ++i)
		for (uint32_t j = 0; j < LITERAL_CODER_SIZE; ++j)
			bit_reset(probs[i][j]);

	return;
}

//////////////////
// Match length //
//////////////////

// Minimum length of a match is two bytes.
#define MATCH_LEN_MIN 2

// Match length is encoded with 4, 5, or 10 bits.
//
// Length   Bits
//  2-9      4 = Choice=0 + 3 bits
// 10-17     5 = Choice=1 + Choice2=0 + 3 bits
// 18-273   10 = Choice=1 + Choice2=1 + 8 bits
#define LEN_LOW_BITS 3
#define LEN_LOW_SYMBOLS (1 << LEN_LOW_BITS)
#define LEN_MID_BITS 3
#define LEN_MID_SYMBOLS (1 << LEN_MID_BITS)
#define LEN_HIGH_BITS 8
#define LEN_HIGH_SYMBOLS (1 << LEN_HIGH_BITS)
#define LEN_SYMBOLS (LEN_LOW_SYMBOLS + LEN_MID_SYMBOLS + LEN_HIGH_SYMBOLS)

// Maximum length of a match is 273 which is a result of the encoding
// described above.
#define MATCH_LEN_MAX (MATCH_LEN_MIN + LEN_SYMBOLS - 1)

////////////////////
// Match distance //
////////////////////

// Different sets of probabilities are used for match distances that have very
// short match length: Lengths of 2, 3, and 4 bytes have a separate set of
// probabilities for each length. The matches with longer length use a shared
// set of probabilities.
#define DIST_STATES 4

// Macro to get the index of the appropriate probability array.
#define get_dist_state(len) \
	((len) < DIST_STATES + MATCH_LEN_MIN \
		? (len) - MATCH_LEN_MIN \
		: DIST_STATES - 1)

// The highest two bits of a match distance (distance slot) are encoded
// using six bits. See fastpos.h for more explanation.
#define DIST_SLOT_BITS 6
#define DIST_SLOTS (1 << DIST_SLOT_BITS)

// Match distances up to 127 are fully encoded using probabilities. Since
// the highest two bits (distance slot) are always encoded using six bits,
// the distances 0-3 don't need any additional bits to encode, since the
// distance slot itself is the same as the actual distance. DIST_MODEL_START
// indicates the first distance slot where at least one additional bit is
// needed.
#define DIST_MODEL_START 4

// Match distances greater than 127 are encoded in three pieces:
//   - distance slot: the highest two bits
//   - direct bits: 2-26 bits below the highest two bits
//   - alignment bits: four lowest bits
//
// Direct bits don't use any probabilities.
//
// The distance slot value of 14 is for distances 128-191 (see the table in
// fastpos.h to understand why).
#define DIST_MODEL_END 14

// Distance slots that indicate a distance <= 127.
#define FULL_DISTANCES_BITS (DIST_MODEL_END / 2)
#define FULL_DISTANCES (1 << FULL_DISTANCES_BITS)

// For match distances greater than 127, only the highest two bits and the
// lowest four bits (alignment) is encoded using probabilities.
#define ALIGN_BITS 4
#define ALIGN_SIZE (1 << ALIGN_BITS)
#define ALIGN_MASK (ALIGN_SIZE - 1)

// LZMA remembers the four most recent match distances. Reusing these distances
// tends to take less space than re-encoding the actual distance value.
#define REPS 4

#endif

/*** End of inlined file: lzma_common.h ***/


/*** Start of inlined file: lzma_decoder.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       lzma_decoder.h
/// \brief      LZMA decoder API
///
//  Authors:    Igor Pavlov
//              Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LZMA_LZMA_DECODER_H
#define LZMA_LZMA_DECODER_H

#include "common.h"

/// Allocates and initializes LZMA decoder
extern lzma_ret lzma_lzma_decoder_init(lzma_next_coder *next,
		const lzma_allocator *allocator,
		const lzma_filter_info *filters);

extern uint64_t lzma_lzma_decoder_memusage(const void *options);

extern lzma_ret lzma_lzma_props_decode(
		void **options, const lzma_allocator *allocator,
		const uint8_t *props, size_t props_size);

/// \brief      Decodes the LZMA Properties byte (lc/lp/pb)
///
/// \return     true if error occurred, false on success
///
extern bool lzma_lzma_lclppb_decode(
		lzma_options_lzma *options, uint8_t byte);

#ifdef LZMA_LZ_DECODER_H
/// Allocate and setup function pointers only. This is used by LZMA1 and
/// LZMA2 decoders.
extern lzma_ret lzma_lzma_decoder_create(
		lzma_lz_decoder *lz, const lzma_allocator *allocator,
		const void *opt, lzma_lz_options *lz_options);

/// Gets memory usage without validating lc/lp/pb. This is used by LZMA2
/// decoder, because raw LZMA2 decoding doesn't need lc/lp/pb.
extern uint64_t lzma_lzma_decoder_memusage_nocheck(const void *options);

#endif

#endif

/*** End of inlined file: lzma_decoder.h ***/


/*** Start of inlined file: lzma_encoder_private.h ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       lzma_encoder_private.h
/// \brief      Private definitions for LZMA encoder
///
//  Authors:    Igor Pavlov
//              Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LZMA_LZMA_ENCODER_PRIVATE_H
#define LZMA_LZMA_ENCODER_PRIVATE_H

#include "lz_encoder.h"
#include "range_encoder.h"

// Macro to compare if the first two bytes in two buffers differ. This is
// needed in lzma_lzma_optimum_*() to test if the match is at least
// MATCH_LEN_MIN bytes. Unaligned access gives tiny gain so there's no
// reason to not use it when it is supported.
#ifdef TUKLIB_FAST_UNALIGNED_ACCESS
#	define not_equal_16(a, b) (read16ne(a) != read16ne(b))
#else
#	define not_equal_16(a, b) \
		((a)[0] != (b)[0] || (a)[1] != (b)[1])
#endif

// Optimal - Number of entries in the optimum array.
#define OPTS (1 << 12)

typedef struct {
	probability choice;
	probability choice2;
	probability low[POS_STATES_MAX][LEN_LOW_SYMBOLS];
	probability mid[POS_STATES_MAX][LEN_MID_SYMBOLS];
	probability high[LEN_HIGH_SYMBOLS];

	uint32_t prices[POS_STATES_MAX][LEN_SYMBOLS];
	uint32_t table_size;
	uint32_t counters[POS_STATES_MAX];

} lzma_length_encoder;

typedef struct {
	lzma_lzma_state state;

	bool prev_1_is_literal;
	bool prev_2;

	uint32_t pos_prev_2;
	uint32_t back_prev_2;

	uint32_t price;
	uint32_t pos_prev;  // pos_next;
	uint32_t back_prev;

	uint32_t backs[REPS];

} lzma_optimal;

struct lzma_lzma1_encoder_s {
	/// Range encoder
	lzma_range_encoder rc;

	/// State
	lzma_lzma_state state;

	/// The four most recent match distances
	uint32_t reps[REPS];

	/// Array of match candidates
	lzma_match matches[MATCH_LEN_MAX + 1];

	/// Number of match candidates in matches[]
	uint32_t matches_count;

	/// Variable to hold the length of the longest match between calls
	/// to lzma_lzma_optimum_*().
	uint32_t longest_match_length;

	/// True if using getoptimumfast
	bool fast_mode;

	/// True if the encoder has been initialized by encoding the first
	/// byte as a literal.
	bool is_initialized;

	/// True if the range encoder has been flushed, but not all bytes
	/// have been written to the output buffer yet.
	bool is_flushed;

	uint32_t pos_mask;         ///< (1 << pos_bits) - 1
	uint32_t literal_context_bits;
	uint32_t literal_pos_mask;

	// These are the same as in lzma_decoder.c. See comments there.
	probability literal[LITERAL_CODERS_MAX][LITERAL_CODER_SIZE];
	probability is_match[STATES][POS_STATES_MAX];
	probability is_rep[STATES];
	probability is_rep0[STATES];
	probability is_rep1[STATES];
	probability is_rep2[STATES];
	probability is_rep0_long[STATES][POS_STATES_MAX];
	probability dist_slot[DIST_STATES][DIST_SLOTS];
	probability dist_special[FULL_DISTANCES - DIST_MODEL_END];
	probability dist_align[ALIGN_SIZE];

	// These are the same as in lzma_decoder.c except that the encoders
	// include also price tables.
	lzma_length_encoder match_len_encoder;
	lzma_length_encoder rep_len_encoder;

	// Price tables
	uint32_t dist_slot_prices[DIST_STATES][DIST_SLOTS];
	uint32_t dist_prices[DIST_STATES][FULL_DISTANCES];
	uint32_t dist_table_size;
	uint32_t match_price_count;

	uint32_t align_prices[ALIGN_SIZE];
	uint32_t align_price_count;

	// Optimal
	uint32_t opts_end_index;
	uint32_t opts_current_index;
	lzma_optimal opts[OPTS];
};

extern void lzma_lzma_optimum_fast(
		lzma_lzma1_encoder *restrict coder, lzma_mf *restrict mf,
		uint32_t *restrict back_res, uint32_t *restrict len_res);

extern void lzma_lzma_optimum_normal(lzma_lzma1_encoder *restrict coder,
		lzma_mf *restrict mf, uint32_t *restrict back_res,
		uint32_t *restrict len_res, uint32_t position);

#endif

/*** End of inlined file: lzma_encoder_private.h ***/

		//C

/*** Start of inlined file: fastpos_table.c ***/
#include "common.h"

const uint8_t lzma_fastpos[1 << FASTPOS_BITS] = {
	  0,  1,  2,  3,  4,  4,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,
	  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,
	 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
	 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
	 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
	 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
	 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
	 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
	 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
	 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
	 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
	 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
	 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
	 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
	 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
	 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
	 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
	 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
	 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
	 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
	 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
	 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
	 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
	 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
	 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
	 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
	 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
	 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
	 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
	 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
	 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
	 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
	 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
	 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
	 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
	 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
	 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
	 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
	 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
	 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
	 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
	 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
	 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
	 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
	 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
	 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
	 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
	 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25
};

/*** End of inlined file: fastpos_table.c ***/



/*** Start of inlined file: fastpos_tablegen.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       fastpos_tablegen.c
/// \brief      Generates the lzma_fastpos[] lookup table
///
//  Authors:    Igor Pavlov
//              Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include <inttypes.h>
#include <stdio.h>

int
main(void)
{
	uint8_t fastpos[1 << FASTPOS_BITS];

	const uint8_t fast_slots = 2 * FASTPOS_BITS;
	uint32_t c = 2;

	fastpos[0] = 0;
	fastpos[1] = 1;

	for (uint8_t slot_fast = 2; slot_fast < fast_slots; ++slot_fast) {
		const uint32_t k = 1 << ((slot_fast >> 1) - 1);
		for (uint32_t j = 0; j < k; ++j, ++c)
			fastpos[c] = slot_fast;
	}

	printf("/* This file has been automatically generated "
			"by fastpos_tablegen.c. */\n\n"
			"#include \"common.h\"\n"
			"#include \"fastpos.h\"\n\n"
			"const uint8_t lzma_fastpos[1 << FASTPOS_BITS] = {");

	for (size_t i = 0; i < (1 << FASTPOS_BITS); ++i) {
		if (i % 16 == 0)
			printf("\n\t");

		printf("%3u", (unsigned int)(fastpos[i]));

		if (i != (1 << FASTPOS_BITS) - 1)
			printf(",");
	}

	printf("\n};\n");

	return 0;
}

/*** End of inlined file: fastpos_tablegen.c ***/


/*** Start of inlined file: lzma2_decoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       lzma2_decoder.c
/// \brief      LZMA2 decoder
///
//  Authors:    Igor Pavlov
//              Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "lz_decoder.h"

typedef struct {
	enum sequence {
		SEQ_CONTROL,
		SEQ_UNCOMPRESSED_1,
		SEQ_UNCOMPRESSED_2,
		SEQ_COMPRESSED_0,
		SEQ_COMPRESSED_1,
		SEQ_PROPERTIES,
		SEQ_LZMA,
		SEQ_COPY,
	} sequence;

	/// Sequence after the size fields have been decoded.
	enum sequence next_sequence;

	/// LZMA decoder
	lzma_lz_decoder lzma;

	/// Uncompressed size of LZMA chunk
	size_t uncompressed_size;

	/// Compressed size of the chunk (naturally equals to uncompressed
	/// size of uncompressed chunk)
	size_t compressed_size;

	/// True if properties are needed. This is false before the
	/// first LZMA chunk.
	bool need_properties;

	/// True if dictionary reset is needed. This is false before the
	/// first chunk (LZMA or uncompressed).
	bool need_dictionary_reset;

	lzma_options_lzma options;
} lzma_lzma2_coder;

static lzma_ret
lzma2_decode(void *coder_ptr, lzma_dict *restrict dict,
		const uint8_t *restrict in, size_t *restrict in_pos,
		size_t in_size)
{
	lzma_lzma2_coder *restrict coder = coder_ptr;

	// With SEQ_LZMA it is possible that no new input is needed to do
	// some progress. The rest of the sequences assume that there is
	// at least one byte of input.
	while (*in_pos < in_size || coder->sequence == SEQ_LZMA)
	switch (coder->sequence) {
	case SEQ_CONTROL: {
		const uint32_t control = in[*in_pos];
		++*in_pos;

		// End marker
		if (control == 0x00)
			return LZMA_STREAM_END;

		if (control >= 0xE0 || control == 1) {
			// Dictionary reset implies that next LZMA chunk has
			// to set new properties.
			coder->need_properties = true;
			coder->need_dictionary_reset = true;
		} else if (coder->need_dictionary_reset) {
			return LZMA_DATA_ERROR;
		}

		if (control >= 0x80) {
			// LZMA chunk. The highest five bits of the
			// uncompressed size are taken from the control byte.
			coder->uncompressed_size = (control & 0x1F) << 16;
			coder->sequence = SEQ_UNCOMPRESSED_1;

			// See if there are new properties or if we need to
			// reset the state.
			if (control >= 0xC0) {
				// When there are new properties, state reset
				// is done at SEQ_PROPERTIES.
				coder->need_properties = false;
				coder->next_sequence = SEQ_PROPERTIES;

			} else if (coder->need_properties) {
				return LZMA_DATA_ERROR;

			} else {
				coder->next_sequence = SEQ_LZMA;

				// If only state reset is wanted with old
				// properties, do the resetting here for
				// simplicity.
				if (control >= 0xA0)
					coder->lzma.reset(coder->lzma.coder,
							&coder->options);
			}
		} else {
			// Invalid control values
			if (control > 2)
				return LZMA_DATA_ERROR;

			// It's uncompressed chunk
			coder->sequence = SEQ_COMPRESSED_0;
			coder->next_sequence = SEQ_COPY;
		}

		if (coder->need_dictionary_reset) {
			// Finish the dictionary reset and let the caller
			// flush the dictionary to the actual output buffer.
			coder->need_dictionary_reset = false;
			dict_reset(dict);
			return LZMA_OK;
		}

		break;
	}

	case SEQ_UNCOMPRESSED_1:
		coder->uncompressed_size += (uint32_t)(in[(*in_pos)++]) << 8;
		coder->sequence = SEQ_UNCOMPRESSED_2;
		break;

	case SEQ_UNCOMPRESSED_2:
		coder->uncompressed_size += in[(*in_pos)++] + 1U;
		coder->sequence = SEQ_COMPRESSED_0;
		coder->lzma.set_uncompressed(coder->lzma.coder,
				coder->uncompressed_size);
		break;

	case SEQ_COMPRESSED_0:
		coder->compressed_size = (uint32_t)(in[(*in_pos)++]) << 8;
		coder->sequence = SEQ_COMPRESSED_1;
		break;

	case SEQ_COMPRESSED_1:
		coder->compressed_size += in[(*in_pos)++] + 1U;
		coder->sequence = coder->next_sequence;
		break;

	case SEQ_PROPERTIES:
		if (lzma_lzma_lclppb_decode(&coder->options, in[(*in_pos)++]))
			return LZMA_DATA_ERROR;

		coder->lzma.reset(coder->lzma.coder, &coder->options);

		coder->sequence = SEQ_LZMA;
		break;

	case SEQ_LZMA: {
		// Store the start offset so that we can update
		// coder->compressed_size later.
		const size_t in_start = *in_pos;

		// Decode from in[] to *dict.
		const lzma_ret ret = coder->lzma.code(coder->lzma.coder,
				dict, in, in_pos, in_size);

		// Validate and update coder->compressed_size.
		const size_t in_used = *in_pos - in_start;
		if (in_used > coder->compressed_size)
			return LZMA_DATA_ERROR;

		coder->compressed_size -= in_used;

		// Return if we didn't finish the chunk, or an error occurred.
		if (ret != LZMA_STREAM_END)
			return ret;

		// The LZMA decoder must have consumed the whole chunk now.
		// We don't need to worry about uncompressed size since it
		// is checked by the LZMA decoder.
		if (coder->compressed_size != 0)
			return LZMA_DATA_ERROR;

		coder->sequence = SEQ_CONTROL;
		break;
	}

	case SEQ_COPY: {
		// Copy from input to the dictionary as is.
		dict_write(dict, in, in_pos, in_size, &coder->compressed_size);
		if (coder->compressed_size != 0)
			return LZMA_OK;

		coder->sequence = SEQ_CONTROL;
		break;
	}

	default:
		assert(0);
		return LZMA_PROG_ERROR;
	}

	return LZMA_OK;
}

static void
lzma2_decoder_end(void *coder_ptr, const lzma_allocator *allocator)
{
	lzma_lzma2_coder *coder = coder_ptr;

	assert(coder->lzma.end == NULL);
	lzma_free(coder->lzma.coder, allocator);

	lzma_free(coder, allocator);

	return;
}

static lzma_ret
lzma2_decoder_init(lzma_lz_decoder *lz, const lzma_allocator *allocator,
		const void *opt, lzma_lz_options *lz_options)
{
	lzma_lzma2_coder *coder = lz->coder;
	if (coder == NULL) {
		coder = lzma_alloc(sizeof(lzma_lzma2_coder), allocator);
		if (coder == NULL)
			return LZMA_MEM_ERROR;

		lz->coder = coder;
		lz->code = &lzma2_decode;
		lz->end = &lzma2_decoder_end;

		coder->lzma = LZMA_LZ_DECODER_INIT;
	}

	const lzma_options_lzma *options = opt;

	coder->sequence = SEQ_CONTROL;
	coder->need_properties = true;
	coder->need_dictionary_reset = options->preset_dict == NULL
			|| options->preset_dict_size == 0;

	return lzma_lzma_decoder_create(&coder->lzma,
			allocator, options, lz_options);
}

extern lzma_ret
lzma_lzma2_decoder_init(lzma_next_coder *next, const lzma_allocator *allocator,
		const lzma_filter_info *filters)
{
	// LZMA2 can only be the last filter in the chain. This is enforced
	// by the raw_decoder initialization.
	assert(filters[1].init == NULL);

	return lzma_lz_decoder_init(next, allocator, filters,
			&lzma2_decoder_init);
}

extern uint64_t
lzma_lzma2_decoder_memusage(const void *options)
{
	return sizeof(lzma_lzma2_coder)
			+ lzma_lzma_decoder_memusage_nocheck(options);
}

extern lzma_ret
lzma_lzma2_props_decode(void **options, const lzma_allocator *allocator,
		const uint8_t *props, size_t props_size)
{
	if (props_size != 1)
		return LZMA_OPTIONS_ERROR;

	// Check that reserved bits are unset.
	if (props[0] & 0xC0)
		return LZMA_OPTIONS_ERROR;

	// Decode the dictionary size.
	if (props[0] > 40)
		return LZMA_OPTIONS_ERROR;

	lzma_options_lzma *opt = lzma_alloc(
			sizeof(lzma_options_lzma), allocator);
	if (opt == NULL)
		return LZMA_MEM_ERROR;

	if (props[0] == 40) {
		opt->dict_size = UINT32_MAX;
	} else {
		opt->dict_size = 2 | (props[0] & 1U);
		opt->dict_size <<= props[0] / 2U + 11;
	}

	opt->preset_dict = NULL;
	opt->preset_dict_size = 0;

	*options = opt;

	return LZMA_OK;
}

/*** End of inlined file: lzma2_decoder.c ***/


/*** Start of inlined file: lzma2_encoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       lzma2_encoder.c
/// \brief      LZMA2 encoder
///
//  Authors:    Igor Pavlov
//              Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "lz_encoder.h"

typedef struct {
	enum {
		SEQ_INIT,
		SEQ_LZMA_ENCODE,
		SEQ_LZMA_COPY,
		SEQ_UNCOMPRESSED_HEADER,
		SEQ_UNCOMPRESSED_COPY,
	} sequence;

	/// LZMA encoder
	void *lzma;

	/// LZMA options currently in use.
	lzma_options_lzma opt_cur;

	bool need_properties;
	bool need_state_reset;
	bool need_dictionary_reset;

	/// Uncompressed size of a chunk
	size_t uncompressed_size;

	/// Compressed size of a chunk (excluding headers); this is also used
	/// to indicate the end of buf[] in SEQ_LZMA_COPY.
	size_t compressed_size;

	/// Read position in buf[]
	size_t buf_pos;

	/// Buffer to hold the chunk header and LZMA compressed data
	uint8_t buf[LZMA2_HEADER_MAX + LZMA2_CHUNK_MAX];
} lzma_lzma2_coder;

static void
lzma2_header_lzma(lzma_lzma2_coder *coder)
{
	assert(coder->uncompressed_size > 0);
	assert(coder->uncompressed_size <= LZMA2_UNCOMPRESSED_MAX);
	assert(coder->compressed_size > 0);
	assert(coder->compressed_size <= LZMA2_CHUNK_MAX);

	size_t pos;

	if (coder->need_properties) {
		pos = 0;

		if (coder->need_dictionary_reset)
			coder->buf[pos] = 0x80 + (3 << 5);
		else
			coder->buf[pos] = 0x80 + (2 << 5);
	} else {
		pos = 1;

		if (coder->need_state_reset)
			coder->buf[pos] = 0x80 + (1 << 5);
		else
			coder->buf[pos] = 0x80;
	}

	// Set the start position for copying.
	coder->buf_pos = pos;

	// Uncompressed size
	size_t size = coder->uncompressed_size - 1;
	coder->buf[pos++] += size >> 16;
	coder->buf[pos++] = (size >> 8) & 0xFF;
	coder->buf[pos++] = size & 0xFF;

	// Compressed size
	size = coder->compressed_size - 1;
	coder->buf[pos++] = size >> 8;
	coder->buf[pos++] = size & 0xFF;

	// Properties, if needed
	if (coder->need_properties)
		lzma_lzma_lclppb_encode(&coder->opt_cur, coder->buf + pos);

	coder->need_properties = false;
	coder->need_state_reset = false;
	coder->need_dictionary_reset = false;

	// The copying code uses coder->compressed_size to indicate the end
	// of coder->buf[], so we need add the maximum size of the header here.
	coder->compressed_size += LZMA2_HEADER_MAX;

	return;
}

static void
lzma2_header_uncompressed(lzma_lzma2_coder *coder)
{
	assert(coder->uncompressed_size > 0);
	assert(coder->uncompressed_size <= LZMA2_CHUNK_MAX);

	// If this is the first chunk, we need to include dictionary
	// reset indicator.
	if (coder->need_dictionary_reset)
		coder->buf[0] = 1;
	else
		coder->buf[0] = 2;

	coder->need_dictionary_reset = false;

	// "Compressed" size
	coder->buf[1] = (coder->uncompressed_size - 1) >> 8;
	coder->buf[2] = (coder->uncompressed_size - 1) & 0xFF;

	// Set the start position for copying.
	coder->buf_pos = 0;
	return;
}

static lzma_ret
lzma2_encode(void *coder_ptr, lzma_mf *restrict mf,
		uint8_t *restrict out, size_t *restrict out_pos,
		size_t out_size)
{
	lzma_lzma2_coder *restrict coder = coder_ptr;

	while (*out_pos < out_size)
	switch (coder->sequence) {
	case SEQ_INIT:
		// If there's no input left and we are flushing or finishing,
		// don't start a new chunk.
		if (mf_unencoded(mf) == 0) {
			// Write end of payload marker if finishing.
			if (mf->action == LZMA_FINISH)
				out[(*out_pos)++] = 0;

			return mf->action == LZMA_RUN
					? LZMA_OK : LZMA_STREAM_END;
		}

		if (coder->need_state_reset)
			return_if_error(lzma_lzma_encoder_reset(
					coder->lzma, &coder->opt_cur));

		coder->uncompressed_size = 0;
		coder->compressed_size = 0;
		coder->sequence = SEQ_LZMA_ENCODE;

	// Fall through

	case SEQ_LZMA_ENCODE: {
		// Calculate how much more uncompressed data this chunk
		// could accept.
		const uint32_t left = LZMA2_UNCOMPRESSED_MAX
				- coder->uncompressed_size;
		uint32_t limit;

		if (left < mf->match_len_max) {
			// Must flush immediately since the next LZMA symbol
			// could make the uncompressed size of the chunk too
			// big.
			limit = 0;
		} else {
			// Calculate maximum read_limit that is OK from point
			// of view of LZMA2 chunk size.
			limit = mf->read_pos - mf->read_ahead
					+ left - mf->match_len_max;
		}

		// Save the start position so that we can update
		// coder->uncompressed_size.
		const uint32_t read_start = mf->read_pos - mf->read_ahead;

		// Call the LZMA encoder until the chunk is finished.
		const lzma_ret ret = lzma_lzma_encode(coder->lzma, mf,
				coder->buf + LZMA2_HEADER_MAX,
				&coder->compressed_size,
				LZMA2_CHUNK_MAX, limit);

		coder->uncompressed_size += mf->read_pos - mf->read_ahead
				- read_start;

		assert(coder->compressed_size <= LZMA2_CHUNK_MAX);
		assert(coder->uncompressed_size <= LZMA2_UNCOMPRESSED_MAX);

		if (ret != LZMA_STREAM_END)
			return LZMA_OK;

		// See if the chunk compressed. If it didn't, we encode it
		// as uncompressed chunk. This saves a few bytes of space
		// and makes decoding faster.
		if (coder->compressed_size >= coder->uncompressed_size) {
			coder->uncompressed_size += mf->read_ahead;
			assert(coder->uncompressed_size
					<= LZMA2_UNCOMPRESSED_MAX);
			mf->read_ahead = 0;
			lzma2_header_uncompressed(coder);
			coder->need_state_reset = true;
			coder->sequence = SEQ_UNCOMPRESSED_HEADER;
			break;
		}

		// The chunk did compress at least by one byte, so we store
		// the chunk as LZMA.
		lzma2_header_lzma(coder);

		coder->sequence = SEQ_LZMA_COPY;
	}

	// Fall through

	case SEQ_LZMA_COPY:
		// Copy the compressed chunk along its headers to the
		// output buffer.
		lzma_bufcpy(coder->buf, &coder->buf_pos,
				coder->compressed_size,
				out, out_pos, out_size);
		if (coder->buf_pos != coder->compressed_size)
			return LZMA_OK;

		coder->sequence = SEQ_INIT;
		break;

	case SEQ_UNCOMPRESSED_HEADER:
		// Copy the three-byte header to indicate uncompressed chunk.
		lzma_bufcpy(coder->buf, &coder->buf_pos,
				LZMA2_HEADER_UNCOMPRESSED,
				out, out_pos, out_size);
		if (coder->buf_pos != LZMA2_HEADER_UNCOMPRESSED)
			return LZMA_OK;

		coder->sequence = SEQ_UNCOMPRESSED_COPY;

	// Fall through

	case SEQ_UNCOMPRESSED_COPY:
		// Copy the uncompressed data as is from the dictionary
		// to the output buffer.
		mf_read(mf, out, out_pos, out_size, &coder->uncompressed_size);
		if (coder->uncompressed_size != 0)
			return LZMA_OK;

		coder->sequence = SEQ_INIT;
		break;
	}

	return LZMA_OK;
}

static void
lzma2_encoder_end(void *coder_ptr, const lzma_allocator *allocator)
{
	lzma_lzma2_coder *coder = coder_ptr;
	lzma_free(coder->lzma, allocator);
	lzma_free(coder, allocator);
	return;
}

static lzma_ret
lzma2_encoder_options_update(void *coder_ptr, const lzma_filter *filter)
{
	lzma_lzma2_coder *coder = coder_ptr;

	// New options can be set only when there is no incomplete chunk.
	// This is the case at the beginning of the raw stream and right
	// after LZMA_SYNC_FLUSH.
	if (filter->options == NULL || coder->sequence != SEQ_INIT)
		return LZMA_PROG_ERROR;

	// Look if there are new options. At least for now,
	// only lc/lp/pb can be changed.
	const lzma_options_lzma *opt = filter->options;
	if (coder->opt_cur.lc != opt->lc || coder->opt_cur.lp != opt->lp
			|| coder->opt_cur.pb != opt->pb) {
		// Validate the options.
		if (opt->lc > LZMA_LCLP_MAX || opt->lp > LZMA_LCLP_MAX
				|| opt->lc + opt->lp > LZMA_LCLP_MAX
				|| opt->pb > LZMA_PB_MAX)
			return LZMA_OPTIONS_ERROR;

		// The new options will be used when the encoder starts
		// a new LZMA2 chunk.
		coder->opt_cur.lc = opt->lc;
		coder->opt_cur.lp = opt->lp;
		coder->opt_cur.pb = opt->pb;
		coder->need_properties = true;
		coder->need_state_reset = true;
	}

	return LZMA_OK;
}

static lzma_ret
lzma2_encoder_init(lzma_lz_encoder *lz, const lzma_allocator *allocator,
		const void *options, lzma_lz_options *lz_options)
{
	if (options == NULL)
		return LZMA_PROG_ERROR;

	lzma_lzma2_coder *coder = lz->coder;
	if (coder == NULL) {
		coder = lzma_alloc(sizeof(lzma_lzma2_coder), allocator);
		if (coder == NULL)
			return LZMA_MEM_ERROR;

		lz->coder = coder;
		lz->code = &lzma2_encode;
		lz->end = &lzma2_encoder_end;
		lz->options_update = &lzma2_encoder_options_update;

		coder->lzma = NULL;
	}

	coder->opt_cur = *(const lzma_options_lzma *)(options);

	coder->sequence = SEQ_INIT;
	coder->need_properties = true;
	coder->need_state_reset = false;
	coder->need_dictionary_reset
			= coder->opt_cur.preset_dict == NULL
			|| coder->opt_cur.preset_dict_size == 0;

	// Initialize LZMA encoder
	return_if_error(lzma_lzma_encoder_create(&coder->lzma, allocator,
			&coder->opt_cur, lz_options));

	// Make sure that we will always have enough history available in
	// case we need to use uncompressed chunks. They are used when the
	// compressed size of a chunk is not smaller than the uncompressed
	// size, so we need to have at least LZMA2_COMPRESSED_MAX bytes
	// history available.
	if (lz_options->before_size + lz_options->dict_size < LZMA2_CHUNK_MAX)
		lz_options->before_size
				= LZMA2_CHUNK_MAX - lz_options->dict_size;

	return LZMA_OK;
}

extern lzma_ret
lzma_lzma2_encoder_init(lzma_next_coder *next, const lzma_allocator *allocator,
		const lzma_filter_info *filters)
{
	return lzma_lz_encoder_init(
			next, allocator, filters, &lzma2_encoder_init);
}

extern uint64_t
lzma_lzma2_encoder_memusage(const void *options)
{
	const uint64_t lzma_mem = lzma_lzma_encoder_memusage(options);
	if (lzma_mem == UINT64_MAX)
		return UINT64_MAX;

	return sizeof(lzma_lzma2_coder) + lzma_mem;
}

extern lzma_ret
lzma_lzma2_props_encode(const void *options, uint8_t *out)
{
	const lzma_options_lzma *const opt = options;
	uint32_t d = my_max(opt->dict_size, LZMA_DICT_SIZE_MIN);

	// Round up to the next 2^n - 1 or 2^n + 2^(n - 1) - 1 depending
	// on which one is the next:
	--d;
	d |= d >> 2;
	d |= d >> 3;
	d |= d >> 4;
	d |= d >> 8;
	d |= d >> 16;

	// Get the highest two bits using the proper encoding:
	if (d == UINT32_MAX)
		out[0] = 40;
	else
		out[0] = get_dist_slot(d + 1) - 24;

	return LZMA_OK;
}

extern uint64_t
lzma_lzma2_block_size(const void *options)
{
	const lzma_options_lzma *const opt = options;

	// Use at least 1 MiB to keep compression ratio better.
	return my_max((uint64_t)(opt->dict_size) * 3, UINT64_C(1) << 20);
}

/*** End of inlined file: lzma2_encoder.c ***/


/*** Start of inlined file: lzma_decoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       lzma_decoder.c
/// \brief      LZMA decoder
///
//  Authors:    Igor Pavlov
//              Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "lz_decoder.h"

#include "range_decoder.h"

// The macros unroll loops with switch statements.
// Silence warnings about missing fall-through comments.
#if TUKLIB_GNUC_REQ(7, 0)
#	pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif

#ifdef HAVE_SMALL

// Macros for (somewhat) size-optimized code.
#define seq_4(seq) seq

#define seq_6(seq) seq

#define seq_8(seq) seq

#define seq_len(seq) \
	seq ## _CHOICE, \
	seq ## _CHOICE2, \
	seq ## _BITTREE

#define len_decode(target, ld, pos_state, seq) \
do { \
case seq ## _CHOICE: \
	rc_if_0(ld.choice, seq ## _CHOICE) { \
		rc_update_0(ld.choice); \
		probs = ld.low[pos_state];\
		limit = LEN_LOW_SYMBOLS; \
		target = MATCH_LEN_MIN; \
	} else { \
		rc_update_1(ld.choice); \
case seq ## _CHOICE2: \
		rc_if_0(ld.choice2, seq ## _CHOICE2) { \
			rc_update_0(ld.choice2); \
			probs = ld.mid[pos_state]; \
			limit = LEN_MID_SYMBOLS; \
			target = MATCH_LEN_MIN + LEN_LOW_SYMBOLS; \
		} else { \
			rc_update_1(ld.choice2); \
			probs = ld.high; \
			limit = LEN_HIGH_SYMBOLS; \
			target = MATCH_LEN_MIN + LEN_LOW_SYMBOLS \
					+ LEN_MID_SYMBOLS; \
		} \
	} \
	symbol = 1; \
case seq ## _BITTREE: \
	do { \
		rc_bit(probs[symbol], , , seq ## _BITTREE); \
	} while (symbol < limit); \
	target += symbol - limit; \
} while (0)

#else // HAVE_SMALL

// Unrolled versions
#define seq_4(seq) \
	seq ## 0, \
	seq ## 1, \
	seq ## 2, \
	seq ## 3

#define seq_6(seq) \
	seq ## 0, \
	seq ## 1, \
	seq ## 2, \
	seq ## 3, \
	seq ## 4, \
	seq ## 5

#define seq_8(seq) \
	seq ## 0, \
	seq ## 1, \
	seq ## 2, \
	seq ## 3, \
	seq ## 4, \
	seq ## 5, \
	seq ## 6, \
	seq ## 7

#define seq_len(seq) \
	seq ## _CHOICE, \
	seq ## _LOW0, \
	seq ## _LOW1, \
	seq ## _LOW2, \
	seq ## _CHOICE2, \
	seq ## _MID0, \
	seq ## _MID1, \
	seq ## _MID2, \
	seq ## _HIGH0, \
	seq ## _HIGH1, \
	seq ## _HIGH2, \
	seq ## _HIGH3, \
	seq ## _HIGH4, \
	seq ## _HIGH5, \
	seq ## _HIGH6, \
	seq ## _HIGH7

#define len_decode(target, ld, pos_state, seq) \
do { \
	symbol = 1; \
case seq ## _CHOICE: \
	rc_if_0(ld.choice, seq ## _CHOICE) { \
		rc_update_0(ld.choice); \
		rc_bit_case(ld.low[pos_state][symbol], , , seq ## _LOW0); \
		rc_bit_case(ld.low[pos_state][symbol], , , seq ## _LOW1); \
		rc_bit_case(ld.low[pos_state][symbol], , , seq ## _LOW2); \
		target = symbol - LEN_LOW_SYMBOLS + MATCH_LEN_MIN; \
	} else { \
		rc_update_1(ld.choice); \
case seq ## _CHOICE2: \
		rc_if_0(ld.choice2, seq ## _CHOICE2) { \
			rc_update_0(ld.choice2); \
			rc_bit_case(ld.mid[pos_state][symbol], , , \
					seq ## _MID0); \
			rc_bit_case(ld.mid[pos_state][symbol], , , \
					seq ## _MID1); \
			rc_bit_case(ld.mid[pos_state][symbol], , , \
					seq ## _MID2); \
			target = symbol - LEN_MID_SYMBOLS \
					+ MATCH_LEN_MIN + LEN_LOW_SYMBOLS; \
		} else { \
			rc_update_1(ld.choice2); \
			rc_bit_case(ld.high[symbol], , , seq ## _HIGH0); \
			rc_bit_case(ld.high[symbol], , , seq ## _HIGH1); \
			rc_bit_case(ld.high[symbol], , , seq ## _HIGH2); \
			rc_bit_case(ld.high[symbol], , , seq ## _HIGH3); \
			rc_bit_case(ld.high[symbol], , , seq ## _HIGH4); \
			rc_bit_case(ld.high[symbol], , , seq ## _HIGH5); \
			rc_bit_case(ld.high[symbol], , , seq ## _HIGH6); \
			rc_bit_case(ld.high[symbol], , , seq ## _HIGH7); \
			target = symbol - LEN_HIGH_SYMBOLS \
					+ MATCH_LEN_MIN \
					+ LEN_LOW_SYMBOLS + LEN_MID_SYMBOLS; \
		} \
	} \
} while (0)

#endif // HAVE_SMALL

/// Length decoder probabilities; see comments in lzma_common.h.
typedef struct {
	probability choice;
	probability choice2;
	probability low[POS_STATES_MAX][LEN_LOW_SYMBOLS];
	probability mid[POS_STATES_MAX][LEN_MID_SYMBOLS];
	probability high[LEN_HIGH_SYMBOLS];
} lzma_length_decoder;

typedef struct {
	///////////////////
	// Probabilities //
	///////////////////

	/// Literals; see comments in lzma_common.h.
	probability literal[LITERAL_CODERS_MAX][LITERAL_CODER_SIZE];

	/// If 1, it's a match. Otherwise it's a single 8-bit literal.
	probability is_match[STATES][POS_STATES_MAX];

	/// If 1, it's a repeated match. The distance is one of rep0 .. rep3.
	probability is_rep[STATES];

	/// If 0, distance of a repeated match is rep0.
	/// Otherwise check is_rep1.
	probability is_rep0[STATES];

	/// If 0, distance of a repeated match is rep1.
	/// Otherwise check is_rep2.
	probability is_rep1[STATES];

	/// If 0, distance of a repeated match is rep2. Otherwise it is rep3.
	probability is_rep2[STATES];

	/// If 1, the repeated match has length of one byte. Otherwise
	/// the length is decoded from rep_len_decoder.
	probability is_rep0_long[STATES][POS_STATES_MAX];

	/// Probability tree for the highest two bits of the match distance.
	/// There is a separate probability tree for match lengths of
	/// 2 (i.e. MATCH_LEN_MIN), 3, 4, and [5, 273].
	probability dist_slot[DIST_STATES][DIST_SLOTS];

	/// Probability trees for additional bits for match distance when the
	/// distance is in the range [4, 127].
	probability pos_special[FULL_DISTANCES - DIST_MODEL_END];

	/// Probability tree for the lowest four bits of a match distance
	/// that is equal to or greater than 128.
	probability pos_align[ALIGN_SIZE];

	/// Length of a normal match
	lzma_length_decoder match_len_decoder;

	/// Length of a repeated match
	lzma_length_decoder rep_len_decoder;

	///////////////////
	// Decoder state //
	///////////////////

	// Range coder
	lzma_range_decoder rc;

	// Types of the most recently seen LZMA symbols
	lzma_lzma_state state;

	uint32_t rep0;      ///< Distance of the latest match
	uint32_t rep1;      ///< Distance of second latest match
	uint32_t rep2;      ///< Distance of third latest match
	uint32_t rep3;      ///< Distance of fourth latest match

	uint32_t pos_mask; // (1U << pb) - 1
	uint32_t literal_context_bits;
	uint32_t literal_pos_mask;

	/// Uncompressed size as bytes, or LZMA_VLI_UNKNOWN if end of
	/// payload marker is expected.
	lzma_vli uncompressed_size;

	////////////////////////////////
	// State of incomplete symbol //
	////////////////////////////////

	/// Position where to continue the decoder loop
	enum {
		SEQ_NORMALIZE,
		SEQ_IS_MATCH,
		seq_8(SEQ_LITERAL),
		seq_8(SEQ_LITERAL_MATCHED),
		SEQ_LITERAL_WRITE,
		SEQ_IS_REP,
		seq_len(SEQ_MATCH_LEN),
		seq_6(SEQ_DIST_SLOT),
		SEQ_DIST_MODEL,
		SEQ_DIRECT,
		seq_4(SEQ_ALIGN),
		SEQ_EOPM,
		SEQ_IS_REP0,
		SEQ_SHORTREP,
		SEQ_IS_REP0_LONG,
		SEQ_IS_REP1,
		SEQ_IS_REP2,
		seq_len(SEQ_REP_LEN),
		SEQ_COPY,
	} sequence;

	/// Base of the current probability tree
	probability *probs;

	/// Symbol being decoded. This is also used as an index variable in
	/// bittree decoders: probs[symbol]
	uint32_t symbol;

	/// Used as a loop termination condition on bittree decoders and
	/// direct bits decoder.
	uint32_t limit;

	/// Matched literal decoder: 0x100 or 0 to help avoiding branches.
	/// Bittree reverse decoders: Offset of the next bit: 1 << offset
	uint32_t offset;

	/// If decoding a literal: match byte.
	/// If decoding a match: length of the match.
	uint32_t len;
} lzma_lzma1_decoder;

static lzma_ret
lzma_decode(void *coder_ptr, lzma_dict *restrict dictptr,
		const uint8_t *restrict in,
		size_t *restrict in_pos, size_t in_size)
{
	lzma_lzma1_decoder *restrict coder = coder_ptr;

	////////////////////
	// Initialization //
	////////////////////

	{
		const lzma_ret ret = rc_read_init(
				&coder->rc, in, in_pos, in_size);
		if (ret != LZMA_STREAM_END)
			return ret;
	}

	///////////////
	// Variables //
	///////////////

	// Making local copies of often-used variables improves both
	// speed and readability.

	lzma_dict dict = *dictptr;

	const size_t dict_start = dict.pos;

	// Range decoder
	rc_to_local(coder->rc, *in_pos);

	// State
	uint32_t state = coder->state;
	uint32_t rep0 = coder->rep0;
	uint32_t rep1 = coder->rep1;
	uint32_t rep2 = coder->rep2;
	uint32_t rep3 = coder->rep3;

	const uint32_t pos_mask = coder->pos_mask;

	// These variables are actually needed only if we last time ran
	// out of input in the middle of the decoder loop.
	probability *probs = coder->probs;
	uint32_t symbol = coder->symbol;
	uint32_t limit = coder->limit;
	uint32_t offset = coder->offset;
	uint32_t len = coder->len;

	const uint32_t literal_pos_mask = coder->literal_pos_mask;
	const uint32_t literal_context_bits = coder->literal_context_bits;

	// Temporary variables
	uint32_t pos_state = dict.pos & pos_mask;

	lzma_ret ret = LZMA_OK;

	// If uncompressed size is known, there must be no end of payload
	// marker.
	const bool no_eopm = coder->uncompressed_size
			!= LZMA_VLI_UNKNOWN;
	if (no_eopm && coder->uncompressed_size < dict.limit - dict.pos)
		dict.limit = dict.pos + (size_t)(coder->uncompressed_size);

	// The main decoder loop. The "switch" is used to restart the decoder at
	// correct location. Once restarted, the "switch" is no longer used.
	switch (coder->sequence)
	while (true) {
		// Calculate new pos_state. This is skipped on the first loop
		// since we already calculated it when setting up the local
		// variables.
		pos_state = dict.pos & pos_mask;

	case SEQ_NORMALIZE:
	case SEQ_IS_MATCH:
		if (unlikely(no_eopm && dict.pos == dict.limit))
			break;

		rc_if_0(coder->is_match[state][pos_state], SEQ_IS_MATCH) {
			rc_update_0(coder->is_match[state][pos_state]);

			// It's a literal i.e. a single 8-bit byte.

			probs = literal_subcoder(coder->literal,
					literal_context_bits, literal_pos_mask,
					dict.pos, dict_get(&dict, 0));
			symbol = 1;

			if (is_literal_state(state)) {
				// Decode literal without match byte.
#ifdef HAVE_SMALL
	case SEQ_LITERAL:
				do {
					rc_bit(probs[symbol], , , SEQ_LITERAL);
				} while (symbol < (1 << 8));
#else
				rc_bit_case(probs[symbol], , , SEQ_LITERAL0);
				rc_bit_case(probs[symbol], , , SEQ_LITERAL1);
				rc_bit_case(probs[symbol], , , SEQ_LITERAL2);
				rc_bit_case(probs[symbol], , , SEQ_LITERAL3);
				rc_bit_case(probs[symbol], , , SEQ_LITERAL4);
				rc_bit_case(probs[symbol], , , SEQ_LITERAL5);
				rc_bit_case(probs[symbol], , , SEQ_LITERAL6);
				rc_bit_case(probs[symbol], , , SEQ_LITERAL7);
#endif
			} else {
				// Decode literal with match byte.
				//
				// We store the byte we compare against
				// ("match byte") to "len" to minimize the
				// number of variables we need to store
				// between decoder calls.
				len = (uint32_t)(dict_get(&dict, rep0)) << 1;

				// The usage of "offset" allows omitting some
				// branches, which should give tiny speed
				// improvement on some CPUs. "offset" gets
				// set to zero if match_bit didn't match.
				offset = 0x100;

#ifdef HAVE_SMALL
	case SEQ_LITERAL_MATCHED:
				do {
					const uint32_t match_bit
							= len & offset;
					const uint32_t subcoder_index
							= offset + match_bit
							+ symbol;

					rc_bit(probs[subcoder_index],
							offset &= ~match_bit,
							offset &= match_bit,
							SEQ_LITERAL_MATCHED);

					// It seems to be faster to do this
					// here instead of putting it to the
					// beginning of the loop and then
					// putting the "case" in the middle
					// of the loop.
					len <<= 1;

				} while (symbol < (1 << 8));
#else
				// Unroll the loop.
				uint32_t match_bit;
				uint32_t subcoder_index;

#	define d(seq) \
		case seq: \
			match_bit = len & offset; \
			subcoder_index = offset + match_bit + symbol; \
			rc_bit(probs[subcoder_index], \
					offset &= ~match_bit, \
					offset &= match_bit, \
					seq)

				d(SEQ_LITERAL_MATCHED0);
				len <<= 1;
				d(SEQ_LITERAL_MATCHED1);
				len <<= 1;
				d(SEQ_LITERAL_MATCHED2);
				len <<= 1;
				d(SEQ_LITERAL_MATCHED3);
				len <<= 1;
				d(SEQ_LITERAL_MATCHED4);
				len <<= 1;
				d(SEQ_LITERAL_MATCHED5);
				len <<= 1;
				d(SEQ_LITERAL_MATCHED6);
				len <<= 1;
				d(SEQ_LITERAL_MATCHED7);
#	undef d
#endif
			}

			//update_literal(state);
			// Use a lookup table to update to literal state,
			// since compared to other state updates, this would
			// need two branches.
			static const lzma_lzma_state next_state[] = {
				STATE_LIT_LIT,
				STATE_LIT_LIT,
				STATE_LIT_LIT,
				STATE_LIT_LIT,
				STATE_MATCH_LIT_LIT,
				STATE_REP_LIT_LIT,
				STATE_SHORTREP_LIT_LIT,
				STATE_MATCH_LIT,
				STATE_REP_LIT,
				STATE_SHORTREP_LIT,
				STATE_MATCH_LIT,
				STATE_REP_LIT
			};
			state = next_state[state];

	case SEQ_LITERAL_WRITE:
			if (unlikely(dict_put(&dict, symbol))) {
				coder->sequence = SEQ_LITERAL_WRITE;
				goto out;
			}

			continue;
		}

		// Instead of a new byte we are going to get a byte range
		// (distance and length) which will be repeated from our
		// output history.

		rc_update_1(coder->is_match[state][pos_state]);

	case SEQ_IS_REP:
		rc_if_0(coder->is_rep[state], SEQ_IS_REP) {
			// Not a repeated match
			rc_update_0(coder->is_rep[state]);
			update_match(state);

			// The latest three match distances are kept in
			// memory in case there are repeated matches.
			rep3 = rep2;
			rep2 = rep1;
			rep1 = rep0;

			// Decode the length of the match.
			len_decode(len, coder->match_len_decoder,
					pos_state, SEQ_MATCH_LEN);

			// Prepare to decode the highest two bits of the
			// match distance.
			probs = coder->dist_slot[get_dist_state(len)];
			symbol = 1;

#ifdef HAVE_SMALL
	case SEQ_DIST_SLOT:
			do {
				rc_bit(probs[symbol], , , SEQ_DIST_SLOT);
			} while (symbol < DIST_SLOTS);
#else
			rc_bit_case(probs[symbol], , , SEQ_DIST_SLOT0);
			rc_bit_case(probs[symbol], , , SEQ_DIST_SLOT1);
			rc_bit_case(probs[symbol], , , SEQ_DIST_SLOT2);
			rc_bit_case(probs[symbol], , , SEQ_DIST_SLOT3);
			rc_bit_case(probs[symbol], , , SEQ_DIST_SLOT4);
			rc_bit_case(probs[symbol], , , SEQ_DIST_SLOT5);
#endif
			// Get rid of the highest bit that was needed for
			// indexing of the probability array.
			symbol -= DIST_SLOTS;
			assert(symbol <= 63);

			if (symbol < DIST_MODEL_START) {
				// Match distances [0, 3] have only two bits.
				rep0 = symbol;
			} else {
				// Decode the lowest [1, 29] bits of
				// the match distance.
				limit = (symbol >> 1) - 1;
				assert(limit >= 1 && limit <= 30);
				rep0 = 2 + (symbol & 1);

				if (symbol < DIST_MODEL_END) {
					// Prepare to decode the low bits for
					// a distance of [4, 127].
					assert(limit <= 5);
					rep0 <<= limit;
					assert(rep0 <= 96);
					// -1 is fine, because we start
					// decoding at probs[1], not probs[0].
					// NOTE: This violates the C standard,
					// since we are doing pointer
					// arithmetic past the beginning of
					// the array.
					assert((int32_t)(rep0 - symbol - 1)
							>= -1);
					assert((int32_t)(rep0 - symbol - 1)
							<= 82);
					probs = coder->pos_special + rep0
							- symbol - 1;
					symbol = 1;
					offset = 0;
	case SEQ_DIST_MODEL:
#ifdef HAVE_SMALL
					do {
						rc_bit(probs[symbol], ,
							rep0 += 1U << offset,
							SEQ_DIST_MODEL);
					} while (++offset < limit);
#else
					switch (limit) {
					case 5:
						assert(offset == 0);
						rc_bit(probs[symbol], ,
							rep0 += 1U,
							SEQ_DIST_MODEL);
						++offset;
						--limit;
					case 4:
						rc_bit(probs[symbol], ,
							rep0 += 1U << offset,
							SEQ_DIST_MODEL);
						++offset;
						--limit;
					case 3:
						rc_bit(probs[symbol], ,
							rep0 += 1U << offset,
							SEQ_DIST_MODEL);
						++offset;
						--limit;
					case 2:
						rc_bit(probs[symbol], ,
							rep0 += 1U << offset,
							SEQ_DIST_MODEL);
						++offset;
						--limit;
					case 1:
						// We need "symbol" only for
						// indexing the probability
						// array, thus we can use
						// rc_bit_last() here to omit
						// the unneeded updating of
						// "symbol".
						rc_bit_last(probs[symbol], ,
							rep0 += 1U << offset,
							SEQ_DIST_MODEL);
					}
#endif
				} else {
					// The distance is >= 128. Decode the
					// lower bits without probabilities
					// except the lowest four bits.
					assert(symbol >= 14);
					assert(limit >= 6);
					limit -= ALIGN_BITS;
					assert(limit >= 2);
	case SEQ_DIRECT:
					// Not worth manual unrolling
					do {
						rc_direct(rep0, SEQ_DIRECT);
					} while (--limit > 0);

					// Decode the lowest four bits using
					// probabilities.
					rep0 <<= ALIGN_BITS;
					symbol = 1;
#ifdef HAVE_SMALL
					offset = 0;
	case SEQ_ALIGN:
					do {
						rc_bit(coder->pos_align[
								symbol], ,
							rep0 += 1U << offset,
							SEQ_ALIGN);
					} while (++offset < ALIGN_BITS);
#else
	case SEQ_ALIGN0:
					rc_bit(coder->pos_align[symbol], ,
							rep0 += 1, SEQ_ALIGN0);
	case SEQ_ALIGN1:
					rc_bit(coder->pos_align[symbol], ,
							rep0 += 2, SEQ_ALIGN1);
	case SEQ_ALIGN2:
					rc_bit(coder->pos_align[symbol], ,
							rep0 += 4, SEQ_ALIGN2);
	case SEQ_ALIGN3:
					// Like in SEQ_DIST_MODEL, we don't
					// need "symbol" for anything else
					// than indexing the probability array.
					rc_bit_last(coder->pos_align[symbol], ,
							rep0 += 8, SEQ_ALIGN3);
#endif

					if (rep0 == UINT32_MAX) {
						// End of payload marker was
						// found. It must not be
						// present if uncompressed
						// size is known.
						if (coder->uncompressed_size
						!= LZMA_VLI_UNKNOWN) {
							ret = LZMA_DATA_ERROR;
							goto out;
						}

	case SEQ_EOPM:
						// LZMA1 stream with
						// end-of-payload marker.
						rc_normalize(SEQ_EOPM);
						ret = LZMA_STREAM_END;
						goto out;
					}
				}
			}

			// Validate the distance we just decoded.
			if (unlikely(!dict_is_distance_valid(&dict, rep0))) {
				ret = LZMA_DATA_ERROR;
				goto out;
			}

		} else {
			rc_update_1(coder->is_rep[state]);

			// Repeated match
			//
			// The match distance is a value that we have had
			// earlier. The latest four match distances are
			// available as rep0, rep1, rep2 and rep3. We will
			// now decode which of them is the new distance.
			//
			// There cannot be a match if we haven't produced
			// any output, so check that first.
			if (unlikely(!dict_is_distance_valid(&dict, 0))) {
				ret = LZMA_DATA_ERROR;
				goto out;
			}

	case SEQ_IS_REP0:
			rc_if_0(coder->is_rep0[state], SEQ_IS_REP0) {
				rc_update_0(coder->is_rep0[state]);
				// The distance is rep0.

	case SEQ_IS_REP0_LONG:
				rc_if_0(coder->is_rep0_long[state][pos_state],
						SEQ_IS_REP0_LONG) {
					rc_update_0(coder->is_rep0_long[
							state][pos_state]);

					update_short_rep(state);

	case SEQ_SHORTREP:
					if (unlikely(dict_put(&dict, dict_get(
							&dict, rep0)))) {
						coder->sequence = SEQ_SHORTREP;
						goto out;
					}

					continue;
				}

				// Repeating more than one byte at
				// distance of rep0.
				rc_update_1(coder->is_rep0_long[
						state][pos_state]);

			} else {
				rc_update_1(coder->is_rep0[state]);

	case SEQ_IS_REP1:
				// The distance is rep1, rep2 or rep3. Once
				// we find out which one of these three, it
				// is stored to rep0 and rep1, rep2 and rep3
				// are updated accordingly.
				rc_if_0(coder->is_rep1[state], SEQ_IS_REP1) {
					rc_update_0(coder->is_rep1[state]);

					const uint32_t distance = rep1;
					rep1 = rep0;
					rep0 = distance;

				} else {
					rc_update_1(coder->is_rep1[state]);
	case SEQ_IS_REP2:
					rc_if_0(coder->is_rep2[state],
							SEQ_IS_REP2) {
						rc_update_0(coder->is_rep2[
								state]);

						const uint32_t distance = rep2;
						rep2 = rep1;
						rep1 = rep0;
						rep0 = distance;

					} else {
						rc_update_1(coder->is_rep2[
								state]);

						const uint32_t distance = rep3;
						rep3 = rep2;
						rep2 = rep1;
						rep1 = rep0;
						rep0 = distance;
					}
				}
			}

			update_long_rep(state);

			// Decode the length of the repeated match.
			len_decode(len, coder->rep_len_decoder,
					pos_state, SEQ_REP_LEN);
		}

		/////////////////////////////////
		// Repeat from history buffer. //
		/////////////////////////////////

		// The length is always between these limits. There is no way
		// to trigger the algorithm to set len outside this range.
		assert(len >= MATCH_LEN_MIN);
		assert(len <= MATCH_LEN_MAX);

	case SEQ_COPY:
		// Repeat len bytes from distance of rep0.
		if (unlikely(dict_repeat(&dict, rep0, &len))) {
			coder->sequence = SEQ_COPY;
			goto out;
		}
	}

	rc_normalize(SEQ_NORMALIZE);
	coder->sequence = SEQ_IS_MATCH;

out:
	// Save state

	// NOTE: Must not copy dict.limit.
	dictptr->pos = dict.pos;
	dictptr->full = dict.full;

	rc_from_local(coder->rc, *in_pos);

	coder->state = state;
	coder->rep0 = rep0;
	coder->rep1 = rep1;
	coder->rep2 = rep2;
	coder->rep3 = rep3;

	coder->probs = probs;
	coder->symbol = symbol;
	coder->limit = limit;
	coder->offset = offset;
	coder->len = len;

	// Update the remaining amount of uncompressed data if uncompressed
	// size was known.
	if (coder->uncompressed_size != LZMA_VLI_UNKNOWN) {
		coder->uncompressed_size -= dict.pos - dict_start;

		// Since there cannot be end of payload marker if the
		// uncompressed size was known, we check here if we
		// finished decoding.
		if (coder->uncompressed_size == 0 && ret == LZMA_OK
				&& coder->sequence != SEQ_NORMALIZE)
			ret = coder->sequence == SEQ_IS_MATCH
					? LZMA_STREAM_END : LZMA_DATA_ERROR;
	}

	// We can do an additional check in the range decoder to catch some
	// corrupted files.
	if (ret == LZMA_STREAM_END) {
		if (!rc_is_finished(coder->rc))
			ret = LZMA_DATA_ERROR;

		// Reset the range decoder so that it is ready to reinitialize
		// for a new LZMA2 chunk.
		rc_reset(coder->rc);
	}

	return ret;
}

static void
lzma_decoder_uncompressed(void *coder_ptr, lzma_vli uncompressed_size)
{
	lzma_lzma1_decoder *coder = coder_ptr;
	coder->uncompressed_size = uncompressed_size;
}

static void
lzma_decoder_reset(void *coder_ptr, const void *opt)
{
	lzma_lzma1_decoder *coder = coder_ptr;
	const lzma_options_lzma *options = opt;

	// NOTE: We assume that lc/lp/pb are valid since they were
	// successfully decoded with lzma_lzma_decode_properties().

	// Calculate pos_mask. We don't need pos_bits as is for anything.
	coder->pos_mask = (1U << options->pb) - 1;

	// Initialize the literal decoder.
	literal_init(coder->literal, options->lc, options->lp);

	coder->literal_context_bits = options->lc;
	coder->literal_pos_mask = (1U << options->lp) - 1;

	// State
	coder->state = STATE_LIT_LIT;
	coder->rep0 = 0;
	coder->rep1 = 0;
	coder->rep2 = 0;
	coder->rep3 = 0;
	coder->pos_mask = (1U << options->pb) - 1;

	// Range decoder
	rc_reset(coder->rc);

	// Bit and bittree decoders
	for (uint32_t i = 0; i < STATES; ++i) {
		for (uint32_t j = 0; j <= coder->pos_mask; ++j) {
			bit_reset(coder->is_match[i][j]);
			bit_reset(coder->is_rep0_long[i][j]);
		}

		bit_reset(coder->is_rep[i]);
		bit_reset(coder->is_rep0[i]);
		bit_reset(coder->is_rep1[i]);
		bit_reset(coder->is_rep2[i]);
	}

	for (uint32_t i = 0; i < DIST_STATES; ++i)
		bittree_reset(coder->dist_slot[i], DIST_SLOT_BITS);

	for (uint32_t i = 0; i < FULL_DISTANCES - DIST_MODEL_END; ++i)
		bit_reset(coder->pos_special[i]);

	bittree_reset(coder->pos_align, ALIGN_BITS);

	// Len decoders (also bit/bittree)
	const uint32_t num_pos_states = 1U << options->pb;
	bit_reset(coder->match_len_decoder.choice);
	bit_reset(coder->match_len_decoder.choice2);
	bit_reset(coder->rep_len_decoder.choice);
	bit_reset(coder->rep_len_decoder.choice2);

	for (uint32_t pos_state = 0; pos_state < num_pos_states; ++pos_state) {
		bittree_reset(coder->match_len_decoder.low[pos_state],
				LEN_LOW_BITS);
		bittree_reset(coder->match_len_decoder.mid[pos_state],
				LEN_MID_BITS);

		bittree_reset(coder->rep_len_decoder.low[pos_state],
				LEN_LOW_BITS);
		bittree_reset(coder->rep_len_decoder.mid[pos_state],
				LEN_MID_BITS);
	}

	bittree_reset(coder->match_len_decoder.high, LEN_HIGH_BITS);
	bittree_reset(coder->rep_len_decoder.high, LEN_HIGH_BITS);

	coder->sequence = SEQ_IS_MATCH;
	coder->probs = NULL;
	coder->symbol = 0;
	coder->limit = 0;
	coder->offset = 0;
	coder->len = 0;

	return;
}

extern lzma_ret
lzma_lzma_decoder_create(lzma_lz_decoder *lz, const lzma_allocator *allocator,
		const void *opt, lzma_lz_options *lz_options)
{
	if (lz->coder == NULL) {
		lz->coder = lzma_alloc(sizeof(lzma_lzma1_decoder), allocator);
		if (lz->coder == NULL)
			return LZMA_MEM_ERROR;

		lz->code = &lzma_decode;
		lz->reset = &lzma_decoder_reset;
		lz->set_uncompressed = &lzma_decoder_uncompressed;
	}

	// All dictionary sizes are OK here. LZ decoder will take care of
	// the special cases.
	const lzma_options_lzma *options = opt;
	lz_options->dict_size = options->dict_size;
	lz_options->preset_dict = options->preset_dict;
	lz_options->preset_dict_size = options->preset_dict_size;

	return LZMA_OK;
}

/// Allocate and initialize LZMA decoder. This is used only via LZ
/// initialization (lzma_lzma_decoder_init() passes function pointer to
/// the LZ initialization).
static lzma_ret
lzma_decoder_init(lzma_lz_decoder *lz, const lzma_allocator *allocator,
		const void *options, lzma_lz_options *lz_options)
{
	if (!is_lclppb_valid(options))
		return LZMA_PROG_ERROR;

	return_if_error(lzma_lzma_decoder_create(
			lz, allocator, options, lz_options));

	lzma_decoder_reset(lz->coder, options);
	lzma_decoder_uncompressed(lz->coder, LZMA_VLI_UNKNOWN);

	return LZMA_OK;
}

extern lzma_ret
lzma_lzma_decoder_init(lzma_next_coder *next, const lzma_allocator *allocator,
		const lzma_filter_info *filters)
{
	// LZMA can only be the last filter in the chain. This is enforced
	// by the raw_decoder initialization.
	assert(filters[1].init == NULL);

	return lzma_lz_decoder_init(next, allocator, filters,
			&lzma_decoder_init);
}

extern bool
lzma_lzma_lclppb_decode(lzma_options_lzma *options, uint8_t byte)
{
	if (byte > (4 * 5 + 4) * 9 + 8)
		return true;

	// See the file format specification to understand this.
	options->pb = byte / (9 * 5);
	byte -= options->pb * 9 * 5;
	options->lp = byte / 9;
	options->lc = byte - options->lp * 9;

	return options->lc + options->lp > LZMA_LCLP_MAX;
}

extern uint64_t
lzma_lzma_decoder_memusage_nocheck(const void *options)
{
	const lzma_options_lzma *const opt = options;
	return sizeof(lzma_lzma1_decoder)
			+ lzma_lz_decoder_memusage(opt->dict_size);
}

extern uint64_t
lzma_lzma_decoder_memusage(const void *options)
{
	if (!is_lclppb_valid(options))
		return UINT64_MAX;

	return lzma_lzma_decoder_memusage_nocheck(options);
}

extern lzma_ret
lzma_lzma_props_decode(void **options, const lzma_allocator *allocator,
		const uint8_t *props, size_t props_size)
{
	if (props_size != 5)
		return LZMA_OPTIONS_ERROR;

	lzma_options_lzma *opt
			= lzma_alloc(sizeof(lzma_options_lzma), allocator);
	if (opt == NULL)
		return LZMA_MEM_ERROR;

	if (lzma_lzma_lclppb_decode(opt, props[0]))
		goto error;

	// All dictionary sizes are accepted, including zero. LZ decoder
	// will automatically use a dictionary at least a few KiB even if
	// a smaller dictionary is requested.
	opt->dict_size = read32le(props + 1);

	opt->preset_dict = NULL;
	opt->preset_dict_size = 0;

	*options = opt;

	return LZMA_OK;

error:
	lzma_free(opt, allocator);
	return LZMA_OPTIONS_ERROR;
}

/*** End of inlined file: lzma_decoder.c ***/


/*** Start of inlined file: lzma_encoder.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       lzma_encoder.c
/// \brief      LZMA encoder
///
//  Authors:    Igor Pavlov
//              Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

/////////////
// Literal //
/////////////

static inline void
literal_matched(lzma_range_encoder *rc, probability *subcoder,
		uint32_t match_byte, uint32_t symbol)
{
	uint32_t offset = 0x100;
	symbol += UINT32_C(1) << 8;

	do {
		match_byte <<= 1;
		const uint32_t match_bit = match_byte & offset;
		const uint32_t subcoder_index
				= offset + match_bit + (symbol >> 8);
		const uint32_t bit = (symbol >> 7) & 1;
		rc_bit(rc, &subcoder[subcoder_index], bit);

		symbol <<= 1;
		offset &= ~(match_byte ^ symbol);

	} while (symbol < (UINT32_C(1) << 16));
}

static inline void
literal(lzma_lzma1_encoder *coder, lzma_mf *mf, uint32_t position)
{
	// Locate the literal byte to be encoded and the subcoder.
	const uint8_t cur_byte = mf->buffer[
			mf->read_pos - mf->read_ahead];
	probability *subcoder = literal_subcoder(coder->literal,
			coder->literal_context_bits, coder->literal_pos_mask,
			position, mf->buffer[mf->read_pos - mf->read_ahead - 1]);

	if (is_literal_state(coder->state)) {
		// Previous LZMA-symbol was a literal. Encode a normal
		// literal without a match byte.
		rc_bittree(&coder->rc, subcoder, 8, cur_byte);
	} else {
		// Previous LZMA-symbol was a match. Use the last byte of
		// the match as a "match byte". That is, compare the bits
		// of the current literal and the match byte.
		const uint8_t match_byte = mf->buffer[
				mf->read_pos - coder->reps[0] - 1
				- mf->read_ahead];
		literal_matched(&coder->rc, subcoder, match_byte, cur_byte);
	}

	update_literal(coder->state);
}

//////////////////
// Match length //
//////////////////

static void
length_update_prices(lzma_length_encoder *lc, const uint32_t pos_state)
{
	const uint32_t table_size = lc->table_size;
	lc->counters[pos_state] = table_size;

	const uint32_t a0 = rc_bit_0_price(lc->choice);
	const uint32_t a1 = rc_bit_1_price(lc->choice);
	const uint32_t b0 = a1 + rc_bit_0_price(lc->choice2);
	const uint32_t b1 = a1 + rc_bit_1_price(lc->choice2);
	uint32_t *const prices = lc->prices[pos_state];

	uint32_t i;
	for (i = 0; i < table_size && i < LEN_LOW_SYMBOLS; ++i)
		prices[i] = a0 + rc_bittree_price(lc->low[pos_state],
				LEN_LOW_BITS, i);

	for (; i < table_size && i < LEN_LOW_SYMBOLS + LEN_MID_SYMBOLS; ++i)
		prices[i] = b0 + rc_bittree_price(lc->mid[pos_state],
				LEN_MID_BITS, i - LEN_LOW_SYMBOLS);

	for (; i < table_size; ++i)
		prices[i] = b1 + rc_bittree_price(lc->high, LEN_HIGH_BITS,
				i - LEN_LOW_SYMBOLS - LEN_MID_SYMBOLS);

	return;
}

static inline void
length(lzma_range_encoder *rc, lzma_length_encoder *lc,
		const uint32_t pos_state, uint32_t len, const bool fast_mode)
{
	assert(len <= MATCH_LEN_MAX);
	len -= MATCH_LEN_MIN;

	if (len < LEN_LOW_SYMBOLS) {
		rc_bit(rc, &lc->choice, 0);
		rc_bittree(rc, lc->low[pos_state], LEN_LOW_BITS, len);
	} else {
		rc_bit(rc, &lc->choice, 1);
		len -= LEN_LOW_SYMBOLS;

		if (len < LEN_MID_SYMBOLS) {
			rc_bit(rc, &lc->choice2, 0);
			rc_bittree(rc, lc->mid[pos_state], LEN_MID_BITS, len);
		} else {
			rc_bit(rc, &lc->choice2, 1);
			len -= LEN_MID_SYMBOLS;
			rc_bittree(rc, lc->high, LEN_HIGH_BITS, len);
		}
	}

	// Only getoptimum uses the prices so don't update the table when
	// in fast mode.
	if (!fast_mode)
		if (--lc->counters[pos_state] == 0)
			length_update_prices(lc, pos_state);
}

///////////
// Match //
///////////

static inline void
match(lzma_lzma1_encoder *coder, const uint32_t pos_state,
		const uint32_t distance, const uint32_t len)
{
	update_match(coder->state);

	length(&coder->rc, &coder->match_len_encoder, pos_state, len,
			coder->fast_mode);

	const uint32_t dist_slot = get_dist_slot(distance);
	const uint32_t dist_state = get_dist_state(len);
	rc_bittree(&coder->rc, coder->dist_slot[dist_state],
			DIST_SLOT_BITS, dist_slot);

	if (dist_slot >= DIST_MODEL_START) {
		const uint32_t footer_bits = (dist_slot >> 1) - 1;
		const uint32_t base = (2 | (dist_slot & 1)) << footer_bits;
		const uint32_t dist_reduced = distance - base;

		if (dist_slot < DIST_MODEL_END) {
			// Careful here: base - dist_slot - 1 can be -1, but
			// rc_bittree_reverse starts at probs[1], not probs[0].
			rc_bittree_reverse(&coder->rc,
				coder->dist_special + base - dist_slot - 1,
				footer_bits, dist_reduced);
		} else {
			rc_direct(&coder->rc, dist_reduced >> ALIGN_BITS,
					footer_bits - ALIGN_BITS);
			rc_bittree_reverse(
					&coder->rc, coder->dist_align,
					ALIGN_BITS, dist_reduced & ALIGN_MASK);
			++coder->align_price_count;
		}
	}

	coder->reps[3] = coder->reps[2];
	coder->reps[2] = coder->reps[1];
	coder->reps[1] = coder->reps[0];
	coder->reps[0] = distance;
	++coder->match_price_count;
}

////////////////////
// Repeated match //
////////////////////

static inline void
rep_match(lzma_lzma1_encoder *coder, const uint32_t pos_state,
		const uint32_t rep, const uint32_t len)
{
	if (rep == 0) {
		rc_bit(&coder->rc, &coder->is_rep0[coder->state], 0);
		rc_bit(&coder->rc,
				&coder->is_rep0_long[coder->state][pos_state],
				len != 1);
	} else {
		const uint32_t distance = coder->reps[rep];
		rc_bit(&coder->rc, &coder->is_rep0[coder->state], 1);

		if (rep == 1) {
			rc_bit(&coder->rc, &coder->is_rep1[coder->state], 0);
		} else {
			rc_bit(&coder->rc, &coder->is_rep1[coder->state], 1);
			rc_bit(&coder->rc, &coder->is_rep2[coder->state],
					rep - 2);

			if (rep == 3)
				coder->reps[3] = coder->reps[2];

			coder->reps[2] = coder->reps[1];
		}

		coder->reps[1] = coder->reps[0];
		coder->reps[0] = distance;
	}

	if (len == 1) {
		update_short_rep(coder->state);
	} else {
		length(&coder->rc, &coder->rep_len_encoder, pos_state, len,
				coder->fast_mode);
		update_long_rep(coder->state);
	}
}

//////////
// Main //
//////////

static void
encode_symbol(lzma_lzma1_encoder *coder, lzma_mf *mf,
		uint32_t back, uint32_t len, uint32_t position)
{
	const uint32_t pos_state = position & coder->pos_mask;

	if (back == UINT32_MAX) {
		// Literal i.e. eight-bit byte
		assert(len == 1);
		rc_bit(&coder->rc,
				&coder->is_match[coder->state][pos_state], 0);
		literal(coder, mf, position);
	} else {
		// Some type of match
		rc_bit(&coder->rc,
			&coder->is_match[coder->state][pos_state], 1);

		if (back < REPS) {
			// It's a repeated match i.e. the same distance
			// has been used earlier.
			rc_bit(&coder->rc, &coder->is_rep[coder->state], 1);
			rep_match(coder, pos_state, back, len);
		} else {
			// Normal match
			rc_bit(&coder->rc, &coder->is_rep[coder->state], 0);
			match(coder, pos_state, back - REPS, len);
		}
	}

	assert(mf->read_ahead >= len);
	mf->read_ahead -= len;
}

static bool
encode_init(lzma_lzma1_encoder *coder, lzma_mf *mf)
{
	assert(mf_position(mf) == 0);

	if (mf->read_pos == mf->read_limit) {
		if (mf->action == LZMA_RUN)
			return false; // We cannot do anything.

		// We are finishing (we cannot get here when flushing).
		assert(mf->write_pos == mf->read_pos);
		assert(mf->action == LZMA_FINISH);
	} else {
		// Do the actual initialization. The first LZMA symbol must
		// always be a literal.
		mf_skip(mf, 1);
		mf->read_ahead = 0;
		rc_bit(&coder->rc, &coder->is_match[0][0], 0);
		rc_bittree(&coder->rc, coder->literal[0], 8, mf->buffer[0]);
	}

	// Initialization is done (except if empty file).
	coder->is_initialized = true;

	return true;
}

static void
encode_eopm(lzma_lzma1_encoder *coder, uint32_t position)
{
	const uint32_t pos_state = position & coder->pos_mask;
	rc_bit(&coder->rc, &coder->is_match[coder->state][pos_state], 1);
	rc_bit(&coder->rc, &coder->is_rep[coder->state], 0);
	match(coder, pos_state, UINT32_MAX, MATCH_LEN_MIN);
}

/// Number of bytes that a single encoding loop in lzma_lzma_encode() can
/// consume from the dictionary. This limit comes from lzma_lzma_optimum()
/// and may need to be updated if that function is significantly modified.
#define LOOP_INPUT_MAX (OPTS + 1)

extern lzma_ret
lzma_lzma_encode(lzma_lzma1_encoder *restrict coder, lzma_mf *restrict mf,
		uint8_t *restrict out, size_t *restrict out_pos,
		size_t out_size, uint32_t limit)
{
	// Initialize the stream if no data has been encoded yet.
	if (!coder->is_initialized && !encode_init(coder, mf))
		return LZMA_OK;

	// Get the lowest bits of the uncompressed offset from the LZ layer.
	uint32_t position = mf_position(mf);

	while (true) {
		// Encode pending bits, if any. Calling this before encoding
		// the next symbol is needed only with plain LZMA, since
		// LZMA2 always provides big enough buffer to flush
		// everything out from the range encoder. For the same reason,
		// rc_encode() never returns true when this function is used
		// as part of LZMA2 encoder.
		if (rc_encode(&coder->rc, out, out_pos, out_size)) {
			assert(limit == UINT32_MAX);
			return LZMA_OK;
		}

		// With LZMA2 we need to take care that compressed size of
		// a chunk doesn't get too big.
		// FIXME? Check if this could be improved.
		if (limit != UINT32_MAX
				&& (mf->read_pos - mf->read_ahead >= limit
					|| *out_pos + rc_pending(&coder->rc)
						>= LZMA2_CHUNK_MAX
							- LOOP_INPUT_MAX))
			break;

		// Check that there is some input to process.
		if (mf->read_pos >= mf->read_limit) {
			if (mf->action == LZMA_RUN)
				return LZMA_OK;

			if (mf->read_ahead == 0)
				break;
		}

		// Get optimal match (repeat position and length).
		// Value ranges for pos:
		//   - [0, REPS): repeated match
		//   - [REPS, UINT32_MAX):
		//     match at (pos - REPS)
		//   - UINT32_MAX: not a match but a literal
		// Value ranges for len:
		//   - [MATCH_LEN_MIN, MATCH_LEN_MAX]
		uint32_t len;
		uint32_t back;

		if (coder->fast_mode)
			lzma_lzma_optimum_fast(coder, mf, &back, &len);
		else
			lzma_lzma_optimum_normal(
					coder, mf, &back, &len, position);

		encode_symbol(coder, mf, back, len, position);

		position += len;
	}

	if (!coder->is_flushed) {
		coder->is_flushed = true;

		// We don't support encoding plain LZMA streams without EOPM,
		// and LZMA2 doesn't use EOPM at LZMA level.
		if (limit == UINT32_MAX)
			encode_eopm(coder, position);

		// Flush the remaining bytes from the range encoder.
		rc_flush(&coder->rc);

		// Copy the remaining bytes to the output buffer. If there
		// isn't enough output space, we will copy out the remaining
		// bytes on the next call to this function by using
		// the rc_encode() call in the encoding loop above.
		if (rc_encode(&coder->rc, out, out_pos, out_size)) {
			assert(limit == UINT32_MAX);
			return LZMA_OK;
		}
	}

	// Make it ready for the next LZMA2 chunk.
	coder->is_flushed = false;

	return LZMA_STREAM_END;
}

static lzma_ret
lzma_encode(void *coder, lzma_mf *restrict mf,
		uint8_t *restrict out, size_t *restrict out_pos,
		size_t out_size)
{
	// Plain LZMA has no support for sync-flushing.
	if (unlikely(mf->action == LZMA_SYNC_FLUSH))
		return LZMA_OPTIONS_ERROR;

	return lzma_lzma_encode(coder, mf, out, out_pos, out_size, UINT32_MAX);
}

////////////////////
// Initialization //
////////////////////

static bool
is_options_valid(const lzma_options_lzma *options)
{
	// Validate some of the options. LZ encoder validates nice_len too
	// but we need a valid value here earlier.
	return is_lclppb_valid(options)
			&& options->nice_len >= MATCH_LEN_MIN
			&& options->nice_len <= MATCH_LEN_MAX
			&& (options->mode == LZMA_MODE_FAST
				|| options->mode == LZMA_MODE_NORMAL);
}

static void
set_lz_options(lzma_lz_options *lz_options, const lzma_options_lzma *options)
{
	// LZ encoder initialization does the validation for these so we
	// don't need to validate here.
	lz_options->before_size = OPTS;
	lz_options->dict_size = options->dict_size;
	lz_options->after_size = LOOP_INPUT_MAX;
	lz_options->match_len_max = MATCH_LEN_MAX;
	lz_options->nice_len = options->nice_len;
	lz_options->match_finder = options->mf;
	lz_options->depth = options->depth;
	lz_options->preset_dict = options->preset_dict;
	lz_options->preset_dict_size = options->preset_dict_size;
	return;
}

static void
length_encoder_reset(lzma_length_encoder *lencoder,
		const uint32_t num_pos_states, const bool fast_mode)
{
	bit_reset(lencoder->choice);
	bit_reset(lencoder->choice2);

	for (size_t pos_state = 0; pos_state < num_pos_states; ++pos_state) {
		bittree_reset(lencoder->low[pos_state], LEN_LOW_BITS);
		bittree_reset(lencoder->mid[pos_state], LEN_MID_BITS);
	}

	bittree_reset(lencoder->high, LEN_HIGH_BITS);

	if (!fast_mode)
		for (uint32_t pos_state = 0; pos_state < num_pos_states;
				++pos_state)
			length_update_prices(lencoder, pos_state);

	return;
}

extern lzma_ret
lzma_lzma_encoder_reset(lzma_lzma1_encoder *coder,
		const lzma_options_lzma *options)
{
	if (!is_options_valid(options))
		return LZMA_OPTIONS_ERROR;

	coder->pos_mask = (1U << options->pb) - 1;
	coder->literal_context_bits = options->lc;
	coder->literal_pos_mask = (1U << options->lp) - 1;

	// Range coder
	rc_reset(&coder->rc);

	// State
	coder->state = STATE_LIT_LIT;
	for (size_t i = 0; i < REPS; ++i)
		coder->reps[i] = 0;

	literal_init(coder->literal, options->lc, options->lp);

	// Bit encoders
	for (size_t i = 0; i < STATES; ++i) {
		for (size_t j = 0; j <= coder->pos_mask; ++j) {
			bit_reset(coder->is_match[i][j]);
			bit_reset(coder->is_rep0_long[i][j]);
		}

		bit_reset(coder->is_rep[i]);
		bit_reset(coder->is_rep0[i]);
		bit_reset(coder->is_rep1[i]);
		bit_reset(coder->is_rep2[i]);
	}

	for (size_t i = 0; i < FULL_DISTANCES - DIST_MODEL_END; ++i)
		bit_reset(coder->dist_special[i]);

	// Bit tree encoders
	for (size_t i = 0; i < DIST_STATES; ++i)
		bittree_reset(coder->dist_slot[i], DIST_SLOT_BITS);

	bittree_reset(coder->dist_align, ALIGN_BITS);

	// Length encoders
	length_encoder_reset(&coder->match_len_encoder,
			1U << options->pb, coder->fast_mode);

	length_encoder_reset(&coder->rep_len_encoder,
			1U << options->pb, coder->fast_mode);

	// Price counts are incremented every time appropriate probabilities
	// are changed. price counts are set to zero when the price tables
	// are updated, which is done when the appropriate price counts have
	// big enough value, and lzma_mf.read_ahead == 0 which happens at
	// least every OPTS (a few thousand) possible price count increments.
	//
	// By resetting price counts to UINT32_MAX / 2, we make sure that the
	// price tables will be initialized before they will be used (since
	// the value is definitely big enough), and that it is OK to increment
	// price counts without risk of integer overflow (since UINT32_MAX / 2
	// is small enough). The current code doesn't increment price counts
	// before initializing price tables, but it maybe done in future if
	// we add support for saving the state between LZMA2 chunks.
	coder->match_price_count = UINT32_MAX / 2;
	coder->align_price_count = UINT32_MAX / 2;

	coder->opts_end_index = 0;
	coder->opts_current_index = 0;

	return LZMA_OK;
}

extern lzma_ret
lzma_lzma_encoder_create(void **coder_ptr,
		const lzma_allocator *allocator,
		const lzma_options_lzma *options, lzma_lz_options *lz_options)
{
	// Allocate lzma_lzma1_encoder if it wasn't already allocated.
	if (*coder_ptr == NULL) {
		*coder_ptr = lzma_alloc(sizeof(lzma_lzma1_encoder), allocator);
		if (*coder_ptr == NULL)
			return LZMA_MEM_ERROR;
	}

	lzma_lzma1_encoder *coder = *coder_ptr;

	// Set compression mode. We haven't validates the options yet,
	// but it's OK here, since nothing bad happens with invalid
	// options in the code below, and they will get rejected by
	// lzma_lzma_encoder_reset() call at the end of this function.
	switch (options->mode) {
		case LZMA_MODE_FAST:
			coder->fast_mode = true;
			break;

		case LZMA_MODE_NORMAL: {
			coder->fast_mode = false;

			// Set dist_table_size.
			// Round the dictionary size up to next 2^n.
			uint32_t log_size = 0;
			while ((UINT32_C(1) << log_size) < options->dict_size)
				++log_size;

			coder->dist_table_size = log_size * 2;

			// Length encoders' price table size
			coder->match_len_encoder.table_size
				= options->nice_len + 1 - MATCH_LEN_MIN;
			coder->rep_len_encoder.table_size
				= options->nice_len + 1 - MATCH_LEN_MIN;
			break;
		}

		default:
			return LZMA_OPTIONS_ERROR;
	}

	// We don't need to write the first byte as literal if there is
	// a non-empty preset dictionary. encode_init() wouldn't even work
	// if there is a non-empty preset dictionary, because encode_init()
	// assumes that position is zero and previous byte is also zero.
	coder->is_initialized = options->preset_dict != NULL
			&& options->preset_dict_size > 0;
	coder->is_flushed = false;

	set_lz_options(lz_options, options);

	return lzma_lzma_encoder_reset(coder, options);
}

static lzma_ret
lzma_encoder_init(lzma_lz_encoder *lz, const lzma_allocator *allocator,
		const void *options, lzma_lz_options *lz_options)
{
	lz->code = &lzma_encode;
	return lzma_lzma_encoder_create(
			&lz->coder, allocator, options, lz_options);
}

extern lzma_ret
lzma_lzma_encoder_init(lzma_next_coder *next, const lzma_allocator *allocator,
		const lzma_filter_info *filters)
{
	return lzma_lz_encoder_init(
			next, allocator, filters, &lzma_encoder_init);
}

extern uint64_t
lzma_lzma_encoder_memusage(const void *options)
{
	if (!is_options_valid(options))
		return UINT64_MAX;

	lzma_lz_options lz_options;
	set_lz_options(&lz_options, options);

	const uint64_t lz_memusage = lzma_lz_encoder_memusage(&lz_options);
	if (lz_memusage == UINT64_MAX)
		return UINT64_MAX;

	return (uint64_t)(sizeof(lzma_lzma1_encoder)) + lz_memusage;
}

extern bool
lzma_lzma_lclppb_encode(const lzma_options_lzma *options, uint8_t *byte)
{
	if (!is_lclppb_valid(options))
		return true;

	*byte = (options->pb * 5 + options->lp) * 9 + options->lc;
	assert(*byte <= (4 * 5 + 4) * 9 + 8);

	return false;
}

#ifdef HAVE_ENCODER_LZMA1
extern lzma_ret
lzma_lzma_props_encode(const void *options, uint8_t *out)
{
	const lzma_options_lzma *const opt = options;

	if (lzma_lzma_lclppb_encode(opt, out))
		return LZMA_PROG_ERROR;

	write32le(out + 1, opt->dict_size);

	return LZMA_OK;
}
#endif

extern LZMA_API(lzma_bool)
lzma_mode_is_supported(lzma_mode mode)
{
	return mode == LZMA_MODE_FAST || mode == LZMA_MODE_NORMAL;
}

/*** End of inlined file: lzma_encoder.c ***/


/*** Start of inlined file: lzma_encoder_optimum_fast.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       lzma_encoder_optimum_fast.c
//
//  Author:     Igor Pavlov
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "memcmplen.h"

#define change_pair(small_dist, big_dist) \
	(((big_dist) >> 7) > (small_dist))

extern void
lzma_lzma_optimum_fast(lzma_lzma1_encoder *restrict coder,
		lzma_mf *restrict mf,
		uint32_t *restrict back_res, uint32_t *restrict len_res)
{
	const uint32_t nice_len = mf->nice_len;

	uint32_t len_main;
	uint32_t matches_count;
	if (mf->read_ahead == 0) {
		len_main = mf_find(mf, &matches_count, coder->matches);
	} else {
		assert(mf->read_ahead == 1);
		len_main = coder->longest_match_length;
		matches_count = coder->matches_count;
	}

	const uint8_t *buf = mf_ptr(mf) - 1;
	const uint32_t buf_avail = my_min(mf_avail(mf) + 1, MATCH_LEN_MAX);

	if (buf_avail < 2) {
		// There's not enough input left to encode a match.
		*back_res = UINT32_MAX;
		*len_res = 1;
		return;
	}

	// Look for repeated matches; scan the previous four match distances
	uint32_t rep_len = 0;
	uint32_t rep_index = 0;

	for (uint32_t i = 0; i < REPS; ++i) {
		// Pointer to the beginning of the match candidate
		const uint8_t *const buf_back = buf - coder->reps[i] - 1;

		// If the first two bytes (2 == MATCH_LEN_MIN) do not match,
		// this rep is not useful.
		if (not_equal_16(buf, buf_back))
			continue;

		// The first two bytes matched.
		// Calculate the length of the match.
		const uint32_t len = lzma_memcmplen(
				buf, buf_back, 2, buf_avail);

		// If we have found a repeated match that is at least
		// nice_len long, return it immediately.
		if (len >= nice_len) {
			*back_res = i;
			*len_res = len;
			mf_skip(mf, len - 1);
			return;
		}

		if (len > rep_len) {
			rep_index = i;
			rep_len = len;
		}
	}

	// We didn't find a long enough repeated match. Encode it as a normal
	// match if the match length is at least nice_len.
	if (len_main >= nice_len) {
		*back_res = coder->matches[matches_count - 1].dist + REPS;
		*len_res = len_main;
		mf_skip(mf, len_main - 1);
		return;
	}

	uint32_t back_main = 0;
	if (len_main >= 2) {
		back_main = coder->matches[matches_count - 1].dist;

		while (matches_count > 1 && len_main ==
				coder->matches[matches_count - 2].len + 1) {
			if (!change_pair(coder->matches[
						matches_count - 2].dist,
					back_main))
				break;

			--matches_count;
			len_main = coder->matches[matches_count - 1].len;
			back_main = coder->matches[matches_count - 1].dist;
		}

		if (len_main == 2 && back_main >= 0x80)
			len_main = 1;
	}

	if (rep_len >= 2) {
		if (rep_len + 1 >= len_main
				|| (rep_len + 2 >= len_main
					&& back_main > (UINT32_C(1) << 9))
				|| (rep_len + 3 >= len_main
					&& back_main > (UINT32_C(1) << 15))) {
			*back_res = rep_index;
			*len_res = rep_len;
			mf_skip(mf, rep_len - 1);
			return;
		}
	}

	if (len_main < 2 || buf_avail <= 2) {
		*back_res = UINT32_MAX;
		*len_res = 1;
		return;
	}

	// Get the matches for the next byte. If we find a better match,
	// the current byte is encoded as a literal.
	coder->longest_match_length = mf_find(mf,
			&coder->matches_count, coder->matches);

	if (coder->longest_match_length >= 2) {
		const uint32_t new_dist = coder->matches[
				coder->matches_count - 1].dist;

		if ((coder->longest_match_length >= len_main
					&& new_dist < back_main)
				|| (coder->longest_match_length == len_main + 1
					&& !change_pair(back_main, new_dist))
				|| (coder->longest_match_length > len_main + 1)
				|| (coder->longest_match_length + 1 >= len_main
					&& len_main >= 3
					&& change_pair(new_dist, back_main))) {
			*back_res = UINT32_MAX;
			*len_res = 1;
			return;
		}
	}

	// In contrast to LZMA SDK, dictionary could not have been moved
	// between mf_find() calls, thus it is safe to just increment
	// the old buf pointer instead of recalculating it with mf_ptr().
	++buf;

	const uint32_t limit = my_max(2, len_main - 1);

	for (uint32_t i = 0; i < REPS; ++i) {
		if (memcmp(buf, buf - coder->reps[i] - 1, limit) == 0) {
			*back_res = UINT32_MAX;
			*len_res = 1;
			return;
		}
	}

	*back_res = back_main + REPS;
	*len_res = len_main;
	mf_skip(mf, len_main - 2);
	return;
}

/*** End of inlined file: lzma_encoder_optimum_fast.c ***/


/*** Start of inlined file: lzma_encoder_optimum_normal.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       lzma_encoder_optimum_normal.c
//
//  Author:     Igor Pavlov
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "memcmplen.h"

////////////
// Prices //
////////////

static uint32_t
get_literal_price(const lzma_lzma1_encoder *const coder, const uint32_t pos,
		const uint32_t prev_byte, const bool match_mode,
		uint32_t match_byte, uint32_t symbol)
{
	const probability *const subcoder = literal_subcoder(coder->literal,
			coder->literal_context_bits, coder->literal_pos_mask,
			pos, prev_byte);

	uint32_t price = 0;

	if (!match_mode) {
		price = rc_bittree_price(subcoder, 8, symbol);
	} else {
		uint32_t offset = 0x100;
		symbol += UINT32_C(1) << 8;

		do {
			match_byte <<= 1;

			const uint32_t match_bit = match_byte & offset;
			const uint32_t subcoder_index
					= offset + match_bit + (symbol >> 8);
			const uint32_t bit = (symbol >> 7) & 1;
			price += rc_bit_price(subcoder[subcoder_index], bit);

			symbol <<= 1;
			offset &= ~(match_byte ^ symbol);

		} while (symbol < (UINT32_C(1) << 16));
	}

	return price;
}

static inline uint32_t
get_len_price(const lzma_length_encoder *const lencoder,
		const uint32_t len, const uint32_t pos_state)
{
	// NOTE: Unlike the other price tables, length prices are updated
	// in lzma_encoder.c
	return lencoder->prices[pos_state][len - MATCH_LEN_MIN];
}

static inline uint32_t
get_short_rep_price(const lzma_lzma1_encoder *const coder,
		const lzma_lzma_state state, const uint32_t pos_state)
{
	return rc_bit_0_price(coder->is_rep0[state])
		+ rc_bit_0_price(coder->is_rep0_long[state][pos_state]);
}

static inline uint32_t
get_pure_rep_price(const lzma_lzma1_encoder *const coder, const uint32_t rep_index,
		const lzma_lzma_state state, uint32_t pos_state)
{
	uint32_t price;

	if (rep_index == 0) {
		price = rc_bit_0_price(coder->is_rep0[state]);
		price += rc_bit_1_price(coder->is_rep0_long[state][pos_state]);
	} else {
		price = rc_bit_1_price(coder->is_rep0[state]);

		if (rep_index == 1) {
			price += rc_bit_0_price(coder->is_rep1[state]);
		} else {
			price += rc_bit_1_price(coder->is_rep1[state]);
			price += rc_bit_price(coder->is_rep2[state],
					rep_index - 2);
		}
	}

	return price;
}

static inline uint32_t
get_rep_price(const lzma_lzma1_encoder *const coder, const uint32_t rep_index,
		const uint32_t len, const lzma_lzma_state state,
		const uint32_t pos_state)
{
	return get_len_price(&coder->rep_len_encoder, len, pos_state)
		+ get_pure_rep_price(coder, rep_index, state, pos_state);
}

static inline uint32_t
get_dist_len_price(const lzma_lzma1_encoder *const coder, const uint32_t dist,
		const uint32_t len, const uint32_t pos_state)
{
	const uint32_t dist_state = get_dist_state(len);
	uint32_t price;

	if (dist < FULL_DISTANCES) {
		price = coder->dist_prices[dist_state][dist];
	} else {
		const uint32_t dist_slot = get_dist_slot_2(dist);
		price = coder->dist_slot_prices[dist_state][dist_slot]
				+ coder->align_prices[dist & ALIGN_MASK];
	}

	price += get_len_price(&coder->match_len_encoder, len, pos_state);

	return price;
}

static void
fill_dist_prices(lzma_lzma1_encoder *coder)
{
	for (uint32_t dist_state = 0; dist_state < DIST_STATES; ++dist_state) {

		uint32_t *const dist_slot_prices
				= coder->dist_slot_prices[dist_state];

		// Price to encode the dist_slot.
		for (uint32_t dist_slot = 0;
				dist_slot < coder->dist_table_size; ++dist_slot)
			dist_slot_prices[dist_slot] = rc_bittree_price(
					coder->dist_slot[dist_state],
					DIST_SLOT_BITS, dist_slot);

		// For matches with distance >= FULL_DISTANCES, add the price
		// of the direct bits part of the match distance. (Align bits
		// are handled by fill_align_prices()).
		for (uint32_t dist_slot = DIST_MODEL_END;
				dist_slot < coder->dist_table_size;
				++dist_slot)
			dist_slot_prices[dist_slot] += rc_direct_price(
					((dist_slot >> 1) - 1) - ALIGN_BITS);

		// Distances in the range [0, 3] are fully encoded with
		// dist_slot, so they are used for coder->dist_prices
		// as is.
		for (uint32_t i = 0; i < DIST_MODEL_START; ++i)
			coder->dist_prices[dist_state][i]
					= dist_slot_prices[i];
	}

	// Distances in the range [4, 127] depend on dist_slot and
	// dist_special. We do this in a loop separate from the above
	// loop to avoid redundant calls to get_dist_slot().
	for (uint32_t i = DIST_MODEL_START; i < FULL_DISTANCES; ++i) {
		const uint32_t dist_slot = get_dist_slot(i);
		const uint32_t footer_bits = ((dist_slot >> 1) - 1);
		const uint32_t base = (2 | (dist_slot & 1)) << footer_bits;
		const uint32_t price = rc_bittree_reverse_price(
				coder->dist_special + base - dist_slot - 1,
				footer_bits, i - base);

		for (uint32_t dist_state = 0; dist_state < DIST_STATES;
				++dist_state)
			coder->dist_prices[dist_state][i]
					= price + coder->dist_slot_prices[
						dist_state][dist_slot];
	}

	coder->match_price_count = 0;
	return;
}

static void
fill_align_prices(lzma_lzma1_encoder *coder)
{
	for (uint32_t i = 0; i < ALIGN_SIZE; ++i)
		coder->align_prices[i] = rc_bittree_reverse_price(
				coder->dist_align, ALIGN_BITS, i);

	coder->align_price_count = 0;
	return;
}

/////////////
// Optimal //
/////////////

static inline void
make_literal(lzma_optimal *optimal)
{
	optimal->back_prev = UINT32_MAX;
	optimal->prev_1_is_literal = false;
}

static inline void
make_short_rep(lzma_optimal *optimal)
{
	optimal->back_prev = 0;
	optimal->prev_1_is_literal = false;
}

#define is_short_rep(optimal) \
	((optimal).back_prev == 0)

static void
backward(lzma_lzma1_encoder *restrict coder, uint32_t *restrict len_res,
		uint32_t *restrict back_res, uint32_t cur)
{
	coder->opts_end_index = cur;

	uint32_t pos_mem = coder->opts[cur].pos_prev;
	uint32_t back_mem = coder->opts[cur].back_prev;

	do {
		if (coder->opts[cur].prev_1_is_literal) {
			make_literal(&coder->opts[pos_mem]);
			coder->opts[pos_mem].pos_prev = pos_mem - 1;

			if (coder->opts[cur].prev_2) {
				coder->opts[pos_mem - 1].prev_1_is_literal
						= false;
				coder->opts[pos_mem - 1].pos_prev
						= coder->opts[cur].pos_prev_2;
				coder->opts[pos_mem - 1].back_prev
						= coder->opts[cur].back_prev_2;
			}
		}

		const uint32_t pos_prev = pos_mem;
		const uint32_t back_cur = back_mem;

		back_mem = coder->opts[pos_prev].back_prev;
		pos_mem = coder->opts[pos_prev].pos_prev;

		coder->opts[pos_prev].back_prev = back_cur;
		coder->opts[pos_prev].pos_prev = cur;
		cur = pos_prev;

	} while (cur != 0);

	coder->opts_current_index = coder->opts[0].pos_prev;
	*len_res = coder->opts[0].pos_prev;
	*back_res = coder->opts[0].back_prev;

	return;
}

//////////
// Main //
//////////

static inline uint32_t
helper1(lzma_lzma1_encoder *restrict coder, lzma_mf *restrict mf,
		uint32_t *restrict back_res, uint32_t *restrict len_res,
		uint32_t position)
{
	const uint32_t nice_len = mf->nice_len;

	uint32_t len_main;
	uint32_t matches_count;

	if (mf->read_ahead == 0) {
		len_main = mf_find(mf, &matches_count, coder->matches);
	} else {
		assert(mf->read_ahead == 1);
		len_main = coder->longest_match_length;
		matches_count = coder->matches_count;
	}

	const uint32_t buf_avail = my_min(mf_avail(mf) + 1, MATCH_LEN_MAX);
	if (buf_avail < 2) {
		*back_res = UINT32_MAX;
		*len_res = 1;
		return UINT32_MAX;
	}

	const uint8_t *const buf = mf_ptr(mf) - 1;

	uint32_t rep_lens[REPS];
	uint32_t rep_max_index = 0;

	for (uint32_t i = 0; i < REPS; ++i) {
		const uint8_t *const buf_back = buf - coder->reps[i] - 1;

		if (not_equal_16(buf, buf_back)) {
			rep_lens[i] = 0;
			continue;
		}

		rep_lens[i] = lzma_memcmplen(buf, buf_back, 2, buf_avail);

		if (rep_lens[i] > rep_lens[rep_max_index])
			rep_max_index = i;
	}

	if (rep_lens[rep_max_index] >= nice_len) {
		*back_res = rep_max_index;
		*len_res = rep_lens[rep_max_index];
		mf_skip(mf, *len_res - 1);
		return UINT32_MAX;
	}

	if (len_main >= nice_len) {
		*back_res = coder->matches[matches_count - 1].dist + REPS;
		*len_res = len_main;
		mf_skip(mf, len_main - 1);
		return UINT32_MAX;
	}

	const uint8_t current_byte = *buf;
	const uint8_t match_byte = *(buf - coder->reps[0] - 1);

	if (len_main < 2 && current_byte != match_byte
			&& rep_lens[rep_max_index] < 2) {
		*back_res = UINT32_MAX;
		*len_res = 1;
		return UINT32_MAX;
	}

	coder->opts[0].state = coder->state;

	const uint32_t pos_state = position & coder->pos_mask;

	coder->opts[1].price = rc_bit_0_price(
				coder->is_match[coder->state][pos_state])
			+ get_literal_price(coder, position, buf[-1],
				!is_literal_state(coder->state),
				match_byte, current_byte);

	make_literal(&coder->opts[1]);

	const uint32_t match_price = rc_bit_1_price(
			coder->is_match[coder->state][pos_state]);
	const uint32_t rep_match_price = match_price
			+ rc_bit_1_price(coder->is_rep[coder->state]);

	if (match_byte == current_byte) {
		const uint32_t short_rep_price = rep_match_price
				+ get_short_rep_price(
					coder, coder->state, pos_state);

		if (short_rep_price < coder->opts[1].price) {
			coder->opts[1].price = short_rep_price;
			make_short_rep(&coder->opts[1]);
		}
	}

	const uint32_t len_end = my_max(len_main, rep_lens[rep_max_index]);

	if (len_end < 2) {
		*back_res = coder->opts[1].back_prev;
		*len_res = 1;
		return UINT32_MAX;
	}

	coder->opts[1].pos_prev = 0;

	for (uint32_t i = 0; i < REPS; ++i)
		coder->opts[0].backs[i] = coder->reps[i];

	uint32_t len = len_end;
	do {
		coder->opts[len].price = RC_INFINITY_PRICE;
	} while (--len >= 2);

	for (uint32_t i = 0; i < REPS; ++i) {
		uint32_t rep_len = rep_lens[i];
		if (rep_len < 2)
			continue;

		const uint32_t price = rep_match_price + get_pure_rep_price(
				coder, i, coder->state, pos_state);

		do {
			const uint32_t cur_and_len_price = price
					+ get_len_price(
						&coder->rep_len_encoder,
						rep_len, pos_state);

			if (cur_and_len_price < coder->opts[rep_len].price) {
				coder->opts[rep_len].price = cur_and_len_price;
				coder->opts[rep_len].pos_prev = 0;
				coder->opts[rep_len].back_prev = i;
				coder->opts[rep_len].prev_1_is_literal = false;
			}
		} while (--rep_len >= 2);
	}

	const uint32_t normal_match_price = match_price
			+ rc_bit_0_price(coder->is_rep[coder->state]);

	len = rep_lens[0] >= 2 ? rep_lens[0] + 1 : 2;
	if (len <= len_main) {
		uint32_t i = 0;
		while (len > coder->matches[i].len)
			++i;

		for(; ; ++len) {
			const uint32_t dist = coder->matches[i].dist;
			const uint32_t cur_and_len_price = normal_match_price
					+ get_dist_len_price(coder,
						dist, len, pos_state);

			if (cur_and_len_price < coder->opts[len].price) {
				coder->opts[len].price = cur_and_len_price;
				coder->opts[len].pos_prev = 0;
				coder->opts[len].back_prev = dist + REPS;
				coder->opts[len].prev_1_is_literal = false;
			}

			if (len == coder->matches[i].len)
				if (++i == matches_count)
					break;
		}
	}

	return len_end;
}

static inline uint32_t
helper2(lzma_lzma1_encoder *coder, uint32_t *reps, const uint8_t *buf,
		uint32_t len_end, uint32_t position, const uint32_t cur,
		const uint32_t nice_len, const uint32_t buf_avail_full)
{
	uint32_t matches_count = coder->matches_count;
	uint32_t new_len = coder->longest_match_length;
	uint32_t pos_prev = coder->opts[cur].pos_prev;
	lzma_lzma_state state;

	if (coder->opts[cur].prev_1_is_literal) {
		--pos_prev;

		if (coder->opts[cur].prev_2) {
			state = coder->opts[coder->opts[cur].pos_prev_2].state;

			if (coder->opts[cur].back_prev_2 < REPS)
				update_long_rep(state);
			else
				update_match(state);

		} else {
			state = coder->opts[pos_prev].state;
		}

		update_literal(state);

	} else {
		state = coder->opts[pos_prev].state;
	}

	if (pos_prev == cur - 1) {
		if (is_short_rep(coder->opts[cur]))
			update_short_rep(state);
		else
			update_literal(state);
	} else {
		uint32_t pos;
		if (coder->opts[cur].prev_1_is_literal
				&& coder->opts[cur].prev_2) {
			pos_prev = coder->opts[cur].pos_prev_2;
			pos = coder->opts[cur].back_prev_2;
			update_long_rep(state);
		} else {
			pos = coder->opts[cur].back_prev;
			if (pos < REPS)
				update_long_rep(state);
			else
				update_match(state);
		}

		if (pos < REPS) {
			reps[0] = coder->opts[pos_prev].backs[pos];

			uint32_t i;
			for (i = 1; i <= pos; ++i)
				reps[i] = coder->opts[pos_prev].backs[i - 1];

			for (; i < REPS; ++i)
				reps[i] = coder->opts[pos_prev].backs[i];

		} else {
			reps[0] = pos - REPS;

			for (uint32_t i = 1; i < REPS; ++i)
				reps[i] = coder->opts[pos_prev].backs[i - 1];
		}
	}

	coder->opts[cur].state = state;

	for (uint32_t i = 0; i < REPS; ++i)
		coder->opts[cur].backs[i] = reps[i];

	const uint32_t cur_price = coder->opts[cur].price;

	const uint8_t current_byte = *buf;
	const uint8_t match_byte = *(buf - reps[0] - 1);

	const uint32_t pos_state = position & coder->pos_mask;

	const uint32_t cur_and_1_price = cur_price
			+ rc_bit_0_price(coder->is_match[state][pos_state])
			+ get_literal_price(coder, position, buf[-1],
			!is_literal_state(state), match_byte, current_byte);

	bool next_is_literal = false;

	if (cur_and_1_price < coder->opts[cur + 1].price) {
		coder->opts[cur + 1].price = cur_and_1_price;
		coder->opts[cur + 1].pos_prev = cur;
		make_literal(&coder->opts[cur + 1]);
		next_is_literal = true;
	}

	const uint32_t match_price = cur_price
			+ rc_bit_1_price(coder->is_match[state][pos_state]);
	const uint32_t rep_match_price = match_price
			+ rc_bit_1_price(coder->is_rep[state]);

	if (match_byte == current_byte
			&& !(coder->opts[cur + 1].pos_prev < cur
				&& coder->opts[cur + 1].back_prev == 0)) {

		const uint32_t short_rep_price = rep_match_price
				+ get_short_rep_price(coder, state, pos_state);

		if (short_rep_price <= coder->opts[cur + 1].price) {
			coder->opts[cur + 1].price = short_rep_price;
			coder->opts[cur + 1].pos_prev = cur;
			make_short_rep(&coder->opts[cur + 1]);
			next_is_literal = true;
		}
	}

	if (buf_avail_full < 2)
		return len_end;

	const uint32_t buf_avail = my_min(buf_avail_full, nice_len);

	if (!next_is_literal && match_byte != current_byte) { // speed optimization
		// try literal + rep0
		const uint8_t *const buf_back = buf - reps[0] - 1;
		const uint32_t limit = my_min(buf_avail_full, nice_len + 1);

		const uint32_t len_test = lzma_memcmplen(buf, buf_back, 1, limit) - 1;

		if (len_test >= 2) {
			lzma_lzma_state state_2 = state;
			update_literal(state_2);

			const uint32_t pos_state_next = (position + 1) & coder->pos_mask;
			const uint32_t next_rep_match_price = cur_and_1_price
					+ rc_bit_1_price(coder->is_match[state_2][pos_state_next])
					+ rc_bit_1_price(coder->is_rep[state_2]);

			//for (; len_test >= 2; --len_test) {
			const uint32_t offset = cur + 1 + len_test;

			while (len_end < offset)
				coder->opts[++len_end].price = RC_INFINITY_PRICE;

			const uint32_t cur_and_len_price = next_rep_match_price
					+ get_rep_price(coder, 0, len_test,
						state_2, pos_state_next);

			if (cur_and_len_price < coder->opts[offset].price) {
				coder->opts[offset].price = cur_and_len_price;
				coder->opts[offset].pos_prev = cur + 1;
				coder->opts[offset].back_prev = 0;
				coder->opts[offset].prev_1_is_literal = true;
				coder->opts[offset].prev_2 = false;
			}
			//}
		}
	}

	uint32_t start_len = 2; // speed optimization

	for (uint32_t rep_index = 0; rep_index < REPS; ++rep_index) {
		const uint8_t *const buf_back = buf - reps[rep_index] - 1;
		if (not_equal_16(buf, buf_back))
			continue;

		uint32_t len_test = lzma_memcmplen(buf, buf_back, 2, buf_avail);

		while (len_end < cur + len_test)
			coder->opts[++len_end].price = RC_INFINITY_PRICE;

		const uint32_t len_test_temp = len_test;
		const uint32_t price = rep_match_price + get_pure_rep_price(
				coder, rep_index, state, pos_state);

		do {
			const uint32_t cur_and_len_price = price
					+ get_len_price(&coder->rep_len_encoder,
							len_test, pos_state);

			if (cur_and_len_price < coder->opts[cur + len_test].price) {
				coder->opts[cur + len_test].price = cur_and_len_price;
				coder->opts[cur + len_test].pos_prev = cur;
				coder->opts[cur + len_test].back_prev = rep_index;
				coder->opts[cur + len_test].prev_1_is_literal = false;
			}
		} while (--len_test >= 2);

		len_test = len_test_temp;

		if (rep_index == 0)
			start_len = len_test + 1;

		uint32_t len_test_2 = len_test + 1;
		const uint32_t limit = my_min(buf_avail_full,
				len_test_2 + nice_len);
		// NOTE: len_test_2 may be greater than limit so the call to
		// lzma_memcmplen() must be done conditionally.
		if (len_test_2 < limit)
			len_test_2 = lzma_memcmplen(buf, buf_back, len_test_2, limit);

		len_test_2 -= len_test + 1;

		if (len_test_2 >= 2) {
			lzma_lzma_state state_2 = state;
			update_long_rep(state_2);

			uint32_t pos_state_next = (position + len_test) & coder->pos_mask;

			const uint32_t cur_and_len_literal_price = price
					+ get_len_price(&coder->rep_len_encoder,
						len_test, pos_state)
					+ rc_bit_0_price(coder->is_match[state_2][pos_state_next])
					+ get_literal_price(coder, position + len_test,
						buf[len_test - 1], true,
						buf_back[len_test], buf[len_test]);

			update_literal(state_2);

			pos_state_next = (position + len_test + 1) & coder->pos_mask;

			const uint32_t next_rep_match_price = cur_and_len_literal_price
					+ rc_bit_1_price(coder->is_match[state_2][pos_state_next])
					+ rc_bit_1_price(coder->is_rep[state_2]);

			//for(; len_test_2 >= 2; len_test_2--) {
			const uint32_t offset = cur + len_test + 1 + len_test_2;

			while (len_end < offset)
				coder->opts[++len_end].price = RC_INFINITY_PRICE;

			const uint32_t cur_and_len_price = next_rep_match_price
					+ get_rep_price(coder, 0, len_test_2,
						state_2, pos_state_next);

			if (cur_and_len_price < coder->opts[offset].price) {
				coder->opts[offset].price = cur_and_len_price;
				coder->opts[offset].pos_prev = cur + len_test + 1;
				coder->opts[offset].back_prev = 0;
				coder->opts[offset].prev_1_is_literal = true;
				coder->opts[offset].prev_2 = true;
				coder->opts[offset].pos_prev_2 = cur;
				coder->opts[offset].back_prev_2 = rep_index;
			}
			//}
		}
	}

	//for (uint32_t len_test = 2; len_test <= new_len; ++len_test)
	if (new_len > buf_avail) {
		new_len = buf_avail;

		matches_count = 0;
		while (new_len > coder->matches[matches_count].len)
			++matches_count;

		coder->matches[matches_count++].len = new_len;
	}

	if (new_len >= start_len) {
		const uint32_t normal_match_price = match_price
				+ rc_bit_0_price(coder->is_rep[state]);

		while (len_end < cur + new_len)
			coder->opts[++len_end].price = RC_INFINITY_PRICE;

		uint32_t i = 0;
		while (start_len > coder->matches[i].len)
			++i;

		for (uint32_t len_test = start_len; ; ++len_test) {
			const uint32_t cur_back = coder->matches[i].dist;
			uint32_t cur_and_len_price = normal_match_price
					+ get_dist_len_price(coder,
						cur_back, len_test, pos_state);

			if (cur_and_len_price < coder->opts[cur + len_test].price) {
				coder->opts[cur + len_test].price = cur_and_len_price;
				coder->opts[cur + len_test].pos_prev = cur;
				coder->opts[cur + len_test].back_prev
						= cur_back + REPS;
				coder->opts[cur + len_test].prev_1_is_literal = false;
			}

			if (len_test == coder->matches[i].len) {
				// Try Match + Literal + Rep0
				const uint8_t *const buf_back = buf - cur_back - 1;
				uint32_t len_test_2 = len_test + 1;
				const uint32_t limit = my_min(buf_avail_full,
						len_test_2 + nice_len);

				// NOTE: len_test_2 may be greater than limit
				// so the call to lzma_memcmplen() must be
				// done conditionally.
				if (len_test_2 < limit)
					len_test_2 = lzma_memcmplen(buf, buf_back,
							len_test_2, limit);

				len_test_2 -= len_test + 1;

				if (len_test_2 >= 2) {
					lzma_lzma_state state_2 = state;
					update_match(state_2);
					uint32_t pos_state_next
							= (position + len_test) & coder->pos_mask;

					const uint32_t cur_and_len_literal_price = cur_and_len_price
							+ rc_bit_0_price(
								coder->is_match[state_2][pos_state_next])
							+ get_literal_price(coder,
								position + len_test,
								buf[len_test - 1],
								true,
								buf_back[len_test],
								buf[len_test]);

					update_literal(state_2);
					pos_state_next = (pos_state_next + 1) & coder->pos_mask;

					const uint32_t next_rep_match_price
							= cur_and_len_literal_price
							+ rc_bit_1_price(
								coder->is_match[state_2][pos_state_next])
							+ rc_bit_1_price(coder->is_rep[state_2]);

					// for(; len_test_2 >= 2; --len_test_2) {
					const uint32_t offset = cur + len_test + 1 + len_test_2;

					while (len_end < offset)
						coder->opts[++len_end].price = RC_INFINITY_PRICE;

					cur_and_len_price = next_rep_match_price
							+ get_rep_price(coder, 0, len_test_2,
								state_2, pos_state_next);

					if (cur_and_len_price < coder->opts[offset].price) {
						coder->opts[offset].price = cur_and_len_price;
						coder->opts[offset].pos_prev = cur + len_test + 1;
						coder->opts[offset].back_prev = 0;
						coder->opts[offset].prev_1_is_literal = true;
						coder->opts[offset].prev_2 = true;
						coder->opts[offset].pos_prev_2 = cur;
						coder->opts[offset].back_prev_2
								= cur_back + REPS;
					}
					//}
				}

				if (++i == matches_count)
					break;
			}
		}
	}

	return len_end;
}

extern void
lzma_lzma_optimum_normal(lzma_lzma1_encoder *restrict coder,
		lzma_mf *restrict mf,
		uint32_t *restrict back_res, uint32_t *restrict len_res,
		uint32_t position)
{
	// If we have symbols pending, return the next pending symbol.
	if (coder->opts_end_index != coder->opts_current_index) {
		assert(mf->read_ahead > 0);
		*len_res = coder->opts[coder->opts_current_index].pos_prev
				- coder->opts_current_index;
		*back_res = coder->opts[coder->opts_current_index].back_prev;
		coder->opts_current_index = coder->opts[
				coder->opts_current_index].pos_prev;
		return;
	}

	// Update the price tables. In LZMA SDK <= 4.60 (and possibly later)
	// this was done in both initialization function and in the main loop.
	// In liblzma they were moved into this single place.
	if (mf->read_ahead == 0) {
		if (coder->match_price_count >= (1 << 7))
			fill_dist_prices(coder);

		if (coder->align_price_count >= ALIGN_SIZE)
			fill_align_prices(coder);
	}

	// TODO: This needs quite a bit of cleaning still. But splitting
	// the original function into two pieces makes it at least a little
	// more readable, since those two parts don't share many variables.

	uint32_t len_end = helper1(coder, mf, back_res, len_res, position);
	if (len_end == UINT32_MAX)
		return;

	uint32_t reps[REPS];
	memcpy(reps, coder->reps, sizeof(reps));

	uint32_t cur;
	for (cur = 1; cur < len_end; ++cur) {
		assert(cur < OPTS);

		coder->longest_match_length = mf_find(
				mf, &coder->matches_count, coder->matches);

		if (coder->longest_match_length >= mf->nice_len)
			break;

		len_end = helper2(coder, reps, mf_ptr(mf) - 1, len_end,
				position + cur, cur, mf->nice_len,
				my_min(mf_avail(mf) + 1, OPTS - 1 - cur));
	}

	backward(coder, len_res, back_res, cur);
	return;
}

/*** End of inlined file: lzma_encoder_optimum_normal.c ***/


/*** Start of inlined file: lzma_encoder_presets.c ***/
///////////////////////////////////////////////////////////////////////////////
//
/// \file       lzma_encoder_presets.c
/// \brief      Encoder presets
/// \note       xz needs this even when only decoding is enabled.
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "common.h"

extern LZMA_API(lzma_bool)
lzma_lzma_preset(lzma_options_lzma *options, uint32_t preset)
{
	const uint32_t level = preset & LZMA_PRESET_LEVEL_MASK;
	const uint32_t flags = preset & ~LZMA_PRESET_LEVEL_MASK;
	const uint32_t supported_flags = LZMA_PRESET_EXTREME;

	if (level > 9 || (flags & ~supported_flags))
		return true;

	options->preset_dict = NULL;
	options->preset_dict_size = 0;

	options->lc = LZMA_LC_DEFAULT;
	options->lp = LZMA_LP_DEFAULT;
	options->pb = LZMA_PB_DEFAULT;

	static const uint8_t dict_pow2[]
			= { 18, 20, 21, 22, 22, 23, 23, 24, 25, 26 };
	options->dict_size = UINT32_C(1) << dict_pow2[level];

	if (level <= 3) {
		options->mode = LZMA_MODE_FAST;
		options->mf = level == 0 ? LZMA_MF_HC3 : LZMA_MF_HC4;
		options->nice_len = level <= 1 ? 128 : 273;
		static const uint8_t depths[] = { 4, 8, 24, 48 };
		options->depth = depths[level];
	} else {
		options->mode = LZMA_MODE_NORMAL;
		options->mf = LZMA_MF_BT4;
		options->nice_len = level == 4 ? 16 : level == 5 ? 32 : 64;
		options->depth = 0;
	}

	if (flags & LZMA_PRESET_EXTREME) {
		options->mode = LZMA_MODE_NORMAL;
		options->mf = LZMA_MF_BT4;
		if (level == 3 || level == 5) {
			options->nice_len = 192;
			options->depth = 0;
		} else {
			options->nice_len = 273;
			options->depth = 512;
		}
	}

	return false;
}

/*** End of inlined file: lzma_encoder_presets.c ***/

/*
 * total 212
drwxr-xr-x 3 robin robin   4096 des.  19 11:04 api
drwxr-xr-x 2 robin robin   4096 des.  19 11:04 check
drwxr-xr-x 2 robin robin   4096 des.  19 11:04 common
drwxr-xr-x 2 robin robin   4096 des.  19 11:04 delta
drwxr-xr-x 2 robin robin   4096 des.  19 11:04 lz
drwxr-xr-x 2 robin robin   4096 des.  19 11:04 lzma
drwxr-xr-x 2 robin robin   4096 des.  19 11:04 rangecoder
drwxr-xr-x 2 robin robin   4096 des.  19 11:04 simple
-rw-r--r-- 1 robin robin   3375 mars  17  2020 Makefile.am
-rw-r--r-- 1 robin robin    410 mars  17  2020 liblzma.pc.in
-rw-r--r-- 1 robin robin 160279 mars  17  2020 Makefile.in
-rw-r--r-- 1 robin robin   2338 mars  17  2020 liblzma.map
-rw-r--r-- 1 robin robin    287 mars  17  2020 liblzma_w32res.rc
-rw-r--r-- 1 robin robin   1509 mars  17  2020 validate_map.sh

./api:
total 40
drwxr-xr-x 2 robin robin  4096 des.  19 11:04 lzma
-rw-r--r-- 1 robin robin   409 mars  17  2020 Makefile.am
-rw-r--r-- 1 robin robin  9866 mars  17  2020 lzma.h
-rw-r--r-- 1 robin robin 18657 mars  17  2020 Makefile.in

./api/lzma:
total 188
-rw-r--r-- 1 robin robin 24858 mars  17  2020 base.h
-rw-r--r-- 1 robin robin  2630 mars  17  2020 bcj.h
-rw-r--r-- 1 robin robin 22107 mars  17  2020 block.h
-rw-r--r-- 1 robin robin  4255 mars  17  2020 check.h
-rw-r--r-- 1 robin robin 24844 mars  17  2020 container.h
-rw-r--r-- 1 robin robin  1865 mars  17  2020 delta.h
-rw-r--r-- 1 robin robin 16520 mars  17  2020 filter.h
-rw-r--r-- 1 robin robin  2604 mars  17  2020 hardware.h
-rw-r--r-- 1 robin robin 23491 mars  17  2020 index.h
-rw-r--r-- 1 robin robin  3914 mars  17  2020 index_hash.h
-rw-r--r-- 1 robin robin 14744 mars  17  2020 lzma12.h
-rw-r--r-- 1 robin robin  8253 mars  17  2020 stream_flags.h
-rw-r--r-- 1 robin robin  3497 mars  17  2020 version.h
-rw-r--r-- 1 robin robin  6546 mars  17  2020 vli.h

./check:
total 196
-rw-r--r-- 1 robin robin  2910 mars  17  2020 check.c
-rw-r--r-- 1 robin robin  2277 mars  17  2020 crc32_fast.c
-rw-r--r-- 1 robin robin  1044 mars  17  2020 crc32_small.c
-rw-r--r-- 1 robin robin   641 mars  17  2020 crc32_table.c
-rw-r--r-- 1 robin robin  2245 mars  17  2020 crc32_tablegen.c
-rw-r--r-- 1 robin robin  1640 mars  17  2020 crc64_fast.c
-rw-r--r-- 1 robin robin   966 mars  17  2020 crc64_small.c
-rw-r--r-- 1 robin robin   641 mars  17  2020 crc64_table.c
-rw-r--r-- 1 robin robin  1744 mars  17  2020 crc64_tablegen.c
-rw-r--r-- 1 robin robin  5405 mars  17  2020 sha256.c
-rw-r--r-- 1 robin robin  4718 mars  17  2020 check.h
-rw-r--r-- 1 robin robin 25758 mars  17  2020 crc32_table_be.h
-rw-r--r-- 1 robin robin 25758 mars  17  2020 crc32_table_le.h
-rw-r--r-- 1 robin robin 31882 mars  17  2020 crc64_table_be.h
-rw-r--r-- 1 robin robin 31882 mars  17  2020 crc64_table_le.h
-rw-r--r-- 1 robin robin   787 mars  17  2020 crc_macros.h
-rw-r--r-- 1 robin robin   970 mars  17  2020 Makefile.inc
-rw-r--r-- 1 robin robin  7228 mars  17  2020 crc32_x86.S
-rw-r--r-- 1 robin robin  6761 mars  17  2020 crc64_x86.S

./common:
total 396
-rw-r--r-- 1 robin robin  5662 mars  17  2020 alone_decoder.c
-rw-r--r-- 1 robin robin  3751 mars  17  2020 alone_encoder.c
-rw-r--r-- 1 robin robin  4902 mars  17  2020 auto_decoder.c
-rw-r--r-- 1 robin robin  2368 mars  17  2020 block_buffer_decoder.c
-rw-r--r-- 1 robin robin 10123 mars  17  2020 block_buffer_encoder.c
-rw-r--r-- 1 robin robin  6988 mars  17  2020 block_decoder.c
-rw-r--r-- 1 robin robin  5766 mars  17  2020 block_encoder.c
-rw-r--r-- 1 robin robin  3710 mars  17  2020 block_header_decoder.c
-rw-r--r-- 1 robin robin  3346 mars  17  2020 block_header_encoder.c
-rw-r--r-- 1 robin robin  2640 mars  17  2020 block_util.c
-rw-r--r-- 1 robin robin 10652 mars  17  2020 common.c
-rw-r--r-- 1 robin robin   840 mars  17  2020 easy_buffer_encoder.c
-rw-r--r-- 1 robin robin   668 mars  17  2020 easy_decoder_memusage.c
-rw-r--r-- 1 robin robin   678 mars  17  2020 easy_encoder.c
-rw-r--r-- 1 robin robin   654 mars  17  2020 easy_encoder_memusage.c
-rw-r--r-- 1 robin robin   727 mars  17  2020 easy_preset.c
-rw-r--r-- 1 robin robin  2510 mars  17  2020 filter_buffer_decoder.c
-rw-r--r-- 1 robin robin  1537 mars  17  2020 filter_buffer_encoder.c
-rw-r--r-- 1 robin robin  8706 mars  17  2020 filter_common.c
-rw-r--r-- 1 robin robin  4424 mars  17  2020 filter_decoder.c
-rw-r--r-- 1 robin robin  7345 mars  17  2020 filter_encoder.c
-rw-r--r-- 1 robin robin  1187 mars  17  2020 filter_flags_decoder.c
-rw-r--r-- 1 robin robin  1407 mars  17  2020 filter_flags_encoder.c
-rw-r--r-- 1 robin robin   526 mars  17  2020 hardware_cputhreads.c
-rw-r--r-- 1 robin robin   683 mars  17  2020 hardware_physmem.c
-rw-r--r-- 1 robin robin 35440 mars  17  2020 index.c
-rw-r--r-- 1 robin robin  8605 mars  17  2020 index_decoder.c
-rw-r--r-- 1 robin robin  5853 mars  17  2020 index_encoder.c
-rw-r--r-- 1 robin robin  8981 mars  17  2020 index_hash.c
-rw-r--r-- 1 robin robin  4586 mars  17  2020 outqueue.c
-rw-r--r-- 1 robin robin  2785 mars  17  2020 stream_buffer_decoder.c
-rw-r--r-- 1 robin robin  4056 mars  17  2020 stream_buffer_encoder.c
-rw-r--r-- 1 robin robin 13001 mars  17  2020 stream_decoder.c
-rw-r--r-- 1 robin robin  9856 mars  17  2020 stream_encoder.c
-rw-r--r-- 1 robin robin 30356 mars  17  2020 stream_encoder_mt.c
-rw-r--r-- 1 robin robin  1326 mars  17  2020 stream_flags_common.c
-rw-r--r-- 1 robin robin  2279 mars  17  2020 stream_flags_decoder.c
-rw-r--r-- 1 robin robin  2059 mars  17  2020 stream_flags_encoder.c
-rw-r--r-- 1 robin robin  2559 mars  17  2020 vli_decoder.c
-rw-r--r-- 1 robin robin  1935 mars  17  2020 vli_encoder.c
-rw-r--r-- 1 robin robin   630 mars  17  2020 vli_size.c
-rw-r--r-- 1 robin robin   601 mars  17  2020 alone_decoder.h
-rw-r--r-- 1 robin robin   763 mars  17  2020 block_buffer_encoder.h
-rw-r--r-- 1 robin robin   576 mars  17  2020 block_decoder.h
-rw-r--r-- 1 robin robin  1890 mars  17  2020 block_encoder.h
-rw-r--r-- 1 robin robin 10467 mars  17  2020 common.h
-rw-r--r-- 1 robin robin   921 mars  17  2020 easy_preset.h
-rw-r--r-- 1 robin robin  1258 mars  17  2020 filter_common.h
-rw-r--r-- 1 robin robin   617 mars  17  2020 filter_decoder.h
-rw-r--r-- 1 robin robin   732 mars  17  2020 filter_encoder.h
-rw-r--r-- 1 robin robin   584 mars  17  2020 index_encoder.h
-rw-r--r-- 1 robin robin  1978 mars  17  2020 index.h
-rw-r--r-- 1 robin robin  4819 mars  17  2020 memcmplen.h
-rw-r--r-- 1 robin robin  4998 mars  17  2020 outqueue.h
-rw-r--r-- 1 robin robin   599 mars  17  2020 stream_decoder.h
-rw-r--r-- 1 robin robin   883 mars  17  2020 stream_flags_common.h
-rw-r--r-- 1 robin robin  1881 mars  17  2020 Makefile.inc

./delta:
total 32
-rw-r--r-- 1 robin robin 1914 mars  17  2020 delta_common.c
-rw-r--r-- 1 robin robin 1910 mars  17  2020 delta_decoder.c
-rw-r--r-- 1 robin robin 3435 mars  17  2020 delta_encoder.c
-rw-r--r-- 1 robin robin  542 mars  17  2020 delta_common.h
-rw-r--r-- 1 robin robin  736 mars  17  2020 delta_decoder.h
-rw-r--r-- 1 robin robin  676 mars  17  2020 delta_encoder.h
-rw-r--r-- 1 robin robin  889 mars  17  2020 delta_private.h
-rw-r--r-- 1 robin robin  429 mars  17  2020 Makefile.inc

./lz:
total 84
-rw-r--r-- 1 robin robin  9130 mars  17  2020 lz_decoder.c
-rw-r--r-- 1 robin robin 17392 mars  17  2020 lz_encoder.c
-rw-r--r-- 1 robin robin 17379 mars  17  2020 lz_encoder_mf.c
-rw-r--r-- 1 robin robin  5841 mars  17  2020 lz_decoder.h
-rw-r--r-- 1 robin robin 10458 mars  17  2020 lz_encoder.h
-rw-r--r-- 1 robin robin  3539 mars  17  2020 lz_encoder_hash.h
-rw-r--r-- 1 robin robin  3252 mars  17  2020 lz_encoder_hash_table.h
-rw-r--r-- 1 robin robin   380 mars  17  2020 Makefile.inc

./lzma:
total 180
-rw-r--r-- 1 robin robin 33959 mars  17  2020 fastpos_table.c
-rw-r--r-- 1 robin robin  1268 mars  17  2020 fastpos_tablegen.c
-rw-r--r-- 1 robin robin  7576 mars  17  2020 lzma2_decoder.c
-rw-r--r-- 1 robin robin 10500 mars  17  2020 lzma2_encoder.c
-rw-r--r-- 1 robin robin 27321 mars  17  2020 lzma_decoder.c
-rw-r--r-- 1 robin robin 18518 mars  17  2020 lzma_encoder.c
-rw-r--r-- 1 robin robin  4402 mars  17  2020 lzma_encoder_optimum_fast.c
-rw-r--r-- 1 robin robin 23286 mars  17  2020 lzma_encoder_optimum_normal.c
-rw-r--r-- 1 robin robin  1763 mars  17  2020 lzma_encoder_presets.c
-rw-r--r-- 1 robin robin  3996 mars  17  2020 fastpos.h
-rw-r--r-- 1 robin robin   819 mars  17  2020 lzma2_decoder.h
-rw-r--r-- 1 robin robin  1186 mars  17  2020 lzma2_encoder.h
-rw-r--r-- 1 robin robin  6947 mars  17  2020 lzma_common.h
-rw-r--r-- 1 robin robin  1554 mars  17  2020 lzma_decoder.h
-rw-r--r-- 1 robin robin  1606 mars  17  2020 lzma_encoder.h
-rw-r--r-- 1 robin robin  3911 mars  17  2020 lzma_encoder_private.h
-rw-r--r-- 1 robin robin   873 mars  17  2020 Makefile.inc

./rangecoder:
total 36
-rw-r--r-- 1 robin robin  827 mars  17  2020 price_table.c
-rw-r--r-- 1 robin robin 1768 mars  17  2020 price_tablegen.c
-rw-r--r-- 1 robin robin 2013 mars  17  2020 price.h
-rw-r--r-- 1 robin robin 2317 mars  17  2020 range_common.h
-rw-r--r-- 1 robin robin 5009 mars  17  2020 range_decoder.h
-rw-r--r-- 1 robin robin 4616 mars  17  2020 range_encoder.h
-rw-r--r-- 1 robin robin  432 mars  17  2020 Makefile.inc

./simple:
total 64
-rw-r--r-- 1 robin robin 1686 mars  17  2020 arm.c
-rw-r--r-- 1 robin robin 1906 mars  17  2020 armthumb.c
-rw-r--r-- 1 robin robin 2740 mars  17  2020 ia64.c
-rw-r--r-- 1 robin robin 1936 mars  17  2020 powerpc.c
-rw-r--r-- 1 robin robin 8705 mars  17  2020 simple_coder.c
-rw-r--r-- 1 robin robin  982 mars  17  2020 simple_decoder.c
-rw-r--r-- 1 robin robin  983 mars  17  2020 simple_encoder.c
-rw-r--r-- 1 robin robin 2006 mars  17  2020 sparc.c
-rw-r--r-- 1 robin robin 3575 mars  17  2020 x86.c
-rw-r--r-- 1 robin robin 2171 mars  17  2020 simple_coder.h
-rw-r--r-- 1 robin robin  622 mars  17  2020 simple_decoder.h
-rw-r--r-- 1 robin robin  641 mars  17  2020 simple_encoder.h
-rw-r--r-- 1 robin robin 2211 mars  17  2020 simple_private.h
-rw-r--r-- 1 robin robin  833 mars  17  2020 Makefile.inc

 */
	}
}
#endif //POCKETLZMA_LZMA_C_H

