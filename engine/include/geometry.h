#ifndef _GEOMETRY_HPP_
#define _GEOMETRY_HPP_

	#include <std_defines.h>
	#include <cmath>
	#include <list>

	#include <Vector2d.h>
	#include <Matrix33.h>
	
	#define _GEOMETRY_ATTR_ inline
	
	Vector2d 	operator+ (const Vector2d &, const Vector2d &);
	Vector2d 	operator- (const Vector2d &, const Vector2d &);
	Vector2d 	operator- (const Vector2d & );
	Vector2d & 	operator+=(		 Vector2d &, const Vector2d &);
	Vector2d & 	operator-=(	 	 Vector2d &, const Vector2d &);
	Vector2d	operator* (const Vector2d &, double );
	Vector2d	operator* (double , const Vector2d &);
	Vector2d	operator* (const Vector2d & , const Vector2d &);
	Vector2d	operator/ (const Vector2d &, double );
	Vector2d	operator/ (double ,const Vector2d & );
	Vector2d &	operator*=( 	 Vector2d &, double );
	Vector2d &	operator/=(		 Vector2d &, double );
	bool 		operator==(const Vector2d &, const Vector2d &);
	bool 		operator!=(const Vector2d &, const Vector2d &);
	
	Matrix33	operator* (const Matrix33 &, const Matrix33 & );
	Vector2d	operator* (const Matrix33 &, const Vector2d & );
	
	Vector2d vector2d(double, double);
	Vector2d normal2d();
	double	 scalarProduct(const Vector2d &, const Vector2d &);
	double 	 vectorProduct(const Vector2d &, const Vector2d &);
	double 	 length(const Vector2d & );
	Vector2d normalize(const Vector2d & );
	Vector2d rotate(const Vector2d &, double);
	Vector2d rotate(const Vector2d &, const Vector2d & );
	Vector2d scale(const Vector2d &, double);
	Vector2d scale(const Vector2d &, const Vector2d & );
	Vector2d mirror(const Vector2d &, const Vector2d & );
	Vector2d transform(const Vector2d &, const Vector2d &, 
		const Vector2d&, double );
	Vector2d transform(const Vector2d &, const Vector2d &, 
		const Vector2d&, const Vector2d & );
	Vector2d leftOrtoNormal(const Vector2d & );
	
	Matrix33 mat33Rot	(const Vector2d & );
	Matrix33 mat33Scl	(const Vector2d & );
	Matrix33 mat33Trp	(const Vector2d & );
	Matrix33 mat33Trans(const Vector2d &, const Vector2d &, const Vector2d &);
	Matrix33 mat33BackTrans(const Vector2d &, const Vector2d &, 
		const Vector2d&);
	
	class Transformation{
		protected:
			Vector2d 	position;
			Vector2d	normal;
			Vector2d	scale;
			
		public:
			Transformation();
			Transformation(const Vector2d &, const Vector2d &, 
				const Vector2d &);
			Transformation(const Transformation & );
			~Transformation();
			void setPositioin(const Vector2d & );
			void setNormal(const Vector2d & );
			void setScale(const Vector2d & );
			Vector2d getPosition() const;
			Vector2d getNormal() const;
			Vector2d getScale() const;
			
			Matrix33 forwardMatrix() const;
			Matrix33 backwardMatrix() const;
			
			Transformation & operator= 	(const Transformation & );
	};
	
	class VERTEX_ARRAY{
		protected:
			Vector2d* 	vertices;
			UINT		buffer_len;
			UINT		vertices_n;
			
			void alloc_vertices_buffer(UINT );
			
		public:
									VERTEX_ARRAY	();
									VERTEX_ARRAY	(const Vector2d*, UINT );
									VERTEX_ARRAY	(const Vector2d&, UINT );
									VERTEX_ARRAY	(const VERTEX_ARRAY& );
			virtual 				~VERTEX_ARRAY	();
			virtual VERTEX_ARRAY* 	clone			() const;
								
					UINT			get_vertices_n	() const;
					const Vector2d* get_vertices	() const;
					void			set_vertices	(const Vector2d*, UINT );
					void			set_vertices	(const Vector2d&, UINT );
					void 			set_vertex		(const Vector2d&, UINT );
					
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
			Vector2d	position;
			Vector2d	normal;
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
			VECTOR_OBJECT(const Vector2d&, const Vector2d & );
			VECTOR_OBJECT(const VECTOR_OBJECT& );
			virtual ~VECTOR_OBJECT();
			virtual VECTOR_OBJECT* 	new_copy() = 0;
			
			UINT		get_type	() const;
			
			void 		set_position(const Vector2d& );
			Vector2d	get_position() const;
			void 		set_normal	(const Vector2d& );
			Vector2d	get_normal	() const;
			void 		set_scale	(double );
			double		get_scale	() const;
			void 		rotate		(double );
			void 		rotate		(const Vector2d& );
			void 		move		(const Vector2d& );
	};
	
	class VECTOR_POINT: public VECTOR_OBJECT{
		public:
			VECTOR_POINT();
			VECTOR_POINT(const Vector2d&, const Vector2d & );
			VECTOR_POINT(const VECTOR_POINT& );
			~VECTOR_POINT();
			
			VECTOR_OBJECT* 	new_copy();
			VECTOR_POINT& operator= (const VECTOR_POINT& );
	};
	
	
	class VECTOR_POLYLINE: public VECTOR_OBJECT{
		protected:	
			UINT	  vertices_quantity;
			Vector2d* vertices;
			
		public:
			static const UINT MIN_VERTICES_QUANTITY = 2;
			VECTOR_POLYLINE();
			VECTOR_POLYLINE(const Vector2d&, const Vector2d &, UINT );
			VECTOR_POLYLINE(const VECTOR_POLYLINE& );
			~VECTOR_POLYLINE();
			VECTOR_OBJECT* new_copy();
			
			VECTOR_POLYLINE& operator= (const VECTOR_POLYLINE& );
			
			UINT			get_vertices_quantity	() const;
			const Vector2d* get_vertices			() const;	
			RETCODE			get_vertex				(UINT, Vector2d& ) const;
			RETCODE			set_vertex				(UINT, const Vector2d& );
	};
	
	class VECTOR_LINE: public VECTOR_POLYLINE{
		protected:
			double length;
			double center_offset;
			
		public:
			VECTOR_LINE();
			VECTOR_LINE(const Vector2d& ,const Vector2d &, double, double);
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
			VECTOR_SHAPE(const Vector2d&, const Vector2d &, UINT );
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
			VECTOR_RECTANGLE(const Vector2d&, const Vector2d &, double, double );
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
			VECTOR_CIRCLE(const Vector2d&, const Vector2d &, double);
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
			VECTOR_COMPOSITE(const Vector2d&, const Vector2d & );
			VECTOR_COMPOSITE(const VECTOR_COMPOSITE& );
			~VECTOR_COMPOSITE();
			VECTOR_OBJECT* new_copy();
			VECTOR_COMPOSITE& operator= (const VECTOR_COMPOSITE& );
			
			UINT get_components_quantity();
			void push_component(VECTOR_OBJECT* );
			const std::list<VECTOR_OBJECT*>& get_components();
			
	};
	
#endif
