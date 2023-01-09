#include <visualizers.h>


using namespace ENGINE;

/* VIS_PATH */
/* ========================================================================== */

VIS_PATH::VIS_PATH
()
{
	init 			= false;
	game_object 	= nullptr;
	graphic_model 	= nullptr;
	colors[0] 		= RGB_COLOR(1.0, 1.0, 1.0);
	colors[1] 		= RGB_COLOR(1.0, 1.0, 1.0);
}

VIS_PATH::VIS_PATH
(
	const VIS_PATH& Smpl
):VIS_PATH()
{
	for(UINT i = 0; i < 2; i++)
		colors[i] = Smpl.colors[i];
	
	path = Smpl.path;
}

VIS_PATH::~VIS_PATH
()
{
	if(game_object != nullptr)
		despawn(game_object);
	
	if(graphic_model != nullptr)
		delete graphic_model;
}

void
VIS_PATH::visualize
()
{
	GELEMENT_CIRCLE* 	circle;
	GELEMENT_LINEGROUP*	line_group;
	GELEMENT_LINE*		line;
	double 				alfa, delta;
	VECTOR2D			last_point;
	
	if(!init){
		game_object = spawn(new GAME_OBJECT(), vector2d(0.0, 0.0), 
			create_normal());
			
		graphic_model = new GRAPHIC_MODEL();
		game_object->set_graphic_model(graphic_model);
		init = true;
	}
	
	graphic_model->clear();
	
	if(path.size() > 0){
		delta = 1.0 / (double) path.size();
		alfa = 0;
		for(const VECTOR2D& point: path){
			circle = new GELEMENT_CIRCLE();
			circle->set_specs(0.3, 16);
			circle->set_color(intp_color(alfa));
			circle->set_position(point);
			graphic_model->push_back(circle);
			alfa += delta;
		}
		
		alfa = 0;
		if(path.size() > 1){
			line_group = new GELEMENT_LINEGROUP();
			last_point = path[0];
			for(UINT i = 1; i < path.size(); i++){
				line = new GELEMENT_LINE();
				line->set_specs(last_point, path[i]);
				line->set_color(intp_color(alfa));
				line_group->push_back(line);
				alfa += delta;
				last_point = path[i];
			
			}
			graphic_model->push_back(line_group);
		}
	}
	
}

RGB_COLOR
VIS_PATH::intp_color
(
	double Alfa
)const
{
	RGB_COLOR rcolor;
	double delt[3];
	
	rcolor.r = colors[0].r + (colors[1].r - colors[0].r) * Alfa;
	rcolor.g = colors[0].g + (colors[1].g - colors[0].g) * Alfa;
	rcolor.b = colors[0].b + (colors[1].b - colors[0].b) * Alfa;
	return rcolor;
}

/* VIS_PATHFINDER */
/* ========================================================================== */
VIS_PATHFINDER::VIS_PATHFINDER
()
{
    init            = false;
    game_object     = nullptr;
    graphic_model   = nullptr;
    lamps_group     = nullptr;
    lamps_array     = nullptr;
    array_width     = 0;
    array_height    = 0;
    
    cl_barred       = RGB_COLOR(1.0f, 0.0f, 0.0f);
    cl_unbarred     = RGB_COLOR(0.0f, 1.0f, 0.0f);
    cl_undefine     = RGB_COLOR(0.8f, 0.8f, 0.8f);
}


VIS_PATHFINDER::VIS_PATHFINDER
(
    const VIS_PATHFINDER& Smpl
):VIS_PATHFINDER()
{
    transform   = Smpl.transform;
    grid        = Smpl.grid;
    cl_barred   = Smpl.cl_barred;
    cl_unbarred = Smpl.cl_unbarred;
    cl_undefine = Smpl.cl_undefine;

}

VIS_PATHFINDER::~VIS_PATHFINDER
()
{
	if(game_object != nullptr)
		despawn(game_object);
	
	if(graphic_model != nullptr)
		delete graphic_model;

    if(lamps_array != nullptr){
        for(UINT i = 0; i < array_width; i++)
            delete [] lamps_array[i];

        delete [] lamps_array;

    }

}

void
VIS_PATHFINDER::visualize
()
{	
    GELEMENT_RECT*    tmp_lamp;
    PATHFINDER::NODE* tmp_node;
    RGB_COLOR*        color;
    VECTOR2D          position;
    VECTOR2D          offset;

    if(!init){
		game_object = spawn(new GAME_OBJECT(), vector2d(0.0, 0.0), 
			create_normal());
			
		graphic_model = new GRAPHIC_MODEL();
        game_object->set_graphic_model(graphic_model);
        
        contour = new GELEMENT_RECT();
        contour->set_specs(1.0, 1.0);
        contour->set_color(cl_undefine);


        lamps_group = new GELEMENT_RECTGROUP();
        lamps_group->set_solid(false);
        
        graphic_model->push_back(contour);
        graphic_model->push_back(lamps_group);

        init = true;
	}

    if(
       grid.get_width()  != array_width ||
       grid.get_height() != array_height
    ){

        lamps_group->clear();

        if(lamps_array != nullptr){
            for(UINT i = 0; i < array_width; i++)
                delete [] lamps_array[i];

            delete [] lamps_array;

        }
        
        array_width  = grid.get_width();
        array_height = grid.get_height();

        lamps_array = new GELEMENT_RECT** [array_width];
        for(UINT i = 0; i < array_width; i++){
            lamps_array[i] = new GELEMENT_RECT* [array_height];
            
            for(UINT j = 0; j < array_height; j++){
                tmp_lamp = new GELEMENT_RECT();        
                tmp_lamp->set_specs(0.7,0.7);
                tmp_lamp->set_solid(true);
                lamps_group->push_back(tmp_lamp);
                lamps_array[i][j] = tmp_lamp;
            }
        }
    }

    offset = 0.5 * vector2d(
		      1.0 - (double)grid.get_width(), 
              1.0 - (double)grid.get_height()
	        );

    for(UINT i = 0 ; i < array_width; i++)
        for(UINT j = 0; j < array_height; j++)
        {
            tmp_node = grid.get_node(PATHFINDER::GRID::INDEX(i, j));
            if(tmp_node != nullptr)
            {
                position = tmp_node->position;
                switch(tmp_node->state){
                    case PATHFINDER::NODE::ST_UNDEFINE:
                    color = &cl_undefine; break;

                    case PATHFINDER::NODE::ST_BARRED:
                    color = &cl_barred; break;

                    case PATHFINDER::NODE::ST_UNBARRED:
                    color = &cl_unbarred; break;

                    default:
                        color = &cl_undefine;
                }
            }
            else
            {
                color       = &cl_undefine;
                position    = vector2d((double)i, (double)j) + offset;
            }

            lamps_array[i][j]->set_position(position);
            lamps_array[i][j]->set_color(*color);

        }

    if(array_height == 0 || array_width == 0)
    {
        lamps_group->visible = false;
    }
    else
    {
        lamps_group->visible = true;
        lamps_group->set_scale(
                vector2d(
                    1.0/(double)array_width, 
                    1.0/(double)array_height
                    )
                );
    }

    graphic_model->set_scale(transform.get_scale());

    game_object->set_position(transform.get_position());
    game_object->set_normal(transform.get_normal());

}


