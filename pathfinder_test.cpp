#include <engine.h>

#include <objects.h>
#include <utilits.h>
#include <vector_utilits.h>
#include <keys.h>
#include <uibe.h>

#include <cstdlib>
#include <cstdio>
#include <list>
#include <vector>
#include <cmath>
#include <algorithm>

#include <pathfind.h>
#include <visualizers.h>

using namespace ENGINE;

/*
class MOVER{
	protected:
		VECTOR2D			position;
		VECTOR2D			normal;
		
		WAYPOINTS			waypoints;
		WAYPOINTS::iterator	waypoint;
		bool				do_move;
	
	public:
		MOVER();
		MOVER(const MOVER& Smpl);
		~MOVER();
		
		void 
		set_position
		(
			const VECTOR2D& Position
		);
		
		void 
		set_normal
		(
			const VECTOR2D& Normal;
		);
		
		void 
		set_waypoints
		(
			const WAYPOINTS& Waypoints
		);
		
		const VECTOR2D& 
		get_position
		();
		
		const VECTOR2D&
		get_normal
		();
		
		void compute();
	
};
*/



static const RGB_COLOR          color_bad               (1.00f, 0.00f, 0.00f);
static const RGB_COLOR          color_good              (0.00f, 1.00f, 0.00f);
static const RGB_COLOR          color_neutral           (0.50f, 0.50f, 0.50f);
static const RGB_COLOR          color_start_point       (1.00f, 0.55f, 0.00f);
static const RGB_COLOR          color_end_point         (0.00f, 1.00f, 0.49f);

static SCENE*                   game_scene;
static GAME_OBJECT*             unit;

static const double             scaner_factor	= 2.0;
static const double             scaner_delta 	= 1.0 	/ scaner_factor;
static PATHFINDER               pathfinder;
static GAME_OBJECT_FILTER       scan_filter;
static COLLISION_MODEL          scan_model;

static VIS_PATH                 vis_path;
static VIS_PATHFINDER           vis_pathfinder;
//static VIS_MOVER                vis_mover;

static UIBE_CONTAINER*          con1;
static UIBE_CONTAINER*          con2;
static PATHFINDER::CONFIGURATION cfg;

static void 
build_walls
(
    SCENE& , 
    const RGB_COLOR& 
);

static GRAPHIC_MODEL*
create_cursor_model
();

static GRAPHIC_MODEL*
create_vector_model
();


/* SCENE */
/* ========================================================================== */
void SCENE::init(){
	
	uibe_init();
	
	game_scene = get_current_scene();
	
	build_walls(*game_scene, RGB_COLOR(0.0f, 0.5f, 0.1f));

	unit = spawn(
		new GAME_OBJECT_PLAYER(), 
		vector2d(-5.0, 0.0), 
		create_normal()
	);

	scan_model = COLLISION_MODEL(VERTEX_RECTANGLE(1.0, 1.0));
	
	scan_filter.inverse = true;
	scan_filter.pointers.push_back(unit);
	
	cfg.evaluator 		= PATHFINDER::EV_ASTAR;
	cfg.width			= 30;
	cfg.height			= 30;
	cfg.filter			= scan_filter;
	cfg.model			= scan_model;
	cfg.scene			= game_scene;
	cfg.transform		= TRNSF_OBJECT(
		vector2d(0.0, 0.0),
		vector2d(0.0, 0.0),
		vector2d(
			0.5* (double)cfg.width, 
			0.5* (double)cfg.height
		)
	);
	
	if(!pathfinder.configure(cfg))
		printf("pathfinder initialization error!\n");
	
	vis_path.colors[0] = color_start_point;
	vis_path.colors[1] = color_end_point;

    vis_pathfinder.transform    = cfg.transform;
    vis_pathfinder.cl_undefine  = RGB_COLOR(0.5, 0.5, 0.0); 

    con1 = uibe_create_container();
    con1->set_position(unit->get_position());
    con1->set_normal(unit->get_normal());

    con2 = uibe_create_container();
}

void SCENE::free(){

	uibe_delete_container(con1);
    uibe_delete_container(con2);
	uibe_free();
}

