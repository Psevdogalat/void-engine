#include <uibe.h>
#include <engine.h>

#include <keys.h>
#include <objects.h>
#include <utilits.h>


#include <list>
#include <algorithm>
#include <cstdio>

using namespace ENGINE;

/* UIBE_CONTAINER */
/* ========================================================================== */
UIBE_CONTAINER::UIBE_CONTAINER(){
	object 		= nullptr;
	collider 	= standard_collider(); 
	
}

UIBE_CONTAINER::UIBE_CONTAINER
(
 const UIBE_CONTAINER& Smpl
)
{
	transform	= Smpl.transform;
	collider	= Smpl.collider;
	object		= Smpl.object;

}

UIBE_CONTAINER::~UIBE_CONTAINER(){

}

void 
UIBE_CONTAINER::set_object
(
 GAME_OBJECT* Object
)
{
	COLLISION_NODE*	 collision_node;
	COLLISION_MODEL* collision_model;
	bool 			 inherited_from_object;
	

	if(Object == nullptr)
		return;
	
	object = Object;
	inherited_from_object = false;

	collision_node = object->get_collision_node();
	if(collision_node == nullptr)
		return;

	collision_model = collision_node->get_collision_model();
	if(collision_model == nullptr)
		return;

	collider = *collision_model;
	inherited_from_object = true;
	
}

GAME_OBJECT* 
UIBE_CONTAINER::get_object
() const
{
	return object;
}

void 
UIBE_CONTAINER::set_position
(
 const VECTOR2D& Position
)
{
	transform.set_position(Position);

	if(object != nullptr)
		object->set_position(Position);

}

VECTOR2D 
UIBE_CONTAINER::get_position
() const
{
	if(object)
		return object->get_position();
	
	return transform.get_position();
}

void 
UIBE_CONTAINER::set_normal
(
 const VECTOR2D& Normal
)
{
	transform.set_normal(Normal);

	if(object != nullptr)
		object->set_normal(Normal);
	
}

VECTOR2D	
UIBE_CONTAINER::get_normal	
() const
{
	if(object)
		return object->get_normal();

	return transform.get_normal();
}

COLLISION_MODEL UIBE_CONTAINER::standard_collider(){
	
	VECTOR2D vertices[] = {
		{-0.5, 0.5},
		{ 0.5, 0.5},
		{ 0.5,-0.5},
		{-0.5,-0.5}
	};
	
	return COLLISION_MODEL(vertices, 4);
}

/* ========================================================================== */

static void camera_movement	();
static void object_movement	();
static void drag_compute	();
static UIBE_CONTAINER* alloc_container	(GAME_OBJECT* );

static CAMERA* 	 			camera;
static const SCENE*			game_scene;
static GAME_OBJECT*			cursor;
static GAME_OBJECT* 		grid;
static GAME_OBJECT*			collider_renderer;

static GRAPHIC_MODEL*		cursor_model;
static GRAPHIC_MODEL*		collider_renderer_model;

static UIBE_CONTAINER* 				interactive_container = nullptr;
static std::list<UIBE_CONTAINER*> 	container_list;
static std::list<GAME_OBJECT*>		new_objects;

static bool 		draw_collisions 	= false;
static bool 		draw_models			= true;
static bool			draw_colliders		= true;

static RGB_COLOR	collider_color(0.7f, 0.3f, 0.5f);

static double		camera_scale_max	= 1.0;
static double		camera_scale_min	= 0.05;
static double 		camera_scale_quant	= 0.01;
static double 		camera_scale		= 0.1;
static const double camera_rot_quant 	= M_PI/32;
static const double camera_mov_quant 	= 0.2;
static VECTOR2D 	camera_proportion 	= {1.5, 1.0};
static VECTOR2D		camera_drag_position= {0.0, 0.0};

static UINT			drag_key			= KEY_MOUSE_L;
static bool			mouse_dragged		= false;
static VECTOR2D		drag_delta			= {0.0, 0.0};
static VECTOR2D 	mouse_last		   	= {0.0, 0.0};
static VECTOR2D 	mouse_coord	 		= {0.0, 0.0};



