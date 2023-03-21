#ifndef __GUI_HPP__
#define __GUI_HPP__

	#include <collisions.hpp>
	#include <graphics.hpp>
	
	#include <list>

	namespace VoidEngine{
		
		class GuiElement : public Transformation2d, 
			public std::list<GuiElement*>
		{
			private:
				bool visible;
				bool enable;
				GRAPHIC_MODEL* 		gmodel;
				COLLISION_MODEL*	smodel;
				
			
			public:
				GuiElement();
				GuiElement(const GuiElement& );
				~GuiElement();

				virtual GuiElement * cloneGuiElement() const;
				virtual void freeGuiElement();
					
				const GRAPHIC_MODEL * get_gmodel	() const;
				const COLLISION_MODEL * get_smodel	() const;
				
				void setVisible(bool );
				void setEnable(bool );
				bool isVisible() const;
				bool isEnable() const;
		};
		
		class GUI_SPRITE: public GuiElement{
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
				
				GuiElement*		clone		() const;
				double			get_width	() const;
				double 			get_height	() const;
				
		};

	}

#endif
