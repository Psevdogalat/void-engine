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
#include <algorithm>

using namespace ENGINE;

static const SCENE*			game_scene;

static GAME_OBJECT*			unit;
static GAME_OBJECT* 		ray_caster;
static GAME_OBJECT* 		ray;
static GAME_OBJECT* 		colliding_point;

static GAME_OBJECT_FILTER 	filter;
static GAME_OBJECT_FILTER 	stop_filter;

static GRAPHIC_MODEL*		ray_caster_model;
static GRAPHIC_MODEL*		ray_model;
static GRAPHIC_MODEL*		colliding_point_model;

static GELEMENT_POLYLINE*	ray_polyline;
static const UINT 			max_ray_bounce = 10;

void build_walls(){
	GAME_OBJECT* wall;
	
	struct wall_info{
		VECTOR2D	position;
		VECTOR2D	normal;
		double		width;
		double 		height;
	}walls_info[] = {
		{vector2d( 3.0, 0.0)				, vector2d(1.0, 0.0), 1.0, 3.0},
		{vector2d( 3.0, 5.0)				, vector2d(1.0, 0.0), 2.0, 2.0},
		{vector2d(-3.0,-5.0)				, vector2d(1.0, 0.0), 2.0, 2.0},
		{vector2d(-5.0, 5.0)				, vector2d(1.0, 1.0), 4.0, 2.0},
		{vector2d( 5.0,-5.0)				, vector2d(1.0, 1.0), 4.0, 2.0},
		
		{vector2d( 0.0  , 8.75)				, create_normal(), 30.0, 0.5},
		{vector2d( 0.0  ,-8.75)				, create_normal(), 30.0, 0.5},
		{vector2d(-14.75, 0.0)				, create_normal(), 0.5 , 17.0},
		{vector2d( 14.75, 0.0)				, create_normal(), 0.5 , 17.0}
	};
	
	for(UINT i = 0; i < (sizeof(walls_info)/sizeof(wall_info)); i++){
		wall = spawn(
			new GAME_OBJECT_WALL(walls_info[i].width, walls_info[i].height, RGB_COLOR(0.0f, 0.5f, 0.0f)), 
			walls_info[i].position, 
			walls_info[i].normal
		);
		
		if(wall)
			uibe_interactive_object_alloc(wall);
	}
	
}

void SCENE::init(){
	
	uibe_init();
	game_scene = get_current_scene();
	
	build_walls();
	
	ray_caster = spawn(new GAME_OBJECT(), vector2d(-4.0, 2.0), rotate_vector(create_normal(), 2*M_PI/3));
	ray_caster_model = create_arrow_model();
	ray_caster_model->set_color(RGB_COLOR(1.0f, 1.0f, 0.0f));
	ray_caster->set_graphic_model(ray_caster_model);
	uibe_interactive_object_alloc(ray_caster);
	
	unit = spawn(new GAME_OBJECT_PLAYER(), vector2d(-8.0, 0.0), create_normal());
	uibe_interactive_object_alloc(unit);
	stop_filter.pointers.push_back(unit);
	
	
	ray = spawn(new GAME_OBJECT(), vector2d(0.0, 0.0), create_normal());
	ray_model = new GRAPHIC_MODEL();
	ray_model->set_color(RGB_COLOR(0.0f, 1.0f, 1.0f));
	ray_polyline = new GELEMENT_POLYLINE();
	ray_model->push_back(ray_polyline);
	ray->set_graphic_model(ray_model);
	ray->set_visible(false);
	
	colliding_point = spawn(new GAME_OBJECT(), vector2d(0.0, 0.0), create_normal());
	colliding_point_model = create_point_model();
	colliding_point_model->set_scale(vector2d(0.2, 0.2));
	colliding_point_model->set_color(RGB_COLOR(0.0f, 0.0f, 1.0f));
	colliding_point->set_graphic_model(colliding_point_model);
	colliding_point->set_visible(false);
	
	uibe_interactive_object_focus(ray_caster);
}

void SCENE::free(){
	
	delete ray_caster_model;
	delete ray_model;
	delete colliding_point_model;
	
	uibe_free();
}

void SCENE::compute(double Frame_time){
	
	uibe_compute(Frame_time);

	RAYCAST_OBJ_INFO_LIST	info_list;
	VECTOR2D				ray_origin;
	VECTOR2D				ray_direction;
	VECTOR2D*				ray_points;
	UINT					ray_points_n;
	
	ray_origin		= ray_caster->get_position();
	ray_direction	= ray_caster->get_normal();
	
	if(game_scene->bouncing_raycast(ray_origin, ray_direction, nullptr, &stop_filter, max_ray_bounce ,info_list)){
		
		UINT i = 0;
		ray_points_n = info_list.size() + 1;
		ray_points = new VECTOR2D[ray_points_n];
		
		ray_points[i] = ray_origin;
		for(RAYCAST_OBJ_INFO info: info_list){
			i++;
			ray_points[i] = ray_origin + ray_direction * info.distance;
			ray_origin 		= ray_points[i];
			ray_direction 	= mirror_vector(ray_direction, info.normal); 
		}
		
		
		ray_polyline->set_vertices(ray_points, ray_points_n);
		ray->set_visible(true);
		
		colliding_point->set_position(ray_points[ray_points_n-1]);
		colliding_point->set_visible(true);
		
		delete [] ray_points;
		
	}else{
		colliding_point->set_visible(false);
		ray->set_visible(false);
	}
	
}

void SCENE::pre_render(){
	
	
}

void SCENE::event(const EVENT_MSG& Event){
	uibe_event(Event);
}
