#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <filesystem>

#define FT_CRITICAL(...) logger << Logger::Level::CRITICAL << __VA_ARGS__ << std::endl
#define FT_ERROR(...) logger << Logger::Level::ERROR << __VA_ARGS__ << std::endl
#define FT_WARNING(...) logger << Logger::Level::WARNING << __VA_ARGS__ << std::endl
#define FT_INFO(...) logger << Logger::Level::INFO << __VA_ARGS__ << std::endl
#define FT_DEBUG(...) logger << Logger::Level::DEBUG << __VA_ARGS__ << std::endl
#define FT_TRACE(...) logger << Logger::Level::TRACE << __VA_ARGS__ << std::endl

/**
 * @brief A class that represents a file.
*/
class File: public std::ofstream
{

public:

	/**
	 * @brief Construct a new File object which opens a file.
	 * 
	 * @param path The path to the file.
	 * 
	 * @throw std::runtime_error if the file could not be opened.
	*/
	File(const std::filesystem::path & path): std::ofstream(path)
	{
		if (!is_open())
		{
			std::runtime_error("Could not open log file: " + path.string());
		}
	}

	File(File &&other): std::ofstream(std::move(other)) {}

	/**
	 * @brief Destroy the File object and close the file.
	*/
	~File()
	{
		close();
	}

};

class LogLevel
{

public:

	/**
	 * @brief The different levels of logging.
	*/
	enum class Value
	{
		CRITICAL = 0,
		ERROR = 1,
		WARNING = 2,
		INFO = 3,
		DEBUG = 4,
		TRACE = 5,
		MAX = 6
	};

	/**
	 * @brief Holds the current level of logging.
	*/
	Value value;

	/**
	 * @brief A map of the different levels of logging to their string representations.
	*/
	inline static std::string const levelToString[static_cast<uint32_t>(Value::MAX)] =
	{
		"[CRITICAL]:",
		"[ERROR]   :",
		"[WARNING] :",
		"[INFO]    :",
		"[DEBUG]   :",
		"[TRACE]   :"
	};

	/**
	 * @brief A map of the different levels of logging to their color codes.
	*/
	inline static std::string const levelToColor[static_cast<uint32_t>(Value::MAX)] =
	{
		"\033[33;41;1m",
		"\033[31m",
		"\033[33m",
		"\033[32m",
		"\033[34m",
		"\033[35m"
	};

};


// TODO: Render the logger thread-safe.


/**
 * @brief A class for logging messages to the console and to files.
*/
class Logger
{

public:

	typedef LogLevel::Value Level;


	/**
	 * @brief Construct a new Logger object.
	*/
	Logger(): m_fileInitialized(false) {}

	/**
	 * @brief Construct a new Logger object and open 5 different files for logging. If the files exist, they will be overwritten.
	 * 
	 * @param path The path to the log files.
	 * 
	 * @throw std::runtime_error if a log file could not be opened.
	*/
	Logger(const std::filesystem::path & path)
	{
		configure(path);
	}

	/**
	 * @brief Open 5 different files for logging. If the files exist, they will be overwritten.
	 * 
	 * @param path The path to the log files.
	 * 
	 * @throw std::runtime_error if a log file could not be opened.
	*/
	void configure(const std::filesystem::path & path)
	{
		m_logFiles[0] = std::make_unique<File>(path.string() + "/critical.log");
		m_logFiles[1] = std::make_unique<File>(path.string() + "/error.log");
		m_logFiles[2] = std::make_unique<File>(path.string() + "/warning.log");
		m_logFiles[3] = std::make_unique<File>(path.string() + "/info.log");
		m_logFiles[4] = std::make_unique<File>(path.string() + "/debug.log");
		m_logFiles[5] = std::make_unique<File>(path.string() + "/trace.log");
		m_fileInitialized = true;
	}

	/**
	 * @brief Set the minimum level of messages to log to the console.
	 * 
	 * @param level The level to set.
	*/
	void setLevel(Level level)
	{
		m_minConsoleLevel = level;
	}

	/**
	 * @brief Return the minimum level of messages to log to the console.
	*/
	Level level() const
	{
		return m_minConsoleLevel;
	}

	/**
	 * @brief Enable or disable timestamps.
	 * 
	 * @param enabled Whether or not to enable timestamps.
	*/
	void setTimestamp(bool enabled)
	{
		m_timestampEnabled = enabled;
	}

