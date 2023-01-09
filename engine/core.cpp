#include <cstdlib>
#include <cstdio>
#include <cstring> 
#include <algorithm>

#include <engine.h>
#include <engine_platform.h>

using namespace ENGINE;

static SCENE* 			current_scene = nullptr;

static TIMERS_LIST 		timers;
static PHYSICS_LIST 	physical_models;

static void key_parser				(const char* );
static void compute_objects 		(double );
static void compute_timers			(double );
static void compute_physics			(double );
static void compute_collisions		();
static void process_spawn_list		();
static void process_despawn_list	();


/* class TIMER */
/* ========================================================================== */
TIMER::TIMER(){time = 0.0; }
TIMER::~TIMER(){ }

bool TIMER::condition(){
	return time == 0.0;
}

void TIMER::set(double Time){
	time = Time;
}

void TIMER::countdown(double Delta){
	if((time -= Delta) < 0.0)
		time = 0.0;		
}

/* class COLLISION_NODE */
/* ========================================================================== */
COLLISION_NODE::COLLISION_NODE
(
	GAME_OBJECT* 		Game_object, 
	bool 				Active, 
	bool 				Passive, 
	bool 				Epa, 
	COLLISION_MODEL* 	Model
){
	game_object = Game_object;
	model		= Model;
	active		= Active;
	passive		= Passive;
	epa			= Epa;
	
}

COLLISION_NODE::~COLLISION_NODE(){ }

bool 
COLLISION_NODE::is_active 		
() const{ return active; }

bool 
COLLISION_NODE::is_passive 		
() const{ return passive;}

bool 
COLLISION_NODE::is_epa 			
() const{ return epa; }

GAME_OBJECT* 
COLLISION_NODE::get_game_object 
() const{ return game_object; }

COLLISION_MODEL* 
COLLISION_NODE::get_collision_model 
() const{ 
	model->set_position	(game_object->get_position());
	model->set_normal	(game_object->get_normal());
	return model; 
}

const COLLISIONS_LIST& 	
COLLISION_NODE::get_collisions () const
{ 
	return collisions; 
}

void 
COLLISION_NODE::add_collision(const COLLISION_INFO&  Info)
{
	collisions.push_back(Info);	
}

void 
COLLISION_NODE::clear_collisions(){
	collisions.clear();
}

void 
COLLISION_NODE::compute(COLLISION_NODE* Partner_node){
	bool 				collised;
	
	COLLISION_INFO 		info;
	COLLISION_INFO		info_partner;
	
	GAME_OBJECT*		object1;
	GAME_OBJECT*		object2;
	
	COLLISION_MODEL*	collision_model1;
	COLLISION_MODEL*	collision_model2;
	
	GJK_SIMPLEX2D		gjk_simplex;
	EPA_INFO2D			epa_info;

	collised 	= false;

	object1		= get_game_object();
	object2		= Partner_node->get_game_object();
	
	for(COLLISION_INFO collision: collisions){
		if(collision.node == Partner_node){
			collised = true;
			info = collision;
			break;
		}
	}
	
	if(!collised){
		collision_model1 = get_collision_model();
		collision_model2 = Partner_node->get_collision_model();
		
		if(mtm_collision2d(
				*collision_model2,
				*collision_model1,
				&gjk_simplex,
				(epa && Partner_node->epa)?&epa_info:nullptr
			)
		){
			info.node = Partner_node;
			
			if(epa && Partner_node->epa){
				info.type 		= COLLISION_INFO::TYPE_EPA;
				info.epa_info 	= epa_info;
				
			}else{
				info.type 		 = COLLISION_INFO::TYPE_RAW_GJK;
				info.gjk_simplex = gjk_simplex;
			}
			
			add_collision(info);
			
			if(Partner_node->is_active()){
				info_partner = info;
				info_partner.node = this;
				if(info.type == COLLISION_INFO::TYPE_EPA){
					info_partner.epa_info.normal *= -1.0;
				}
				Partner_node->add_collision(info_partner);
			}
			
			collised = true;
		}
		
	}
	
	if(collised)
		object1->collision(object2, &info);
	
}

