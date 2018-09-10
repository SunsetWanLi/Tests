#pragma  once
template <class T>
class ResultMatcher
{
	public :
		virtual bool publish(T object)=0;
		virtual bool isCancelled()=0;
};