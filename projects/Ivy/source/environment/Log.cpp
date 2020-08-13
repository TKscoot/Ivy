#include "ivypch.h"
#include "Log.h"

Ivy::Ptr<spdlog::logger> Ivy::Debug::mCoreLogger;
Ivy::Ptr<spdlog::logger> Ivy::Debug::mAppLogger;

void Ivy::Debug::Initialize()
{
	spdlog::sink_ptr logSinkCore = CreatePtr<spdlog::sinks::stdout_color_sink_mt>();
	spdlog::sink_ptr logSinkApp = CreatePtr<spdlog::sinks::stdout_color_sink_mt>();

	logSinkCore->set_pattern("%H:%M:%S|%n|%^%l: %v%$");
	logSinkApp->set_pattern("%H:%M:%S|%n|%^%l: %v%$");

	mCoreLogger = CreatePtr<spdlog::logger>("IvyCore", logSinkCore);
	spdlog::register_logger(mCoreLogger);

	mCoreLogger->set_level(spdlog::level::trace);
	mCoreLogger->flush_on(spdlog::level::trace);

	mAppLogger = CreatePtr<spdlog::logger>("App", logSinkApp);
	spdlog::register_logger(mAppLogger);

	mCoreLogger->set_level(spdlog::level::trace);
	mCoreLogger->flush_on(spdlog::level::trace);
}


void Ivy::Debug::SetAppPattern(const char * pattern)
{
	mAppLogger->set_pattern(pattern);
}

void Ivy::Debug::SetCorePattern(const char * pattern)
{
	mCoreLogger->set_pattern(pattern);
}

