#include <collisions.h>
#include <vector2d.h>


/* GJK algorith implementation */
//==============================================================================
Vector2d gjkSupport(const Vector2d * vertices, size_t verticesQuantity, 
	Vector2d  direction)
{
	size_t 		imaxVertex;
	double 		maxDistance;
	double 		distance;
	
	if(verticesQuantity < 1)
		return vector2d(0.0, 0.0);
	
	imaxVertex	= 0;
	maxDistance = scalarProduct2(vertices[imaxVertex], direction);
		
	for(unsigned int i = 1; i < verticesQuantity; i++){
		distance = scalarProduct2(vertices[i], direction);
		if(distance > maxDistance){
			maxDistance = distance;
			imaxVertex 	= i;
		}
	}
	
	return Vertices[imaxVertex];
};


Vector2d gjkSupportSCO( const Vector2d * vertices1, size_t verticesQuantity1, 
	const Vector2d * vertices2, size_t verticesQuantity2, 
	const Vector2d & direction)
{
	return gjkSupport(vertices1, verticesQuantity1, direction) - 
		gjkSupport(vertices2, verticesQuantity2,-direction);
};


bool gjkValidateSimplex(Vector2d * simplex, size_t * simplexSize, 
	Vector2d * direction)
{
	Vector2d baVector;
	Vector2d acVector;
	
	switch(*simplexSize){
		case 1:
			*direction = negV2d(*direction);
		break;
		case 2:
			baVector = simplex[1] - simplex[0];
			*direction = vector2d(-baVector.y, baVector.x);
			
			if(vectorProduct2(baVector, negV2d(simplex[1])) < 0.0){
				baVector  = simplex[0];
				simplex[0] = simplex[1];
				simplex[1] = baVector;
				*direction  = negV2d(*direction);
			}

		break;
		case 3:
			baVector = simplex[2] - Simplex[1];
			acVector = simplex[0] - simplex[2];
			if(vproduct2d(acVector, -Simplex[2]) < 0.0){
				if(vector_product2d(baVector, -Simplex[1]) < 0.0){
					Simplex[0]		= Simplex[2];
					simplexSize	= 1;
					Direction		= -Simplex[0];
					
				}else{
					Simplex[1] 		= Simplex[2];
					simplexSize 	= 2;
					Direction 		= vector2d(acVector.y, -acVector.x);
				}
				
			}else
				if(vector_product2d(baVector, -Simplex[1]) < 0.0){
					Simplex[0]		= Simplex[2];
					simplexSize 	= 2;
					Direction 		= vector2d(baVector.y, -baVector.x);
					
				}else
					return true;
		break;
	}
	
	return false;
}


bool gjk_collision2d(
	const Vector2d* Vertices1, 
	size_t  			Vertices1_n, 
	const Vector2d* Vertices2, 
	size_t  			Vertices2_n, 
	GJK_SIMPLEX2D* 	Simplex
){
	Vector2d 	direction;
	Vector2d 	support_vertex;
	Vector2d* 	simplex;
	size_t 	 	simplex_size;
	
	simplex_size	= 0;
	simplex 		= new Vector2d[3];
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
				for(size_t i=0; i<3; i++)
					Simplex->values[i] = simplex[i];
			
			delete [] simplex;
			return true;
		}
	}
	
	delete [] simplex;
	return false;
}

