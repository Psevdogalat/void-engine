#ifndef _COLLISIONS_H_
#define _COLLISIONS_H_

	#include <std_defines.h>
	#include <geometry.h>
	
	/* collision model class, used for convenient setting of collision figures 
	in space */ 
	class COLLISION_MODEL: public TRNSF_OBJECT{
	protected:
		VECTOR2D* 	raw_vertices;
		VECTOR2D* 	vertices;
		UINT		vertices_n;
		
	public:
		
		COLLISION_MODEL();
		COLLISION_MODEL(const COLLISION_MODEL& 	);
		COLLISION_MODEL(const VECTOR_SHAPE& 	);
		COLLISION_MODEL(const VERTEX_ARRAY& 	);
		COLLISION_MODEL(const VECTOR2D*, UINT 	);
		~COLLISION_MODEL();
		
		UINT 			get_vertices_n	() const;
		const VECTOR2D* get_raw_vertices() const;
		const VECTOR2D*	get_vertices	() const;
		
		COLLISION_MODEL& operator= (const COLLISION_MODEL& Rval);
	};

	/* Gilbert — Johnson — Keerthi algorithm */
	/* 2D simplex for GJK algorithm */
	typedef struct{
		VECTOR2D values[3];
	}GJK_SIMPLEX2D;
	
	/* gjk collision check function betwen two convex shapes 		*/
	/* given by arrays of vertices. the function is not 			*/
	/* commutative with respect to the return value of the simplex 	*/
	bool gjk_collision2d(
		/* first shape array */
		const VECTOR2D* Vertices1, 
		UINT 			Vertices1_n , 
		/* second shape array */
		const VECTOR2D*	Vertices2, 
		UINT			Vertices2_n,
		/* if not nullptr return through themself simplex2d value */
		GJK_SIMPLEX2D*	Simplex
	);
	
	
	/* Expanding Polytope Algorithm */
	/* EPA info struct */
	typedef struct{
		VECTOR2D 	normal;
		double 		distance;
		UINT		polytope_size;
	}EPA_INFO2D;
	
	/* function return additional information abount normal and 	*/
	/* penetration depth one shape in another, based on information */
	/* from previous call gjk_collision2d on the same source data 	*/
	EPA_INFO2D epa_info2d(
		/* first shape array */
		const VECTOR2D* 		Vertices1, 
		UINT					Vertices1_n,
		/* second shape array */		
		const VECTOR2D*			Vertices2, 
		UINT					Vertices2_n, 
		/* simplex from previous call gjk_collision2d */ 
		const GJK_SIMPLEX2D& 	Simplex,
		/* if not nullptr, copy polytope in given addres */ 
		VECTOR2D**				Polytope
	);
	
	/* ray cast  */ 
	/* ray cast info struct*/
	typedef struct{
		VECTOR2D 	normal;
		double 		distance;
	}RAYCAST_INFO2D;
	
	/* function check first intersection ray given from the origin and */
	/* shape given by array of vertices, additional instersection info */ 
	/* return to RAYCAST_INFO struct */
	bool raycast2d(
		/* direction of ray given by normal vector */
		const VECTOR2D& Direction, 
		/* shape array */ 
		const VECTOR2D*	Vertices, 
		const UINT		Vertices_n, 
		/* if not nullptr return through themself raycast info*/ 
		RAYCAST_INFO2D*	Info
	);
	
	/* function check first intersection given by Origin and Direction 	*/ 
	/* ray and collision mode, additional instersection info return to 	*/ 
	/* RAYCAST_INFO struct */
	bool raycast2d(
		/* origin of ray */
		const VECTOR2D& 		Origin,
		/* direction of ray given by normal vector */
		const VECTOR2D& 		Direction, 
		/* collision model */
		const COLLISION_MODEL& 	Model,
		/* if not nullptr return through themself raycast info*/ 
		RAYCAST_INFO2D* 		Info
	);
	
	/* mtm_collision: model to model collision 				*/
	/* test collision between tho models, 					*/
	/* return true if collision detected, otherwise false	*/
	bool mtm_collision2d(
		/* first model, with which collision tested			*/
		const COLLISION_MODEL& 	Model_first	,
		/* second model, which collision tested				*/
		const COLLISION_MODEL& 	Model_second,
		/* if not nullptr, return gjk simplex through themself */		
		GJK_SIMPLEX2D*			Gjk_simplex, 
		/* if not nullptr, return epa info through themself */		
		EPA_INFO2D*				Epa_info 	 
	);	

	/* ptm_collision: point to model collision 				*/
	/* test collision between point and collision model, 	*/
	/* return true if collision detected, otherwise false	*/
	bool ptm_collision2d(
		/* first model, with which collision tested			*/
		const COLLISION_MODEL& 	Model_first	,
		/* point, which collision tested					*/
		const VECTOR2D& 		Point,
		/* if not nullptr, return gjk simplex through themself */		
		GJK_SIMPLEX2D*			Gjk_simplex, 
		/* if not nullptr, return epa info through themself */		
		EPA_INFO2D*				Epa_info 	 
	);		
	
	
#endif