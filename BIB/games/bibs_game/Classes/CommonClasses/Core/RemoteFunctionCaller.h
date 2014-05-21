#ifndef RemoteFunctionCaller_h
#define RemoteFunctionCaller_h

#include "CommonMacros.h"
#include "CollectionHelper.h"
#include "QFunction.h"
#include <map>


template<typename T>
struct CallOperatorExtractor {};

template<typename TClass_, typename TReturnType_, typename TArg_>
struct CallOperatorExtractor<TReturnType_ (TClass_::*)(TArg_) const>
{
	typedef TReturnType_ TReturnType;
	typedef TArg_ TArg;
};

template<typename TClass_, typename TReturnType_>
struct CallOperatorExtractor<TReturnType_ (TClass_::*)() const>
{
	typedef TReturnType_ TReturnType;
	typedef void TArg;
};


template<typename T>
struct remove_reference_trait { typedef T type; };

template<typename T>
struct remove_reference_trait<T&> { typedef T type; };


class RemoteFunctionCaller
{
	struct QFunctionHolderBase
	{
		virtual ~QFunctionHolderBase() {}
	};

	template<typename TReturnType, typename TArg>
	struct QFunctionHolder : public QFunctionHolderBase
	{
		private_ QFunction<TReturnType(TArg)> _func;
		public_ QFunction<TReturnType(TArg)>& GetQFunction() { return _func; }
	};

    template<typename TReturnType>
	struct QFunctionHolder<TReturnType, void> : public QFunctionHolderBase
	{
		private_ QFunction<TReturnType()> _func;
		public_ QFunction<TReturnType()>& GetQFunction() { return _func; }
	};
    

	public_ static RemoteFunctionCaller& Inst()
	{
		static RemoteFunctionCaller _inst;
		return _inst;
	}

	private_ std::map<char*, QFunctionHolderBase*> _mapFromEventToFunction;


	private_ RemoteFunctionCaller() {}
	private_ ~RemoteFunctionCaller()
	{
		for_each_iterator(_mapFromEventToFunction)
			delete it->second;
	}

	private_ template<class TEventClass>
	QFunctionHolderBase* GetEventBase()
	{
		auto key = get_unique_id<TEventClass>();
		auto mapValue = _mapFromEventToFunction.find(key);
		if (mapValue == _mapFromEventToFunction.end())
			return nullptr;
		return mapValue->second;
	}

	private_ template<class TEventClass, typename TReturnType, typename Arg>
	QFunctionHolder<TReturnType, Arg>* GetEvent()
	{
		return (QFunctionHolder<TReturnType, Arg>*)GetEventBase<TEventClass>();
	}

	private_ template<class TEventClass, typename TReturnType, typename Arg>
	QFunctionHolder<TReturnType, Arg>* GetOrCreateEvent()
	{
		auto evnt = GetEvent<TEventClass, TReturnType, Arg>();
		if (evnt == nullptr)
		{
			auto key = get_unique_id<TEventClass>();
			evnt = new QFunctionHolder<TReturnType, Arg>();
			_mapFromEventToFunction[key] = evnt;
		}

		return evnt;
	}

	public_ template<class TEventClass, typename TFunction>
	void RegisterCallback(const TFunction& func)
	{
		typedef typename remove_reference_trait<decltype(func)>::type TFunctionSynonim;
		typedef CallOperatorExtractor<decltype(&TFunctionSynonim::operator())> FuncTypes;

		auto qFunction = GetOrCreateEvent<TEventClass, typename FuncTypes::TReturnType, typename FuncTypes::TArg>();
		qFunction->GetQFunction() = func;
	}

	public_ template<class TEventClass>
	void UnregisterCallback()
	{
		QFunctionHolderBase* func = GetEventBase<TEventClass>();
		if (func == nullptr)
			return;

		delete func; //destructing a void pointer will not call destructors, so QFunctionHolderBase class exist
		auto key = get_unique_id<TEventClass>();
		_mapFromEventToFunction.erase(key);
	}

	public_ template<class TEventClass, typename TReturnType, typename Arg>
	TReturnType Call(const Arg& arg)
	{
		auto func = GetEvent<TEventClass, TReturnType, Arg>();
		if (func == nullptr)
			return TReturnType();

		return (func->GetQFunction())(arg);
	}

	public_ template<class TEventClass, typename TReturnType>
	TReturnType Call()
	{
		auto func = GetEvent<TEventClass, TReturnType, void>();
		if (func == nullptr)
			return TReturnType();

		return (func->GetQFunction())();
	}
};

//

#if UNIT_TEST == 1

	static bool unittest1()
	{
		struct EventOne {};

		//
		//TwoWayMessageLocator::Inst().RegisterCallback<EventOne>([=](int x){ return 42 + x; });
		RemoteFunctionCaller::Inst().RegisterCallback<EventOne>([=](){ return 42; });
		int ss = RemoteFunctionCaller::Inst().Call<EventOne, int>();
		RemoteFunctionCaller::Inst().UnregisterCallback<EventOne>();

		//CallOperatorExtractor<decltype(&EventOne::operator())>::TArg ss11 = 3;

		auto aa = [=](int x) { return 5; };
		const auto& aa1 = aa;
		typedef remove_reference_trait<decltype(aa1)>::type AAOneSynonim;
		typedef CallOperatorExtractor<decltype(&AAOneSynonim::operator())> AAOneSynonimTypes;
		AAOneSynonimTypes::TArg ss1111 = 3;
	}
	static bool isR = unittest1();

#endif

//

#endif
