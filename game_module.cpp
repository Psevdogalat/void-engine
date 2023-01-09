#include <engine.h>

#include <objects.h>
#include <utilits.h>
#include <keys.h>
#include <controllers.h>

#include <vector_utilits.h>
#include <uibe.h>

#include <cstdlib>
#include <cstdio>
#include <list>
#include <cmath>

using namespace ENGINE;

CAMERA*			game_camera;
SCENE*			game_scene;
bool 			camera_focus 		= false;

PLAYER_CONTROLLER* 				player_controller;
ARTIFICIAL_PLAYER_CONTROLLER*	artificial_player_controller;

GUIELEMENT*			root_gui;

GAME_OBJECT* 		player_spawn;
GAME_OBJECT* 		enemy_spawn;

GAME_OBJECT_PLAYER* player;
RGB_COLOR			player_color(0.0f, 1.0f, 0.0f);

GAME_OBJECT_PLAYER* enemy;
RGB_COLOR			enemy_color(1.0f, 0.3f, 0.3f);

TIMER* start_timer;
double start_time	=	0.5;
bool   round_begin;

UINT   counter_player;
UINT   counter_enemy;

static void build_walls		();
static void init_game		();
static void spawn_player	();
static void spawn_enemy		();
static void start_round		();
static void end_round		();
static void on_player_dead	(GAME_OBJECT* );
static void on_enemy_dead	(GAME_OBJECT* );

static GUI_SPRITE* gui_sprite;

void SCENE::init(){

	uibe_init();
	game_scene 	= (SCENE*)get_current_scene();
	game_camera	= &(game_scene->camera);
	
	player_controller 				= new PLAYER_CONTROLLER();
	artificial_player_controller	= new ARTIFICIAL_PLAYER_CONTROLLER();
	
	
	
	
	root_gui = new GUIELEMENT();
	gui_sprite = new GUI_SPRITE();
	gui_sprite->set_position(vector2d(-(1.5-0.2), 1.0-0.2));
	//gui_sprite->set_position(vector2d(-0.6, 0.5));
	gui_sprite->set_width (0.4);
	gui_sprite->set_height(0.4);
	gui_sprite->set_texture("texture1", 856, 856);
	gui_sprite->set_window(0, 0, 856, 856);
	root_gui->push_back(gui_sprite);
	
	game_scene->gui_root = root_gui;
	
	
	player_spawn = spawn(new GAME_OBJECT_POINT(), vector2d(-10,0), create_normal());
	enemy_spawn  = spawn(new GAME_OBJECT_POINT(), vector2d( 10,0),-create_normal());
	
	build_walls();
	
	init_game();

}

void SCENE::free(){
	
	delete player_controller;
	delete artificial_player_controller;
	delete root_gui;
}

void SCENE::compute(double Frame_time){
	
	
	
	if(round_begin && start_timer->condition()){
		start_round();
		round_begin = false;
	}
	
	if(key_front_down(KEY_D)){
		gui_sprite->set_visible(!gui_sprite->is_visible());
	}
	
	if(key_front_down(KEY_B)){
		if(camera_focus == true){
			game_camera->set_position(vector2d(0.0, 0.0));
			camera_focus = false;
		}else
			camera_focus = true;
	}

	
	player_controller->compute();
	artificial_player_controller->compute();
	
	if(camera_focus){
		if(player)
			game_camera->set_position	(player->get_position());
	}
	
	uibe_compute(Frame_time);
}

void SCENE::pre_render(){
	if(camera_focus){
		if(player)
			game_camera->set_position	(player->get_position());
	}
	
}

void SCENE::event(const EVENT_MSG& Event){
	uibe_event(Event);
	
}

/* ======================================================== */

