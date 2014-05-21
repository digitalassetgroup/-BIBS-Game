#include "ui_fullscreen_quad.h"
#include "../utilities/ig_utilities.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace cocos2d;

namespace ig_interface
{
	const cocos2d::ccColor3B FullscreenQuad::ColorBlack = ccc3(0x00, 0x00, 0x00);
	const cocos2d::ccColor3B FullscreenQuad::ColorWhite = ccc3(0xFF, 0xFF, 0xFF);
	
	FullscreenQuad* FullscreenQuad::makeFullscreenQuad(const cocos2d::ccColor3B& color, const float opacity)
	{
		FullscreenQuad* newQuad = new FullscreenQuad();
		newQuad->setColor(color);
		newQuad->setOpacity(opacity);
		return newQuad;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	FullscreenQuad::FullscreenQuad(void) :
		m_color(cocos2d::ccc3(0x00, 0x00, 0x00)),
		m_opacity(0.0f),
		m_shouldTrackCamera(true),
		m_isColoring(false),
		m_isOneMinusAlpha(false)
	{
		autorelease();
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	FullscreenQuad::~FullscreenQuad(void)
	{
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void FullscreenQuad::draw(void)
	{
#if COCOS2D_VERSION < 0x00020000		
		GLint oldBlendDST(GL_ZERO);
		glGetIntegerv(GL_BLEND_DST, &oldBlendDST);
		
		GLint oldBlendSRC(GL_ZERO);
		glGetIntegerv(GL_BLEND_SRC, &oldBlendSRC);
		
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_ALPHA_TEST);
		
		if(true == m_isColoring)
		{
			glBlendFunc(GL_ONE, GL_SRC_COLOR);
		}
		else if(true == m_isOneMinusAlpha)
		{
			//glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		
		
		const cocos2d::CCPoint camPos(0, 0);// = toVec(g_camera->getDefinitePosition());
		const float yPos(m_shouldTrackCamera ? camPos.y : 0.0f);
		
		const cocos2d::CCPoint vertices[4] = 
		{
			ccp(0.0f, yPos + 0.0f), ccp(gScreenSize.width, yPos + 0.0f),
			ccp(gScreenSize.width, yPos + gScreenSize.height), ccp(0.0f, yPos + gScreenSize.height)
		};		
	
		glColor4ub(m_color.r, m_color.g, m_color.b, static_cast<GLubyte>(255.0f * m_opacity));
		cocos2d::ccDrawPoly(vertices, 4, true, true);
		

		
		/* Reset the Blend Func back to the original state. */
		glBlendFunc(static_cast<GLenum>(oldBlendSRC), static_cast<GLenum>(oldBlendDST));
		

		
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnable(GL_TEXTURE_2D);
#endif
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//namespace ig_interface
