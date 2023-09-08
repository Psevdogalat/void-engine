#ifndef _COLLISIONS2_H_
#define _COLLISIONS2_H_

	#include <geometry.h>
	#include <collisions.h>
	
	UINT find_left_bottom_vertex(const VECTOR2D*, UINT );
	UINT find_distant_vertex	(const VECTOR2D*, UINT, const VECTOR2D& );
	
	void get_mch_jarvis	 (const VECTOR2D*, UINT, VECTOR2D*&, UINT& );
	void minkovskiy_sum	 (const VECTOR2D*, UINT, const VECTOR2D*, 
		UINT, VECTOR2D*&, UINT& );
	void minkovskiy_diff (const VECTOR2D*, UINT, const VECTOR2D*, UINT, 
		VECTOR2D*&, UINT& );

#endif
