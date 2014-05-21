#pragma once
#ifndef PropertyDefiner_h
#define PropertyDefiner_h

template<class PropertyType, class ClassType>
class PropertyDefiner
{
    typedef PropertyType (ClassType::*pgetter)() /*const*/;
    typedef void (ClassType::*psetter)(PropertyType);
	
    ClassType* _obj;
    pgetter _getter;
    psetter _setter;

public:
	PropertyDefiner() {}

    void Init(ClassType* obj, pgetter getter, psetter setter)
    {
		_obj = obj;
		_getter = getter;
		_setter = setter;
	}

    operator PropertyType() 
	{ 
		return (_obj->*_getter)(); 
	}

    PropertyType operator=(PropertyType value) 
	{
		(_obj->*_setter)(value);
		return value;
	}
};

template<class PropertyType, class ClassType>
class PropertyDefinerGetter
{
    typedef PropertyType (ClassType::*pgetter)() /*const*/;
	
    ClassType* _obj;
    pgetter _getter;

public:
	PropertyDefinerGetter() { }

	void Init(ClassType* obj, pgetter getter)
	{
		_obj = obj;
		_getter = getter;
	}
	
#ifdef _MSC_VER //для отладки в VisualStudio //_WIN32  (defined(WIN32) && defined(_WINDOWS))
	PropertyType Get() //для watch-window дебаггера
	{
		return (_obj->*_getter)();
		//return operator PropertyType();
	}

    operator PropertyType() 
	{ 
		return Get();
		//return (_obj->*_getter)();
	}
#else
    operator PropertyType() 
	{ 
		return (_obj->*_getter)();
	}
#endif
};

#endif
