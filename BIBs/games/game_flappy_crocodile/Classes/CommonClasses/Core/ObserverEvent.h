#pragma once
#ifndef ObserverEvent_h
#define ObserverEvent_h

#include "CoreAll.h"
#include <algorithm>
#include <functional>
#include <list>
#include <map>

template<class T> //, T
char* get_unique_id() 
{
	static char dummy;
	return &dummy;
}


struct DummyFunction
{
	protected_ DummyFunction() {}
	public_ virtual ~DummyFunction() {}
};


class ObserverEventBase
{
	protected_ ObserverEventBase() {}
	public_ virtual ~ObserverEventBase() {}
	public_ virtual void DetachWithParent(void* pointer) = 0;
	public_ virtual bool IsEmpty() const = 0;
};

typedef void* NoType;  //ObserverEvent c функцией без аргументов
template<typename T> T GetDefaultType() { return T(); }
template<> NoType GetDefaultType<NoType>(); //template specialization

template<typename Arg, typename TReturnType = void>
class ObserverEvent : public ObserverEventBase
{
	// Function object base class.
	struct AbstractFunction : public DummyFunction
	{
		protected_ AbstractFunction() {}
		public_ virtual TReturnType operator()(const Arg&) = 0;
		public_ virtual ~AbstractFunction() {}
	};

	//Concrete function object classes.
	// Callback class template.
	template<typename Function>
	class Callback : public AbstractFunction 
	{
		public_ explicit Callback(Function fn) : _function(fn) {}

		public_ virtual TReturnType operator()(const Arg& arg) 
		{
			return _function(arg);
		}

		private_ Function _function;
	};

	//класс-прокладка, которая из фенкции с одним параметром передает функции без параметров
	//NOTE: надо поизучать stl - может быть, за меня уже была сделана эта прокладка
	//mem_fun, bind1st, bind2nd!!
	template<typename ClassType>
	class ZeroArgumentFunction : public AbstractFunction
	{
		typedef TReturnType (ClassType::*MemberType)();

		private_ ClassType* _pointer;
		//private_ void (ClassType::*_member)();
		private_ MemberType _member;

		public_ ZeroArgumentFunction(ClassType* pointer, MemberType member)
			: _pointer(pointer), _member(member) { }

		public_ virtual TReturnType operator()(const Arg& arg) 
		{
			return (_pointer->*_member)();
		}
	};

	#pragma region constructors and operator=

	public_ ObserverEvent() {}
	private_ ObserverEvent(const ObserverEvent& other);
	private_ const ObserverEvent& operator=(ObserverEvent& other);

	//операция переноса
	//public_ ObserverEvent(ObserverEvent& other)
	//{
	//	*this = other;
	//}

	public_ template<class Pointer>
	ObserverEvent(Pointer* pointer, TReturnType (Pointer::*member)(Arg))
	{
		AttachMember(pointer, member);
	}

	public_ template<class Pointer>
	ObserverEvent(Pointer* pointer, TReturnType (Pointer::*member)())
	{
		AttachMember(pointer, member);
	}

	public_ ObserverEvent(TReturnType (*member)(Arg))
	{
		AttachFunction(member);
	}

	//операция переноса
	//public_ const ObserverEvent& operator=(ObserverEvent& other)
	//{
	//	for_each_ref(auto& c, other._callbacks)
	//	{
	//		this->_callbacks.emplace(c);
	//	}}

	//	other._callbacks.clear();
	//	return *this;
	//}

	#pragma endregion

	// Destroy an Event.
	public_ ~ObserverEvent()
	{
		ClearObservers();
	}

	private_ void ClearObservers()
	{
		for_each_ref(auto& c, _callbacks)
		{
			delete c.second;
		}
		for_each_ref_end

		_callbacks.clear();
		_parentsToCallbacks.clear();
	}

	// Attach a simple function to an Event.
	private_ template<typename Function> 
	DummyFunction* Attach(int hash, Function fn)
	{
		auto dummyFunction = new Callback<Function>(fn);
		_callbacks[hash] = dummyFunction;
		return dummyFunction;
	}

	public_ template<typename Function> 
	DummyFunction* AttachFunction(Function fn)
	{
		int hash = GetHash(NULL, &fn);
		return Attach(hash, fn);
	}

	public_ template<typename Function> 
	void DetachFunction(Function fn)
	{
		int hash = GetHash(NULL, &fn);
		DetachMemberWithHash(hash);
	}

	private_ uint GetHash(void* pointer, void* ptrToMember) const
	{
		uint memberAddress = *((uint*)ptrToMember);
		uint result = ((uint)pointer)^memberAddress;
		return result;
	}
	
	public_ template<class Pointer>
	DummyFunction* AttachMember(Pointer* pointer, TReturnType (Pointer::*member)(Arg))
	{
		const uint hash = GetHash(pointer, &member);		
		auto dummyFunction = Attach(hash, std::bind1st(std::mem_fun(member), pointer));
		_parentsToCallbacks.push_back(ParentPair((void*)pointer, hash));
		return dummyFunction;
	}

