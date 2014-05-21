
#include "SKPPlatform.h"
#include "Box2D/Box2D.h"


SKPPlatform::SKPPlatform()
:
_oneWay(false)
{
}

SKPPlatform* SKPPlatform::create(const char *pszFileName)
{
    SKPPlatform* pRet = new SKPPlatform();
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

void SKPPlatform::defineBody(b2BodyDef * bodyDef)
{
	CCPhysicsSprite::defineBody(bodyDef);
	bodyDef->type = b2_staticBody;
}

void SKPPlatform::defineFixture(b2FixtureDef * fixtureDef)
{
	CCPhysicsSprite::defineFixture(fixtureDef);
	fixtureDef->restitution         = 0;
}

void SKPPlatform::handlePreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{

}





























