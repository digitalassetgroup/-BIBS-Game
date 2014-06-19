#include "FlappyGameplayLayer.h"
#include "CorePlatform/SpriteHelper.h"
#include "CorePlatform/InputHelper.h"
#include "Core/MathHelper.h"
#include "Core/CollectionHelper.h"
#include "Core/StringHelper.h"
#include "CorePlatform/CCAdvancedSprite.h"
#include "Core/QFunction.h"
#include "CorePlatform/TotalTimeCounter.h"
#include "CoreDragon/NodeEffects/CCCallLambda.h"
#include "CorePlatform/NodeHelper.h"
#include "Dragon/ui/ui_button.h"
#include "Dragon/ui/i_btn_delegate.h"
#include "CoreDragon/UI/CCLabelWithShadow.h"
#include "CorePlatform/NodeScheduleLambda.h"
#include "sk_game_services/sk_game_services.h"
#include "../Utility/SoundManager.h"
#include "CoreDragon/NodeEffects/CCOpacityTo.h"
using namespace ig_interface;


static const float _groundSpeed = 240;
static const float _horizontalDistanceBetweenWalls = 250;
static const float _verticalDistanceBetweenWalls = 150;
static const float _crocodileAccelerationY = -13;


namespace CrocodileState
{
	enum State
	{
		Normal,
		KnockedToWall,
		KnockedToGround
	};
}


static CCRect GetLittleRect(const CCRect& rect, float percent)
{
	CCRect newRect = rect;
	CCSize newSize = rect.size * percent;
	newRect.size = newSize;
	newRect.origin = rect.origin + rect.size * (1 - percent) * 0.5f;
	return newRect;
}


class Crocodile
{
private:
	CCAdvancedSprite* _spriteMain;
	float _speedY;
	CrocodileState::State _state;

public:
	CrocodileState::State GetState() { return _state; }

public:
	Crocodile(CCNode* parent)
		: _spriteMain(),
		_speedY(0),
		_state(CrocodileState::Normal)
	{
		auto winSize = InputHelper::GetUnscaledWindowSize();

		auto textureCrocodile = SpriteHelper::LoadTexture("textures/crocodile/crocodile.png");
		textureCrocodile->setAliasTexParameters();

		_spriteMain = new CCAdvancedSprite();
		_spriteMain->autorelease();
		_spriteMain->initWithTexture(textureCrocodile);
		parent->addChild(_spriteMain, 10);			
		_spriteMain->setPosition(ccp(winSize.width * 0.33f, winSize.height * 0.6f));
		_spriteMain->setScale(0.75f);

		SetFlyFrames();
		UpdateEasyInOut(0);
	}	

	~Crocodile()
	{
		_spriteMain->removeFromParent();
	}	

	enum { _maxSpeedY = 6 };

	void UpdateSpriteAnimation(float dt)
	{
		_spriteMain->ManualUpdate(dt);
	}

	void UpdateEasyInOut(float dt)
	{
		auto winSize = InputHelper::GetUnscaledWindowSize();

		float percent = TotalTimeCounter::GetTotalTime() - int(TotalTimeCounter::GetTotalTime());
		if (int(TotalTimeCounter::GetTotalTime())%2 == 1)
			percent = 1 - percent;
		float y = Math::EasyInOut(winSize.height * 0.6f - 25, winSize.height * 0.6f + 25, percent, 2);
		_spriteMain->setPositionY(y);
	}

	void Update(float dt)
	{
		if (_state == CrocodileState::Normal || _state == CrocodileState::KnockedToWall)
		{
			UpdateSpriteAnimation(dt);

			_speedY += dt * _crocodileAccelerationY;
			_spriteMain->addPositionY(_speedY);

			//rotate crocodile
			const float upAngle = -35;
			const float downAngle = 90;
			float currentAngle = _spriteMain->getRotation();

			float rotationPercent = Math::InverseLerp(-11, _maxSpeedY, _speedY);

			float newAngle = Math::EasyOut(downAngle, upAngle, rotationPercent, 3);
			//float newAngle = Math::Lerp(downAngle, upAngle, rotationPercent);
			newAngle = Math::MoveTowardsAngle(currentAngle, newAngle, 15); //rotate slowly
			_spriteMain->setRotation(newAngle);
		}
		else if (_state == CrocodileState::KnockedToGround)
		{
			UpdateSpriteAnimation(dt);
		}
	}

	void OnTouchBegin()
	{
		SoundManager::Inst().PlaySoundFly();
		_speedY = _maxSpeedY;
	}

