#include <geometry.hpp>
#include <geometry2.h>

#include <cstdio>

/* Vector2d methods realisation */
Vector2d operator+(const Vector2d & v1, const Vector2d & v2)
{
	return add2d(v1, v2);
};

Vector2d operator-(const Vector2d & v1, const Vector2d & v2)
{
	return sub2d(v1, v2);
};

Vector2d operator- (const Vector2d & v1)
{
	return neg2d(v1);
};

Vector2d & operator+=(Vector2d & v1, const Vector2d & v2)
{
	v1 = add2d(v1, v2);
	return v1;
};

Vector2d & operator-=(Vector2d & v1, const Vector2d & v2){
	v1 = sub2d(v1, v2);
	return v1;
};

Vector2d  operator* (const Vector2d & v1, double multiplier)
{
	return scalarScale2d(v1, multiplier);
};

Vector2d operator* (double multiplier, const Vector2d & v)
{
	return scalarScale2d(multiplier, v1);
};

Vector2d operator* (const Vector2d & v1, const Vector2d & v2)
{
	return scl2d(v1, v2);
};

Vector2d  operator/ (const Vector2d & vd1, double d2)
{
	return vector2d(vd1.x/d2, vd1.y/d2);
};

Vector2d operator/ (double d1, const Vector2d & vd2 )
{
	return vector2d(d1/vd2.x, d1/vd2.y);
};

Vector2d & operator*=(Vector2d & v1, double multiplier)
{
	v1 = scalarScale2d(v1, multiplier);	
	return v1;
};

Vector2d & operator/=(Vector2d & v1, double d2)
{
	v1.x /= d1;
	v1.y /= d1;
	return v1;
};

bool operator==(const Vector2d & v1, const Vector2d & v2)
{
	if((v1.x == v2.x) && (v1.y == v2.y))
		return true;
	
	return false;
};

bool operator!=(const Vector2d & v1, const Vector2d & v2)
{
	if(	v1 == v2)
		return false;
	
	return true;
};

Vector2d operator* (const Matrix33d & m1, const Vector2d & v2)
{
	return mulM33V2d(m1, v2);	
};

Matrix33d operator*	(const Matrix33di & m1, const Matrix33d & m2)
{
		return mulM33d(m1, m2);
};

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
	const Vector2d& Position, 
	const Vector2d& Normal, 
	const Vector2d&	Scale
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
	const Vector2d& Position
)
{
	position = Position; 	
}

void 	 
TRNSF_OBJECT::set_normal	
(
	const Vector2d& Normal
)	
{
	normal = normalize_vector(Normal); 
}

void 	 
TRNSF_OBJECT::set_scale	
(
	const Vector2d& Scale
)
{
	scale = Scale; 	
}

Vector2d 
TRNSF_OBJECT::get_position	
() const 
{
	return position; 	
}

Vector2d 
TRNSF_OBJECT::get_normal	
() const 
{
	return normal; 	
}

Vector2d 
TRNSF_OBJECT::get_scale	
() const 
{
	return scale; 	
}

Matrix33d 
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