	/**
	 * @brief Set the message level.
	 * 
	 * @param level The level to set.
	 * 
	 * @throw std::runtime_error if a message is currently being logged.
	*/
	Logger & operator<<(Level level)
	{
		if (!m_currentMsg.str().empty())
		{
			std::runtime_error("Cannot change message level while a message is being logged. Please flush the message first with std::endl.");
		}

		m_nextMsgLevel = level;
		return *this;
	}

	/**
	 * @brief Transfer a manipulator to the stringstream buffer waiting to be flushed. If the manipulator is std::endl, the buffer will be flushed.
	 * 
	 * @param manipulator The manipulator to transfer.
	*/
	Logger & operator<<(std::ostream & (*manipulator)(std::ostream &))
	{
		if (manipulator == static_cast<std::ostream & (*)(std::ostream &)>(std::endl))
		{
			_flush();
		}
		else
		{
			m_currentMsg << manipulator;
		}

		return *this;
	}

	/**
	 * @brief Transfer a argument to the stringstream buffer waiting to be flushed.
	*/
	template <typename T>
	Logger & operator<<(T const & arg)
	{
		m_currentMsg << arg;
		return *this;
	}

private:

	bool m_fileInitialized = false;

	std::stringstream m_currentMsg;

	std::unique_ptr<File> m_logFiles[static_cast<int>(LogLevel::Value::MAX)];
	LogLevel::Value m_minConsoleLevel = LogLevel::Value::TRACE;
	LogLevel::Value m_nextMsgLevel = LogLevel::Value::INFO;

	bool m_timestampEnabled = true;


	/**
	 * @brief Flush the stringstream buffer to the console and to the appropriates log files.
	*/
	void _flush()
	{
		if (m_currentMsg.str().empty())
		{
			return;
		}
		_writeToConsole(m_currentMsg.str());
		if (m_fileInitialized)
			_writeToFile(m_currentMsg.str());
		m_currentMsg.str("");
	}

	/**
	 * @brief Return the appropriate log file for the message level.
	*/
	File & _logFile(Level level) const
	{
		return *m_logFiles[static_cast<int>(level)].get();
	}

	/**
	 * @brief Write the message level to the console if the message level is greater than or equal to the minimum console level.
	*/
	void _writeToConsole(std::string const & message)
	{
		if (m_nextMsgLevel <= m_minConsoleLevel)
		{
			std::cout << _timestamp() << _levelHeader(m_nextMsgLevel) << message << std::endl;
		}
	}

	/**
	 * @brief Write the message level to the appropriate log file.
	*/
	void _writeToFile(std::string const & message)
	{

		std::string finalMessage = _timestamp() + _levelHeader(m_nextMsgLevel, false) + message;
		switch (m_nextMsgLevel)
		{
			case LogLevel::Value::CRITICAL:
				_logFile(LogLevel::Value::CRITICAL) << finalMessage << std::endl;
			case LogLevel::Value::ERROR:
				_logFile(LogLevel::Value::ERROR) << finalMessage << std::endl;
			case LogLevel::Value::WARNING:
				_logFile(LogLevel::Value::WARNING) << finalMessage << std::endl;
			case LogLevel::Value::INFO:
				_logFile(LogLevel::Value::INFO) << finalMessage << std::endl;
			case LogLevel::Value::DEBUG:
				_logFile(LogLevel::Value::DEBUG) << finalMessage << std::endl;
			case LogLevel::Value::TRACE:
				_logFile(LogLevel::Value::TRACE) << finalMessage << std::endl;
			default:
				break;
		}
	}

	/**
	 * @brief Return the header for the message level.
	*/
	std::string _levelHeader(Level level, bool color = true)
	{
		std::stringstream ss;
		ss << (color ? LogLevel::levelToColor[static_cast<int>(level)] : "")
			<< LogLevel::levelToString[static_cast<int>(level)]
			<< (color ? "\033[0m " : " ");
		return ss.str();
	}

	/**
	 * @brief Return the current timestamp.
	*/
	std::string _timestamp()
	{
		if (!m_timestampEnabled)
		{
			return "";
		}
		std::time_t t = std::time(nullptr);
		std::tm tm = *std::localtime(&t);
		std::stringstream ss;
		ss << std::put_time(&tm, "[%Y-%m-%d %H:%M:%S] ");
		return ss.str();
	}

};

extern Logger logger;