/* class GAME_OBJECT */
/* basic game object class */
/* ========================================================================== */
GAME_OBJECT::GAME_OBJECT ()
{
	type 			= GOT_UNDEFINE;
	
	visible 		= true;
	
	position		= {0.0f, 0.0f};
	normal			= create_normal();
	
	name 			= nullptr;
	collision_node 	= nullptr;
	graphic_model 	= nullptr;
	physical_model  = nullptr;
}

GAME_OBJECT::GAME_OBJECT
(
	const char* 	Name, 
	const VECTOR2D& Position, 
	const VECTOR2D& Normal
):GAME_OBJECT()
{
	set_name(Name);
	position 		= Position;
	normal			= normalize_vector(Normal);
}

GAME_OBJECT::~GAME_OBJECT(){
	set_name			(nullptr);
	set_graphic_model	(nullptr);
	set_collision_node	(nullptr);
}

const char*	
GAME_OBJECT::get_name() const
{
	return (const char*) name;
}

void 
GAME_OBJECT::set_name(const char* Name)
{
	if(name != nullptr){
		free(name);
		name = nullptr;
	}
	
	if(Name != nullptr){
		name = (char*)malloc(strlen(Name)+1);
		strcpy(name,Name);
	}
}

const UINT 
GAME_OBJECT::get_type() const
{
	return type;
}

void 
GAME_OBJECT::compute()
{ 

}

void 
GAME_OBJECT::collision(GAME_OBJECT* Object, const COLLISION_INFO* Param)
{
	//printf("GO %s collision with GO %s\n", get_name(), Object->get_name());
}

void 
GAME_OBJECT::spawn()
{
	
}

void 
GAME_OBJECT::despawn()
{
	
}

void 
GAME_OBJECT::set_graphic_model(GRAPHIC_MODEL* Graphic_model)
{
	graphic_model = Graphic_model;
}

GRAPHIC_MODEL* 
GAME_OBJECT::get_graphic_model() const
{
	return graphic_model;
}

void 
GAME_OBJECT::set_visible(bool Visible)
{
		visible = Visible;
}

bool 
GAME_OBJECT::is_visible() const
{
	if(graphic_model != nullptr && visible)
		return true;
	
	return false;
}

void 
GAME_OBJECT::set_collision_node(COLLISION_NODE* Node)
{
	collision_node = Node;
}

COLLISION_NODE* 
GAME_OBJECT::get_collision_node() const
{
	return collision_node;
}

bool 
GAME_OBJECT::is_collisible() const
{
	if(collision_node != nullptr)
		return true;
		
	return false;
}

void 
GAME_OBJECT::set_physical_model(PHYSICAL_MODEL* Model_ptr)
{	
	physical_model = Model_ptr;
}

PHYSICAL_MODEL*	
GAME_OBJECT::get_physical_model() const
{
	return physical_model;
}

bool 
GAME_OBJECT::is_physical() const
{
	if(physical_model != nullptr)
		return true;
	
	return false;
}

void 	
GAME_OBJECT::set_position(const VECTOR2D& Position )
{
	position = Position;
	
	if(physical_model != nullptr)
		physical_model->position = position;	
}

VECTOR2D	
GAME_OBJECT::get_position() const
{
	return position;
}

void 	
GAME_OBJECT::set_normal(const VECTOR2D& Normal)
{
	normal = normalize_vector(Normal);
	
	if(physical_model != nullptr)
		physical_model->normal = normal;
}

VECTOR2D	
GAME_OBJECT::get_normal() const
{
	return normal;
}

/* class GAME_OBJECT_FILTER */
/* ========================================================================== */
GAME_OBJECT_FILTER::GAME_OBJECT_FILTER
()
{
	filter_func = default_filt;
	inverse 	= false;
}

GAME_OBJECT_FILTER::GAME_OBJECT_FILTER
(
	const GAME_OBJECT_FILTER& Smpl
):GAME_OBJECT_FILTER()
{
	*this = Smpl;
}

GAME_OBJECT_FILTER::~GAME_OBJECT_FILTER
(){}

bool GAME_OBJECT_FILTER::filt
(const GAME_OBJECT* Object) const
{
	if(!filter_func)
		return true;
	
	return filter_func(pointers, Object) != inverse;
}

bool 
GAME_OBJECT_FILTER::default_filt
(
	const std::list<GAME_OBJECT*>& 	Pointers, 
	const GAME_OBJECT* 				Object
)
{
	for(GAME_OBJECT* target: Pointers){
		if(Object == target)
			return true;
	}
		
	return false;
}

