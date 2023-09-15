#include <collisions.hpp>

CollisionModel::CollisionModel(){};

CollisionModel::CollisionModel(const CollisionModel & sample):
	Transformation2d(sample), VectorArray2d(sample){};

CollisionModel::CollisionModel(const VectorArray2d & array):
	VectorArray2d(array){};

CollisionModel::CollisionModel(const Vector2d * const array, 
	const size_t arraySize): VectorArray2d(array, arraySize){};

CollisionModel::~CollisionModel(){};

CollisionModel & CollisionModel::operator= (const CollisionModel & rval)

{
	this->~CollisionModel();
	new(this) CollisionModel(rval);
	
	return *this;
}



/* model to model check collision function*/
bool mtmCollision2d(
	const CollisionModel & model1,
	const CollisionModel & model2,
	GJKSimplex2d simplex,
	EPAInfo2d * const info 	 
){
	const Vector2d * array1; size_t size1;
	const Vector2d * array2; size_t size2;
	
	array1 	= Model_first.get_vertices();
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

bool raycast2d(
	const Vector2d & origin,
	const Vector2d & direction, 
	const CollisionModel & model,
	RaycastInfo2 * const info
){
	MATRIX33 tm;
	const Vector2d*	raw_vertices;
	Vector2d*		vertices;
	UINT			vertices_n;
	bool 			collision;
	
	tm = mat33_trp(-Origin) * Model.get_forward_matrix();
	
	vertices_n 		= Model.get_vertices_n();
	raw_vertices	= Model.get_raw_vertices();
	vertices 		= new Vector2d[vertices_n];

	for(UINT i=0; i<vertices_n; i++)
		vertices[i] = tm * raw_vertices[i];
	
	collision = raycast2d(Direction, vertices, vertices_n, Info);
	
	delete [] vertices;
	return collision;
}

