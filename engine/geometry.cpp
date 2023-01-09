#include <geometry.h>
#include <std_defines.h>

#include <cstdio>
#include <cmath>

/* VECTOR2D methods realisation */
VECTOR2D operator+(const VECTOR2D& V1, const VECTOR2D& V2){
	VECTOR2D r_vector;
	r_vector.x = V1.x + V2.x;
	r_vector.y = V1.y + V2.y;
	return r_vector;
}

VECTOR2D operator-(const VECTOR2D& V1, const VECTOR2D& V2){
	VECTOR2D r_vector;
	r_vector.x = V1.x - V2.x;
	r_vector.y = V1.y - V2.y;
	return r_vector;
}

VECTOR2D operator- (const VECTOR2D& V){
	VECTOR2D r_vector;
	r_vector.x = -V.x;
	r_vector.y = -V.y;
	return r_vector;
}

VECTOR2D& operator+=(VECTOR2D& V1, const VECTOR2D& V2){
	V1.x += V2.x;
	V1.y += V2.y;
	return V1;
}

VECTOR2D& operator-=(VECTOR2D& V1, const VECTOR2D& V2){
	V1.x -= V2.x;
	V1.y -= V2.y;
	return V1;
}

VECTOR2D  operator* (const VECTOR2D& V, double Multiplier){
	VECTOR2D r_vector;
	r_vector.x = V.x * Multiplier;
	r_vector.y = V.y * Multiplier;
	return r_vector;
}

VECTOR2D operator* (double Multiplier, const VECTOR2D& V){
	return V * Multiplier;
}

VECTOR2D operator* (const VECTOR2D& V1, const VECTOR2D& V2){
	return vector2d(V1.x * V2.x, V1.y * V2.y);
}

VECTOR2D operator* (const MATRIX33& M, const VECTOR2D& V){
	VECTOR2D retv;
	retv.x = M.values[0][0] * V.x + M.values[0][1] * V.y + M.values[0][2];	
	retv.y = M.values[1][0] * V.x + M.values[1][1] * V.y + M.values[1][2];	
	return retv;
}

VECTOR2D  operator/ (const VECTOR2D& V, double Divider){
	VECTOR2D r_vector;
	r_vector.x = V.x / Divider;
	r_vector.y = V.y / Divider;
	return r_vector;
}

VECTOR2D	operator/ (double D,const VECTOR2D& Divider ){
	return vector2d(D/Divider.x, D/Divider.y);
}

VECTOR2D& operator*=(VECTOR2D& V, double Multiplier){
	V.x *= Multiplier;
	V.y *= Multiplier;
	return V;
}

VECTOR2D& operator/=(VECTOR2D& V, double Divider){
	V.x /= Divider;
	V.y /= Divider;
	return V;
}

bool operator==(const VECTOR2D& V1, const VECTOR2D& V2){
	if((V1.x == V2.x) && (V1.y == V2.y))
		return true;
	
	return false;
}

bool operator!=(const VECTOR2D& V1, const VECTOR2D& V2){
	if(	V1 == V2)
		return false;
	
	return true;
}

VECTOR2D vector2d(double X, double Y){
	VECTOR2D r_vector;
	r_vector.x = X;
	r_vector.y = Y;
	return r_vector;
}

VECTOR2D create_normal(){
	return vector2d(1,0);
}

double scalar_product2d(const VECTOR2D& V1, const VECTOR2D& V2){
	return V1.x * V2.x + V1.y * V2.y;
}

double vector_product2d(const VECTOR2D& V1, const VECTOR2D& V2){
	return V1.x * V2.y - V2.x * V1.y;
}

double 	 vector_length		(const VECTOR2D& V){
	return sqrt(scalar_product2d(V,V));
}

VECTOR2D normalize_vector(const VECTOR2D& V){
	double 		v_length;
	
	v_length = vector_length(V);
	
	if(v_length == 0)
		return create_normal();
	
	return V / v_length;
}


