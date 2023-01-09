#include <engine.h>
#include <graphics.h>
#include <memory_manage.h>
#include <png_images.h>
#include <gui.h>

#include <gl/glew.h>
#include <cppfs/fs.h>
#include <cppfs/FileHandle.h>
#include <cppfs/FileIterator.h>
#include <cppfs/FilePath.h>

#include <list>
#include <unordered_map>
#include <string>
#include <cstring>
#include <algorithm>
#include <cstdio>

using namespace ENGINE;

/* GL raw structs */
typedef struct {
	GLfloat x;
	GLfloat y;
}GLVECTOR2D;

typedef struct {
	GLfloat cells[3][3];
}GLMATRIX33;

typedef struct{
	GLfloat r;
	GLfloat g;
	GLfloat b;
}GLRGB;

typedef struct{
	GLVECTOR2D 	coord;
	GLRGB		color;
}GLVATTR_DEF;

typedef struct{
	GLVECTOR2D 	coord;
	GLRGB		color;
	GLVECTOR2D	uv;
}GLVATTR_COT; 

typedef struct{
	GLVECTOR2D pos;
	GLVECTOR2D nor;
	GLVECTOR2D scl;
}GLTRANS;


#define __GLFASTCALL__ inline

__GLFASTCALL__ static GLVECTOR2D 	
glvector2d	
(const VECTOR2D&  );

__GLFASTCALL__ static GLRGB			
glrgb		
(const RGB_COLOR& );

__GLFASTCALL__ static GLRGB 		
glrgb_blend	
(const GLRGB&, const GLRGB& );

__GLFASTCALL__ static GLVATTR_DEF 	
glvattr_def	
(const VECTOR2D& , const RGB_COLOR& );

__GLFASTCALL__ static GLVATTR_COT 	
glvattr_cot	
(const VECTOR2D& , const RGB_COLOR& , const VECTOR2D& );

__GLFASTCALL__ static GLMATRIX33	
glmat_rot	
(const GLVECTOR2D& );

__GLFASTCALL__ static GLMATRIX33	
glmat_scl	
(const GLVECTOR2D& );

__GLFASTCALL__ static GLMATRIX33	
glmat_trp	
(const GLVECTOR2D& );

__GLFASTCALL__ static GLMATRIX33	
operator*	
(const GLMATRIX33&, const GLMATRIX33& );

__GLFASTCALL__ static GLVECTOR2D	
operator*	
(const GLMATRIX33&, const GLVECTOR2D& );

__GLFASTCALL__ static GLMATRIX33	
glmat_trans	
(const GLVECTOR2D&, const GLVECTOR2D&, const GLVECTOR2D& );

/* render control ============================================================*/
static GLint 			gl_version[2];
static bool 			gl_core_profile;
static bool				draw_graphic_models;
static bool 			draw_collision_models;
static bool 			draw_physical_models;
static bool				draw_ui;
static RGB_COLOR		collision_model_color;
static RGB_COLOR		physical_model_color;
static RGB_COLOR 		default_color;
static VECTOR2D			camera_proportion;

static GLuint			gl_vertex_buffer; 	//vbo
static GLuint			gl_element_buffer;	//ebo
static GLuint			gl_vao;				//vertex array object
static GLuint			gl_vao_color_tex;	//vao for colored textured objects

#define INDEX_DEF_SHAD_PROG 0
#define INDEX_COT_SHAD_PROG 1

typedef struct{
	GLuint 		id;
	const char* str_id;
	const char* vshad_src;
	const char* fshad_src;
}SHAD_PROG_INFO;

static const char* def_vshad_src = 
	"#version 330 core 					 		\n"
	"layout (location = 0) in vec2 in_coord; 	\n"
	"layout (location = 1) in vec3 in_color;	\n"
	"out 		vec4 color;						\n"
	"uniform 	mat3 trans;						\n"
	"											\n"
	"void main() 						 		\n"
	"{ 									 		\n"
	"	vec3 vcoord = trans * vec3(in_coord, 1.0);		\n"
	"												\n"
	"	gl_Position = vec4(vcoord.xy, 0.0, 1.0);	\n"
	"	color = vec4(in_color, 1.0);				\n"
	"}";
		
static const char* def_fshad_src = 
	"#version 330 core			\n"
	"in vec4 color;				\n"
	"out vec4 FragColor;		\n"
	"							\n"
	"void main()				\n"
	"{							\n"
	"	gl_FragColor = color;	\n"
	"}; ";
	
static const char* cot_vshad_src = 
	"#version 330 core 					 		\n"
	"layout (location = 0) in vec2 in_coord; 	\n"
	"layout (location = 1) in vec3 in_color;	\n"
	"layout (location = 2) in vec2 in_tex_coord;\n"
	"out 		vec4 color;						\n"
	"out 		vec2 tex_coord;					\n"
	"uniform 	mat3 trans;						\n"
	"											\n"
	"void main() 						 		\n"
	"{ 									 		\n"
	"	vec3 vcoord = trans * vec3(in_coord, 1.0);	\n"
	"												\n"
	"	gl_Position = vec4(vcoord.xy, 0.0, 1.0);	\n"
	"	color 		= vec4(in_color, 1.0); 			\n"
	"	tex_coord 	= vec2(in_tex_coord.x, 1.0f - in_tex_coord.y);\n"
	"}";
	
static const char* cot_fshad_src = 
	"#version 330 core			\n"
	"in vec4 color;				\n"
	"in vec2 tex_coord;			\n"
	"uniform sampler2D in_tex;	\n"
	"out vec4 FragColor;		\n"
	"void main()				\n"
	"{							\n"
	"	FragColor = texture(in_tex, tex_coord) * color;\n"
	"}; ";
	
static SHAD_PROG_INFO shad_progs[] = {
	{0, "default"		, def_vshad_src, def_fshad_src },
	{0, "color_texture"	, cot_vshad_src, cot_fshad_src }
};



static bool			init_shaders					();
static void 		core_profile_reder				();
static void 		compatibility_profile_render	();

/*
static void 		
build_primitives				
(VECTOR_OBJECT*, COLOR_LIST&, COLOR_LIST::iterator&, const GLMATRIX33&);
*/

static void 		
build_primitives_POINT			
(VECTOR_POINT*, 	GLRGB, const GLMATRIX33& );

static void 		
build_primitives_POLYLINE		
(VECTOR_POLYLINE*, 	GLRGB, const GLMATRIX33& );

static void 		
build_primitives_SHAPE			
(VECTOR_SHAPE*, 	GLRGB, const GLMATRIX33& );

static void 		
build_primitives_CIRCLE			
(VECTOR_CIRCLE*, 	GLRGB, const GLMATRIX33& );

static void 		
build_primitives_collision_model
(COLLISION_MODEL*,  GLRGB, const GLMATRIX33& );

static GLMATRIX33  	build_tmat_vo					
(VECTOR_OBJECT* Object);

static GLMATRIX33	
build_gelement_tmat	
(GELEMENT* );

static void 
build_element
(GELEMENT* 				,const GLRGB& ,const GLMATRIX33& );

static void 
build_element
(GELEMENT_GROUP* 		,const GLRGB& ,const GLMATRIX33& );

static void 
build_element
(GELEMENT_LINE* 		,const GLRGB& ,const GLMATRIX33& );

