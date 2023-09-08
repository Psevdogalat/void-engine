#ifndef _GEOMETRY_HPP_
#define _GEOMETRY_HPP_

#include <geometry2.h>

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

Matrix33d	operator* (const Matrix33d &, const Matrix33d & );
Vector2d	operator* (const Matrix33d &, const Vector2d & );

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
	Transformation2d();
	Transformation2d(const Vector2d &, const Vector2d &, 
		const Vector2d &);
	Transformation2d(const Transformation2d & );
	~Transformation2d();
	
	void setTransformation(const Transformation2d &);
	void setTransformation(const Vector2d &, const Vector2d &, 
		const Vector2d &);

	Matrix33d forwardMatrix() const;
	Matrix33d backwardMatrix() const;

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

#endif
