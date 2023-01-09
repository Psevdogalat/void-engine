#ifndef __VISUALIZERS_H__
#define __VISUALIZERS_H__

#include <list>
#include <vector>

#include <engine.h>
#include <pathfind.h>

namespace ENGINE
{

class VIS_PATHFINDER{
	protected:
		GAME_OBJECT*			game_object;
		GRAPHIC_MODEL*			graphic_model;
        GELEMENT_RECT*          contour;
		GELEMENT_RECTGROUP*		lamps_group;
		GELEMENT_RECT***		lamps_array;
        UINT                    array_width;
        UINT                    array_height;
        bool                    init;		
	
	public:
		TRNSF_OBJECT		transform;
		PATHFINDER::GRID	grid;

		RGB_COLOR 			cl_barred;	
        RGB_COLOR           cl_unbarred;
        RGB_COLOR           cl_undefine;
		VIS_PATHFINDER();
		VIS_PATHFINDER(const VIS_PATHFINDER& Smpl);
		~VIS_PATHFINDER();
		
		void 
		visualize
		();

};


class VIS_PATH{
	protected:
		GAME_OBJECT* 			game_object;
		GRAPHIC_MODEL*			graphic_model;
		bool 					init;
		
		RGB_COLOR
		intp_color
		(
			double Alfa
		)const;
		
	public:
		RGB_COLOR 				colors[2];
		std::vector<VECTOR2D>	path;
	
		VIS_PATH();
		VIS_PATH(const VIS_PATH& Smpl);
		~VIS_PATH();
				
		void
		visualize
		();
};

}

#endif
