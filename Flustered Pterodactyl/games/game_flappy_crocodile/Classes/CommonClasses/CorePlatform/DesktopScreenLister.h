#pragma once
#ifndef DesktopScreenLister_h
#define DesktopScreenLister_h

#include "Core/CoreAll.h"
#include "CorePlatform/CorePlatformAll.h"
using namespace cocos2d;
#include "CorePlatform/LogHelper.h"
#include "CorePlatform/SpriteHelper.h"
#include "CorePlatform/TotalTimeCounter.h"

//�������� ������, ��� �� ������� ����� � iPhone
class DesktopScreenLister
{
	private_ CCNode* _screen;   //��� view
	private_ float _segmentWidth;  //������ �������� (�� ������� ����� iPhone ��� ������ ������)
	private_ int _segmentCount;    //���������� ���. ������
	private_ float _segmentBottom;
	private_ float _segmentTop;

	private_ float _touchSpeedX; //�������� ����
	private_ CCTouch* _touch; //���� �� NULL, �� ������ ���������� ���
	private_ float _neededViewX;

	private_ int _currentSegmentNumber;
	public_ int GetCurrentSegmentNumber() 
	{ 
		return _currentSegmentNumber;
		//return GetNearestSegmentNumber(-_screen->getPosition().x); 
	}

	public_ void SetSegmentNumber(int segmentNumber)
	{
		_neededViewX = segmentNumber * _segmentWidth;
	}

	public_ DesktopScreenLister(CCNode* screen, float segmentWidth, int segmentCount, float segmentBottom, float segmentTop)
		: _touchSpeedX(), _screen(screen), _segmentWidth(segmentWidth), _segmentCount(segmentCount),
		  _segmentBottom(segmentBottom), _segmentTop(segmentTop), 
		  _currentSegmentNumber(0), _touch(), _neededViewX()
	{
	}

	public_ void TouchBegin(CCTouch* touch, const CCPoint& currentPoint)
	{
		if (!Digit(currentPoint.y).IsBetween(_segmentBottom, _segmentTop))
			return;

        if (_touch != NULL)
            return;        
		_touch = touch;
	}

	public_ void TouchMoved(CCTouch* touch, const CCPoint& prevPoint, const CCPoint& currentPoint)
	{
		if (touch != _touch)
			return;

		_touchSpeedX = currentPoint.x - prevPoint.x;
		float layerX = GetLayerX();
		//���� ��������� ����� �� ������� ���� ������, �� ������� �������� ���� � 2 ����
		if (layerX < 0 || layerX > (_segmentCount - 1)*_segmentWidth)
			_touchSpeedX = 0.5f*_touchSpeedX;

		float newLayerX = layerX - _touchSpeedX;
		_screen->setPositionX(-newLayerX);
		_currentSegmentNumber = GetNearestSegmentNumber(newLayerX, 0);
		//MY_LOG("newLayerX = " << newLayerX << "; num = " << _currentSegmentNumber);	
	}

	private_ int GetNearestSegmentNumber(float layerX, float touchSpeedX)
	{
		int leftSegmentNumber = (int)Math::TruncF(layerX/_segmentWidth);
		float leftSegmentX = leftSegmentNumber*_segmentWidth;
		float neededSegmentNumber = 0;

		if (leftSegmentNumber < 0)
			neededSegmentNumber = 0;
		else if (leftSegmentNumber >= _segmentCount - 1)
			neededSegmentNumber = (_segmentCount - 1);
		else if (Math::Abs(touchSpeedX) > 10) //���� ��������� ������, �� ���� � ������ �����
			neededSegmentNumber = touchSpeedX < 0 ? leftSegmentNumber + 1 : leftSegmentNumber; 
		else //���� �� ��������� ����� � ����� �����, �� ���� �� ����������� � ����� �����, ����� - ���� � ������ �����
			neededSegmentNumber = (layerX < leftSegmentX + _segmentWidth/2) ? leftSegmentNumber : leftSegmentNumber + 1;

		return neededSegmentNumber;
	}

	//���������� ��������� X �� �����
	private_ float GetNearestSegmentX(float layerX)
	{
		return GetNearestSegmentNumber(layerX, _touchSpeedX)*_segmentWidth;
	}

	private_ float GetLayerX()
	{
		return -_screen->getPosition().x;
	}

	public_ bool TouchEnded(CCTouch* touch)
	{
		if (_touch != touch)
			return false;

		bool isHappened = false;

		float layerX = GetLayerX();
		if ((int)layerX % (int)(_segmentWidth) != 0)
		{
			isHappened = true;
			_neededViewX = GetNearestSegmentX(layerX);
			_currentSegmentNumber = GetNearestSegmentNumber(layerX, _touchSpeedX);

			//���� ���� ������ ��������, ���������� ������ ������������
			_neededViewX += 0.25f * Math::Clamp(-_touchSpeedX, -(_segmentWidth/2 - 1), _segmentWidth/2 - 1);
		}

		_touch = NULL;
		_touchSpeedX = 0;
		return isHappened;
	}

	public_ float GetDistanceToNeededX()
	{
		return _neededViewX - GetLayerX();
	}

