#include <geometry.h>
#include <math.h>
#include <string.h>

Vector2d vector2d(double x, double y)
{
	Vector2d v;
	v.x = x;
	v.y = y;
	return v;
};

Vector2d normal2d()
{
	Vector2d v;
	v.x = 1.0;
	v.y = 0.0;
	return v;
};

Vector2d add2d(const Vector2d v1, const Vector2d v2)
{
	Vector2d v;
	v.x = v1.x + v2.x;
	v.y = v1.y + v2.y;
	return v;
};

Vector2d sub2d(const Vector2d v1, const Vector2d v2)
{
	Vector2d v;
	v.x = v1.x - v2.x;
	v.y = v1.y - v2.y;
	return v;
};

Vector2d neg2d(const Vector2d v1)
{
	Vector2d v;
	v.x = -v1.x;
	v.y = -v1.y;
	return v;
};

double len2d(const Vector2d v1)
{
	return sqrt(scalarProd2d(v1, v1));
};

double scalarProd2d(const Vector2d v1, const Vector2d v2)
{
	return v1.x * v2.x + v1.y * v2.y;
};

double vectorProd2d(const Vector2d v1, const Vector2d v2)
{
	return v1.x * v2.y - v2.x * v1.y;
};

Vector2d nor2d(const Vector2d v1)
{
	double l;
	
	l = len2d(v1);

	if(l == 0)
		return normal2d();
	
	return scalarScl2d(v1,  1/l);
};

Vector2d scalarrv22d(const Vector2d v1, double a2)
{
	Vector2d v;
	v.x = v1.x*cos(a2) - v1.y*sin(a2);
	v.y = v1.x*sin(a2) + v1.y*cos(a2);
	return v;
};

Vector2d rot2d(const Vector2d v1, const Vector2d r2)
{
	Vector2d v;
	v.x = v1.x * r2.x  - v1.y * r2.y; 
	v.y = v1.x * r2.y  + v1.y * r2.x;
	return v;
};

Vector2d scalarsv32d(const Vector2d v1, double s1){
	Vector2d v;
	v.x = v1.x * s1;
	v.y = v1.y * s1;
	return v;
}

Vector2d scl2d(const Vector2d v1, const Vector2d v2){
	Vector2d v;
	v.x = v1.x * v2.x;
	v.y = v1.y * v2.y;
	return v;
}

Vector2d mir2d(const Vector2d v1, const Vector2d v2)
{
	Vector2d v3;
	v3 = nor2d(v2);
	return sub2d(v1, scalarsv32d(v3, scalarProd2d(v1,v3) * 2)); 
};

Vector2d scalarTrn2d(const Vector2d v1, const Vector2d pv2, 
	const Vector2d rv3, double s4)
{
	return add2d(scalarsv32d(rot2d(v1, rv3), s4), pv2);
};


Vector2d trn2d(const Vector2d v1, const Vector2d pv2, const Vector2d rv3, 
	const Vector2d sv4)
{
	return add2d(scl2d(rot2d(v1, rv3), sv4), pv2);
};

Vector2d leftOrtoNormal(const Vector2d v1)
{
	return nor2d(vector2d(-v1.y, v1.x));
};

Vector2d mulM33V2d(const Matrix33d m1, const Vector2d v2)
{
	Vector2d v;
	v.x = m1.m[0][0] * v2.x + m1.m[0][1] * v2.y + m1.m[0][2];	
	v.y = m1.m[1][0] * v2.x + m1.m[1][1] * v2.y + m1.m[1][2];	
	return v;
};

Matrix33d mulM33d(const Matrix33d m1, const Matrix33d m2)
{
	unsigned int i,j,k;
	Matrix33d m;
	memset(&m, 0, sizeof(m));
	
	for(i = 0; i<3; i++)
		for(j = 0; j<3; j++)
			for(k = 0; k<3; k++)
				m.m[i][j] += m1.m[i][k] * m2.m[k][j];

	return m;
};

Matrix33d matrix33drv2(const Vector2d rv1)
{
	return (Matrix33d){
		rv1.x, -rv1.y, 	0,
		rv1.y,  rv1.x, 	0,
		0,		0,		1
	};
};

Matrix33d matrix33dsv3(const Vector2d  sv1)
{
	return (Matrix33d){
		sv1.x, 	0, 		0,
		0,  	sv1.y, 	0,
		0,		0,		1
	};
};

Matrix33d matrix33dTrp(const Vector2d pv1)
{
	return (Matrix33d){
		1, 			0, 		pv1.x,
		0,  		1, 		pv1.y,
		0,			0,		1
	};
};

Matrix33d matrix33dTrn(const Vector2d pv1, const Vector2d rv2, 
	const Vector2d sv3)
{
	return (Matrix33d){
		sv3.x * rv2.x, 	   -sv3.y*rv2.y, 		pv1.x,
		sv3.x * rv2.y,		sv3.y*rv2.x, 		pv1.y,
		0,					0,					1		
	};
};

Matrix33d matrix33dTrnBack(const Vector2d pv1, const Vector2d rv2, 
	const Vector2d sv3)
{
	double scl1, scl2;
	scl1 = (1.0 / sv3.x);
	scl2 = (1.0 / sv3.y);

	return (Matrix33d){
		 scl1 * rv2.x,	scl1 * rv2.y, -scl1 * (pv1.x * rv2.x + pv1.y * rv2.y),
		-scl2 * rv2.y,	scl2 * rv2.x,  scl2 * (pv1.x * rv2.y - pv1.y * rv2.x),
		0,			   	0,				1
	};

};


