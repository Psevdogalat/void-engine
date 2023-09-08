#ifndef _COLLISIONS_HPP_
#define _COLLISIONS_HPP_

	#include <geometry.hpp>
	#include <collision2.h>
	
	/* collision model class, used for convenient setting of collision figures 
	in space */ 
	class CollisionModel: public Transformation2d, public VectorArray2
	{
		public:
		CollisionModel();
		CollisionModel(const CollisionModel &);
		CollisionModel(const VectorArray2 &);
		CollisionModel(const Vector2d *, size_t);
		~CollisionModel();

		bool raycast(const Vector2d &, const Vector2d &, RaycastInfo2 *) const;
		bool collision(const CollisionModel &, GJKSimplex2 *, EPAInfo2 *) 
			const;	
		bool collision(const Vector2d &, GJKSimplex2 *, EPAInfo2 *) const;		
		
		CollisionModel & operator= (const CollisionModel &);
	};

	class CollisionNode;
	class GameObject;

	typedef struct
	{
		enum CollisionType
		{
			CT_GJK,
			CT_EPA,
			CT_RAY
		};
		
		CollisionType type;
		GameObject * object;
		CollisionNode * node;
		
		union{			
			GJKSimplex2	 gjk;
			EPAInfo2	 epa;
			RaycastInfo2 ray;
		};
		
	}CollisionInfo;

	class CollisionNode
	{
		public:
		typedef std::list<CollisionInfo> CollisionsList;	

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
		CollisionModel *  getCollisionModel()const;
		
		bool imprint(CollisionNode * );
			
	};

#endif