	public_ void UpdateViewX(ccTime deltaTime)
	{
		if (_touch != NULL)
			return;

		float layerX = GetLayerX();
		float nearestDoorX = GetNearestSegmentX(_neededViewX);
		if (Math::IsNumEquals(nearestDoorX, layerX))
			return;		

		_currentSegmentNumber = GetNearestSegmentNumber(layerX, _touchSpeedX);
		if (Math::IsNumEquals(nearestDoorX, _neededViewX) && Math::Abs(_neededViewX-layerX) < 1.1f)
		{
			_screen->setPositionX(-_neededViewX);
		}
		else
		{
			float minSpeedX = 1;

			float distancccce = Math::Abs(_neededViewX-layerX) + Math::Abs(_neededViewX-nearestDoorX);
			float speedX = TotalTimeCounter::GetDeltaValueByDeltaTime(0.1f*distancccce, deltaTime);	
			//MY_LOG("speedX = " << eedX);
			float newLayerX = Math::MoveTowards(layerX, _neededViewX, Math::Max(speedX, minSpeedX));
			int directionX = Math::Sign(newLayerX - layerX);
			if (directionX * newLayerX >= directionX * _neededViewX)
			{
				_neededViewX = nearestDoorX;
			}

			_screen->setPositionX(Math::RoundF(-newLayerX));
		}
	}
};


class PendulumUpdater
{
	private_ float _initialRotation; //���� ����� � ��������� ����� - �.�. ����������� �����������.  //���� <0, �� ����������� ������
	private_ float _speed;  //������� �������� ��������
	public_ float GetSpeed() { return _speed; }

	private_ CCSpriteWrapper* _sprite;
	private_ float _minRotationAmpletude;
	private_ float _rotationLimit; //����������� � ��������� �������� (���� == 0, �� ����������� ���)

	public_ PendulumUpdater(CCSpriteWrapper* sprite, float rotationLimit = 0) 
		: _sprite(sprite), _speed(), _minRotationAmpletude(), _initialRotation(), _rotationLimit(rotationLimit)
	{
	}

	public_ void SetInitialRotation(float initialRotation) 	
	{ 
        return; //doesn't work (
		_initialRotation = initialRotation;
		if (_rotationLimit != 0)
			_initialRotation = Math::Clamp(_initialRotation, -_rotationLimit, _rotationLimit);
	}

	//currentSpeed - ������� �������� ��������
	//return - newRotation
	private_ static float UpdatePendulum(float& currentSpeed, float currentRotation, float& minRotationAmpletude,
		float initialRotation, float rotationLimit)
	{
		if (currentSpeed == 0 && currentRotation == initialRotation)
			return initialRotation;

		const float lockFriction = 0.2f;
		const float lockAcceleration = 1;

		if (currentRotation == initialRotation)
		{
			return currentRotation + currentSpeed;
		}

		int rotationSign = Math::Sign(currentRotation - initialRotation);
		float oldSpeed = currentSpeed;
		currentSpeed = currentSpeed - rotationSign * lockAcceleration;
		float newRotation = Math::GetNormalizedAngle(currentRotation + currentSpeed);
		
        //MY_LOG("newRot = " << newRotation << "; speed = " << currentSpeed << ";initial = " << initialRotation);
        
		//���� ������ ����������� � ��������
		if (rotationLimit != 0 && Math::Abs(newRotation) > rotationLimit) 
		{
			newRotation = Math::Sign(newRotation) * rotationLimit;
			currentSpeed = 0;
		}

		if (Math::Sign(currentRotation - initialRotation) != Math::Sign(newRotation - initialRotation))
		{
			//������������ �������� ����������� � ������ �����
			currentSpeed *= (1 - lockFriction);
		}

		if (Math::Abs(newRotation - initialRotation) > minRotationAmpletude)
		{
			minRotationAmpletude *= 0.95f;
			newRotation = Math::Sign(newRotation - initialRotation) * minRotationAmpletude + initialRotation;
			currentSpeed = 0;
			//MY_LOG("SIG!");
		}

		if (Math::Sign(oldSpeed) == -Math::Sign(currentSpeed) || currentSpeed == 0)
		{
			//���� �������� �������� ����, �� ���� ������� ����� ��������� � ��������������� �������, ������ ����� ������
			if (Math::Abs(newRotation - initialRotation) < minRotationAmpletude)
			{
				//MY_LOG("new ampletude = " << newRotation << "; min = " << minRotationAmpletude);
				minRotationAmpletude = Math::Abs(newRotation - initialRotation);
			}
		}

		//MY_LOG("lockSpeed = " << currentSpeed << "; rotation = " << newRotation);
		if (Math::Abs(currentSpeed) < 1 && Math::Abs(currentRotation - initialRotation) < 1)
		{
			currentSpeed = 0;
			return initialRotation;
		}

		return newRotation;
	}

	public_ bool Update(ccTime deltaTime)
	{
		float newRotation = UpdatePendulum(_speed, _sprite->getRotation(), _minRotationAmpletude, _initialRotation, 
			_rotationLimit);
		if (newRotation != _sprite->getRotation())
		{
			_sprite->setRotation(newRotation);
			return true;
		}

		return false;
	}

	public_ bool TouchEnded(const CCPoint& currentPoint)
	{
		if (_sprite->GetRect().containsPoint(currentPoint))
		{
			//����������� �����
			_speed += (_speed == 0 ? 1 : Math::Sign(_speed))*5;
			_minRotationAmpletude = 360;
			SetSpeed(_speed);
			return true;
		}

		return false;
	}

	public_ void SetSpeed(float speed)
	{
		_speed = Math::Clamp(speed, -20.0f, 20.0f);
		_minRotationAmpletude = 360;
	}
};


#endif
