#pragma once

#ifndef __SKP_PLATFORM_H___
#define __SKP_PLATFORM_H___

#include "CCPhysicsSprite.h"

USING_NS_CC_EXT;

class b2Fixture;
class b2Contact;
class b2Manifold;

class SKPPlatform : public CCPhysicsSprite
{
public:
	SKPPlatform();
    static SKPPlatform * create(const char *pszFileName);
	virtual void defineBody(b2BodyDef * bodyDef);
	virtual void defineFixture(b2FixtureDef * fixtureDef);
    virtual void handlePreSolve(b2Contact* contact, const b2Manifold* oldManifold);
private:
	bool _oneWay;
};





#endif //#ifndef __SKP_PLATFORM_H___