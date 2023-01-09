#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

	#include <std_defines.h>
	#include <geometry.h>
	#include <list>
	#include <string>
	
	namespace ENGINE{
		
		class RGB_COLOR{
		public:
			float r;
			float g;
			float b;
			RGB_COLOR();
			RGB_COLOR(float, float, float);
			RGB_COLOR(const RGB_COLOR& );
			RGB_COLOR& operator= (const RGB_COLOR& );
		};
		
		class COLORED_OBJECT{
			protected:
				RGB_COLOR color;
				
			public:
									COLORED_OBJECT	();
									COLORED_OBJECT	(const COLORED_OBJECT& );
									~COLORED_OBJECT	();
				void 				set_color		(const RGB_COLOR& );
				const RGB_COLOR&	get_color		() const;
			
		};
		
		
		/*
		class MULTYCOLORED_OBJECT{
			protected:
				UINT		colors_n;
				RGB_COLOR* 	colors
			
			public:
				MULTYCOLOR();
				~MULTYCOLOR();
				UINT 				get_color_quantity	();
				const RGB_COLOR*	get_colors			();
				void 				set_colors			(const RGB_COLOR* );
				const RGB_COLOR*	get_color			(UINT );
				void 				set_color			(const RGB_COLOR&, UINT);
			
		};
		*/
		
		#define GELEMENT_TVOID 			0
		#define GELEMENT_TGROUP			1
		#define GELEMENT_TPOINT			2
		#define GELEMENT_TLINE			3
		#define GELEMENT_TPOLYLINE		4
		#define GELEMENT_TSHAPE			5
		#define GELEMENT_TRECT			6
		#define GELEMENT_TCIRCLE		7
		#define GELEMENT_TSPRITE		8
		#define GELEMENT_TLINEGROUP		9
		#define GELEMENT_TRECTGROUP		10
		
		#define GLAYERS_NUM				8
		#define GLAYER_DEFAULT			4
		
		class GELEMENT : public TRNSF_OBJECT, public COLORED_OBJECT{
			protected:
				UINT 		type;
			
			public:
				bool		visible;

									GELEMENT		();
									GELEMENT		(const GELEMENT& );
				virtual 			~GELEMENT		()	{};
				virtual GELEMENT* 	clone			() const = 0;
				
				UINT 				get_type		() const;
				GELEMENT&			operator=		(const GELEMENT& );
				 
		};
		
		class GELEMENT_GROUP : public GELEMENT
        {
            protected:
                std::list<GELEMENT*> list;

			public:
							GELEMENT_GROUP	();
							GELEMENT_GROUP	(const GELEMENT_GROUP& );
							~GELEMENT_GROUP	();
				GELEMENT* 	clone			() const;

               	typedef std::list<GELEMENT*>::iterator 		    iterator; 
				typedef std::list<GELEMENT*>::const_iterator 	const_iterator;
				
				size_t 			size		() const;
				const_iterator 	begin		() const;
				const_iterator 	end			() const;
				void 			push_back	(GELEMENT* );
                void            clear       ();

		};
		
		class GELEMENT_POLYLINE : public GELEMENT{
			protected:
				VERTEX_ARRAY vertices;
			
			public:
								GELEMENT_POLYLINE	();
								GELEMENT_POLYLINE 	(const GELEMENT_POLYLINE& );
								~GELEMENT_POLYLINE	();
				GELEMENT* 		clone				() const;	
		
				UINT 			get_vertices_n		() const;
				const VECTOR2D* get_vertices		() const;
				void 			set_vertices		(const VECTOR2D*, UINT );
				void 			set_vertex			(const VECTOR2D&, UINT );
		};
		
		class GELEMENT_SHAPE : public GELEMENT{
			protected: 
				VERTEX_ARRAY vertices;
			
			public:
								GELEMENT_SHAPE	();
								GELEMENT_SHAPE	(const GELEMENT_SHAPE& );
								~GELEMENT_SHAPE	();
				GELEMENT* 		clone			() const;	
		
				UINT 			get_vertices_n	() const;
				const VECTOR2D* get_vertices	() const;
				void 			set_vertices	(const VECTOR2D*, UINT );
				void 			set_vertex		(const VECTOR2D&, UINT );
		};
		
		class GELEMENT_LINE : public GELEMENT{
			protected:
				VERTEX_LINE line;
				
			public:
								GELEMENT_LINE	();
								GELEMENT_LINE	(const GELEMENT_LINE& );
								~GELEMENT_LINE	();
				GELEMENT* 		clone			() const;
				
				void 			set_specs		(double, double);
				
				void 
				set_specs		
				(
					const VECTOR2D&,  
					const VECTOR2D&
				);
				
				UINT 			get_vertices_n	() const;
				const VECTOR2D* get_vertices	() const;
		};
		
		
		class GELEMENT_RECT : public GELEMENT{
			protected:
				VERTEX_RECTANGLE 	rectangle;
				bool 				solid;
			
			public:
								GELEMENT_RECT	();
								GELEMENT_RECT	(const GELEMENT_RECT& );
								~GELEMENT_RECT	();
				GELEMENT* 		clone			() const;		
			
				void 			set_specs		(double, double);
				UINT 			get_vertices_n	() const;
				const VECTOR2D* get_vertices	() const;
				
				void 			set_solid		(bool );
				bool			is_solid		() const;
		};
		
		class GELEMENT_CIRCLE : public GELEMENT{
			protected:
				VERTEX_CIRCLE circle;
			
			public:
								GELEMENT_CIRCLE	();
								GELEMENT_CIRCLE	(const GELEMENT_CIRCLE& );
								~GELEMENT_CIRCLE();
				GELEMENT* 		clone			() const;		
		
				void 			set_specs		(double, UINT);
				UINT 			get_vertices_n	() const;
				const VECTOR2D* get_vertices	() const;
		};
		
		class GELEMENT_SPRITE : public GELEMENT{
			protected:
				VERTEX_RECTANGLE 	rectangle;
				std::string 		texture_name;
				UINT				texture_width;
				UINT				texture_height;
				VECTOR2D			texture_coords[4];
				
			public:
								GELEMENT_SPRITE		();
								GELEMENT_SPRITE		(const GELEMENT_SPRITE& );
								~GELEMENT_SPRITE	();
				GELEMENT* 		clone				() const;	
				
				void set_specs	(double, double);
				void set_texture(const std::string&, UINT, UINT );
				void set_window	(UINT, UINT, UINT, UINT);
				
				UINT				get_tcoords_n		() const;
				const VECTOR2D*		get_tcoords			() const;
				UINT 				get_vertices_n		() const;
				const VECTOR2D* 	get_vertices		() const;
				const std::string&  get_texture_name	() const;
			
		};
		
		class GELEMENT_LINEGROUP : public GELEMENT{
			protected:
				typedef std::list<GELEMENT_LINE*> GEL_LINE_PLIST;
				GEL_LINE_PLIST lines;
			
			public:
				GELEMENT_LINEGROUP	();
				GELEMENT_LINEGROUP	(const GELEMENT_LINEGROUP& );
				~GELEMENT_LINEGROUP	();
				
				GELEMENT* 		clone		() const;	
				
				typedef GEL_LINE_PLIST::iterator iterator; 
				typedef GEL_LINE_PLIST::const_iterator const_iterator;
				
				size_t 			size		() const;
				const_iterator 	begin		() const;
				const_iterator 	end			() const;
				void 			push_back	(GELEMENT_LINE* );
                void            clear       ();
		};
		
		class GELEMENT_RECTGROUP : public GELEMENT{
			protected:
				typedef std::list<GELEMENT_RECT*> GEL_RECT_PLIST;
				GEL_RECT_PLIST 	rects;
				bool 			solid;
			
			public:
				GELEMENT_RECTGROUP	();
				GELEMENT_RECTGROUP	(const GELEMENT_RECTGROUP& );
				~GELEMENT_RECTGROUP	();
				
				GELEMENT* 		clone		() const;	
				
				typedef GEL_RECT_PLIST::iterator 		iterator; 
				typedef GEL_RECT_PLIST::const_iterator 	const_iterator;
				
				size_t 			size		() const;
				const_iterator 	begin		() const;
				const_iterator 	end			() const;
				void 			push_back	(GELEMENT_RECT* );
				
				void 			set_solid	(bool );
				bool			is_solid	() const;
                void            clear       ();
		};
		
		class GRAPHIC_MODEL: public GELEMENT_GROUP{
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
