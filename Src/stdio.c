#include <stdio.h>
//#include <string.h>
#include "COM.h"

#define STDIO_COM	COM1
struct __FILE
{
	int Handle;
};
int fputc(int ch,FILE *f)
{
	FILE * volatile F = f;
	if(F == stdout)
		COMS[STDIO_COM]->Transmit((unsigned char)ch);
	if(F == stderr)
		COMS[STDIO_COM]->Transmit((unsigned char)ch);
	return ch;
}
int fgetc(FILE *f)
{
	unsigned char ch;
	while(COMS[STDIO_COM]->GetRecivedDataSize() == 0);
	ch = COMS[STDIO_COM]->GetRecivedData();
	if(stdin == f)
		fputc(ch,stdout);
	return ch;
}
#ifdef __MICROLIB /*__CC_ARM*/
/// To suppress MDK-ARM Linker Error: L6218E: Undefined symbol __fread_bytes_avail (referred from ios.o).
size_t __fread_bytes_avail(void * __restrict ptr,
                    size_t count, FILE * __restrict stream) //__attribute__((__nonnull__(1,3)))
{
        return (size_t)-1;
}


#include <wchar.h>
/// To suppress MDK-ARM Linker Error: L6218E: Undefined symbol mbsinit (referred from ios.o).
int mbsinit(const mbstate_t * ps)
{
    return -1;
}

/// To suppress MDK-ARM Linker Error: L6218E: Undefined symbol mbsinit (referred from ios.o).
wchar_t *wmemmove(wchar_t * __restrict s1,
                      const wchar_t * __restrict s2, size_t n) //__attribute__((__nonnull__(1,2)))
{
        return NULL;
}
#endif /*__MICROLIB*/
