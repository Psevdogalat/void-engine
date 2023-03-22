#include <gui.hpp>

using namespace VoidEngine;

GuiObject::GuiObject()
{
	visible = true;
	enable  = true;
	gmodel	= nullptr;
	smodel = nullptr;
};

GuiObject::GuiObject(const GuiObject & Smpl): Transformation2d(Smpl)
{
	visible = Smpl.visible;
	enable 	= Smpl.enable;
	
	if(Smpl.gmodel != nullptr){
		gmodel = new GraphicModel(*Smpl.gmodel);
	}else
		gmodel = nullptr;

	if(Smpl.smodel != nullptr){
		smodel = new CollisionModel(*Smpl.smodel);
	}else
		smodel = nullptr;
	
};

GuiObject::~GuiObject(){};

const GraphicModel * GuiObject::getGmodel() const{return gmodel;};
const CollisionModel * GuiObject::getSmodel() const{return smodel;};

void GuiObject::setVisible(bool inVisible){visible = inVisible;};
void GuiObject::setEnable(bool inEnable){enable = inEnable;};
bool GuiObject::isVisible() const {return visible;};
bool GuiObject::isEnable() const {return enable;};
