#pragma once
#ifndef SUPER_KID_UI_FULLSCREEN_QUAD_H
#define SUPER_KID_UI_FULLSCREEN_QUAD_H "$Rev: 229 $"

#include "cocos2d.h"

namespace ig_interface
{
	
	class FullscreenQuad : public cocos2d::CCNode
	{
	public:
		static const cocos2d::ccColor3B ColorBlack;
		static const cocos2d::ccColor3B ColorWhite;
		
		~FullscreenQuad(void);
		static FullscreenQuad* makeFullscreenQuad(const cocos2d::ccColor3B& color = ColorBlack, const float opacity = 1.0f);
		
		virtual void draw(void);
	
		void setOpacity(const float opacity) { m_opacity = opacity; }
		float getOpacity() const { return m_opacity; }
		
		void setColor(const cocos2d::ccColor3B color) { m_color = color; }
		
		/**
		 * @param shouldTrackCamera - True if quad should be positioned at global camera pos
		 */
		void setShouldTrackCamera(const bool shouldTrackCamera) { m_shouldTrackCamera = shouldTrackCamera; };
		
		/**
		 * @param isColoring - True if quad should colorize nodes drawn before itself
		 */
		void setIsColoring(const bool isColoring) { m_isColoring = isColoring; };
	
		void setIsOneMinusAlpha(const bool isOneMinusAlpha) { m_isOneMinusAlpha = isOneMinusAlpha; }
		
	private:
		FullscreenQuad(void);
		FullscreenQuad(const FullscreenQuad& obj);
		FullscreenQuad& operator=(const FullscreenQuad& obj);
	
		float m_opacity;
		cocos2d::ccColor3B m_color;
		bool m_shouldTrackCamera;
		bool m_isColoring;
		bool m_isOneMinusAlpha;
	};
	
};	//namespace ig_interface

#endif /* SUPER_KID_UI_FULLSCREEN_QUAD_H */
