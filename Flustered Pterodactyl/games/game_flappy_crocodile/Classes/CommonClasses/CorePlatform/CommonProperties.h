#pragma once
#ifndef CommonProperties_h
#define CommonProperties_h

namespace CommonProperties
{
	bool IsNeedRetina();
	void SetIsRetina(bool isRetina);
	bool IsRetina();
	float GetScreenScale();	
	void SetScreenScale(float scaleFactor);
	float GetScaleByRetina();

	bool IsLoadHDImages();
	void SetIsLoadHDImages(bool value);	
	bool IsOrientationUpsideDown();
	void SetIsOrientationUpsideDown(bool value);
}

#endif
