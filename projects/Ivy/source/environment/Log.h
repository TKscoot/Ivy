#pragma once
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
		/// Prints uncolored (white) message to stdout
		///
		template<typename... Args>
		static void Log(const char *fmt, const Args &... args)
		{
			mAppLogger->trace(fmt, std::forward<Args>(args)...);
		}

		///
		/// Prints green success message to stdout
		///
		template<typename... Args>
		static void Info(const char *fmt, const Args &... args)
		{
			mAppLogger->info(fmt, std::forward<Args>(args)...);
		}

		///
		/// Prints yellow warning message to stdout
		///
		template<typename... Args>
		static void Warning(const char *fmt, const Args &... args)
		{
			mAppLogger->warn(fmt, std::forward<Args>(args)...);

		}

		///
		/// Prints Red error message to stdout
		///
		template<typename... Args>
		static void Error(const char *fmt, const Args &... args)
		{
			mAppLogger->error(fmt, std::forward<Args>(args)...);
		}

		///
		/// Prints red highlighted critical error message to stdout
		///
		template<typename... Args>
		static void Critical(const char *fmt, const Args &... args)
		{
			mAppLogger->critical(fmt, std::forward<Args>(args)...);
		}

		// Core/Engine logging
		
		///
		/// Prints uncolored (white) message to stdout
		///
		template<typename... Args>
		static void CoreLog(const char *fmt, const Args &... args)
		{
			mCoreLogger->trace(fmt, std::forward<Args>(args)...);
		}

		///
		/// Prints green success message to stdout
		///
		template<typename... Args>
		static void CoreInfo(const char *fmt, const Args &... args)
		{
			mCoreLogger->info(fmt, std::forward<Args>(args)...);
		}

		///
		/// Prints yellow warning message to stdout
		///
		template<typename... Args>
		static void CoreWarning(const char *fmt, const Args &... args)
		{
			mCoreLogger->warn(fmt, std::forward<Args>(args)...);
		}

		///
		/// Prints Red error message to stdout
		///
		template<typename... Args>
		static void CoreError(const char *fmt, const Args &... args)
		{
			mCoreLogger->error(fmt, std::forward<Args>(args)...);
		}

		///
		/// Prints red highlighted critical error message to stdout
		///
		template<typename... Args>
		static void CoreCritical(const char *fmt, const Args &... args)
		{
			mCoreLogger->critical(fmt, std::forward<Args>(args)...);
		}

	private:
		static void Initialize();


		static Ptr<spdlog::logger> mCoreLogger;
		static Ptr<spdlog::logger> mAppLogger;
	};
}