GAME_OBJECT_FILTER&
GAME_OBJECT_FILTER::operator=
(
	const GAME_OBJECT_FILTER& Rval
)
{
	inverse 	= Rval.inverse;
	filter_func	= Rval.filter_func;
	pointers	= Rval.pointers;
	
	return *this;
}	


/* class CAMERA */
/* ========================================================================== */
CAMERA::CAMERA
(){}

CAMERA::~CAMERA
(){}

void 
CAMERA::set_area
( const VECTOR_RECTANGLE& Area )
{*((VECTOR_RECTANGLE*)this) = Area; }

void 
CAMERA::set_constraint
(const VECTOR_RECTANGLE& Constraint)
{constraint = Constraint; }

void 
CAMERA::move
(const VECTOR2D& Position)
{
	VECTOR2D new_position;
	
	VECTOR2D constraint_vertices[4];
	VECTOR2D constraint_position;
	double	 constraint_scale;
	
	constraint_scale	= constraint.get_scale();
	constraint_position = constraint.get_position();
	
	for(UINT ivertex=0; ivertex < 4; ivertex++ ){
		constraint.get_vertex(ivertex, constraint_vertices[ivertex]);
		constraint_vertices[ivertex] *= constraint_scale;
		constraint_vertices[ivertex] += constraint_position;
	}
	
	new_position = Position;
	if(new_position.x < constraint_vertices[3].x){
		new_position.x = constraint_vertices[3].x;
		
	}else if(new_position.x > constraint_vertices[1].x){
		new_position.x = constraint_vertices[1].x;
		
	}
	
	if(new_position.y < constraint_vertices[3].y){
		new_position.y = constraint_vertices[3].y;
		
	}else if(new_position.y > constraint_vertices[1].y){
		new_position.y = constraint_vertices[1].y;
		
	}
	
	position = new_position;
}

/* class SCENE */
/* ========================================================================== */
SCENE::SCENE
()
{
	//printf("scene construct!\n");
}

SCENE::~SCENE
()
{
	for(GAME_OBJECT* object: spawn_list)
		delete object;
		
	for(GAME_OBJECT* object: game_objects)
		delete object;
	
	//printf("scene destruct!\n");
}

GAME_OBJECT*
SCENE::spawn
(
	GAME_OBJECT* Object,
	const VECTOR2D& Position,
	const VECTOR2D& Normal
)
{
	if(!Object)
		return nullptr;

	Object->set_position(Position);
	Object->set_normal(Normal);

	spawn_list.push_back(Object);
	return Object;
}

void
SCENE::despawn
(
	GAME_OBJECT* Object
)
{
	if(!Object)
		return;
	
	despawn_list.push_back(Object);
}


void 
SCENE::__compute
(
	double dt
)
{

	GAME_OBJECTS_LIST::iterator it;

	/* spawn objects */
	for(GAME_OBJECT* object: spawn_list){
		object->spawn();
		game_objects.push_back(object);
	}
	
	spawn_list.clear();

	/* collision compute */
	for(COLLISION_NODE* active_collision : active_collisions){
		for(COLLISION_NODE* passive_collision: passive_collisions){
			if(active_collision == passive_collision)
				continue;
			
			active_collision->compute(passive_collision);
		}
		
		active_collision->clear_collisions();
	}
	
	/* scene compute */
	compute(dt);

	/* objects compute */
	for(GAME_OBJECT* object: game_objects)
		object->compute();

	/* despawn objects */
	for(GAME_OBJECT* object: despawn_list){
		
		it = std::find(
			game_objects.begin(), 
			game_objects.end(), 
			object
		);
		
		if(it != game_objects.end()){
			game_objects.erase(it);
			object->despawn();
			delete object;
		}
	}
	
	despawn_list.clear();

}

