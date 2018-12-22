#pragma once

#include "Hooks.h"
#include "Controls.h"

struct RagebotConfig
{
	
};

struct HitscanConfig
{
	std::vector<MultiBoxItem> hitboxes;
};

struct AAConfig
{
	
};

struct ResolverConfig
{
	
};

struct LegitConfig
{

};

struct VisualsConfig
{
	
};

struct MiscConfig
{

};

struct SkinChangerConfig
{
	bool bEnabled;
	bool bOverrideKnife;
	int iKnife;
	int iKnifePaintkit;
	float flKnifeWear;
	int iKnifeQuality;

	int iCZ75_Painkit;
	int iEagle_Painkit;
	int iDuals_Painkit;
	int iFiveSeveN_Painkit;
	int iGlock18_Painkit;
	int ip2k_Painkit;
	int iP250_Painkit;
	int iR8_Painkit;
	int itec9_Painkit;
	int ip250_Painkit;
	int iUsp_Painkit;
	int ir8_Painkit;
	int ifiveseven_Painkit;

	int iAK47_Painkit;
	bool bAK47_ST;

	int iAUG_Painkit;
	bool bAUG_ST;

	int iAWP_Painkit;
	bool bAWP_ST;

	int iFAMAS_Painkit;
	bool bFAMAS_ST;

	int iM4A1S_Painkit;
	bool bM4A1S_ST;

	int iSSG08_Painkit;
	bool bSSG08_ST;
};


extern HitscanConfig hitscanconfig;

extern ResolverConfig resolverconfig;



extern SkinChangerConfig skinconfig;

struct Settings
{
	void Update();
};

extern Settings settings;