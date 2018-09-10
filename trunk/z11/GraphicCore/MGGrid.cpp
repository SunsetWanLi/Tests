#include "MGGrid.h"
#include "MGImage.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "../System/Shared/MGLog.h"
#include "../System/Device/MGDevice.h"
#include "../System/IO/MGBundle.h"
#include "../GraphicCore/MGPNGCodec.h"
#include "../GraphicCore/MGPPMCodec.h"
#ifndef M_PI
#define M_PI        3.14159265358979323846264338327950288
#endif
#ifndef M_PI_2
#define M_PI_2      1.57079632679489661923132169163975144   /* pi/2           */
#endif
#ifndef DIRECT_OPENGL
#ifndef glOrtho
#define glOrtho glOrthof
#endif
#endif

#ifndef DEFAULT_GRID_STEP
#define DEFAULT_GRID_STEP 32
#endif

namespace z11
{
	MGGrid::~MGGrid(void)
	{
		delete_and_set_null(m_pImage);
		delete_array_and_set_null(m_pVertices);
		delete_array_and_set_null(m_pOriginalVertices);
		delete_array_and_set_null(m_pTexCoordinates);
		delete_array_and_set_null(m_pIndices);
	}

	void MGGrid::calculateVertexPoints( void )
	{
		if (m_pImage==NULL)
		{
			return ;
		}
		float width = (float)m_pImage->getAdapterWidth();
		float height = (float)m_pImage->getAdapterHeight();
		float imageH = m_pImage->getHeight();
        float imageW = m_pImage->getWidth();

		//int numQuads = m_sGridSize.width * m_sGridSize.height;

		//m_pVertices = malloc(numQuads * 12 * sizeof(GLfloat));
		//m_pOriginalVertices = malloc(numQuads * 12 * sizeof(GLfloat));
		//m_pTexCoordinates = malloc(numQuads * 8 * sizeof(GLfloat));
		//m_pIndices = (uint_16 *)malloc(numQuads * 6 * sizeof(GLushort));
		
		if (m_sGridSize.width==0||m_sGridSize.height==0)
		{
			assert("you should init m_sGridSize first");
		}
		int numQuads=(m_sGridSize.width+1)*(m_sGridSize.height+1);
		m_pVertices=new GLfloat[numQuads *3];
		m_pOriginalVertices=new GLfloat[numQuads *3];
		m_pTexCoordinates=new GLfloat[numQuads*2];
		m_pIndices=new uint_16[m_sGridSize.width*m_sGridSize.height*6];

		//memset(m_pVertices,0,sizeof(m_pVertices));
		//memset(m_pOriginalVertices,0,sizeof(m_pOriginalVertices));
		//memset(m_pTexCoordinates,0,sizeof(m_pTexCoordinates));
		//memset(m_pIndices,0,sizeof(m_pIndices));

		float *vertArray = (float*)m_pVertices;
		float *texArray = (float*)m_pTexCoordinates;
		GLushort *idxArray = m_pIndices;

		int x, y ,i;

		for (x = 0; x < m_sGridSize.width; ++x)
		{
			for (y = 0; y < m_sGridSize.height; ++y)
			{
				int idx = (y * m_sGridSize.width) + x;

				float x1 = x * m_obStep.width;
				float x2 = x1 + m_obStep.width;
				float y1 = y * m_obStep.height;
				float y2= y1 + m_obStep.height;
                
                if (x2 > imageW) {
                    x2 = imageW;
                }
                
                if (y2 > imageH) {
                    y2 = imageH;
                }

				GLushort a = (GLushort)(x * (m_sGridSize.height + 1) + y);
				GLushort b = (GLushort)((x + 1) * (m_sGridSize.height + 1) + y);
				GLushort c = (GLushort)((x + 1) * (m_sGridSize.height + 1) + (y + 1));
				GLushort d = (GLushort)(x * (m_sGridSize.height + 1) + (y + 1));

				GLushort tempidx[6] = {a, b, d, b, c, d};

				memcpy(&idxArray[6*idx], tempidx, 6*sizeof(GLushort));

				int l1[4] = {a*3, b*3, c*3, d*3};
				MGVertex e = {x1, y1, 0};
				MGVertex f = {x2, y1, 0};
				MGVertex g = {x2, y2, 0};
				MGVertex h = {x1, y2, 0};

				MGVertex l2[4] = {e, f, g, h};

				int tex1[4] = {a*2, b*2, c*2, d*2};
				MGPoint tex2[4] = {MGPointMake(x1, y1), MGPointMake(x2, y1), MGPointMake(x2, y2), MGPointMake(x1, y2)};

				for (i = 0; i < 4; ++i)
				{
					vertArray[l1[i]] = l2[i].x *m_scale.axis_x;
					vertArray[l1[i] + 1] = l2[i].y*m_scale.axis_y;
					vertArray[l1[i] + 2] = l2[i].z*m_scale.axis_z;

					texArray[tex1[i]] = tex2[i].x / width;
					if (m_bIsTextureFlipped)
					{
						texArray[tex1[i] + 1] = (imageH - tex2[i].y) / height;
					}
					else
					{
						texArray[tex1[i] + 1] = tex2[i].y / height;
					}
				}
			}
		}
        
//        for (int i = 0; i < numQuads; ++i) {
//            MGLogD("numQuads %d Vertices is : %f, %f, %f.", i, m_pVertices[3*i], m_pVertices[3*i + 1], m_pVertices[3*i + 2]);
//            MGLogD("numQuads %d TextCoord is : %f, %f.", i, m_pTexCoordinates[2*i], m_pTexCoordinates[2*i + 1]);
//        }
//        
//        MGLogD("Grid Size is : %d, %d.", this->m_sGridSize.width, this->m_sGridSize.height);
//        MGLogD("Grid Size is : %f, %f.", imageW, imageH);

		memcpy(m_pOriginalVertices, m_pVertices, (m_sGridSize.width+1) * (m_sGridSize.height+1) * sizeof(MGVertex));
	}

