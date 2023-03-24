#ifndef __GEOMETRY2_H__
#define __GEOMETRY2_H__


#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	double x;
	double y; 
}Vector2d;

typedef struct
{
	double m[3][3];
}Matrix33d;


Vector2d vector2d(double, double);
Vector2d normal2d();

Vector2d add2d(const Vector2d, const Vector2d);
Vector2d sub2d(const Vector2d, const Vector2d);
Vector2d neg2d(const Vector2d);	
Vector2d nor2d(const Vector2d);
double   len2d(const Vector2d);
Vector2d rot2d(Vector2d, const Vector2d);
Vector2d mir2d(Vector2d, const Vector2d);
Vector2d scl2d(Vector2d, const Vector2d);
Vector2d trn2d(Vector2d, const Vector2d, const Vector2d, const Vector2d);
Vector2d leftOrtoNormal2d(const Vector2d);
double scalarProd2d(const Vector2d, const Vector2d);
double vectorProd2d(const Vector2d, const Vector2d);

Vector2d scalarRot2d(const Vector2d, double);
Vector2d scalarScl2d(const Vector2d, double);
Vector2d scalarTrn2d(const Vector2d, const Vector2d, const Vector2d, double);

Matrix33d matix33dRot(const Vector2d);
Matrix33d matix33dScl(const Vector2d);
Matrix33d matix33dTrp(const Vector2d);
Matrix33d matix33dTrn(const Vector2d, const Vector2d, const Vector2d);
Matrix33d matix33dTrnBack(const Vector2d, const Vector2d, const Vector2d);

Vector2d mulM33V2d(const Matrix33d, const Vector2d);
Matrix33d mulM33d(const Matrix33d, const Matrix33d);

#ifdef __cplusplus
}
#endif

#endif