VECTOR2D rotate_vector(const VECTOR2D& V, double Angle){
	VECTOR2D r_vector;
	r_vector.x = V.x*cos(Angle) - V.y*sin(Angle);
	r_vector.y = V.x*sin(Angle) + V.y*cos(Angle);
	
	return r_vector;
}

VECTOR2D rotate_vector(const VECTOR2D& V, const VECTOR2D& Rotator){
	VECTOR2D normal;
	VECTOR2D r_vector;
	
	normal = normalize_vector(Rotator);
	r_vector.x = V.x * normal.x  - V.y * normal.y; 
	r_vector.y = V.x * normal.y  + V.y * normal.x;
	
	return r_vector;
}


VECTOR2D scale_vector(const VECTOR2D& V, double Scale){
	return V * Scale;
}

VECTOR2D scale_vector(const VECTOR2D& V, const VECTOR2D& Scale_vector){
	VECTOR2D r_vector;
	
	r_vector.x = V.x * Scale_vector.x;
	r_vector.y = V.y * Scale_vector.y;
	
	return r_vector;
}

VECTOR2D mirror_vector(const VECTOR2D& V, const VECTOR2D& Direction){
	VECTOR2D normal = normalize_vector(Direction);
	return V - 2 * scalar_product2d(V,normal)* normal; 
}

VECTOR2D transform_vertex(const VECTOR2D& Vertex,const VECTOR2D& Position, const VECTOR2D& Normal, double Scale ){
	return rotate_vector(Vertex, Normal) * Scale + Position;
}


VECTOR2D transform_vertex(const VECTOR2D& Vertex, const VECTOR2D& Position, const VECTOR2D& Normal, const VECTOR2D& Scale){
	return scale_vector(rotate_vector(Vertex, Normal), Scale) + Position;
}

VECTOR2D left_orto_normal	(const VECTOR2D& V){
	return normalize_vector(vector2d(-V.y, V.x));
}

/* MATRIX33 methods*/
MATRIX33	operator*	(const MATRIX33& M1, const MATRIX33& M2){
	UINT 		i,j,k;
	MATRIX33	rmat;
	for(i = 0; i<3; i++)
		for(j = 0; j<3; j++){
			rmat.values[i][j] = 0.0;
			for(k = 0; k<3; k++)
				rmat.values[i][j] += M1.values[i][k] * M2.values[k][j];
		}
	return rmat;
}


MATRIX33	mat33_rot	(const VECTOR2D& Rotor){
	return (MATRIX33){
		Rotor.x, -Rotor.y, 	0,
		Rotor.y,  Rotor.x, 	0,
		0,			0,		1
	};
}

MATRIX33	mat33_scl	(const VECTOR2D& Scale){
	return (MATRIX33){
		Scale.x, 	0, 		0,
		0,  	Scale.y, 	0,
		0,			0,		1
	};
}
MATRIX33	mat33_trp	(const VECTOR2D& Position){
	return (MATRIX33){
		1, 			0, 		Position.x,
		0,  		1, 		Position.y,
		0,			0,		1
	};
}
MATRIX33	mat33_trans	(
	const VECTOR2D& Pos, 
	const VECTOR2D& Rot, 
	const VECTOR2D& Scl
){
	return (MATRIX33){
		Scl.x * Rot.x, 	   -Scl.y*Rot.y, 		Pos.x,
		Scl.x * Rot.y,		Scl.y*Rot.x, 		Pos.y,
					0,				  0,				 1
	};
} 

MATRIX33 mat33_trans_back (
	const VECTOR2D& Pos, 
	const VECTOR2D& Rot, 
	const VECTOR2D& Scl
){
	double scl1, scl2;
	scl1 = (1.0 / Scl.x);
	scl2 = (1.0 / Scl.y);

	return (MATRIX33){
		 scl1 * Rot.x,	
		 scl1 * Rot.y, 	
		-scl1 * (Pos.x * Rot.x + Pos.y * Rot.y),

		-scl2 * Rot.y ,	
		 scl2 * Rot.x, 		  
		 scl2 * (Pos.x * Rot.y - Pos.y * Rot.x),
		
		0,	0,		 1
	};

}

