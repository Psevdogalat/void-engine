#include <collision.h>
#include <geometry.h>


/* GJK algorith implementation ============================================= */
Vector2d gjkSupport(const Vector2d * vertices, size_t verticesQuantity, 
	const Vector2d  direction)
{
	size_t 		imaxVertex;
	double 		maxDistance;
	double 		distance;
	
	if(verticesQuantity < 1)
		return vector2d(0.0, 0.0);
	
	imaxVertex	= 0;
	maxDistance = scalarProd2d(vertices[imaxVertex], direction);
		
	for(unsigned int i = 1; i < verticesQuantity; i++){
		distance = scalarProd2d(vertices[i], direction);
		if(distance > maxDistance){
			maxDistance = distance;
			imaxVertex 	= i;
		}
	}
	
	return vertices[imaxVertex];
};


Vector2d gjkSupportSCO(
	const Vector2d * const array1, const size_t size1 , 
	const Vector2d * const array2, const size_t size2, 
	const Vector2d direction)
{
	return sub2d(
		gjkSupport(array1, size1, direction), 
		gjkSupport(array2, size2, neg2d(direction))
	);
};

bool gjkValidateSimplex(Vector2d * const simplex, size_t * const simplexSize, 
	Vector2d * const direction)
{
	Vector2d baVector;
	Vector2d acVector;
	
	switch(*simplexSize){
		case 1:
			*direction = neg2d(*direction);
		break;
		case 2:
			baVector = sub2d(simplex[1], simplex[0]);
			*direction = vector2d(-baVector.y, baVector.x);
			
			if(vectorProd2d(baVector, neg2d(simplex[1])) < 0.0){
				baVector  = simplex[0];
				simplex[0] = simplex[1];
				simplex[1] = baVector;
				*direction  = neg2d(*direction);
			}

		break;
		case 3:
			baVector = sub2d(simplex[2], simplex[1]);
			acVector = sub2d(simplex[0], simplex[2]);
			if(vectorProd2d(acVector, neg2d(simplex[2])) < 0.0)
			{
				if(vectorProd2d(baVector, neg2d(simplex[1])) < 0.0)
				{
					simplex[0] = simplex[2];
					*simplexSize	= 1;
					*direction = neg2d(simplex[0]);
					
				}else{
					simplex[1] 		= simplex[2];
					*simplexSize 	= 2;
					*direction 		= vector2d(acVector.y, -acVector.x);
				}
				
			}else
				if(vectorProd2d(baVector, neg2d(simplex[1])) < 0.0){
					simplex[0]		= simplex[2];
					*simplexSize 	= 2;
					*direction 		= vector2d(baVector.y, -baVector.x);
					
				}else
					return true;
		break;
	}
	
	return false;
}


bool gjkCollision2d(
	const Vector2d * const Vertices1, const size_t Vertices1_n, 
	const Vector2d * const Vertices2, const size_t Vertices2_n, 
	GJKSIMPLEX2D * const Simplex
){
	Vector2d 	direction;
	Vector2d 	support_vertex;
	Vector2d* 	simplex;
	size_t 	 	simplex_size;
	
	simplex_size	= 0;
	direction 		= vector2d(0.0, 1.0);
	
	while(true){
		support_vertex = gjkSupportSco(
			Vertices1, Vertices1_n, 
			Vertices2, Vertices2_n, 
			direction
		);
		
		if(scalar_product2d(support_vertex, direction) < 0.0)
			break;
		
		simplex[simplex_size++] = support_vertex;
		
		if(gjk_validate_simplex(simplex, simplex_size, direction)){
			
			if(Simplex != nullptr)
				for(size_t i=0; i<3; i++)
					Simplex->values[i] = simplex[i];
			
			delete [] simplex;
			return true;
		}
	}
	
	delete [] simplex;
	return false;
}

/* EPA algorith implementation ============================================= */
typedef struct{
	Vector2d 		normal;
	double 			distance;
	unsigned int 	index;
}EpaEdgeInfo;

void epaFindClosestEdge(const Vector2d * const polytope, 
	const size_t polytopeSize, EpaEdgeInfo * const edgeInfo)
{
	unsigned int index;
	Vector2d edge, normal;
	double distance;

	index = polytopeSize - 1;
	edge = polytope[index] - polytope[0];
	normal = nor2d(vector2d(-edge.y, edge.x));
	distance = scalarProd2d(polytope[index], normal);

	edgeInfo->index = index;
	edgeInfo->distance = distance;
	edgeInfo->normal = normal;

	for(index--; index < polytopeSize; index--){
		edge = polytope[index] - polytope[index + 1];
		normal = nor2d(vector2d(-edge.y, edge.x));
		distance = scalarProd2d(polytope[index], normal);

		if(distance < edgeInfo->distance){
			edgeInfo->index = index;
			edgeInfo->distance = distance;
			edgeInfo->normal = normal;
		}			
	}
}

void epaInsertVertex(Vector2d * const polytope, size_t * const polytopeSize, 
	const unsigned int index, const Vector2d vertex)
{
	for(unsigned int i = polytopeSize; i > Index + 1; i--)
		polytope[i] = polytope[i - 1];
	
	polytope[Index + 1] = vertex;
	polytopeSize++;
}

void getEpaInfo2d(
	const Vector2d * const array1, const size_t size1, 
	const Vector2d * const array2, const size_t size2,
	const GjkSimplex2d * const	simplex,
	Vector2d**	polytope, const size_t polytopeBufferSize
	EpaInfo2d * const epaInfo
){
	EpaInfo2d info;
	size_t polytopeSize;
	Vector2d vertex;
	double distance;
	EpaEdgeInfo	closestEdgeInfo;

	polytopeSize = 3;
	memcpy(polytope, simplex, sizeof(Vector2d) * 3);
	
	//polytope expansion
	while(true){
		epaFindClosestEdge(polytope, polytopeSize, &closestEdgeInfo);
		
		vertex = gjkSupportSCO(array1, size1, array2, size2,
			closestEdgeInfo.normal);
		
		distance = scalarProd2d(vertex, closestEdgeInfo.normal);
		//0.0000001 tolerance constant to prevent looping due floating point error
		if(distance - closestEdgeInfo.distance > 0.0000001 && 
			polytopeSize < polytopeBufferSize)
		{
			epaInsertVertex(polytope, polytopeSize, closest_edge.index, 
				vertex);
			
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
	
