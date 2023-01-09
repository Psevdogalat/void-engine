/*
 USER INTERFACE BASIC EXTENSION
 common functionality for test modules to facilitate user interaction with 
 in-game elements
*/

#ifndef __UIBE_H__
#define __UIBE_H__

#include <engine.h>

namespace ENGINE{

	class UIBE_CONTAINER{
		protected:
		GAME_OBJECT* object;
		
		public:
		TRNSF_OBJECT		transform;	
		COLLISION_MODEL		collider;

		UIBE_CONTAINER();
		UIBE_CONTAINER(const UIBE_CONTAINER& );
		~UIBE_CONTAINER();

		static COLLISION_MODEL	standard_collider	();
		
		void 		 set_object			(GAME_OBJECT* );
		GAME_OBJECT* get_object			() const;
		void 		 set_position		(const VECTOR2D& );
		VECTOR2D 	 get_position		() const;
		void 		 set_normal			(const VECTOR2D& );
		VECTOR2D	 get_normal			() const;
		
		};

	void uibe_init		();
	void uibe_free		();
	void uibe_compute	(double );
	void uibe_event		(const ENGINE::EVENT_MSG& );
	
	void uibe_interactive_object_alloc	(GAME_OBJECT* );
	void uibe_interactive_object_free	(GAME_OBJECT* );
	void uibe_interactive_object_focus	(GAME_OBJECT* );
	
	UIBE_CONTAINER* 
	uibe_create_container
	(
	 const COLLISION_MODEL& 
	);	

	UIBE_CONTAINER*
	uibe_create_container
	();

	void
	uibe_delete_container
	(
	 const UIBE_CONTAINER*
	);

	void 
	uibe_focus_container
	(
		const UIBE_CONTAINER* 
	);

	VECTOR2D	uibe_get_mouse_coord();
	
}	

#endif