/* TRNSF_OBJECT */
TRNSF_OBJECT::TRNSF_OBJECT
()
{
	position 	= {0.0, 0.0};
	normal 		= create_normal();
	scale		= {1.0, 1.0};
}	

TRNSF_OBJECT::TRNSF_OBJECT
(
	const VECTOR2D& Position, 
	const VECTOR2D& Normal, 
	const VECTOR2D&	Scale
)
{
	position 	= Position;
	normal		= normalize_vector(Normal);
	scale		= Scale;
}

TRNSF_OBJECT::TRNSF_OBJECT
(
	const TRNSF_OBJECT& Smpl
)
{
	position 	= Smpl.position;
	normal		= Smpl.normal;
	scale		= Smpl.scale;
}

TRNSF_OBJECT::~TRNSF_OBJECT
()
{}
			
void 	 
TRNSF_OBJECT::set_position	
(
	const VECTOR2D& Position
)
{
	position = Position; 	
}

void 	 
TRNSF_OBJECT::set_normal	
(
	const VECTOR2D& Normal
)	
{
	normal = normalize_vector(Normal); 
}

void 	 
TRNSF_OBJECT::set_scale	
(
	const VECTOR2D& Scale
)
{
	scale = Scale; 	
}

VECTOR2D 
TRNSF_OBJECT::get_position	
() const 
{
	return position; 	
}

VECTOR2D 
TRNSF_OBJECT::get_normal	
() const 
{
	return normal; 	
}

VECTOR2D 
TRNSF_OBJECT::get_scale	
() const 
{
	return scale; 	
}

MATRIX33 
TRNSF_OBJECT::get_forward_matrix	
() const 
{
	return mat33_trans
	(
		position, 
		normal, 
		scale
	);
}

MATRIX33 
TRNSF_OBJECT::get_backward_matrix	
() const 
{
	
	return mat33_trans_back
	(
		position, 
		normal, 
		scale
	);

}

TRNSF_OBJECT&
TRNSF_OBJECT::operator=
(
	const TRNSF_OBJECT& Rval
)
{
	position 	= Rval.position;
	normal		= Rval.normal;
	scale		= Rval.scale;
	
	return *this;
}

/* VERTEX_ARRAY */
	VERTEX_ARRAY::VERTEX_ARRAY(){
		vertices	= nullptr;
		vertices_n 	= 0;
		buffer_len	= 0;
		
	}
	
	VERTEX_ARRAY::VERTEX_ARRAY(const VECTOR2D* Vertices, UINT Vertices_n){
		vertices	= nullptr;
		vertices_n 	= 0;
		buffer_len 	= 0;
		
		set_vertices(Vertices, Vertices_n);
	}
	
	VERTEX_ARRAY::VERTEX_ARRAY(const VECTOR2D& Vertex, UINT Vertices_n){
		vertices	= nullptr;
		vertices_n 	= 0;
		buffer_len 	= 0;
		
		set_vertices(Vertex, Vertices_n);
	}
	
	VERTEX_ARRAY::VERTEX_ARRAY(const VERTEX_ARRAY& Vertex_array){
		vertices	= nullptr;
		vertices_n 	= 0;
		buffer_len 	= 0;
		
		set_vertices(Vertex_array.get_vertices(), Vertex_array.get_vertices_n());
	}
	
	VERTEX_ARRAY::~VERTEX_ARRAY(){
		if(vertices != nullptr)
			delete [] vertices;
	}
	
	void VERTEX_ARRAY::alloc_vertices_buffer(UINT Buffer_len){
		if(buffer_len < Buffer_len){
			buffer_len = Buffer_len;
			if(vertices != nullptr)
				delete [] vertices;
			
			vertices = new VECTOR2D[buffer_len];
		}
	}
	
	VERTEX_ARRAY* 	VERTEX_ARRAY::clone			() const {return new VERTEX_ARRAY(*this);}
	UINT 			VERTEX_ARRAY::get_vertices_n() const {return vertices_n;}
	const VECTOR2D*	VERTEX_ARRAY::get_vertices	() const {return (const VECTOR2D*) vertices;}
	
	void VERTEX_ARRAY::set_vertices(const VECTOR2D* Vertices, UINT Vertices_n){
		alloc_vertices_buffer(Vertices_n);
		
		vertices_n = Vertices_n;
		if(vertices != nullptr && Vertices != nullptr)
			for(UINT i = vertices_n; i--; )
				vertices[i] = Vertices[i];
			
	}
	
	void VERTEX_ARRAY::set_vertices(const VECTOR2D& Vertex, UINT Vertices_n){
		alloc_vertices_buffer(Vertices_n);
		
		vertices_n = Vertices_n;
		if(vertices != nullptr)
			for(UINT i = vertices_n; i--; )
				vertices[i] = Vertex;
			
	}
	
	void VERTEX_ARRAY::set_vertex(const VECTOR2D& Vertex, UINT Index){
		if(Index < vertices_n){
			vertices[Index] = Vertex;
		}
		
	}
	
	VERTEX_ARRAY& VERTEX_ARRAY::operator= (const VERTEX_ARRAY& Vertex_array){
		set_vertices(Vertex_array.get_vertices(), Vertex_array.get_vertices_n());
		return *this;
	}
	
	
