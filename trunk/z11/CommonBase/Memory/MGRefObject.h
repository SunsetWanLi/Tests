//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE 
///    Copyright (c) 2011 Zhejiang University. 
///    All rights reserved.  
///  
/// @file    (MGRefObject.h)  
/// @brief (本文件实现的功能的简述)  
///  
///(本文件实现的功能的详述)  
///  
/// @version 0.1   (版本声明)  
/// @author        (Gu RongFang)  
/// @date          (11-12-05)  
///  
///  
///    修订说明：最初版本  
////////////////////////////////////////////////////////////////////////// 
#ifndef _MGREFOBJECT_H_
#define _MGREFOBJECT_H_
#include "../MGObject.h"
namespace z11
{
	class MGCoder;

	/* counting reference to reference-counted objects */
	template<typename T> class Ref {
	private:
	public:
		T *object_;
		explicit Ref(T *o = 0) :
		object_(0) {
#ifdef DEBUG_COUNTING
			cout << "instantiating Ref " << this << " from pointer" << o << "\n";
#endif
			reset(o);
		}

		explicit Ref(const T &o) :
		object_(0) {
#ifdef DEBUG_COUNTING
			cout << "instantiating Ref " << this << " from reference\n";
#endif
			reset(const_cast<T *>(&o));
		}

		Ref(const Ref &other) :
		object_(0) {
#ifdef DEBUG_COUNTING
			cout << "instantiating Ref " << this << " from Ref " << &other << "\n";
#endif
			reset(other.object_);
		}

		template<class Y>
		Ref(const Ref<Y> &other) :
		object_(0) {
#ifdef DEBUG_COUNTING
			cout << "instantiating Ref " << this << " from reference\n";
#endif

			T *cast=dynamic_cast<T *>(other.object_);
			reset(cast);
			//reset(other.object_);
		}

		~Ref() {
#ifdef DEBUG_COUNTING
			cout << "destroying Ref " << this << " with " <<
				(object_ ? typeid(*object_).name() : "NULL") << " " << object_ << "\n";
#endif
			if (object_) {
				object_->release();
			}
		}

		void reset(T *o) {
#ifdef DEBUG_COUNTING
			cout << "resetting Ref " << this << " from " <<
				(object_ ? typeid(*object_).name() : "NULL") << " " << object_ <<
				" to " << (o ? typeid(*o).name() : "NULL") << " " << o << "\n";
#endif
			if (o) {
				o->retain();
			}
			if (object_ != 0) {
				object_->release();
			}
			object_ = o;
		}
		Ref& operator=(const Ref &other) {
			reset(other.object_);
			return *this;
		}
		template<class Y>
		Ref& operator=(const Ref<Y> &other) {
			T *cast=dynamic_cast<T *>(other.object_);
			reset(cast);
			//reset(other.object_);
			return *this;
		}
		Ref& operator=(T* o) {
			reset(o);
			return *this;
		}
		template<class Y>
		Ref& operator=(Y* o) {
			reset(o);
			return *this;
		}

		T& operator*() {
			return *object_;
		}
		T* operator->() const {
			return object_;
		}
		operator T*() const {
			return object_;
		}

		bool operator==(const T* that) {
			return object_ == that;
		}
		bool operator==(const Ref &other) const {
			return object_ == other.object_ || *object_ == *(other.object_);
		}
		template<class Y>
		bool operator==(const Ref<Y> &other) const {
			return object_ == other.object_ || *object_ == *(other.object_);
		}

		bool operator<(const Ref &other) const {
			return object_ < other.object_ ;
		}

		bool operator!=(const T* that) {
			return !(*this == that);
		}

		bool empty() const {
			return object_ == 0;
		}

		//template<class Y>
		//friend std::ostream& operator<<(std::ostream &out, Ref<Y>& ref);
	};


	//the base class of reference-counted objects 
	class MGRefObject:public MGObject
	{

	protected:
		// object id
		unsigned int		m_uID;
		// count of refrence
		unsigned int		m_uReference;
		// is the object autoreleased
		bool		m_bManaged;		
		//MGMutex *locker;
	public:
		MGRefObject(void);
		virtual ~MGRefObject(void);

		virtual Ref<MGRefObject> mutableCopy();
		virtual Ref<MGRefObject> copy();
		virtual void release(void);
		virtual void retain(void);
		//MGRefObject* autorelease(void);
		//MGRefObject* copy(void);
		bool isSingleRefrence(void);
		unsigned int retainCount(void);
		virtual bool isEqual(const MGRefObject* pObject) const;
		virtual void encodeWithCoder(MGCoder *encoder);
		virtual MGRefObject* initWithCoder(const MGCoder *decoder);
		friend class MGAutoreleasePool;
	};


	typedef  Ref<MGRefObject> MGRefObjectRef;
#define NULLREF Ref<MGRefObject>(NULL)
}


#endif //_MGREFOBJECT_H_
