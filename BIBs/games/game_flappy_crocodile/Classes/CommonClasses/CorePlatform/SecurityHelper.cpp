#include "SecurityHelper.h"
#include "CorePlatform/TotalTimeCounter.h"

static float _tOverall = 0; //для замера скорости DecryptXor

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

//эта версия в 7 раз быстрее, чем DecryptXor. т.к. здесь обработка идет по 4 байта
//а в DecryptXor обработка идет по одному байту
void SecurityHelper::DecryptXorFast(unsigned char* buffer, unsigned long size)
{
	if (!buffer)
		return;
        
	float tBegin = TotalTimeCounter::GetRealTime();

	unsigned char key[] = { 0xE5, 0x51, 0xE5, 0xBF };
	unsigned int keyInt = *((unsigned int*)key); //для справки - в inte обратный порядок байтов        
	unsigned long sizeInInt = size / 4; //в integerе 4 байта

	for (unsigned long i = 0; i < sizeInInt; ++i)
	{
		((unsigned int*)buffer)[i] ^= keyInt;
	}

	//обрабатываем последние байты файла
	for (unsigned long i = sizeInInt * 4; i < size; ++i)
	{
		buffer[i] = (unsigned char)(buffer[i] ^ key[i % 4]);
	}

	_tOverall += (TotalTimeCounter::GetRealTime() - tBegin);
}