	CCRect GetBoundingBox() const
	{
		//TODO: надо юзать box2d!
		auto box = _spriteMain->boundingBox();
		box.size.height = box.size.height * 0.55f; //сверху картинки крокодильчика - пусто

		//wroong!
		//float boxAngle = int(Math::Abs(_spriteMain->getRotation())) % 45;
		//float boxPercent = Math::Lerp(0.3f, 0.7f, boxAngle / 45);

		//
		//auto resultRect = GetLittleRect(box, 1 - boxPercent);
		auto resultRect = GetLittleRect(box, 0.5f);
		return resultRect;
	}

	CCSprite* GetSprite()
	{
		return _spriteMain;
	}

	void KnockToWall()
	{
		_state = CrocodileState::KnockedToWall;
		SetFallFrames();
	}

	//крокодильчик стукнулся ровно об землю
	void KnockToGround(float maxGroundY)
	{
		if (_state == CrocodileState::KnockedToGround)
			return;

		_state = CrocodileState::KnockedToGround;
		_spriteMain->setRotation(90);
		float positionY = maxGroundY + _spriteMain->boundingBox().size.height * 0.38f;
		_spriteMain->setPositionY(positionY);
	}

private:
	void SetFlyFrames()
	{
		CCSpriteFrame* flyFrames[9] = { 0 };
		for (int i = 0; i < 9; ++i)
		{
			string textureName = StringHelper::format("textures/crocodile/anim/fly_%d.png", i + 1);			
			flyFrames[i] = SpriteHelper::GetSpriteFrameFromTextureName(textureName);
		}

		_spriteMain->setFrames(flyFrames);
		_spriteMain->startAnimation(1, 9, -1, nullptr, nullptr, 9, false, false);
	}

	void SetFallFrames()
	{
		CCSpriteFrame* fallFrames[3] = { 0 };
		for (int i = 0; i < 3; ++i)
		{
			string textureName = StringHelper::format("textures/crocodile/anim/fall_%d.png", i + 1);			
			fallFrames[i] = SpriteHelper::GetSpriteFrameFromTextureName(textureName);
		}

		_spriteMain->setFrames(fallFrames);
		_spriteMain->startAnimation(1, 3, -1, nullptr, nullptr, 9, false, false);
	}
};


class Wall
{
private:
	CCSprite* _wallUpBorder;
	CCSprite* _wallUpBody;
	CCSprite* _wallDownBorder;
	CCSprite* _wallDownBody;
	CCSprite* _allWalls[4];
	CCSprite* _spriteStar;
	float _holePositionPercentY; //позиция отверстия в трубах

public:
	float GetHolePositionPercentY() const { return _holePositionPercentY; }

public:
	~Wall()
	{
		_wallUpBorder->removeFromParent();
		_wallUpBody->removeFromParent();
		_wallDownBorder->removeFromParent();
		_wallDownBody->removeFromParent();
		SAFE_REMOVEFROMPARENT(_spriteStar);
	}

	Wall(CCNode* parent, float positionUp, float positionDown, float holePositionPercentY)
		: _wallUpBorder(),
		_wallUpBody(),
		_wallDownBorder(),
		_wallDownBody(),
		_allWalls(),
		_spriteStar()
	{
		auto winSize = InputHelper::GetUnscaledWindowSize();
		float positionX = winSize.width + 50;

		auto textureWallBorder = SpriteHelper::LoadTexture("textures/wall_border.png");
		auto textureWallBody = SpriteHelper::LoadTexture("textures/wall_body.png");
		textureWallBorder->setAliasTexParameters();
		textureWallBody->setAliasTexParameters();

		_allWalls[0] = _wallUpBody = SpriteHelper::CreateSpriteWithTexture(parent, 1, textureWallBody, ccp(0, 1));
		_allWalls[1] = _wallUpBorder = SpriteHelper::CreateSpriteWithTexture(parent, 1, textureWallBorder, ccp(0, 1));
		_allWalls[2] = _wallDownBody = SpriteHelper::CreateSpriteWithTexture(parent, 1, textureWallBody, ccp(0, 0));
		_allWalls[3] = _wallDownBorder = SpriteHelper::CreateSpriteWithTexture(parent, 1, textureWallBorder, ccp(0, 0));

		ForEachWall([=](CCSprite* wall) { wall->setPositionX(positionX); });
		ForEachWall([=](CCSprite* wall) { wall->setScale(2); wall->setScaleX(2.5f); });

		_wallUpBody->setPositionY(positionUp);
		_wallDownBody->setPositionY(positionDown);
		_wallUpBorder->setFlipY(true);

		//scale body
		_holePositionPercentY = holePositionPercentY;
		float wallSpaceHeight = (positionUp - positionDown) - _verticalDistanceBetweenWalls;		
		const float upWallHeight = _holePositionPercentY * wallSpaceHeight;
		const float downWallHeight = wallSpaceHeight - upWallHeight;

		const float upWallBodyHeight = upWallHeight - _wallUpBorder->boundingBox().size.height;
		const float downWallBodyHeight = downWallHeight - _wallDownBorder->boundingBox().size.height;

		_wallUpBody->setScaleY(upWallBodyHeight / _wallUpBody->getContentSize().height);
		_wallDownBody->setScaleY(downWallBodyHeight / _wallDownBody->getContentSize().height);
		_wallUpBorder->setPositionY(_wallUpBody->boundingBox().getMinY());
		_wallDownBorder->setPositionY(_wallDownBody->boundingBox().getMaxY());

		//
		_spriteStar = SpriteHelper::CreateSpriteWithTextureName(parent, 1, "textures/heart.png");
		_spriteStar->setScale(1.5f);
		_spriteStar->setPositionX(_wallUpBody->boundingBox().getMidX());
		_spriteStar->setPositionY((_wallUpBody->boundingBox().getMinY() + _wallDownBody->boundingBox().getMaxY()) / 2);
	}

