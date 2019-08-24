#pragma once

#ifdef NEW
#undef NEW
#endif

#ifdef MALLOC
#undef MALLOC
#endif

#ifdef FREE
#undef FREE
#endif

#ifdef REALLOC
#undef REALLOC
#endif

#ifdef __MEMMGR__

	#define NEW new(__FILE__,__LINE__)
	#define DELETE delete
	#define MALLOC(size) MEMMGR_malloc(size,__FILE__,__LINE__)
	#define REALLOC MEMMGR_realloc
	#define FREE MEMMGR_free

	void		push_file_line(const char absolute_filename, int line);

	// C
	void        *MEMMGR_malloc(size_t  size,  const  char  *absolute_filename,  int  line);
	void        *MEMMGR_realloc(void *ptr, size_t  size,  const  char  *absolute_filename,  int  line);
	void		MEMMGR_free(void  *pointer,  const  char  *absolute_filename,  int  line );

	// C++
	void*  operator  new(size_t  size);
	//void*  operator  new(size_t  size,    const  char  *absolute_filename,     int  line);

	void*  operator  new[](size_t  size);
	//void*  operator  new[](size_t  size,const  char  *absolute_filename,    int  line);

	void    operator  delete(void  *p);
	//void    operator  delete(void  *p,const  char  *absolute_filename,   int  line);

	void    operator  delete[](void  *p);
	//void    operator  delete[](void  *p,const  char  *file,   int  line);
#else

	#define NEW		new
	#define DELETE	delete
	#define MALLOC	malloc
	#define REALLOC realloc
	#define FREE	free
#endif