void SCENE::compute(double Frame_time){
	VECTOR2D				start_point;
	VECTOR2D				end_point;
	VECTOR2D				in_scene_cursor_coord;
	std::vector<VECTOR2D>	waypoints;
	TRNSF_OBJECT            trnsf;

	uibe_compute(Frame_time);
	
    unit->set_position(con1->get_position());
    unit->set_normal(con1->get_normal());

    trnsf = con2->transform;
    trnsf.set_scale(cfg.transform.get_scale());

    pathfinder.set_transform(trnsf);
    vis_pathfinder.transform = trnsf;

	in_scene_cursor_coord = 
		game_scene->translate_to_scene(uibe_get_mouse_coord());
		
	if(key_front_down(KEY_MOUSE_R)){
		start_point = unit->get_position();
		end_point 	= in_scene_cursor_coord;
		
		waypoints = pathfinder.get_path(start_point, end_point);
		if(waypoints.size() != 0 ){
			
			vis_path.path = waypoints;
			vis_path.visualize();

            vis_pathfinder.grid = pathfinder.get_grid();
			
		}else{
			printf("empty path!\n");
		}
	}

    vis_pathfinder.visualize();
}

void SCENE::pre_render(){
	
}

void SCENE::event(const EVENT_MSG& Event){
	uibe_event(Event);

}


/* UTILIT PROCEDURES */
/* ========================================================================== */
static void build_walls(SCENE& Scene,const RGB_COLOR& Color){
	GAME_OBJECT* wall;
	
	struct wall_info{
		VECTOR2D	position;
		VECTOR2D	normal;
		double		width;
		double 		height;
	}walls_info[] = {
		{vector2d( 3.0, 0.0)	, vector2d(1.0, 0.0), 	1.0, 3.0},
		{vector2d( 3.0, 5.0)	, vector2d(1.0, 0.0), 	2.0, 2.0},
		{vector2d(-3.0,-5.0)	, vector2d(1.0, 0.0), 	2.0, 2.0},
		{vector2d(-5.0, 5.0)	, vector2d(1.0, 1.0), 	4.0, 2.0},
		{vector2d( 5.0,-5.0)	, vector2d(1.0, 1.0), 	4.0, 2.0},
		
		{vector2d( 0.0  , 8.75)	, create_normal(), 		30.0, 0.5},
		{vector2d( 0.0  ,-8.75)	, create_normal(), 		30.0, 0.5},
		{vector2d(-14.75, 0.0)	, create_normal(), 		0.5 , 17.0},
		{vector2d( 14.75, 0.0)	, create_normal(), 		0.5 , 17.0}
	};
	
	for(UINT i = 0; i < (sizeof(walls_info)/sizeof(wall_info)); i++){
		wall = new GAME_OBJECT_WALL(
			walls_info[i].width, 
			walls_info[i].height, 
			Color
		);
		
		if(Scene.spawn( wall, walls_info[i].position, walls_info[i].normal))
			uibe_interactive_object_alloc(wall);
	}
	
}

static GRAPHIC_MODEL*
create_cursor_model
(){
	GRAPHIC_MODEL* 		model;
	GELEMENT*			lgroup;
	
	model 	= create_vector_model();
	lgroup	= *(model->begin());
	lgroup->set_position(
		rotate_vector(
			create_normal(),
			-M_PI/4
		)
	);
	lgroup->set_normal(
		create_normal() * -1.0
	);
	
	return model;
}

static GRAPHIC_MODEL*
create_vector_model
()
{
	GRAPHIC_MODEL* 		model;
	GELEMENT_LINEGROUP*	lgroup;
	GELEMENT_LINE		line;
	
	model 	= new GRAPHIC_MODEL();
	lgroup	= new GELEMENT_LINEGROUP();
	
	line.set_specs(1.0, 0.0);
	lgroup->push_back(new GELEMENT_LINE(line));
	
	line.set_position(vector2d(1.0, 0.0));
	line.set_specs(0.2, 0.0);
	
	line.set_normal(
		rotate_vector(
			create_normal(),
			3*M_PI/2
		)
	);
	lgroup->push_back(new GELEMENT_LINE(line));
	
	line.set_normal(
		rotate_vector(
			create_normal(),
			-3*M_PI/2
		)
	);
	lgroup->push_back(new GELEMENT_LINE(line));
	
	model->push_back(lgroup);
	
	return model;
}

