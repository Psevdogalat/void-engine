#ifndef _COLLISIONS_HPP_
#define _COLLISIONS_HPP_

	#include <geometry.hpp>
	#include <collision.h>
	
	/* collision model class, used for convenient setting of collision figures 
		in space */ 
	class CollisionModel: public Transformation2d, public VectorArray2d
	{
		public:
		CollisionModel();
		CollisionModel(const CollisionModel &);
		CollisionModel(const VectorArray2d &);
		CollisionModel(const Vector2d * const, const size_t);
		~CollisionModel();

		bool raycast(const Vector2d &, const Vector2d &, RaycastInfo2d *) 
			const;

		bool collision(const CollisionModel &, GJKSimplex2d , EPAInfo2d *) 
			const;	

		bool collision(const Vector2d &, GJKSimplex2d , EPAInfo2d *) const;		
		
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
		union{			
			GJKSimplex2d gjk;
			EPAInfo2d epa;
			RaycastInfo2d ray;
		};
		
	}CollisionInfo;

	/*
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
	*/

#endif