static void 
build_element
(GELEMENT_RECT* 		,const GLRGB& ,const GLMATRIX33& );

static void 
build_element
(GELEMENT_CIRCLE* 		,const GLRGB& ,const GLMATRIX33& );

static void 
build_element
(GELEMENT_POLYLINE*		,const GLRGB& ,const GLMATRIX33& );

static void 
build_element
(GELEMENT_SHAPE* 		,const GLRGB& ,const GLMATRIX33& );

static void 
build_element
(GELEMENT_SPRITE* 		,const GLRGB& ,const GLMATRIX33& );

static void 
build_element
(GELEMENT_LINEGROUP* 	,const GLRGB& ,const GLMATRIX33& );

static void 
build_element
(GELEMENT_RECTGROUP* 	,const GLRGB& ,const GLMATRIX33& );

static void 
build_guielement 
(GUIELEMENT* , const GLMATRIX33&);

#define DEFAULT_TEXURE_LITERAL	"default"

typedef struct{
	char* 	path;
	char* 	str_id;
	GLuint	gl_id;
	size_t	width;
	size_t 	height;
	GLenum	pixel_type;
}TEXTURE_INFO;
typedef std::unordered_map<std::string, TEXTURE_INFO> TEXTURE_MAP;

static GLuint			gl_default_texure;
static TEXTURE_MAP		textures;
static PNG_RGBA8_IMAGE	png_loader;

static char* 	allocate_cstr		(const std::string& Src);
static void 	init_default_texture();
static void 	init_texture_info	(TEXTURE_INFO& );
static void 	free_texture_info	(TEXTURE_INFO& );
static bool 	load_texture		(const std::string&, const std::string&);
static void 	load_textures		();
static GLuint	get_texture			(const std::string& );

typedef struct{
	GLenum 		type;
	GLuint		texture;
	GLuint 		shader_prog;
	GLuint		vao;
	GLubyte*	attrs;
	GLubyte*	indexes;
	size_t		attrs_size;
	size_t		indexes_size;
	GLMATRIX33*	ptrans;
}PRIMITIVE_INFO;

typedef std::list<PRIMITIVE_INFO> 	PPRIMITIVE_LIST;
typedef std::list<PRIMITIVE_INFO*> 	PPRIMITIVE_PLIST;
typedef std::list<GRAPHIC_MODEL*> 	GMODEL_PLIST;

static PPRIMITIVE_LIST 	primitive_list;
//static PPRIMITIVE_PLIST primitive_list;

static cyclic_mmngr		primitive_buffer(MM_MAX_MEMORY_SIZE, 128*1024);
static GMODEL_PLIST 	model_layers[GLAYERS_NUM];

__GLFASTCALL__ static void 		set_primitive_ptrs	(void* , PRIMITIVE_INFO&);
__GLFASTCALL__ static size_t 	get_primitive_asize	(const PRIMITIVE_INFO&	);

void ENGINE::set_draw_graphic_models(bool Flag){
	draw_graphic_models = Flag;
}

void ENGINE::set_draw_collision_models(bool Flag){
	draw_collision_models = Flag;
}

void ENGINE::set_draw_physical_models(bool Flag){
	draw_physical_models = Flag;
}

bool ENGINE::init_graphic(){
	
	glGetIntegerv(GL_MAJOR_VERSION, &gl_version[0]);
	glGetIntegerv(GL_MINOR_VERSION, &gl_version[1]);
	
	GLint profile_mask;
	glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile_mask);
	gl_core_profile = profile_mask & GL_CONTEXT_CORE_PROFILE_BIT;
	
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_BLEND);
	glEnable(GL_MULTISAMPLE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	if(gl_core_profile){
		
		//init std vao
		glGenVertexArrays(1, &gl_vao);
		glGenBuffers(1, &gl_vertex_buffer);
		glGenBuffers(1, &gl_element_buffer);
		
		glBindVertexArray(gl_vao);
		glBindBuffer(GL_ARRAY_BUFFER, gl_vertex_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_element_buffer);
		
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(0));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		
		glBindVertexArray(0);
		
		//init color texture vao
		glGenVertexArrays(1, &gl_vao_color_tex);
		glBindVertexArray(gl_vao_color_tex);
		glBindBuffer(GL_ARRAY_BUFFER, gl_vertex_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_element_buffer);
		
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(0));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(5*sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		
		glBindVertexArray(0);
		
		init_shaders();
		init_default_texture();
		
		load_textures();
	}
	
	
	default_color 			= RGB_COLOR(0.5f, 0.5f, 0.5f);
	collision_model_color	= RGB_COLOR(1.0f, 1.0f, 0.0f);
	physical_model_color	= RGB_COLOR(0.0f, 1.0f, 0.0f);
	
	draw_graphic_models 	= true;
	draw_collision_models	= false;
	draw_physical_models	= false;
	draw_ui					= true;
	
	return true;
}

void ENGINE::free_graphic(){
	
}

void ENGINE::render_graphic(){	
	
	SCENE* 	scene;
	CAMERA*	camera;
	
	scene 	= (SCENE*)get_current_scene();
	camera 	= &scene->camera;
	
	scene->pre_render();
	
	GRAPHIC_MODEL* 			graphic_model;
	
	VECTOR2D				camera_position;
	VECTOR_RECTANGLE		camera_rectangle;
	VECTOR2D				camera_reverse_normal;
	double 					camera_scale;
	
	GLMATRIX33 				camera_proportion_matrix;
	GLMATRIX33 				scene_trans_matrix;
	GLMATRIX33				model_trans_matrix;
	
	camera_rectangle = (VECTOR_RECTANGLE)*camera;
	camera_rectangle.set_scale(0.999);
	camera_rectangle.set_normal(create_normal());
	camera_rectangle.set_position(vector2d(0,0));
	
	camera_position			= camera->get_position();
	camera_scale 			= camera->get_scale();
	
	camera_reverse_normal	= camera->get_normal();
	camera_reverse_normal.y *= -1.0;
	
	camera_proportion.x =  camera->get_height()/camera->get_width();
	camera_proportion.y =  1.0;
	//camera_proportion 	*= 2.0;
	
	camera_proportion_matrix = glmat_scl(glvector2d(camera_proportion));
	
	scene_trans_matrix = camera_proportion_matrix * 
		glmat_scl((GLVECTOR2D){(GLfloat)camera_scale, (GLfloat)camera_scale}) *  
		glmat_rot(glvector2d(camera_reverse_normal)) *
		glmat_trp(glvector2d(-camera_position));

	
	if(draw_graphic_models){
		for(GAME_OBJECT* game_object: scene->game_objects){
			
			if(!game_object->is_visible())
				continue;
			
			graphic_model = game_object->get_graphic_model();
			if(graphic_model == nullptr)
				continue;
			
			graphic_model->set_position	(game_object->get_position());
			graphic_model->set_normal	(game_object->get_normal());
			
			model_layers[graphic_model->get_layer()].push_back(graphic_model);
		}
		
		for(UINT i=0; i<GLAYERS_NUM; i++){
			for(GRAPHIC_MODEL* model : model_layers[i]){
				model_trans_matrix = scene_trans_matrix * glmat_trans(
						glvector2d(model->get_position()),
						glvector2d(model->get_normal()),
						glvector2d(model->get_scale())
					);
					
				for(GELEMENT* graphic_elemet: *model)				
					build_element(
						graphic_elemet,
						glrgb(model->get_color()),
						model_trans_matrix
					);
			}
			
			model_layers[i].clear();
		}
	
		
	}
	
	/*
	if(draw_graphic_models){
		for(GAME_OBJECT* game_object: scene->game_objects){
			
			if(!game_object->is_visible())
				continue;
			
			graphic_model = game_object->get_graphic_model();
			if(graphic_model == nullptr)
				continue;
			
			model_trans_matrix = scene_trans_matrix * glmat_trans(
					glvector2d(game_object->get_position()),
					glvector2d(game_object->get_normal()),
					glvector2d(graphic_model->get_scale())
				);
				
			for(GELEMENT* graphic_elemet: *graphic_model)				
				build_element(
					graphic_elemet,
					glrgb(graphic_model->get_color()),
					model_trans_matrix
				);
		}
		
	}
	*/
	
	COLLISION_NODE* collision_node;
	if(draw_collision_models)
		for(GAME_OBJECT* game_object: scene->game_objects){
			if(game_object->is_collisible()){
				collision_node = game_object->get_collision_node();
				build_primitives_collision_model(collision_node->get_collision_model(),
					glrgb(collision_model_color),
					scene_trans_matrix * glmat_trans(
						glvector2d(game_object->get_position()),
						glvector2d(game_object->get_normal()),
						(GLVECTOR2D){1.0, 1.0}
					)
				);
			}
		}
		
	if(draw_physical_models)
		for(GAME_OBJECT* game_object: scene->game_objects){
			
		}
	
	if(draw_ui && scene->gui_root != nullptr)
		for(GUIELEMENT* guielement: *scene->gui_root)
			build_guielement(guielement, camera_proportion_matrix);
	
	
	build_primitives_SHAPE(&camera_rectangle, (GLRGB){1.0f, 0.0f, 0.0f},
		glmat_scl(glvector2d(camera_proportion * 2.0)) 
	);
	
	gl_core_profile? 
		core_profile_reder(): 
		compatibility_profile_render();
	
	primitive_list.clear();
	primitive_buffer.clear();
	
}

