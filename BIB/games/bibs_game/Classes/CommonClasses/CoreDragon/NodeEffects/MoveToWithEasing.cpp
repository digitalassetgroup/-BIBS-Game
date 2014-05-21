#include "MoveToWithEasing.h"
#include "Core/MathHelper.h"
using namespace DracoGames;

MoveToWithEasing::MoveToWithEasing()
	: CCActionInterval(), m_elapsedManual(0), 
	_easingType(EasingCurveType::EasingCurveLinear), _easingBackOvershoot(0), _node()
{
}

MoveToWithEasing* MoveToWithEasing::create(float duration, const CCPoint& position)
{
    auto action = new MoveToWithEasing();
    action->initWithDuration(duration, position);
    action->autorelease();

    return action;
}

bool MoveToWithEasing::initWithDuration(float duration, const CCPoint& position)
{
    if (CCActionInterval::initWithDuration(duration))
    {
        m_endPosition = position;
        return true;
    }

    return false;
}

CCObject* MoveToWithEasing::copyWithZone(CCZone *pZone)
{
    CCZone* pNewZone = NULL;
    MoveToWithEasing* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject) 
    {
        //in case of being called at sub class
        pCopy = (MoveToWithEasing*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new MoveToWithEasing();
        pZone = pNewZone = new CCZone(pCopy);
    }

    CCActionInterval::copyWithZone(pZone);

    pCopy->initWithDuration(m_fDuration, m_endPosition);
	pCopy->_node = _node;

    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

void MoveToWithEasing::startWithTarget(CCNode* target)
{
    CCActionInterval::startWithTarget(target);
    m_startPosition = GetTarget()->getPosition();
    m_delta = ccpSub(m_endPosition, m_startPosition);
}

MoveToWithEasing* MoveToWithEasing::SetEasingType(DracoGames::EasingCurveType easingType, float backOvershoot)
{
	_easingType = easingType;
	_easingBackOvershoot = backOvershoot;
	return this;
}

MoveToWithEasing* MoveToWithEasing::WithNode(CCNode* node) 
{ 
	_node = node;
	return this;
}

CCNode* MoveToWithEasing::GetTarget() 
{ 
	return _node != nullptr ? _node : m_pTarget; 
}

void MoveToWithEasing::update(float time)
{
	m_elapsedManual = time * m_fDuration; //we cant use m_elapsed, because in CCSequence m_elapsed not incremented
	float percent = m_elapsedManual / m_fDuration;
	//char ch[30]; sprintf(ch, "%f", m_elapsed); CCLog(ch);  //log

	if (percent > 1.0f)
		percent = 1.0f;
	
	//calculate realPercent
	bool isHasOvershoot = !Math::IsZero(_easingBackOvershoot);
	if (isHasOvershoot)
		EasingCurves::push_overshoot(_easingBackOvershoot);
	const float realPercent = EasingCurves::eval(percent, _easingType);	
	if (isHasOvershoot)
		EasingCurves::pop_overshoot();

	CCPoint posNew;
	posNew.x = m_startPosition.x + (m_endPosition.x - m_startPosition.x) * realPercent;
	posNew.y = m_startPosition.y + (m_endPosition.y - m_startPosition.y) * realPercent;
	GetTarget()->setPosition(posNew);
}