	void Update(float dx)
	{
		ForEachWall([=](CCSprite* wall) { wall->addPositionX(dx); });

		if (_spriteStar != nullptr)
			_spriteStar->addPositionX(dx);
	}

	bool IsIntersect(const CCRect& rect)
	{
		if (_wallUpBorder->boundingBox().intersectsRect(rect))
			return true;
		if (_wallDownBorder->boundingBox().intersectsRect(rect))
			return true;
		if (_wallDownBody->boundingBox().intersectsRect(rect))
			return true;

		auto wallUpBox = _wallUpBody->boundingBox();
		wallUpBox.size.height = 5000; //если птичка вылетает за экран, то коллизимся с трубой в любом случае
		if (wallUpBox.intersectsRect(rect))
			return true;

		return false;
	}

	bool IsCanDeleteWall()
	{
		bool isCanDelete = _wallUpBorder->boundingBox().getMaxX() <= 0;
		return isCanDelete;
	}

	bool IsCrocodilePassWall(const CCRect& crocodileRect)
	{
		//old approach
		//float wallLeftX = _wallUpBody->boundingBox().getMinX();
		//return (wallLeftX < crocodileRect.getMinX());

		if (_spriteStar == nullptr)
			return false;
		
		auto box = _spriteStar->boundingBox();
		box.size.height = 150;
		box.origin.y -= 50;
		return box.intersectsRect(crocodileRect);
	}

	void RemoveStar()
	{
		SAFE_REMOVEFROMPARENT(_spriteStar);
	}

private:
	template<typename TFunc>
	void ForEachWall(const TFunc& func)
	{
		for (auto wall : _allWalls)
		{
			func(wall);
		}
	}
};


class BackgroundScroller : public CCNode
{
private:
	FlappyGameplayLayer* _parent;
	CCTexture2D* _textureBg;
	CCTexture2D* _textureGround;
	vector<CCSprite*> _spriteGrounds;
	CCSprite* _spriteGroundLast;
	vector<Wall*> _walls;
	float _lastWallDistance;
	float _lastParticleTime;
	Crocodile* _crocodile;
	vector<CCSprite*> _crocodileParticles;
	Wall* _nextWallToPass;
	float _elapsedSecondsFromDeath;
	float _elapsedSecondsFromRunningBegin;	

public:
	BackgroundScroller(FlappyGameplayLayer* parent) 
		: _parent(parent),
		_crocodile(),
		_textureBg(),
		_textureGround(),
		_spriteGroundLast(),
		_lastParticleTime(0),
		_lastWallDistance(0),
		_elapsedSecondsFromDeath(0),
		_elapsedSecondsFromRunningBegin(0),
		_nextWallToPass()
	{
	}

	~BackgroundScroller()
	{
		CC_SAFE_DELETE(_crocodile);
	}

	void Init()
	{
		auto winSize = InputHelper::GetUnscaledWindowSize();

		_textureBg = SpriteHelper::LoadTexture("textures/bg.png");
		_textureBg->setAliasTexParameters();
		_textureGround = SpriteHelper::LoadTexture("textures/ground.png");
		_textureGround->setAliasTexParameters();

		const float bgScale = 1.4f;

		//init bg
		auto bgSize = _textureBg->getContentSize();
		for (float bgPositionX = 0; bgPositionX < winSize.width; bgPositionX += bgSize.width * bgScale)
		{
			auto spriteBg = SpriteHelper::CreateSpriteWithTexture(this, -2, _textureBg, ccp(0, 1));			
			spriteBg->setPosition(ccp(bgPositionX, winSize.height));
			spriteBg->setScale(bgScale);
		}

		//init ground
		auto groundSize = _textureGround->getContentSize();
		const float groundScale = (winSize.height - bgSize.height * bgScale + 10) / groundSize.height;
		for (float bgPositionX = 0; bgPositionX < winSize.width + groundSize.width; bgPositionX += groundSize.width * groundScale)
		{
			auto spriteGround = SpriteHelper::CreateSpriteWithTexture(this, 0, _textureGround, ccp(0, 0));
			spriteGround->setPosition(ccp(bgPositionX, 0));
			_spriteGrounds.push_back(spriteGround);
			spriteGround->setScale(groundScale);
		}

		_spriteGroundLast = _spriteGrounds.back();

		//init crocodile
		 _crocodile = new Crocodile(this);
	}

