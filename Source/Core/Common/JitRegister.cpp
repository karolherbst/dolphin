// Copyright 2014 Dolphin Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#include <cinttypes>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <string>

#include "Common/CommonTypes.h"
#include "Common/FileUtil.h"
#include "Common/JitRegister.h"
#include "Common/StringUtil.h"
#include "Core/ConfigManager.h"

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

#if defined USE_OPROFILE && USE_OPROFILE
#include <opagent.h>
#endif

#if defined USE_VTUNE
#include <jitprofiling.h>
#pragma comment(lib, "libittnotify.lib")
#pragma comment(lib, "jitprofiling.lib")
#endif

#if defined USE_OPROFILE && USE_OPROFILE
static op_agent_t s_agent = nullptr;
#endif

static File::IOFile s_perf_map_file;

namespace JitRegister
{

void Init()
{
#if defined USE_OPROFILE && USE_OPROFILE
	s_agent = op_open_agent();
#endif

	const std::string& perf_dir = SConfig::GetInstance().m_LocalCoreStartupParameter.m_perfDir;
	if (!perf_dir.empty())
	{
		std::string filename = StringFromFormat("%s/perf-%d.map", perf_dir.data(), getpid());
		s_perf_map_file.Open(filename, "w");
		// Disable buffering in order to avoid missing some mappings
		// if the event of a crash:
		std::setvbuf(s_perf_map_file.GetHandle(), NULL, _IONBF, 0);
	}
}

void Shutdown()
{
#if defined USE_OPROFILE && USE_OPROFILE
	op_close_agent(s_agent);
	s_agent = nullptr;
#endif

#ifdef USE_VTUNE
	iJIT_NotifyEvent(iJVM_EVENT_TYPE_SHUTDOWN, nullptr);
#endif

	if (s_perf_map_file.IsOpen())
		s_perf_map_file.Close();
}

void Register(const void* base_address, u32 code_size,
	const char* name, u32 original_address)
{
#if !(defined USE_OPROFILE && USE_OPROFILE) && !defined(USE_VTUNE)
	if (!s_perf_map_file.IsOpen())
		return;
#endif

	std::string symbol_name;
	if (original_address)
		symbol_name = StringFromFormat("%s_%x", name, original_address);
	else
		symbol_name = name;

#if defined USE_OPROFILE && USE_OPROFILE
	op_write_native_code(s_agent, symbol_name.data(), (u64)base_address,
		base_address, code_size);
#endif

#ifdef USE_VTUNE
	iJIT_Method_Load jmethod = {0};
	jmethod.method_id = iJIT_GetNewMethodID();
	jmethod.class_file_name = "";
	jmethod.source_file_name = __FILE__;
	jmethod.method_load_address = base_address;
	jmethod.method_size = code_size;
	jmethod.line_number_size = 0;
	jmethod.method_name = symbol_name.data();
	iJIT_NotifyEvent(iJVM_EVENT_TYPE_METHOD_LOAD_FINISHED, (void*)&jmethod);
#endif

	// Linux perf /tmp/perf-$pid.map:
	if (s_perf_map_file.IsOpen())
	{
		std::string entry = StringFromFormat(
			"%" PRIx64 " %x %s\n",
			(u64)base_address, code_size, symbol_name.data());
		s_perf_map_file.WriteBytes(entry.data(), entry.size());
	}
}

}
