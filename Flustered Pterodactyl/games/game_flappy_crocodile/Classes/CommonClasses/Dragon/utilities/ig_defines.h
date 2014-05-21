#pragma once
#ifndef _DracoGames_ig_defines_h_
#define _DracoGames_ig_defines_h_

#define IG_UNUSED(var)	(void)var

#define IG_TRANSLATE(textKey)	DracoGames::igTextTranslator::getInstance()->translate(textKey)
#define TRANSLATE(textKey)		IG_TRANSLATE(textKey)		/* This will be deprecated after Super Kid Cannon. */

#define TILT_ENABLED false
#define FLICK_ENABLED false
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Ideally pointers should be wrapped in a class for protection and their destruction usually happens in the destructor.
// This isn't really 'safe' for various reasons
template<class T> void SafeDelete(T*& pVal)
{
    if(nullptr != pVal) 
    { 
        delete pVal;
        pVal = nullptr;
    }
}

// Ideally pointers should be wrapped in a class for protection and their destruction usually happens in the destructor.
// This isn't really 'safe' for various reasons
template<class T> void SafeDeleteArray(T*& pVal)
{
    if(nullptr != pVal) 
    { 
        delete[] pVal;
        pVal = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif /* _DracoGames_ig_defines_h_ */
