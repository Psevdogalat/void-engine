#include <collisions.h>

/* class COLLISION_MODEL */
//==============================================================================

COLLISION_MODEL::COLLISION_MODEL(){
	vertices_n 		= 0;
	raw_vertices	= nullptr;
	vertices 		= nullptr;
	
}

COLLISION_MODEL::COLLISION_MODEL(const COLLISION_MODEL& Model):
	TRNSF_OBJECT(Model){
	
	vertices_n 			= Model.vertices_n;
	vertices 			= new VECTOR2D[2*vertices_n];
	raw_vertices 		= vertices + vertices_n;
	
	for(UINT i = 0; i < vertices_n; i++)
		raw_vertices[i] = Model.raw_vertices[i];
	
}

COLLISION_MODEL::COLLISION_MODEL(const VECTOR_SHAPE& Shape){

	vertices_n 			= Shape.get_vertices_quantity();
	vertices 			= new VECTOR2D[2*vertices_n];
	raw_vertices 		= vertices + vertices_n;
	
	for(UINT i = 0; i < vertices_n; i++)
		Shape.get_vertex(i, raw_vertices[i]);
	
}

COLLISION_MODEL::COLLISION_MODEL
(
	const VERTEX_ARRAY& Vertex_array
)
{
	const VECTOR2D* array_vertices; 
	
	vertices_n 		= Vertex_array.get_vertices_n();
	array_vertices	= Vertex_array.get_vertices();
	
	vertices 			= new VECTOR2D[2*vertices_n];
	raw_vertices 		= vertices + vertices_n;
	
	for(UINT i = 0; i < vertices_n; i++)
		raw_vertices[i] = array_vertices[i];
	
}

COLLISION_MODEL::COLLISION_MODEL(const VECTOR2D* Vertices, UINT Vertices_n){
	
	vertices_n 		= (Vertices_n < 3)?3:Vertices_n;
	vertices 		= new VECTOR2D[2*vertices_n];
	raw_vertices	= vertices + vertices_n;
	
	for(UINT i = 0; i < Vertices_n; i++)
		raw_vertices[i] = Vertices[i];
	
	for(UINT i = Vertices_n; i < vertices_n; i++)
		raw_vertices[i] = vector2d(0.0, 0.0);
	
}

COLLISION_MODEL::~COLLISION_MODEL(){
	if(vertices != nullptr)
		delete [] vertices;
	
}

UINT COLLISION_MODEL::get_vertices_n() const{
	return vertices_n;
}

const VECTOR2D* COLLISION_MODEL::get_vertices() const{
	MATRIX33 tm;
	
	tm = get_forward_matrix();
	
	for(UINT i = 0; i < vertices_n; i++)
		vertices[i] = tm * raw_vertices[i];
	
	return (const VECTOR2D*) vertices;
}

const VECTOR2D* COLLISION_MODEL::get_raw_vertices() const{
	return (const VECTOR2D*) raw_vertices;
}

COLLISION_MODEL& 
COLLISION_MODEL::operator= 
(
	const COLLISION_MODEL& Rval
)
{
	this->~COLLISION_MODEL();
	new(this) COLLISION_MODEL(Rval);
	
	return *this;
}

/* GJK algorith implementation */
//==============================================================================
VECTOR2D gjk_support(
	const VECTOR2D* Vertices, UINT Vertices_quantity, 
	const VECTOR2D& Direction
){
	UINT 		imax_vertex;
	double 		max_distance;
	double 		distance;
	
	if(Vertices_quantity < 1)
		return vector2d(0.0, 0.0);
	
	imax_vertex	= 0;
	max_distance = scalar_product2d(Vertices[imax_vertex], Direction);
		
	for(UINT i = 1; i < Vertices_quantity; i++){
		distance = scalar_product2d(Vertices[i], Direction);
		if(distance > max_distance){
			max_distance 	= distance;
			imax_vertex 	= i;
		}
	}
	
	return Vertices[imax_vertex];
}


VECTOR2D gjk_support_sco(
	const VECTOR2D* Vertices1, UINT Vertices1_quantity, 
	const VECTOR2D* Vertices2, UINT Vertices2_quantity, 
	const VECTOR2D& Direction
){
	return (
		gjk_support(Vertices1, Vertices1_quantity, Direction) - 
		gjk_support(Vertices2, Vertices2_quantity,-Direction)
	);
}


bool gjk_validate_simplex(VECTOR2D* Simplex, UINT& Simplex_size, VECTOR2D& Direction){
	VECTOR2D ba_vector;
	VECTOR2D ac_vector;
	
	switch(Simplex_size){
		case 1:
			Direction = -Direction;
		break;
		case 2:
			ba_vector = Simplex[1] - Simplex[0];
			Direction = vector2d(-ba_vector.y, ba_vector.x);
			
			if(vector_product2d(ba_vector, -Simplex[1]) < 0.0){
				ba_vector  = Simplex[0];
				Simplex[0] = Simplex[1];
				Simplex[1] = ba_vector;
				Direction  = -Direction;
			}
		break;
		case 3:
			ba_vector = Simplex[2] - Simplex[1];
			ac_vector = Simplex[0] - Simplex[2];
			if(vector_product2d(ac_vector, -Simplex[2]) < 0.0){
				if(vector_product2d(ba_vector, -Simplex[1]) < 0.0){
					Simplex[0]		= Simplex[2];
					Simplex_size	= 1;
					Direction		= -Simplex[0];
					
				}else{
					Simplex[1] 		= Simplex[2];
					Simplex_size 	= 2;
					Direction 		= vector2d(ac_vector.y, -ac_vector.x);
				}
				
			}else
				if(vector_product2d(ba_vector, -Simplex[1]) < 0.0){
					Simplex[0]		= Simplex[2];
					Simplex_size 	= 2;
					Direction 		= vector2d(ba_vector.y, -ba_vector.x);
					
				}else
					return true;
		break;
	}
	
	return false;
}