static void init_texture_info(TEXTURE_INFO& Info){
	Info.path 	= nullptr;
	Info.str_id = nullptr;
}

static void free_texture_info(TEXTURE_INFO& Info){
	if(Info.path != nullptr)
		free(Info.path);
	
	if(Info.str_id != nullptr)
		free(Info.str_id);
}

static char* allocate_cstr(const std::string& Src){
	char* 	alloc_str;
	size_t	buffer_size;
	
	buffer_size = Src.size() + 1; 
	alloc_str 	= (char*) malloc(buffer_size);
	memcpy(alloc_str, Src.c_str(), buffer_size);
	
	return alloc_str;
}

static void load_textures(){
	
	std::string 		stext_dir_path;
	cppfs::FileHandle 	text_dir;
	cppfs::FileHandle	text_file;
	cppfs::FilePath		text_dir_path;
	cppfs::FilePath		text_file_path;
	
	get_absolute_path(stext_dir_path, "textures/");
	text_dir_path	= stext_dir_path;
	text_dir		= cppfs::fs::open(text_dir_path.path());
	
	if(text_dir.exists() && text_dir.isDirectory()){	
	
		for(cppfs::FileIterator it = text_dir.begin(); it != text_dir.end(); ++it){
			text_file_path = text_dir.path() + *it;
			
			text_file = cppfs::fs::open(text_file_path.path());
			if(text_file.exists() && text_file.isFile() && text_file_path.extension() == ".png"){
				
				if(load_texture(text_file_path.path(), text_file_path.baseName())){
					printf("texture %s sucsesfully loaded\n", text_file_path.path().c_str());
				}else
					printf("texture [%s] load error\n", text_file_path.path().c_str());
			}
		}
	}
}