/* VERTEX_LINE */
	VERTEX_LINE::VERTEX_LINE(){
		set_specs(0.0, 0.0);	
	}
	
	VERTEX_LINE::VERTEX_LINE(double Length, double Offset){
		set_specs(Length, Offset);
	}
	
	VERTEX_LINE::VERTEX_LINE(const VERTEX_LINE& Line){
		set_specs(Line.length, Line.offset);
	}
	
	VERTEX_ARRAY* 	VERTEX_LINE::clone		() const {return new VERTEX_LINE(*this);}
	double 			VERTEX_LINE::get_length	() const {return length;}
	double 			VERTEX_LINE::get_offset	() const {return offset;}
	
	void 	VERTEX_LINE::set_specs(double Length, double Offset){
		length 	= (Length >= 0.0)?Length: -Length;
		offset 	= (Offset <  0.0)?0.0: (Offset > 1.0)?1.0: Offset;
		
		VECTOR2D vertices[2] = {
			vector2d(-length * offset, 			0.0),
			vector2d( length * (1.0 - offset), 	0.0)
		};
		
		set_vertices(vertices, 2);
	}
	
	VERTEX_LINE& VERTEX_LINE::operator=(const VERTEX_LINE& Line){
		set_specs(Line.length, Line.offset);
		return *this;
	}
	
/* VERTEX_RECTANGLE */
	VERTEX_RECTANGLE::VERTEX_RECTANGLE(){ 
		set_specs(0.0, 0.0);
	}
	
	VERTEX_RECTANGLE::VERTEX_RECTANGLE(double Width, double Height){
		set_specs(Width, Height);
	}
	
	VERTEX_RECTANGLE::VERTEX_RECTANGLE(const VERTEX_RECTANGLE& Rect){
		set_specs(Rect.width, Rect.height);
	}
	
	VERTEX_ARRAY* 	VERTEX_RECTANGLE::clone		() const {return new VERTEX_RECTANGLE(*this);}
	double 			VERTEX_RECTANGLE::get_width	() const {return width;}
	double 			VERTEX_RECTANGLE::get_height() const {return height;}
	
	void 	VERTEX_RECTANGLE::set_specs(double Width, double Height){
		width 	= (Width  < 0.0)?0.0: Width;
		height 	= (Height < 0.0)?0.0: Height;
		
		VECTOR2D vertices[4] = {
			vector2d(-width/2.0, height/2.0),
			vector2d( width/2.0, height/2.0),
			vector2d( width/2.0,-height/2.0),
			vector2d(-width/2.0,-height/2.0)
		};
		
		set_vertices(vertices, 4);
	}
	
	VERTEX_RECTANGLE& VERTEX_RECTANGLE::operator=(const VERTEX_RECTANGLE& Rect){
		set_specs(Rect.width, Rect.height);
		return *this;
	}
	
