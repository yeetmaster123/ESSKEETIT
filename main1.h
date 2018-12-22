/* includes and shit */
#pragma once
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <process.h>
#include <assert.h>
#include <Mmsystem.h>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <memory>
#include <Psapi.h>
#pragma comment( lib, "Winmm.lib" )

#define WIN32_LEAN_AND_MEAN
#pragma optimize("gsy",on)

#include "crc321.h"
#include "sdk1.h"
#include "vmt1.h"

extern std::uint8_t* PatternScan(void* module, const char* signature);