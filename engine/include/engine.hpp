#ifndef __ENGINE_HPP__
#define	__ENGINE_HPP__

	#include <geometry.hpp>
	#include <collisions.hpp>
	#include <physics.hpp>
	#include <events.hpp>
	#include <graphics.hpp>
	#include <gui.hpp>
	
	#include <list>	

	namespace VoidEngine{
		
		class Timer{
			protected:
				double time;
			
			public:
				
				Timer();
				~Timer();
				
				bool condition	();
				void set		(double );
				void countdown	(double );
			
		};
		
		class CollisionNode;
		
		//objects collision info struct
		typedef struct{
			enum CollisionType
			{
				CT_GJK,
				CT_EPA
			};
			
			CollisionNode * node;
			CollisionType 	type;
			
			union{			
				GJK2DSimplex	gjk;
				EPA2DInfo		epa;
			}data;
			
		}CollisionInfo;
		
		//collision list type definition
		typedef std::list<CollisionInfo> COLLISIONS_LIST;
		
		//default game object type 
		static const UINT GOT_UNDEFINE 	= 0;
		
		#define INVALID_ID 0
		
		//game object class
		//base class for all game objects in scenes
		class GameObject{
			public:
				typedef unsigned int TypeId;
				typedef unsigned int ObjectId;

			protected:
				bool 				visible;
				
				char*				name;
				TypeId 				type;
				ObjectId			id;
				
				Vector2d			position;
				Vector2d			direction;
				
				GraphicModel *		graphicModel;
				PhysicModel *		physicModel;
				CollisionNode *		collisionNode;

			public:	
				
				GameObject();
				GameObject(const char *, const Vector2d &, const Vector2d & );
				virtual ~GameObject();
				
				//virtual GameObject* 	new_copy() = 0;
				virtual void compute();
				virtual void collision(GameObject* , const CollisionInfo* );
				virtual void spawn();
				virtual void despawn();
				
				const char*		getName() const;
				void 			setName(const char* );
				
				const TypeId	getType() const;
				
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
				
				void 			set_position		(const Vector2d& );
				Vector2d		get_position		() const;
				
				void 			set_normal			(const Vector2d& );
				Vector2d		get_normal			() const;
				
		};
		
		//clss for filtering game_objects
		class GameObject_FILTER{
			protected:
				static bool default_filt
				(
					const std::list<GameObject*>& , 
					const GameObject* 
				);
				
			public:
				std::list<GameObject*> pointers;
				bool(* filter_func)
				(
					const std::list<GameObject*>& , 
					const GameObject* 
				);
				
				bool inverse;
				
				bool filt(const GameObject* ) const;
				GameObject_FILTER();
				GameObject_FILTER(const GameObject_FILTER& );
				~GameObject_FILTER();
				
				GameObject_FILTER& operator= (const GameObject_FILTER& );
		};

		//class of collision node
		//collision node used in compute collisions
		class COLLISION_NODE{
			protected:
				GameObject*		game_object;
				bool 				active;
				bool 				passive;
				bool				epa;
				COLLISION_MODEL*	model;
				COLLISIONS_LIST		collisions;
				
			public:
			
				COLLISION_NODE(
					GameObject*, 
					bool, 
					bool, 
					bool, 
					COLLISION_MODEL*);
					
				~COLLISION_NODE();
				
				bool is_active	() const;
				bool is_passive	() const;
				bool is_epa		() const;
				
				void add_collision	(const CollisionInfo& );
				const COLLISIONS_LIST& 	get_collisions	()const;
				void clear_collisions();

				GameObject*			get_game_object		()const;
				COLLISION_MODEL*		get_collision_model	()const;
				
				void 					compute(COLLISION_NODE* );
				
		};
		
		class CAMERA: public VECTOR_RECTANGLE{
			protected:
				VECTOR_RECTANGLE constraint;
				
			public:
				CAMERA();
				~CAMERA();
				
				void move			(const Vector2d& );
				void set_area		(const VECTOR_RECTANGLE& );
				void set_constraint (const VECTOR_RECTANGLE& );

		};

		/* raycast info type */
		typedef struct{
			const GameObject* 	object;
			Vector2d			normal;
			double 				distance;		
		}RAYCAST_OBJ_INFO;
		
		/*lists types */
		typedef std::list<GameObject* > 	GameObjectS_LIST;
		typedef std::list<TIMER*>		 	TIMERS_LIST;
		typedef std::list<PHYSICAL_MODEL*>	PHYSICS_LIST;
		typedef std::list<COLLISION_NODE*>	COLLISION_NODE_LIST;
		typedef std::list<RAYCAST_OBJ_INFO> RAYCAST_OBJ_INFO_LIST;

		class SCENE{
			public :
				CAMERA 				camera;
				GameObjectS_LIST	game_objects;
				GameObjectS_LIST	spawn_list;
				GameObjectS_LIST	despawn_list;
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
					const Vector2d&, 
					const Vector2d&, 
					const GameObject_FILTER*, 
					RAYCAST_OBJ_INFO* 
				) const;
				
				bool collision_test
				(
					const COLLISION_MODEL&		Model, 
					const GameObject_FILTER*	Filter,  
					COLLISIONS_LIST* 			Info_list	
				) const;
				
				bool bouncing_raycast
				(
					const Vector2d&, const Vector2d&, 
					const GameObject_FILTER*, 
					const GameObject_FILTER*, 
					UINT, RAYCAST_OBJ_INFO_LIST& 
				) const;
				
				Vector2d translate_to_scene	
				(
					const Vector2d&, 
					const CAMERA* 
				) const;
				
				
				
				Vector2d translate_from_scene	
				(
					const Vector2d&, 
					const CAMERA* 
				) const;
				
				Vector2d translate_from_scene(const Vector2d& ) const;
				Vector2d translate_to_scene	 (const Vector2d& ) const;

                GameObject* 
                spawn
                (
                    GameObject* Object,
                    const Vector2d& Position,
                    const Vector2d& Normal
                );

                void despawn(GameObject* Object);    

		};	
	

		enum RetCode
		{
			SUCCESS,
			ERR
		};

		class Engine
		{
			public:
			Engine();
			~Engine();

			static Engine * getInstance();

			int run(int argc, const char **argv);
			int addScene(Scene * scene);
			int removeScene(Scene * scene);
			int setTitle(const char * title);
			
			bool getAbsolutePath(std::string & pathAbsolute, 
				const std::string & path);
			
			virtual int onInit();			
			virtual int onFree();

			private:
			static Engine * instance = NULL;
			std::list<Scene *> scenes;

			int init();
			int loop();
			int handleEvent(EVENT_MSG & event);
			int calculate();
			int render();
			int free();

			int initPlatform();
			int freePlatform();
			int loopPlatform();

			int initRenderer();
			int freeRenderer();

		};	
	}

#endif