/* VERTEX_CIRCLE */	
	VERTEX_CIRCLE::VERTEX_CIRCLE(){ 
		set_specs(0.0, 0);
	}
	
	VERTEX_CIRCLE::VERTEX_CIRCLE(double Radius, UINT Factor){
		set_specs(Radius, Factor);
	}
	
	VERTEX_CIRCLE::VERTEX_CIRCLE(const VERTEX_CIRCLE& Circle){
		set_specs(Circle.radius, Circle.factor);
	}
	
	VERTEX_ARRAY* 	VERTEX_CIRCLE::clone		() const {return new VERTEX_CIRCLE(*this);}
	double 			VERTEX_CIRCLE::get_radius	() const {return radius;}
	UINT 			VERTEX_CIRCLE::get_factor	() const {return factor;}
	
	void 	VERTEX_CIRCLE::set_specs(double Radius, UINT Factor){
		double 		angle;
		
		radius 	= (Radius < 0.0	)?0.0: Radius;
		factor 	= (Factor < 3	)?3	 : Factor;
		
		VERTEX_ARRAY::vertices_n = factor;
		VERTEX_ARRAY::alloc_vertices_buffer(factor);
		angle = 2*M_PI/factor;
		
		for(UINT i = 0; i < factor; i++)
			VERTEX_ARRAY::vertices[i] =  rotate_vector(create_normal(),angle * i) * radius;
	}
	
	VERTEX_CIRCLE& VERTEX_CIRCLE::operator=(const VERTEX_CIRCLE& Circle){
		set_specs(Circle.radius, Circle.factor);
		return *this;
	}
	
	
/* legacy =========================================================== */
/* class VECTOR_OBJECT */
VECTOR_OBJECT::VECTOR_OBJECT(){
	scale 		= 1.0;
	position	= vector2d(0.0, 0.0);
	normal 		= create_normal();
}

VECTOR_OBJECT::VECTOR_OBJECT(const VECTOR2D& Position, const VECTOR2D& Normal){
	scale 		= 1.0;
	position 	= Position;
	normal		= normalize_vector(Normal);
}

VECTOR_OBJECT::VECTOR_OBJECT(const VECTOR_OBJECT& Object){
	type 		= Object.type;
	scale 		= Object.scale;
	position 	= Object.position;
	normal		= Object.normal;
}


VECTOR_OBJECT::~VECTOR_OBJECT(){
	//printf("VECTOR_OBJECT destruct!\n");
}

UINT VECTOR_OBJECT::get_type() const{
	return type;
}

void VECTOR_OBJECT::set_position(const VECTOR2D& Position){
	position = Position;
}

VECTOR2D VECTOR_OBJECT::get_position() const{
	return position;
}

void VECTOR_OBJECT::set_normal(const VECTOR2D& Normal){
	normal		= normalize_vector(Normal);
}

VECTOR2D VECTOR_OBJECT::get_normal() const{
	return normal;
}

void VECTOR_OBJECT::rotate(double Angle){
	normal = rotate_vector(normal, Angle);
}

void VECTOR_OBJECT::rotate(const VECTOR2D& Rotator){
	normal = rotate_vector(normal, Rotator);
}

void VECTOR_OBJECT::move( const VECTOR2D& Vector){
	position += Vector;
}

void VECTOR_OBJECT::set_scale(double Scale){
	if(Scale >= 0.0001 && Scale <= 1000.0)
		scale = Scale;
}

double VECTOR_OBJECT::get_scale() const{
	return scale;
}


/* class VECTOR_POINT */

VECTOR_POINT::VECTOR_POINT(){
	type = VECTOR_OBJECT::T_POINT;
}

VECTOR_POINT::VECTOR_POINT(const VECTOR2D& Position, const VECTOR2D& Normale): 
VECTOR_OBJECT(Position, Normale){
	type = VECTOR_OBJECT::T_POINT;
}

VECTOR_POINT::VECTOR_POINT(const VECTOR_POINT& Point):
VECTOR_OBJECT((const VECTOR_OBJECT&)Point){
	type = VECTOR_OBJECT::T_POINT;
}

