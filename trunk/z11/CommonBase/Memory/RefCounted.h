#pragma once
#include "../NonCopyable.h"
#include "../CommonDef.h"
#undef NDEBUG
#define NDEBUG //test
namespace z11
{
	class RefCountedBase:private NonCopyable
	{
	public:
		static bool ImplementsThreadSafeReferenceCounting() { return false; }
		bool HasOneRef() const { return ref_count_ == 1; }

	protected:
		RefCountedBase();
		~RefCountedBase();

		void AddRef() const;

		// 对象删除自己时返回true.
		bool Release() const;

	private:
		mutable int ref_count_;
#ifndef NDEBUG
		mutable bool in_dtor_;
#endif

		//DISALLOW_COPY_AND_ASSIGN(RefCountedBase);
	};

	template <class T>
	class RefCounted:public RefCountedBase
	{
	public:
		RefCounted(){}
		virtual ~RefCounted(){}
		void AddRef()
		{
			RefCountedBase::AddRef();
		}
		void Release()
		{
			if(RefCountedBase::Release())
			{
				delete static_cast<const T *>(this);
			}
		}
	};

	template<class T>
	class scoped_refptr
	{
	public:
		scoped_refptr():ptr_(NULL)
		{

		}

		scoped_refptr(T *p):ptr_(p)
		{
			if (ptr_)
			{
				ptr_->AddRef();
			}
		}

		template<typename U>
		scoped_refptr(const scoped_refptr<U>& r) : ptr_(r.get())
		{
			if(ptr_)
			{
				ptr_->AddRef();
			}
		}

		~scoped_refptr()
		{
			if (ptr_)
			{
				ptr_->Release();
			}
		}

		T* get() const { return ptr_; }
		operator T*() const { return ptr_; }
		T* operator->() const { return ptr_; }

		// 释放指针.  
		// 返回对象当前拥有的指针. 如果指针为NULL, 返回NULL.   
		// 操作完成后, 对象拥有一个NULL指针, 不再拥有任何对象.  
		T* release()
		{
			T* retVal = ptr_;
			ptr_ = NULL;
			return retVal;
		}
		
		scoped_refptr<T>& operator=(T* p)
		{
			// 先调用AddRef, 这样自我赋值也能工作.  
			if(p)
			{
				p->AddRef();
			}
			if(ptr_ )
			{
				ptr_ ->Release();
			}
			ptr_ = p;
			return *this;
		}

		scoped_refptr<T>& operator=(const scoped_refptr<T>& r)
		{
			return *this = r.ptr_;
		}
		
		template<typename U>
		scoped_refptr<T>& operator=(const scoped_refptr<U>& r)
		{
			return *this = r.get();
		}

		void swap(T** pp)
		{
			T* p = ptr_;
			ptr_ = *pp;
			*pp = p;
		}

		void swap(scoped_refptr<T>& r)
		{
			swap(&r.ptr_);
		}

	protected:
		T* ptr_;
	};
}