	public_ template<class Pointer>
	DummyFunction* AttachMember(Pointer* pointer, TReturnType (Pointer::*member)())
	{
		auto dummyFunction = new ZeroArgumentFunction<Pointer>(pointer, member);
		const uint hash = GetHash(pointer, &member);		
		_callbacks[hash] = dummyFunction;
		_parentsToCallbacks.push_back(ParentPair((void*)pointer, hash));
		return dummyFunction;
	}

	public_ template<class Pointer>
	DummyFunction* AttachMember(Pointer* pointer, TReturnType (Pointer::*member)() const)
	{
		typedef TReturnType (Pointer::*NonConstFunc)();
		auto nonConstMember = (NonConstFunc)member;
		return AttachMember(pointer, nonConstMember);
	}

	private_ uint GetHashOfDummyFunction(DummyFunction* fn)
	{
		for_each_ref(auto& c, _callbacks)
		{
			if (c.second == fn)
				return c.first;
		}}

		return 0;
	}

	private_ void DetachMember(DummyFunction* fn)
	{
		if (fn == NULL)
			return;

		uint hash = GetHashOfDummyFunction(fn);
		DetachMemberWithHash(hash);
	}

	private_ void DetachMemberWithHash(uint hash)
	{
		if (hash == 0)
			return;

		auto callback = _callbacks.find(hash);
		if (callback == _callbacks.end())
			return;

		delete callback->second;
		_callbacks.erase(callback);
	}

	private_ void RemoveFromParentList(void* pointer)
	{
		CollectionHelper::RemoveIf(_parentsToCallbacks, [=](const ParentPair& p) { 
			return p.first == pointer; });
	}

	public_ template<class Pointer>
	void DetachMember(Pointer* pointer, TReturnType (Pointer::*member)(Arg))
	{
		int hash = GetHash(pointer, &member);
		DetachMemberWithHash(hash);
		RemoveFromParentList(pointer);
	}

	public_ template<class Pointer>
	void DetachMember(Pointer* pointer, TReturnType (Pointer::*member)())
	{
		int hash = GetHash(pointer, &member);
		DetachMemberWithHash(hash);
		RemoveFromParentList(pointer);
	}

	public_ template<class Pointer>
	void DetachMember(Pointer* pointer, TReturnType (Pointer::*member)() const)
	{
		int hash = GetHash(pointer, &member);
		DetachMemberWithHash(hash);
		RemoveFromParentList(pointer);
	}

	public_ void DetachWithParent(void* pointer)
	{
		for_each_ref(const auto& p, _parentsToCallbacks)
		{
			if (p.first == pointer)
				DetachMemberWithHash(p.second);
		}
		for_each_ref_end

		RemoveFromParentList(pointer);
	}

	public_ bool IsEmpty() const
	{
		return _callbacks.empty();
	}

    private_ template<typename T>
    struct NotifyTag { typedef T type; };


	private_ template<typename TNotifierReturnType>
    TNotifierReturnType NotifyInner(const Arg& arg, NotifyTag<TNotifierReturnType>) const
    {
        if (_callbacks.empty())
            return TNotifierReturnType();
    
        typedef AbstractFunction Base;
        auto function = std::bind2nd(std::mem_fun(&Base::operator()), arg);
        auto callbacksNew = _callbacks; //копируем в локальную переменную,
        //т.к. при вызове function может удалиться элемент => итератор станет некооректным
        TNotifierReturnType result;
    
        for_each_ref(auto& c, callbacksNew) {
            result = function(c.second);
        }
        for_each_ref_end
    
        return result;
    }

    private_ void NotifyInner(const Arg& arg, NotifyTag<void>) const
	{
		if (_callbacks.empty())
			return;

		typedef AbstractFunction Base;
		auto function = std::bind2nd(std::mem_fun(&Base::operator()), arg);
		auto callbacksNew = _callbacks; //копируем в локальную переменную,
										   //т.к. при вызове function может удалиться элемент => итератор станет некооректным
		for_each_ref(auto& c, callbacksNew) {
			function(c.second);
		}
		for_each_ref_end
    }

	// Notify Observers that an Event
	// has occurred.
	public_ TReturnType Notify(const Arg& arg) const
	{
		return NotifyInner(arg, NotifyTag<TReturnType>());
	}

	public_ TReturnType Notify() const
	{
		return Notify(GetDefaultType<Arg>());
	}

	// List of function objects.
	private_ std::map<uint, AbstractFunction*> _callbacks;
	private_ typedef std::pair<void*, uint> ParentPair;
	private_ std::vector<ParentPair> _parentsToCallbacks;
};


class EventLocator : NonCopyable
{
	public_ static EventLocator& Inst()
	{
		static EventLocator _inst;
		return _inst;
	}

