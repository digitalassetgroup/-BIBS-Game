#include "SecurityHelper.h"
#include "CorePlatform/TotalTimeCounter.h"

static float _tOverall = 0; //��� ������ �������� DecryptXor

void SecurityHelper::DecryptXor(unsigned char* buffer, unsigned long size)
{
	if (!buffer)
		return;
        
	float tBegin = TotalTimeCounter::GetRealTime();

	//unsigned char key[] = { 233, 12, 56, 129, 141 };
	unsigned char key[] = { 0xE5, 0x51, 0xE5, 0xBF };
	unsigned char keyLength = sizeof(key) / sizeof(unsigned char);
        
	for (unsigned long i = 0; i < size; ++i)
	{
		buffer[i] = (unsigned char)(buffer[i] ^ key[i % keyLength]);
	}

	_tOverall += (TotalTimeCounter::GetRealTime() - tBegin);
}

//��� ������ � 7 ��� �������, ��� DecryptXor. �.�. ����� ��������� ���� �� 4 �����
//� � DecryptXor ��������� ���� �� ������ �����
void SecurityHelper::DecryptXorFast(unsigned char* buffer, unsigned long size)
{
	if (!buffer)
		return;
        
	float tBegin = TotalTimeCounter::GetRealTime();

	unsigned char key[] = { 0xE5, 0x51, 0xE5, 0xBF };
	unsigned int keyInt = *((unsigned int*)key); //��� ������� - � inte �������� ������� ������        
	unsigned long sizeInInt = size / 4; //� integer� 4 �����

	for (unsigned long i = 0; i < sizeInInt; ++i)
	{
		((unsigned int*)buffer)[i] ^= keyInt;
	}

	//������������ ��������� ����� �����
	for (unsigned long i = sizeInInt * 4; i < size; ++i)
	{
		buffer[i] = (unsigned char)(buffer[i] ^ key[i % 4]);
	}

	_tOverall += (TotalTimeCounter::GetRealTime() - tBegin);
}