	//return true, if game ends
	bool Update(float dt)
	{
		if (_parent->GetState() == FlappyGameplayState::GameOverScreen)
		{
			_crocodile->UpdateSpriteAnimation(dt);
		}
		else if (_parent->GetState() == FlappyGameplayState::Tutorial || _parent->GetState() == FlappyGameplayState::MainMenu)
		{
			const float dx = -dt * _groundSpeed;
			UpdateGround(dx);			
			_crocodile->UpdateSpriteAnimation(dt);
			_crocodile->UpdateEasyInOut(dt);
			UpdateCrocodileParticles(dt, dx);
		}
		else if (_parent->GetState() == FlappyGameplayState::Running)
		{
			_elapsedSecondsFromRunningBegin += dt;
			const float dx = -dt * _groundSpeed;

			UpdateGround(dx);
			UpdateWalls(dx);
			_crocodile->Update(dt);
			UpdateCrocodileParticles(dt, dx);

			if (_nextWallToPass != nullptr && _nextWallToPass->IsCrocodilePassWall(_crocodile->GetBoundingBox()))
			{
				SoundManager::Inst().PlaySoundBonus();
				_nextWallToPass->RemoveStar();
				int index = CollectionHelper::GetIndexOfElement(_walls, _nextWallToPass);
				_nextWallToPass = _walls[index + 1];
				_parent->AddOneScore();				
			}

			if (IsCrocodileCollisioningGround() || IsCrocodileCollisioningWalls())
			{
				_parent->SetState(FlappyGameplayState::ScrollingStop);
				_crocodile->KnockToWall();
				if (IsCrocodileCollisioningGround())
				{
					_crocodile->KnockToGround(_spriteGrounds[0]->boundingBox().getMaxY());
				}
				SoundManager::Inst().PlaySoundCannon();

				//
				auto layerColor = CCLayerColor::create(ccc4(255, 255, 255, 1));
				layerColor->setOpacity(0);
				_parent->addChild(layerColor, 100);
				layerColor->runAction(CCSequence::createWithTwoActions(
					CCEaseIn::create(CCFadeIn::create(0.04f), 3), CCEaseOut::create(CCFadeOut::create(0.2f), 3)));
				layerColor->runAction(CCCallLambda::create(0.3f, [=] { layerColor->removeFromParent(); }));
			}
		}
		else if (_parent->GetState() == FlappyGameplayState::ScrollingStop)
		{
			_crocodile->Update(dt);

			if (_crocodile->GetState() == CrocodileState::KnockedToWall)
			{
				if (IsCrocodileCollisioningGround())
				{
					SoundManager::Inst().PlaySoundCannon();
					_crocodile->KnockToGround(_spriteGrounds[0]->boundingBox().getMaxY());
					_elapsedSecondsFromDeath = 0;
				}
				else
				{
					UpdateCrocodileParticles(dt, 0);
				}
			}
			else if (_crocodile->GetState() == CrocodileState::KnockedToGround)
			{
				_elapsedSecondsFromDeath += dt;
				if (_elapsedSecondsFromDeath > 0.4f)
				{
					return true;
				}
			}
		}

		return false;
	}

	void OnTouchBeginCrocodile()
	{
		_crocodile->OnTouchBegin();
	}

private:
	void UpdateGround(float dx)
	{
		auto winSize = InputHelper::GetUnscaledWindowSize();
		auto groundSize = _textureGround->getContentSize();
		CCRect screenRect(0, 0, winSize.width, winSize.height);
		
		for (auto spriteGround : _spriteGrounds)
		{
			spriteGround->addPositionX(dx);
		}

		//replace ground to forward
		for (auto spriteGround : _spriteGrounds)
		{
			if (spriteGround->boundingBox().getMaxX() <= 0)
			{
				spriteGround->setPositionX(_spriteGroundLast->boundingBox().getMaxX() - 1);
				_spriteGroundLast = spriteGround;
				break;
			}
		}
	}

