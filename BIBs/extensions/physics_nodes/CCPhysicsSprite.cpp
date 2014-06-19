/* Copyright (c) 2012 Scott Lembcke and Howling Moon Software
 * Copyright (c) 2012 cocos2d-x.org
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "CCPhysicsSprite.h"
#include "support/CCPointExtension.h"

#if CC_ENABLE_BOX2D_INTEGRATION
#include "Box2D/Box2D.h"
static float m_fPTMRatio = 30.0f;
#elif CC_ENABLE_CHIPMUNK_INTEGRATION
#include "chipmunk.h"
#endif

NS_CC_EXT_BEGIN

CCPhysicsSprite::CCPhysicsSprite()
: m_bIgnoreBodyRotation(false)
#if CC_ENABLE_BOX2D_INTEGRATION
, m_pB2Body(NULL),
_beginContactCallEnabled(false),
_endContactCallEnabled(false),
_preContactCallEnabled(false),
_postContactCallEnabled(false),
_type(SKPT_UnknownType)
#elif CC_ENABLE_CHIPMUNK_INTEGRATION
, m_pCPBody(NULL)
#endif
{}

CCPhysicsSprite* CCPhysicsSprite::create()
{
    CCPhysicsSprite* pRet = new CCPhysicsSprite();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    
    return pRet;
}

CCPhysicsSprite* CCPhysicsSprite::createWithTexture(CCTexture2D *pTexture)
{
    CCPhysicsSprite* pRet = new CCPhysicsSprite();
    if (pRet && pRet->initWithTexture(pTexture))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

CCPhysicsSprite* CCPhysicsSprite::createWithTexture(CCTexture2D *pTexture, const CCRect& rect)
{
    CCPhysicsSprite* pRet = new CCPhysicsSprite();
    if (pRet && pRet->initWithTexture(pTexture, rect))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

CCPhysicsSprite* CCPhysicsSprite::createWithSpriteFrame(CCSpriteFrame *pSpriteFrame)
{
    CCPhysicsSprite* pRet = new CCPhysicsSprite();
    if (pRet && pRet->initWithSpriteFrame(pSpriteFrame))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

CCPhysicsSprite* CCPhysicsSprite::createWithSpriteFrameName(const char *pszSpriteFrameName)
{
    CCPhysicsSprite* pRet = new CCPhysicsSprite();
    if (pRet && pRet->initWithSpriteFrameName(pszSpriteFrameName))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

CCPhysicsSprite* CCPhysicsSprite::create(const char *pszFileName)
{
    CCPhysicsSprite* pRet = new CCPhysicsSprite();
    if (pRet && pRet->initWithFile(pszFileName))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

CCPhysicsSprite* CCPhysicsSprite::create(const char *pszFileName, const CCRect& rect)
{
    CCPhysicsSprite* pRet = new CCPhysicsSprite();
    if (pRet && pRet->initWithFile(pszFileName, rect))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

// this method will only get called if the sprite is batched.
// return YES if the physic's values (angles, position ) changed.
// If you return NO, then nodeToParentTransform won't be called.
bool CCPhysicsSprite::isDirty()
{
    return true;
}

bool CCPhysicsSprite::isIgnoreBodyRotation() const
{
    return m_bIgnoreBodyRotation;
}

void CCPhysicsSprite::setIgnoreBodyRotation(bool bIgnoreBodyRotation)
{
    m_bIgnoreBodyRotation = bIgnoreBodyRotation;
}

// Override the setters and getters to always reflect the body's properties.
const CCPoint& CCPhysicsSprite::getPosition()
{
#if CC_ENABLE_BOX2D_INTEGRATION == 1
	if (m_pB2Body)
	{
		updatePosFromPhysics();
	}
#endif
    return CCNode::getPosition();
}

void CCPhysicsSprite::getPosition(float* x, float* y)
{
    updatePosFromPhysics();
    return CCNode::getPosition(x, y);
}

float CCPhysicsSprite::getPositionX()
{
    updatePosFromPhysics();
    return m_obPosition.x;
}

float CCPhysicsSprite::getPositionY()
{
    updatePosFromPhysics();
    return m_obPosition.y;
}

#if CC_ENABLE_BOX2D_INTEGRATION

b2Body* CCPhysicsSprite::getB2Body() const
{
    return m_pB2Body;
}

void CCPhysicsSprite::setB2Body(b2Body *pBody)
{
    m_pB2Body = pBody;
}

// Override the setters and getters to always reflect the body's properties.



void CCPhysicsSprite::setB2objectScale(float scale)
{
	m_fPTMRatio = 30.0f * scale;
}

void CCPhysicsSprite::updatePosFromPhysics()
{
    b2Vec2 pos = m_pB2Body->GetPosition();
    float x = pos.x * m_fPTMRatio;
    float y = pos.y * m_fPTMRatio;
    m_obPosition = ccp(x,y);
}

void CCPhysicsSprite::setPosition(const CCPoint &pos)
{
	if (m_pB2Body)
	{
		float angle = m_pB2Body->GetAngle();
		m_pB2Body->SetTransform(b2Vec2(pos.x / m_fPTMRatio, pos.y / m_fPTMRatio), angle);
	}
	else
	{
		CCSprite::setPosition(pos);
	}
}

float CCPhysicsSprite::getRotation()
{
    return (m_bIgnoreBodyRotation || !m_pB2Body ? CCSprite::getRotation() :
            CC_RADIANS_TO_DEGREES(m_pB2Body->GetAngle()));
}

void CCPhysicsSprite::setRotation(float fRotation)
{
    if (m_bIgnoreBodyRotation || !m_pB2Body)
    {
        CCSprite::setRotation(fRotation);
    }
    else
    {
        b2Vec2 p = m_pB2Body->GetPosition();
        float radians = CC_DEGREES_TO_RADIANS(fRotation);
        m_pB2Body->SetTransform(p, radians);
    }
}

// returns the transform matrix according the Box2D Body values
CCAffineTransform CCPhysicsSprite::nodeToParentTransform()
{
	if (!m_pB2Body)
	{
		return CCSprite::nodeToParentTransform();
	}
    b2Vec2 pos  = m_pB2Body->GetPosition();
	
	float x = pos.x * m_fPTMRatio;
	float y = pos.y * m_fPTMRatio;
	
	if (m_bIgnoreAnchorPointForPosition)
    {
		x += m_obAnchorPointInPoints.x;
		y += m_obAnchorPointInPoints.y;
	}
	
	// Make matrix
	float radians = m_pB2Body->GetAngle();
	float c = cosf(radians);
	float s = sinf(radians);
	
	// Although scale is not used by physics engines, it is calculated just in case
	// the sprite is animated (scaled up/down) using actions.
	// For more info see: http://www.cocos2d-iphone.org/forum/topic/68990
	if (!m_obAnchorPointInPoints.equals(CCPointZero))
    {
		x += ((c * -m_obAnchorPointInPoints.x * m_fScaleX) + (-s * -m_obAnchorPointInPoints.y * m_fScaleY));
		y += ((s * -m_obAnchorPointInPoints.x * m_fScaleX) + (c * -m_obAnchorPointInPoints.y * m_fScaleY));
	}
    
	// Rot, Translate Matrix
	m_sTransform = CCAffineTransformMake( c * m_fScaleX,	s * m_fScaleX,
									     -s * m_fScaleY,	c * m_fScaleY,
									     x,	y );
	
	return m_sTransform;
}



b2Body* CCPhysicsSprite::setB2BoxBodyFromSprite(b2World * world)
{
	if (!world)
	{
		return 0;
	}
	if (m_pB2Body)
	{
		CCLOGWARN("Calling createB2BoxBody when m_pB2Body is not NULL, this can be potential error");
		return m_pB2Body;
	}
	if(getParent())
	{
		CCAssert(false, "This is not expected use case of CCPhysicsSprite::setB2BoxBodyFromSprite, it can produce incorrect results. Contact Draco, LLC to find out what can be done");
		return 0;
	}
	
	b2BodyDef def;
	CCPoint pt = CCSprite::getPosition();
	def.position.x = pt.x / m_fPTMRatio;
	def.position.y = pt.y / m_fPTMRatio;
	float rad =  -(float)CC_DEGREES_TO_RADIANS(CCSprite::getRotation());
	def.angle = rad;
	defineBody(&def);
	
	b2Body* res = world->CreateBody(&def);
	
	CCSize size = getContentSize();
	b2PolygonShape shape;
	
	b2Vec2 center;
	center.x =  m_fScaleX * (size.width * 0.5 - m_obAnchorPointInPoints.x) / m_fPTMRatio;
	center.y =  m_fScaleY * (size.height * 0.5 - m_obAnchorPointInPoints.y) / m_fPTMRatio;
	
	shape.SetAsBox(m_fScaleX * 0.5f * size.width / m_fPTMRatio, m_fScaleY * 0.5 * size.height / m_fPTMRatio, center, 0);

	b2FixtureDef fdef;	
	defineFixture(&fdef);

	fdef.shape = &shape;
	res->CreateFixture(&fdef);
	setB2Body(res);
	res->SetUserData(this);
	return res;
}

void CCPhysicsSprite::defineBody(b2BodyDef * bodyDef)
{
	bodyDef->type = b2_dynamicBody;
}

void CCPhysicsSprite::defineFixture(b2FixtureDef * fixtureDef)
{
	fixtureDef->density     = 1.0f;
	fixtureDef->friction    = 0.6f;
	fixtureDef->restitution = 0.3;
}

void CCPhysicsSprite::setVelocityInPixels(float x, float y)
{
	if (m_pB2Body)
	{
		m_pB2Body->SetLinearVelocity(b2Vec2(x / m_fPTMRatio, y / m_fPTMRatio));
	}
}

void CCPhysicsSprite::handleBeginContact(b2Contact* contact)
{
	
}

void CCPhysicsSprite::handleEndContact(b2Contact* contact)
{
	
}

void CCPhysicsSprite::handlePreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	
}

void CCPhysicsSprite::handlePostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	
}

CCPhysicsSprite * CCPhysicsSprite::getOther(b2Contact * contact)
{
	CCPhysicsSprite * o = (CCPhysicsSprite *)contact->GetFixtureA()->GetBody()->GetUserData();
	return this == o ? (CCPhysicsSprite *)contact->GetFixtureB()->GetBody()->GetUserData() : o;
}

b2Vec2 CCPhysicsSprite::Rotateb2Vec2(const b2Vec2& vector, float angle)
{
	float cs = cos(angle);
	float sn = sin(angle);
	return b2Vec2(vector.x * cs - vector.y * sn, vector.x * sn + vector.y * cs);
}

b2Vec2 CCPhysicsSprite::getWorldNormal(b2Contact * contact)
{
	b2WorldManifold m;
	contact->GetWorldManifold(&m);
	return m.normal;
}

#elif CC_ENABLE_CHIPMUNK_INTEGRATION

cpBody* CCPhysicsSprite::getCPBody() const
{
    return m_pCPBody;
}

void CCPhysicsSprite::setCPBody(cpBody *pBody)
{
    m_pCPBody = pBody;
}

void CCPhysicsSprite::updatePosFromPhysics()
{
    cpVect cpPos = cpBodyGetPos(m_pCPBody);
    m_obPosition = ccp(cpPos.x, cpPos.y);
}

void CCPhysicsSprite::setPosition(const CCPoint &pos)
{
    cpVect cpPos = cpv(pos.x, pos.y);
    cpBodySetPos(m_pCPBody, cpPos);
}

float CCPhysicsSprite::getRotation()
{
    return (m_bIgnoreBodyRotation ? CCSprite::getRotation() : -CC_RADIANS_TO_DEGREES(cpBodyGetAngle(m_pCPBody)));
}

void CCPhysicsSprite::setRotation(float fRotation)
{
    if (m_bIgnoreBodyRotation)
    {
        CCSprite::setRotation(fRotation);
    }
    else
    {
        cpBodySetAngle(m_pCPBody, -CC_DEGREES_TO_RADIANS(fRotation));
    }
}

// returns the transform matrix according the Chipmunk Body values
CCAffineTransform CCPhysicsSprite::nodeToParentTransform()
{
    // Although scale is not used by physics engines, it is calculated just in case
	// the sprite is animated (scaled up/down) using actions.
	// For more info see: http://www.cocos2d-iphone.org/forum/topic/68990
	cpVect rot = (m_bIgnoreBodyRotation ? cpvforangle(-CC_DEGREES_TO_RADIANS(m_fRotationX)) : m_pCPBody->rot);
	float x = m_pCPBody->p.x + rot.x * -m_obAnchorPointInPoints.x * m_fScaleX - rot.y * -m_obAnchorPointInPoints.y * m_fScaleY;
	float y = m_pCPBody->p.y + rot.y * -m_obAnchorPointInPoints.x * m_fScaleX + rot.x * -m_obAnchorPointInPoints.y * m_fScaleY;
	
	if (m_bIgnoreAnchorPointForPosition)
    {
		x += m_obAnchorPointInPoints.x;
		y += m_obAnchorPointInPoints.y;
	}
	
	return (m_sTransform = CCAffineTransformMake(rot.x * m_fScaleX, rot.y * m_fScaleX,
                                                 -rot.y * m_fScaleY, rot.x * m_fScaleY,
                                                 x,	y));
}

#endif

NS_CC_EXT_END
