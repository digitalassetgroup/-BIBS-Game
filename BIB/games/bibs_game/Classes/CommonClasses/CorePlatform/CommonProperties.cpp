#include "CommonProperties.h"
#include "LogHelper.h"

static bool _isNeedRetina = true;
bool CommonProperties::IsNeedRetina() { return _isNeedRetina; }

static float _screenScale = 1.0f;
//������� ����� ������ ������������ �������� (1 - ���������� ������ ��������)
float CommonProperties::GetScreenScale() { return _screenScale; }
void CommonProperties::SetScreenScale(float value) { _screenScale = value; }

//������� ������� ����� �������� (1 - hd-�������, 0.5 - sd-�������)
float CommonProperties::GetScaleByRetina() { return CommonProperties::IsRetina() ? 1.0f : 0.5f; }

static bool _isLoadHDImages = false;
//true - ��������� �� ������������� hd-��������
//true ������ ���������������, ���� ������� hd-�������� �� iPad � android-�����������
//false - ���� �������� sd-��������
bool CommonProperties::IsLoadHDImages() { return _isLoadHDImages; }
void CommonProperties::SetIsLoadHDImages(bool value) { _isLoadHDImages = value; }

static bool _isRetina = false;
//���� �� ������ - �.�. ������ ������������ hd-�������� (���� true), ��� sd-�������� (���� false)
bool CommonProperties::IsRetina() {	return _isRetina; }
void CommonProperties::SetIsRetina(bool isRetina) {  _isRetina = isRetina; }

static bool _isOrientationUpsideDown = false;
bool CommonProperties::IsOrientationUpsideDown() { return _isOrientationUpsideDown; }
void CommonProperties::SetIsOrientationUpsideDown(bool value)
{
    _isOrientationUpsideDown = value;
    //MY_LOG("isUpsideDown = " << (_isOrientationUpsideDown ? 1 : 0));    
}