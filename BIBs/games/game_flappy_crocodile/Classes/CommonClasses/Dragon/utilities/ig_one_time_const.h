#pragma once
#ifndef _DracoGames_igOneTimeConst_h_
#define _DracoGames_igOneTimeConst_h_

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ig_assert.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace DracoGames
{
	/**
	 *	Templatized class for a type whose value is set once.  Usually when the value is unknown at initialization but
	 *	should behave like a constant value once the value is known.
	 *
	 *	@note the type must have a default ctor or copy ctor pending which you use during initialization.
	 *	@note the type must also have a public assignment operator, which will be called only once.
	 */
	
	template <typename T> class igOneTimeConst
	{
	public:
		igOneTimeConst(void) : m_hasBeenSet(false)
		{
		}
		
		explicit igOneTimeConst(const T& startValue) : m_value(startValue), m_hasBeenSet(false)
		{
		}
		
		igOneTimeConst(const igOneTimeConst<T>& obj) : m_value(obj.m_value), m_hasBeenSet(obj.m_hasBeenSet)
		{
		}
		
		igOneTimeConst& operator=(const igOneTimeConst<T>& obj)
		{
			obj.assertIfNotSet();
			assertIfSet();
			setValue(obj.m_value);
			return *this;
		}
		
		
		/**
		 *	Change the value within the object.  This can only be called once, and MUST be called once before any
		 *	operators can be used, or the value read from.
		 *
		 *	@warning must be called before the value is ever read and CAN NOT be called again.
		 */
		void setValue(const T& newValue)
		{
			assertIfSet();
			m_value = newValue;
			m_hasBeenSet = true;
		}
		
		const T& getValue(void)
		{
			assertIfNotSet();
			return m_value;
		}
		
		/**
		 *	Checks if the value has been changed in the lifetime of the object.
		 */
		bool hasBeenSet(void) const { return m_hasBeenSet; }
		
		/**
		 *	Checks if setValue() has been called and throws an assert if it has already been called.
		 *	@note this would mean the value cannot be set again as it has already once been set.
		 */
		void assertIfSet(void) const { assert_if(true == hasBeenSet(), "Value has already been set once, cannot be set again."); }
		
		/**
		 *	Checks if setValue() has been called and throws an assert if it has not been called.
		 *	@note this would mean the value is not readable yet as it has not been set.
		 */
		void assertIfNotSet(void) const { assert_if(false == hasBeenSet(), "Cannot read the value of this constant yet."); }		
		
		/* Operators to make the object behave like the Type it contains. */
		operator const T&() const { assertIfNotSet(); return m_value; }
				
		bool operator==(const T& rhs) const { assertIfNotSet(); return (m_value == rhs) ? true : false; }
		friend bool operator==(const T& lhs, const igOneTimeConst& rhs) { rhs.assertIfNotSet(); return (rhs.m_value == lhs) ? true : false; }
		
		bool operator!=(const T& rhs) const { assertIfNotSet(); return (m_value != rhs) ? true : false; }
		friend bool operator!=(const T& lhs, const igOneTimeConst& rhs) { rhs.assertIfNotSet(); return (rhs.m_value != lhs) ? true : false; }
		
		const T& operator->(void) const { assertIfNotSet(); return m_value; }

	private:			
		T m_value;
		bool m_hasBeenSet;
	};
		
} /* namespace DracoGames */

#endif /* _DracoGames_igOneTimeConst_h_ */
