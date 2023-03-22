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

/* EPA algorith implementation */
//=================================================================================================
typedef struct{
	Vector2d 	normal;
	double 		distance;
	UINT		index;
}EPA_EDGE_INFO;

EPA_EDGE_INFO epa_find_closest_edge(const Vector2d* Polytope, UINT Polytope_size){
	EPA_EDGE_INFO 	edge_info;
	UINT 			i;
	Vector2d 		edge, normal;
	double 			distance;

	i = Polytope_size-1;
	
	edge 				= Polytope[i] - Polytope[0];
	edge_info.normal 	= normalize_vector(vector2d(-edge.y, edge.x));
	edge_info.distance	= scalar_product2d(Polytope[i], edge_info.normal);
	edge_info.index		= i;
	
	for(i--; i < Polytope_size; i--){
		edge = Polytope[i] - Polytope[i+1];
		normal 		= normalize_vector(vector2d(-edge.y, edge.x));
		distance	= scalar_product2d(Polytope[i], normal);
		if(distance < edge_info.distance){
			edge_info.distance 	= distance;
			edge_info.normal 	= normal;
			edge_info.index		= i;
		}			
	}
	
	return edge_info;
}

void epa_insert_vertex(Vector2d* Polytope , UINT& Polytope_size, UINT Index, Vector2d Vertex){
	for(UINT i = Polytope_size; i > Index+1; i--)
		Polytope[i] = Polytope[i-1];
	
	Polytope[Index+1] = Vertex;
	Polytope_size++;
}

EPA_INFO2D epa_info2d(
	const Vector2d* 		Vertices1, 
	const UINT  			Vertices1_n, 
	const Vector2d* 		Vertices2, 
	const UINT  			Vertices2_n,
	const GJK_SIMPLEX2D& 	Simplex,
	Vector2d**				Polytope
){
	EPA_INFO2D 		info;
	Vector2d*		polytope;
	UINT			polytope_size;
	UINT			polytope_size_max;
	Vector2d 		vertex;
	double 			distance;
	EPA_EDGE_INFO	closest_edge;
	
	//alloc memory for polytope, polytope size not higher sco size
	polytope_size_max =  Vertices1_n * Vertices2_n;
	polytope = new Vector2d[polytope_size_max];
	
	//copy simplex in polytope
	polytope_size = 3;
	for(UINT i = 0; i < polytope_size; i++)
		polytope[i] = Simplex.values[i];
	
	//polytope expansion
	while(true){
		closest_edge = epa_find_closest_edge(polytope, polytope_size);
		
		vertex = gjk_support_sco(
			Vertices1, Vertices1_n,
			Vertices2, Vertices2_n,
			closest_edge.normal
		);
		
		distance = scalar_product2d(vertex, closest_edge.normal);
		//0.0000001 tolerance constant to prevent looping due floating point error
		if(distance - closest_edge.distance > 0.0000001 && polytope_size < polytope_size_max){
			epa_insert_vertex(
				polytope			, polytope_size, 
				closest_edge.index	, vertex
			);
			
		}else{
			info.distance 		= closest_edge.distance;
			info.normal			= closest_edge.normal;
			info.polytope_size	= polytope_size;
			
			if(Polytope != nullptr)
				for(UINT i=0; i<polytope_size; i++)
					(*Polytope)[i] = polytope[i];
			
			break;
		}
	}
	
	delete [] polytope;
		
	return info;
}

/* raycast to shape implementation */
//=================================================================================================
bool raycast2d(
	const Vector2d& Direction, 
	const Vector2d* Vertices, 
	const UINT 		Vertices_n, 
	RAYCAST_INFO2D*	Info
){
	
	Vector2d 	vertex_a;
	Vector2d 	vertex_b;
	Vector2d	normal;
	double 		vec_a;
	double 		vec_b;
	double 		distance;
	double 		test_distance;
	UINT		intersections;
	
	intersections 	= 0;
	vertex_a 		= Vertices[Vertices_n-1];
	
	for(UINT i = 0; i < Vertices_n; i++){
		vertex_b = Vertices[i];
		
		vec_a = vector_product2d(Direction, vertex_a);
		vec_b = vector_product2d(Direction, vertex_b);
		
		if(vec_a * vec_b < 0){
			test_distance 		= vector_product2d(vertex_a, vertex_b)/ (vec_b - vec_a);
			
			if(!intersections || distance > test_distance){
				distance 	= test_distance;
				normal 		= left_orto_normal(vertex_b - vertex_a);
				intersections++;
			}
		}
		
		vertex_a = vertex_b;
	}
	
	if(intersections){
		if(Info != nullptr){
			Info->distance 	= distance;
			Info->normal	= normal;
		}
		
		return true;	
	}
	
	return false;
}

bool raycast2d(
	const Vector2d& 		Origin,
	const Vector2d& 		Direction, 
	const CollisionModel& 	Model,
	RAYCAST_INFO2D* 		Info
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
