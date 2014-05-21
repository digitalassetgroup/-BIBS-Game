#include "CCAdvancedSprite.h"
#include <sprite_nodes/CCSpriteFrame.h>

CCAdvancedSprite::CCAdvancedSprite()
:m_pfnSelectorDelegate(0)
,m_pTarget(0)
,m_isPlistLoaded(false)
,m_ElaspeTime(0.0)
,m_IncrementValue(0)
,m_isAnimationRunning(false)
,m_AnimationFrames(NULL),
m_framesCount(0),
m_isManualUpdate(true),
_isPauseAnimation(false),
_constantSize(),
_delays()
{
   
}

CCAdvancedSprite::~CCAdvancedSprite()
{
    if (m_isPlistLoaded)
	{
        CC_SAFE_DELETE_ARRAY(m_AnimationFrames);
	}

	ClearSpriteFrames();
}

void CCAdvancedSprite::ClearSpriteFrames()
{
	if (m_AnimationFrames == NULL)
		return;

	for (size_t i = 0; i < m_framesCount; ++i) {
		m_AnimationFrames[i]->release();
	}

	CC_SAFE_DELETE_ARRAY(m_AnimationFrames);
}

void CCAdvancedSprite::setFrames(const char* pList)
{
 //   m_isPlistLoaded = true;
 //   
 //   CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(pList);
 //   const char *pszPath = CCFileUtils::fullPathForFilename(pList);
	//auto dict = CCFileUtils::dictionaryWithContentsOfFile(pszPath);
 //   auto framesDict = (CCDictionary<string, CCObject*>*)dict->objectForKey(string("frames"));

 //   framesDict->begin();
	//std::string key = "";
	//while((CCDictionary<std::string, CCObject*>*)framesDict->next(&key))
	//{
	//	m_AnimationFrames->push_back(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(key.c_str()));
 //   }

 //   setDisplayFrame(m_AnimationFrames[0]);
}

void CCAdvancedSprite::setFrames(CCSpriteFrame** frames, int framesCount)
{
	ClearSpriteFrames();

	m_AnimationFrames = new CCSpriteFrame*[framesCount];
	for (int i = 0; i < framesCount; ++i) {
		m_AnimationFrames[i] = frames[i];
		m_AnimationFrames[i]->retain();
	}

	m_framesCount = framesCount;
	m_CurrentIndex = 0;
    setDisplayFrame(m_AnimationFrames[m_CurrentIndex]);
}

void CCAdvancedSprite::setFrames(CCSpriteFrame** frames, int framesCount, int displayTextureIndex)
{
    m_AnimationFrames = frames;
	m_framesCount = framesCount;
	m_CurrentIndex = displayTextureIndex;
	setDisplayFrame(m_AnimationFrames[m_CurrentIndex]);
}

void CCAdvancedSprite::SetCurrentFrame()
{
	setDisplayFrame(m_AnimationFrames[m_CurrentIndex]);
}

void CCAdvancedSprite::increaseCurrentIndex()
{
    if(m_AnimationMode && (m_EndingIndex - m_CurrentIndex) != 0)
        m_IncrementValue += (m_EndingIndex - m_CurrentIndex) / abs(m_EndingIndex - m_CurrentIndex);
    else if((m_StartingIndex - m_CurrentIndex) != 0)
        m_IncrementValue += (m_StartingIndex - m_CurrentIndex) / abs(m_StartingIndex - m_CurrentIndex);
}

void CCAdvancedSprite::SetConstantSizeInSprite()
{
#if COCOS2D_VERSION >= 0x00020000
	m_obRect.size = CCSize(_constantSize.width, _constantSize.height);
	setTextureRect(m_obRect, false, m_obRect.size);
#else
	m_obRectInPixels.size = _constantSize;
	setTextureRect(m_obRectInPixels);
#endif
}

void CCAdvancedSprite::SetConstantFrameSize(const CCSize& size)
{ 
	_constantSize = size;
	SetConstantSizeInSprite();
}