VECTOR_POINT::~VECTOR_POINT(){
	//printf("Point destruct!\n");
}

VECTOR_OBJECT* VECTOR_POINT::new_copy(){
	return new VECTOR_POINT(*this);
}

VECTOR_POINT& VECTOR_POINT::operator= (const VECTOR_POINT& Point){
	*((VECTOR_OBJECT*)this) = (const VECTOR_OBJECT&)Point;
	
	return *this;
}

/* class VECTOR_POLYLINE */

VECTOR_POLYLINE::VECTOR_POLYLINE(){
	type 				= VECTOR_OBJECT::T_POLYLINE;
	vertices_quantity	= MIN_VERTICES_QUANTITY;
	vertices 			= new VECTOR2D[vertices_quantity];
	
}

VECTOR_POLYLINE::VECTOR_POLYLINE(const VECTOR2D& Position, const VECTOR2D& Normale, UINT Quantity):
VECTOR_OBJECT(Position, Normale){
	type = VECTOR_OBJECT::T_POLYLINE;
	
	if(Quantity < MIN_VERTICES_QUANTITY){
		vertices_quantity = MIN_VERTICES_QUANTITY;
	}else
		vertices_quantity = Quantity;
	
	vertices 			= new VECTOR2D[vertices_quantity];
}

VECTOR_POLYLINE::VECTOR_POLYLINE(const VECTOR_POLYLINE& Polyline):
VECTOR_OBJECT((const VECTOR_OBJECT&) Polyline){
	type = VECTOR_OBJECT::T_POLYLINE;
	
	vertices_quantity 	= Polyline.vertices_quantity;
	vertices 			= new VECTOR2D[vertices_quantity];
	for(UINT ivertex = 0 ; ivertex < vertices_quantity; ivertex++)
		vertices[ivertex] = Polyline.vertices[ivertex];
}

VECTOR_POLYLINE::~VECTOR_POLYLINE(){
	delete [] vertices;
	//printf("VECTOR_POLYLINE destruct!\n");
}

VECTOR_OBJECT* VECTOR_POLYLINE::new_copy(){
	return new VECTOR_POLYLINE(*this);
}

VECTOR_POLYLINE& VECTOR_POLYLINE::operator= (const VECTOR_POLYLINE& Polyline ){
	*((VECTOR_OBJECT*)this) = (const VECTOR_OBJECT&)Polyline;
	
	delete [] vertices;
	
	vertices_quantity 	= Polyline.vertices_quantity;
	vertices 			= new VECTOR2D[vertices_quantity];
	for(UINT ivertex = 0 ; ivertex < vertices_quantity; ivertex++)
		vertices[ivertex] = Polyline.vertices[ivertex];
	
	return *this;
}

UINT VECTOR_POLYLINE::get_vertices_quantity() const{
	return vertices_quantity;
}


const VECTOR2D* VECTOR_POLYLINE::get_vertices() const{
	return vertices;
}	

RETCODE VECTOR_POLYLINE::get_vertex(UINT Index, VECTOR2D& R_vertex) const{
	if(Index >= vertices_quantity)
		return RET_IDETIFIER_INVALID;
	
	R_vertex = vertices[Index];
	
	return RET_SUCCESS;
}

RETCODE VECTOR_POLYLINE::set_vertex(UINT Index, const VECTOR2D& Vertex){
	if(Index >= vertices_quantity)
		return RET_IDETIFIER_INVALID;
	
	vertices[Index] = Vertex;
	
	return RET_SUCCESS;
}


/* class VECTOR_LINE realisation */

VECTOR_LINE::VECTOR_LINE(){
	type = VECTOR_OBJECT::T_LINE;
}

