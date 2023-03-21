#ifndef __GUI_HPP__
#define __GUI_HPP__

	#include <collisions.hpp>
	#include <graphics.hpp>
	
	#include <list>

	namespace VoidEngine{
		
		class GuiObject : public Transformation2d, 
			public std::list<GuiObject*>
		{
			private:
				bool visible;
				bool enable;
				GraphicModel * gmodel;
				CollisionModel * smodel;
				
			
			public:
				GuiObject();
				GuiObject(const GuiObject &);
				~GuiObject();

				virtual GuiObject * cloneGuiObject() const = 0;
				virtual void freeGuiObject() = 0;
					
				const GraphicModel * getGmodel	() const;
				const CollisionModel * getSmodel	() const;
				
				void setVisible(bool );
				void setEnable(bool );
				bool isVisible() const;
				bool isEnable() const;
		};
		
	}

#endif
