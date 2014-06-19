#pragma once
#ifndef TextureManagerBase_h
#define TextureManagerBase_h

#include "Core/CoreAll.h"
#include "CorePlatform/CorePlatformAll.h"
#include <map>
using namespace cocos2d;
namespace cocos2d { class CCTexture2D; }
struct TextureLocalInfo;

class TextureManagerBase : NonCopyable
{
	private_ std::map<CCTexture2D**, TextureLocalInfo*> _mapTextures;
	private_ std::map<int, vector<CCTexture2D**> > _mapSceneTextures;

	protected_ void AddToMapScenes(int sceneType, CCTexture2D** texture);
	protected_ virtual void AddToMapTextures(CCTexture2D** texture, const string& path, const string& plistPath, bool noNeedHDSuffix = false, bool isAntialias = true);
	private_ void ClearMapTextures();
	private_ void UnloadTexture(CCTexture2D** texture);
	private_ CCTexture2D* LoadTextureFromMap(CCTexture2D** texture);
	protected_ virtual bool CanLoadTexture(CCTexture2D** texture) { return true; }

	protected_ bool _isMemoryWarning;
	protected_ TextureManagerBase();
	public_ virtual ~TextureManagerBase();
	public_ void HandleMemoryWarning();
	public_ virtual void LoadTexturesForScene(int sceneType);
	private_ void UnloadUnusedTexturesForFirstScene(int firstScene, int secondScene);
	private_ void UnloadUnusedTextures(int currentSceneType);
	public_ void TryUnloadUnusedTextures(int newSceneType);
	protected_ virtual void OnTryUnloadUnusedTextures(int newSceneType) = 0;
};

#endif
