#include <graphics.h>
#include <cstdio>

using namespace ENGINE;	
		
/* RGB_COLOR */ 
	RGB_COLOR::RGB_COLOR(){
		r = 0.0;
		g = 0.0;
		b = 0.0;
	}

	RGB_COLOR::RGB_COLOR(float R, float G, float B){
		r = R;
		g = G;
		b = B;
	}

	RGB_COLOR::RGB_COLOR(const RGB_COLOR& Color){
		r = Color.r;
		g = Color.g;
		b = Color.b;
	}

	RGB_COLOR& RGB_COLOR::operator= (const RGB_COLOR& Color){
		r = Color.r;
		g = Color.g;
		b = Color.b;
		return *this;
	}

/* COLORED_OBJECT */ 
	COLORED_OBJECT::COLORED_OBJECT	(){color = RGB_COLOR(1.0f, 1.0f, 1.0f);}
	COLORED_OBJECT::COLORED_OBJECT	(const COLORED_OBJECT& Smpl){
		color = Smpl.color;
	}
	
	COLORED_OBJECT::~COLORED_OBJECT	(){}
	
	void COLORED_OBJECT::set_color(const RGB_COLOR& Color){
		color = Color;
	}
	
const RGB_COLOR& COLORED_OBJECT::get_color() const {
	return color;
}

/* GELEMENT */
GELEMENT::GELEMENT	(){	
	type 		= GELEMENT_TVOID;
}

GELEMENT::GELEMENT(const GELEMENT& Smpl):
	TRNSF_OBJECT(Smpl), COLORED_OBJECT(Smpl){
	type		= Smpl.type;
}

UINT 			GELEMENT::get_type		() const {return type;};

GELEMENT&		GELEMENT::operator=		(const GELEMENT& Smpl){
	position 	= Smpl.position;
	normal		= Smpl.normal;
	scale		= Smpl.scale;
	return *this;
}
				

/* GELEMENT_GROUP */
/* ========================================================================== */
GELEMENT_GROUP::GELEMENT_GROUP	
()
{
    type = GELEMENT_TGROUP;
};

GELEMENT_GROUP::GELEMENT_GROUP
(
    const GELEMENT_GROUP& Smpl
):GELEMENT(Smpl)
{
	for(GELEMENT* sub_primitive: Smpl)
		push_back(sub_primitive->clone());
}

GELEMENT_GROUP::~GELEMENT_GROUP	(){
    clear();
}

GELEMENT* GELEMENT_GROUP::clone() const{
	return new GELEMENT_GROUP(*this);
}

size_t
GELEMENT_GROUP::size
() const
{
    return list.size();
}

GELEMENT_GROUP::const_iterator
GELEMENT_GROUP::begin
() const
{
    return list.begin();
}

GELEMENT_GROUP::const_iterator
GELEMENT_GROUP::end
() const
{
    return list.end();
}   

void 
GELEMENT_GROUP::push_back
(
    GELEMENT* Element
)
{
    list.push_back(Element);
}

void
GELEMENT_GROUP::clear
()
{
    for(GELEMENT* Element: list)
        delete Element;

    list.clear();
}

	
/* GELEMENT_LINE */
/*============================================================================*/
	GELEMENT_LINE::GELEMENT_LINE(){type = GELEMENT_TLINE;}
	
	GELEMENT_LINE::GELEMENT_LINE(const GELEMENT_LINE& Smpl):
	GELEMENT(Smpl){
		line = Smpl.line;
	}
	
	GELEMENT_LINE::~GELEMENT_LINE(){}
	
	GELEMENT* GELEMENT_LINE::clone() const{
		return new GELEMENT_LINE(*this);
	}
	
	void GELEMENT_LINE::set_specs(double Length, double Offset){
		line.set_specs(Length, Offset);
	}
	
	void 
	GELEMENT_LINE::set_specs
	(
		const VECTOR2D& V1,  
		const VECTOR2D& V2
	){
		VECTOR2D 	line_vector;
		double 		length;
		
		line_vector = V2 - V1;
		length = vector_length(line_vector);
		
		line.set_specs(length, 0.0);
		
		set_position(V1);
		set_normal(normalize_vector(line_vector));
		
	}
	
	UINT GELEMENT_LINE::get_vertices_n() const{
		return line.get_vertices_n();
	}

	const VECTOR2D* GELEMENT_LINE::get_vertices() const{
		return line.get_vertices();
	}
	
