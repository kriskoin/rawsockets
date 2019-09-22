#ifndef __TYPESDEF_H
#define __TYPESDEF_H
// Some useful types that are bit-size specific.
// These definitions are for the i386 class 32-bit CPU.  Other definitions
// will need to be made for other CPUs.
typedef int			INT32;	// 32 bits, signed
typedef short			INT16;	// 16 bits, signed
typedef signed   char     INT8;	// 8 bits,  signed
typedef unsigned int	WORD32;	// 32 bits, unsigned
typedef unsigned short  WORD16;	// 16 bits, unsigned
typedef unsigned char   BYTE8;	// 8 bits,  unsigned
typedef unsigned char   BOOL8;	// 8 bits,  unsigned
typedef unsigned char   byte;	// 8 bits,  unsigned
typedef int		BOOL;
// These are the old type definitions... use the new ones (above) whenever possible.
typedef unsigned short  word;	// 16 bits, unsigned
typedef unsigned long   dword;	// 32 bits, unsigned
typedef signed   char   schar;	// 8 bits, signed
// ***** End of old type definitions *****



#endif