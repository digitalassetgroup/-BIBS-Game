#pragma once
#ifndef cannonboy_cannon_arc_helper_node_h
#define cannonboy_cannon_arc_helper_node_h

#include "cocos2d.h"
#include "../utilities/DragonVec2.h"
 
class CannonArcHelperNode : public cocos2d::CCNode
{
public:
	
	enum { LINE_SEGMENT_THRESHOLD = 20 };
	
	CannonArcHelperNode() : m_arcPath(nullptr), m_drawPath(false)
	{
		
	}
	virtual void draw(void);
	
	void setArcPath(std::vector<DragonVec2>* path)
	{
		m_arcPath = path;
	}
	
	void setDraw(bool draw)
	{
		m_drawPath = draw;
	}
	bool getDraw()
	{
		return m_drawPath;
	}
	
private:
	
	std::vector<DragonVec2>* m_arcPath;
	bool m_drawPath;
};

#endif
