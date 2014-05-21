#pragma once
#ifndef SecurityHelper_h
#define SecurityHelper_h

namespace SecurityHelper
{
	void DecryptXor(unsigned char* buffer, unsigned long size);
	void DecryptXorFast(unsigned char* buffer, unsigned long size);
}

#endif
