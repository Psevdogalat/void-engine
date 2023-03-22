#include <memoryManage.hpp>
#include <cstdlib>
#include <cstdint>
#include <cstring>


void cyclic_mmngr::init(){
	chunks 			= 0;
	amem_size 		= 0;
	mng_mem_size	= 0;
	cyl_mem_size	= 0;
	error_no 		= 0;
	
	mng_chunk_list = alloc_mng_chunk();
	if(mng_chunk_list == nullptr) 
		throw 0;
	
	cyl_chunk_list 	= alloc_chunk(chunk_size);
	if(cyl_chunk_list == nullptr){
		free(mng_chunk_list);
		throw 1;
	}	
	
	max_fill = get_fill();
}

cyclic_mmngr::cyclic_mmngr(){
	chunk_size 	= MM_MIN_CHUNK_SIZE;
	mem_limit 	= MM_MAX_MEMORY_SIZE;
	
	init();
	
}

cyclic_mmngr::cyclic_mmngr(size_t Mem_limit, size_t Chunk_size){
	chunk_size 	= (Chunk_size < MM_MIN_CHUNK_SIZE)?
		MM_MIN_CHUNK_SIZE:
		Chunk_size;
	
	Mem_limit = (Mem_limit < 2*chunk_size)?2*chunk_size:Mem_limit;

	mem_limit= (Mem_limit > MM_MAX_MEMORY_SIZE)?
		MM_MAX_MEMORY_SIZE:
		Mem_limit;
		
	init();
}

cyclic_mmngr::~cyclic_mmngr(){
	lmchunk_info* head;
	lmchunk_info* last;
	lmchunk_info* next;
	void*		  chunk;
	
	head = cyl_chunk_list;
	while(head != nullptr){
		free(head->info.head);
		head = head->next;
	}
	
	last = nullptr;
	head = mng_chunk_list;
	while(head != nullptr){
		next = head->next;
		head->next = last;
		last = head;
		head = next;
	} 
	
	while(last != nullptr){
		chunk	= last->info.head;
		last 	= last->next;
		free(chunk);
	}
	
}

lmchunk_info* cyclic_mmngr::alloc_mng_chunk(){
		lmchunk_info*	precord;
		void*			new_mng_chunk;
		
		new_mng_chunk = malloc(chunk_size);
		if(new_mng_chunk == nullptr){
			error_no = MM_ERR_ALLOC;
			return nullptr;
		}
		
		precord = (lmchunk_info*) new_mng_chunk;
		
		*precord = (lmchunk_info){
			new_mng_chunk, (uint8_t*)new_mng_chunk + sizeof(lmchunk_info), 
			chunk_size,
			chunk_size - sizeof(lmchunk_info),
			nullptr
		};
		
		mng_mem_size 	+= sizeof(lmchunk_info);
		amem_size		+= chunk_size;	
		chunks++;
		
		return precord;
}

lmchunk_info* cyclic_mmngr::alloc_chunk(size_t Size){
	void* 			chunk;
	lmchunk_info*	precord;
	
	if( (Size > mem_limit) || ((mem_limit - Size) < amem_size)){
		error_no = MM_ERR_MEMLIMIT;
		return nullptr;
	}
	
	chunk = malloc(Size);
	if(chunk == nullptr){
		error_no = MM_ERR_ALLOC;
		return nullptr;
	}
	
	precord = alloc_record();
	if(precord == nullptr){
		free(chunk);
		return nullptr;
	}

	*precord = (lmchunk_info){
		chunk, chunk, Size, Size,
		nullptr
	};
	
	amem_size += Size;
	chunks++;
	
	return precord;
}

lmchunk_info* cyclic_mmngr::alloc_record(){
	lmchunk_info* 	precord;
	lmchunk_info*	head;
	
	head = mng_chunk_list;
	for(;;head = head->next){
		for(;;head = head->next){
			if(head->info.rem >= sizeof(lmchunk_info)){
				precord 		= (lmchunk_info*) head->info.cur;
				head->info.cur	= (uint8_t*)head->info.cur + 
					sizeof(lmchunk_info);
				head->info.rem 	-= sizeof(lmchunk_info);
				mng_mem_size 	+= sizeof(lmchunk_info);
				return precord;
			}
			
			if(head->next == nullptr) break;
		};
		
		head->next = alloc_mng_chunk();
		
		if(head->next == nullptr) break;
	}
	
	return nullptr;
}

void* cyclic_mmngr::alloc(size_t Size){
	lmchunk_info* 	head;
	void*			mem_ptr;
	
	head = cyl_chunk_list;
	for(;;head = head->next){
		for(;;head = head->next){
			if(head->info.rem >= Size){
				mem_ptr 		= head->info.cur;
				head->info.cur 	= ((uint8_t*)head->info.cur + Size);
				head->info.rem 	-= Size;
				cyl_mem_size	+= Size;
				
				if(get_fill() > max_fill)max_fill = get_fill();
				
				return mem_ptr;
			}
			
			if(head->next == nullptr) break;
		};
		
		head->next = alloc_chunk((Size < chunk_size)?chunk_size:Size);
		
		if(head->next == nullptr) break;
	}
	
	return nullptr;
}

void cyclic_mmngr::clear(){
	lmchunk_info* head;
	
	for(head = cyl_chunk_list; head != nullptr; head = head->next){
		head->info.rem = head->info.size;
		head->info.cur = head->info.head;
	}
	
	cyl_mem_size = 0;
}

size_t cyclic_mmngr::get_chunk_size()const{return chunk_size;}
size_t cyclic_mmngr::get_mem_limit()const{return mem_limit;}
unsigned int cyclic_mmngr::get_chunks()const{return chunks;}
size_t cyclic_mmngr::get_alloc_mem()const{return amem_size;	}
size_t cyclic_mmngr::get_fill()const {return cyl_mem_size + mng_mem_size;};
size_t cyclic_mmngr::get_max_fill()const{return max_fill;}
unsigned int cyclic_mmngr::get_errno()const{return error_no;}
void cyclic_mmngr::clear_errno(){error_no = 0;}	