/* GELEMENT_POLYLINE */
/*============================================================================*/
	GELEMENT_POLYLINE::GELEMENT_POLYLINE(){type = GELEMENT_TPOLYLINE;}
	GELEMENT_POLYLINE::GELEMENT_POLYLINE(const GELEMENT_POLYLINE& Smpl):
		GELEMENT(Smpl){
		vertices = Smpl.vertices;
	}
	
	GELEMENT_POLYLINE::~GELEMENT_POLYLINE(){}
	
	GELEMENT* GELEMENT_POLYLINE::clone() const{
		return new GELEMENT_POLYLINE(*this);
	}
	
	UINT GELEMENT_POLYLINE::get_vertices_n() const{
		return vertices.get_vertices_n();
	}
	
	const VECTOR2D* GELEMENT_POLYLINE::get_vertices() const{
		return vertices.get_vertices();
	}
	
	void GELEMENT_POLYLINE::set_vertices(const VECTOR2D* Vertices, UINT Vertices_n){
		vertices.set_vertices(Vertices, Vertices_n);
	}
	
	void GELEMENT_POLYLINE::set_vertex(const VECTOR2D& Vertex, UINT Index){
		vertices.set_vertex(Vertex, Index);
	}

/* GELEMENT_SHAPE */
/*============================================================================*/
	GELEMENT_SHAPE::GELEMENT_SHAPE(){type = GELEMENT_TSHAPE;}
	
	GELEMENT_SHAPE::GELEMENT_SHAPE(const GELEMENT_SHAPE& Smpl):
	GELEMENT(Smpl){
		vertices = Smpl.vertices;
	}
	
	GELEMENT_SHAPE::~GELEMENT_SHAPE(){}
	
	GELEMENT* GELEMENT_SHAPE::clone() const{
		return new GELEMENT_SHAPE(*this);
	}

	UINT GELEMENT_SHAPE::get_vertices_n() const{
		return vertices.get_vertices_n();
	}
	
	const VECTOR2D* GELEMENT_SHAPE::get_vertices() const{
		return vertices.get_vertices();
	}
	
	void GELEMENT_SHAPE::set_vertices(const VECTOR2D* Vertices, UINT Vertices_n){
		vertices.set_vertices(Vertices, Vertices_n);
	}
	
	void GELEMENT_SHAPE::set_vertex(const VECTOR2D& Vertex, UINT Index){
		vertices.set_vertex(Vertex, Index);
	}

/* GELEMENT_RECT */ 
/*============================================================================*/
	GELEMENT_RECT::GELEMENT_RECT(){
		type 	= GELEMENT_TRECT;
		solid 	= false;
	}
	
	GELEMENT_RECT::GELEMENT_RECT(const GELEMENT_RECT& Smpl):
	GELEMENT(Smpl){
		rectangle 	= Smpl.rectangle;
		solid		= Smpl.solid;
	}
	
	GELEMENT_RECT::~GELEMENT_RECT(){}
	
	GELEMENT* GELEMENT_RECT::clone() const{
		return new GELEMENT_RECT(*this);
	}
	
	void GELEMENT_RECT::set_specs	(double Width, double Height){
		rectangle.set_specs(Width, Height);
	}
	
	UINT GELEMENT_RECT::get_vertices_n() const{
		return rectangle.get_vertices_n();
	}
	
	const VECTOR2D* GELEMENT_RECT::get_vertices() const{
		return rectangle.get_vertices();
	}
	
	void GELEMENT_RECT::set_solid	(bool Solid){ solid = Solid;}
	bool GELEMENT_RECT::is_solid	() const	{ return solid;}
	