VECTOR_LINE::VECTOR_LINE(const VECTOR2D& Position ,const VECTOR2D& Normale, double Length, double Center_offset):
	VECTOR_POLYLINE(Position, Normale, VECTOR_POLYLINE::MIN_VERTICES_QUANTITY){
	type = VECTOR_OBJECT::T_LINE;
	
	if(Length < 0.0){
		length = 0.0;
	}else
		length = Length;
	
	if(Center_offset < -1.0){
		center_offset = -1.0;
	}else if(Center_offset > 1.0){
		center_offset = 1.0;
	}else
		center_offset = Center_offset;
	
	vertices[0] = vector2d(-(length * center_offset)		, 0.0);
	vertices[1] = vector2d( (length * (1.0 - center_offset)), 0.0);
}

VECTOR_LINE::VECTOR_LINE(const VECTOR_LINE& Line):
	VECTOR_POLYLINE((const VECTOR_POLYLINE&)Line){
	
	type 			= VECTOR_OBJECT::T_LINE;
	length 			= Line.length;
	center_offset 	= Line.center_offset; 
}

VECTOR_LINE::~VECTOR_LINE(){
	//printf("VECTOR_LINE destruct!\n");
}

VECTOR_OBJECT* VECTOR_LINE::new_copy(){
	return new VECTOR_LINE(*this);
}

VECTOR_LINE& VECTOR_LINE::operator= (const VECTOR_LINE& Line){
	*((VECTOR_POLYLINE*)this) = (const VECTOR_POLYLINE&)Line;
	type 			= VECTOR_OBJECT::T_LINE;
	length 			= Line.length;
	center_offset 	= Line.center_offset;
	return *this;
}

double VECTOR_LINE::get_length(){
	return length;
}

double VECTOR_LINE::get_center_offset(){
	return center_offset;
}

/* class VECTOR_SHAPE */

VECTOR_SHAPE::VECTOR_SHAPE(){
	type = VECTOR_OBJECT::T_SHAPE;
}

VECTOR_SHAPE::VECTOR_SHAPE(const VECTOR2D& Position, const VECTOR2D& Normale, UINT Quantity):
VECTOR_POLYLINE(Position, Normale, Quantity < MIN_VERTICES_QUANTITY?MIN_VERTICES_QUANTITY:Quantity){
	type = VECTOR_OBJECT::T_SHAPE;
}

VECTOR_SHAPE::VECTOR_SHAPE(const VECTOR_SHAPE& Shape):
VECTOR_POLYLINE((const VECTOR_POLYLINE&) Shape){
	type = VECTOR_OBJECT::T_SHAPE;
}

VECTOR_SHAPE::~VECTOR_SHAPE(){
	//printf("VECTOR_SHAPE destruct!\n");
}

VECTOR_OBJECT* VECTOR_SHAPE::new_copy(){
	return new VECTOR_SHAPE(*this);
}

VECTOR_SHAPE& VECTOR_SHAPE::operator= (const VECTOR_SHAPE& Shape){
	*((VECTOR_POLYLINE*)this) = (const VECTOR_POLYLINE&)Shape;
	return *this;
}


/* class VECTOR_RECTANGLE realisation */

VECTOR_RECTANGLE::VECTOR_RECTANGLE():
VECTOR_SHAPE(vector2d(0, 0), create_normal(), 4){
	type 	= VECTOR_OBJECT::T_RECTANGLE;
	width 	= 0;
	height 	= 0;
}

VECTOR_RECTANGLE::VECTOR_RECTANGLE(const VECTOR2D& Position, const VECTOR2D& Normale, double Width, double Height ):
VECTOR_SHAPE(Position, Normale, 4){
	type 	= VECTOR_OBJECT::T_RECTANGLE;
	width 	= std::abs(Width);
	height 	= std::abs(Height);
	
	vertices[0] = vector2d( -width/2, height/2);
	vertices[1] = vector2d(  width/2, height/2);
	vertices[2] = vector2d(  width/2,-height/2);
	vertices[3] = vector2d( -width/2,-height/2);
}

VECTOR_RECTANGLE::VECTOR_RECTANGLE(const VECTOR_RECTANGLE& Rectangle):
VECTOR_SHAPE((const VECTOR_SHAPE&) Rectangle){
	type 	= VECTOR_OBJECT::T_RECTANGLE;
	width	= Rectangle.width;
	height	= Rectangle.height;
}

