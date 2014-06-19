#pragma once
#ifndef CoroutineGenerator_h
#define CoroutineGenerator_h

#include "NonCopyable.h"
#include "CollectionHelper.h"

#define yield_return(X) \
	do \
	{ \
		Generator::m_state = __LINE__; return X; \
		case __LINE__:;\
	} while(0)

#define yield_break() \
	{ \
		Generator::m_state = 0; \
		return true; \
	} while(0)

#define GEN_BEGIN_METHOD \
	{ \
		switch (Generator::m_state) \
		{ \
			case 0:

#define GEN_BEGIN() \
	operator()(float deltaTime) \
	{ \
		switch (Generator::m_state) \
		{ \
			case 0:

#define GEN_END } return true; }

class Generator : NonCopyable
{
	protected_ int m_state;
    public_ Generator() : m_state(0) { }
	public_ virtual ~Generator() { };
};

//sample
struct IntegerGenerator : public Generator
{
	private_ int i;
    
	public_ int GEN_BEGIN()
	{
		for (i = 11; i <= 13; ++i)
			yield_return(i);

		for (i = 25; i <= 28; ++i)
			yield_return(i);

		i = 0;

        while(true)
        {
			++i;
            yield_return(i);
            
            if (i > 10)
				yield_return(0);
		}
	}
    GEN_END
};

//sample
/*IntegerGenerator x;    
while (int k = x())
{
    int a = k;
}*/

class BoolGenerator : public Generator
{
	private_ bool _isAutoDelete;
	public_ void SetIsAutoDelete(bool value) { _isAutoDelete = value; }
	public_ bool IsAutoDelete() { return _isAutoDelete; }

	protected_ BoolGenerator() : _isAutoDelete(false) {}
	public_ virtual ~BoolGenerator() { };
	public_ virtual bool operator()(float deltaTime) = 0;
};


class BoolGeneratorHolder
{
	public_ static BoolGeneratorHolder* Inst()
	{
		static BoolGeneratorHolder inst;
		return &inst;
	}

	private_ vector<BoolGenerator*> _list;
	private_ vector<BoolGenerator*> _listToRemove;

	~BoolGeneratorHolder()
	{
		ClearAll();
	}

	public_ void ClearAll()
	{
		for_each(auto generator, _list)
			delete generator;
		_list.clear();
	}
	
	//isAutoDelete = true - по окончании анимации физически удалить (delete) generator
	public_ void Add(BoolGenerator* generator, bool isAutoDelete)
	{
		generator->SetIsAutoDelete(isAutoDelete);
		_list.push_back(generator);
	}
	
	public_ void Remove(BoolGenerator* generator, bool isAutoDelete)
	{
		if (generator == NULL) 
			return;

		CollectionHelper::Remove(_list, generator);
		if (isAutoDelete)
			delete generator;
	}

	public_ bool Contains(BoolGenerator* generator)
	{
		return CollectionHelper::Contains(_list, generator);
	}

	//вызывается каждый фрейм
	public_ void Update(float delta)
	{
		if (_list.empty())
			return;

		//выполняем все генераторы
		for_each(auto generator, _list)
		{
			if ((*generator)(delta)) //если генератор выполнился, то удаляем его
				_listToRemove.push_back(generator);
		}

		if (!_listToRemove.empty())
		{
			for_each(auto generator, _listToRemove)
				Remove(generator, generator->IsAutoDelete());
			_listToRemove.clear();
		}
	}
};


//SafeDeleteAnimation позволят удалить себя из класса-парента, где находится поинтер на SafeDeleteAnimation
class SafeDeleteAnimation : public BoolGenerator
{
	private_ BoolGenerator** _pointerToAnimation;

	public_ ~SafeDeleteAnimation() 
	{ 
		(*_pointerToAnimation) = NULL;
	}

	public_ void InitPointerToAnimation(BoolGenerator** pointerToAnimation)
	{
		_pointerToAnimation = pointerToAnimation;
		(*_pointerToAnimation) = (BoolGenerator*)this;
	}
};


template<typename TChild>
static TChild* Anim_Add(TChild* animation, bool isAutoDelete, BoolGenerator** pointerToAnimation)
{
	animation->InitPointerToAnimation(pointerToAnimation);
	BoolGeneratorHolder::Inst()->Add(animation, isAutoDelete);
	return animation;
}

template<typename TChild>
static TChild* Anim_Add(TChild* animation, bool isAutoDelete, TChild** pointerToAnimation)
{
	return Anim_Add(animation, isAutoDelete, (BoolGenerator**)pointerToAnimation);
}

template<typename TChild>
static void Anim_Add_ifNotExists(TChild* animation, bool isAutoDelete)
{
	if (!BoolGeneratorHolder::Inst()->Contains(animation))
		BoolGeneratorHolder::Inst()->Add(animation, isAutoDelete);
}

static void Anim_RemoveWithDelete(BoolGenerator** animation)
{
	if (*animation != NULL)
		BoolGeneratorHolder::Inst()->Remove(*animation, true);
}

template<typename TChild>
inline static void Anim_RemoveWithDelete(TChild** animation)
{
	Anim_RemoveWithDelete((BoolGenerator**)animation);
}

#endif
