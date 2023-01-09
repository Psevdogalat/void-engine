#include <utilits.h>
#include <graphics.h>

using namespace ENGINE;

GRAPHIC_MODEL* create_arrow_model(){
	GRAPHIC_MODEL* 	r_model;

	VECTOR2D vertices[] = {
		vector2d(0.0, 0.0),
		vector2d(0.9, 0.0),
		vector2d(0.9, 0.1),
		vector2d(1.0, 0.0),
		vector2d(0.9,-0.1),
		vector2d(0.9, 0.0)
	};
	
	GELEMENT_POLYLINE* polyline = new GELEMENT_POLYLINE();
	polyline->set_vertices(vertices, sizeof(vertices)/sizeof(VECTOR2D));
	r_model = new GRAPHIC_MODEL();
	r_model->push_back(polyline);
	return r_model;
}

GRAPHIC_MODEL* create_point_model(){
	GRAPHIC_MODEL* 		r_model;
	GELEMENT_LINE*	line;
	GELEMENT_CIRCLE* 	circle;

	r_model = new GRAPHIC_MODEL();
	circle 	= new GELEMENT_CIRCLE();
	circle->set_specs(0.5, 32);
	r_model->push_back(circle);
	
	line = new GELEMENT_LINE();
	line->set_specs(1.0, 0.5);
	line->set_normal(rotate_vector(create_normal(), M_PI/4));
	r_model->push_back(line->clone());
	line->set_normal(rotate_vector(create_normal(),-M_PI/4));
	r_model->push_back(line);
	
	return r_model;
}


GRAPHIC_MODEL* create_grid_model(double Width, double Height){
	GRAPHIC_MODEL* model;
	
	UINT w_count;
	UINT h_count;
	static const double delta = 0.2; 
	
	RGB_COLOR	lo_color(0.2f, 0.2f, 0.2f);
	RGB_COLOR	hi_color(1.0f, 1.0f, 1.0f);
	
	GELEMENT_LINE 		line;
	GELEMENT_LINEGROUP* group1;
	GELEMENT_LINEGROUP* group2;
	
	group1 = new GELEMENT_LINEGROUP();
	group1->set_color(lo_color);
	
	group2 = new GELEMENT_LINEGROUP();
	group2->set_color(hi_color);
	
	model = new GRAPHIC_MODEL();
	model->push_back(group1);
	model->push_back(group2);
	
	line.set_position	(vector2d(0.0, 0.0));
	
	line.set_normal		(vector2d(0.0, 1.0));
	line.set_specs		(Height	, 0.5);
	group1->push_back(new GELEMENT_LINE(line));
	
	line.set_normal		(vector2d(1.0, 0.0));
	line.set_specs		(Width	, 0.5);
	group1->push_back(new GELEMENT_LINE(line));
	
	w_count = ((UINT)(Width /delta))/2;
	h_count = ((UINT)(Height/delta))/2;
	
	line.set_normal	(vector2d(0.0, 1.0));
	for(UINT i = 1; i <= w_count; i++){
		if(i%5 == 0){
			line.set_specs		(Height	, 0.5);
			
			line.set_position	(vector2d( delta*(double)i, 0.0));
			group1->push_back(new GELEMENT_LINE(line));
			line.set_position	(vector2d(-delta*(double)i, 0.0));
			group1->push_back(new GELEMENT_LINE(line));
			
			line.set_specs		(2*delta, 0.5);
			
			line.set_position	(vector2d( delta*(double)i, 0.0));
			group2->push_back(new GELEMENT_LINE(line));
			line.set_position	(vector2d(-delta*(double)i, 0.0));
			group2->push_back(new GELEMENT_LINE(line));
			
		}else{
			line.set_specs		(delta, 0.5);
			
			line.set_position	(vector2d( delta*(double)i, 0.0));
			group2->push_back(new GELEMENT_LINE(line));
			line.set_position	(vector2d(-delta*(double)i, 0.0));
			group2->push_back(new GELEMENT_LINE(line));
		}
	}
	
	line.set_normal	(vector2d(1.0, 0.0));
	for(UINT i = 1; i <= h_count; i++){
		if(i%5 == 0){
			line.set_specs		(Width, 0.5);
			
			line.set_position	(vector2d(0.0,  delta*(double)i));
			group1->push_back(new GELEMENT_LINE(line));
			line.set_position	(vector2d(0.0, -delta*(double)i));
			group1->push_back(new GELEMENT_LINE(line));
			
			line.set_specs		(2*delta, 0.5);
			
			line.set_position	(vector2d(0.0,  delta*(double)i));
			group2->push_back(new GELEMENT_LINE(line));
			line.set_position	(vector2d(0.0, -delta*(double)i));
			group2->push_back(new GELEMENT_LINE(line));
			
		}else{
			line.set_specs		(delta, 0.5);
			
			line.set_position	(vector2d(0.0,  delta*(double)i));
			group2->push_back(new GELEMENT_LINE(line));
			line.set_position	(vector2d(0.0, -delta*(double)i));
			group2->push_back(new GELEMENT_LINE(line));

		}
	}
	
	return model;
}