void ENGINE::uibe_init(){
	
	init_key_map();
	set_draw_graphic_models		(draw_models	);
	set_draw_collision_models	(draw_collisions);
	
	game_scene  = get_current_scene();
	camera 		= (CAMERA*)&game_scene->camera;
	
	camera->set_area(VECTOR_RECTANGLE(vector2d(0.0, 0.0), create_normal(), 
		camera_proportion.x, camera_proportion.y));
		
	camera->set_scale(camera_scale);
	
	grid = spawn(new METRIC_GRID(40.0, 20.0), vector2d(0,0), create_normal());
	grid->set_visible(true);
	
	cursor = spawn(new GAME_OBJECT(), vector2d(0.0, 0.0), create_normal());
	cursor_model = create_point_model();
	cursor_model->set_scale(vector2d(0.2, 0.2));
	cursor_model->set_color(RGB_COLOR(1.0f, 0.0f, 0.0f));
	cursor_model->set_layer(GLAYERS_NUM-1);
	cursor->set_graphic_model(cursor_model);
	cursor->set_visible(true);
	
	collider_renderer = spawn(new GAME_OBJECT(), 
				vector2d(0.0, 0.0), create_normal());
	collider_renderer_model = new GRAPHIC_MODEL();
	collider_renderer_model->set_color(collider_color);
	collider_renderer_model->set_layer(GLAYERS_NUM-1);
	collider_renderer->set_graphic_model(collider_renderer_model);
	collider_renderer->set_visible(false);
}

void ENGINE::uibe_free(){
	
	for(UIBE_CONTAINER* container: container_list)
		delete container;
	
	delete cursor_model;
	delete collider_renderer_model;
	
}

void ENGINE::uibe_compute(double Frame_time){
	
	for(GAME_OBJECT* object: new_objects)
		alloc_container(object);
	
	
	drag_compute();
	camera_movement();
	object_movement();
	
	
	if(key_front_down(KEY_B)){
		printf("Break point!\n");
	}
	
	if(key_front_down(KEY_Z))
		grid->set_visible(!grid->is_visible());
	
	if(key_front_down(KEY_C)){
		draw_collisions = !draw_collisions;
		set_draw_collision_models(draw_collisions);
	}
	
	if(key_front_down(KEY_ESC))
		terminate();
	
	if(draw_colliders){
		GELEMENT_SHAPE*			collider;
		const COLLISION_MODEL*	v_colider;

		collider_renderer_model->clear();
		
		for(UIBE_CONTAINER* container: container_list){
			collider = new GELEMENT_SHAPE();
				
				v_colider = &container->collider;

			
			collider->set_vertices(
					v_colider->get_raw_vertices(), 
					v_colider->get_vertices_n()
				);

			collider->set_position	(container->get_position());
			collider->set_normal	(container->get_normal());

			collider_renderer_model->push_back(collider);
		}
	}

	collider_renderer->set_visible(draw_colliders);
	
}

void ENGINE::uibe_event		(const EVENT_MSG& Event){
	
	const EVENT_KEY_RECORD& 	key_record 		= Event.event_record.key;
	const EVENT_MOUSE_RECORD& 	mouse_record 	= Event.event_record.mouse;
	
	unsigned int key;
	
	switch(Event.event){
		case EV_KEY_PRESS:
			
			key = find_key_by_code(key_record.key_code);
			if(key != INVALID_KEY){
				if(key_record.key_down == true){
					toggle_key_down(key);
				}else
					toggle_key_up(key);

			}else{
				printf("KEY_PRESS %s KEY_CODE %d\n", 
						(key_record.key_down == true)? "DOWN":"UP", 
						key_record.key_code
				);
			}

		break;
		case EV_MOUSE_WHEEL:
			if(mouse_record.delta > 0 && camera_scale < camera_scale_max){
				camera_scale+= camera_scale_quant;
				camera->set_scale(camera_scale);
				
			}else if(mouse_record.delta < 0 && camera_scale > camera_scale_min){
				camera_scale-= camera_scale_quant;
				camera->set_scale(camera_scale);
				
			}
		break;
		case EV_MOUSE_MOVE:
			mouse_coord = scale_vector(
					vector2d(mouse_record.x , mouse_record.y),
					camera_proportion
				);
			
		break;
		case EV_MOUSE_MKEY_DOWN:
			toggle_key_down(KEY_MOUSE_M);
		break;
		case EV_MOUSE_MKEY_UP:
			toggle_key_up(KEY_MOUSE_M);
		break;
		case EV_MOUSE_LKEY_DOWN:
			toggle_key_down(KEY_MOUSE_L);
		break;
		case EV_MOUSE_LKEY_UP:
			toggle_key_up(KEY_MOUSE_L);
		break;
		case EV_MOUSE_RKEY_DOWN:
			toggle_key_down(KEY_MOUSE_R);
		break;
		case EV_MOUSE_RKEY_UP:
			toggle_key_up(KEY_MOUSE_R);
		break;
	}
	
}

void ENGINE::uibe_interactive_object_alloc	(GAME_OBJECT* Object){
	new_objects.push_back(Object);
}