	MGGrid::MGGrid():m_pImage(NULL),m_bIsTextureFlipped(false),m_pVertices(NULL),m_pOriginalVertices(NULL),m_pTexCoordinates(NULL),m_pIndices(NULL)
	{
		m_scale.axis_x=1;
		m_scale.axis_y=1;
		m_scale.axis_z=1;
	}

	bool MGGrid::initWithSize( MGIntSize gridSize, MGImage *pTexture, bool bFlipped )
	{
		delete_and_set_null(m_pImage);
		m_sGridSize = gridSize;
		m_pImage = pTexture;
		m_bIsTextureFlipped = bFlipped;
		m_obStep.width	= DEFAULT_GRID_STEP;// m_pImage->getWidth() / m_sGridSize.width;
		m_obStep.height	= DEFAULT_GRID_STEP;//m_pImage->getHeight()/ m_sGridSize.height;

		calculateVertexPoints();
		return true;
	}

	z11::MGVertex MGGrid::originalVertex( const MGIntPoint &pos )
	{
		int index = (pos.x * (m_sGridSize.height+1) + pos.y) * 3;
		float *vertArray = (float*)m_pOriginalVertices;

		MGVertex vert = {vertArray[index], vertArray[index+1], vertArray[index+2]};

		return vert;
	}

	void MGGrid::setVertex( const MGIntPoint &pos,const MGVertex& vertex )
	{
		int index = (pos.x * (m_sGridSize.height + 1) + pos.y) * 3;
		float *vertArray = (float*)m_pVertices;
		vertArray[index] = vertex.x;
		vertArray[index+1] = vertex.y;
		vertArray[index+2] = vertex.z;
	}

	void MGGrid::update(float time)
	{
		float tt = MAXVALUE(0, time - 0.25f);
		float deltaAy = (tt * tt * 500);
		float ay = -100 - deltaAy;

		float deltaTheta = - (float) M_PI_2 * sqrtf( time) ;
		float theta = /*0.01f */ + (float) M_PI_2 +deltaTheta;

		float sinTheta = sinf(theta);
		float cosTheta = cosf(theta);

		for (int i = 0; i <= m_sGridSize.width; ++i)
		{
			for (int j = 0; j <= m_sGridSize.height; ++j)
			{
				// Get original vertex
				MGVertex p = originalVertex(MGIntPointMake(i ,j));

				float R = sqrtf((p.x * p.x) + ((p.y - ay) * (p.y - ay)));
				float r = R * sinTheta;
				float alpha = asinf( p.x / R );
				float beta = alpha / sinTheta;
				float cosBeta = cosf( beta );

				p.x = ( r * sinf(beta));
				p.y = ( R + ay - ( r * (1 - cosBeta) * sinTheta));

				// We scale z here to avoid the animation being
				// too much bigger than the screen due to perspectve transform
				p.z = (r * ( 1 - cosBeta ) * cosTheta) / 7;// "100" didn't work for

				//	Stop z coord from dropping beneath underlying page in a transition
				// issue #751
//				if( p.z < 0.5f )
//				{
//					p.z = 0.5f;
//				}

				// Set new coords
				setVertex(MGIntPointMake(i, j), p);
                //MGLogD("(%d,%d)=%f,%f,%f",i,j,p.x,p.y,p.z);

			}
		}
        //MGLogD("MGGrid One Frame ===============================================");
	}

