#include <gui.h>

using namespace ENGINE;

/* GUIELEMENT */	
	GUIELEMENT::GUIELEMENT(){
		visible = true;
		enable  = true;
		gmodel	= nullptr;
		smodel = nullptr;
	}
	
	GUIELEMENT::GUIELEMENT(const GUIELEMENT& Smpl):
	TRNSF_OBJECT(Smpl){
		visible = Smpl.visible;
		enable 	= Smpl.enable;
		
		if(Smpl.gmodel != nullptr){
			gmodel = new GRAPHIC_MODEL(*Smpl.gmodel);
		}else
			gmodel = nullptr;
	
		if(Smpl.smodel != nullptr){
			smodel = new COLLISION_MODEL(*Smpl.smodel);
		}else
			smodel = nullptr;
		
	}
	
	GUIELEMENT::~GUIELEMENT(){
		for(GUIELEMENT* sub_element: *this)
			delete sub_element;
	}
	
	GUIELEMENT* GUIELEMENT::clone() const{
		return new GUIELEMENT(*this);
	}
	
	const GRAPHIC_MODEL* 	GUIELEMENT::get_gmodel() const{
		return gmodel;
	}
	
	const COLLISION_MODEL* 	GUIELEMENT::get_smodel() const{
		return smodel;
	}
	
	void GUIELEMENT::set_visible(bool Visible){visible = Visible;	}
	void GUIELEMENT::set_enable	(bool Enable ){enable = Enable;		}
	bool GUIELEMENT::is_visible	() const {return visible;	}
	bool GUIELEMENT::is_enable	() const {return enable;	}

/* GUI_SPRITE */	
	GUI_SPRITE::GUI_SPRITE(){
		width 	= 0;
		height 	= 0;
		gmodel = new GRAPHIC_MODEL();
		sprite = new GELEMENT_SPRITE();
		gmodel->push_back(sprite);
	}
	
	GUI_SPRITE::GUI_SPRITE(const GUI_SPRITE& Smpl):
		GUIELEMENT(Smpl){
		width 	= Smpl.width;
		height	= Smpl.height;
		
		sprite = nullptr;
		for(GELEMENT* gelement: *gmodel)
			if(gelement->get_type() == GELEMENT_TSPRITE){
				sprite = (GELEMENT_SPRITE*)gelement;
				break;
			}
			
		throw 1;
	}
	
	GUI_SPRITE::~GUI_SPRITE(){
		if(gmodel != nullptr)
			delete gmodel;
		
		if(smodel != nullptr)
			delete smodel;
	}
	
	void GUI_SPRITE::set_width	(double Width){
		width 	= (Width < 0.0)? 0.0: Width;
		sprite->set_specs(width, height);
	}
	
	void GUI_SPRITE::set_height	(double	Height){
		height	= (Height < 0.0)? 0.0: Height;
		sprite->set_specs(width, height);
	}
	
	void GUI_SPRITE::set_texture (const std::string& Name, UINT Width, UINT Height){
		sprite->set_texture(Name, Width, Height);
	}
	
	void GUI_SPRITE::set_window	(UINT X, UINT Y, UINT Width, UINT Height){
		sprite->set_window(X, Y, Width, Height);
	}
	
	GUIELEMENT*		GUI_SPRITE::clone		() const{
		return new GUI_SPRITE(*this);
	}
	
	double GUI_SPRITE::get_width	() const {return width; }
	double GUI_SPRITE::get_height	() const {return height;}
	