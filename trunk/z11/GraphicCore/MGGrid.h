#pragma once
#include "../CommonBase/MGStructure.h"
#include "GraphicStructure.h"
namespace z11
{
	class MGImage;
	class MGGrid
	{
	public:
		MGGrid(void);
		virtual ~MGGrid(void);
		bool initWithSize(MGIntSize gridSize, MGImage *pTexture, bool bFlipped);
		void blit();
		void update(float time);

		MGVertex vertex(const MGIntPoint& pos);
		MGVertex originalVertex(const MGIntPoint &pos);
		void setVertex(const MGIntPoint &pos,const MGVertex& vertex);
		void setScale(const MGRectScale &scale)
		{
		    m_scale=scale;
		}

	private:
		void calculateVertexPoints(void);

		MGImage *	m_pImage;
		MGIntSize	m_sGridSize;
		MGSize m_obStep;
		bool m_bIsTextureFlipped;
		MGRectScale m_scale;

		float		*m_pVertices;
		float		*m_pOriginalVertices;
		float		*m_pTexCoordinates;
		uint_16 	*m_pIndices;
	};

	class MGScreenGrid:public MGGrid
	{
	public:
		bool initWithScreen();
		virtual ~MGScreenGrid();

	};

}
