
#pragma once
#define _WINSOCKAPI_

#include <Windows.h>
#include <cstdlib>
#include <cstdio>
#include <math.h>
#include <string>
#include <conio.h>
#include <vector>
#include "MetaInfo.h"
enum TeamID : int
{
	TEAM_UNASSIGNED,
	TEAM_SPECTATOR,
	TEAM_TERRORIST,
	TEAM_COUNTER_TERRORIST,
};