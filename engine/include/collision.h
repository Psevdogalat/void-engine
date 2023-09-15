#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__

#include <stdlib.h>
#include <stdbool.h>
#include <geometry.h>

#ifdef __cplusplus
extern "C"{
#endif

	/* Gilbert — Johnson — Keerthi algorithm */
	/* 2D simplex for GJK algorithm */
	typedef Vector2d GJKSimplex2d[3];
	
	/* gjk collision check function betwen two convex shapes 		*/
	/* given by arrays of vertices. the function is not 			*/
	/* commutative with respect to the return value of the simplex 	*/
	bool gjkCollision2d(
		const Vector2d * const array1, const size_t arraySize1, 
		const Vector2d * const array2, const size_t arraySize2,
		GJKSimplex2d simplex);
	
	/* Expanding Polytope Algorithm */
	/* EPA info struct */
	typedef struct{
		Vector2d normal;
		double distance;
		size_t polytopeSize;
	}EPAInfo2d;
	
	/* function return additional information abount normal and 	*/
	/* penetration depth one shape in another, based on information */
	/* from previous call gjk_collision2d on the same source data 	*/
	void getEpaInfo2d(
		const Vector2d * const array1, const size_t arraySize1, 
		const Vector2d * const array2, const size_t arraySize2, 
		const GJKSimplex2d simplex, Vector2d * const polytope, 
		const size_t polytopeBufferSize, EPAInfo2d * const epaInfo);
	
	/* ray cast  */ 
	/* ray cast info struct*/
	typedef struct{
		Vector2d normal;
		double distance;
	}RaycastInfo2d;
	
	/* function check first intersection ray given from the origin and */
	/* shape given by array of vertices, additional instersection info */ 
	bool raycast2d(const Vector2d direction, const Vector2d * const vertices, 
		const size_t verticesNumber, RaycastInfo2d * const info);

#ifdef __cplusplus
}
#endif

#endif
