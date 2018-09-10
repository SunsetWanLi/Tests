#pragma once
#include <string>
#include <set>
#include "../Memory/MGRefObject.h"
#include "TimeStamp.h"
typedef int mode_t;
#ifdef ERROR
#undef ERROR
#endif
namespace z11
{
	class Priority
	{
	public:
		  /**
         * Predefined Levels of Priorities. These correspond to the
         * priority levels used by syslog(3).
         **/
        typedef enum {EMERG  = 0, 
		      FATAL  = 0,
                      ALERT  = 100,
                      CRIT   = 200,
                      ERROR  = 300, 
                      WARN   = 400,
                      NOTICE = 500,
                      INFO   = 600,
                      DEBUG  = 700,
                      NOTSET = 800
        } PriorityLevel;

		 /**
         * The type of Priority Values
         **/
        typedef int Value;

	};

	    /**
     * The internal representation of logging events. When a affirmative
     * logging decision is made a <code>LoggingEvent</code> instance is
     * created. This instance is passed around the different log4cpp
     * components.
     *
     * <p>This class is of concern to those wishing to extend log4cpp. 
     **/
    struct  LoggingEvent {
    public:
        /**
         * Instantiate a LoggingEvent from the supplied parameters.
         *
         * <p>Except <code>timeStamp</code> all the other fields of
         * <code>LoggingEvent</code> are filled when actually needed.
         * <p>
         * @param category The category of this event.
         * @param message  The message of this event.
         * @param ndc The nested diagnostic context of this event. 
         * @param priority The priority of this event.
         **/
        LoggingEvent(const std::string& category, const std::string& message, 
                     const std::string& ndc, Priority::Value priority);


        /** The category name. */
        const std::string categoryName;

        /** The application supplied message of logging event. */
        const std::string message;

        /** The nested diagnostic context (NDC) of logging event. */
        const std::string ndc;

        /** Priority of logging event. */
        Priority::Value priority;

        /** The name of thread in which this logging event was generated,
            e.g. the PID. 
        */
        const std::string threadName;

        /** The number of seconds elapsed since the epoch 
            (1/1/1970 00:00:00 UTC) until logging event was created. */
        TimeStamp timeStamp;
    };

	class Appender:public MGRefObject
	{
	public:
		  /**
         * Constructor for Appender. Will only be used in getAppender() (and
         * in derived classes of course).
         * @param name The name of this Appender.
         **/
        Appender(const std::string& name);

		  /**
         * Log in Appender specific way.
         * @param event  The LoggingEvent to log.
         **/
        virtual void doAppend(const LoggingEvent& event) = 0;

		virtual ~Appender()
		{

		}
	protected:
		virtual void _append(const LoggingEvent& event) = 0;

	private: 
		const std::string _name;
	};
	class LayoutAppender:public Appender
	{
	public:
		   /**
         * Log in Appender specific way. 
         * @param event  The LoggingEvent to log. 
         **/
        virtual void doAppend(const LoggingEvent& event);
		  LayoutAppender(const std::string& name);
		virtual ~LayoutAppender()
		{
		}

	};
	class ConsoleAppender:public LayoutAppender
	{

	};
	class DebuggerAppender :public LayoutAppender
	{

	};
	class FileAppender :public LayoutAppender
	{
	public:
		   /**
           Constructs a FileAppender.
           @param name the name of the Appender.
           @param fileName the name of the file to which the Appender has 
           to log.
           @param append whether the Appender has to truncate the file or
           just append to it if it already exists. Defaults to 'true'.
           @param mode file mode to open the logfile with. Defaults to 00644.
        **/  
        FileAppender(const std::string& name, const std::string& fileName,
			bool append = true, mode_t mode = 00644);
	protected:
		virtual void _append(const LoggingEvent& event);
	private:
		const std::string _fileName;
		int _fd;
		int _flags;
		mode_t _mode;
	};
	typedef std::set<Ref<Appender>> AppenderSet;
	class Category
	{
	public:
		static Category& getRoot();
		static Category& getInstance(const std::string& name);
		void setPriority(Priority::Value priority);

        /** 
         * Log a message with error priority.
         * @param stringFormat Format specifier for the string to write 
         * in the log file.
         * @param ... The arguments for stringFormat 
         **/  
        void error(const char* stringFormat, ...);

        /** 
         * Log a message with error priority.
         * @param message string to write in the log file
         **/  
        void error(const std::string& message);

		/** 
         * Returns true if the chained priority of the Category is equal to
         * or higher than given priority.
         * @param priority The priority to compare with.
         * @returns whether logging is enable for this priority.
         **/
		virtual bool isPriorityEnabled(Priority::Value priority) const;
		 
		/**
         * Starting from this Category, search the category hierarchy for a
         * set priority and return it. Otherwise, return the priority 
         *  of the root category.
         * 
         * <p>The Category class is designed so that this method executes as
         * quickly as possible.
         **/
        virtual Priority::Value getChainedPriority() const;

		/**
         * Returns the assigned Priority, if any, for this Category.
         * @return Priority - the assigned Priority, can be Priority::NOTSET
         **/
        virtual Priority::Value getPriority() const;

		  /**
         * Returns the parent category of this category, or NULL
         * if the category is the root category.
         * @return the parent category.
         **/
        virtual const Category* getParent() const;

		 /**
         * Returns the parent category of this category, or NULL
         * if the category is the root category.
         * @return the parent category.
         **/
        virtual Category* getParent();

		  /**
         * Adds an Appender to this Category.
         * This method passes ownership from the caller to the Category.
         * @since 0.2.7
         * @param appender The Appender to wich this category has to log.
         * @exception std::invalid_argument if the appender is NULL.
         **/
        virtual void addAppender(Ref<Appender>& appender);

		/**
         * Call the appenders in the hierarchy starting at
         *  <code>this</code>.  If no appenders could be found, emit a
         * warning.
         * 
         * <p>This method always calls all the appenders inherited form the
         * hierracy circumventing any evaluation of whether to log or not to
         * log the particular log request.
         * 
         * @param event the LogginEvent to log.
         **/
        virtual void callAppenders(const LoggingEvent& event);

		virtual ~Category()
		{

		}
		 /**
         * Return the category name.
         * @returns The category name.
        */       
        virtual const std::string& getName() const;
	protected:
		   /**
         * Constructor 
         * @param name the fully qualified name of this Category
         * @param parent the parent of this parent, or NULL for the root 
         * Category
         * @param priority the priority for this Category. Defaults to
         * Priority::NOTSET
         **/
        Category(const std::string& name, Category* parent, 
                                Priority::Value priority = Priority::NOTSET);

		virtual void _logUnconditionally(Priority::Value priority, 
			const char* format, 
			va_list arguments);

		 /** 
         * Unconditionally log a message with the specified priority.
         * @param priority The priority of this log message.
         * @param message string to write in the log file
         **/  
        virtual void _logUnconditionally2(Priority::Value priority, 
                                          const std::string& message);

	private:
		volatile Priority::Value _priority;
		Category*_parent;

		/* prevent copying and assignment */
		Category(const Category& other);
		Category& operator=(const Category& other);

		/** The name of this category. */
		const std::string _name;

		AppenderSet _appender;
	};
	void LogD(const char *szFormat, ...);
	void LogI(const char *szFormat, ...);
}