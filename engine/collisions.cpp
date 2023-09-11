#include <collisions.hpp>

CollisionModel::CollisionModel(){};

CollisionModel::CollisionModel(const CollisionModel & sample):
	Transformation2d(sample), VectorArray2(sample){};

CollisionModel::CollisionModel(const VectorArray2 & array):
	VectorArray2(array){};

CollisionModel::CollisionModel(const Vector2d * array, size_t arraySize):
	VectorArray2(array, arraySize){};

CollisionModel::~CollisionModel(){};

CollisionModel & CollisionModel::operator= (const CollisionModel & rval)
{
	this->~CollisionModel();
	new(this) CollisionModel(rval);
	
	return *this;
}



/* model to model check collision function*/
bool mtm_collision2d(
	const CollisionModel& 	Model_first	,
	const CollisionModel& 	Model_second,
	GJK_SIMPLEX2D*			Gjk_simplex,
	EPA_INFO2D*				Epa_info 	 
){
	const Vector2d* vertices1;
	const Vector2d* vertices2;
	UINT	 		vertices1_n;
	UINT			vertices2_n;
	GJK_SIMPLEX2D	simplex;
	
	vertices1 	= Model_first.get_vertices();
	vertices1_n	= Model_first.get_vertices_n();
	vertices2 	= Model_second.get_vertices();
	vertices2_n	= Model_second.get_vertices_n();
	
	if(
		gjk_collision2d(
			vertices1, vertices1_n,
			vertices2, vertices1_n,
			&simplex
		)
	){
		if(Gjk_simplex != nullptr)
			*Gjk_simplex = simplex; 
		
		
		if(Epa_info != nullptr)
			*Epa_info = 
				epa_info2d(
					vertices1, vertices1_n,
					vertices2, vertices1_n,
					simplex,
					nullptr
				);
		
		
		return true;
	}
	
	return false;
}

bool ptm_collision2d(
	const CollisionModel& 	Model_first	,
	const Vector2d& 		Point,
	GJK_SIMPLEX2D*			Gjk_simplex, 
	EPA_INFO2D*				Epa_info 	 
){
	const Vector2d* vertices1;
	UINT	 		vertices1_n;
	GJK_SIMPLEX2D	simplex;
	
	vertices1 	= Model_first.get_vertices();
	vertices1_n	= Model_first.get_vertices_n();
	
	if(
		gjk_collision2d(
			vertices1	, vertices1_n,
			&Point		, 1,
			&simplex
		)
	){
		if(Gjk_simplex != nullptr)
			*Gjk_simplex = simplex; 
		
		
		if(Epa_info != nullptr)
			*Epa_info = 
				epa_info2d(
					vertices1	, vertices1_n,
					&Point		, 1,
					simplex,
					nullptr
				);
		
		
		return true;
	}
	
	return false;
		
}
