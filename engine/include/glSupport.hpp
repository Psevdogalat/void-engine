#ifndef __GLSUPPORT_HPP_
#define __GLSUPPORT_HPP_

#include <gl/glew.h>

typedef struct {
	GLfloat x;
	GLfloat y;
}GLVector2d;

typedef GLfloat GLMatrix33[3][3];

typedef struct{
	GLfloat r;
	GLfloat g;
	GLfloat b;
}GLRGBColor;

typedef struct{
	GLVector2d 	coord;
	GLRGBColor	color;
}GLVATTR_DEF;

typedef struct{
	GLVector2d 	coord;
	GLRGBColor	color;
	GLVector2d	uv;
}GLVATTR_COT; 

typedef struct{
	GLVector2d pos;
	GLVector2d nor;
	GLVector2d scl;
}GLTRANS;

#define __GLFASTCALL__ inline

__GLFASTCALL__ GLVector2d glVvector2d(const Vector2d &  )
__GLFASTCALL__ GLRGB glRGBColor(const RGB_COLOR & );
__GLFASTCALL__ GLRGB glrgb_blend(const GLRGB &, const GLRGB & );
__GLFASTCALL__ GLVATTR_DEF glvattr_def(const Vector2d &, 
	const RGB_COLOR& );
__GLFASTCALL__ GLVATTR_COT 	glvattr_cot(const Vector2d& , const RGB_COLOR& , 
	const VECTOR2D& );
__GLFASTCALL__ GLMATRIX33 glmat_rot(const GLVector2d& );
__GLFASTCALL__ GLMATRIX33 glmat_scl(const GLVector2d& );
__GLFASTCALL__ GLMATRIX33 glmat_trp(const GLVector2d& );
__GLFASTCALL__ GLMATRIX33 operator*(const GLMATRIX33&, const GLMATRIX33& );
__GLFASTCALL__ GLVector2d operator*(const GLMATRIX33&, const GLVector2d& );
__GLFASTCALL__ GLMATRIX33 glmat_trans(const GLVector2d&, 
	const GLVector2d&, const GLVector2d& );

#endif
