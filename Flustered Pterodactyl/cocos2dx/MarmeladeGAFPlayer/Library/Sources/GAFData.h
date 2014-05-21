#pragma once

#ifndef __GAF_DATA_H__
#define __GAF_DATA_H__

#include "cocoagaf/GAFObject.h"


namespace GAF
{

/**
 @class GAFData
Simple NSData-like class for internal usage. Made public since it can be useful
for GAF Marmalade users. It does NOT release memory by default - be careful.
 */
	
class GAFData : public CCObject
{
public:
	inline GAFData()
	:
	_size(0),
	ptr(0),
	_delete_data(false)
	{
		
	}
	
	inline GAFData(unsigned char * _ptr, int size, bool delete_data = false)
	:
	ptr(_ptr),
	_size(size),
	_delete_data(delete_data)
	{
		
	}
	~GAFData()
	{
		if (_delete_data && ptr)
		{
			delete [] ptr;
		}
	}
	
	inline unsigned char * bytes() const
	{
		return ptr;
	}
	void setBytes(unsigned char * bytes)
	{
		ptr = bytes;
	}
	inline unsigned long size() const
	{
		return _size;
	}
	void setSize(unsigned long size)
	{
		_size = size;
	}
/// By default GAFData does NOT release memory.
/// However, it is possible to delete internal array in destructor.
/// @param deleteData - delete internal array in desctructor or not
	void setDeleteData(bool deleteData)
	{
		_delete_data = deleteData;
	}
private:
	unsigned long _size;
	unsigned char * ptr;
	bool _delete_data;
}; // GAFData
	
}  // namespace GAF


#endif // __GAF_DATA_H__
