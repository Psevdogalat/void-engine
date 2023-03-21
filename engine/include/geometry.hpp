#ifndef _GEOMETRY_HPP_
#define _GEOMETRY_HPP_

	#include <Vector2d.h>
	#include <Matrix33.h>
	
	#include <cmath>
	#include <list>

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

	class OrientedObject2d{
		private:
		Vector2d position;
		Vector2d normal;
		
		public:
		OrientedObject2d();
		OrientedObject2d(const OrientedObject2d & );
		OrientedObject2d(const Vector2d &, const Vector2d &);
		~OrientedObject2d();
		void setPositioin(const Vector2d & );
		void setNormal(const Vector2d & );
		const Vector2d & getPosition() const;
		const Vector2d & getNormal() const;
		
		OrientedObject2d & operator= (const OrientedObject2d & );
	};

	class ScalableObject2d{
		private:
		Vector2d scale;

		public:
		ScalableObject2d();
		ScalableObject2d(const ScalableObject2d &);
		ScalableObject2d(const Vector2d &);
		~ScalableObject2d();

		ScalableObject2d & operator= (const ScalableObject2d &);	
	};
	
	class Transformation2d: public OrientedObject2d, public ScalableObject2d{
		public:
		Transformation();
		Transformation(const Vector2d &, const Vector2d &, 
			const Vector2d &);
		Transformation(const Transformation & );
		~Transformation();
		
		void setTransformation(const Transformation2d &);
		void setTransformation(const Vector2d &, const Vector2d &, 
			const Vector2d &);

		Matrix33 forwardMatrix() const;
		Matrix33 backwardMatrix() const;

		Transformation2d & operator= 	(const Transformation2d &);
	};
	
	class VectorArray2
	{
		private:
		Vector2d * buffer;
		size_t size;

		public:		
		VectorArray2(size_t );
		VectorArray2(const VectorArray2 & );
		~VectorArray2();

		size_t getSize() const;
		void setVertices(const Vector2d *, size_t );
		void SetVertices(const VectorArray2 & );
		Vector2d * getVertices();
		
		VectorArray2 & operator= (const VectorArray2 &);

	};

	class Line
	{
		private:
		double length;

		public:
		Line();
		Line(const Line & );
		Line(double );
		~Line();

		void setLine(const Line &);
		void setLength(double );
		double getLength() const;	
	};

	class Rectangle
	{
		private:
		double windth, height;

		public:
		Rectangle();
		Rectangle(const Rectangle & );
		Rectangle(double, double );
		~Rectangle();

		void setRectangle(const Rectangle &);
		void setWidth(double );
		void setHeight(double );
		double getWidth() const;
		double getHeight() const;
	};

	class Circle
	{
		private:
		double radius;	

		public:
		Circle();
		Circle(const Circle &);
		Circle(double );
		~Circle();

		void setCircle(const Circle &);
		void setRadius();
		double getRadius() const;
	};

	class VectorFormater2
	{
		public:
		VectorFormater2();
		~VectorFormater2();		

		virtual void format(VectorArray2 &) const  = 0;
		virtual void format(Vector2d *, size_t ) const = 0;
		virtual size_t getFormatSize() const  = 0;
	};

	class LineFormater2: public Line, public Transformation2d,
		public VectorFormater2
	{
		public:
		LineFormater2();
		LineFormater2(const LineFormater2 &);	
		~LineFormater2();

		void format(VectorArray2 &);
		void format(Vector2d *, size_t);		
	};

	class RectangleFormater2: public Rectangle, public Transformation2d,
		public VectorFormater2
	{
		public:
		RectangleFormater2();
		RectangleFormater2(const RectangleFormater2 &);
		~RectangleFormater2();

		void format(VectorArray2 &);
		void format(Vector2d *, size_t);	
	};

	class CircleFormater2: public Circle, public Transformation2d,
		public VectorFormater2
	{
		public:
		CircleFormater2();
		CircleFormater2(const CircleFormater2 &);
		~CircleFormater2();

		void format(VectorArray2 &);
		void format(Vector2d *, size_t);		
	};

	class ArrayFormater2: public VectorArray2, public Transformation2d,
		public VectorFormater2
	{
		ArrayFormater2();
		ArrayFormater2(const ArrayFormater2 &);
		~ArrayFormater2();

		void format(VectorArray2 &);
		void format(Vector2d *, size_t);	
	};

	
			
	
	class VertexArray{
		private:
		Vector2d* 	vertices;
		UINT		buffer_len;
		UINT		vertices_n;
			
		void allocBuffer(size_t );
			
		public:
		VertexArray();
		VertexArray(const Vector2d *, size_t );
		VertexArray(const Vector2d &, size_t );
		VertexArray(const VertexArray & );
		~VertexArray();
								
		size_t getSize() const;
		const Vector2d* getVertices() const;
		void setVertices(const Vector2d *, size_t );
		void setVertices(const Vector2d &, size_t );
		void setVertex(const Vector2d &, unsigned int);
				
		VertexArray& operator= 	(const VertexArray& );
	};
	
	class VertexLine : public VertexArray{
		private:
		double length;
		double offset;
			
		public:
		VertexLine();
		VertexLine(double, double);
		VertexLine(const VertexLine& );
		~VertexLine(){};

		VertexArray * clone() const;
		
		double getLength() const;
		double getOffset() const;
		void setSpecs(double, double);
			
		VertexLine & operator= (const VertexLine& );
			
	};
	
	class VertexRectangle: public VertexArray{
		protected:
		double width;
		double height;
			
		public:
		VertexRectangle();
		VertexRectangle(double, double );
		VertexRectangle(const VertexRectangle &);
		~VertexRectangle(){};

		VertexArray * clone() const;
			
		double getWidth() const;
		double getHeight() const;
		void setSpecs(double, double );
		
		VertexRectangle & operator= (const VertexRectangle & );

	};
	
	
	class VertexCircle: public VertexArray{	
		protected:
		double 	radius;
		UINT	factor;
		
		public:
		VertexCircle();
		VertexCircle(double, UINT );
		VertexCircle(const VertexCircle& );
		~VertexCircle(){};
			
		VertexArray * clone() const;
			
		double getRadius() const;
		unsigned int getFactor() const;
		void setSpecs(double, unsigned int );
			
		VertexCircle & operator= (const VertexCircle& );
	};
	
	
	/* legacy */ 
	/* просто для справки, потом необходимо удолить! 
	class VectorObject: public Transformation2d{
		public:
			enum Type
			{
				T_POINT,
				T_POLYLINE,
				T_LINE,
				T_SHAPE,
				T_RECTANGLE,
				T_CIRCLE,
				T_COMPOSITE
			};

		protected:
			Type type;
			
		public:
			
			VectorObject();
			VectorObject(const Vector2d &, const Vector2d & );
			VectorObject(const VectorObject & );
			virtual ~VectorObject();

			virtual VectorObject * 	new_copy() = 0;
			
			Type	getType	() const;
	};
	
	class VECTOR_POINT: public VectorObject{
		public:
			VECTOR_POINT();
			VECTOR_POINT(const Vector2d&, const Vector2d & );
			VECTOR_POINT(const VECTOR_POINT& );
			~VECTOR_POINT();
			
			VectorObject* 	new_copy();
			VECTOR_POINT& operator= (const VECTOR_POINT& );
	};
	
	
	class VECTOR_POLYLINE: public VectorObject{
		protected:	
			UINT	  vertices_quantity;
			Vector2d* vertices;
			
		public:
			static const UINT MIN_VERTICES_QUANTITY = 2;
			VECTOR_POLYLINE();
			VECTOR_POLYLINE(const Vector2d&, const Vector2d &, UINT );
			VECTOR_POLYLINE(const VECTOR_POLYLINE& );
			~VECTOR_POLYLINE();
			VectorObject* new_copy();
			
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
			VectorObject* new_copy();
			
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
			VectorObject* new_copy();
			
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
			VectorObject* new_copy();
			
			VECTOR_RECTANGLE& operator= (const VECTOR_RECTANGLE& );
			
			double get_width	();
			double get_height	();
			
	};
	
	
	class VECTOR_CIRCLE: public VectorObject{	
		protected:
			double radius;
		
		public :
			VECTOR_CIRCLE();
			VECTOR_CIRCLE(const Vector2d&, const Vector2d &, double);
			VECTOR_CIRCLE(const VECTOR_CIRCLE& );
			~VECTOR_CIRCLE();
			VectorObject* new_copy();
			
			double get_radius();	
	};
	
	class VECTOR_COMPOSITE: public VectorObject{
		protected:
			UINT components_quantity;
			std::list<VectorObject*> components;
		
		public:
			VECTOR_COMPOSITE();
			VECTOR_COMPOSITE(const Vector2d&, const Vector2d & );
			VECTOR_COMPOSITE(const VECTOR_COMPOSITE& );
			~VECTOR_COMPOSITE();
			VectorObject* new_copy();
			VECTOR_COMPOSITE& operator= (const VECTOR_COMPOSITE& );
			
			UINT get_components_quantity();
			void push_component(VectorObject* );
			const std::list<VectorObject*>& get_components();
			
	};
	*/

#endif
