#pragma once
#ifndef NonCopyable_h
#define NonCopyable_h

#define DEFINE_PROPERTY(type, fieldName, propertyName) \
	private: type fieldName; \
	public: type Get##propertyName() { return fieldName; } \
	public: void Set##propertyName(type value) { fieldName = value; }

#define DEFINE_PROPERTY_GET(type, fieldName, propertyName) \
	private: type fieldName; \
	public: type Get##propertyName() { return fieldName; } 


namespace noncopyable_  // protection from unintended ADL
{
	class noncopyable
	{
	protected:
		noncopyable() {}
		~noncopyable() {}
	private:  // emphasize the following members are private
		noncopyable(const noncopyable&);
		const noncopyable& operator=(const noncopyable&);
	};
}

typedef noncopyable_::noncopyable NonCopyable;

#endif
