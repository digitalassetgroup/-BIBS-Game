#include "CommonProperties.h"
#include "LogHelper.h"

static bool _isNeedRetina = true;
bool CommonProperties::IsNeedRetina() { return _isNeedRetina; }

static float _screenScale = 1.0f;
//вернуть скейл экрана относительно картинок (1 - идентичный размер картинок)
float CommonProperties::GetScreenScale() { return _screenScale; }
void CommonProperties::SetScreenScale(float value) { _screenScale = value; }

//вернуть текущий скейл картинок (1 - hd-дисплей, 0.5 - sd-дисплей)
float CommonProperties::GetScaleByRetina() { return CommonProperties::IsRetina() ? 1.0f : 0.5f; }

static bool _isLoadHDImages = false;
//true - загружать ли принудительно hd-картинки
//true должно устанавливаться, если юзаются hd-картинки на iPad и android-устройствах
//false - если грузятся sd-картинки
bool CommonProperties::IsLoadHDImages() { return _isLoadHDImages; }
void CommonProperties::SetIsLoadHDImages(bool value) { _isLoadHDImages = value; }

static bool _isRetina = false;
//есть ли ретина - т.е. сейчас используются hd-картинки (если true), или sd-картинки (если false)
bool CommonProperties::IsRetina() {	return _isRetina; }
void CommonProperties::SetIsRetina(bool isRetina) {  _isRetina = isRetina; }

static bool _isOrientationUpsideDown = false;
bool CommonProperties::IsOrientationUpsideDown() { return _isOrientationUpsideDown; }
void CommonProperties::SetIsOrientationUpsideDown(bool value)
{
    _isOrientationUpsideDown = value;
    //MY_LOG("isUpsideDown = " << (_isOrientationUpsideDown ? 1 : 0));    
}