#include <iostream>
#include "COM.h"
namespace std {
	typedef struct __FILE_Driver
	{
		unsigned long Handle;
		
		
		
		int (*feof)(FILE *stream);
		int (*ferror)(FILE *stream);
		int (*fflush)(FILE *stream);
		
		int (*fseek)(FILE *stream, long int offset, int whence);
		long int (*ftell)(FILE *stream);
		size_t (*fwrite)(const void *ptr, size_t size, size_t nmemb, FILE *stream);
		size_t (*fread)(void *ptr, size_t size, size_t nmemb, FILE *stream);
		int (*fgetc)(FILE *stream);
		int (*fputc)(int ch, FILE *stream);
		
		int (*fclose)(FILE *stream);
	}FileDirver, *pFileDirver;
	struct __FILE
	{
		int handle;
		//COM_Number com;
		FileDirver Dirver;
	};
	
	
	
	int COM_feof(FILE *stream)
	{
		return 0;
	}
	int COM_ferror(FILE *stream)
	{
		return stream->Dirver.Handle < COM_TOTAL;
	}
	int COM_fflush(FILE *stream)
	{
		return 0;
	}
	
	int COM_fseek(FILE *stream, long int offset, int whence)
	{
		return 0;
	}
	long int COM_ftell(FILE *stream)
	{
		return 0;
	}
	size_t COM_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
	{
		nmemb *= size;
		COMS[stream->Dirver.Handle]->Transmits((unsigned char *)ptr, nmemb);
		return nmemb;
	}
	size_t COM_fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
	{
		size *= nmemb;
		while(COMS[stream->Dirver.Handle]->GetRecivedDataSize() < size);
		COMS[stream->Dirver.Handle]->GetRecivedDatas((unsigned char*)ptr, size);
		return size;
	}
	int COM_fgetc(FILE *stream)
	{
		unsigned char ch;
		while(COMS[stream->Dirver.Handle]->GetRecivedDataSize() == 0);
		ch = COMS[stream->Dirver.Handle]->GetRecivedData();
		if(stdin == stream)
			fputc(ch,stdout);
		return ch;
	}
	int COM_fputc(int ch, FILE *stream)
	{
		COMS[stream->Dirver.Handle]->Transmit((unsigned char)ch);
		return ch;
	}
	
	int COM_fclose(FILE *stream)
	{
		COMS[stream->Dirver.Handle]->SetEnable(0);
		stream->Dirver.Handle = COM_TOTAL;
		return 0;
	}
	
	int feof(FILE *stream){return stream->Dirver.feof(stream);}
	int ferror(FILE *stream){return stream->Dirver.ferror(stream);}
	int fflush(FILE *stream){return stream->Dirver.fflush(stream);}
	
	int fseek(FILE *stream, long int offset, int whence){return stream->Dirver.fseek(stream, offset, whence);}
	long int ftell(FILE *stream){return stream->Dirver.ftell(stream);}
	size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream){return stream->Dirver.fwrite(ptr,size, nmemb, stream);}
	size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream){return stream->Dirver.fread(ptr, size, nmemb, stream);}
	int fgetc(FILE *stream){return stream->Dirver.fgetc(stream);}
	int fputc(int ch, FILE *stream){return stream->Dirver.fputc(ch, stream);}
	
	int fclose(FILE *stream){int i = stream->Dirver.fclose(stream);free(stream);return i;}
	FILE *fopen(const char *filename, const char *mode)
	{
		FILE* f = (FILE*)malloc(sizeof(FILE));
		
		if(('C' == filename[0]) && ('O' == filename[1]) && ('M' == filename[2]) && ('1' <= filename[3]) && (filename[3] <= '1' + COM_TOTAL) && (0 == filename[4]))
		{
			f->Dirver.Handle = filename[3] - '1';
			COMS[f->Dirver.Handle]->Initilize();
			
			f->Dirver.feof = COM_feof;
			f->Dirver.ferror = COM_ferror;
			f->Dirver.fflush = COM_fflush;
			
			f->Dirver.fseek = COM_fseek;
			f->Dirver.ftell = COM_ftell;
			f->Dirver.fwrite = COM_fwrite;
			f->Dirver.fread = COM_fread;
			f->Dirver.fgetc = COM_fgetc;
			f->Dirver.fputc = COM_fputc;
			f->Dirver.fclose = COM_fclose;
		}
		
		return f;
	}
	FILE __stdout = {1, {COM4, COM_feof,COM_ferror,COM_fflush,COM_fseek,COM_ftell,COM_fwrite,COM_fread,COM_fgetc,COM_fputc,COM_fclose}};
	FILE __stdin = {0, {COM4, COM_feof,COM_ferror,COM_fflush,COM_fseek,COM_ftell,COM_fwrite,COM_fread,COM_fgetc,COM_fputc,COM_fclose}};
	FILE __stderr = {2, {COM4, COM_feof,COM_ferror,COM_fflush,COM_fseek,COM_ftell,COM_fwrite,COM_fread,COM_fgetc,COM_fputc,COM_fclose}};
	#ifdef __MICROLIB /*__CC_ARM*/
	/// To suppress MDK-ARM Linker Error: L6218E: Undefined symbol __fread_bytes_avail (referred from ios.o).
	size_t __fread_bytes_avail(void * __restrict ptr,
					size_t count, FILE * __restrict stream) //__attribute__((__nonnull__(1,3)))
	{
		if(count > ftell(stream))
			count = ftell(stream);
		if(!count)
			count = 1;
		return (size_t)fread(ptr, count, 1, stream);
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
		for(size_t i=0; i<n; i++)
		{
			s1[i] = s2[i];
		}
		return NULL;
	}
#endif /*__MICROLIB*/
}
