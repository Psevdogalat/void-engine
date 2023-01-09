#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

	#include <std_defines.h>
	#include <cmath>
	#include <list>
	
	#define _GEOMETRY_ATTR_ inline
	
	typedef struct{
		double	x;
		double  y;
	}VECTOR2D;
	
	typedef struct{
		double values[3][3];
	}MATRIX33;
	
	VECTOR2D 	operator+ (const VECTOR2D&, const VECTOR2D& );
	VECTOR2D 	operator- (const VECTOR2D&, const VECTOR2D& );
	VECTOR2D 	operator- (const VECTOR2D& );
	VECTOR2D& 	operator+=(		 VECTOR2D&, const VECTOR2D& );
	VECTOR2D& 	operator-=(	 	 VECTOR2D&, const VECTOR2D& );
	VECTOR2D	operator* (const VECTOR2D&, double );
	VECTOR2D	operator* (double , const VECTOR2D&);
	VECTOR2D	operator* (const VECTOR2D& , const VECTOR2D&);
	VECTOR2D	operator/ (const VECTOR2D&, double );
	VECTOR2D	operator/ (double ,const VECTOR2D& );
	VECTOR2D&	operator*=( 	 VECTOR2D&, double );
	VECTOR2D&	operator/=(		 VECTOR2D&, double );
	bool 		operator==(const VECTOR2D&, const VECTOR2D& );
	bool 		operator!=(const VECTOR2D&, const VECTOR2D& );
	
	MATRIX33	operator* (const MATRIX33&, const MATRIX33& );
	VECTOR2D	operator* (const MATRIX33&, const VECTOR2D& );
	
	VECTOR2D vector2d			(double, double);
	VECTOR2D create_normal		();
	double	 scalar_product2d	(const VECTOR2D&, const VECTOR2D&);
	double 	 vector_product2d	(const VECTOR2D&, const VECTOR2D&);
	double 	 vector_length		(const VECTOR2D& );
	VECTOR2D normalize_vector	(const VECTOR2D& );
	VECTOR2D rotate_vector		(const VECTOR2D&, double);
	VECTOR2D rotate_vector		(const VECTOR2D&, const VECTOR2D& );
	VECTOR2D scale_vector		(const VECTOR2D&, double);
	VECTOR2D scale_vector		(const VECTOR2D&, const VECTOR2D& );
	VECTOR2D mirror_vector		(const VECTOR2D&, const VECTOR2D& );
	VECTOR2D transform_vertex	(const VECTOR2D&, const VECTOR2D&, 
		const VECTOR2D&, double );
	VECTOR2D transform_vertex	(const VECTOR2D&, const VECTOR2D&, 
		const VECTOR2D&, const VECTOR2D& );
	VECTOR2D left_orto_normal	(const VECTOR2D& );
	
	MATRIX33 mat33_rot			(const VECTOR2D& );
	MATRIX33 mat33_scl			(const VECTOR2D& );
	MATRIX33 mat33_trp			(const VECTOR2D& );
	MATRIX33 mat33_trans		(const VECTOR2D&, const VECTOR2D&, 
		const VECTOR2D&);
	MATRIX33 mat33_trans_back	(const VECTOR2D&, const VECTOR2D&, 
		const VECTOR2D&);
	
	class TRNSF_OBJECT{
		protected:
			VECTOR2D 	position;
			VECTOR2D	normal;
			VECTOR2D	scale;
			
		public:
			TRNSF_OBJECT();
			TRNSF_OBJECT(const VECTOR2D&, const VECTOR2D&, const VECTOR2D&);
			TRNSF_OBJECT(const TRNSF_OBJECT& );
			~TRNSF_OBJECT();
			void 				set_position	(const VECTOR2D& );
			void 				set_normal		(const VECTOR2D& );
			void 				set_scale		(const VECTOR2D& );
			VECTOR2D			get_position	() const;
			VECTOR2D			get_normal		() const;
			VECTOR2D			get_scale		() const;
			
			MATRIX33			
			get_forward_matrix	
			() const;
			
			MATRIX33			
			get_backward_matrix	
			() const;
			
			TRNSF_OBJECT&
			operator=
			(
				const TRNSF_OBJECT& Rval
			);
	};
	
	class VERTEX_ARRAY{
		protected:
			VECTOR2D* 	vertices;
			UINT		buffer_len;
			UINT		vertices_n;
			
			void alloc_vertices_buffer(UINT );
			
		public:
									VERTEX_ARRAY	();
									VERTEX_ARRAY	(const VECTOR2D*, UINT );
									VERTEX_ARRAY	(const VECTOR2D&, UINT );
									VERTEX_ARRAY	(const VERTEX_ARRAY& );
			virtual 				~VERTEX_ARRAY	();
			virtual VERTEX_ARRAY* 	clone			() const;
								
					UINT			get_vertices_n	() const;
					const VECTOR2D* get_vertices	() const;
					void			set_vertices	(const VECTOR2D*, UINT );
					void			set_vertices	(const VECTOR2D&, UINT );
					void 			set_vertex		(const VECTOR2D&, UINT );
					
					VERTEX_ARRAY& 	operator= 		(const VERTEX_ARRAY& );
	};
	
	class VERTEX_LINE : public VERTEX_ARRAY{
		protected:
			double 		length;
			double 		offset;
			
		public:
							VERTEX_LINE			();
							VERTEX_LINE			(double, double);
							VERTEX_LINE			(const VERTEX_LINE& );
							~VERTEX_LINE		(){};
			VERTEX_ARRAY* 	clone				() const;
			
			double 			get_length			() const;
			double 			get_offset			() const;
			void 			set_specs			(double, double);
			
			VERTEX_LINE& 	operator= 			(const VERTEX_LINE& );
			
	};
	
	class VERTEX_RECTANGLE: public VERTEX_ARRAY{
		protected:
			double width;
			double height;
			
		public:
							VERTEX_RECTANGLE	();
							VERTEX_RECTANGLE	(double, double );
							VERTEX_RECTANGLE	(const VERTEX_RECTANGLE& );
							~VERTEX_RECTANGLE	(){};
			VERTEX_ARRAY* 	clone				() const;
			
			double 			get_width			() const;
			double			get_height			() const;
			void 			set_specs			(double, double );
			
			VERTEX_RECTANGLE& operator= (const VERTEX_RECTANGLE& );
			
	};
	
	
	class VERTEX_CIRCLE: public VERTEX_ARRAY{	
		protected:
			double 	radius;
			UINT	factor;
		
		public :
							VERTEX_CIRCLE	();
							VERTEX_CIRCLE	(double, UINT );
							VERTEX_CIRCLE	(const VERTEX_CIRCLE& );
							~VERTEX_CIRCLE	(){};
			VERTEX_ARRAY* 	clone			() const;
			
			double 			get_radius		() const;
			UINT			get_factor		() const;
			void 			set_specs		(double, UINT );
			
			VERTEX_CIRCLE& 	operator=		(const VERTEX_CIRCLE& );
	};
	
	
	/* legacy */ 
	class VECTOR_OBJECT{
		protected:
			UINT		type;
			VECTOR2D	position;
			VECTOR2D	normal;
			double 		scale;
			
		public:
			static const UINT T_POINT 		= 1;
			static const UINT T_POLYLINE 	= 2;
			static const UINT T_LINE 		= 3;
			static const UINT T_SHAPE 		= 4;
			static const UINT T_RECTANGLE 	= 5;
			static const UINT T_CIRCLE 		= 6;
			static const UINT T_COMPOSITE 	= 7;
			
			VECTOR_OBJECT();
			VECTOR_OBJECT(const VECTOR2D&, const VECTOR2D& );
			VECTOR_OBJECT(const VECTOR_OBJECT& );
			virtual ~VECTOR_OBJECT();
			virtual VECTOR_OBJECT* 	new_copy() = 0;
			
			UINT		get_type	() const;
			
			void 		set_position(const VECTOR2D& );
			VECTOR2D	get_position() const;
			void 		set_normal	(const VECTOR2D& );
			VECTOR2D	get_normal	() const;
			void 		set_scale	(double );
			double		get_scale	() const;
			void 		rotate		(double );
			void 		rotate		(const VECTOR2D& );
			void 		move		(const VECTOR2D& );
	};
	
	class VECTOR_POINT: public VECTOR_OBJECT{
		public:
			VECTOR_POINT();
			VECTOR_POINT(const VECTOR2D&, const VECTOR2D& );
			VECTOR_POINT(const VECTOR_POINT& );
			~VECTOR_POINT();
			
			VECTOR_OBJECT* 	new_copy();
			VECTOR_POINT& operator= (const VECTOR_POINT& );
	};
	
	
	class VECTOR_POLYLINE: public VECTOR_OBJECT{
		protected:	
			UINT	  vertices_quantity;
			VECTOR2D* vertices;
			
		public:
			static const UINT MIN_VERTICES_QUANTITY = 2;
			VECTOR_POLYLINE();
			VECTOR_POLYLINE(const VECTOR2D&, const VECTOR2D&, UINT );
			VECTOR_POLYLINE(const VECTOR_POLYLINE& );
			~VECTOR_POLYLINE();
			VECTOR_OBJECT* new_copy();
			
			VECTOR_POLYLINE& operator= (const VECTOR_POLYLINE& );
			
			UINT			get_vertices_quantity	() const;
			const VECTOR2D* get_vertices			() const;	
			RETCODE			get_vertex				(UINT, VECTOR2D& ) const;
			RETCODE			set_vertex				(UINT, const VECTOR2D& );
	};
	
	class VECTOR_LINE: public VECTOR_POLYLINE{
		protected:
			double length;
			double center_offset;
			
		public:
			VECTOR_LINE();
			VECTOR_LINE(const VECTOR2D& ,const VECTOR2D&, double, double);
			VECTOR_LINE(const VECTOR_LINE& );
			~VECTOR_LINE();
			VECTOR_OBJECT* new_copy();
			
			VECTOR_LINE& operator= (const VECTOR_LINE& );
			
			double get_length		();
			double get_center_offset();
	};
	
	class VECTOR_SHAPE: public VECTOR_POLYLINE{
		public:
			static const UINT MIN_VERTICES_QUANTITY = 3;
			VECTOR_SHAPE();
			VECTOR_SHAPE(const VECTOR2D&, const VECTOR2D&, UINT );
			VECTOR_SHAPE(const VECTOR_SHAPE& );
			~VECTOR_SHAPE();
			VECTOR_OBJECT* new_copy();
			
			VECTOR_SHAPE& operator= (const VECTOR_SHAPE& );
	};
	
	
	class VECTOR_RECTANGLE: public VECTOR_SHAPE{
		protected:
			double width;
			double height;
			
		public:
			VECTOR_RECTANGLE();
			VECTOR_RECTANGLE(const VECTOR2D&, const VECTOR2D&, double, double );
			VECTOR_RECTANGLE(const VECTOR_RECTANGLE& );
			~VECTOR_RECTANGLE();
			VECTOR_OBJECT* new_copy();
			
			VECTOR_RECTANGLE& operator= (const VECTOR_RECTANGLE& );
			
			double get_width	();
			double get_height	();
			
	};
	
	
	class VECTOR_CIRCLE: public VECTOR_OBJECT{	
		protected:
			double radius;
		
		public :
			VECTOR_CIRCLE();
			VECTOR_CIRCLE(const VECTOR2D&, const VECTOR2D&, double);
			VECTOR_CIRCLE(const VECTOR_CIRCLE& );
			~VECTOR_CIRCLE();
			VECTOR_OBJECT* new_copy();
			
			double get_radius();	
	};
	
	class VECTOR_COMPOSITE: public VECTOR_OBJECT{
		protected:
			UINT components_quantity;
			std::list<VECTOR_OBJECT*> components;
		
		public:
			VECTOR_COMPOSITE();
			VECTOR_COMPOSITE(const VECTOR2D&, const VECTOR2D& );
			VECTOR_COMPOSITE(const VECTOR_COMPOSITE& );
			~VECTOR_COMPOSITE();
			VECTOR_OBJECT* new_copy();
			VECTOR_COMPOSITE& operator= (const VECTOR_COMPOSITE& );
			
			UINT get_components_quantity();
			void push_component(VECTOR_OBJECT* );
			const std::list<VECTOR_OBJECT*>& get_components();
			
	};
	
#endif
