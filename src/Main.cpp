#include "Config.h"
#include "Hooks.h"

#include <stddef.h>

using namespace RE::BSScript;
using namespace SKSE;
using namespace SKSE::log;
using namespace SKSE::stl;

void
MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
    switch (a_msg->type)
    {
        case SKSE::MessagingInterface::kDataLoaded:
            Config::ReadConfig();
            InitializeHooking();
            break;
    }
}

void
InitLog()
{
    auto path = logger::log_directory();
    if (!path)
    {
        SKSE::stl::report_and_fail("Unable to lookup SKSE logs directory.");
    }
    *path /= Version::PROJECT;
    *path += L".log";

    auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

    auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

    log->set_level(spdlog::level::info);
    log->flush_on(spdlog::level::info);

    spdlog::set_default_logger(std::move(log));
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] %v");
}

extern "C" DLLEXPORT bool SKSEAPI
SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
    InitLog();
    logger::info("Dodge Framework loaded");

    SKSE::Init(a_skse);

    auto messaging = SKSE::GetMessagingInterface();
    if (!messaging->RegisterListener("SKSE", MessageHandler))
    {
        return false;
    }

    return true;
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() noexcept
{
    SKSE::PluginVersionData v;
    v.PluginName(Version::PROJECT.data());
    v.PluginVersion(Version::VERSION);
    v.UsesAddressLibrary();
    v.UsesNoStructs();
    return v;
}();

extern "C" DLLEXPORT bool SKSEAPI
SKSEPlugin_Query(const SKSE::QueryInterface*, SKSE::PluginInfo* pluginInfo)
{
    pluginInfo->name        = Version::PROJECT.data();
    pluginInfo->infoVersion = SKSE::PluginInfo::kVersion;
    pluginInfo->version     = 1;
    return true;
}
