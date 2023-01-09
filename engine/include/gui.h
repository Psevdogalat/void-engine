#ifndef __GUI_H__
#define __GUI_H__

	#include <collisions.h>
	#include <graphics.h>
	#include <list>

	namespace ENGINE{
		
		class GUIELEMENT : public TRNSF_OBJECT, public std::list<GUIELEMENT*>{
			protected:
				bool 				visible;
				bool				enable;
				GRAPHIC_MODEL* 		gmodel;
				COLLISION_MODEL*	smodel;
				
			
			public:
						GUIELEMENT	();
						GUIELEMENT 	(const GUIELEMENT& );
				virtual ~GUIELEMENT	();
				virtual GUIELEMENT*				
					clone () const;
					
				const GRAPHIC_MODEL* 	
					get_gmodel	() const;
					
				const COLLISION_MODEL* 	
					get_smodel	() const;
				
				void 	set_visible	(bool );
				void 	set_enable	(bool );
				bool	is_visible	() const;
				bool	is_enable	() const;
			
		};
		
		class GUI_SPRITE: public GUIELEMENT{
			protected:
				GELEMENT_SPRITE* 	sprite;
				double 				width;
				double 				height;
			
			public:
								GUI_SPRITE	();
								GUI_SPRITE	(const GUI_SPRITE& );
								~GUI_SPRITE	();
				
				void 			set_width	(double );
				void 			set_height	(double	);
				void 			set_texture (const std::string&, UINT, UINT );
				void 			set_window	(UINT, UINT, UINT, UINT);
				
				GUIELEMENT*		clone		() const;
				double			get_width	() const;
				double 			get_height	() const;
				
		};

	}

#endif