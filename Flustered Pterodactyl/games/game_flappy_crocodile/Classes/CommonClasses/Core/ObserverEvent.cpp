#include "ObserverEvent.h"

template<> NoType GetDefaultType<NoType>() { return (NoType)NULL; } //template specialization

#if UNIT_TEST == 1
	
	static void staticFunction(int z)
	{
		int x = 6;
	}

	static bool unittest()
	{
		class Dummy_one 
		{
			public_ void Hi(float a)
			{
				float g = a + 5;
			}

			public_ void HiNoo(float a)
			{
				float g = 4;
			}

			public_ void Wassup()
			{
				float k = 4;
			}
		};
		
		class X { };
		Dummy_one d;

		ObserverEvent<float> s;
		s.AttachMember(&d, &Dummy_one::HiNoo);
		s.Notify(2.3f);
		s.DetachMember(&d, &Dummy_one::HiNoo);

		auto fun2 = EventLocator::Inst().RegisterCallback<X>(&d, &Dummy_one::Wassup);
		EventLocator::Inst().Notify<X>();

		auto fun1 = EventLocator::Inst().RegisterCallback<X>(&d, &Dummy_one::HiNoo);

		auto fun = EventLocator::Inst().RegisterCallback<Dummy_one>(&d, &Dummy_one::Hi);
		EventLocator::Inst().Notify<Dummy_one>(2.5f);
		EventLocator::Inst().Notify<X>(5);

		//EventLocator::Inst().UnregisterCallback<Dummy_one>(fun);
		EventLocator::Inst().Notify<Dummy_one>(1);

		//----copy_constructor_and_=_test
		struct A
		{
			void Do() { }
			void Do1() { }
			static void StaticDo(int z)
			{
				int x = 0;
			}
		};
		A a;

		ObserverEvent<NoType> ev1;
		ObserverEvent<NoType> ev2(&a, &A::Do);

		ev2.AttachMember(&a, &A::Do1);
		//ev1 = ev2; //move!
		//---------------------------

		//----attach and detach function---
		ObserverEvent<int> ev11 = &A::StaticDo;
		ObserverEvent<int> ev10(&staticFunction);
		ev10.AttachFunction(&A::StaticDo);
		ev10.Notify(1);
		ev10.DetachFunction(&A::StaticDo);
		ev10.Notify(2);
		ev10.DetachFunction(&staticFunction);
		//----------------------------

		//-----return type----
		class SomeReturnEvent {};
		class SomeReturnEvent2 {};

		struct ReturnTypeExample {
			int GetInt() { return 42; }
			int GetInt2(int x) { return 42.2f + x; }
		};

		ReturnTypeExample returnTypeExample;
		EventLocator::Inst().RegisterCallback<SomeReturnEvent>(&returnTypeExample, &ReturnTypeExample::GetInt);
		float result = EventLocator::Inst().NotifyWithReturnType<SomeReturnEvent, float>();

		EventLocator::Inst().RegisterCallback<SomeReturnEvent2>(&returnTypeExample, &ReturnTypeExample::GetInt2);
		float result2 = EventLocator::Inst().NotifyWithReturnType<SomeReturnEvent2, float>(5.0f);

		return true;
	}

	static bool isR = unittest();
#endif
