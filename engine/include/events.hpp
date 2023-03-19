#ifndef _EVENTS_H_
#define _EVENTS_H_
	
	#include <vector2d.h>
	
	namespace VoidEngine{

		#define EV_KEY_PRESS 		0x0005
		#define EV_MOUSE_WHEEL		0x0006
		#define EV_MOUSE_MOVE		0x0007
		#define EV_MOUSE_LKEY_DOWN	0x0008
		#define EV_MOUSE_LKEY_UP	0x0009
		#define EV_MOUSE_RKEY_DOWN	0x000A
		#define EV_MOUSE_RKEY_UP	0x000B
		#define EV_MOUSE_MKEY_DOWN	0x000C
		#define EV_MOUSE_MKEY_UP	0x000D
	
		typedef struct{
			bool 			down;
			unsigned int 	code;
			unsigned int 	controlState;
		}KeyEvent;
		
		
		typedef struct{
			Vector2d 	coord;
			int			delta;
		}MouseEvent;
		
		typedef struct{
			UINT 	id;
			union
		 	{
				KeyEvent 	key;
				MouseEvent	mouse;
			};
		}Event;
	
	}
	
#endif