bool 
SCENE::collision_test
(
	const COLLISION_MODEL&		Model, 
	const GAME_OBJECT_FILTER*	Filter,  
	COLLISIONS_LIST* 			Info_list	
) const
{
	bool 				collised;
	COLLISION_INFO 		info;
	GAME_OBJECT*		node_object;
	COLLISION_MODEL*	node_model;
	EPA_INFO2D			epa_info;

	collised 	= false;
	for(COLLISION_NODE* node :passive_collisions){
		node_object	= node->get_game_object();
		node_model	= node->get_collision_model();
			
		if(Filter && !Filter->filt(node_object))
			break;
	
		
		if(
			mtm_collision2d
			(
				*node_model,
				Model,
				nullptr,
				(Info_list != nullptr)?&epa_info:nullptr
			)
		){
			collised = true;
			
			if(Info_list != nullptr){
				info.node 		= node;
				info.type 		= COLLISION_INFO::TYPE_EPA;
				info.epa_info 	= epa_info;
				
				Info_list->push_back(info);
			}else
				break;
			
		}
	}
	
	return collised;
}

bool 
SCENE::bouncing_raycast
(
	const VECTOR2D& 			Origin, 
	const VECTOR2D& 			Direction, 
	const GAME_OBJECT_FILTER* 	Filter, 
	const GAME_OBJECT_FILTER* 	Stop_filter, 
	UINT 						Bounce_limit, 
	RAYCAST_OBJ_INFO_LIST& 		Info_list
) const
{	
	UINT		 		intersections;
	RAYCAST_OBJ_INFO 	info;
	VECTOR2D	 		origin;
	VECTOR2D	 		direction;
	
	intersections	= 0;	
	origin 			= Origin;
	direction		= Direction;
	
	while(raycast(origin, direction, Filter, &info)){
		intersections++;
		Info_list.push_back(info);
		
		if(Stop_filter && Stop_filter->filt(info.object))
			break;
		
		if(Bounce_limit == intersections - 1)
			break;
		
		origin 		+= direction * info.distance;
		direction	= mirror_vector(direction, info.normal);
	}
	
	if(intersections)
		return true;
	
	return false;
}

bool 
SCENE::raycast
(
	const VECTOR2D& 			Origin, 
	const VECTOR2D& 			Direction,
	const GAME_OBJECT_FILTER* 	Filter, 
	RAYCAST_OBJ_INFO* 			Info
) const
{
	const GAME_OBJECT*			object;
	const COLLISION_NODE_LIST*	collisions_list;
	const COLLISION_MODEL*		collision_model;
	RAYCAST_OBJ_INFO			obj_info;
	RAYCAST_INFO2D				info;
	bool 						collision;
	
	collision 		= false;
	collisions_list	= (const COLLISION_NODE_LIST*)&passive_collisions;
	
	for(COLLISION_NODE* node: *collisions_list){
		object = node->get_game_object();
		
		if(Filter && !Filter->filt(object))
			continue;
		
		collision_model = node->get_collision_model();		
		
		if(
			raycast2d(Origin, Direction, *collision_model, &info) && 
			info.distance >= 0.0
		)
			if(!collision || obj_info.distance > info.distance){
				obj_info.object 	= object;
				obj_info.distance	= info.distance;
				obj_info.normal		= info.normal;
				collision 			= true;
				
				if(Info == nullptr)
					break;
			}	
	}

	if(collision && Info != nullptr)
		*Info = obj_info;
	
	return collision;
}	

VECTOR2D 
SCENE::translate_to_scene
(
	const VECTOR2D& Coord, 
	const CAMERA* 	Camera
) const
{
	if(Camera)	
		return  transform_vertex
		(
			Coord, 
			Camera->get_position(), 
			Camera->get_normal(), 
			1.0/ Camera->get_scale()
		);
	
	
	return translate_to_scene(Coord, &camera);
}

VECTOR2D 
SCENE::translate_from_scene
(
	const VECTOR2D& Coord, 
	const CAMERA* 	Camera
) const
{
	if(Camera){
		return transform_vertex
		(
			Coord, 
			-Camera->get_position(), 
			Camera->get_normal() * vector2d(1.0, -1.0), 
			Camera->get_scale()
		);
	}
	
	return translate_to_scene(Coord, &camera);
}

VECTOR2D 
SCENE::translate_to_scene
(const VECTOR2D& Coord) const
{return translate_to_scene(Coord, &camera); }

VECTOR2D 
SCENE::translate_from_scene
(const VECTOR2D& Coord) const
{return translate_from_scene(Coord, &camera); }




/* engine methods definition */
//==============================================================================

void 
ENGINE::eng_init
(const char* Args)
{
	current_scene = new SCENE();
	
	if(current_scene)
		current_scene->init();

	init_graphic();
}