	void UpdateWalls(float dx)
	{
		if (_elapsedSecondsFromRunningBegin < 1.3f)
			return;

		float yGroundDown = _spriteGrounds[0]->boundingBox().getMaxY();
		auto winSize = InputHelper::GetUnscaledWindowSize();
		_lastWallDistance += dx;

		if (_walls.empty() || Math::Abs(_lastWallDistance) >= _horizontalDistanceBetweenWalls)
		{
			float minHoleY = 0.2f;
			float maxHoleY = 0.8f;
			if (!_walls.empty())
			{
				float prevHoleY = _walls.back()->GetHolePositionPercentY();
				const float deltaY = 0.35f;
				minHoleY = Math::Max(minHoleY, prevHoleY - deltaY);
				maxHoleY = Math::Min(maxHoleY, prevHoleY + deltaY);
			}

			auto wall = new Wall(this, winSize.height, yGroundDown, Math::RandomRangeF(minHoleY, maxHoleY));
			_walls.push_back(wall);
			_lastWallDistance = 0;

			if (_nextWallToPass == nullptr)
				_nextWallToPass = wall;
		}

		for (auto wall : _walls)
		{
			wall->Update(dx);
		}

		//delete wall
		for (auto wall : _walls)
		{
			if (wall->IsCanDeleteWall())
			{
				CollectionHelper::Remove(_walls, wall);
				CC_SAFE_DELETE(wall);
				break;
			}
		}
	}

	bool IsCrocodileCollisioningWalls() const
	{
		auto crocodileBox = _crocodile->GetBoundingBox();
		for (auto wall : _walls)
		{
			if (wall->IsIntersect(crocodileBox))
				return true;
		}

		return false;
	}

	bool IsCrocodileCollisioningGround() const
	{
		auto crocodileBox = _crocodile->GetBoundingBox();
		for (auto ground : _spriteGrounds)
		{
			if (ground->boundingBox().intersectsRect(crocodileBox))
				return true;
		}

		return false;
	}

	void UpdateCrocodileParticles(float dt, float dx)
	{
		_lastParticleTime += dt;

		if (_crocodileParticles.empty() || Math::Abs(_lastParticleTime) > 0.1f)
		{
			_lastParticleTime = 0;	
			auto sprite = SpriteHelper::CreateSpriteWithTextureName(this, 3, "textures/heart.png");
			sprite->setScale(0.5f);
			sprite->setPosition(_crocodile->GetSprite()->getPosition());
			sprite->runAction(CCFadeOut::create(1));
			_crocodileParticles.push_back(sprite);					
		}

		for (auto sprite : _crocodileParticles)
		{
			sprite->addPositionX(dx);
		}

		//delete particle
		if (_crocodileParticles[0]->boundingBox().getMaxX() <= 0)
		{
			_crocodileParticles[0]->removeFromParent();
			CollectionHelper::Remove(_crocodileParticles, _crocodileParticles[0]);
		}
	}
};



class FlappyGameOverLayer : public CCNode, public ig_interface::IBtnDelegate
{
private:
	ig_interface::Button* _butRange;
	ig_interface::Button* _butPlay;
	FlappyGameplayLayer* _parent;
	CCLabelWithShadow* _labScore;
	CCLabelWithShadow* _labMaxScore;
	CCLayerColor* _layerColor;

public:
	FlappyGameOverLayer(FlappyGameplayLayer* parent)
		: _parent(parent),
		_butRange(),
		_butPlay(),
		_labScore(),
		_labMaxScore(),
		_layerColor()
	{
	}

