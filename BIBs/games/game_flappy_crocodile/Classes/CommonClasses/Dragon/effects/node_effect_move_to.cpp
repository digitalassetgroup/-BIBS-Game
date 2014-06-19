#include "node_effect_move_to.h"

#include "../utilities/ig_defines.h"
#include "../utilities/ig_assert.h"
#include "../utilities/ig_utilities.h"

#include "cocos2d.h"
using namespace cocos2d;
using namespace DracoGames;
using namespace DracoGames::Utils;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MoveToEffect::MoveToEffect(cocos2d::CCNode& nodeToEffect) :
	INodeEffect(nodeToEffect),
	m_totalTime(-1.0f),
	m_moveToTimer(-1.0f),
	m_delayTimer(-1.0f),
    m_overshoot(DEFAULT_OVERSHOOT),
	m_startPosition(nodeToEffect.getPosition())
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MoveToEffect::moveWithDirectionAndSpeed(const cocos2d::CCPoint& goalPosition, const cocos2d::CCPoint& moveDirection,
											 const float moveSpeed, const float totalTime,
                                             DracoGames::EasingCurveType easingX /*= DracoGames::EasingCurveLinear*/, 
                                             DracoGames::EasingCurveType easingY /*= DracoGames::EasingCurveUnknown*/)
{
    moveWithDirectionAndDistance(goalPosition, moveDirection, totalTime * moveSpeed, totalTime, easingX, easingY);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MoveToEffect::moveWithDirectionAndDistance(const cocos2d::CCPoint& goalPosition, const cocos2d::CCPoint& moveDirection,
                                  const float distance, const float totalTime, 
                                  DracoGames::EasingCurveType easingX /*= DracoGames::EasingCurveLinear*/, 
                                  DracoGames::EasingCurveType easingY /*= DracoGames::EasingCurveUnknown*/) // use easingX if not specified
{
    assert_if(totalTime <= 0.0f, "Invalid value for parameter; totalTime(%f). Expected greater than 0.0f\n", totalTime);
	assert_if(distance <= 0.0f, "Invalid value for parameter; distance(%f). Expected greater than 0.0f\n", distance);
	assert_if(false == isEqual(1.0f, DistanceTo(0.0f, 0.0f, moveDirection.x, moveDirection.y)), 
			  "Invalid value for parameter; moveDirection(%f, %f). Expected normalized vector.\n", moveDirection.x, moveDirection.y);
	
	m_goalPosition = goalPosition;
	m_startPosition.x = m_goalPosition.x - (moveDirection.x * distance);
	m_startPosition.y = m_goalPosition.y - (moveDirection.y * distance);
	m_node.setPosition(m_startPosition);
	
	m_totalTime.setValue(totalTime);
	m_moveToTimer = m_totalTime;
    
    m_easingX = easingX;
    if (easingY == EasingCurveUnknown)
    {
        m_easingY = m_easingX;
    }
    else
    {
        m_easingY = easingY;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MoveToEffect::setMoveDelay(const float delayInSeconds)
{
	assert_if(delayInSeconds <= 0.0f, "Invalid value for parameter; delayInSeconds(%f). Expected greater than 0.0f.\n", delayInSeconds);
	
	m_delayTimer = delayInSeconds;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MoveToEffect::onStart(void)
{
	m_node.setPosition(m_startPosition);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MoveToEffect::onFinish(void)
{
	m_node.setPosition(m_goalPosition);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MoveToEffect::setOvershoot(const float overshoot)
{
    m_overshoot = overshoot;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MoveToEffect::onUpdate(const float deltaTime)
{	
	assert_if(deltaTime < 0.0f, "Invalid value for parameter; deltaTime(%f). Expected greater than 0.0f\n", deltaTime);
	m_totalTime.assertIfNotSet();
	assert_if(m_totalTime <= 0.0f, "Total time in not a valid value, expected greater than 0.0f.\n");
	assert_if(m_moveToTimer < 0.0f, "Total time has not been properly set.\n");
	
	if(m_delayTimer >= 0.0f)
	{	/* The moving of the object is currently delayed. */
		m_delayTimer -= deltaTime;
		return;
	}
	
	m_moveToTimer -= deltaTime;
	
	const float percentage(Clamp(1.0f - (m_moveToTimer / m_totalTime), 0.0f, 1.0f));
	CCPoint newPosition;
    DracoGames::EasingCurves::push_overshoot(m_overshoot);
	newPosition.x = DracoGames::EasingCurves::easing_interpolate(m_startPosition.x, m_goalPosition.x, percentage, m_easingX);
	newPosition.y = DracoGames::EasingCurves::easing_interpolate(m_startPosition.y, m_goalPosition.y, percentage, m_easingY);
    DracoGames::EasingCurves::pop_overshoot();
	m_node.setPosition(newPosition);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MoveToEffect::isComplete(void) const
{
	return (m_moveToTimer < 0.0f) ? true : false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
