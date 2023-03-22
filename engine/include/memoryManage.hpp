#ifndef __MEMORY_MANAGE_HPP__
#define __MEMORY_MANAGE_HPP__

	#include <cstdlib>
	
	#define __MMFASTCALL__
	
	#define MM_MAX_MEMORY_SIZE 	16777216 //16Mb
	#define MM_MIN_CHUNK_SIZE 	512
	#define MM_MAX_CHUNKS(a) ((MM_MAX_MEMORY_SIZE)/(a))
	
	#define MM_ERR_ALLOC		1
	#define MM_ERR_CHUNKLIMIT	2
	#define MM_ERR_MEMLIMIT		3
	
	typedef struct{
		void* 		head;
		void* 		cur;
		size_t		rem;
		size_t		size;
	}mchunk_info;
	
	typedef struct _lmchunk_info{
		mchunk_info 	info;
		_lmchunk_info* 	next;
	}lmchunk_info;
	
	class cyclic_mmngr{
		protected:			
		size_t 			chunk_size;
		size_t			mem_limit;
		unsigned int 	error_no;
		
		size_t			amem_size;
		
		unsigned int	chunks;
		size_t			mng_mem_size;
		size_t 			cyl_mem_size;
		size_t 			max_fill;
		
		lmchunk_info*	mng_chunk_list;
		lmchunk_info*	cyl_chunk_list;
		
		void 			init				();
		lmchunk_info*	alloc_mng_chunk		();
		lmchunk_info*	alloc_record		();
		lmchunk_info*	alloc_chunk			( size_t);
			
		public: 
		cyclic_mmngr();
		cyclic_mmngr(size_t , size_t );
		~cyclic_mmngr();
		
		void* 			alloc			(size_t );
		void  			clear			();
		
		size_t 			get_chunk_size	()			const;
		size_t 			get_mem_limit	()			const;
		unsigned int   	get_chunks		()			const;
		size_t			get_alloc_mem	()			const;
		size_t			get_fill		()			const;
		size_t  		get_max_fill	()			const;
		unsigned int 	get_errno		()			const;
		void 			clear_errno		();
	};

#endif
