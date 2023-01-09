#ifndef __ENGINE_H__
#define	__ENGINE_H__

	#include <geometry.h>
	#include <collisions.h>
	#include <physics.h>
	#include <events.h>
	#include <graphics.h>
	#include <platform.h>
	#include <gui.h>
	
	#include <std_defines.h>
	#include <list>
	

	namespace ENGINE{
		
		class TIMER{
			protected:
				double time;
			
			public:
				
				TIMER();
				~TIMER();
				
				bool condition	();
				void set		(double );
				void countdown	(double );
			
		};
		
		class COLLISION_NODE; //for cross reference;
		
		//objects collision info struct
		typedef struct{
			COLLISION_NODE* 				node;
			enum{TYPE_RAW_GJK,TYPE_EPA} 	type;
			
			union{			
				GJK_SIMPLEX2D	gjk_simplex;
				EPA_INFO2D		epa_info;
			};
			
		}COLLISION_INFO;
		
		//collision list type definition
		typedef std::list<COLLISION_INFO> COLLISIONS_LIST;
		
		//default game object type 
		static const UINT GOT_UNDEFINE 	= 0;
		
		#define INVALID_ID 0
		
		//game object class
		//base class for all game objects in scenes
		class GAME_OBJECT{
			protected:
				bool 				visible;
				
				char*				name;
				UINT 				type;
				UINT				id;
				
				VECTOR2D			position;
				VECTOR2D			normal;
				
				GRAPHIC_MODEL*		graphic_model;
				PHYSICAL_MODEL*		physical_model;
				COLLISION_NODE*		collision_node;

			public:	
				
				GAME_OBJECT();
				GAME_OBJECT(const char*, const VECTOR2D&, const VECTOR2D& );
				virtual ~GAME_OBJECT();
				
				
				//virtual GAME_OBJECT* 	new_copy() = 0;
				virtual void compute();
				virtual void collision(GAME_OBJECT* , const COLLISION_INFO* );
				virtual void spawn();
				virtual void despawn();
				
				const char*		get_name() const;
				void 			set_name(const char* );
				
				const UINT 		get_type() const;
				
				void 			set_graphic_model	(GRAPHIC_MODEL* );
				GRAPHIC_MODEL* 	get_graphic_model	() const;
				void 			set_visible			(bool );
				bool 			is_visible			() const;
				
				void 			 set_collision_node	(COLLISION_NODE* );
				COLLISION_NODE*  get_collision_node	() const;
				bool			 is_collisible		() const;
				
				void 			set_physical_model	(PHYSICAL_MODEL* );
				PHYSICAL_MODEL*	get_physical_model	() const;
				bool			is_physical			() const;
				
				void 			set_position		(const VECTOR2D& );
				VECTOR2D		get_position		() const;
				
				void 			set_normal			(const VECTOR2D& );
				VECTOR2D		get_normal			() const;
				
		};
		
		//clss for filtering game_objects
		class GAME_OBJECT_FILTER{
			protected:
				static bool default_filt
				(
					const std::list<GAME_OBJECT*>& , 
					const GAME_OBJECT* 
				);
				
			public:
				std::list<GAME_OBJECT*> pointers;
				bool(* filter_func)
				(
					const std::list<GAME_OBJECT*>& , 
					const GAME_OBJECT* 
				);
				
				bool inverse;
				
				bool filt(const GAME_OBJECT* ) const;
				GAME_OBJECT_FILTER();
				GAME_OBJECT_FILTER(const GAME_OBJECT_FILTER& );
				~GAME_OBJECT_FILTER();
				
				GAME_OBJECT_FILTER& operator= (const GAME_OBJECT_FILTER& );
		};

		//class of collision node
		//collision node used in compute collisions
		class COLLISION_NODE{
			protected:
				GAME_OBJECT*		game_object;
				bool 				active;
				bool 				passive;
				bool				epa;
				COLLISION_MODEL*	model;
				COLLISIONS_LIST		collisions;
				
			public:
			
				COLLISION_NODE(
					GAME_OBJECT*, 
					bool, 
					bool, 
					bool, 
					COLLISION_MODEL*);
					
				~COLLISION_NODE();
				
				bool is_active	() const;
				bool is_passive	() const;
				bool is_epa		() const;
				
				void add_collision	(const COLLISION_INFO& );
				const COLLISIONS_LIST& 	get_collisions	()const;
				void clear_collisions();

				GAME_OBJECT*			get_game_object		()const;
				COLLISION_MODEL*		get_collision_model	()const;
				
				void 					compute(COLLISION_NODE* );
				
		};
		
		class CAMERA: public VECTOR_RECTANGLE{
			protected:
				VECTOR_RECTANGLE constraint;
				
			public:
				CAMERA();
				~CAMERA();
				
				void move			(const VECTOR2D& );
				void set_area		(const VECTOR_RECTANGLE& );
				void set_constraint (const VECTOR_RECTANGLE& );

		};

		/* raycast info type */
		typedef struct{
			const GAME_OBJECT* 	object;
			VECTOR2D			normal;
			double 				distance;		
		}RAYCAST_OBJ_INFO;
		
		/*lists types */
		typedef std::list<GAME_OBJECT* > 	GAME_OBJECTS_LIST;
		typedef std::list<TIMER*>		 	TIMERS_LIST;
		typedef std::list<PHYSICAL_MODEL*>	PHYSICS_LIST;
		typedef std::list<COLLISION_NODE*>	COLLISION_NODE_LIST;
		typedef std::list<RAYCAST_OBJ_INFO> RAYCAST_OBJ_INFO_LIST;

		class SCENE{
			public :
				CAMERA 				camera;
				GAME_OBJECTS_LIST	game_objects;
				GAME_OBJECTS_LIST	spawn_list;
				GAME_OBJECTS_LIST	despawn_list;
				COLLISION_NODE_LIST	active_collisions;
				COLLISION_NODE_LIST	passive_collisions;
				GUIELEMENT*			gui_root = nullptr;
                
                //UINT (event_handler*)(UINT MSG, void* Ptr1 
				
			public:
				SCENE();
				~SCENE();
				
                void __compute(double );

				virtual void init();
				virtual void free();
				virtual void pre_render();
				virtual void event(const EVENT_MSG& );
				virtual void compute(double );
				
				bool raycast(
					const VECTOR2D&, 
					const VECTOR2D&, 
					const GAME_OBJECT_FILTER*, 
					RAYCAST_OBJ_INFO* 
				) const;
				
				bool collision_test
				(
					const COLLISION_MODEL&		Model, 
					const GAME_OBJECT_FILTER*	Filter,  
					COLLISIONS_LIST* 			Info_list	
				) const;
				
				bool bouncing_raycast
				(
					const VECTOR2D&, const VECTOR2D&, 
					const GAME_OBJECT_FILTER*, 
					const GAME_OBJECT_FILTER*, 
					UINT, RAYCAST_OBJ_INFO_LIST& 
				) const;
				
				VECTOR2D translate_to_scene	
				(
					const VECTOR2D&, 
					const CAMERA* 
				) const;
				
				
				
				VECTOR2D translate_from_scene	
				(
					const VECTOR2D&, 
					const CAMERA* 
				) const;
				
				VECTOR2D translate_from_scene(const VECTOR2D& ) const;
				VECTOR2D translate_to_scene	 (const VECTOR2D& ) const;

                GAME_OBJECT* 
                spawn
                (
                    GAME_OBJECT* Object,
                    const VECTOR2D& Position,
                    const VECTOR2D& Normal
                );

                void 
                despawn
                (
                    GAME_OBJECT* Object
                );    

		};	
		
		/*
		void 			game_module_entrypoint	();
		SCENE*			register_scene			(SCENE* );
		SCENE*			set_current_scene		(SCENE* );
		void 			free					(SCENE* );
		*/
		
		SCENE* 	        get_current_scene		();
				
		TIMER* 			create_timer			();
		void 			free_timer				(TIMER* 			);
		
		PHYSICAL_MODEL* create_physical_model	();
		void 			free_physical_model		(PHYSICAL_MODEL* 	);
		
		COLLISION_NODE*	create_collision_node	
		(
			GAME_OBJECT*, 
			bool, 
			bool, 
			bool, 
			COLLISION_MODEL* 
		);
		
		void free_collision_node (COLLISION_NODE* );
		
		GAME_OBJECT* 	spawn		
		(
			GAME_OBJECT*, 
			const VECTOR2D&, 
			const VECTOR2D& 
		);
		
		void despawn (GAME_OBJECT* );

	}

#endif