/* GELEMENT_CIRCLE */
	GELEMENT_CIRCLE::GELEMENT_CIRCLE(){type = GELEMENT_TCIRCLE;}
	
	GELEMENT_CIRCLE::GELEMENT_CIRCLE(const GELEMENT_CIRCLE& Smpl):
	GELEMENT(Smpl){
		circle = Smpl.circle;
	}
	
	GELEMENT_CIRCLE::~GELEMENT_CIRCLE(){}
	
	GELEMENT* GELEMENT_CIRCLE::clone() const{
		return new GELEMENT_CIRCLE(*this);
	}
	
	void GELEMENT_CIRCLE::set_specs(double Radius, UINT Factor){
		circle.set_specs(Radius, Factor);
	}
	
	UINT GELEMENT_CIRCLE::get_vertices_n() const{
		return circle.get_vertices_n();
	}
	
	const VECTOR2D* GELEMENT_CIRCLE::get_vertices()const{
		return circle.get_vertices();
	}
	
/* GELEMENT_SPRITE */
/*============================================================================*/
	GELEMENT_SPRITE::GELEMENT_SPRITE(){
		type 			= GELEMENT_TSPRITE;
		texture_name 	= "";
		texture_width	= 1;
		texture_height	= 1;
		
		for(UINT i = 4; i--; )
			texture_coords[i] = vector2d(0.0, 0.0);
	}
	
	GELEMENT_SPRITE::GELEMENT_SPRITE(const GELEMENT_SPRITE& Smpl):
	GELEMENT(Smpl){
		rectangle 		= Smpl.rectangle;
		texture_name 	= Smpl.texture_name;
		texture_width	= Smpl.texture_width;
		texture_height	= Smpl.texture_height;
		for(UINT i = 4;i--;)
			texture_coords[i] = Smpl.texture_coords[i];
	}
	
	GELEMENT_SPRITE::~GELEMENT_SPRITE(){}
	
	GELEMENT* GELEMENT_SPRITE::clone() const{
		return new GELEMENT_SPRITE(*this);
	}
	
	void GELEMENT_SPRITE::set_specs(double Width, double Height){
		rectangle.set_specs(Width, Height);
	}
	
	void GELEMENT_SPRITE::set_texture (const std::string& Name, UINT Width, UINT Height){
		texture_name 	= Name;
		texture_width	= (Width  > 0)?Width : 1;
		texture_height	= (Height > 0)?Height: 1;
	}
	
	void GELEMENT_SPRITE::set_window(UINT X, UINT Y, UINT Width, UINT Height){
		UINT width, height;
		width  = (Width  > 0)?Width : 1;
		height = (Height > 0)?Height: 1;
		double tex_cof_x = 1.0/(double)texture_width;
		double tex_cof_y = 1.0/(double)texture_height;
		texture_coords[0] = vector2d((X 		)*tex_cof_x, (Y+height	)*tex_cof_y);
		texture_coords[1] =	vector2d((X+width 	)*tex_cof_x, (Y+height	)*tex_cof_y);
		texture_coords[2] =	vector2d((X+width 	)*tex_cof_x, (Y		  	)*tex_cof_y);
		texture_coords[3] =	vector2d((X		  	)*tex_cof_x, (Y		  	)*tex_cof_y);
	}
	
	const VECTOR2D*	GELEMENT_SPRITE::get_tcoords	 () const{
		return (const VECTOR2D*) texture_coords;
	}
	
	UINT			GELEMENT_SPRITE::get_tcoords_n () const{
		return 4;
	}
	
	UINT 			GELEMENT_SPRITE::get_vertices_n() const{
		return rectangle.get_vertices_n();
	}
	
	const VECTOR2D* GELEMENT_SPRITE::get_vertices	 () const{
		return rectangle.get_vertices();
	}
	
	const std::string& GELEMENT_SPRITE::get_texture_name() const{
		return texture_name;
	}
	