static bool load_texture(const std::string& Path, const std::string& Str_id){
	TEXTURE_INFO	info;
	TEXTURE_INFO	replace_info;
	
	if(png_loader.load(Path.c_str())){
		
		init_texture_info(info);
		info.path 		= allocate_cstr(Path);
		info.str_id 	= allocate_cstr(Str_id);
		info.width		= png_loader.get_width();
		info.height		= png_loader.get_height();
		info.pixel_type = GL_RGBA;
		
		glGenTextures(1, &info.gl_id);
		glBindTexture(GL_TEXTURE_2D, info.gl_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		glTexImage2D(
			GL_TEXTURE_2D, 0, info.pixel_type, info.width, info.height, 0,
			info.pixel_type, GL_UNSIGNED_BYTE, png_loader.get_buffer()
		);
		
		TEXTURE_MAP::iterator it;
		it = textures.find(Str_id);
		if(it != textures.end()){
			replace_info = it->second;
			printf("texture %s replaced by %s\n\t last path %s \n", 
				info.str_id, info.path, replace_info.path);
				
			glDeleteTextures(1,&replace_info.gl_id);
			free_texture_info(replace_info);
		}
		textures[Str_id] = info;
		
		return true;
	}
	
	return false;
}

static void init_default_texture(){
	TEXTURE_INFO info;
	static GLubyte buffer[]={
		0xff, 0x00, 0xff, 0xff,
		0x00, 0x00, 0x00, 0xff,
		0x00, 0x00, 0x00, 0xff,
		0xff, 0x00, 0xff, 0xff
	};
	
	init_texture_info(info);
	info.path 		= allocate_cstr("");
	info.str_id 	= allocate_cstr(DEFAULT_TEXURE_LITERAL);
	info.width		= 2;
	info.height		= 2;
	info.pixel_type = GL_RGBA;
	
	glGenTextures(1, &info.gl_id);
	glBindTexture(GL_TEXTURE_2D, info.gl_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTexImage2D(
		GL_TEXTURE_2D, 0, info.pixel_type, info.width, info.height, 0,
		info.pixel_type, GL_UNSIGNED_BYTE, buffer
	);
	
	gl_default_texure = info.gl_id;
	
	textures[DEFAULT_TEXURE_LITERAL] = info;
}

static GLuint get_texture(const std::string& Str_id){
	TEXTURE_MAP::iterator 	it;
	TEXTURE_INFO 			info;
	
	it = textures.find(Str_id);
	if(it != textures.end()){
		return it->second.gl_id;
	}
		
	return gl_default_texure;
}

static bool init_shaders(){
	
	int 	succes;
	char	info_log[512];

	#define SHAD_INFOS_SIZE 2
	#define INDEX_VERTEX_SHAD 	0
	#define INDEX_FRAGMENT_SHAD 1
	struct __SHAD_INFO{
		GLenum			type;
		GLuint			id;
		char*			str_id;
		const char**	src;
	}shad_infos[SHAD_INFOS_SIZE] = {
		{GL_VERTEX_SHADER, 		0, (char*)"vertex_shader"	, nullptr},
		{GL_FRAGMENT_SHADER, 	0, (char*)"fragment_shader"	, nullptr}
	};
	
	SHAD_PROG_INFO* prog_info;
	__SHAD_INFO*	shad_info;
	for(UINT i = 0; i < sizeof(shad_progs)/sizeof(SHAD_PROG_INFO); i++){
		prog_info = &shad_progs[i];
		
		printf("init shader programm [%s] ...\n", prog_info->str_id);
		
		shad_infos[INDEX_VERTEX_SHAD	].src = &prog_info->vshad_src;
		shad_infos[INDEX_FRAGMENT_SHAD	].src = &prog_info->fshad_src;
		
		UINT ishad;
		
		for(ishad = 0; ishad < SHAD_INFOS_SIZE; ishad++){
			shad_info = &shad_infos[ishad];
			
			shad_info->id = glCreateShader(shad_info->type);
			glShaderSource	(shad_info->id, 1, shad_info->src, nullptr);
			glCompileShader	(shad_info->id);
			glGetShaderiv	(shad_info->id,GL_COMPILE_STATUS, &succes);
				
			if(succes) continue;
				
			glGetShaderInfoLog(shad_info->id, 512, nullptr, info_log);
			printf("%s compile error\n %s\n", shad_info->str_id, info_log);
			
			ishad++;
			for(;ishad-- > 0;) glDeleteShader(shad_infos[ishad].id);
			break;
		}
		
		if(!succes)break;
		
		prog_info->id = glCreateProgram();
		for(ishad = 0; ishad < SHAD_INFOS_SIZE; ishad++)
			glAttachShader(prog_info->id, shad_infos[ishad].id);
		
		glLinkProgram(prog_info->id);
		glGetProgramiv(prog_info->id, GL_LINK_STATUS, &succes);
		
		if(succes){
			printf("shader_program %s link succes\n", prog_info->str_id);
			for(;ishad-- > 0;) glDeleteShader(shad_infos[ishad].id);
			continue;
		}
		
		glGetProgramInfoLog(prog_info->id, 512, nullptr, info_log);
		printf("shader_program %s link error\n %s\n", prog_info->str_id, info_log);
		
		for(;ishad-- > 0;) glDeleteShader(shad_infos[ishad].id);
		glDeleteProgram(prog_info->id);
		break;
	}
	
	if(succes)
		return true;
		
	return false;
}

static void core_profile_reder(){
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	GLuint 		shad_prog 	= 0;
	for(PRIMITIVE_INFO info: primitive_list){
		glBindVertexArray	(info.vao);
		
		if(shad_prog != info.shader_prog){
			shad_prog = info.shader_prog;
			glUseProgram(shad_prog);
		}
		
		glUniformMatrix3fv(
			glGetUniformLocation(shad_prog,"trans"), 1, true,
			(const GLfloat*) info.ptrans
		);
		
		if(shad_prog == shad_progs[INDEX_COT_SHAD_PROG].id)
			glBindTexture(GL_TEXTURE_2D, info.texture);
		
		
		glBufferData	(GL_ARRAY_BUFFER, 			info.attrs_size, 	info.attrs, 	GL_STREAM_DRAW);
		glBufferData	(GL_ELEMENT_ARRAY_BUFFER, 	info.indexes_size, 	info.indexes, 	GL_STREAM_DRAW);
		glDrawElements	(info.type, info.indexes_size/sizeof(GLuint), GL_UNSIGNED_INT, 0);
		if(glGetError() != GL_NO_ERROR){
			printf("Render error \n");
		}			
	}
	
}

static void compatibility_profile_render(){
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();

	UINT 		gl_paint_command;
	RGB_COLOR	color;
	//printf("primitives: %d \n",primitive_list.size());
	/*
	for(GRAPHIC_VECTOR_PRIMITIVE* primitive: primitive_list){
		switch(primitive->type){
			case GRAPHIC_VECTOR_PRIMITIVE::T_POLYLINE:
				gl_paint_command = GL_LINE_STRIP;
			break;
			case GRAPHIC_VECTOR_PRIMITIVE::T_POLYGON:
				gl_paint_command = GL_LINE_LOOP;
			break;
			default:
				gl_paint_command = GL_LINE_STRIP;
		}
		color = primitive->color;
		
		glColor3f(color.r, color.g, color.b);   
		glBegin(gl_paint_command);
			for(VECTOR2D vertex :primitive->verteces){
				vertex = scale_vector(vertex, camera_proportion);
				glVertex2f(vertex.x, vertex.y);
			}
			
		glEnd();
		
	}
	*/
	glPopMatrix();
	
}

static GLMATRIX33 build_tmat_vo(VECTOR_OBJECT* Object){
	return glmat_trans(glvector2d(Object->get_position()), glvector2d(Object->get_normal()),
		glvector2d((VECTOR2D){1.0, 1.0} * Object->get_scale())
	);
}

/*
static void build_primitives(VECTOR_OBJECT* Object,COLOR_LIST& Color_list, COLOR_LIST::iterator& Color,
	const GLMATRIX33& Trans){

	RGB_COLOR				color;
	COLOR_LIST::iterator 	next_color;
	
	color = (Color != Color_list.end())?*Color:default_color;

	switch(Object->get_type()){
		case VECTOR_OBJECT::T_COMPOSITE:
			for(VECTOR_OBJECT* sub_object: ((VECTOR_COMPOSITE*)Object)->get_components()){
				build_primitives(sub_object, Color_list, Color,  Trans * build_tmat_vo(Object));
			}
			return;
		break;
		case VECTOR_OBJECT::T_POINT:
			build_primitives_POINT((VECTOR_POINT*)Object, 		glrgb(color), Trans);
		break;
		case VECTOR_OBJECT::T_LINE:
		case VECTOR_OBJECT::T_POLYLINE:
			build_primitives_POLYLINE((VECTOR_POLYLINE*)Object, glrgb(color), Trans);
		break;
		case VECTOR_OBJECT::T_SHAPE:
		case VECTOR_OBJECT::T_RECTANGLE:
			build_primitives_SHAPE((VECTOR_SHAPE*)Object, 		glrgb(color), Trans);
		break;
		case VECTOR_OBJECT::T_CIRCLE:
			build_primitives_CIRCLE((VECTOR_CIRCLE*)Object, 	glrgb(color), Trans);
		break;
	}
	
	next_color = Color;
	if(++next_color != Color_list.end())
		Color++;
}
*/

static void build_primitives_POINT(VECTOR_POINT* Object, GLRGB Color, const GLMATRIX33& Trans){
	PRIMITIVE_INFO 	info;
	GLuint			nvertex;
	void* 			amem;
	GLVATTR_DEF*	pattrs;
	GLuint*			pindexes;
	
	const GLfloat coord = 0.01;
	
	nvertex	= 4;
	
	memset(&info, 0, sizeof(PRIMITIVE_INFO));
	info.type 			= GL_LINES;
	info.vao			= gl_vao;
	info.shader_prog	= shad_progs[INDEX_DEF_SHAD_PROG].id;
	info.attrs_size		= nvertex * sizeof(GLVATTR_DEF	);
	info.indexes_size	= nvertex * sizeof(GLuint		);
	
	amem = primitive_buffer.alloc(get_primitive_asize(info));
	if(amem){
		set_primitive_ptrs(amem, info);
		pattrs 			= (GLVATTR_DEF*) 	info.attrs;
		pindexes		= (GLuint*)			info.indexes;
		
		*info.ptrans	=  Trans * build_tmat_vo(Object);
			
		pattrs[0] = (GLVATTR_DEF){(GLVECTOR2D){-coord, coord}, Color};
		pattrs[1] = (GLVATTR_DEF){(GLVECTOR2D){ coord, coord}, Color};
		pattrs[2] = (GLVATTR_DEF){(GLVECTOR2D){ coord,-coord}, Color};
		pattrs[3] = (GLVATTR_DEF){(GLVECTOR2D){-coord,-coord}, Color};
		
		pindexes[0] = 0;
		pindexes[1] = 2;
		pindexes[2] = 1;
		pindexes[3] = 3;
		
		primitive_list.push_back(info);
	}
}

static void build_primitives_POLYLINE(VECTOR_POLYLINE* Object, GLRGB Color, const GLMATRIX33& Trans){
	PRIMITIVE_INFO 	info;
	GLuint			nvertex;
	void* 			amem;
	GLVATTR_DEF*	pattrs;
	GLuint*			pindexes;
	
	VECTOR2D 		vertex;
	
	nvertex		= Object->get_vertices_quantity();
	
	memset(&info, 0, sizeof(PRIMITIVE_INFO));
	info.type 			= GL_LINE_STRIP;
	info.shader_prog	= shad_progs[INDEX_DEF_SHAD_PROG].id;
	info.vao			= gl_vao;
	info.attrs_size		= nvertex * sizeof(GLVATTR_DEF	);
	info.indexes_size	= nvertex * sizeof(GLuint		);
	
	amem = primitive_buffer.alloc(get_primitive_asize(info));
	if(amem){
		set_primitive_ptrs(amem, info);
		pattrs 			= (GLVATTR_DEF*) 	info.attrs;
		pindexes		= (GLuint*)			info.indexes;
		
		*info.ptrans	=  Trans * build_tmat_vo(Object);
		
		for(UINT ivertex = 0; ivertex < nvertex; ivertex++){
			Object->get_vertex(ivertex,vertex);
			pattrs	[ivertex] = (GLVATTR_DEF){glvector2d(vertex), Color};
			pindexes[ivertex] = ivertex;
		}
		
		primitive_list.push_back(info);
	}
}

static void build_primitives_SHAPE(VECTOR_SHAPE* Object, GLRGB Color, const GLMATRIX33& Trans){
	PRIMITIVE_INFO 	info;
	GLuint			nvertex;
	void* 			amem;
	GLVATTR_DEF*	pattrs;
	GLuint*			pindexes;
	
	VECTOR2D 		vertex;
	
	nvertex		= Object->get_vertices_quantity();
	
	memset(&info, 0, sizeof(PRIMITIVE_INFO));
	info.type 			= GL_LINE_LOOP;
	info.shader_prog	= shad_progs[INDEX_DEF_SHAD_PROG].id;
	info.vao			= gl_vao;
	info.attrs_size		= nvertex * sizeof(GLVATTR_DEF	);
	info.indexes_size	= nvertex * sizeof(GLuint		);
	
	amem = primitive_buffer.alloc(get_primitive_asize(info));
	if(amem){
		set_primitive_ptrs(amem, info);
		pattrs 			= (GLVATTR_DEF*) 	info.attrs;
		pindexes		= (GLuint*)			info.indexes;
		
		*info.ptrans	=  Trans * build_tmat_vo(Object);
		
		for(UINT ivertex = 0; ivertex < nvertex; ivertex++){
			Object->get_vertex(ivertex,vertex);
			pattrs	[ivertex] = (GLVATTR_DEF){glvector2d(vertex), Color};
			pindexes[ivertex] = ivertex;
		}
		
		primitive_list.push_back(info);
	}
}

static void build_primitives_CIRCLE(VECTOR_CIRCLE* Object, GLRGB Color, const GLMATRIX33& Trans){
	PRIMITIVE_INFO 	info;
	GLuint			nvertex;
	void* 			amem;
	GLVATTR_DEF*	pattrs;
	GLuint*			pindexes;
	
	VECTOR2D 		vertex;
	double 	 		radius;
	double 			angle;
	
	nvertex		= 32;
	angle 		= 2*M_PI/nvertex;
	radius 		= Object->get_radius();
	
	memset(&info, 0, sizeof(PRIMITIVE_INFO));
	info.type 			= GL_LINE_LOOP;
	info.shader_prog	= shad_progs[INDEX_DEF_SHAD_PROG].id;
	info.vao			= gl_vao;
	info.attrs_size		= nvertex * sizeof(GLVATTR_DEF	);
	info.indexes_size	= nvertex * sizeof(GLuint		);
	
	amem = primitive_buffer.alloc(get_primitive_asize(info));
	if(amem){
		set_primitive_ptrs(amem, info);
		pattrs 			= (GLVATTR_DEF*) 	info.attrs;
		pindexes		= (GLuint*)			info.indexes;
		
		*info.ptrans	=  Trans * build_tmat_vo(Object);
		
		for(UINT ivertex = 0; ivertex < nvertex; ivertex++){
			vertex = rotate_vector(create_normal(),angle * ivertex) * radius; 
			pattrs	[ivertex] = (GLVATTR_DEF){glvector2d(vertex), Color};
			pindexes[ivertex] = ivertex;
		}
		
		primitive_list.push_back(info);
	}
}


static void build_primitives_collision_model(COLLISION_MODEL* Model, GLRGB Color, const GLMATRIX33& Trans){
	PRIMITIVE_INFO 	info;
	GLuint			nvertex;
	const VECTOR2D*	vertices;
	void* 			amem;
	GLVATTR_DEF*	pattrs;
	GLuint*			pindexes;
	
	
	nvertex		= Model->get_vertices_n();
	vertices	= (VECTOR2D*)Model->get_raw_vertices();
	
	memset(&info, 0, sizeof(PRIMITIVE_INFO));
	info.type 			= GL_LINE_LOOP;
	info.shader_prog	= shad_progs[INDEX_DEF_SHAD_PROG].id;
	info.vao			= gl_vao;
	info.attrs_size		= nvertex * sizeof(GLVATTR_DEF	);
	info.indexes_size	= nvertex * sizeof(GLuint		);
	
	amem = primitive_buffer.alloc(get_primitive_asize(info));
	if(amem){
		set_primitive_ptrs(amem, info);
		pattrs 			= (GLVATTR_DEF*) 	info.attrs;
		pindexes		= (GLuint*)			info.indexes;

		*info.ptrans	=  Trans;
		
		for(UINT ivertex = 0; ivertex < nvertex; ivertex++){
			pattrs	[ivertex] = (GLVATTR_DEF){glvector2d(vertices[ivertex]), Color};
			pindexes[ivertex] = ivertex;
		}
		
		primitive_list.push_back(info);
	}

}

static GLMATRIX33	
build_gelement_tmat
(GELEMENT* Element)
{
	return glmat_trans(
		glvector2d(Element->get_position()), 
		glvector2d(Element->get_normal()),
		glvector2d(Element->get_scale())
	);
}

static void 
build_element 
(
	GELEMENT* 			Element, 
	const GLRGB& 		Color, 
	const GLMATRIX33& 	T_matrix
){
	
	switch(Element->get_type()){
		case GELEMENT_TGROUP	: 
			build_element((GELEMENT_GROUP*	) Element, Color, T_matrix); 
			break;
		
		case GELEMENT_TLINE		: 
			build_element((GELEMENT_LINE*) 		Element, Color, T_matrix); 
			break;
		
		case GELEMENT_TRECT		: 
			build_element((GELEMENT_RECT*) 		Element, Color, T_matrix); 
			break;
		
		case GELEMENT_TCIRCLE	: 
			build_element((GELEMENT_CIRCLE*) 	Element, Color, T_matrix); 
			break;
		
		case GELEMENT_TPOLYLINE	: 
			build_element((GELEMENT_POLYLINE*) 	Element, Color, T_matrix); 
			break;
		
		case GELEMENT_TSHAPE	: 
			build_element((GELEMENT_SHAPE* ) 	Element, Color, T_matrix); 
			break;
		
		case GELEMENT_TSPRITE	: 
			build_element((GELEMENT_SPRITE*) 	Element, Color, T_matrix); 
			break;
		
		case GELEMENT_TLINEGROUP: 
			build_element((GELEMENT_LINEGROUP*) Element, Color, T_matrix); 
			break;
		
				
		case GELEMENT_TRECTGROUP: 
			build_element((GELEMENT_RECTGROUP*) Element, Color, T_matrix); 
			break;
	}
}

static void build_element (GELEMENT_GROUP* Group, const GLRGB& Color, const GLMATRIX33& T_matrix){
	GLMATRIX33 	t_matrix;
	GLRGB	  	color;	
	
	t_matrix = T_matrix * glmat_trans(
		glvector2d(Group->get_position()),
		glvector2d(Group->get_normal()),
		glvector2d(Group->get_scale())
	);
	
	color = glrgb_blend(glrgb(Group->get_color()) ,Color);
	
	for(GELEMENT* element: *Group)
		build_element(element, color, t_matrix);
	
}

static void build_element (GELEMENT_LINE* Element, const GLRGB& Color, const GLMATRIX33& T_matrix){
	PRIMITIVE_INFO 	info;
	GLuint			vertices_n;
	const VECTOR2D* vertices;
	void* 			amem;
	GLVATTR_DEF*	pattrs;
	GLuint*			pindexes;
	GLRGB			color;
	
	vertices_n		= Element->get_vertices_n();
	
	memset(&info, 0, sizeof(PRIMITIVE_INFO));
	info.type 			= GL_LINE_STRIP;
	info.shader_prog	= shad_progs[INDEX_DEF_SHAD_PROG].id;
	info.vao			= gl_vao;
	info.attrs_size		= vertices_n * sizeof(GLVATTR_DEF	);
	info.indexes_size	= vertices_n * sizeof(GLuint		);
	
	amem = primitive_buffer.alloc(get_primitive_asize(info));
	if(amem){
		set_primitive_ptrs(amem, info);
		pattrs 			= (GLVATTR_DEF*) 	info.attrs;
		pindexes		= (GLuint*)			info.indexes;
		
		*info.ptrans	=  T_matrix * build_gelement_tmat(Element);
		
		color = glrgb_blend(glrgb(Element->get_color()) ,Color);
		
		vertices = Element->get_vertices();
		for(UINT i = 0; i < vertices_n; i++){
			pattrs	[i] = (GLVATTR_DEF){
				glvector2d(vertices[i]), color};
			pindexes[i] = i;
		}
		
		primitive_list.push_back(info);
	}
}

static void 
build_element
(
	GELEMENT_RECT* 		Element, 
	const GLRGB& 		Color, 
	const GLMATRIX33& 	T_matrix
){
	PRIMITIVE_INFO 	info;
	GLuint			vertices_n;
	const VECTOR2D* vertices;
	void* 			amem;
	GLVATTR_DEF*	pattrs;
	GLuint*			pindexes;
	GLRGB			color;
	
	vertices_n		= Element->get_vertices_n();
	GLuint indexes[] = {
		0,1,3,
		1,2,3
	};
	
	info.type 			= (Element->is_solid())?GL_TRIANGLES:GL_LINE_LOOP;
	info.shader_prog	= shad_progs[INDEX_DEF_SHAD_PROG].id;
	info.vao			= gl_vao;
	info.attrs_size		= 4 * sizeof(GLVATTR_DEF	);
	info.indexes_size	= ((Element->is_solid())?6:4) * sizeof(GLuint);
	
	amem = primitive_buffer.alloc(get_primitive_asize(info));
	if(amem){
		set_primitive_ptrs(amem, info);
		pattrs 			= (GLVATTR_DEF*) 	info.attrs;
		pindexes		= (GLuint*)			info.indexes;
		
		*info.ptrans	=  T_matrix * build_gelement_tmat(Element);
		
		color = glrgb_blend(glrgb(Element->get_color()) ,Color);
		
		vertices = Element->get_vertices();
		
		if(Element->is_solid()){
			for(UINT i = 0; i < 4; i++){
				pattrs	[i] = (GLVATTR_DEF){
					glvector2d(vertices[i]), color};
			}
			
			for(UINT i = 0; i < 6; i++)
				pindexes[i] = indexes[i];
			
		}else{
			for(UINT i = 0; i < 4; i++){
				pattrs	[i] = (GLVATTR_DEF){
					glvector2d(vertices[i]), color};
					
				pindexes[i] = i;
			}
			
		}
		
		primitive_list.push_back(info);
	}
}

static void 
build_element
(
	GELEMENT_CIRCLE* 	Element, 
	const GLRGB& 		Color, 
	const GLMATRIX33& 	T_matrix
){
	PRIMITIVE_INFO 	info;
	GLuint			vertices_n;
	const VECTOR2D* vertices;
	void* 			amem;
	GLVATTR_DEF*	pattrs;
	GLuint*			pindexes;
	GLRGB			color;
	
	vertices_n		= Element->get_vertices_n();
	
	memset(&info, 0, sizeof(PRIMITIVE_INFO));
	info.type 			= GL_LINE_LOOP;
	info.shader_prog	= shad_progs[INDEX_DEF_SHAD_PROG].id;
	info.vao			= gl_vao;
	info.attrs_size		= vertices_n * sizeof(GLVATTR_DEF	);
	info.indexes_size	= vertices_n * sizeof(GLuint		);
	
	amem = primitive_buffer.alloc(get_primitive_asize(info));
	if(amem){
		set_primitive_ptrs(amem, info);
		pattrs 			= (GLVATTR_DEF*) 	info.attrs;
		pindexes		= (GLuint*)			info.indexes;
		
		*info.ptrans	=  T_matrix * build_gelement_tmat(Element);
		
		color = glrgb_blend(glrgb(Element->get_color()) ,Color);
		
		vertices = Element->get_vertices();
		for(UINT i = 0; i < vertices_n; i++){
			pattrs	[i] = (GLVATTR_DEF){
				glvector2d(vertices[i]), color};
			pindexes[i] = i;
		}
		
		primitive_list.push_back(info);
	}
}

static void 
build_element
(
	GELEMENT_POLYLINE* 	Element, 
	const GLRGB& 		Color, 
	const GLMATRIX33& 	T_matrix
){
	PRIMITIVE_INFO 	info;
	GLuint			vertices_n;
	const VECTOR2D* vertices;
	void* 			amem;
	GLVATTR_DEF*	pattrs;
	GLuint*			pindexes;
	GLRGB			color;
	
	vertices_n		= Element->get_vertices_n();
	
	memset(&info, 0, sizeof(PRIMITIVE_INFO));
	info.type 			= GL_LINE_STRIP;
	info.shader_prog	= shad_progs[INDEX_DEF_SHAD_PROG].id;
	info.vao			= gl_vao;
	info.attrs_size		= vertices_n * sizeof(GLVATTR_DEF	);
	info.indexes_size	= vertices_n * sizeof(GLuint		);
	
	amem = primitive_buffer.alloc(get_primitive_asize(info));
	if(amem){
		set_primitive_ptrs(amem, info);
		pattrs 			= (GLVATTR_DEF*) 	info.attrs;
		pindexes		= (GLuint*)			info.indexes;
		
		*info.ptrans	=  T_matrix * build_gelement_tmat(Element);
		
		color = glrgb_blend(glrgb(Element->get_color()) ,Color);
		
		vertices = Element->get_vertices();
		for(UINT i = 0; i < vertices_n; i++){
			pattrs	[i] = (GLVATTR_DEF){
				glvector2d(vertices[i]), color};
			pindexes[i] = i;
		}
		
		primitive_list.push_back(info);
	}
}

static void build_element(GELEMENT_SHAPE* Element, const GLRGB& Color, const GLMATRIX33& T_matrix){
	PRIMITIVE_INFO 	info;
	GLuint			vertices_n;
	const VECTOR2D* vertices;
	void* 			amem;
	GLVATTR_DEF*	pattrs;
	GLuint*			pindexes;
	GLRGB			color;
	
	vertices_n		= Element->get_vertices_n();
	
	memset(&info, 0, sizeof(PRIMITIVE_INFO));
	info.type 			= GL_LINE_LOOP;
	info.shader_prog	= shad_progs[INDEX_DEF_SHAD_PROG].id;
	info.vao			= gl_vao;
	info.attrs_size		= vertices_n * sizeof(GLVATTR_DEF	);
	info.indexes_size	= vertices_n * sizeof(GLuint		);
	
	amem = primitive_buffer.alloc(get_primitive_asize(info));
	if(amem){
		set_primitive_ptrs(amem, info);
		pattrs 			= (GLVATTR_DEF*) 	info.attrs;
		pindexes		= (GLuint*)			info.indexes;
		
		*info.ptrans	=  T_matrix * build_gelement_tmat(Element);
		
		color = glrgb_blend(glrgb(Element->get_color()) ,Color);
		
		vertices = Element->get_vertices();
		for(UINT i = 0; i < vertices_n; i++){
			pattrs	[i] = (GLVATTR_DEF){
				glvector2d(vertices[i]), color};
			pindexes[i] = i;
		}
		
		primitive_list.push_back(info);
	}
}

static void 
build_element
(
	GELEMENT_SPRITE* 	Element, 
	const GLRGB& 		Color, 
	const GLMATRIX33& 	T_matrix
){
	PRIMITIVE_INFO 	info;
	GLuint			vertices_n;
	const VECTOR2D* vertices;
	void* 			amem;
	GLVATTR_COT*	pattrs;
	GLuint*			pindexes;
	GLRGB			color;
	
	const VECTOR2D* texture_coords;
	GLuint indexes[] = {
		0,1,3,
		1,2,3
	};
	
	vertices_n		= Element->get_vertices_n();
	
	if(vertices_n != 4)
		return;
	
	memset(&info, 0, sizeof(PRIMITIVE_INFO));
	info.type 			= GL_TRIANGLES;
	info.shader_prog	= shad_progs[INDEX_COT_SHAD_PROG].id;
	info.vao			= gl_vao_color_tex;
	info.texture		= get_texture(Element->get_texture_name());
	info.attrs_size		= 4 * sizeof(GLVATTR_COT	);
	info.indexes_size	= 6 * sizeof(GLuint			);
	
	amem = primitive_buffer.alloc(get_primitive_asize(info));
	if(amem){
		set_primitive_ptrs(amem, info);
		pattrs 			= (GLVATTR_COT*) 	info.attrs;
		pindexes		= (GLuint*)			info.indexes;
		
		*info.ptrans	=  T_matrix * build_gelement_tmat(Element);
		
		color = glrgb_blend(glrgb(Element->get_color()) ,Color);
		
		vertices 		= Element->get_vertices();
		texture_coords	= Element->get_tcoords();
		for(UINT i = 0; i < 4; i++)
			pattrs	[i] = (GLVATTR_COT){
				glvector2d(vertices[i]), 
				color,
				glvector2d(texture_coords[i])
			};
		
		for(UINT i = 0; i < 6; i++)
			pindexes[i] = indexes[i];
		
		primitive_list.push_back(info);
	}
	
}

static void 
build_element
(
	GELEMENT_LINEGROUP* 	Element,
	const GLRGB& 			Color,
	const GLMATRIX33&		T_matrix 
){
	PRIMITIVE_INFO 	info;
	GLuint			vertices_n;
	const VECTOR2D* vertices;
	void* 			amem;
	GLVATTR_DEF*	pattrs;
	GLuint*			pindexes;
	GLRGB			color;
	GLRGB			group_color;
	GLMATRIX33		line_tmat;
	
	if(Element->size() == 0)
		return;
	
	vertices_n	= Element->size() * 2;
	
	memset(&info, 0, sizeof(PRIMITIVE_INFO));
	info.type 			= GL_LINES;
	info.shader_prog	= shad_progs[INDEX_DEF_SHAD_PROG].id;
	info.vao			= gl_vao;
	info.attrs_size		= vertices_n * sizeof(GLVATTR_DEF	);
	info.indexes_size	= vertices_n * sizeof(GLuint		);
	
	amem = primitive_buffer.alloc(get_primitive_asize(info));
	if(amem){
		set_primitive_ptrs(amem, info);
		pattrs 			= (GLVATTR_DEF*) 	info.attrs;
		pindexes		= (GLuint*)			info.indexes;
		
		*info.ptrans	=  T_matrix * build_gelement_tmat(Element);
		
		group_color = glrgb_blend(glrgb(Element->get_color()) ,Color);
		
		UINT index = 0;
		for(GELEMENT_LINE* line: *Element){
			line_tmat 	= build_gelement_tmat(line);
			vertices 	= line->get_vertices();
			color 		= glrgb_blend(glrgb(line->get_color()) ,group_color);
			
			for(UINT i = 0; i<2; i++){
				pattrs	[index] = (GLVATTR_DEF)
					{
						line_tmat * glvector2d(vertices[i]), 
						color
					};
					
				pindexes[index] = index;
				index++;
			}
		}
		
		primitive_list.push_back(info);
	}
}

static void 
build_element
(
	GELEMENT_RECTGROUP* 	Element,
	const GLRGB& 			Color,
	const GLMATRIX33&		T_matrix 
){
	PRIMITIVE_INFO 	info;
	GLuint			vertices_n;
	const VECTOR2D* vertices;
	void* 			amem;
	GLVATTR_DEF*	pattrs;
	GLuint*			pindexes;
	GLRGB			color;
	GLRGB			group_color;
	GLMATRIX33		rect_tmat;
	
	UINT 			args_n;
	UINT 			indexes_n;
	UINT			rects_n;
	UINT			smpl_indexes_n;
	GLuint*			smpl_indexes;
	UINT 			iattr;
	UINT 			iindex;
	
	GLuint trg_indexes[] = {
		0,1,3,
		1,2,3
	};
	
	GLuint lin_indexes[] = {
		0,1,
		1,2,
		2,3,
		3,0
	};
	
	rects_n = Element->size();
	
	if(rects_n == 0)
		return;
	
	if(Element->is_solid()){
		smpl_indexes_n 	= 6;
		smpl_indexes	= trg_indexes;
		
	}else{
		smpl_indexes_n 	= 8;
		smpl_indexes	= lin_indexes;
		
	}
	
	args_n 		= rects_n * 4;
	indexes_n	= rects_n * smpl_indexes_n;
	
	info.type 			= (Element->is_solid())?GL_TRIANGLES:GL_LINES;
	info.shader_prog	= shad_progs[INDEX_DEF_SHAD_PROG].id;
	info.vao			= gl_vao;
	info.attrs_size		= args_n 	* sizeof(GLVATTR_DEF);
	info.indexes_size	= indexes_n * sizeof(GLuint);
	
	amem = primitive_buffer.alloc(get_primitive_asize(info));
	if(amem){
		set_primitive_ptrs(amem, info);
		pattrs 			= (GLVATTR_DEF*) 	info.attrs;
		pindexes		= (GLuint*)			info.indexes;
		
		*info.ptrans	=  T_matrix * build_gelement_tmat(Element);
		
		group_color 	= glrgb_blend(glrgb(Element->get_color()) ,Color);
		
		iattr 	= 0;
		iindex	= 0;
		for(GELEMENT_RECT* rect: *Element){
			rect_tmat 	= build_gelement_tmat(rect);
			vertices 	= rect->get_vertices();
			color 		= glrgb_blend(glrgb(rect->get_color()) ,group_color);
			
			for(UINT i = 0; i < smpl_indexes_n; i++){
				pindexes[iindex] = smpl_indexes[i] + iattr;
				iindex++;
			}
			
			for(UINT i = 0; i < 4; i++){
				pattrs	[iattr] = (GLVATTR_DEF)
				{
					rect_tmat * glvector2d(vertices[i]), 
					color
				};
				iattr++;
			}
			
		}
		
		primitive_list.push_back(info);
	}
}

static void 
build_guielement
(
	GUIELEMENT* 		Element, 
	const GLMATRIX33& 	T_matrix
){
	GLMATRIX33 				t_matrix;
	GLMATRIX33 				t_gmodel_matrix;
	const GRAPHIC_MODEL*	gmodel;
	
	if(!Element->is_visible())
		return;
	
	gmodel = Element->get_gmodel();
	if(gmodel == nullptr)
		return;
	
	t_matrix = T_matrix  *
		glmat_trans(
			glvector2d(Element->get_position()),
			glvector2d(Element->get_normal()),
			glvector2d(Element->get_scale())
		);
	
	
	t_gmodel_matrix = t_matrix * 
		glmat_scl(
			glvector2d(gmodel->get_scale())
		);
		
	for(GELEMENT* graphic_elemet: *gmodel){
		build_element(
			graphic_elemet,
			glrgb(gmodel->get_color()),
			t_gmodel_matrix
		);
	}
	
	for(GUIELEMENT* guielement: *Element)
		build_guielement(guielement, t_matrix);
		
}

__GLFASTCALL__ static GLVECTOR2D 	glvector2d	(const VECTOR2D& Coord){
	return (GLVECTOR2D){(GLfloat)Coord.x,(GLfloat)Coord.y};
}

__GLFASTCALL__ static GLRGB		glrgb		(const RGB_COLOR& Color){
	return (GLRGB) {(GLfloat)Color.r, (GLfloat)Color.g, (GLfloat)Color.b};
}
__GLFASTCALL__ static GLVATTR_DEF 	glvattr_def	(const VECTOR2D& Coord, const RGB_COLOR& Color){
	return (GLVATTR_DEF){glvector2d(Coord), glrgb(Color)}; 
}

__GLFASTCALL__ static GLVATTR_COT 	glvattr_cot	(const VECTOR2D& Coord, const RGB_COLOR& Color, const VECTOR2D& Uv){
	return (GLVATTR_COT){glvector2d(Coord), glrgb(Color), glvector2d(Coord) };
}

__GLFASTCALL__ static GLMATRIX33	glmat_rot	(const GLVECTOR2D& Rotor){
	return (GLMATRIX33){
		Rotor.x, -Rotor.y, 	0,
		Rotor.y,  Rotor.x, 	0,
		0,			0,		1
	};
}

__GLFASTCALL__ static GLMATRIX33	glmat_scl	(const GLVECTOR2D& Scale){
	return (GLMATRIX33){
		Scale.x, 	0, 		0,
		0,  	Scale.y, 	0,
		0,			0,		1
	};
}
__GLFASTCALL__ static GLMATRIX33	glmat_trp	(const GLVECTOR2D& Position){
	return (GLMATRIX33){
		1, 			0, 		Position.x,
		0,  		1, 		Position.y,
		0,			0,		1
	};
}
__GLFASTCALL__ static GLMATRIX33	operator*	(const GLMATRIX33& M1, const GLMATRIX33& M2){
	GLuint i,j,k;
	GLMATRIX33	rmat;
	for(i = 0; i<3; i++)
		for(j = 0; j<3; j++){
			rmat.cells[i][j] = 0.0;
			for(k = 0; k<3; k++)
				rmat.cells[i][j] += M1.cells[i][k] * M2.cells[k][j];
		}
	return rmat;
}


__GLFASTCALL__ static GLVECTOR2D	operator*	(const GLMATRIX33& M, const GLVECTOR2D& V){
	GLVECTOR2D retv;
	retv.x = M.cells[0][0] * V.x + M.cells[0][1] * V.y + M.cells[0][2];	
	retv.y = M.cells[1][0] * V.x + M.cells[1][1] * V.y + M.cells[1][2];	
	return retv;
}

__GLFASTCALL__ static GLMATRIX33	glmat_trans	(const GLVECTOR2D& Pos, 
	const GLVECTOR2D& Rot, const GLVECTOR2D& Scl){
	//return  glmat_trp(Pos) * glmat_rot(Rot) * glmat_scl(Scl);
	return (GLMATRIX33){
		Scl.x * Rot.x, 	   -Scl.y*Rot.y, 		Pos.x,
		Scl.x * Rot.y,		Scl.y*Rot.x, 		Pos.y,
					0,				  0,				 1
	};
}

__GLFASTCALL__ static void set_primitive_ptrs(void* Mem, PRIMITIVE_INFO& Info){
		Info.attrs 		= (GLubyte*) Mem;
		Info.indexes	= Info.attrs + Info.attrs_size;
		Info.ptrans		= (GLMATRIX33*)	(Info.indexes + Info.indexes_size);
}

__GLFASTCALL__ static size_t 	get_primitive_asize	(const PRIMITIVE_INFO& Info){
	return (Info.attrs_size + Info.indexes_size + sizeof(GLMATRIX33));
}

__GLFASTCALL__ static GLRGB glrgb_blend	(const GLRGB& Src_color, const GLRGB& Bld_color ){
	return (GLRGB){
		Src_color.r * Bld_color.r,
		Src_color.g * Bld_color.g,
		Src_color.b * Bld_color.b
	};
}