	//////////////////////////////////////////////////////////////////////////
	typedef float kmScalar ;
	typedef struct kmMat4 {
		float mat[16];
	} kmMat4;
	static kmMat4* const kmMat4Identity(kmMat4* pOut)
	{
		memset(pOut->mat, 0, sizeof(float) * 16);
		pOut->mat[0] = pOut->mat[5] = pOut->mat[10] = pOut->mat[15] = 1.0f;
		return pOut;
	}
	kmMat4* const kmMat4OrthographicProjection(kmMat4* pOut, kmScalar left,
		kmScalar right, kmScalar bottom,
		kmScalar top, kmScalar nearVal,
		kmScalar farVal)
	{
		kmScalar tx = -((right + left) / (right - left));
		kmScalar ty = -((top + bottom) / (top - bottom));
		kmScalar tz = -((farVal + nearVal) / (farVal - nearVal));

		kmMat4Identity(pOut);
		pOut->mat[0] = 2 / (right - left);
		pOut->mat[5] = 2 / (top - bottom);
		pOut->mat[10] = -2 / (farVal - nearVal);
		pOut->mat[12] = tx;
		pOut->mat[13] = ty;
		pOut->mat[14] = tz;

		return pOut;
	}
	//////////////////////////////////////////////////////////////////////////

	void set2DProjection()
	{
        MGIntSize winSize(MGDevice::getScreenWidth(),MGDevice::getScreenHeight());
		glLoadIdentity();

		// set view port for user FBO, fixed bug #543 #544
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, winSize.width, 0, winSize.height, 0, 2500);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
		gluLookAt( 0, 0, winSize.height / (2 * tan(M_PI / 6)),
                  0, 0, 0,
                  0.0f, 1.0f, 0.0f
                  );
        glViewport((GLsizei)0, (GLsizei)0, (GLsizei)winSize.width, (GLsizei)winSize.height);
	}
	void set3DProjection()
	{
		MGIntSize winSize(MGDevice::getScreenWidth(),MGDevice::getScreenHeight());
		// set view port for user FBO, fixed bug #543 #544
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60, (GLfloat)winSize.width/winSize.height, 0.0001f, 2500.0f);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt( winSize.width/2, winSize.height/2, winSize.height / (2 * tan(M_PI / 6)),
			winSize.width/2, winSize.height/2, 0,
			0.0f, 1.0f, 0.0f
			);
        glViewport(0, 0, (GLsizei)winSize.width, (GLsizei)winSize.height);
	}
	void z11Projection()
	{
        z11::MGSize size(MGDevice::getScreenWidth(),MGDevice::getScreenHeight());

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60, (GLfloat)size.width/size.height, 0.0001f, 2500.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt( size.width/2, size.height/2, size.height / (2 * tan(M_PI / 6)),
			size.width/2, size.height/2, 0,
			0.0f, 1.0f, 0.0f);

        glViewport(0, 0, size.width, size.height);
		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	}
	void MGGrid::blit()
	{
		if (m_pImage == NULL)
		{
			return ;
		}
        m_pImage->sendDataToContext();
		glColor4f(1.0f,1.0f,1.0f,1.0f);
		//glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		MGGraphicEngineContext context = m_pImage->context;
		GLint activeTextureID;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &activeTextureID);
		glBindTexture(GL_TEXTURE_2D, context.texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
		set2DProjection();

// liuhao: There is no function named glClearDepthf() in Win32!!
#ifdef WIN32
        glDepthRange(0.0, 1.0);     // If we don't do this, it will act abnormal in windows. //
		glClearDepth(1.0);
#else
        glDepthRangef(0.0f, 1.0f);  // If we don't do this, it will act abnormal in some android devices. //
		glClearDepthf(1.0f);
#endif
        glClear(GL_DEPTH_BUFFER_BIT); // If we don't do this, it will act abnormal in windows and some android devices. //
		glDepthFunc(GL_LEQUAL);

		//before blit

		int n = m_sGridSize.width * m_sGridSize.height;

		// Default GL states: GL_TEXTURE_2D, GL_VERTEX_ARRAY, GL_COLOR_ARRAY, GL_TEXTURE_COORD_ARRAY
		// Needed states: GL_TEXTURE_2D, GL_VERTEX_ARRAY, GL_TEXTURE_COORD_ARRAY
		// Unneeded states: GL_COLOR_ARRAY
		//glDisableClientState(GL_COLOR_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, m_pVertices);
		glTexCoordPointer(2, GL_FLOAT, 0, m_pTexCoordinates);
		glDrawElements(GL_TRIANGLES, (GLsizei)n * 6, GL_UNSIGNED_SHORT, m_pIndices);

		// restore GL default state
		//glEnableClientState(GL_COLOR_ARRAY);

		//after blit
        glDisable(GL_DEPTH_TEST);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_TEXTURE_2D);
		z11Projection();
		//glPopMatrix();
		//set3DProjection();

	}

	z11::MGVertex MGGrid::vertex( const MGIntPoint& pos )
	{
		int index = (pos.x * (m_sGridSize.height+1) + pos.y) * 3;
		float *vertArray = (float*)m_pVertices;

		MGVertex vert = {vertArray[index], vertArray[index+1], vertArray[index+2]};

		return vert;
	}


    bool MGScreenGrid::initWithScreen()
    {
        //capture image pixels
        int width=MGDevice::getScreenWidth();
        int height=MGDevice::getScreenHeight();
        int line_width=width * 4;
        unsigned char *data=new unsigned char[line_width*height];
        glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,data);