VECTOR_RECTANGLE::~VECTOR_RECTANGLE(){
	//printf("VECTOR_RECTANGLE destruct!\n");
}

VECTOR_OBJECT* VECTOR_RECTANGLE::new_copy(){
	return new VECTOR_RECTANGLE(*this);
}

VECTOR_RECTANGLE& VECTOR_RECTANGLE::operator= (const VECTOR_RECTANGLE& Rectangle){
	*((VECTOR_SHAPE*)this) = (const VECTOR_SHAPE&)Rectangle;
	
	width	= Rectangle.width;
	height	= Rectangle.height;
	return *this;
}

double VECTOR_RECTANGLE::get_width(){
	return width;
}

double VECTOR_RECTANGLE::get_height(){
	return height;
}


/* class VECTOR_CIRCLE */

VECTOR_CIRCLE::VECTOR_CIRCLE(){
	type 	= VECTOR_OBJECT::T_CIRCLE;
	radius 	= 0;
}

VECTOR_CIRCLE::VECTOR_CIRCLE(const VECTOR2D& Position, const VECTOR2D& Normale, double Radius):
VECTOR_OBJECT(Position, Normale){
	type 	= VECTOR_OBJECT::T_CIRCLE;
	radius 	= std::abs(Radius);
}

VECTOR_CIRCLE::VECTOR_CIRCLE(const VECTOR_CIRCLE& Circle):
VECTOR_OBJECT((const VECTOR_OBJECT&)Circle){
	type 	= VECTOR_OBJECT::T_CIRCLE;
	radius 	= Circle.radius;
}

VECTOR_CIRCLE::~VECTOR_CIRCLE(){
	//printf("VECTOR_CIRCLE destruct!\n");
}

VECTOR_OBJECT* VECTOR_CIRCLE::new_copy(){
	return new VECTOR_CIRCLE(*this);
}

double VECTOR_CIRCLE::get_radius(){
	return radius;
}

/* class VECTOR_COMPOSITE */

VECTOR_COMPOSITE::VECTOR_COMPOSITE(){
	type 				= VECTOR_OBJECT::T_COMPOSITE;
	components_quantity = 0;
}

VECTOR_COMPOSITE::VECTOR_COMPOSITE(const VECTOR2D& Position, const VECTOR2D& Normale):
VECTOR_OBJECT(Position, Normale){
	type 				= VECTOR_OBJECT::T_COMPOSITE;
	components_quantity = 0;
}

VECTOR_COMPOSITE::VECTOR_COMPOSITE(const VECTOR_COMPOSITE& Composite ):
VECTOR_OBJECT((const VECTOR_OBJECT&)Composite){
	type 				= VECTOR_OBJECT::T_COMPOSITE;
	components_quantity = Composite.components_quantity;
	for(VECTOR_OBJECT* object: Composite.components)
		components.push_back(object->new_copy());
}

VECTOR_COMPOSITE::~VECTOR_COMPOSITE(){
	for(VECTOR_OBJECT* object: components)
		delete object;
}

VECTOR_OBJECT* VECTOR_COMPOSITE::new_copy(){
	return new VECTOR_COMPOSITE(*this);
}

VECTOR_COMPOSITE& VECTOR_COMPOSITE::operator= (const VECTOR_COMPOSITE& Composite){
	*((VECTOR_OBJECT*)this) = (const VECTOR_OBJECT&)Composite;
	
	components_quantity = Composite.components_quantity;
	for(VECTOR_OBJECT* object: Composite.components)
		components.push_back(object->new_copy());
	
	return *this;
}

UINT VECTOR_COMPOSITE::get_components_quantity(){
	return components_quantity;
}

void VECTOR_COMPOSITE::push_component(VECTOR_OBJECT* Object){
	if(Object != nullptr){
		components.push_back(Object->new_copy());
		components_quantity++;
	}
}

const std::list<VECTOR_OBJECT*>& VECTOR_COMPOSITE::get_components(){
	return (const std::list<VECTOR_OBJECT*>&)components;
}