	void Init()
	{
		auto winSize = InputHelper::GetUnscaledWindowSize();

		auto spriteGameOver = SpriteHelper::CreateSpriteWithTextureName(this, 0, "textures/gui/game_over.png");
		spriteGameOver->getTexture()->setAliasTexParameters();
		spriteGameOver->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.69f));
		spriteGameOver->setScale(1.5f);

		auto spriteScorePanel = SpriteHelper::CreateSpriteWithTextureName(this, 0, "textures/gui/b3_1.png");
		spriteScorePanel->getTexture()->setAliasTexParameters();
		spriteScorePanel->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.55f));
		spriteScorePanel->setScale(1.5f);

		_labMaxScore = CCLabelWithShadow::create("0", "fonts/hoog0555.fnt", ccp(3, -3), kCCTextAlignmentLeft);
		addChild(_labMaxScore, 0);
		_labMaxScore->SetLabelAnchorPoint(ccp(0, 0.5f));
		_labMaxScore->setScale(0.7f);
		_labMaxScore->setColor(ccc3(128, 200, 200));
		_labMaxScore->setPosition(ccp(spriteScorePanel->getPositionX() + 25, spriteScorePanel->getPositionY() - 16));

		_labScore = CCLabelWithShadow::create("0", "fonts/hoog0555.fnt", ccp(3, -3), kCCTextAlignmentLeft);
		addChild(_labScore, 0);
		_labScore->SetLabelAnchorPoint(ccp(0, 0.5f));
		_labScore->setScale(0.7f);
		_labScore->setColor(ccc3(128, 200, 200));
		_labScore->setPosition(ccp(spriteScorePanel->getPositionX() + 25, spriteScorePanel->getPositionY() + 22));

		//spriteScorePanel
		//

		_butPlay = AddCoreButton(this, this, "textures/gui/b_play.png");
		float butX = spriteScorePanel->boundingBox().getMinX() + (_butPlay->GetUpSprite()->getContentSize().width * _butPlay->getScale())/2;
		_butPlay->setPosition(ccp(butX, winSize.height * 0.43f));

		_butRange = AddCoreButton(this, this, "textures/gui/b_rang.png");
		butX = spriteScorePanel->boundingBox().getMaxX() - (_butRange->GetUpSprite()->getContentSize().width * _butRange->getScale())/2;
		_butRange->setPosition(ccp(butX, winSize.height * 0.43f));

		//
		_layerColor = CCLayerColor::create(ccc4(255, 255, 255, 255 * 0.4f));
		addChild(_layerColor, -10);
	}

	static ig_interface::Button* AddCoreButton(CCNode* parent, ig_interface::IBtnDelegate* delegate, const string& textureName)
	{
		auto button = Button::MakeButtonWithTextures(textureName, textureName, delegate, "");
		((CCSprite*)button->GetUpSprite())->getTexture()->setAliasTexParameters();
		((CCSprite*)button->GetDownSprite())->getTexture()->setAliasTexParameters();
		button->SetScale(1.5f);
		button->SetButtonDownScale(0.95f);
		parent->addChild(button, 0);
		return button;
	}

	void SetActive(bool isActive)
	{
		if (isActive)
		{
			_butPlay->registerWithTouchDispatcher();
			_butRange->registerWithTouchDispatcher();
		}
		else
		{
			_butPlay->unregisterWithTouchDispatcher();
			_butRange->unregisterWithTouchDispatcher();
		}

		this->setVisible(isActive);
	}

	void SetScore(int score)
	{
		_labScore->SetString(StringHelper::ToString(score));

		static const char* maxScoreString = "sdkghpPPasSS";
		int maxScore = CCUserDefault::sharedUserDefault()->getIntegerForKey(maxScoreString, 0);
		if (score > maxScore)
		{
			SoundManager::Inst().PlaySoundWin();
			maxScore = score;
			CCUserDefault::sharedUserDefault()->setIntegerForKey(maxScoreString, maxScore);
			sk::game_services::submit_score(0, maxScore);
		}
		else
		{
			SoundManager::Inst().PlaySoundLoose();
		}

		_labMaxScore->SetString(StringHelper::ToString(maxScore));
	}

private:
	virtual void onClickAction(Button* const target) override
	{
		if (target == _butPlay)
		{
			_parent->Restart(false);
		}
		else if (target == _butRange)
		{
			sk::game_services::show_leaderboard(0);
		}
	}

	virtual void onTouch(Button* const target, const TouchEvent& evt) override
	{
		if (evt.phase == TouchPhase::Began)
		{
			SoundManager::Inst().PlaySoundButton();
		}
	}
};


class FlappyMainScreenLayer : public CCNode, public ig_interface::IBtnDelegate
{
private:
	FlappyGameplayLayer* _parent;
	ig_interface::Button* _butPlay;
	ig_interface::Button* _butRange;
	ig_interface::Button* _butRate;
	ig_interface::Button* _butMoreGames;
	CCSprite* _spriteTitle;

public:
	FlappyMainScreenLayer(FlappyGameplayLayer* parent)
		: _parent(parent),
		_butPlay(),
		_butRange(),
		_butRate(),
		_butMoreGames(),
		_spriteTitle()
	{
	}

