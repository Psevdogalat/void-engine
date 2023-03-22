#ifndef __MATRIX33_H__
#define __MATRIX33_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef double Matrix33[3][3];
	
Matrix33 mat33dRot(const Vector2d);
Matrix33 mat33dScl(const Vector2d);
Matrix33 mat33dTrp(const Vector2d);
Matrix33 mat33dTrn(const Vector2d, const Vector2d, const Vector2d);
Matrix33 mat33dBackTrn(const Vector2d, const Vector2d, const Vector2d);

#ifdef __cplusplus
}
#endif

#endif