/* GELEMENT_LINEGROUP */
/*============================================================================*/		
	GELEMENT_LINEGROUP::GELEMENT_LINEGROUP
	()
	{type = GELEMENT_TLINEGROUP; }
	
	GELEMENT_LINEGROUP::GELEMENT_LINEGROUP
	(const GELEMENT_LINEGROUP& Smpl):
	GELEMENT(Smpl)
	{ 
		for(GELEMENT_LINE* line:Smpl)
			lines.push_back(new GELEMENT_LINE(*line));
	}
		
	GELEMENT_LINEGROUP::~GELEMENT_LINEGROUP
	()
	{
		for(GELEMENT_LINE* line:lines)
			delete line;
	}
	
	GELEMENT*
	GELEMENT_LINEGROUP::clone
	() const
	{return new GELEMENT_LINEGROUP(*this); }
	
	size_t
	GELEMENT_LINEGROUP::size
	() const
	{return lines.size(); }
	
	GELEMENT_LINEGROUP::const_iterator
	GELEMENT_LINEGROUP::begin
	() const
	{return lines.begin(); }
	
	GELEMENT_LINEGROUP::const_iterator
	GELEMENT_LINEGROUP::end
	() const
	{return lines.end(); }
	
	void
	GELEMENT_LINEGROUP::push_back
	(GELEMENT_LINE* Line)
	{lines.push_back(Line); }

    void GELEMENT_LINEGROUP::clear
    ()
    {
        for(GELEMENT_LINE* line: lines)
            delete line;

        lines.clear();
    }
	
/* GELEMENT_RECTGROUP */
/*============================================================================*/		
	GELEMENT_RECTGROUP::GELEMENT_RECTGROUP
	()
	{
		type 	= GELEMENT_TRECTGROUP; 
		solid 	= false;
	}
	
	GELEMENT_RECTGROUP::GELEMENT_RECTGROUP
	(const GELEMENT_RECTGROUP& Smpl):
	GELEMENT(Smpl)
	{ 
		for(GELEMENT_RECT* rect:Smpl)
			rects.push_back(new GELEMENT_RECT(*rect));
		
		solid = Smpl.solid;
	}
		
	GELEMENT_RECTGROUP::~GELEMENT_RECTGROUP
	()
	{
		for(GELEMENT_RECT* rect:rects)
			delete rect;
	}
	
	GELEMENT*
	GELEMENT_RECTGROUP::clone
	() const
	{return new GELEMENT_RECTGROUP(*this); }
	
	size_t
	GELEMENT_RECTGROUP::size
	() const
	{return rects.size(); }
	
	GELEMENT_RECTGROUP::const_iterator
	GELEMENT_RECTGROUP::begin
	() const
	{return rects.begin(); }
	
	GELEMENT_RECTGROUP::const_iterator
	GELEMENT_RECTGROUP::end
	() const
	{return rects.end(); }
	
	void
	GELEMENT_RECTGROUP::push_back
	(GELEMENT_RECT* Rect)
	{rects.push_back(Rect); }
	
	void 
	GELEMENT_RECTGROUP::set_solid	
	(bool Solid)
	{ solid = Solid;}
	
	bool 
	GELEMENT_RECTGROUP::is_solid	
	() const	
	{ return solid;}

    void 
    GELEMENT_RECTGROUP::clear
    ()
    {
        for(GELEMENT_RECT* rect: rects)
            delete rect;

        rects.clear();
    }
/* GRAPHIC_MODEL */
/*============================================================================*/
	GRAPHIC_MODEL::GRAPHIC_MODEL
	()
	{layer = GLAYER_DEFAULT; }
	
	GRAPHIC_MODEL::GRAPHIC_MODEL
	(const GRAPHIC_MODEL& Smpl):
	GELEMENT_GROUP(Smpl)
	{ layer = Smpl.layer;}
	
	GRAPHIC_MODEL::~GRAPHIC_MODEL(){}
	
	UINT
	GRAPHIC_MODEL::get_layer
	()
	{return layer; }
	
	void 
	GRAPHIC_MODEL::set_layer
	(UINT Layer)
	{layer = (Layer >= GLAYERS_NUM)?GLAYERS_NUM-1:Layer; } 
	
	