void 
ENGINE::eng_free
()
{
	current_scene->free();
	delete current_scene;
	
	for(TIMER* timer: timers)
		delete timer;
	
	for(PHYSICAL_MODEL* model: physical_models)
		delete model;
		
}

void 
ENGINE::eng_compute
(
	double Frame_time
)
{
	
	compute_timers	(Frame_time);
	compute_physics(Frame_time);

	if(current_scene)
		current_scene->__compute(Frame_time);

}

void 
ENGINE::eng_render
()
{
	render_graphic(); 
}

void 
ENGINE::eng_event
(const EVENT_MSG& Event)
{
	if(current_scene)
		current_scene->event(Event); 
}

static void 
compute_timers
(double Frame_time)
{
	for(TIMER* timer: timers)
		timer->countdown(Frame_time);
	
}

static void 
compute_physics
(double Frame_time)
{
	PHYSICAL_MODEL* 	physical_model;
	
	if(current_scene)
		for(GAME_OBJECT* object: current_scene->game_objects){
		
		if(!object->is_physical())
			continue;
	
		physical_model = object->get_physical_model();			
	
		physical_model->position 	= object->get_position();
		physical_model->normal		= object->get_normal();
		
		physical_model->compute(Frame_time);
		
		object->set_position(physical_model->position);
		object->set_normal	(physical_model->normal);
		
	}
}


/* timers */
//==============================================================================
TIMER* 	
ENGINE::create_timer
()
{
	TIMER* new_timer = new TIMER();
	timers.push_back(new_timer);
	return new_timer;
}

void 
ENGINE::free_timer
(TIMER* Timer_ptr)
{
	TIMERS_LIST::iterator timer_container;
	
	timer_container = std::find(timers.begin(), timers.end(), Timer_ptr);
	if(timer_container != timers.end()){
		timers.erase(timer_container);
		delete Timer_ptr;
	}
		
}

/* physics and collisions */ 
//==============================================================================
PHYSICAL_MODEL* 
ENGINE::create_physical_model
()
{
	PHYSICAL_MODEL* new_physical_model  = new PHYSICAL_MODEL();
	physical_models.push_back(new_physical_model);
	return new_physical_model;
	
}

void 
ENGINE::free_physical_model
(PHYSICAL_MODEL* Physical_model_ptr)
{
	PHYSICS_LIST::iterator model_container;
	
	model_container = std::find(
		physical_models.begin(), 
		physical_models.end(), 
		Physical_model_ptr
	);
	
	if(model_container != physical_models.end()){
		physical_models.erase(model_container);
		delete Physical_model_ptr;
	}
		
}

COLLISION_NODE*	
ENGINE::create_collision_node
(
	GAME_OBJECT* 		Object, 
	bool 				Active, 
	bool 				Passive, 
	bool 				Epa, 
	COLLISION_MODEL* 	Model
){
	COLLISION_NODE* node;
	
	if(!current_scene)
		return nullptr;
	
	if(Object == nullptr)
		return nullptr;
	
	if(Model == nullptr)
		return nullptr;
	
	node = new COLLISION_NODE(Object, Active, Passive, Epa, Model);
	
	if(Active)
		current_scene->active_collisions.push_back(node);
	
	if(Passive)
		current_scene->passive_collisions.push_back(node);
	
	return node;
}

void 
ENGINE::free_collision_node
(COLLISION_NODE* Node)
{

	if(!current_scene)
		return;

	if(Node == nullptr)
		return;
	
	if(Node->is_active())
		current_scene->active_collisions.remove(Node);
	
	if(Node->is_passive())
		current_scene->passive_collisions.remove(Node);
	
	delete Node;
}

/* global scene API */
/* ========================================================================== */
SCENE* 
ENGINE::get_current_scene
()
{
	return current_scene; 
}

GAME_OBJECT* 
ENGINE::spawn
(
	GAME_OBJECT* 	Object, 
	const VECTOR2D& Position, 
	const VECTOR2D& Normal
){
	if(!current_scene)
		return nullptr;

	return current_scene->spawn(Object, Position, Normal);
}

void 
ENGINE::despawn
(GAME_OBJECT* Object)
{
	if(current_scene)
		current_scene->despawn(Object);
}


