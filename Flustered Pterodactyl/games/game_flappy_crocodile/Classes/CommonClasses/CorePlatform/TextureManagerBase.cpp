#include "TextureManagerBase.h"
#include "CorePlatform/LogHelper.h"
#include "CorePlatform/SpriteHelper.h"
#include <cocos2d.h>

void TextureManagerBase::HandleMemoryWarning()
{
	MY_LOG_WARNING_F("MEMORY WARNING!!!!!!!!!1111111111111");
	_isMemoryWarning = true;
}

#pragma region TextureMap

struct TextureLocalInfo
{
	string Path;
	string PlistPath;
	bool IsAntialias;
	bool NoNeedHDSuffix;
	CCTexture2D** Texture;
	//добавить ли такое свойство как антиал€синг?

	TextureLocalInfo(CCTexture2D** texture, const string& path, const string& plistPath, bool noNeedHDSuffix, bool isAntialias)
		: Texture(texture), Path(path), PlistPath(plistPath), NoNeedHDSuffix(noNeedHDSuffix), IsAntialias(isAntialias) { }

	void LoadTexture()
	{
		if (*Texture != NULL)
			return;

		*Texture = LoadTextureForced();
	}

	CCTexture2D* LoadTextureForced()
	{
		CCTexture2D* result = NULL;
		if (!PlistPath.empty())
			result = SpriteHelper::LoadAtlasTexture(Path.c_str(), PlistPath.c_str(), NoNeedHDSuffix);
		else
			result = SpriteHelper::LoadTexture(Path.c_str(), NoNeedHDSuffix);

		if (!IsAntialias)
			result->setAliasTexParameters();

		return result;
	}
};

void TextureManagerBase::AddToMapScenes(int sceneType, CCTexture2D** texture)
{
	_mapSceneTextures[sceneType].push_back(texture);
}

void TextureManagerBase::AddToMapTextures(CCTexture2D** texture, const string& path, const string& plistPath, bool noNeedHDSuffix, bool isAntialias) 
{
	_mapTextures[texture] = new TextureLocalInfo(texture, path, plistPath, noNeedHDSuffix, isAntialias);
}

CCTexture2D* TextureManagerBase::LoadTextureFromMap(CCTexture2D** texture)
{
	if (*texture == NULL)
	{
		_mapTextures[texture]->LoadTexture();
		MY_LOG("Load texture: " << _mapTextures[texture]->Path);
	}
	return *texture;	
}

void TextureManagerBase::ClearMapTextures()
{
	for_each_ref(auto& c, _mapTextures)
	{
		delete c.second;
	}
	for_each_ref_end

	_mapTextures.clear();
}

TextureManagerBase::TextureManagerBase() 
	: _isMemoryWarning()
{
}

TextureManagerBase::~TextureManagerBase()
{
	ClearMapTextures();
}

#pragma endregion

void TextureManagerBase::LoadTexturesForScene(int sceneType)
{
	auto textures = _mapSceneTextures[sceneType];
	for_each(CCTexture2D** texture, textures)
	{
		if (CanLoadTexture(texture))
			LoadTextureFromMap(texture);
		
		//каждый раз, когда мы загружаем новую текстуру, может быть поймано сообщение memory_warning
		TryUnloadUnusedTextures(sceneType);
	}
}

void TextureManagerBase::UnloadTexture(CCTexture2D** texture)
{
	if (*texture == NULL)
		return;
	
	CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromTexture(*texture);
	if ((*texture)->retainCount() > 1)  //
	{
		_mapTextures[texture]->LoadTextureForced();
		return;
	}
	
	MY_ASSERT((*texture)->retainCount() == 1, ""); //TODO: сделать Assert, который просто пишет в лог
	MY_LOG("Remove texture: " << _mapTextures[texture]->Path);
	CCTextureCache::sharedTextureCache()->removeTexture(*texture);  //тут вызываетс€ release, который к этому времени должен быть = 1
	*texture = NULL;
}

void TextureManagerBase::TryUnloadUnusedTextures(int newSceneType)
{
	if (_isMemoryWarning)  //comment for debug
	{
		_isMemoryWarning = false;
		OnTryUnloadUnusedTextures(newSceneType);
		UnloadUnusedTextures(newSceneType);
	}
}

//выгрузить те текстуры, которые не используютс€
void TextureManagerBase::UnloadUnusedTextures(int currentSceneType)
{
	if (currentSceneType == 0/*GameSceneType::No*/)
		return;

	for_each_ref(auto& scenePair, _mapSceneTextures)
	{
		if (scenePair.first == currentSceneType)
			continue;

		UnloadUnusedTexturesForFirstScene(scenePair.first, currentSceneType);
	}
	for_each_ref_end
}

void TextureManagerBase::UnloadUnusedTexturesForFirstScene(int firstScene, int secondScene)
{
	//return; //дл€ тестов

	//ищем те текстуры, которые нужно выгрузить - то есть те, которые используютс€ в firstScene, но Ќ≈ используютс€ в secondScene
	auto firstSceneRanges = _mapSceneTextures[firstScene];
	auto secondSceneRanges = _mapSceneTextures[secondScene];

	vector<CCTexture2D**> texturesNeedToDelete;
	for (auto it1 = firstSceneRanges.begin(); it1 != firstSceneRanges.end(); ++it1) {
		bool hasElementFromFirstSceneInSecondScene = false;  //есть ли текстура из 1-й сцены во 2-й сцене		
		for (auto it2 = secondSceneRanges.begin(); it2 != secondSceneRanges.end(); ++it2) {
			if (*it1 == *it2) { hasElementFromFirstSceneInSecondScene = true; break; }
		}
		if (!hasElementFromFirstSceneInSecondScene)
			texturesNeedToDelete.push_back(*it1);
	}

	for_each(auto t, texturesNeedToDelete)
	{
		UnloadTexture(t);
	}
}