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
		
		//default game object type 
		static const UINT GOT_UNDEFINE 	= 0;
		
		#define INVALID_ID 0
		
		//game object class
		//base class for all game objects in scenes
		class GameObject: public OrientedObject2d{
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
				
				//virtual GameObject* 	newCopy() = 0;
				virtual void onCompute();
				virtual void onCollision(GameObject *, const CollisionInfo * );
				virtual void onSpawn();
				virtual void onDespawn();
				
				const char*		getName() const;
				void 			setName(const char* );
		
				const TypeId	getType() const;
				
				void 			setGraphicModel(GraphicModel * );
				GraphicModel * 	getGraphicModel() const;
				void 			setVisible(bool );
				bool 			isVisible() const;
				
				void 			setCollisionNode(CollisionNode * );
				CollisionNode * getCollisionNode() const;
				bool 			isCollisible() const;
	
				void 			setPhysicModel(PhysicModel * );
				PhysicModel *	getPhysicModel() const;
				bool			isPhysical() const;
				
		};
		
		//clss for filtering game_objects
		class GameObjectFilter{
			public:
				GameObjectFilter();
				~GameObjectFilter();

				virtual	bool check(const GameObject *) const = 0;
		};

		//class of collision node
		//collision node used in compute collisions
		class CollisionNode{
			private:
				GameObject *		object;
				bool 				active;
				bool 				passive;
				bool				epa;
				CollisionModel *	model;
				CollisionsList		collisions;
				
			public:
				CollisionNode(GameObject *, bool, bool, bool, 
					CollisionModel *);
					
				~CollisionNode();
				
				bool isActive() const;
				bool isPassive() const;
				bool isEpa() const;
				
				CollisionsList &  getCollisions()const;
				GameObject *	  getGameObject()const;
				CollisionModeli * getCollisionModel()const;
				
				bool imprint(CollisionNode * );
				
		};
		
		class Camera: public VectorRectangle{
			private:
			VectorRectangle constraint;
				
			public:
			Camera();
			~Camera();
			
			void move(const Vector2d & );
			void setArea(const VectorRectangle & );
			void setConstraint(const VectorRectangle & );
		};

		/* raycast info type */
		typedef struct{
			const GameObject* 	object;
			Vector2d			normal;
			double 				distance;		
		}RaycastObjInfo;
		
		class Scene{
			public :
			Camera * camera;
			std::list<GameObject *>	objects;
			std::list<GameObject *>	spawnList;
			std::list<GameObject *> despawnList;

			std::list<CollisionNode *>	activeCollisions;
			std::list<CollisionNode *>	passiveCollisions;

			GuiObject *	guiRoot = nullptr;
			
			public:
			Scene();
			~Scene();
			
			void calc(double );

			virtual void onInit();
			virtual void onFree();
			virtual void onRender();
			virtual void onEvent(const EVENT_MSG& );
			virtual void onCalc(double );
			
			bool raycast(const Vector2di &, const Vector2d &, 
				const GameObjectFilter *, RaycastObjInfo *) const;
			
			bool collision(const CollisionModel *, const GameObjectFilter *,  
				std::list<CollisionInfo> * infoList) const;
			
			bool bouncingRaycast(const Vector2d &, const Vector2d &, 
				const GameObjectFilter *, const GameObjectFilter *, 
				unsigned int, std::list<RaycastObjInfo> &) const;
			
			int spawn(GameObject *, const Vector2d &, const Vector2d &);
			int despawn(GameObject * );    

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
