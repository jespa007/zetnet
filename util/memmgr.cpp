#include	<stdlib.h>
#include	<stdio.h>
#include	<memory.h>
#include	<string.h>
#include	<memory.h>
#include	<stdarg.h>

#ifdef _WIN32
#include	<windows.h>
#endif

#include	"memmgr.h"


//--------------------------------------------------------------------------------------------
// DEFINES
#define	MAX_MEMPOINTERS			80000
#define	MAX_FILENAME_LENGTH		256
#define MAX_REGISTER_FILELINES	32

#define	GET_SIZE_PTR(p)		*((int  *)((char  *)p  -  sizeof(int) - sizeof(int)))
#define GET_PREHEADER(p)	((PointerPreHeapInfo    *)((char  *)p-sizeof(PointerPreHeapInfo)))
#define GET_POSTHEADER(p)	((PointerPostHeapInfo  *)((char  *)p+(GET_SIZE_PTR(p))))
#define	KEY_NOT_FOUND		-1

#define LOG_INFO(s, ...)	print_log(stdout,s, ##__VA_ARGS__)
#define LOG_ERROR(s, ...)	print_log(stderr,s, ##__VA_ARGS__)

#ifdef __MEMMGR__

	typedef enum{
		RESET = 0,
		BRIGHT = 1,
		DIM = 2,
		UNDERLINE = 3,
		BLINK = 4,
		REVERSE = 7,
		HIDDEN = 8

	}TERM_CMD;

	typedef enum{

		BLACK = 0,
		RED = 1,
		GREEN = 2,
		YELLOW = 3,
		BLUE = 4,
		MAGENTA = 5,
		CYAN = 6,
		WHITE = 7
	}TERM_COLOR;

	typedef enum{
			UNKNOWN_ALLOCATE=0,
			MALLOC_ALLOCATOR,  //  by  default
			NEW_ALLOCATOR,
			NEW_WITH_BRACETS_ALLOCATOR,
			MAX_ALLOCATE_TYPES
	}ALLOCATOR_TYPE;

	//--------------------------------------------------------------------------------------------
	// STRUCTS

	typedef  struct{
		void  	*ptr;
		char  	filename[MAX_FILENAME_LENGTH];
		int  	line;
	}InfoAllocatedPointer;

	typedef  struct{
		void *pointer; //this is the element to  be ordered.
		int index;
	}PointerIndex;

	typedef  struct{
		int		type_allocator;
		int		offset_mempointer_table;
		char	filename[MAX_FILENAME_LENGTH];  //  base    		-16-256
		int		line;          					//  base          	-16
		int		size;                      		//  base          	-8
		int		pre_crc;                		//  base          	-4
	}PointerPreHeapInfo;

	typedef  struct{
		int		post_crc;
	}PointerPostHeapInfo;

	typedef struct{
		const char *file;
		int line;
	}RegisteredFileLine

	//--------------------------------------------------------------------------------------------
	// GLOBAL VARS

	static int	n_allocated_bytes  =  0;
	static int	n_allocated_pointers  =  0;
	static bool	memmgr_was_init  =  false;

	static void	*allocated_pointer[MAX_MEMPOINTERS];
	static int 	free_pointer_idx[MAX_MEMPOINTERS]={0};
	static int 	n_free_pointers=0;
	static PointerIndex allocated_pointer_idx[MAX_MEMPOINTERS]; // the same allocatedPointers it will have.


	static RegisteredFileLine registered_fileline[MAX_REGISTER_FILELINES]={0};

	//--------------------------------------------------------------------------------------------
	void  print_status(void);
	void  free_all_allocated_pointers( );

	int get_index_to_insert(PointerIndex *arr, int size, void * key)
	{
	  int low = 0, high = size - 1;

	  if (key > arr[high].pointer) return size;

	  while (high > low) {
		int mid = (low + high) / 2;
		if (arr[mid].pointer > key) high = mid;
		else low = mid + 1;
	  }

	  return low;
	}

	int binary_search(PointerIndex * A, void * key, int imin, int imax)
	{
	  // continue searching while [imin,imax] is not empty
	  while (imax >= imin)
		{
		  // calculate the midpoint for roughly equal partition
		  int imid = (imin + imax ) >> 1;
		  if(A[imid].pointer == key)
			// key found at index imid
			return imid;
		  // determine which subarray to search
		  else if (A[imid].pointer < key)
			// change min index to search upper subarray
			imin = imid + 1;
		  else
			// change max index to search lower subarray
			imax = imid - 1;
		}
	  // key was not found
	  return KEY_NOT_FOUND;
	}

	//--------------------------------------------------------------------------------------------
	void  get_filename(char  *filename, const char *absolute_filename)
	{
		const  char  *to_down_ptr;
		int  i, lenght;

		if(absolute_filename==NULL)
		{
			return;
		}

		if((lenght = (strlen(absolute_filename)-1)) > 0)
		{

			to_down_ptr = &absolute_filename[lenght-1];
			//  get  name  ...
			if((to_down_ptr-1)  >=  absolute_filename)
			{
				do
				{
						to_down_ptr--;

				}while(*(to_down_ptr-1)  !=  '\\'  &&  *(to_down_ptr-1)  !=  '/'  &&  to_down_ptr  >  absolute_filename);
			}

			sprintf(filename,"%s",to_down_ptr);
		}
	}

	RegisteredFileLine *pop_file_line(){
		if(n_registered_filelines>0){
			return &registered_fileline[n_registered_filelines--];
		}
		return NULL;
	}

	void set_color_terminal(FILE *std_type, int attr, int fg, int bg)
	{

		char command[13];

		/* Command is the control command to the terminal */
		sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
		fprintf(std_type, "%s", command);
	}

	#ifndef  __GNUC__
	#pragma  managed(push,  off)
	#endif
	void  print_log(FILE *std, const  char  *string_text, ...) {

		FILE *std_type = stderr;
		char  text[4096] = { 0 };
		va_list  ap;

		va_start(ap,  string_text);
		vsprintf(text,  string_text,  ap);
		va_end(ap);

		//  Results  Are  Stored  In  Text
	#ifdef _WIN32
	  SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_RED   | FOREGROUND_INTENSITY);
	#else // ansi color
		set_color_terminal(std_type, TERM_CMD::BRIGHT, TERM_COLOR::RED, TERM_COLOR::BLACK);
	#endif
		fprintf(std_type, "%s", text);
	#ifdef _WIN32
		SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE);
	#else // ansi color
		set_color_terminal(std_type, TERM_CMD::BRIGHT, TERM_COLOR::WHITE, TERM_COLOR::BLACK);
	#endif


		fprintf(std_type, "\n");
		fflush(std_type);
	}

	#ifndef  __GNUC__
	#pragma  managed(pop)
	#endif

	void  init()
	{

		if(!memmgr_was_init)
		{
			n_allocated_bytes  =  0;
			memset(allocated_pointer,0,sizeof(allocated_pointer));
			n_allocated_bytes  =  0;
			n_allocated_pointers  =  0;

			memmgr_was_init  =  true;
			n_free_pointers = MAX_MEMPOINTERS-1;
			memset(&allocated_pointer_idx,-1,sizeof(allocated_pointer_idx));


			for(int i = 0; i < n_free_pointers; i++){
				free_pointer_idx[i]=MAX_MEMPOINTERS-1-i;
			}


			LOG_INFO("*******************************");
			LOG_INFO("Memory mannagement initialized!");
			LOG_INFO("*******************************");

		}
	}
	//--------------------------------------------------------------------------------------------
	int  get_free_cell_memptr_table()
	{
		if(n_free_pointers > 0){
			return free_pointer_idx[n_free_pointers];
		}
		return KEY_NOT_FOUND; // no memory free...
	}
	//--------------------------------------------------------------------------------------------
	bool  is_pointer_registered(void *pointer)
	{
		int          i  =  0;

		int pos = binary_search(allocated_pointer_idx,pointer, 0, n_allocated_pointers);

		if(pos >=0){
			if(allocated_pointer_idx[pos].pointer == pointer)
				return true;
		}

		return  false;
	}
	//--------------------------------------------------------------------------------------------
	void	MEMMGR_registerFileLine(const char absolute_filename, int line){

	}

	void 	*MEMMGR_malloc(size_t  size,  const  char  *absolute_filename,  int  line)
	{
		PointerPreHeapInfo  *heap_allocat  =  NULL;
		void  *pointer  =  NULL;
		int  random_number,index;


		if(!memmgr_was_init)  init();  //  auto_inicialize  return  malloc(size);

		heap_allocat  =  (PointerPreHeapInfo  *)malloc(sizeof(PointerPreHeapInfo)  +  sizeof(PointerPostHeapInfo)  +  size);
		if(heap_allocat
				&&
				((index  =  get_free_cell_memptr_table())  !=  -1))
		{
			heap_allocat->size  =  size;
			heap_allocat->offset_mempointer_table  =  index;

			get_filename(heap_allocat->filename,  absolute_filename);
			heap_allocat->line  =  line;

			random_number  =  (unsigned(rand()%0xFFFF)  <<  16)  |  (rand()%0xFFFF);

			heap_allocat->pre_crc  =  random_number;
			heap_allocat->size        =  size;
			heap_allocat->type_allocator  =  MALLOC_ALLOCATOR;

			allocated_pointer[index] 	    = heap_allocat;

			*((int  *)((char  *)heap_allocat+size+sizeof(PointerPreHeapInfo)))  =  random_number;

			n_allocated_bytes  +=  size;
			n_allocated_pointers++;

			pointer  =  ((char  *)heap_allocat+sizeof(PointerPreHeapInfo));
	//#ifdef __USE_FAST_DICOTOMIC__
			//-----------------------------------------------------------------
			int pos = get_index_to_insert(allocated_pointer_idx,n_allocated_pointers,pointer);

			for (int i = n_allocated_pointers-1; i >= pos; --i){
				allocated_pointer_idx[i+1] = allocated_pointer_idx[i];
			}
			allocated_pointer_idx[pos].pointer = heap_allocat;
			allocated_pointer_idx[pos].index = index;

			//-----------------------------------------------------------------
	//#endif

			n_free_pointers--;

			//  memset  pointer
			memset(pointer,0,size);

			return    (pointer);  //  return  base  pointer

		}

		LOG_ERROR("Table full of pointers or not enought memory");
		exit(EXIT_FAILURE);
	}
	//--------------------------------------------------------------------------------------------
	void  MEMMGR_free(void  *pointer,  const  char  *absolute_filename,  int  line  )//,  char  *name,  int  file)
	{
		PointerPreHeapInfo    *preheap_allocat    =  NULL;
		PointerPostHeapInfo  *postheap_allocat  =  NULL;
		void  *base_pointer;
		char  filename[MAX_FILENAME_LENGTH];

		get_filename(filename,absolute_filename);

		if(pointer)
		{

			//  Getheaders...
			base_pointer  =  preheap_allocat    =  GET_PREHEADER(pointer);
			postheap_allocat  =  GET_POSTHEADER(pointer);

			//  Check  headers...
			if(preheap_allocat->pre_crc  ==  postheap_allocat->post_crc)  //  crc  ok  :)
			{

				if(preheap_allocat->offset_mempointer_table  >=  0  &&  preheap_allocat->offset_mempointer_table  <=  MAX_MEMPOINTERS)
				{
					//  deallocate  pointer  will  be  ok  :)


					//  Mark  freed...
					allocated_pointer[preheap_allocat->offset_mempointer_table]  =  NULL;

					if(n_free_pointers<(MAX_MEMPOINTERS-1)){
						n_free_pointers++;
						free_pointer_idx[n_free_pointers] = preheap_allocat->offset_mempointer_table;
					}
					else {
						LOG_ERROR("reached max pointers!");
					}

	//#ifdef __USE_FAST_DICOTOMIC__
					//-----------------------------------------------------------------
					// DELETE ORDERED ELEMENT...
					int pos = binary_search(allocated_pointer_idx,base_pointer, 0, n_allocated_pointers);
					if(pos >= 0){
						memcpy(&allocated_pointer_idx[pos],&allocated_pointer_idx[pos+1],(n_allocated_pointers-pos)*sizeof(PointerIndex));
					}
	//#endif
					//----------------------------------------------

					n_allocated_bytes-=preheap_allocat->size;
					n_allocated_pointers--;
					free(base_pointer);

				}
				else
				{
					LOG_ERROR("MEM  ERROR:  bad  index  mem  table  in  file  \"%s\"  at  line  %i.",filename,line);
				}
			}
			else
			{
				LOG_ERROR("MEM  ERROR:  Bad  crc  pointer  \"%s\"  at  line  %i.",filename,line);
			}
		}
		else
		{
			 LOG_ERROR("ERROR:  passed  pointer  is  null  \"%s\"  at  row  %i.",filename,line);
		}

	}

	void *MEMMGR_realloc(void *ptr, size_t size,  const  char  *absolute_filename,  int  line) {

		if (ptr==NULL) {
			// NULL ptr. realloc should act like malloc.
			return MEMMGR_malloc(size, absolute_filename, line);
		}

		PointerPreHeapInfo  *pre_head  =  GET_PREHEADER(ptr);


		if ((size_t)pre_head->size >= size) {
			// We have enough space. Could free some once we implement split.
			return ptr;
		}

		// Need to really realloc. Malloc new space and free old space.
		// Then copy old data to new space.
		void *new_ptr;
		new_ptr = MEMMGR_malloc(size, absolute_filename, line);

		if (!new_ptr) {
			return NULL; // TODO: set errno on failure.
		}
		memcpy(new_ptr, ptr, pre_head->size);
		MEMMGR_free(ptr, absolute_filename, line);
		return new_ptr;
	}
	//--------------------------------------------------------------------------------------------
	void  print_error_on_wrong_deallocate_method(int  allocator, const char *filename, int line)
	{


		switch(allocator)
		{
		case  MALLOC_ALLOCATOR:
			LOG_ERROR("ERROR:  allocated_pointer  at  filename  \"%s\"  line  %i  must  freed  with  function  free().",filename,  line);
			break;
		case  NEW_ALLOCATOR:
			LOG_ERROR("ERROR:  allocated_pointer  at  filename  \"%s\"  line  %i  must  freed  with  operator  delete.",filename,  line);
			break;
		case  NEW_WITH_BRACETS_ALLOCATOR:
			LOG_ERROR("ERROR:  allocated_pointer  at  filename  \"%s\"  line  %i  must  freed  with  operator  delete[].",filename,  line);
			break;
		}
	}

	//----------------------------------------------------------------------------------------
	void  pre_check_pointer_before_free(void  *p,  const  char  *absolute_filename,  int  line  )//,  char  *name,  int  file)
	{
		char  filename[MAX_FILENAME_LENGTH];
		get_filename(filename,absolute_filename);

		if(p)
		{
			PointerPreHeapInfo  *pre_head  =  GET_PREHEADER(p);

			if(pre_head->type_allocator  ==  MALLOC_ALLOCATOR)
			{
				MEMMGR_free(p,  filename,  line);
			}
			else  //  error
			{
				print_error_on_wrong_deallocate_method(pre_head->type_allocator, filename, line);
			}
		}
		else
		{
			LOG_ERROR("ERROR:  NULL  pointer  to  deallocate  at  filename  \"%s\"  line  %i.",filename,  line);
		}
	}
	//----------------------------------------------------------------------------------------
	void  free_all_allocated_pointers( )//,  char  *name,  int  file)
	{

		void *p;

		for(int i = 0; i < MAX_MEMPOINTERS; i++)
		{
			p = allocated_pointer[i];
			if(p)
			{
				//PointerPreHeapInfo  *pre_head  =  (PointerPreHeapInfo *)p;
				MEMMGR_free((char *)p+sizeof(PointerPreHeapInfo),"free_all_allocated_pointers()",0);
				allocated_pointer[i] = NULL;
			}
		}
	}
	//--------------------------------------------------------------------------------------------
	#ifdef  __cplusplus
	void*  operator  new(size_t  size)
	{



		void * p  =  MEMMGR_malloc(size,absolute_filename,line);

		if(p)
		{
			PointerPreHeapInfo  *pre_head  =  GET_PREHEADER(p);
			pre_head->type_allocator  =  NEW_ALLOCATOR;
		}

		return  p;
	}
	//--------------------------------------------------------------------------------------------
	void*  operator  new[](size_t  size,  const  char  *absolute_filename,  const  unsigned  int  line)
	{

		void *p = NULL;

		p  =  MEMMGR_malloc(size,absolute_filename, line);
		PointerPreHeapInfo  *pre_head  =  GET_PREHEADER(p);
		pre_head->type_allocator  =  NEW_WITH_BRACETS_ALLOCATOR;

		return  p;

	}
	//--------------------------------------------------------------------------------------------
	void  operator  delete(void  *p,  const  char  *absolute_filename,  const  unsigned  int  line) throw()//,  const  char  *file,  const  unsigned  int  line,  const  char  *func)
	{
		char  filename[MAX_FILENAME_LENGTH];
		get_filename(filename,absolute_filename);

		if(p)
		{
			if(!is_pointer_registered((char *)p-sizeof(PointerPreHeapInfo)))
			{
				LOG_ERROR("(%s:%i): allocated_pointer NOT REGISTERED OR POSSIBLE MEMORY CORRUPTION?!?!",filename,  line);
			}
			else
			{

				PointerPreHeapInfo   *pre_head   =  GET_PREHEADER(p);
				PointerPostHeapInfo  *post_head  =  GET_POSTHEADER(p);

				if(pre_head->pre_crc  ==  post_head->post_crc)
				{
					if(pre_head->type_allocator  ==  NEW_ALLOCATOR)
					{
						MEMMGR_free(p,  filename,  line);
					}
					else  //  error
					{
						print_error_on_wrong_deallocate_method(pre_head->type_allocator,  filename,  line);
					}
				}
				else
				{
					LOG_ERROR("(%s:%i): CRC  error!",filename,  line);
				}
			}
		}
		else
		{
			LOG_ERROR("ERROR:  NULL  pointer  to  deallocate  at  filename  \"%s\"  line  %i.",filename,  line);
		}

	}
	//--------------------------------------------------------------------------------------------


	void  operator  delete[](void  *p,  const  char  *absolute_filename,  const  unsigned  int  line) throw()//,  const  char  *file,  const  unsigned  int  line,  const  char  *func)
	{

		char  filename[MAX_FILENAME_LENGTH];
		get_filename(filename,absolute_filename);

		if(p)
		{

			if(!is_pointer_registered((char *)p-sizeof(PointerPreHeapInfo)))
			{
				LOG_ERROR("(%s:%i): allocated_pointer NOT REGISTERED WITH MALLOC OR NEW!",filename,  line);
			}
			else
			{

				PointerPreHeapInfo  *pre_head  =  GET_PREHEADER(p);

				if(pre_head->type_allocator  ==  NEW_WITH_BRACETS_ALLOCATOR)
				{
					MEMMGR_free(p,  absolute_filename,  line);
				}
				else  //  error
				{
					print_error_on_wrong_deallocate_method(pre_head->type_allocator,  filename,  line);
				}
			}
		}
		else
		{
			LOG_ERROR("ERROR:  NULL  pointer  to  deallocate  at  filename  \"%s\"  line  %i",filename,  line);
		}
	}
	#endif
	//--------------------------------------------------------------------------------------------
	void  print_status(void)
	{
		PointerPreHeapInfo    *preheap_allocat;
		int  i;

		for(i  =  0;  i  <  MAX_MEMPOINTERS;  i++)
		{
			if((preheap_allocat  =  (PointerPreHeapInfo    *)allocated_pointer[i]))
			{
				if(preheap_allocat->line>0 && (strcmp("??",preheap_allocat->filename)!=0)) // leak from others libs
				{
					LOG_ERROR("%s:%i:Allocated  pointer  NOT  DEALLOCATED (%p).",preheap_allocat->filename,  preheap_allocat->line,allocated_pointer[preheap_allocat->offset_mempointer_table]);
				}
			}
		}
		//-----
		if(n_allocated_pointers>0  ||  n_allocated_bytes>0)
		{
			LOG_ERROR("bytes  to  deallocate  =  %i  bytes",n_allocated_bytes);
			LOG_ERROR("Mempointers  to  deallocate  =  %i",n_allocated_pointers);
		}
		else
		{
			LOG_INFO("MEMRAM:ok.");
		}
	}

#endif