void CCAdvancedSprite::ManualUpdate(ccTime dt)
{
	if (!m_isAnimationRunning) return;
	if (_isPauseAnimation) return;

    m_ElaspeTime += dt;

	if (_delays != NULL)
		m_FrameRate = (m_CurrentIndex == -1) ? _delays[m_framesCount - 1] : _delays[m_CurrentIndex];

    while (m_ElaspeTime >= m_FrameRate) 
    {
        m_ElaspeTime -= m_FrameRate;
        
        m_CurrentIndex += m_IncrementValue;

		
        setDisplayFrame(m_AnimationFrames[m_CurrentIndex]);
	
		//при смене кадра размер кадра оставляем прежний
		if (!_constantSize.equals(CCSizeZero))
		{			
			SetConstantSizeInSprite();
		}
		//

         //Forward Animation....
        if (m_CurrentIndex == m_EndingIndex) 
        {
            if(m_NeedToRunReverseAnimation)
            {
                m_AnimationMode = false;
                increaseCurrentIndex();
                return;
            }
            
            m_RunningLoop++;
            if(m_RunningLoop == m_NumberOfLoop)
            {
                stopAnimation();
                //Raising Callback.
                if(m_pTarget != NULL && m_pfnSelectorDelegate != NULL)
                    (m_pTarget->*m_pfnSelectorDelegate)();
                
                //Remove Object by Itself.
                if(m_NeedToDeleteItself)
                    removeObjectItself();
                return;
            }
            else
            {				
                m_CurrentIndex = m_StartingIndex - 1;
            }
        }
        
        //Reverse Animation...
        else if(m_NeedToRunReverseAnimation && m_CurrentIndex == m_StartingIndex)
        {
            m_RunningLoop++;
            if(m_RunningLoop == m_NumberOfLoop)
            {
                stopAnimation();
                //Raising Callback.
                if(m_pTarget != 0 && m_pfnSelectorDelegate != 0)
                    (m_pTarget->*m_pfnSelectorDelegate)();
                
                //Remove Object by Itself.
                if(m_NeedToDeleteItself)
                    removeObjectItself();
                return;
            }
            else
            {
                m_AnimationMode = true;
                increaseCurrentIndex();
            }
        }
    }
}

void CCAdvancedSprite::startAnimation(int startInd, int endInd, int number_Loop, SEL_CallFunc pfnSelectorDelegate, CCObject *pTarget, int NumberOfFramesPerSecond, bool NeedToRunReverseAnimation, bool NeedToDeleteItself)
{
    if(m_isAnimationRunning)
    {
        stopAnimation();
        
        //Reset pointer.
        m_pfnSelectorDelegate = 0;
        m_pTarget = 0;
        m_ElaspeTime = 0.0f;
    }
    
    //Assign values.
    m_isAnimationRunning = true;
    
    m_StartingIndex = startInd - 1;
    m_EndingIndex = endInd - 1;
    
    m_NumberOfLoop = number_Loop;
    
    m_pfnSelectorDelegate = pfnSelectorDelegate;
    m_pTarget = pTarget;
    if(NumberOfFramesPerSecond != -1)
        m_FrameRate = calculateFrameRate(NumberOfFramesPerSecond);
    
    //Initilize values....
    m_NeedToRunReverseAnimation = NeedToRunReverseAnimation;
    m_AnimationMode = true;
    m_CurrentIndex = m_StartingIndex;
    m_RunningLoop = 0;
    m_IncrementValue = 0;
    m_NeedToDeleteItself = NeedToDeleteItself;
    increaseCurrentIndex();
	if (!m_isManualUpdate)
		this->scheduleUpdateWithPriority(-1);
    resumeAnimation();
}

void CCAdvancedSprite::stopAnimation()
{
    m_isAnimationRunning = false;

	if (!m_isManualUpdate)
		this->unscheduleUpdate();
}

void CCAdvancedSprite::removeObjectItself()
{
    this->removeFromParentAndCleanup(true);
    delete this;
}

void CCAdvancedSprite::pauseAnimation()
{ 
	_isPauseAnimation = true;
	if (!m_isManualUpdate)
		pauseSchedulerAndActions(); 
}

void CCAdvancedSprite::resumeAnimation()
{
	_isPauseAnimation = false;
	if (!m_isManualUpdate)
		resumeSchedulerAndActions(); 
}