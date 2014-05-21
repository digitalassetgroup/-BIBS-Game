#ifndef FlappyGameplayLayer_h
#define FlappyGameplayLayer_h

#include "CoreDragon/TouchableLayerBase.h"
#include <cocos2d.h>
#include <vector>
#include <string>
using std::vector;
using std::string;
using namespace cocos2d;

class BackgroundScroller;
class FlappyGameOverLayer;
class FlappyMainScreenLayer;
class CCLabelWithShadow;


namespace FlappyGameplayState
{
	enum State
	{
		MainMenu,
		Tutorial,
		Running,
		ScrollingStop,
		GameOverScreen
	};
}


class FlappyGameplayLayer : public TouchableLayerBase
{
private:
	BackgroundScroller* _backgroundScroller;	
	FlappyGameplayState::State _state;
	CCNode* _nodeTutorial;
	FlappyGameOverLayer* _nodeGameOver;
	FlappyMainScreenLayer* _nodeMainScreen;
	CCLabelWithShadow* _labelUpperScore;
	CCSprite* _labelUpperScoreHeart;
	int _score;

public:
	static SceneBase* GetScene();
	~FlappyGameplayLayer();
	FlappyGameplayState::State GetState() const { return _state; }
	void SetState(FlappyGameplayState::State state) { _state = state; }
	void AddOneScore();
	void Restart(bool isInMainScreen);

private:
	FlappyGameplayLayer();
	virtual void OnEnter() override;
	virtual void OnLeave() override;
	virtual void update(float dt) override;
	virtual void ccTouchesBegan(CCSet* touches, CCEvent* event) override;
	virtual void ccTouchesEnded(CCSet* touches, CCEvent* event) override;
	void AddCircle(const CCPoint& point);	
	void InitNodeTutorial(bool isFromMainScreen);
	void OnTouchBeginTutorial();
	virtual void keyBackClicked() override;
};

#endif