void build_walls(){
	VECTOR2D 		p_coord; 	
	VECTOR2D 		e_coord;	
	
	p_coord = player_spawn->get_position();
	e_coord = enemy_spawn->get_position();
	
	GAME_OBJECT* wall;
	
	struct wall_info{
		bool 		is_killer;
		VECTOR2D	position;
		VECTOR2D	normal;
		double		width;
		double 		height;
	}walls_info[] = {
		{false, p_coord + vector2d( 0.00, 1.25)	,create_normal() 	, 3.0, 0.5},
		{false, p_coord + vector2d( 0.00,-1.25)	,create_normal() 	, 3.0, 0.5},
		{false, p_coord + vector2d(-1.25, 0.00)	,create_normal() 	, 0.5, 2.0},
		{false, e_coord + vector2d( 0.00, 1.25)	,create_normal() 	, 3.0, 0.5},
		{false, e_coord + vector2d( 0.00,-1.25)	,create_normal() 	, 3.0, 0.5},
		{false, e_coord + vector2d( 1.25, 0.00)	,create_normal() 	, 0.5, 2.0},
			   
		{false, vector2d( 0.0, 0.0)				, vector2d(1.0, 0.0), 1.0, 3.0},
		{false, vector2d( 3.0, 5.0)				, vector2d(1.0, 0.0), 2.0, 2.0},
		{false, vector2d(-3.0,-5.0)				, vector2d(1.0, 0.0), 2.0, 2.0},
		{false, vector2d(-5.0, 5.0)				, vector2d(1.0, 1.0), 4.0, 2.0},
		{false, vector2d( 5.0,-5.0)				, vector2d(1.0, 1.0), 4.0, 2.0},
			   
		{true , vector2d( 0.0  , 8.75)				, create_normal(), 30.0, 0.5},
		{true , vector2d( 0.0  ,-8.75)				, create_normal(), 30.0, 0.5},
		{true , vector2d(-14.75, 0.0)				, create_normal(), 0.5 , 17.0},
		{true , vector2d( 14.75, 0.0)				, create_normal(), 0.5 , 17.0}
	};
	
	for(UINT i = 0; i < (sizeof(walls_info)/sizeof(wall_info)); i++){
		wall = spawn(
			new GAME_OBJECT_WALL(walls_info[i].width, walls_info[i].height, RGB_COLOR(0.0f, 0.5f, 0.0f)), 
			walls_info[i].position, 
			walls_info[i].normal
		);
		
		if(walls_info[i].is_killer)
			wall->set_name("bullet_killer");
	}
	
}

void init_game(){
	start_timer = create_timer();
	start_timer->set(start_time);
	
	round_begin 	= true;
	counter_enemy 	= 0;
	counter_player	= 0;
	
	player 	= nullptr;
	enemy	= nullptr;
}	

void on_player_dead	(GAME_OBJECT* );
void on_enemy_dead	(GAME_OBJECT* );

void spawn_player(){
	player 	= new GAME_OBJECT_PLAYER();
	player->set_color(player_color);
	player->set_on_dead(on_player_dead);
	player_controller->set_control_unit(player);
	artificial_player_controller->set_enemy_unit(player);
	spawn(player, player_spawn->get_position(), player_spawn->get_normal());
}

void spawn_enemy(){
	enemy 	= new GAME_OBJECT_PLAYER();
	enemy->set_color(enemy_color);
	enemy->set_on_dead(on_enemy_dead);
	artificial_player_controller->set_control_unit(enemy);
	spawn(enemy, enemy_spawn->get_position(),enemy_spawn->get_normal());

}

void start_round(){
	if(!player)
		spawn_player();
	//player->set_invul(true);
	
	if(!enemy)
		spawn_enemy();
	//enemy->set_invul(true);
	
}

void end_round(){
	round_begin = true;
	start_timer->set(start_time);
	/*
	despawn(player);
	despawn(enemy);
	*/
	
	
	
	printf("Player % d Enemy % d\n",counter_player, counter_enemy);

}

void on_player_dead(GAME_OBJECT* Killer){
	GAME_OBJECT_BULLET* bullet;
	if(Killer->get_type() == GOT_BULLET){
		bullet = (GAME_OBJECT_BULLET*) Killer;
		if(bullet->get_owner() == player){
			counter_player--;
		}
		
		if(bullet->get_owner() == enemy){
			counter_enemy++;
		}
		
		player_controller->set_control_unit(nullptr);
		artificial_player_controller->set_enemy_unit(nullptr);
		player = nullptr;
	}
	
	end_round();
}

void on_enemy_dead(GAME_OBJECT* Killer){
	GAME_OBJECT_BULLET* bullet;
	if(Killer->get_type() == GOT_BULLET){
		bullet = (GAME_OBJECT_BULLET*) Killer;
		if(bullet->get_owner() == enemy){
			counter_enemy--;
		}
		
		if(bullet->get_owner() == player){
			counter_player++;
		}
		
		artificial_player_controller->set_control_unit(nullptr);
		enemy = nullptr;
	}
	
	end_round();
}
