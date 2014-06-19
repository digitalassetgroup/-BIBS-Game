#pragma once
#ifndef _DracoGames_ig_constants_h_
#define _DracoGames_ig_constants_h_

#include "cocos2d.h"

namespace DracoGames
{	
	namespace Constants
	{
		static const cocos2d::CCPoint OriginPoint(0.0f, 0.0f);
		
		namespace Colors
		{
			static const cocos2d::ccColor3B Black = cocos2d::ccc3(0x00, 0x00, 0x00);			
			static const cocos2d::ccColor3B White = cocos2d::ccc3(0xFF, 0xFF, 0xFF);
			
			static const cocos2d::ccColor3B PauseLabelColor = cocos2d::ccc3(0xff, 0x8c, 0x10);
			static const cocos2d::ccColor3B ButtonUpLabelColor = cocos2d::ccc3(0xFF, 0xFF, 0xFF);
			static const cocos2d::ccColor3B ButtonDownLabelColor = White;
			static const cocos2d::ccColor3B ButtonSpriteDisabled = cocos2d::ccc3(0x64, 0x64, 0x64);
			static const cocos2d::ccColor3B JadesCountColor = cocos2d::ccc3(0xfc, 0xf8, 0x10);
		};
	};
};

#endif /* _DracoGames_ig_constants_h_ */