void ENGINE::uibe_interactive_object_free	(GAME_OBJECT* Object){
	for(UIBE_CONTAINER* container: container_list)
		if(container->get_object() == Object){
			if(interactive_container == container)
				interactive_container = nullptr;
			
			container_list.remove(container);
			return;
		}
		
}

void ENGINE::uibe_interactive_object_focus	(GAME_OBJECT* Object){
	for(UIBE_CONTAINER* container: container_list)
		if(container->get_object() == Object){
			interactive_container = container;
			return;
		}
		
}

static UIBE_CONTAINER* alloc_container(GAME_OBJECT* Object){
	UIBE_CONTAINER* new_container;
	
	if(Object != nullptr)
		for(UIBE_CONTAINER* container: container_list)
			if(container->get_object() == Object)
				return nullptr;		
	
	new_container = new UIBE_CONTAINER();
	new_container->set_object(Object);
	container_list.push_back(new_container);
	return new_container;
}

static void camera_movement(){
	
	if(key_state(KEY_SHIFT)){
		if(key_front_down(KEY_E))
			camera->set_normal(
				rotate_vector(camera->get_normal(), camera_rot_quant)
			);
		
		
		if(key_front_down(KEY_Q))
			camera->set_normal(
				rotate_vector(camera->get_normal(),-camera_rot_quant)
			);
	}
}

static void object_movement(){
	
	if(!interactive_container){
		cursor->set_visible(false);
		return;
	}
	
	cursor->set_visible(true);
	
	if(key_state(KEY_Q))
		interactive_container->set_normal(
			rotate_vector(interactive_container->get_normal(), M_PI/32)
		);
	
	
	if(key_state(KEY_E))
		interactive_container->set_normal(
			rotate_vector(interactive_container->get_normal(),-M_PI/32)
		);
		
	cursor->set_position(interactive_container->get_position());
}

static void drag_compute(){
	
		VECTOR2D 				scene_cursor_coord;
		COLLISION_MODEL*		collider;
		
		scene_cursor_coord = game_scene->translate_to_scene(mouse_coord);
		
		/*
		if(mouse_coord != mouse_last){
			printf("mouse coord %f %f S:%f \n",
				mouse_coord.x, mouse_coord.y , camera->get_scale());
			printf("scene coord %f %f \n", 
				scene_cursor_coord.x, scene_cursor_coord.y );
		}
		*/
		
		
		if(key_front_down(drag_key)){
			mouse_dragged = true;
			if(interactive_container){
				drag_delta = interactive_container->get_position() - 
					scene_cursor_coord;
			}else{
				drag_delta = mouse_coord ;
				camera_drag_position = camera->get_position();
			}
		}
		
		if(key_front_up(drag_key))
			mouse_dragged = false;
		
		if(mouse_dragged){
			if(interactive_container){
				interactive_container->set_position
				(
					scene_cursor_coord + drag_delta
				);

			}else{				
				camera->set_position(camera_drag_position - 
					transform_vertex((mouse_coord - drag_delta),
						vector2d(0.0, 0.0), 
						camera->get_normal() ,
						0.5/ camera->get_scale()
					)
				);
			}
		}
		
		if(!mouse_dragged){
			interactive_container = nullptr;
			for(UIBE_CONTAINER* container: container_list){
				collider = &container->collider;
				collider->set_position(container->get_position());
				collider->set_normal(container->get_normal());

				if(
					ptm_collision2d(*collider, scene_cursor_coord, 
						nullptr, nullptr)
				  )
				{
					interactive_container = container;
					break;
				}
			}
		}
	
}

VECTOR2D	
ENGINE::uibe_get_mouse_coord
()
{
	return mouse_coord; 
}

UIBE_CONTAINER*
ENGINE::uibe_create_container
(
	const COLLISION_MODEL& Collider
)
{
	UIBE_CONTAINER* container;
	container = alloc_container(nullptr);
	container->collider = Collider;
	return container;
}

UIBE_CONTAINER*
ENGINE::uibe_create_container
()
{
	UIBE_CONTAINER* container;
	container = alloc_container(nullptr);
	return container;
}

void
ENGINE::uibe_delete_container
(
	const UIBE_CONTAINER* Container
)
{
	container_list.remove((UIBE_CONTAINER*) Container);
	delete Container;
}

void 
ENGINE::uibe_focus_container
(
	const UIBE_CONTAINER* Container
)
{
	if(Container == nullptr)
		interactive_container = nullptr;

	for(UIBE_CONTAINER* container: container_list)
		if(container == Container)
		{
			interactive_container = (UIBE_CONTAINER*) Container;
			break;
		}
}

