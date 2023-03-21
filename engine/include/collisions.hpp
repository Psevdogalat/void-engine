#ifndef _COLLISIONS_HPP_
#define _COLLISIONS_HPP_

	#include <geometry.hpp>
	#include <collisions.h>
	
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
		bool collision(const CollisionModel &, GJKSimplex2 *, 
			EPAInfo2 *) const;	
		bool collision(const Vector2d &, GJKSimplex2 *, EPAInfo2 *) const;		
		
		CollisionModel & operator= (const CollisionModel &);
	};

		
#endif