	void Init()
	{
		auto winSize = InputHelper::GetUnscaledWindowSize();

		_spriteTitle = SpriteHelper::CreateSpriteWithTextureName(this, 0, "textures/gui/fp.png");
		_spriteTitle->getTexture()->setAliasTexParameters();
		_spriteTitle->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.75f));
		_spriteTitle->setScale(1.5f);

		_butRate = FlappyGameOverLayer::AddCoreButton(this, this, "textures/gui/b_rate.png");
		_butRate->setPosition(ccp(winSize.width/2, winSize.height * 0.45f));

		_butPlay = FlappyGameOverLayer::AddCoreButton(this, this, "textures/gui/b_play.png");
		_butPlay->setPosition(_butRate->getPosition() + ccp(-67, -66));

		_butRange = FlappyGameOverLayer::AddCoreButton(this, this, "textures/gui/b_rang.png");
		_butRange->setPosition(_butRate->getPosition() + ccp(67, -66));

		_butMoreGames = FlappyGameOverLayer::AddCoreButton(this, this, "textures/gui/b_mg.png");
		_butMoreGames->setPosition(_butRate->getPosition() + ccp(0, -132));
	}

	void SetActive(bool isActive)
	{
		Button* buttons[] = { _butPlay, _butRate, _butRange, _butMoreGames };
		for (auto button : buttons)
		{
			isActive ? button->registerWithTouchDispatcher() : button->unregisterWithTouchDispatcher();
		}

		CCSprite* sprites[] = { _spriteTitle, GetUpperSprite(_butPlay), GetUpperSprite(_butRate), 
			GetUpperSprite(_butRange), GetUpperSprite(_butMoreGames) };

		if (isActive)
		{
			this->setVisible(isActive);
			for (auto sprite : sprites)
			{
				sprite->setOpacity(255);
			}
		}
		else
		{
			for (auto sprite : sprites)
			{
				sprite->runAction(CCFadeOut::create(0.5f));
			}

			this->runAction(CCCallLambda::create(0.5f, [=] { this->setVisible(isActive); }));
		}		
	}

private:
	CCSprite* GetUpperSprite(Button* button)
	{
		return (CCSprite*)button->GetUpSprite();
	}

	virtual void onClickAction(Button* const target) override
	{
		if (target == _butPlay)
		{
			_parent->Restart(false);
		}
		else if (target == _butRange)
		{
			sk::game_services::show_leaderboard(0);
		}
		else if (target == _butRate)
		{
			sk::game_services::on_rate_me_pressed();
		}
		else if (target == _butMoreGames)
		{
			sk::game_services::show_more_games();
		}
	}

	virtual void onTouch(Button* const target, const TouchEvent& evt) override
	{
		if (evt.phase == TouchPhase::Began)
		{
			SoundManager::Inst().PlaySoundButton();
		}
	}
};


SceneBase* FlappyGameplayLayer::GetScene()
{
	return SceneBaseWithToucheableLayers::GetSceneWithLayer(-1, new FlappyGameplayLayer());
}

FlappyGameplayLayer::FlappyGameplayLayer()
	: _backgroundScroller(),
	_state(FlappyGameplayState::MainMenu),
	_nodeTutorial(),
	_nodeGameOver(),
	_nodeMainScreen(),
	_labelUpperScore(),
	_labelUpperScoreHeart(),
	_score()
{
}

FlappyGameplayLayer::~FlappyGameplayLayer()
{
}


void FlappyGameplayLayer::OnEnter()
{
	TouchableLayerBase::OnEnter();
	scheduleUpdate();
	
	SoundManager::Inst().PlayBgMusic();

	auto winSize = InputHelper::GetUnscaledWindowSize();

	_labelUpperScore = CCLabelWithShadow::create("0", "fonts/hoog0555.fnt", ccp(3, -3));
	_labelUpperScore->SetLabelAnchorPoint(ccp(0, 0.5f));
	addChild(_labelUpperScore, 20);
	_labelUpperScore->setPosition(ccp(winSize.width * 0.51f, winSize.height * 0.85f));
	_labelUpperScore->setVisible(false);

	_labelUpperScoreHeart = SpriteHelper::CreateSpriteWithTextureName(this, 20, "textures/heart.png");
	_labelUpperScoreHeart->setVisible(false);
	_labelUpperScoreHeart->setScale(1.5f);
	_labelUpperScoreHeart->setPosition(ccp(winSize.width * 0.46f, winSize.height * 0.845f));

	//
	_nodeGameOver = SpriteHelper::AddNodeAndRelease(this, 21, new FlappyGameOverLayer(this));
	_nodeGameOver->Init();
	_nodeGameOver->SetActive(false);

	//
	_nodeMainScreen = SpriteHelper::AddNodeAndRelease(this, 50, new FlappyMainScreenLayer(this));
	_nodeMainScreen->Init();
	_nodeMainScreen->SetActive(true);
	//

	Restart(true);
}

void FlappyGameplayLayer::Restart(bool isInMainScreen)
{
	//reinit backgroundScroller
	if (isInMainScreen || _state != FlappyGameplayState::MainMenu)
	{
		if (_backgroundScroller != nullptr)
		{
			_backgroundScroller->removeFromParent();
			_backgroundScroller = nullptr;
		}

		_backgroundScroller = new BackgroundScroller(this);
		_backgroundScroller->autorelease();
		_backgroundScroller->Init();
		addChild(_backgroundScroller);
	}

	if (isInMainScreen)
	{
		SetState(FlappyGameplayState::MainMenu);
	}
	else
	{
		_nodeMainScreen->SetActive(false);

		InitNodeTutorial(_state == FlappyGameplayState::MainMenu);
		SetState(FlappyGameplayState::Tutorial);		

		_score = -1;
		AddOneScore();

		_nodeGameOver->SetActive(false);		
	}
}

