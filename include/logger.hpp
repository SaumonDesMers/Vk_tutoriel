#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <map>
#include <filesystem>

/**
 * @brief A class that represents a file.
*/
class File: public std::ofstream {

public:

	/**
	 * @brief Construct a new File object which opens a file.
	 * 
	 * @param path The path to the file.
	 * 
	 * @throw std::runtime_error if the file could not be opened.
	*/
	File(const std::filesystem::path & p_path): std::ofstream(p_path) {
		if (!is_open()) {
			throw std::runtime_error("Could not open log file: " + p_path.string());
		}
	}

	File(File &&p_other): std::ofstream(std::move(p_other)) {}

	/**
	 * @brief Destroy the File object and close the file.
	*/
	~File() {
		close();
	}

};

class LogLevel {

public:

	/**
	 * @brief The different levels of logging.
	*/
	enum class Value {
		DEBUG = 0,
		INFO,
		WARNING,
		ERROR,
		CRITICAL
	};

	/**
	 * @brief Holds the current level of logging.
	*/
	Value value;

	/**
	 * @brief A map of the different levels of logging to their string representations.
	*/
	inline static std::string const levelToString[5] = {
		"DEBUG   :",
		"INFO    :",
		"WARNING :",
		"ERROR   :",
		"CRITICAL:"
	};

	/**
	 * @brief A map of the different levels of logging to their color codes.
	*/
	inline static std::string const levelToColor[5] = {
		"\x1b[34m",
		"\x1b[32m",
		"\x1b[33m",
		"\x1b[31m",
		"\x1b[33;41;1;5m"
	};

};


// TODO: Render the logger thread-safe.


/**
 * @brief A class for logging messages to the console and to files.
*/
class Logger {

public:

	typedef LogLevel::Value Level;

private:

	bool _fileInitialized = false;

	std::stringstream _currentMsg;

	std::unique_ptr<File> _logFiles[5];
	LogLevel::Value _minConsoleLevel = LogLevel::Value::DEBUG;
	LogLevel::Value _nextMsgLevel = LogLevel::Value::DEBUG;

	bool _timestampEnabled = true;


	/**
	 * @brief Flush the stringstream buffer to the console and to the appropriates log files.
	*/
	void _flush() {

		if (_currentMsg.str().empty()) {
			return;
		}
		_writeToConsole(_currentMsg.str());
		if (_fileInitialized)
			_writeToFile(_currentMsg.str());
		_currentMsg.str("");
	}

	/**
	 * @brief Return the appropriate log file for the message level.
	*/
	File & _logFile(Level p_level) const {
		return *_logFiles[static_cast<int>(p_level)].get();
	}

	/**
	 * @brief Write the message level to the console if the message level is greater than or equal to the minimum console level.
	*/
	void _writeToConsole(std::string const & p_message) {
		if (_nextMsgLevel >= _minConsoleLevel) {
			std::cout << _timestamp() << _levelHeader(_nextMsgLevel) << p_message << std::endl;
		}
	}

	/**
	 * @brief Write the message level to the appropriate log file.
	*/
	void _writeToFile(std::string const & p_message) {

		std::string finalMessage = _timestamp() + _levelHeader(_nextMsgLevel, false) + p_message;
		switch (_nextMsgLevel) {
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
		}
	}

	/**
	 * @brief Return the header for the message level.
	*/
	std::string _levelHeader(Level p_level, bool p_color = true) {
		std::stringstream ss;
		ss << (p_color ? LogLevel::levelToColor[static_cast<int>(p_level)] : "")
			<< LogLevel::levelToString[static_cast<int>(p_level)]
			<< (p_color ? "\x1b[0m " : " ");
		return ss.str();
	}

	/**
	 * @brief Return the current timestamp.
	*/
	std::string _timestamp() {
		if (!_timestampEnabled) {
			return "";
		}
		std::time_t t = std::time(nullptr);
		std::tm tm = *std::localtime(&t);
		std::stringstream ss;
		ss << std::put_time(&tm, "[%Y-%m-%d %H:%M:%S] ");
		return ss.str();
	}

	/**
	 * @brief Check if the logger is initialized.
	 * 
	 * @throw std::runtime_error if the logger is not initialized.
	*/
	void _checkIfInitialized() {
		if (!_fileInitialized) {
			throw std::runtime_error("Logger not initialized. Call Logger::configureFile() before using the logger.");
		}
	}

public:

	/**
	 * @brief Construct a new Logger object.
	*/
	Logger(): _fileInitialized(false) {}

	/**
	 * @brief Construct a new Logger object and open 5 different files for logging. If the files exist, they will be overwritten.
	 * 
	 * @param p_path The path to the log files.
	 * 
	 * @throw std::runtime_error if a log file could not be opened.
	*/
	Logger(const std::filesystem::path & p_path) {
		configureFile(p_path);
	}

	/**
	 * @brief Open 5 different files for logging. If the files exist, they will be overwritten.
	 * 
	 * @param p_path The path to the log files.
	 * 
	 * @throw std::runtime_error if a log file could not be opened.
	*/
	void configureFile(const std::filesystem::path & p_path) {
		_logFiles[0] = std::make_unique<File>(p_path.string() + "/debug.log");
		_logFiles[1] = std::make_unique<File>(p_path.string() + "/info.log");
		_logFiles[2] = std::make_unique<File>(p_path.string() + "/warning.log");
		_logFiles[3] = std::make_unique<File>(p_path.string() + "/error.log");
		_logFiles[4] = std::make_unique<File>(p_path.string() + "/critical.log");
		_fileInitialized = true;
	}

	/**
	 * @brief Set the minimum level of messages to log to the console.
	 * 
	 * @param p_level The level to set.
	*/
	void setLevel(Level p_level) {
		_minConsoleLevel = p_level;
	}

	/**
	 * @brief Return the minimum level of messages to log to the console.
	*/
	Level level() const {
		return _minConsoleLevel;
	}

	/**
	 * @brief Enable or disable timestamps.
	 * 
	 * @param p_enabled Whether or not to enable timestamps.
	*/
	void setTimestamp(bool p_enabled) {
		_timestampEnabled = p_enabled;
	}

	/**
	 * @brief Set the message level.
	 * 
	 * @param p_level The level to set.
	 * 
	 * @throw std::runtime_error if a message is currently being logged.
	*/
	Logger & operator<<(Level p_level) {

		if (!_currentMsg.str().empty()) {
			throw std::runtime_error("Cannot change message level while a message is being logged. Please flush the message first with std::endl.");
		}

		_nextMsgLevel = p_level;
		return *this;
	}

	/**
	 * @brief Transfer a manipulator to the stringstream buffer waiting to be flushed. If the manipulator is std::endl, the buffer will be flushed.
	 * 
	 * @param p_manipulator The manipulator to transfer.
	*/
	Logger & operator<<(std::ostream & (*p_manipulator)(std::ostream &)) {

		if (p_manipulator == static_cast<std::ostream & (*)(std::ostream &)>(std::endl)) {
			_flush();
		}
		else {
			_currentMsg << p_manipulator;
		}

		return *this;
	}

	/**
	 * @brief Transfer a argument to the stringstream buffer waiting to be flushed.
	*/
	template <typename T>
	Logger & operator<<(T const & p_arg) {

		_currentMsg << p_arg;
		return *this;
	}

};

extern Logger logger;