GRAPHIC_MODEL* create_base_unit_model(){
	GRAPHIC_MODEL* 		r_model;
	GELEMENT_RECT* 		rectangle;
	GELEMENT_CIRCLE*	circle;		
	GELEMENT_LINE*		line;				
	
	r_model = new GRAPHIC_MODEL();
	
	/*
	rectangle 	= new GELEMENT_RECT();
	rectangle->set_specs(1.0, 1.0);
	
	circle 		= new GELEMENT_CIRCLE();
	circle->set_specs(0.5, 32);
	
	line 		= new GELEMENT_LINE();
	line->set_position(vector2d(0.5, 0.0));
	line->set_specs(0.2 , 0.0);
	
	r_model->push_back(rectangle);
	r_model->push_back(circle);
	r_model->push_back(line);
	*/
	
	GELEMENT_SPRITE* sprite = new GELEMENT_SPRITE();
	sprite->set_normal	(rotate_vector(create_normal(), -M_PI/2));
	sprite->set_specs	(1.0, 1.2);
	sprite->set_texture	("tank_2", 500, 500);
	sprite->set_window	(0,0, 285, 285);
	r_model->push_back(sprite);
	
	return r_model;
}
	
GRAPHIC_MODEL* create_bullet_model(){
	GRAPHIC_MODEL* 		r_model;
	GELEMENT_CIRCLE*	circle;
	
	r_model = new GRAPHIC_MODEL();
	
	/*
	circle 		= new GELEMENT_CIRCLE();
	circle->set_specs(0.1, 32);		
	r_model->push_back(circle);
	*/
	
	GELEMENT_SPRITE* sprite = new GELEMENT_SPRITE();
	sprite->set_normal	(rotate_vector(create_normal(), -M_PI/2));
	sprite->set_specs	(0.2, 0.6);
	sprite->set_texture	("tank_2", 500, 500);
	sprite->set_window	(123,370, 35, 100);
	r_model->push_back(sprite);
	
	r_model->set_color(RGB_COLOR(0.5f, 0.0f, 0.8f));
	return r_model;
}


void scale_vertices(VECTOR_POLYLINE* Polyline, const VECTOR2D& Scale){
	VECTOR2D Vertex;
	for(UINT ivertex = 0; ivertex < Polyline->get_vertices_quantity(); ivertex++){
		Polyline->get_vertex(ivertex,Vertex);
		Vertex.x *= Scale.x;
		Vertex.y *= Scale.y;
		Polyline->set_vertex(ivertex,Vertex);
	}
}

/*
GRAPHIC_MODEL_VECTOR create_wall_model_1(const VECTOR2D& Scale){
	GRAPHIC_MODEL_VECTOR r_model;
	VECTOR_RECTANGLE rectangle(vector2d(0,0)	, create_normal(), Scale.x, Scale.y);
	
	r_model.set_vector_object(&rectangle);
	return r_model;
}


GRAPHIC_MODEL_VECTOR create_wall_model_2(const VECTOR2D& Scale){
	GRAPHIC_MODEL_VECTOR r_model;
	VECTOR_SHAPE shape(vector2d(0,0)	, create_normal(), 8);
	
	shape.set_vertex(0,vector2d(-0.5, 0.5));
	shape.set_vertex(1,vector2d( 0.5, 0.5));
	shape.set_vertex(2,vector2d( 0.4, 0.4));
	shape.set_vertex(3,vector2d(-0.4, 0.4));
	shape.set_vertex(4,vector2d(-0.4,-0.4));
	shape.set_vertex(5,vector2d( 0.4,-0.4));
	shape.set_vertex(6,vector2d( 0.5,-0.5));
	shape.set_vertex(7,vector2d(-0.5,-0.5));
	
	scale_vertices(&shape, Scale);
	
	r_model.set_vector_object(&shape);
	
	return r_model;
}

GRAPHIC_MODEL_VECTOR create_wall_model_3(const VECTOR2D& Scale){
	GRAPHIC_MODEL_VECTOR r_model;
	VECTOR_SHAPE shape(vector2d(0,0)	, create_normal(), 8);
	
	shape.set_vertex(0,vector2d(-0.5, 0.5));
	shape.set_vertex(1,vector2d(   0, 0.2));
	shape.set_vertex(2,vector2d( 0.5, 0.5));
	shape.set_vertex(3,vector2d( 0.2,   0));
	shape.set_vertex(4,vector2d( 0.5,-0.5));
	shape.set_vertex(5,vector2d(   0,-0.2));
	shape.set_vertex(6,vector2d(-0.5,-0.5));
	shape.set_vertex(7,vector2d(-0.2,   0));
	
	scale_vertices(&shape, Scale);
	
	r_model.set_vector_object(&shape);
	
	return r_model;
	
}


GRAPHIC_MODEL_VECTOR create_wall_model_4(const VECTOR2D& Scale){
	GRAPHIC_MODEL_VECTOR r_model;
	VECTOR_SHAPE shape(vector2d(0,0)	, create_normal(), 11);
	
	shape.set_vertex(0 ,vector2d(-0.5, 0.2));
	shape.set_vertex(1 ,vector2d(-0.4, 0.5));
	shape.set_vertex(2 ,vector2d(-0.1, 0.3));
	shape.set_vertex(3 ,vector2d( 0.0, 0.5));
	shape.set_vertex(4 ,vector2d( 0.3, 0.3));
	shape.set_vertex(5 ,vector2d( 0.5, 0.5));
	shape.set_vertex(6 ,vector2d( 0.4, 0.0));
	shape.set_vertex(7 ,vector2d( 0.5,-0.4));
	shape.set_vertex(8 ,vector2d( 0.0,-0.5));
	shape.set_vertex(9 ,vector2d(-0.5,-0.4));
	shape.set_vertex(10,vector2d(-0.3,-0.1));
	
	scale_vertices(&shape, Scale);
	
	r_model.set_vector_object(&shape);
	
	return r_model;
}
*/