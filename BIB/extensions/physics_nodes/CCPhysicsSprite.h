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
#ifndef __PHYSICSNODES_CCPHYSICSSPRITE_H__
#define __PHYSICSNODES_CCPHYSICSSPRITE_H__

#include "cocos2d.h"
#include "ExtensionMacros.h"

#define CC_ENABLE_CHIPMUNK_INTEGRATION 1

#if  CC_ENABLE_BOX2D_INTEGRATION

class b2Body;
class b2World;
class b2BodyDef;
class b2FixtureDef;
class b2Contact;
class b2Manifold;
class b2ContactImpulse;

#include "Box2D/Common/b2Math.h"

enum SKPCategories
{
	SKP_GoodGuys = 1,
	SKP_BadGuys  = 1 << 1,
	SKP_Level    = 1 << 2,
	SKP_Items    = 1 << 3
};

enum SKPType
{
	SKPT_UnknownType,
	SKPT_Sensor
};

#elif CC_ENABLE_CHIPMUNK_INTEGRATION
#include "chipmunk.h"

#else // CC_ENABLE_BOX2D_INTEGRATION
#error "You must define either CC_ENABLE_CHIPMUNK_INTEGRATION or CC_ENABLE_BOX2D_INTEGRATION to use CCPhysicsSprite.h"
#endif

NS_CC_EXT_BEGIN
/** A CCSprite subclass that is bound to a physics body.
 It works with:
 - Chipmunk: Preprocessor macro CC_ENABLE_CHIPMUNK_INTEGRATION should be defined
 - Objective-Chipmunk: Preprocessor macro CC_ENABLE_CHIPMUNK_INTEGRATION should be defined
 - Box2d: Preprocessor macro CC_ENABLE_BOX2D_INTEGRATION should be defined

 Features and Limitations:
 - Scale and Skew properties are ignored.
 - Position and rotation are going to updated from the physics body
 - If you update the rotation or position manually, the physics body will be updated
 - You can't enble both Chipmunk support and Box2d support at the same time. Only one can be enabled at compile time
 * @js NA
 * @lua NA
 */
class CCPhysicsSprite : public CCSprite
{
protected:
    bool    m_bIgnoreBodyRotation;


#if CC_ENABLE_BOX2D_INTEGRATION
    b2Body  *m_pB2Body;
	bool _beginContactCallEnabled;
	bool _endContactCallEnabled;
	bool _preContactCallEnabled;
	bool _postContactCallEnabled;
	SKPType _type;
#elif CC_ENABLE_CHIPMUNK_INTEGRATION
    cpBody  *m_pCPBody;
#endif // CC_ENABLE_CHIPMUNK_INTEGRATION
public:
    CCPhysicsSprite();

    static CCPhysicsSprite* create();
    /** Creates an sprite with a texture.
     The rect used will be the size of the texture.
     The offset will be (0,0).
     */
    static CCPhysicsSprite* createWithTexture(CCTexture2D *pTexture);

    /** Creates an sprite with a texture and a rect.
     The offset will be (0,0).
     */
    static CCPhysicsSprite* createWithTexture(CCTexture2D *pTexture, const CCRect& rect);

    /** Creates an sprite with an sprite frame. */
    static CCPhysicsSprite* createWithSpriteFrame(CCSpriteFrame *pSpriteFrame);

    /** Creates an sprite with an sprite frame name.
     An CCSpriteFrame will be fetched from the CCSpriteFrameCache by name.
     If the CCSpriteFrame doesn't exist it will raise an exception.
     @since v0.9
     */
    static CCPhysicsSprite* createWithSpriteFrameName(const char *pszSpriteFrameName);

    /** Creates an sprite with an image filename.
     The rect used will be the size of the image.
     The offset will be (0,0).
     */
    static CCPhysicsSprite* create(const char *pszFileName);

    /** Creates an sprite with an image filename and a rect.
     The offset will be (0,0).
     */
    static CCPhysicsSprite* create(const char *pszFileName, const CCRect& rect);

    virtual bool isDirty();

    /** Keep the sprite's rotation separate from the body. */
    bool isIgnoreBodyRotation() const;
    void setIgnoreBodyRotation(bool bIgnoreBodyRotation);

    virtual const CCPoint& getPosition();
    virtual void getPosition(float* x, float* y);
    virtual float getPositionX();
    virtual float getPositionY();
    virtual void setPosition(const CCPoint &position);
    virtual float getRotation();
    virtual void setRotation(float fRotation);
    virtual CCAffineTransform nodeToParentTransform();

 #if  CC_ENABLE_BOX2D_INTEGRATION
    /** Body accessor when using box2d */
    b2Body* getB2Body() const;
    void setB2Body(b2Body *pBody);
	
	// note - this function does NOT set body, it is just utily function
	b2Body* setB2BoxBodyFromSprite(b2World * world);
	virtual void defineBody(b2BodyDef * bodyDef);
	virtual void defineFixture(b2FixtureDef * fixtureDef);
	
	void setVelocityInPixels(float x, float y);
	
	inline bool beginContactCallEnabled() const
	{
		return _beginContactCallEnabled;
	}
	
	inline bool endContactCallEnabled() const
	{
		return _endContactCallEnabled;
	}
	
	inline bool preContactCallEnabled() const
	{
		return _preContactCallEnabled;
	}
	
	inline bool postContactCallEnabled() const
	{
		return _postContactCallEnabled;
	}
	
	inline SKPType skptype() const
	{
		return _type;
	}
	
	CCPhysicsSprite * getOther(b2Contact * contact);
	
	virtual void handleBeginContact(b2Contact* contact);
    virtual void handleEndContact(b2Contact* contact);
    virtual void handlePreSolve(b2Contact* contact, const b2Manifold* oldManifold);
    virtual void handlePostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
	
	// box2d and porting utilties
	static b2Vec2 Rotateb2Vec2(const b2Vec2& vector, float angle);
	static b2Vec2 getWorldNormal(b2Contact * contact);
	
	static void setB2objectScale(float scale);
#elif CC_ENABLE_CHIPMUNK_INTEGRATION
	/** Body accessor when using regular Chipmunk */
    cpBody* getCPBody() const;
    void setCPBody(cpBody *pBody);
#endif // CC_ENABLE_BOX2D_INTEGRATION

protected:
    void updatePosFromPhysics();
};

NS_CC_EXT_END

#endif // __PHYSICSNODES_CCPHYSICSSPRITE_H__
