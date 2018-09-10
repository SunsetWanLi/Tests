#include "MGGL.h"

#include <math.h>
#ifndef M_PI
#define M_PI        3.14159265358979323846264338327950288   /* pi */
#endif

#ifdef __cplusplus
extern "C"
{
#endif
	//void gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
	//{
	//	GLfloat xmin, xmax, ymin, ymax;

	//	ymax = zNear * (GLfloat)tan(fovy * (float)M_PI / 360);
	//	ymin = -ymax;
	//	xmin = ymin * aspect;
	//	xmax = ymax * aspect;

	//	glFrustumf(xmin, xmax,
	//		ymin, ymax,
	//		zNear, zFar);
	//}

//	void gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
//		GLfloat centerx, GLfloat centery, GLfloat centerz,
//		GLfloat upx, GLfloat upy, GLfloat upz)
//	{
//		GLfloat m[16];
//
//#define M(row,col)  m[col*4+row]
//		M(0, 0) = 1.0f;//x[0];
//		M(0, 1) = 0.0f;//x[1];
//		M(0, 2) = 0.0f;//x[2];
//		M(0, 3) = 0.0f;
//		M(1, 0) = 0.0f;//y[0];
//		M(1, 1) = -1.0f;//y[1];
//		M(1, 2) = 0.0f;//y[2];
//		M(1, 3) = 0.0f;
//		M(2, 0) = 0.0f;//z[0];
//		M(2, 1) = 0.0f;//z[1];
//		M(2, 2) = 1.0f;//z[2];
//		M(2, 3) = 0.0f;
//		M(3, 0) = 0.0f;
//		M(3, 1) = 0.0f;
//		M(3, 2) = 0.0f;
//		M(3, 3) = 1.0f;
//#undef M
//
//		glMultMatrixf(m);
//
//
//		/* Translate Eye to Origin */
//		glTranslatef(-eyex, -eyey, -eyez);
//	}


#ifdef __cplusplus
}
#endif