bool gjk_collision2d(
	const VECTOR2D* Vertices1, 
	UINT  			Vertices1_n, 
	const VECTOR2D* Vertices2, 
	UINT  			Vertices2_n, 
	GJK_SIMPLEX2D* 	Simplex
){
	VECTOR2D 	direction;
	VECTOR2D 	support_vertex;
	VECTOR2D* 	simplex;
	UINT 	 	simplex_size;
	
	simplex_size	= 0;
	simplex 		= new VECTOR2D[3];
	direction 		= vector2d(0.0, 1.0);
	
	while(true){
		support_vertex = gjk_support_sco(
			Vertices1, Vertices1_n, 
			Vertices2, Vertices2_n, 
			direction
		);
		
		if(scalar_product2d(support_vertex, direction) < 0.0)
			break;
		
		simplex[simplex_size++] = support_vertex;
		
		if(gjk_validate_simplex(simplex, simplex_size, direction)){
			
			if(Simplex != nullptr)
				for(UINT i=0; i<3; i++)
					Simplex->values[i] = simplex[i];
			
			delete [] simplex;
			return true;
		}
	}
	
	delete [] simplex;
	return false;
}

/* EPA algorith implementation */
//=================================================================================================
typedef struct{
	VECTOR2D 	normal;
	double 		distance;
	UINT		index;
}EPA_EDGE_INFO;

EPA_EDGE_INFO epa_find_closest_edge(const VECTOR2D* Polytope, UINT Polytope_size){
	EPA_EDGE_INFO 	edge_info;
	UINT 			i;
	VECTOR2D 		edge, normal;
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

void epa_insert_vertex(VECTOR2D* Polytope , UINT& Polytope_size, UINT Index, VECTOR2D Vertex){
	for(UINT i = Polytope_size; i > Index+1; i--)
		Polytope[i] = Polytope[i-1];
	
	Polytope[Index+1] = Vertex;
	Polytope_size++;
}

EPA_INFO2D epa_info2d(
	const VECTOR2D* 		Vertices1, 
	const UINT  			Vertices1_n, 
	const VECTOR2D* 		Vertices2, 
	const UINT  			Vertices2_n,
	const GJK_SIMPLEX2D& 	Simplex,
	VECTOR2D**				Polytope
){
	EPA_INFO2D 		info;
	VECTOR2D*		polytope;
	UINT			polytope_size;
	UINT			polytope_size_max;
	VECTOR2D 		vertex;
	double 			distance;
	EPA_EDGE_INFO	closest_edge;
	
	//alloc memory for polytope, polytope size not higher sco size
	polytope_size_max =  Vertices1_n * Vertices2_n;
	polytope = new VECTOR2D[polytope_size_max];
	
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
	const VECTOR2D& Direction, 
	const VECTOR2D* Vertices, 
	const UINT 		Vertices_n, 
	RAYCAST_INFO2D*	Info
){
	
	VECTOR2D 	vertex_a;
	VECTOR2D 	vertex_b;
	VECTOR2D	normal;
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
	const VECTOR2D& 		Origin,
	const VECTOR2D& 		Direction, 
	const COLLISION_MODEL& 	Model,
	RAYCAST_INFO2D* 		Info
){
	MATRIX33 tm;
	const VECTOR2D*	raw_vertices;
	VECTOR2D*		vertices;
	UINT			vertices_n;
	bool 			collision;
	
	tm = mat33_trp(-Origin) * Model.get_forward_matrix();
	
	vertices_n 		= Model.get_vertices_n();
	raw_vertices	= Model.get_raw_vertices();
	vertices 		= new VECTOR2D[vertices_n];

	for(UINT i=0; i<vertices_n; i++)
		vertices[i] = tm * raw_vertices[i];
	
	collision = raycast2d(Direction, vertices, vertices_n, Info);
	
	delete [] vertices;
	return collision;
}
	

/* model to model check collision function*/
bool mtm_collision2d(
	const COLLISION_MODEL& 	Model_first	,
	const COLLISION_MODEL& 	Model_second,
	GJK_SIMPLEX2D*			Gjk_simplex,
	EPA_INFO2D*				Epa_info 	 
){
	const VECTOR2D* vertices1;
	const VECTOR2D* vertices2;
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
	const COLLISION_MODEL& 	Model_first	,
	const VECTOR2D& 		Point,
	GJK_SIMPLEX2D*			Gjk_simplex, 
	EPA_INFO2D*				Epa_info 	 
){
	const VECTOR2D* vertices1;
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