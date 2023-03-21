#ifndef _COLLISIONS_H_
#define _COLLISIONS_H_

#ifdef __cplusplus
extern "C"{
#endif

	/* Gilbert — Johnson — Keerthi algorithm */
	/* 2D simplex for GJK algorithm */
	typedef Vector2d[3] GJKSimplex2;
	
	/* gjk collision check function betwen two convex shapes 		*/
	/* given by arrays of vertices. the function is not 			*/
	/* commutative with respect to the return value of the simplex 	*/
	bool gjk2(const Vector2d *, size_t, const Vector2d *, size_t,
		GJKSimplex2 *);
	
	
	/* Expanding Polytope Algorithm */
	/* EPA info struct */
	typedef struct{
		Vector2d 	normal;
		double 		distance;
	}EPAInfo2;
	
	/* function return additional information abount normal and 	*/
	/* penetration depth one shape in another, based on information */
	/* from previous call gjk_collision2d on the same source data 	*/
	bool epa2(const Vector2d *, size_t, const Vector2d *, size_t, 
		const GJKSimplex2 *, Vector2d *, size_t *, EPAInfo2 *);
	
	/* ray cast  */ 
	/* ray cast info struct*/
	typedef struct{
		Vector2d 	normal;
		double 		distance;
	}RaycastInfo2;
	
	/* function check first intersection ray given from the origin and */
	/* shape given by array of vertices, additional instersection info */ 
	bool raycast2d(Vector2d, const Vector2d *, size_t, RaycastInfo2 *);

#ifdef __cplusplus
}
#endif

#endif
