#ifndef _GRAPHICS_HPP_
#define _GRAPHICS_HPP_

	#include <geometry.hpp>
	#include <list>
	#include <string>
	
	namespace VoidEngine{
		
		class RgbColor{
		public:
			float r;
			float g;
			float b;
			RgbColor();
			RgbColor(float, float, float);
			RgbColor(const RgbColor & );
			RgbColor & operator= (const RgbColor & );
		};
		
		class RGBObject{
			private:
			RgbColor color;
				
			public:
			RGBObject();
			RGBObject(const RGBObject &);
			~RGBObject();
			void setColor(const RgbColor &);
			const RgbColor & getColor() const;
			
		};
		
		
		/*
		class MULTYRGBObject{
			protected:
				UINT		colors_n;
				RgbColor* 	colors
			
			public:
				MULTYCOLOR();
				~MULTYCOLOR();
				UINT 				get_color_quantity	();
				const RgbColor*	get_colors			();
				void 				set_colors			(const RgbColor* );
				const RgbColor*	get_color			(UINT );
				void 				set_color			(const RgbColor&, UINT);
			
		};
		*/
		
		#define GLAYERS_NUM				8
		#define GLAYER_DEFAULT			4
		
		class GraphicObject : public Transformation2d, public RGBObject{
			public:
			enum Type
			{
				T_VOID,
				T_GROUP,
				T_POINT,
				T_LINE,
				T_POLYLINE,
				T_SHAPE,
				T_RECT,
				T_CIRCLE,
				T_SPRITE,
				T_LINEGROUP,
				T_RECTGROUP
			};

			protected:
			Type type;
			
			public:
			bool visible;

			GraphicObject();
			virtual	~GraphicObject(){};

			virtual GraphicObject * cloneGraphicObject() const = 0;
				
			Type getType() const;
			GraphicObject &	operator= (const GraphicObject &);
				 
		};
		
		class GraphicGroup : public GraphicObject
        {
			public:
            typedef std::list<GraphicObject *>::iterator iterator; 
			typedef std::list<GraphicObject *>::const_iterator const_iterator;

            private:
			std::list<GraphicObject *> objects;

			public:
			GraphicGroup();
			GraphicGroup(const GraphicGroup &);
			~GraphicGroup();

			GraphicObject * cloneGraphicObject() const;

			size_t size() const;
			const_iterator begin() const;
			const_iterator end() const;
			void push_back(GraphicObject *);
			void clear();

		};
		
		class GraphicPolyline : public GraphicObject, 
			public Vector2dBuffer
		{
			public:
			GraphicPolyline();
			GraphicPolyline(const GraphicPolyline & );
			~GraphicPolyline();

			GraphicObject * cloneGraphicObject() const;	
		};
		
		class GraphicShape : public GraphicObject, 
			public Vector2dBuffer
		{
			public:
			GraphicShape();
			GraphicShape(const GraphicShape & );
			~GraphicShape();

			GraphicObject *	cloneGraphicObject() const;	
		};
		
		class GraphicLine : public GraphicObject
			public Vector2dBuffer
		{
			public:
			GraphicLine();
			GraphicLine(const GraphicLine & );
			~GraphicLine();
			
			GraphicObject *	cloneGraphicObject() const;

			void setSpecs(double, double );
			void setSpecs(const Vector2d &,  const Vector2d & );
		};
		
		
		class GraphicRectangle : public GraphicObject, 
			public Vector2dBuffer
		{
			private:
				bool solid;
			
			public:
			GraphicRectangle();
			GraphicRectangle(const GraphicRectangle & );
			~GraphicRectangle();
			
			GraphicObject *	cloneGraphicObject() const;		
			
			void setSpecs(double, double );
			void setSolid(bool );
			bool isSolid() const;
		};
		
		class GraphicCircle : public GraphicObject,
			public Vector2dBuffer
		{
			public:
			GraphicCircle();
			GraphicCircle(const GraphicCircle & );
			~GraphicCircle();

			GraphicObject *	cloneGraphicObject() const;		
		
			void setSpecs(double, unsigned int );
		};
		

		class GraphicSprite : public GraphicObject,
			public Vector2dBuffer
		{
			private:
				std::string textureStrId;
				Transformation2d textureWindow;

			public:
				GraphicSprite();
				GraphicSprite(const GraphicSprite & );
				~GraphicSprite();

				GraphicObject * cloneGraphicObject() const;	
				
				void setSpecs(double, double);
				void setTexture(const std::string &, double, double);
				void setTextureWindow();
				
				UINT				get_tcoords_n		() const;
				const Vector2d*		get_tcoords			() const;
				UINT 				get_vertices_n		() const;
				const Vector2d* 	get_vertices		() const;
				const std::string&  get_texture_name	() const;
			
		};
		
		class GraphicLineGROUP : public GraphicObject{
			protected:
				typedef std::list<GraphicLine*> GEL_LINE_PLIST;
				GEL_LINE_PLIST lines;
			
			public:
				GraphicLineGROUP	();
				GraphicLineGROUP	(const GraphicLineGROUP& );
				~GraphicLineGROUP	();
				
				GraphicObject* 		clone		() const;	
				
				typedef GEL_LINE_PLIST::iterator iterator; 
				typedef GEL_LINE_PLIST::const_iterator const_iterator;
				
				size_t 			size		() const;
				const_iterator 	begin		() const;
				const_iterator 	end			() const;
				void 			push_back	(GraphicLine* );
                void            clear       ();
		};
		
		class GraphicRectangleGROUP : public GraphicObject{
			protected:
				typedef std::list<GraphicRectangle*> GEL_RECT_PLIST;
				GEL_RECT_PLIST 	rects;
				bool 			solid;
			
			public:
				GraphicRectangleGROUP	();
				GraphicRectangleGROUP	(const GraphicRectangleGROUP& );
				~GraphicRectangleGROUP	();
				
				GraphicObject* 		clone		() const;	
				
				typedef GEL_RECT_PLIST::iterator 		iterator; 
				typedef GEL_RECT_PLIST::const_iterator 	const_iterator;
				
				size_t 			size		() const;
				const_iterator 	begin		() const;
				const_iterator 	end			() const;
				void 			push_back	(GraphicRectangle* );
				
				void 			set_solid	(bool );
				bool			is_solid	() const;
                void            clear       ();
		};
		
		class GRAPHIC_MODEL: public GraphicGroup{
			protected:
				UINT 	layer;
			
			public:
							GRAPHIC_MODEL	();
							GRAPHIC_MODEL	(const GRAPHIC_MODEL& );
							~GRAPHIC_MODEL	();
					
					UINT 	get_layer		();
					void 	set_layer		(UINT );
		};
		
		
		bool init_graphic				();
		void free_graphic				();
		void render_graphic				();
		
		void set_draw_graphic_models	(bool );
		void set_draw_collision_models	(bool );
		void set_draw_physical_models	(bool );
	}
	
#endif
