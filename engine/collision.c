#include <collision.h>
#include <geometry.h>
#include <string.h>


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

bool gjkValidateSimplex(GJKSimplex2d simplex, 
	size_t * const simplexSize, Vector2d * const direction )
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
	const Vector2d * const array1, const size_t arraySize1, 
	const Vector2d * const array2, const size_t arraySize2, 
	GJKSimplex2d simplex
){
	Vector2d direction;
	Vector2d supportVertex;
	size_t simplexSize;
	
	simplexSize	= 0;
	direction = vector2d(0.0, 1.0);
	
	while(true){
		supportVertex = gjkSupportSCO(array1, arraySize1, array2, arraySize2, 
			direction );
		
		if( scalarProd2d(supportVertex, direction) < 0.0 )
			break;
		
		simplex[simplexSize++] = supportVertex;
		
		if( gjkValidateSimplex(simplex, &simplexSize, &direction) )
			return true;
	}
	
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
	edge = sub2d(polytope[index], polytope[0]);
	normal = nor2d(vector2d(-edge.y, edge.x));
	distance = scalarProd2d(polytope[index], normal);

	edgeInfo->index = index;
	edgeInfo->distance = distance;
	edgeInfo->normal = normal;

	for(index--; index < polytopeSize; index--){
		edge = sub2d(polytope[index], polytope[index + 1]);
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
	for(unsigned int i = *polytopeSize; i > index + 1; i--)
		polytope[i] = polytope[i - 1];
	
	polytope[index + 1] = vertex;
	(*polytopeSize)++;
}

void getEpaInfo2d(
	const Vector2d * const array1, const size_t size1, 
	const Vector2d * const array2, const size_t size2,
	const GJKSimplex2d simplex, Vector2d * const polytope, 
	const size_t polytopeBufferSize,
	EPAInfo2d * const epaInfo
){
	size_t polytopeSize;
	Vector2d vertex;
	double distance;
	EpaEdgeInfo	closestEdgeInfo;

	polytopeSize = 3;
	memcpy(polytope, simplex, sizeof(GJKSimplex2d));
	
	//polytope expansion
	while(true){
		epaFindClosestEdge(polytope, polytopeSize, &closestEdgeInfo);
		
		vertex = gjkSupportSCO(array1, size1, array2, size2,
			closestEdgeInfo.normal);
		
		distance = scalarProd2d(vertex, closestEdgeInfo.normal);

		/* 0.0000001 tolerance constant to prevent looping due floating 
			point error */
		if(distance - closestEdgeInfo.distance > 0.0000001 && 
			polytopeSize < polytopeBufferSize)
		{
			epaInsertVertex(polytope, &polytopeSize, closestEdgeInfo.index, 
				vertex);
			
		}
		else 
			break;

	}

	epaInfo->distance = closestEdgeInfo.distance;
	epaInfo->normal	= closestEdgeInfo.normal;
	epaInfo->polytopeSize = polytopeSize;

}

/* raycast to shape implementation ========================================= */
bool raycast2d(const Vector2d direction, const Vector2d * const vertices, 
	const size_t verticesNumber, RaycastInfo2d * const info)
{
	Vector2d vertexA;
	Vector2d vertexB;
	Vector2d normal;
	double vecA;
	double vecB;
	double distance;
	double testDistance;
	unsigned int intersections;
	
	intersections = 0;
	vertexA = vertices[verticesNumber - 1];
	
	for(unsigned int i = 0; i < verticesNumber; i++){
		vertexB = vertices[i];
		
		vecA = vectorProd2d(direction, vertexA);
		vecB = vectorProd2d(direction, vertexB);
		
		if(vecA * vecB < 0)
		{
			testDistance = vectorProd2d(vertexA, vertexB)/(vecB - vecA);
			
			if(intersections == 0 || distance > testDistance)
			{
				distance = testDistance;
				normal = leftOrtoNormal2d( sub2d(vertexB, vertexA));
				intersections++;
			}
		}
		
		vertexA = vertexB;
	}
	
	if(intersections != 0){
		if(info != NULL){
			info->distance 	= distance;
			info->normal	= normal;
		}
		
		return true;	
	}
	
	return false;
}