	private_ std::map<char*, ObserverEventBase*> _mapEvents;

	private_ EventLocator() { }
	private_ ~EventLocator()
	{
		for_each_iterator(_mapEvents)
			delete it->second;
	}

	#pragma region private

	private_ template<class TEventClass>
	ObserverEventBase* GetEventBase()
	{
		auto key = get_unique_id<TEventClass>();
		auto mapValue = _mapEvents.find(key);
		if (mapValue == _mapEvents.end())
			return NULL;
		return mapValue->second;
	}

	private_ template<class TEventClass, typename Arg, typename TReturnType>
	ObserverEvent<Arg, TReturnType>* GetEvent()
	{
		return (ObserverEvent<Arg, TReturnType>*)GetEventBase<TEventClass>();
	}

	private_ template<class TEventClass, typename Arg, typename TReturnType>
	ObserverEvent<Arg, TReturnType>* GetOrCreateEvent()
	{
		auto evnt = GetEvent<TEventClass, Arg, TReturnType>();
		if (evnt == NULL)
		{
			auto key = get_unique_id<TEventClass>();
			evnt = new ObserverEvent<Arg, TReturnType>();
			_mapEvents[key] = evnt;
		}

		return evnt;
	}

	#pragma endregion 

	public_ template<class TEventClass, typename TReturnType, typename Arg, class Pointer>
	DummyFunction* RegisterCallback(Pointer* pointer, TReturnType (Pointer::*member)(Arg))
	{
		auto evnt = GetOrCreateEvent<TEventClass, Arg, TReturnType>();
		return evnt->AttachMember(pointer, member);
	}

	public_ template<class TEventClass, typename TReturnType, class Pointer>
	DummyFunction* RegisterCallback(Pointer* pointer, TReturnType (Pointer::*member)())
	{
		auto evnt = GetOrCreateEvent<TEventClass, NoType, TReturnType>();
		return evnt->AttachMember(pointer, member);
	}

	private_ template<class TEventClass>
	void UnregisterCallback(DummyFunction* function)
	{
		auto evnt = GetEventBase<TEventClass>();
		if (evnt == NULL)
			return;

		evnt->DetachMember(function);
	}

	private_ void TryRemoveFromMapEvents(char* key)
	{
		auto evnt = _mapEvents[key];
		if (!evnt->IsEmpty())
			return;

		delete evnt;
		_mapEvents.erase(key);
	}

	public_ template<class TEventClass, typename TReturnType, class Pointer>
	void UnregisterCallback(Pointer* pointer, TReturnType (Pointer::*member)())
	{
		auto evnt = GetEvent<TEventClass, NoType, TReturnType>();
		if (evnt == NULL)
			return;

		evnt->DetachMember(pointer, member);
		TryRemoveFromMapEvents(get_unique_id<TEventClass>());
	}

	public_ template<class TEventClass, typename TReturnType, typename Arg, class Pointer>
	void UnregisterCallback(Pointer* pointer, TReturnType (Pointer::*member)(Arg))
	{
		auto evnt = GetEvent<TEventClass, Arg, TReturnType>();
		if (evnt == NULL)
			return;

		evnt->DetachMember(pointer, member);
		TryRemoveFromMapEvents(get_unique_id<TEventClass>());
	}

    private_ vector<char*> _tempKeys;
    
	public_ void UnregisterAllCallbacks(void* pointer)
	{
		for (auto it = _mapEvents.begin(); it != _mapEvents.end(); ++it)
		{
			it->second->DetachWithParent(pointer);
			if (it->second->IsEmpty())
			{
                delete it->second;
                _tempKeys.push_back(it->first);
			}
		}
        
        //remove elements from _mapEvents
        while (!_tempKeys.empty())
        {
            char* key = _tempKeys.back();
            _mapEvents.erase(key);
            _tempKeys.pop_back();
        }
        
        _tempKeys.clear();
	}

	//можно назвать как publish
	public_ template<class TEventClass, typename Arg>
	void Notify(const Arg& arg)
	{
		auto evnt = GetEvent<TEventClass, Arg, void>();
		if (evnt == NULL)
			return;

		evnt->Notify(arg);
	}

	public_ template<class TEventClass>
	void Notify()
	{
		auto evnt = GetEvent<TEventClass, NoType, void>();
		if (evnt == NULL)
			return;

		evnt->Notify();
	}

	public_ template<class TEventClass, typename TReturnType, typename Arg>
	TReturnType NotifyWithReturnType(const Arg& arg)
	{
		auto evnt = GetEvent<TEventClass, Arg, TReturnType>();
		if (evnt == NULL)
			return TReturnType();

		return evnt->Notify(arg);
	}

	public_ template<class TEventClass, typename TReturnType>
	TReturnType NotifyWithReturnType()
	{
		auto evnt = GetEvent<TEventClass, NoType, TReturnType>();
		if (evnt == NULL)
			return TReturnType();

		return evnt->Notify();
	}

};

#endif
