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
				PhysicalModel *		physicalModel;
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
	
				void 			setPhysicalModel(PhysicalModel * );
				PhysicalModel *	getPhysicalModel() const;
				bool			isPhysical() const;
				
		};
		
		//clss for filtering game_objects
		class GameObjectFilter{
			public:
			GameObjectFilter();
			~GameObjectFilter();

			virtual	bool check(const GameObject *) const = 0;
		};

				
		class Camera: public OrientedObject2d, public Rectangle{
			public:
			Camera();
			~Camera();

		};

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
			virtual void onEvent(const Event &);
			virtual void onCalc(double );
			
			bool raycast(const Vector2d &, const Vector2d &, 
				const GameObjectFilter *, CollisionInfo * ) const;
			
			bool collision(const CollisionModel *, const GameObjectFilter *,  
				std::list<CollisionInfo> * infoList) const;
			
			bool bouncingRaycast(const Vector2d &, const Vector2d &, 
				const GameObjectFilter *, const GameObjectFilter *, 
				unsigned int, std::list<CollisionInfo> *) const;
			
			int spawn(GameObject *, const Vector2d &, const Vector2d &);
			int despawn(GameObject * );    

		};	
	
		class Engine
		{
			public:
			Engine();
			~Engine();

			static Engine & getInstance();

			int run(int argc, const char **argv);
			int addScene(Scene * scene);
			int removeScene(Scene * scene);
			int setTitle(const char * title);
			
			bool getAbsolutePath(std::string & pathAbsolute, 
				const std::string & path);
			
			int handleEvent(Event &);
			int render();
			int calculate();

			virtual int onInit();			
			virtual int onFree();

			private:
			std::list<Scene *> scenes;

			int init();
			int loop();
			int free();

			int initPlatform();
			int freePlatform();
			int loopPlatform();

			int initRenderer();
			int freeRenderer();

		};	
	}

#endif
