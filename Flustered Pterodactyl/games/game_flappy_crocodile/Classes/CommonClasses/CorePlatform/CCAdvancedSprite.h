#pragma once
#ifndef CCAdvancedSprite_h
#define CCAdvancedSprite_h

#include "Core/CoreAll.h"
#include "CorePlatform/CorePlatformAll.h"
using namespace cocos2d; 
#include "SpriteHelper.h"

class CCAdvancedSprite : public CCSpriteWrapper
{
    CCSpriteFrame** m_AnimationFrames; //массив указателей на CCSpriteFrame
	size_t m_framesCount;
	float* _delays;

	bool _isPauseAnimation;
    //Indexing variable.
    int m_StartingIndex, m_EndingIndex, m_CurrentIndex, m_IncrementValue;    
    //Loopoing variable.
    int m_RunningLoop, m_NumberOfLoop;    
    //Frame Rate.
    float m_FrameRate;    
    //Callback funciton pointer.
    SEL_CallFunc m_pfnSelectorDelegate;
    CCObject *m_pTarget;    
    //Frame Rate Calculation variable.
    ccTime m_ElaspeTime;
	bool m_isManualUpdate;
    
    //Animation mode variable.
    bool m_NeedToRunReverseAnimation, m_AnimationMode, m_isPlistLoaded;
    bool m_isAnimationRunning;
    bool m_NeedToDeleteItself;
    //Local funciton.
    float calculateFrameRate(int number_frames){return 1.0f / number_frames;}
    void increaseCurrentIndex();
    
    void removeObjectItself();
	virtual void update(ccTime dt) { ManualUpdate(dt); };

    void setFrames(CCSpriteFrame** frames, int framesCount, int displayTextureIndex);

	CCSize _constantSize;

private:
	void SetConstantSizeInSprite();
	void ClearSpriteFrames();

public:
	void setFrames(CCSpriteFrame** frames, int framesCount);
	template<int N> void setFrames(CCSpriteFrame* (&frames)[N]) //(&frames)[N] is a reference to an array of N CCSpriteFrame*
	{
		setFrames(frames, N);
	}

	//установить кастомные врем€ задержек кадра.  оличество задержек = количество кадров
	void SetDelays(float* delays) { _delays = delays; }

	void ManualUpdate(ccTime dt);

	void SetIsManualUpdate(bool value) { m_isManualUpdate = value; }
	bool GetIsManualUpdate() { return m_isManualUpdate; }

	//размер фрейма
	void SetConstantFrameSize(const CCSize& size);
	CCSize GetConstantFrameSize() { return _constantSize; }

    //
    CCAdvancedSprite();
    ~CCAdvancedSprite();
    
    //Add Frames to run Animation.
    void setFrames(const char* pList);
    
	bool IsAnimationIsRunning() { return m_isAnimationRunning; }

    //Sets Frame Rate for running Animation
    void setFrameRate(int NumberOfFramesPerSecond){m_FrameRate = calculateFrameRate(NumberOfFramesPerSecond);}
    //void setFrameRate(float frameRate){ m_FrameRate = frameRate;}
	void SetCurrentIndex(int currentIndex) { m_CurrentIndex = currentIndex; }
	int GetCurrentIndex() { return m_CurrentIndex; }
    
    /***************************  Animation Function *************************
    
     startInd - Starting index of loaded array for running Animation
     endInd - Ending index of loaded array for running Animation
     number_Loop - Number of Loop that Animaiton to run.
     pfnSelectorDelegate - Function pointer for Notification.
     pTarget - Class pointer for calling Funciton notification.
     NumberOfFramesPerSecond - Number of Frames to run per Second.
     NeedToRunReverseAnimation - Needs to run reverse Animation.
     NeedToDeleteItself - Needs the object to delete after finish the animation.
     
    ***************************************************************************/
    void SetCurrentFrame();

    void startAnimation(int startInd, int endInd, int number_Loop, SEL_CallFunc pfnSelectorDelegate, CCObject* pTarget,int NumberOfFramesPerSecond, bool NeedToRunReverseAnimation, bool NeedToDeleteItself);
    
    //Running Animation from Current index to Parameter Index.
    void moveTo(int index){ startAnimation(m_CurrentIndex, index, 1, m_pfnSelectorDelegate, m_pTarget, -1, false, m_NeedToDeleteItself);}
    
    //Controller for Animation.
    void pauseAnimation();
    void resumeAnimation();
    void stopAnimation();
}; 

#endif
