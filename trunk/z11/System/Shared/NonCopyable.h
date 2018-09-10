#ifndef _NON_COPYABLE_H_
#define _NON_COPYABLE_H_

//
namespace z11
{
	//;

	// Based on Dave Abrahams contribution to the Boost library.
	class NonCopyable
	{

		//! Restrict the copy constructor
		NonCopyable(const NonCopyable&);

		//! Restrict the assignment operator
		const NonCopyable& operator=(const NonCopyable&);

	protected:

		//! Create a NonCopyable object
		NonCopyable() { }

		//! Destroy a NonCopyable object
		virtual ~NonCopyable() { }

	}; // NonCopyable 


}  // namespace 
#endif
