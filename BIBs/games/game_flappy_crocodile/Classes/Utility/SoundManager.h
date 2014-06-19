#ifndef SoundManager_h
#define SoundManager_h

#include <string>
using std::string;

class SoundManager
{
public:
	static SoundManager& Inst();
	void PlaySound(const string& soundName);
	void PlayBgMusic();
	void PlaySoundFly();	
	void PlaySoundCannon();
	void PlaySoundWin();
	void PlaySoundLoose();
	void PlaySoundBonus();
	void PlaySoundButton();	
};

#endif