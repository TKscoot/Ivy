#pragma once
// fmtlib defines
#define FMT_DEPRECATED

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Types.h"

#undef ERROR

namespace Ivy
{
	class Debug
	{
	// Make Engine a friend of this class so it has access to private members
	// because userspace should not get access of Initialize() but it needs to
	// be called in core
	friend class Engine;
	
	public:

		///
		/// Sets the formatting of the application logger
		/// @param see spdlog formatting documentation for examples
		///
		void SetAppPattern(const char* pattern);


		///
		/// Sets the formatting of the core logger
		/// @param see spdlog formatting documentation for examples
		///
		void SetCorePattern(const char* pattern);


		// Application logging



		///
		/// Prints white trace message to stdout
		///
		template<typename T>
		static void Log(T &msg)
		{
			mAppLogger->trace(msg);
		}

		///
		/// Prints green info message to stdout
		///
		template<typename T>
		static void Info(T &msg)
		{
			mAppLogger->info(msg);
		}

		///
		/// Prints yellow warning message to stdout
		///
		template<typename T>
		static void Warning(T &msg)
		{
			mAppLogger->warn(msg);
		}

		///
		/// Prints red error message to stdout
		///
		template<typename T>
		static void Error(T &msg)
		{
			mAppLogger->error(msg);
		}

		///
		/// Prints red highlighted critical error message to stdout
		///
		template<typename T>
		static void Critical(T &msg)
		{
			mAppLogger->critical(msg);
		}

		///
		/// Prints uncolored (white) core/engine message to stdout
		///
		template<typename T>
		static void CoreLog(T &msg)
		{
			mCoreLogger->trace(msg);
		}

		///
		/// Prints green success core/engine message to stdout
		///
		template<typename T>
		static void CoreInfo(T &msg)
		{
			mCoreLogger->info(msg);
		}

		///
		/// Prints yellow warning core/engine message to stdout
		///
		template<typename T>
		static void CoreWarning(T &msg)
		{
			mCoreLogger->warn(msg);
		}

		///
		/// Prints Red error core/engine message to stdout
		///
		template<typename T>
		static void CoreError(T &msg)
		{
			mCoreLogger->error(msg);
		}

		///
		/// Prints red highlighted critical error core/engine message to stdout
		///
		template<typename T>
		static void CoreCritical(T &msg)
		{
			mCoreLogger->critical(msg);
		}

		


		///
		/// Prints white trace message to stdout
		///
		template<typename FormatString, typename... Args>
		static void Log(const FormatString &fmt, const Args &... args)
		{
			mAppLogger->trace(fmt, args...);
		}

		///
		/// Prints green info message to stdout
		///
		template<typename FormatString, typename... Args>
		static void Info(const FormatString &fmt, const Args &... args)
		{
			mAppLogger->info(fmt, args...);
		}

		///
		/// Prints yellow warning message to stdout
		///
		template<typename FormatString, typename... Args>
		static void Warning(const FormatString &fmt, const Args &... args)
		{
			mAppLogger->warn(fmt, args...);

		}

		///
		/// Prints red error message to stdout
		///
		template<typename FormatString, typename... Args>
		static void Error(const FormatString &fmt, const Args &... args)
		{
			mAppLogger->error(fmt, args...);
		}

		///
		/// Prints red highlighted critical error message to stdout
		///
		template<typename FormatString, typename... Args>
		static void Critical(const FormatString &fmt, const Args &... args)
		{
			mAppLogger->critical(fmt, args...);
		}

		// Core/Engine logging
		
		///
		/// Prints uncolored (white) core/engine message to stdout
		///
		template<typename FormatString, typename... Args>
		static void CoreLog(const FormatString &fmt, const Args &... args)
		{
			mCoreLogger->trace(fmt, args...);
		}

		///
		/// Prints green success core/engine message to stdout
		///
		template<typename FormatString, typename... Args>
		static void CoreInfo(const FormatString &fmt, const Args &... args)
		{
			mCoreLogger->info(fmt, args...);
		}

		///
		/// Prints yellow warning core/engine message to stdout
		///
		template<typename FormatString, typename... Args>
		static void CoreWarning(const FormatString &fmt, const Args &... args)
		{
			mCoreLogger->warn(fmt, args...);
		}

		///
		/// Prints Red error core/engine message to stdout
		///
		template<typename FormatString, typename... Args>
		static void CoreError(const FormatString &fmt, const Args &... args)
		{
			mCoreLogger->error(fmt, args...);
		}

		///
		/// Prints red highlighted critical error core/engine message to stdout
		///
		template<typename FormatString, typename... Args>
		static void CoreCritical(const FormatString &fmt, const Args &... args)
		{
			mCoreLogger->critical(fmt, args...);
		}

	private:
		static void Initialize();


		static Ptr<spdlog::logger> mCoreLogger;
		static Ptr<spdlog::logger> mAppLogger;
	};
}