void FlappyGameplayLayer::OnLeave()
{
	unscheduleUpdate();
	TouchableLayerBase::OnLeave();
}

void FlappyGameplayLayer::update(float dt)
{
	if (_state == FlappyGameplayState::MainMenu
		|| _state == FlappyGameplayState::Tutorial 
		|| _state == FlappyGameplayState::Running
		|| _state == FlappyGameplayState::ScrollingStop)
	{
		bool isGameEnded = _backgroundScroller->Update(dt);
		if (isGameEnded)
		{
			SetState(FlappyGameplayState::GameOverScreen);
			_nodeGameOver->SetScore(_score);
			_nodeGameOver->SetActive(true);
			_labelUpperScore->setVisible(false);
			_labelUpperScoreHeart->setVisible(false);
		}
	}
	else if (_state == FlappyGameplayState::GameOverScreen)
	{
		_backgroundScroller->Update(dt);
	}
}

void FlappyGameplayLayer::ccTouchesBegan(CCSet* touches, CCEvent* event)
{
	TouchableLayerBase::ccTouchesBegan(touches, event);

	auto point = static_cast<CCTouch*>(touches->anyObject())->getLocation();

	switch (_state)
	{
		case FlappyGameplayState::Tutorial:
			OnTouchBeginTutorial();
			break;
		case FlappyGameplayState::Running:
			_backgroundScroller->OnTouchBeginCrocodile();
			break;
	}
}

void FlappyGameplayLayer::OnTouchBeginTutorial()
{
	_state = FlappyGameplayState::Running;
	_backgroundScroller->OnTouchBeginCrocodile();

	CCArray_ForEach(CCNode*, child, _nodeTutorial->getChildren())
	{
		child->stopAllActions();
		float time = ((CCSprite*)child)->getOpacity() * 1.0f / 255.0f;
		child->runAction(CCFadeTo::create(time, 0));
	}
	CCArray_ForEachEnd()
			
	_nodeTutorial->runAction(CCCallLambda::create(1, [=]{ SAFE_REMOVEFROMPARENT(_nodeTutorial); }));
}

void FlappyGameplayLayer::ccTouchesEnded(CCSet* touches, CCEvent* event)
{
	TouchableLayerBase::ccTouchesEnded(touches, event);

	auto point = static_cast<CCTouch*>(touches->anyObject())->getLocation();
}

void FlappyGameplayLayer::InitNodeTutorial(bool isFromMainScreen)
{
	auto winSize = InputHelper::GetUnscaledWindowSize();

	_nodeTutorial = SpriteHelper::CreateEmptyNode(this, 20);		
	auto spriteGetReady = SpriteHelper::CreateSpriteWithTextureName(_nodeTutorial, 0, "textures/gui/get_ready.png");
	spriteGetReady->getTexture()->setAliasTexParameters();
	auto spriteTap = SpriteHelper::CreateSpriteWithTextureName(_nodeTutorial, 0, "textures/gui/instruction_tap.png");
	spriteTap->getTexture()->setAliasTexParameters();
	spriteGetReady->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.75f));
	spriteTap->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.41f));

	spriteGetReady->setScale(1.5f);
	spriteTap->setScale(1.5f);

	spriteGetReady->setOpacity(0);
	spriteTap->setOpacity(0);
	_labelUpperScore->setVisible(true);
	_labelUpperScoreHeart->setVisible(true);
	_labelUpperScore->setOpacity(0);
	_labelUpperScoreHeart->setOpacity(0);

	auto func = [=]
	{
		_labelUpperScore->runAction(CCEaseOut::create(CCFadeIn::create(0.15f), 5));
		_labelUpperScoreHeart->runAction(CCOpacityTo::create(1, 180));
		spriteGetReady->runAction(CCFadeIn::create(1));
		spriteTap->runAction(CCFadeIn::create(1));
	};

	if (isFromMainScreen)
	{
		NodeScheduleLambda(_nodeTutorial, 1, func);
	}
	else
	{
		func();
	}
}

void FlappyGameplayLayer::AddOneScore()
{
	_score += 1;
	string strScore = StringHelper::ToString(_score);
	_labelUpperScore->SetString(strScore);
}

static void ExitFromApp()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_WP8
    CCDirector::sharedDirector()->end();
#else
    exit(0);
#endif
}

void FlappyGameplayLayer::keyBackClicked()
{
	ExitFromApp();
}