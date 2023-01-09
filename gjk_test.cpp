#include <engine.h>

#include <objects.h>
#include <utilits.h>
#include <keys.h>
#include <controllers.h>

#include <vector_utilits.h>
#include <collisions2.h>
#include <uibe.h>

#include <cstdlib>
#include <cstdio>
#include <list>
#include <cmath>

using namespace ENGINE;
  
GAME_OBJECT* 	object1;
GAME_OBJECT* 	object2;
GAME_OBJECT* 	renderer;

GRAPHIC_MODEL* 	object1_model;
GRAPHIC_MODEL* 	object2_model;
GRAPHIC_MODEL* 	renderer_model;

GELEMENT_SHAPE*	mink_diff_shape;
GELEMENT_SHAPE*	politope_shape;
GELEMENT_GROUP*	points_group;

VECTOR2D shape1_vertices[]={
	vector2d(-1.0, 1.0),
	vector2d( 1.0, 1.0),
	vector2d( 1.0,-1.0),
	vector2d(-1.0,-1.0),
};

VECTOR2D shape2_vertices[]={
	vector2d(-1.0, 1.0),
	vector2d( 1.0, 1.0),
	vector2d( 1.0,-1.0),
	vector2d(-1.0,-1.0)
};

void SCENE::init(){
	
	uibe_init();
	
	GELEMENT_SHAPE* shape1;
	GELEMENT_SHAPE* shape2;
	
	object1 = spawn(new GAME_OBJECT(), vector2d(0.0, 0.0), create_normal());
	object1_model = new GRAPHIC_MODEL();
	object1_model->set_color(RGB_COLOR(0.2, 1.0, 0.2));
	shape1 = new GELEMENT_SHAPE();
	shape1->set_vertices(shape1_vertices, sizeof(shape1_vertices)/sizeof(VECTOR2D));
	object1_model->push_back(shape1);
	object1->set_graphic_model(object1_model);
	uibe_interactive_object_alloc(object1);
	
	object2 = spawn(new GAME_OBJECT(), vector2d(5.0, 0.0), create_normal());
	object2_model = new GRAPHIC_MODEL();
	object2_model->set_color(RGB_COLOR(1.0, 0.0, 0.0));
	shape2 = new GELEMENT_SHAPE();
	shape2->set_vertices(shape2_vertices, sizeof(shape2_vertices)/sizeof(VECTOR2D));
	object2_model->push_back(shape2);
	object2->set_graphic_model(object2_model);
	uibe_interactive_object_alloc(object2);
	
	renderer = spawn(new GAME_OBJECT(), vector2d(0.0, 0.0), create_normal());
	renderer_model 	= new GRAPHIC_MODEL();
	mink_diff_shape = new GELEMENT_SHAPE();
	mink_diff_shape->set_color(RGB_COLOR(0.8, 0.0, 0.8));
	politope_shape	= new GELEMENT_SHAPE();
	politope_shape->set_color(RGB_COLOR(0.0, 0.5, 0.8));
	points_group	= new GELEMENT_GROUP();
	points_group->set_color(RGB_COLOR(0.4, 0.8, 0.4));
	renderer_model->push_back(mink_diff_shape);
	renderer_model->push_back(politope_shape);
	renderer_model->push_back(points_group);
	renderer->set_graphic_model(renderer_model);
	
	uibe_interactive_object_focus(object1);
}

void SCENE::free(){
	
	delete object1_model;
	delete object2_model;
	delete renderer_model;
	
	uibe_free();
}

void SCENE::compute(double Frame_time){
	
	uibe_compute(Frame_time);
	
	
	GRAPHIC_MODEL*		model;
	
	VECTOR2D* 			vertices1;
	VECTOR2D* 			vertices2;
	UINT	 			vertices1_n;
	UINT				vertices2_n;
	VECTOR2D			delta;
	
	VECTOR2D*			mink_diff;
	UINT				mink_diff_vn;
	VECTOR2D*			mink_hull;
	UINT				mink_hull_vn;
	
	GJK_SIMPLEX2D		gjk_simplex;
	EPA_INFO2D  		epa_info;
	
	VECTOR2D* 			epa_politope;
	UINT				epa_politope_size;
	
	GELEMENT_CIRCLE* 	new_circle;


	delta = object1->get_position() - object2->get_position();

	vertices1_n = sizeof(shape1_vertices)/sizeof(VECTOR2D);
	vertices1 = new VECTOR2D[vertices1_n];
	for(UINT i = 0; i < vertices1_n; i++)
		vertices1[i] = transform_vertex(
			shape1_vertices[i],
			object1->get_position(), 
			object1->get_normal(), 1.0
		);
	
	vertices2_n = sizeof(shape2_vertices)/sizeof(VECTOR2D);
	vertices2 = new VECTOR2D[vertices2_n];
	for(UINT i = 0; i < vertices2_n; i++)
		vertices2[i] = transform_vertex(
			shape2_vertices[i],
			object2->get_position(), 
			object2->get_normal(), 1.0
		);
	
	minkovskiy_diff(vertices2, vertices2_n, vertices1, vertices1_n, mink_diff, mink_diff_vn);
	
	get_mch_jarvis(
		mink_diff, mink_diff_vn, 
		mink_hull, mink_hull_vn
	);

	epa_politope_size 	= 0;
	epa_politope 		= nullptr; 
	
	if(gjk_collision2d(
		vertices2,vertices2_n,
		vertices1,vertices1_n,
		&gjk_simplex)
	){
		
		epa_politope = new VECTOR2D[vertices1_n * vertices2_n]; 
		
		epa_info = epa_info2d(
			vertices2, vertices2_n,
			vertices1, vertices1_n,
			gjk_simplex,
			&epa_politope
		);
		
		epa_politope_size = epa_info.polytope_size;
		
		printf("{%f %f} %f\n",epa_info.normal.x, epa_info.normal.y, epa_info.distance);
	}
	
	
	if(epa_politope){
		politope_shape->set_vertices(epa_politope, epa_politope_size);
		delete [] epa_politope;
	}else
		politope_shape->set_vertices(nullptr, 0);
	
	
	mink_diff_shape->set_vertices(mink_hull, mink_hull_vn);
	
	
	
	points_group->clear();
	for(UINT i = 0; i < mink_diff_vn; i++){
		new_circle = new GELEMENT_CIRCLE();
		new_circle->set_specs(0.2, 32);
		new_circle->set_position(mink_diff[i]);
		points_group->push_back(new_circle);
	}
		
	
	delete [] mink_diff;
	delete [] mink_hull;
	delete [] vertices1;
	delete [] vertices2;

	
}

void SCENE::pre_render(){
	
	
}

void SCENE::event(const EVENT_MSG& Event){
	uibe_event(Event);
}
