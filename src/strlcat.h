#ifndef HAS_OBSD_STRLCAT
#define HAS_OBSD_STRLCAT

#include <sys/cdefs.h>
#include <sys/types.h>

#include <string.h>

#ifdef 	__linux__ 
#ifndef __BSD_VISIBLE
extern 	size_t	 strlcat(char *, const char *, size_t)
		__attribute__ ((__bounded__(__string__,1,3)));
#endif /* __BSD_VISIBLE */
#endif /* __linux__ */

#endif /* HAS_OBSD_STRLCAT */


