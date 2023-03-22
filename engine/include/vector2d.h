#ifndef __VECTOR2D_H__
#define __VECTOR2D_H__


#ifdef __cplusplus
extern "C" {
#endif

typedef double Vector2d[2];

Vector2d vector2d(double, double);
Vector2d normal2d();

Vector2d add2d(const Vector2d);
Vector2d sub2d(const Vector2d);
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
Vector2d scalarScale2d(const Vector2d, double);
Vector2d scalarTrn2d(const Vector2d, const Vector2d, const Vector2d, double );

#ifdef __cplusplus
}
#endif

#endif
