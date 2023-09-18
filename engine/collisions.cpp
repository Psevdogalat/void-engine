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
	
	array1 = model1.getVertices();
	size1 = model1.getSize();
	array2 = model2.getVertices();
	size2 = model2.getSize();
	
	if( gjkCollision2d(array1, size1, array2, size2, simplex) )
	{
		if(info != nullptr)
		{
			Vector2d * polytopeBuffer;
			size_t polytopeBufferSize;

			polytopeBufferSize = size1 * size2;
			polytopeBuffer = (Vector2d *) malloc(sizeof(Vector2d) * 
				polytopeBufferSize);
			getEpaInfo2d(array1, size1, array2, size2, simplex, polytopeBuffer,
				polytopeBufferSize, info);
			free(polytopeBuffer);
		}
		
		return true;
	}
	
	return false;
}

bool ptmCollision2d( const CollisionModel & model, const Vector2d & point,
	GJKSimplex2d simplex, EPAInfo2d * info)
{
	const Vector2d * array1;
	size_t size1;

	array1 = model.getVertices();
	size1 = model.getSize();
	
	if( gjkCollision2d(array1, size1, &point, 1, simplex) )
	{
		if(info != nullptr)
		{
			Vector2d * polytopeBuffer;

			polytopeBuffer = (Vector2d *) malloc(sizeof(Vector2d) * size1);
			getEpaInfo2d(array1, size1, &point, 1, simplex, polytopeBuffer,
				size1, info);
			free(polytopeBuffer);
		}

		return true;
	}
	
	return false;
		
}

bool raycast2d(
	const Vector2d & origin,
	const Vector2d & direction, 
	const CollisionModel & model,
	RaycastInfo2d * const info
){
	Matrix33d tm;
	const Vector2d*	raw_vertices;
	Vector2d*		vertices;
	size_t			vertices_n;
	bool 			collision;
	
	tm = mat33_trp(-origin) * model.get_forward_matrix();
	
	vertices_n 		= Model.get_vertices_n();
	raw_vertices	= Model.get_raw_vertices();
	vertices 		= new Vector2d[vertices_n];

	for(UINT i=0; i<vertices_n; i++)
		vertices[i] = tm * raw_vertices[i];
	
	collision = raycast2d(Direction, vertices, vertices_n, Info);
	
	delete [] vertices;
	return collision;
}