//        FILE *pFile;
		char header[256];
		int x,y;



		sprintf(header, "P6\n%d %d\n255\n", width, height);
		int header_size=strlen(header);
		int output_line_size=width*3;
		unsigned int buffer_size=header_size+output_line_size * height;
		unsigned char *buffer=new unsigned char[buffer_size];
		memcpy(buffer,header,header_size);
		unsigned char *pt=buffer+header_size;
		unsigned char *line_src=data+line_width*(height-1);
		for (y = 0; y < height; y++) {
			unsigned char *src=line_src ;
		    for(x = 0; x<width;x++)
		    {
		        memcpy(pt,src ,3);
		        pt+=3;
		        src+=4;
			}
            //*(pt-9)=*(pt-8)=*(pt-7)=0;
            //*(pt-6)=*(pt-5)=*(pt-4)=0;
            //*(pt-3)=*(pt-2)=*(pt-1)=0;
			line_src-=line_width;
		}
		//FILE *file=fopen(MGBundle::mainBundle()->pathWithFileName("image.ppm").c_str(),"wb");
		//fwrite(buffer,buffer_size,1,file);
		//fclose(file);
		z11::MGData *_data=z11::MGData::dataWithBytesNoCopy(buffer,buffer_size);
		MGImage *m_image=z11::MGImage::imageWithData(_data);
        //end capture image pixels

        int width_s = width / DEFAULT_GRID_STEP;
        int height_s = height / DEFAULT_GRID_STEP;
        if (width % DEFAULT_GRID_STEP != 0) {
            width_s += 1;
        }
        if (height_s % DEFAULT_GRID_STEP != 0) {
            height_s += 1;
        }
		initWithSize(MGIntSize(width_s,height_s),m_image,false);
		//if (false)
		//{
		//	std::vector<unsigned char> vt;
		//	MGPPMCodec::encode(m_image,vt);


		//	FILE *file=fopen("D:/1.png","wb");
		//	if (file!=NULL)
		//	{
		//		fwrite(&vt[0],vt.size(),1,file);
		//		fclose(file);
		//	}
		//}
		delete_and_set_null(_data);
		delete_and_set_null(data);
		return true;
    }

	MGScreenGrid::~MGScreenGrid()
	{

	}
}
