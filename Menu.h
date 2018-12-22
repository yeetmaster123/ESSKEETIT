

#pragma once

#include "GUI.h"
#include "Controls.h"

class CRageBotTab : public CTab
{
public:
	void Setup();

	
};

class CLegitTab : public CTab
{
public:
	void Setup();

	

};

class CVisualTab : public CTab
{
public:
	void Setup();

	
};

class CMiscTab : public CTab
{
public:
	void Setup();

	
};

class CSkinsTab : public CTab
{
public:
	void Setup();

	CCheckBox SkinChangerEnabled;
	CLabel EnabledLabel;
	
	CGroupBox2 OverrideModelGroup;
	CCheckBox OverrideKnife;
	CComboBox KnifeSelection;
	CButton SkinFullUpdate;
	CButton KnifeFullUpdate;
	CButton PistolFullUpdate;
	CListBox KnifeSkin;
	CSlider KnifeWear;
	CTextField KnifeSeed;
	CComboBox KnifeEntityQuality;

	CGroupBox SkinsGroup;

	CComboBox CZ75_Skin;
	CComboBox Eagle_Skin;
	CComboBox duals_Skin;
	CComboBox fiveseven_Skin;
	CComboBox Glock18_Skin;
	CComboBox p2k_Skin;
	CComboBox p250_Skin;
	CComboBox r8_Skin;
	CComboBox tec9_Skin;
	CComboBox Usp_Skin;

	CComboBox AK47_Skin;
	CCheckBox AK47_ST;

	CComboBox AUG_Skin;
	CCheckBox AUG_ST;

	CComboBox AWP_Skin;
	CCheckBox AWP_ST;

	CComboBox FAMAS_Skin;
	CCheckBox FAMAS_ST;

	CComboBox M4A1S_Skin;
	CCheckBox M4A1S_ST;

	CComboBox SSG08_Skin;
	CCheckBox SSG08_ST;
};

class CPlayersTab : public CTab
{
public:
	void Setup();

	CListBox PlayerListControl;
	CGroupBox PlayerSettingsGroup;
	CCheckBox PlayerPriority;
	CCheckBox PlayerFriendly;
	CCheckBox PlayerPreferBody;
	CCheckBox PlayerForcePitch;
	CComboBox PlayerForcePitch_Pitch;
	CCheckBox PlayerForceYaw;
	CComboBox PlayerForceYaw_Yaw;
};

class YetiWindow : public CWindow
{
public:
	void Setup();

};
namespace Menu
{
	void SetupMenu();
	void DoFrame();

	extern YetiWindow Window;
};