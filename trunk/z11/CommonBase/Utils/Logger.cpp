#include "Logger.h"
#include <stdarg.h>
#include "StringUtils.h"
#    include <io.h>
#include <fcntl.h>
namespace z11
{

	void LogD(const char *szFormat, ...)
	{
		va_list pArgs ;
		va_start (pArgs, szFormat) ;
		string ret=StringUtils::vform(szFormat,pArgs);
		Category::getRoot().error(ret);
		va_end (pArgs) ;
	}
	void LogI(const char *szFormat, ...)
	{
		va_list pArgs ;
		va_start (pArgs, szFormat) ;
		string ret=StringUtils::vform(szFormat,pArgs);
		Category::getRoot().error(ret);
		va_end (pArgs) ;
	}

	/************************************************************************/
	/*                    Category                                                                        */
	/************************************************************************/
	Category& Category::getInstance( const std::string& name )
	{
		static Category category(name, NULL, Priority::INFO);
		return category;
	}

	Category& Category::getRoot()
	{
		return getInstance("");
	}

	void Category::setPriority( Priority::Value priority )
	{
		_priority=priority;
	}

	bool Category::isPriorityEnabled( Priority::Value priority ) const
	{
		return(getChainedPriority() >= priority);
	}

	Priority::Value Category::getChainedPriority() const
	{
		const Category* c = this;
		while(c->getPriority() >= Priority::NOTSET) { 
			c = c->getParent();
		}

		return c->getPriority();
	}

	Priority::Value Category::getPriority() const
	{
		 return _priority; 
	}

	const Category* Category::getParent() const
	{
		return _parent;
	}

	Category* Category::getParent()
	{
		return _parent;
	}

	Category::Category( const std::string& name, Category* parent, Priority::Value priority /*= Priority::NOTSET*/ ) 
		:_name(name),_parent(parent),_priority(priority)
	{
	}

	void Category::error( const char* stringFormat, ... )
	{
		if (isPriorityEnabled(Priority::ERROR)) {
			va_list va;
			va_start(va,stringFormat);
			_logUnconditionally(Priority::ERROR, stringFormat, va);
			va_end(va);
		}
	}

	void Category::error( const std::string& message )
	{
		if (isPriorityEnabled(Priority::ERROR))
			_logUnconditionally2(Priority::ERROR, message);
	}

	void Category::_logUnconditionally( Priority::Value priority, const char* format, va_list arguments )
	{
		 _logUnconditionally2(priority, StringUtils::vform(format, arguments));
	}

	void Category::_logUnconditionally2( Priority::Value priority, const std::string& message )
	{
		//LoggingEvent event(getName(), message, NDC::get(), priority);
		LoggingEvent event(getName(), message, "", priority);
		callAppenders(event);
	}

	void Category::addAppender(Ref<Appender>& appender )
	{
		if (!appender.empty()) 
		{
				AppenderSet::iterator i = _appender.find(appender);
				if (_appender.end() == i) {
					// not found
					_appender.insert(appender);
					//_ownsAppender[appender] = true;
				}
		} 
	}

	void Category::callAppenders( const LoggingEvent& event )
	{
		if (!_appender.empty()) {
			for(AppenderSet::const_iterator i = _appender.begin();
				i != _appender.end(); i++) {
					(*i)->doAppend(event);
			}
		}
	}

	const std::string& Category::getName() const
	{
		return _name;
	}
	LoggingEvent::LoggingEvent( const std::string& category, const std::string& message, const std::string& ndc, Priority::Value priority )
		: categoryName(categoryName),
		message(message),
		ndc(ndc),
		priority(priority),
		threadName("")
		//threadName(threading::getThreadId())
	{

	}

	FileAppender::FileAppender( const std::string& name, const std::string& fileName, bool append /*= true*/, mode_t mode /*= 00644*/ ) : 
	LayoutAppender(name),
		_fileName(fileName),
		_flags(O_CREAT | O_APPEND | O_WRONLY),
		_mode(mode)
	{
		if (!append)
			_flags |= O_TRUNC;
		_fd = ::open(_fileName.c_str(), _flags, _mode);
	}

	void FileAppender::_append( const LoggingEvent& event )
	{
		string message="FileAppender::_append";
		if (!::write(_fd, message.data(), message.length())) {
			// XXX help! help!
		}
	}

	Appender::Appender( const std::string& name ) :_name(name)
	{

	}

	LayoutAppender::LayoutAppender( const std::string& name ) :Appender(name)
	{

	}

	void LayoutAppender::doAppend( const LoggingEvent& event )
	{
		_append(event);
	}
}