Matrix33d 
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
	
	VERTEX_ARRAY::VERTEX_ARRAY(const Vector2d* Vertices, UINT Vertices_n){
		vertices	= nullptr;
		vertices_n 	= 0;
		buffer_len 	= 0;
		
		set_vertices(Vertices, Vertices_n);
	}
	
	VERTEX_ARRAY::VERTEX_ARRAY(const Vector2d& Vertex, UINT Vertices_n){
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
			
			vertices = new Vector2d[buffer_len];
		}
	}
	
	VERTEX_ARRAY* 	VERTEX_ARRAY::clone			() const {return new VERTEX_ARRAY(*this);}
	UINT 			VERTEX_ARRAY::get_vertices_n() const {return vertices_n;}
	const Vector2d*	VERTEX_ARRAY::get_vertices	() const {return (const Vector2d*) vertices;}
	
	void VERTEX_ARRAY::set_vertices(const Vector2d* Vertices, UINT Vertices_n){
		alloc_vertices_buffer(Vertices_n);
		
		vertices_n = Vertices_n;
		if(vertices != nullptr && Vertices != nullptr)
			for(UINT i = vertices_n; i--; )
				vertices[i] = Vertices[i];
			
	}
	
	void VERTEX_ARRAY::set_vertices(const Vector2d& Vertex, UINT Vertices_n){
		alloc_vertices_buffer(Vertices_n);
		
		vertices_n = Vertices_n;
		if(vertices != nullptr)
			for(UINT i = vertices_n; i--; )
				vertices[i] = Vertex;
			
	}
	
	void VERTEX_ARRAY::set_vertex(const Vector2d& Vertex, UINT Index){
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
		
		Vector2d vertices[2] = {
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
		
		Vector2d vertices[4] = {
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

VECTOR_OBJECT::VECTOR_OBJECT(const Vector2d& Position, const Vector2d& Normal){
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

void VECTOR_OBJECT::set_position(const Vector2d& Position){
	position = Position;
}

Vector2d VECTOR_OBJECT::get_position() const{
	return position;
}

void VECTOR_OBJECT::set_normal(const Vector2d& Normal){
	normal		= normalize_vector(Normal);
}

Vector2d VECTOR_OBJECT::get_normal() const{
	return normal;
}

void VECTOR_OBJECT::rotate(double Angle){
	normal = rotate_vector(normal, Angle);
}

void VECTOR_OBJECT::rotate(const Vector2d& Rotator){
	normal = rotate_vector(normal, Rotator);
}

void VECTOR_OBJECT::move( const Vector2d& Vector){
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

VECTOR_POINT::VECTOR_POINT(const Vector2d& Position, const Vector2d& Normale): 
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
	vertices 			= new Vector2d[vertices_quantity];
	
}

VECTOR_POLYLINE::VECTOR_POLYLINE(const Vector2d& Position, const Vector2d& Normale, UINT Quantity):
VECTOR_OBJECT(Position, Normale){
	type = VECTOR_OBJECT::T_POLYLINE;
	
	if(Quantity < MIN_VERTICES_QUANTITY){
		vertices_quantity = MIN_VERTICES_QUANTITY;
	}else
		vertices_quantity = Quantity;
	
	vertices 			= new Vector2d[vertices_quantity];
}

VECTOR_POLYLINE::VECTOR_POLYLINE(const VECTOR_POLYLINE& Polyline):
VECTOR_OBJECT((const VECTOR_OBJECT&) Polyline){
	type = VECTOR_OBJECT::T_POLYLINE;
	
	vertices_quantity 	= Polyline.vertices_quantity;
	vertices 			= new Vector2d[vertices_quantity];
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
	vertices 			= new Vector2d[vertices_quantity];
	for(UINT ivertex = 0 ; ivertex < vertices_quantity; ivertex++)
		vertices[ivertex] = Polyline.vertices[ivertex];
	
	return *this;
}

UINT VECTOR_POLYLINE::get_vertices_quantity() const{
	return vertices_quantity;
}


const Vector2d* VECTOR_POLYLINE::get_vertices() const{
	return vertices;
}	

RETCODE VECTOR_POLYLINE::get_vertex(UINT Index, Vector2d& R_vertex) const{
	if(Index >= vertices_quantity)
		return RET_IDETIFIER_INVALID;
	
	R_vertex = vertices[Index];
	
	return RET_SUCCESS;
}

RETCODE VECTOR_POLYLINE::set_vertex(UINT Index, const Vector2d& Vertex){
	if(Index >= vertices_quantity)
		return RET_IDETIFIER_INVALID;
	
	vertices[Index] = Vertex;
	
	return RET_SUCCESS;
}


/* class VECTOR_LINE realisation */

VECTOR_LINE::VECTOR_LINE(){
	type = VECTOR_OBJECT::T_LINE;
}

VECTOR_LINE::VECTOR_LINE(const Vector2d& Position ,const Vector2d& Normale, double Length, double Center_offset):
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

VECTOR_SHAPE::VECTOR_SHAPE(const Vector2d& Position, const Vector2d& Normale, UINT Quantity):
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

VECTOR_RECTANGLE::VECTOR_RECTANGLE(const Vector2d& Position, const Vector2d& Normale, double Width, double Height ):
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

VECTOR_CIRCLE::VECTOR_CIRCLE(const Vector2d& Position, const Vector2d& Normale, double Radius):
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

VECTOR_COMPOSITE::VECTOR_COMPOSITE(const Vector2d& Position, const Vector2d& Normale):
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
