#include "SoundManager.h"
#include "SimpleAudioEngine.h"
#include "platform/CCPlatformConfig.h"
#include "Core/MathHelper.h"
#include "Core/StringHelper.h"
#include "platform/CCFileUtils.h"
using namespace CocosDenshion;
using namespace cocos2d;

SoundManager& SoundManager::Inst()
{
	static SoundManager inst;
	return inst;
}

static string GetSoundExtension()
{
	#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
		return ".mp3";
	#else
		return ".ogg";
	#endif
}

static string GetMusicExtension()
{
	#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
		return ".mp3";
	#else
		return ".ogg";
	#endif
}

void SoundManager::PlaySound(const string& soundName)
{
	string soundPath = "sound/" + soundName + GetSoundExtension();

	SimpleAudioEngine::sharedEngine()->playEffect(
		CCFileUtils::sharedFileUtils()->fullPathForFilename(soundPath.c_str()).c_str());
}

void SoundManager::PlayBgMusic()
{
	string musicPath = "sound/Candy Parade_131105_Loop" + GetMusicExtension();

	SimpleAudioEngine::sharedEngine()->playBackgroundMusic(
		CCFileUtils::sharedFileUtils()->fullPathForFilename(musicPath.c_str()).c_str(), true);
}

void SoundManager::PlaySoundFly()
{
	string sounds[] = { "fly0", "fly1", "fly2" };
	int soundIndex = Math::RandomRange(0, 2);
	PlaySound(sounds[soundIndex]);
}

void SoundManager::PlaySoundCannon()
{
	PlaySound("cannon");
}

void SoundManager::PlaySoundWin()
{
	PlaySound("win");
}

void SoundManager::PlaySoundLoose()
{
	PlaySound("loose");
}

void SoundManager::PlaySoundBonus()
{
	PlaySound("bonus");
}

void SoundManager::PlaySoundButton()
{
	PlaySound("button");
}
