#include "node_effect_button_slide_in.h"

#include "../utilities/ig_defines.h"
#include "../utilities/ig_assert.h"
#include "../utilities/ig_utilities.h"
#include "../utilities/ig_easing_curves.h"

//#include "cannonboy.h"
#include "cocos2d.h"
using namespace cocos2d;
using namespace DracoGames;
using namespace DracoGames::Utils;

static const float MaxButtonSlideDistance(300.0f);
//static const float MaxButtonSlideTime(1.25f);
static const float MaxButtonSlideTime(0.75f);	/* 60% faster. */

const float NodeEffectButtonSlideIn::SceneStartDelay(0.5f);
const float NodeEffectButtonSlideIn::PerNodeDelay(0.1f);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

NodeEffectButtonSlideIn::NodeEffectButtonSlideIn(cocos2d::CCNode& nodeToEffect, const ButtonSlideDirection& fromDirection,
												 const float slideDelay) :
	INodeEffect(nodeToEffect),
	m_moveToTimer(MaxButtonSlideTime),
	m_delayTimer(slideDelay)
{
	CCPoint startPosition(m_node.getPosition());
	CCPoint finalPosition(m_node.getPosition());
	
	switch (fromDirection)
	{
		case SLIDE_FROM_LEFT:	startPosition.x += -MaxButtonSlideDistance * g_convertRatio->x;		break;
		case SLIDE_FROM_RIGHT:	startPosition.x += MaxButtonSlideDistance * g_convertRatio->x;		break;			
		case SLIDE_FROM_TOP:	startPosition.y += MaxButtonSlideDistance * g_convertRatio->y;		break;
		case SLIDE_FROM_BOTTOM:	startPosition.y += -MaxButtonSlideDistance * g_convertRatio->y;		break;
		default:				assert_if(true, "Unkown direction to try sliding from.\n");			
	};
	
//	m_startPosition.setValue(startPosition);
//	m_finalPosition.setValue(finalPosition);
	
	m_startPosition = startPosition;
	m_finalPosition = finalPosition;
}

NodeEffectButtonSlideIn* NodeEffectButtonSlideIn::clone() const
{
	return NULL; //not implemened
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void NodeEffectButtonSlideIn::onStart(void)
{
//	m_startPosition.assertIfNotSet();
	m_node.setPosition(m_startPosition);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void NodeEffectButtonSlideIn::onFinish(void)
{
//	m_finalPosition.assertIfNotSet();
	m_node.setPosition(m_finalPosition);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void NodeEffectButtonSlideIn::onUpdate(const float deltaTime)
{	
	assert_if(deltaTime < 0.0f, "Invalid value for paramater; deltaTime(%f). Expected greater than 0.0f\n", deltaTime);
	assert_if(m_moveToTimer < 0.0f, "Total time has not been properly set.\n");
//	m_startPosition.assertIfNotSet();	
//	m_finalPosition.assertIfNotSet();
	
	if(m_delayTimer >= 0.0f)
	{	/* The moving of the object is currently delayed. */
		m_delayTimer -= deltaTime;
		return;
	}
	
	m_moveToTimer -= deltaTime;
	
	CCPoint newPosition;
	const CCPoint finalPosition(m_finalPosition);
	const CCPoint startPosition(m_startPosition);
	
	const float percentage(1.0f - Clamp((m_moveToTimer / MaxButtonSlideTime), 0.0f, 1.0f));	
	
	DracoGames::EasingCurves::push_overshoot(1.0f);
	const float value(DracoGames::EasingCurves::ec_out_back(percentage));
	DracoGames::EasingCurves::pop_overshoot();		
	
	newPosition.x = ((finalPosition.x - startPosition.x) * value) + startPosition.x;
	newPosition.y = ((finalPosition.y - startPosition.y) * value) + startPosition.y;
	m_node.setPosition(newPosition);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool NodeEffectButtonSlideIn::isComplete(void) const
{
	return (m_moveToTimer < 0.0f) ? true : false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

NodeEffectButtonSlideOut::NodeEffectButtonSlideOut(cocos2d::CCNode& nodeToEffect, const ButtonSlideDirection& towardDirection,
												   const float slideDelay) :
	NodeEffectButtonSlideIn(nodeToEffect, towardDirection, slideDelay)
{
	std::swap(m_startPosition, m_finalPosition);
}
