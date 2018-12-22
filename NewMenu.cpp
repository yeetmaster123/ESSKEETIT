

#include <d3d9.h>
#include <dinput.h>
#include "NewMenu.h"
#include "imgui\imgui.h"
#include "imgui\dx9\imgui_dx9.h"
#include "ImGui/imgui_internal.h"
#include "shonaxsettings.h"
#include "varibles.h"
#include "Font.h"
#include "GameSearch.h"
#include "Hooks.h"
#include "Protobuffs.h"
#include "sticker.h"
#include "NewEventLog.h"
CGameSearch search;
/*
b1g menu
hitsounds
checkboxes
*/

bool _visible = true;
namespace ImGui
{

	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	IMGUI_API bool ComboBoxArray(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		if (values.empty()) { return false; }
		return Combo(label, currIndex, vector_getter,
			static_cast<void*>(&values), values.size());
	}
}
void morphBtnActiveTab()
{
	auto& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Button] = ImColor(0.13f, 0.13f, 0.13f, 1.f);
	style.Colors[ImGuiCol_ButtonHovered] = ImColor(0.13f, 0.13f, 0.13f, 1.f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.13f, 0.13f, 0.13f, 1.f);
	style.Colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);
}

void morphBtnNormalTab()
{
	auto& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Button] = ImColor(40, 42, 47, 255);
	style.Colors[ImGuiCol_ButtonHovered] = ImColor(40, 42, 47, 255);
	style.Colors[ImGuiCol_ButtonActive] = ImColor(40, 42, 47, 255);
	style.Colors[ImGuiCol_Text] = ImColor(90, 90, 90, 255);
}

static char ConfigName[64] = { 0 };




struct hud_weapons_t {
	std::int32_t* get_weapon_count() {
		return reinterpret_cast<std::int32_t*>(std::uintptr_t(this) + 0x80);
	}
};
template<class T>
static T* FindHudElement(const char* name)
{
	static auto pThis = *reinterpret_cast<DWORD**>(search.signature("client_panorama.dll", "B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08") + 1);

	static auto find_hud_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(search.signature("client_panorama.dll", "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28"));
	return (T*)find_hud_element(pThis, name);
}

void KnifeApplyCallbk()
{

	static auto clear_hud_weapon_icon_fn =
		reinterpret_cast<std::int32_t(__thiscall*)(void*, std::int32_t)>(
			search.signature("client_panorama.dll", "55 8B EC 51 53 56 8B 75 08 8B D9 57 6B FE 2C 89 5D FC"));

	auto element = FindHudElement<std::uintptr_t*>("CCSGO_HudWeaponSelection");

	auto hud_weapons = reinterpret_cast<hud_weapons_t*>(std::uintptr_t(element) - 0xA0);
	if (hud_weapons == nullptr)
		return;

	if (!*hud_weapons->get_weapon_count())
		return;

	for (std::int32_t i = 0; i < *hud_weapons->get_weapon_count(); i++)
		i = clear_hud_weapon_icon_fn(hud_weapons, i);

	typedef void(*ForceUpdate) (void);
	ForceUpdate FullUpdate = (ForceUpdate)search.FindSignaturenew("engine.dll", "FullUpdate", "A1 ? ? ? ? B9 ? ? ? ? 56 FF 50 14 8B 34 85");
	FullUpdate();
}





void DrawMenu()
{

	static int tab_count = 0;
	//ImGui::BeginChild("##tabs", ImVec2(150, 0), true, 0);
	//{

	const char* tabs[] = { "     Ragebot", "     Legitbot", "     Visuals", "     Misc", "     Changers", "     Configs" };
	const char* hitmarker[] = { "None", "Nnware", "Your mad", "Skeet", "Shrek", "Aimware", "Rifk 7 beep", "BameWare" };
	const char* buttons[] = { "Default", "Fill", "Circle", "Rectangle", "Checkmark" };
	ImGui::PushItemWidth(140);
	ImGui::ListBox("##tablist", &tab_count, tabs, ARRAYSIZE(tabs), 24);
	ImGui::PopItemWidth();

	//(ImVec2(600, 580));
	//}
	//ImGui::EndChild();
	ImGui::SameLine();
	ImGui::BeginChild("##tabs2", ImVec2(0, 0), true, 0);
	{

		switch (tab_count)
		{
		case 0:
		{
			ImGui::Text("Ragebot");
				ImGui::Checkbox("Ragebot enabled", &Shonax::settingsxd.aimbot_enabled);
				ImGui::newSliderFloat("Ragebot fov", &Shonax::settingsxd.aimbotfov, 0.0f, 180.f, ("%.1f"));
				ImGui::PushItemWidth(150.0f);
				ImGui::Combo("Ragebot mode", &Shonax::settingsxd.aimbot_mode, aimmode, ARRAYSIZE(aimmode));
				ImGui::PushItemWidth(150.0f);
				ImGui::Combo("Selections", &Shonax::settingsxd.selection_mode, selections, ARRAYSIZE(selections));
				ImGui::Checkbox("NoRecoil", &Shonax::settingsxd.norecoil);
				ImGui::Checkbox("Autofire", &Shonax::settingsxd.autofire);
				//ImGui::Checkbox("FriendlyFire", &Shonax::settingsxd.friendlyfire);
				ImGui::PushItemWidth(150.0f);
				ImGui::Combo("Autofire mode", &Shonax::settingsxd.autofire_mode, aytofiremode, ARRAYSIZE(aytofiremode));
				ImGui::Combo("Ragebot Bone", &Shonax::settingsxd.autofire_bone, autofirebone, ARRAYSIZE(autofirebone));
				ImGui::Hotkey("Autofire hotkey", &Shonax::settingsxd.autofirehotkey);
				ImGui::PushItemWidth(150.0f);
				ImGui::Combo("Revolver mode", &Shonax::settingsxd.revolver_mode, revolvermode, ARRAYSIZE(revolvermode));
				ImGui::Checkbox("Autowall enabled", &Shonax::settingsxd.autowall);
				ImGui::newSliderFloat("Autowall mid dam", &Shonax::settingsxd.autowalldamage, 5.0f, 110.f, ("%.1f"));
				ImGui::Checkbox("Prefer body enabled", &Shonax::settingsxd.preferbody);
				ImGui::PushItemWidth(150.0f);
				ImGui::newSliderFloat("Body aim on hp", &Shonax::settingsxd.bodyaimonxp, 0.0f, 100.f, ("%.1f"));
				ImGui::Checkbox("Prefer body if dead", &Shonax::settingsxd.bodyaimifdead);
				ImGui::Checkbox("Prefer body with awp", &Shonax::settingsxd.bodyaimiawp);
				ImGui::PushItemWidth(150.0f);
				ImGui::Combo("Body awp mode", &Shonax::settingsxd.bodyaim_mode, bodyaimmode, ARRAYSIZE(bodyaimmode));
				ImGui::Checkbox("Prefer body with scout", &Shonax::settingsxd.bodyaimiscout);
				ImGui::PushItemWidth(150.0f);
				ImGui::Combo("Body scout mode", &Shonax::settingsxd.bodyaim_mode1, bodyaimmode, ARRAYSIZE(bodyaimmode));
				ImGui::Checkbox("Multipoint", &Shonax::settingsxd.multipoint);
				ImGui::newSliderFloat("Multipoint scale", &Shonax::settingsxd.pointscale, 0.0f, 100.f, ("%.1f"));
				ImGui::Checkbox("Hitchance", &Shonax::settingsxd.hitchance);
				ImGui::newSliderFloat("Hitchance scale", &Shonax::settingsxd.hitchancescale, 0.0f, 100.f, ("%.1f"));
				ImGui::Text("Other");
				ImGui::Checkbox("Predict engine", &Shonax::settingsxd.enginepred);
				ImGui::Checkbox("Accuracy while standing", &Shonax::settingsxd.accuracystanding);
				ImGui::Checkbox("Autostop", &Shonax::settingsxd.autostop);
				//ImGui::PushItemWidth(150.0f);
				ImGui::Combo("Autostop mode", &Shonax::settingsxd.autostop_mode, autostopmode, ARRAYSIZE(autostopmode));
				ImGui::Checkbox("Autoscope", &Shonax::settingsxd.autoscope);
				ImGui::Checkbox("Antiaim correction", &Shonax::settingsxd.antiaimcorrection);
				//ImGui::Combo("Antiaim correction type", &Shonax::settingsxd.antiaimcorrectiontype, antiaimcorrectiontype, ARRAYSIZE(antiaimcorrectiontype));
				ImGui::Checkbox("Resolver", &Shonax::settingsxd.shitdogresolver);
				ImGui::Checkbox("Override enable", &Shonax::settingsxd.overrideenable);
				ImGui::Hotkey("Override hotkey", &Shonax::settingsxd.overridehotkey);
				ImGui::Checkbox("Disable Interpolation", &Shonax::settingsxd.disabieInterpolation);
				ImGui::Checkbox("Backtrack", &Shonax::settingsxd.backtrack);
				ImGui::Checkbox("Linear Extrapolations", &Shonax::settingsxd.linearextrapolations);
				ImGui::Checkbox("Resolver Info", &Shonax::settingsxd.resolverdebug);
				ImGui::Checkbox("LBY Indicator", &Shonax::settingsxd.lbyindicator);
				ImGui::Text("Bones:");
				ImGui::BeginChild("Bones", ImVec2(0, 120), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
				for (int i = 0; i < ARRAYSIZE(MultiHitboxes); ++i)
				{
					ImGui::Selectable(MultiHitboxes[i], &Shonax::settingsxd.multiHitboxes[i]);
				}
				ImGui::EndChild();
				ImGui::Separator();
				ImGui::Text("Anti Aim");
				ImGui::PushItemWidth(150.0f);
				ImGui::Combo("AA Type", &Shonax::settingsxd.antiaimmode, antiaimmode, ARRAYSIZE(antiaimmode));
				if (Shonax::settingsxd.antiaimmode == 0)
				{
					ImGui::PushItemWidth(150.0f);
					ImGui::Combo("Stand P", &Shonax::settingsxd.standaapitch, aatypepitch, ARRAYSIZE(aatypepitch));
					ImGui::PushItemWidth(150.0f);
					ImGui::Combo("Stand Y", &Shonax::settingsxd.standaayaw, aatypeyaw, ARRAYSIZE(aatypeyaw));
					ImGui::newSliderFloat("Stand Yaw offset", &Shonax::settingsxd.yawoffset, -180.0f, 180.f, ("%.1f"));
					ImGui::PushItemWidth(150.0f);
					ImGui::Combo("Fake S Y", &Shonax::settingsxd.fakestandaayaw, fakeaatypeyaw, ARRAYSIZE(fakeaatypeyaw));
					ImGui::newSliderFloat("Fake Stand Yaw offset", &Shonax::settingsxd.fakeyawoffset, -180.0f, 180.f, ("%.1f"));
					ImGui::Checkbox("Stand clean up", &Shonax::settingsxd.standaacleanup);
				}
				if (Shonax::settingsxd.antiaimmode == 1)
				{
					ImGui::PushItemWidth(150.0f);
					ImGui::Combo("Move P", &Shonax::settingsxd.moveaapitch, aatypepitch, ARRAYSIZE(aatypepitch));
					ImGui::PushItemWidth(150.0f);
					ImGui::Combo("Move Y", &Shonax::settingsxd.moveaayaw, aatypeyaw, ARRAYSIZE(aatypeyaw));
					ImGui::newSliderFloat("Move Yaw offset", &Shonax::settingsxd.moveyawoffset, -180.0f, 180.f, ("%.1f"));
					ImGui::PushItemWidth(150.0f);
					ImGui::Combo("Fake M Y", &Shonax::settingsxd.movefakeaayaw, fakeaatypeyaw, ARRAYSIZE(fakeaatypeyaw));
					ImGui::newSliderFloat("Fake Move Yaw offset", &Shonax::settingsxd.movefakeyawoffset, -180.0f, 180.f, ("%.1f"));
					ImGui::Checkbox("Move clean up", &Shonax::settingsxd.moveaacleanup);
				}
				if (Shonax::settingsxd.antiaimmode == 2)
				{
					ImGui::PushItemWidth(150.0f);
					ImGui::Combo("Edge", &Shonax::settingsxd.edge_mode, edgemode, ARRAYSIZE(edgemode));
					ImGui::PushItemWidth(150.0f);
					ImGui::Combo("Edge P", &Shonax::settingsxd.edgeaapitch, aatypepitch, ARRAYSIZE(aatypepitch));
					ImGui::PushItemWidth(150.0f);
					ImGui::Combo("Edge Y", &Shonax::settingsxd.edgeaayaw, edgeaatypeyaw, ARRAYSIZE(edgeaatypeyaw));
					ImGui::newSliderFloat("Edge Yaw offset", &Shonax::settingsxd.edgeyawoffset, -180.0f, 180.f, ("%.1f"));
					ImGui::PushItemWidth(150.0f);
					ImGui::Combo("Fake E Y", &Shonax::settingsxd.edgefakeyaw, edgeaatypeyaw, ARRAYSIZE(edgeaatypeyaw));
					ImGui::newSliderFloat("Fake Edge Yaw offset", &Shonax::settingsxd.edgefakeyawoffset, -180.0f, 180.f, ("%.1f"));
				}
				if (Shonax::settingsxd.antiaimmode == 3)
				{
					ImGui::Checkbox("Dormant check", &Shonax::settingsxd.dormantcheck);
					ImGui::PushItemWidth(150.0f);
					ImGui::Combo("Auto dir", &Shonax::settingsxd.autodirection_mode, edgemode, ARRAYSIZE(edgemode));
					ImGui::newSliderFloat("Speen seed", &Shonax::settingsxd.speedseed, 0.0f, 5.f, ("%.1f"));
					ImGui::newSliderFloat("Jitter range", &Shonax::settingsxd.jitterrange, 0.0f, 180.f, ("%.1f"));
					ImGui::Checkbox("LBY Breaker", &Shonax::settingsxd.lbybreaker);
					ImGui::newSliderFloat("LBY Delta", &Shonax::settingsxd.lbydelta, -180.0f, 180.f, ("%.1f"));
				}

				ImGui::Checkbox("Manual angle flip", &Shonax::settingsxd.manualangeflip);
				ImGui::Checkbox("Manual angle flip arrow", &Shonax::settingsxd.manualangefliparrow);
				ImGui::Hotkey("Left flip", &Shonax::settingsxd.leftflip);
				ImGui::Hotkey("Right flip", &Shonax::settingsxd.rightflip);
				ImGui::Hotkey("Back flip", &Shonax::settingsxd.backflip);
		}
		break;
		case 1:
		{
			ImGui::Text("Legitbot");
				ImGui::Combo("Legit weapon", &Shonax::settingsxd.legitweapon, legitweapon, ARRAYSIZE(legitweapon));
				if (Shonax::settingsxd.legitweapon == 0)
				{
					ImGui::Checkbox("Pistol enabled", &Shonax::settingsxd.pistolenabled);
					ImGui::Hotkey("Pistol hotkey", &Shonax::settingsxd.pistolhotkey);
					ImGui::newSliderFloat("Pistol fov", &Shonax::settingsxd.pistolfov, 0.0f, 5.f, ("%.1f"));
					ImGui::newSliderFloat("Pistol smooth", &Shonax::settingsxd.pistolsmooth, 0.0f, 5.f, ("%.1f"));
					ImGui::newSliderFloat("Pistol recoil", &Shonax::settingsxd.pistolrecoil, 0.0f, 2.f, ("%.1f"));
					ImGui::Checkbox("Autopistol", &Shonax::settingsxd.autopistol);
					ImGui::PushItemWidth(150.0f);
					//ImGui::Combo("Pistol mode", &Shonax::settingsxd.pistolmode, aimmode, ARRAYSIZE(aimmode));
					ImGui::Checkbox("Pistol autofire", &Shonax::settingsxd.pistolautofire);
					ImGui::Combo("Pistolfiremode", &Shonax::settingsxd.pistolautofire_mode, aytofiremode, ARRAYSIZE(aytofiremode));
					ImGui::Combo("Pistol Bone", &Shonax::settingsxd.pistolbone, autofirebone, ARRAYSIZE(autofirebone));
					ImGui::PushItemWidth(150.0f);
					ImGui::Combo("Pistol react", &Shonax::settingsxd.reactiontime, reactiontime, ARRAYSIZE(reactiontime));
				}
				if (Shonax::settingsxd.legitweapon == 1)
				{
					ImGui::Checkbox("Smg enabled", &Shonax::settingsxd.smgenabled);
					ImGui::Hotkey("Smg hotkey", &Shonax::settingsxd.smghotkey);
					ImGui::newSliderFloat("Smg fov", &Shonax::settingsxd.smgfov, 0.0f, 5.f, ("%.1f"));
					ImGui::newSliderFloat("Smg smooth", &Shonax::settingsxd.smgsmooth, 0.0f, 5.f, ("%.1f"));
					ImGui::newSliderFloat("Smg recoil", &Shonax::settingsxd.smgrecoil, 0.0f, 2.f, ("%.1f"));
					ImGui::PushItemWidth(150.0f);
					//ImGui::Combo("Smg mode", &Shonax::settingsxd.smgmode, aimmode, ARRAYSIZE(aimmode));
					ImGui::Checkbox("Smg autofire", &Shonax::settingsxd.smgautofire);
					ImGui::Combo("Smgfiremode", &Shonax::settingsxd.smgautofire_mode, aytofiremode, ARRAYSIZE(aytofiremode));
					ImGui::Combo("Smg Bone", &Shonax::settingsxd.smgbone, autofirebone, ARRAYSIZE(autofirebone));
					ImGui::PushItemWidth(150.0f);
					ImGui::Combo("Smg react", &Shonax::settingsxd.smgreactiontime, reactiontime, ARRAYSIZE(reactiontime));
				}
				if (Shonax::settingsxd.legitweapon == 2)
				{
					ImGui::Checkbox("Rifle enabled", &Shonax::settingsxd.rifleenabled);
					ImGui::Hotkey("Rifle hotkey", &Shonax::settingsxd.riflehotkey);
					ImGui::newSliderFloat("Rifle fov", &Shonax::settingsxd.riflefov, 0.0f, 5.f, ("%.1f"));
					ImGui::newSliderFloat("Rifle smooth", &Shonax::settingsxd.riflesmooth, 0.0f, 5.f, ("%.1f"));
					ImGui::newSliderFloat("Rifle recoil", &Shonax::settingsxd.riflerecoil, 0.0f, 2.f, ("%.1f"));
					ImGui::PushItemWidth(150.0f);
					//ImGui::Combo("Rifle mode", &Shonax::settingsxd.riflemode, aimmode, ARRAYSIZE(aimmode));
					ImGui::Checkbox("Rifle autofire", &Shonax::settingsxd.rifleautofire);
					ImGui::Combo("Riflefiremode", &Shonax::settingsxd.rifleautofire_mode, aytofiremode, ARRAYSIZE(aytofiremode));
					ImGui::Combo("Rifle Bone", &Shonax::settingsxd.riflebone, autofirebone, ARRAYSIZE(autofirebone));
					ImGui::PushItemWidth(150.0f);
					ImGui::Combo("Rifle react", &Shonax::settingsxd.riflereactiontime, reactiontime, ARRAYSIZE(reactiontime));
				}
				if (Shonax::settingsxd.legitweapon == 3)
				{

					ImGui::Checkbox("Shotgun enabled", &Shonax::settingsxd.shotgunenabled);
					ImGui::Hotkey("Shotgun hotkey", &Shonax::settingsxd.shotgunhotkey);
					ImGui::newSliderFloat("Shotgun fov", &Shonax::settingsxd.shotgunfov, 0.0f, 5.f, ("%.1f"));
					ImGui::newSliderFloat("Shotgun smooth", &Shonax::settingsxd.shotgunsmooth, 0.0f, 5.f, ("%.1f"));
					ImGui::newSliderFloat("Shotgun recoil", &Shonax::settingsxd.shotgunrecoil, 0.0f, 2.f, ("%.1f"));
					ImGui::PushItemWidth(150.0f);
					//ImGui::Combo("Shotgun mode", &Shonax::settingsxd.shotgunmode, aimmode, ARRAYSIZE(aimmode));
					ImGui::Checkbox("Shotgun autofire", &Shonax::settingsxd.shotgunautofire);
					ImGui::Combo("Shotgunfiremode", &Shonax::settingsxd.shotgunautofire_mode, aytofiremode, ARRAYSIZE(aytofiremode));
					ImGui::Combo("Shotgun Bone", &Shonax::settingsxd.shotgunbone, autofirebone, ARRAYSIZE(autofirebone));
					ImGui::PushItemWidth(150.0f);
					ImGui::Combo("Shotgun react", &Shonax::settingsxd.shotgunreactiontime, reactiontime, ARRAYSIZE(reactiontime));
				}

				if (Shonax::settingsxd.legitweapon == 4)
				{

					ImGui::Checkbox("Sniper enabled", &Shonax::settingsxd.sniperenabled);
					ImGui::Hotkey("Sniper hotkey", &Shonax::settingsxd.sniperhotkey);
					ImGui::newSliderFloat("Sniper fov", &Shonax::settingsxd.sniperfov, 0.0f, 5.f, ("%.1f"));
					ImGui::newSliderFloat("Sniper smooth", &Shonax::settingsxd.snipersmooth, 0.0f, 5.f, ("%.1f"));
					ImGui::newSliderFloat("Sniper recoil", &Shonax::settingsxd.sniperrecoil, 0.0f, 2.f, ("%.1f"));
					ImGui::PushItemWidth(150.0f);
					//ImGui::Combo("Sniper mode", &Shonax::settingsxd.snipermode, aimmode, ARRAYSIZE(aimmode));
					ImGui::Checkbox("Sniper autofire", &Shonax::settingsxd.sniperautofire);
					ImGui::Combo("Sniperfiremode", &Shonax::settingsxd.sniperautofire_mode, aytofiremode, ARRAYSIZE(aytofiremode));
					ImGui::Combo("Sniper Bone", &Shonax::settingsxd.sniperbone, autofirebone, ARRAYSIZE(autofirebone));
					ImGui::PushItemWidth(150.0f);
					ImGui::Combo("Sniper react", &Shonax::settingsxd.sniperreactiontime, reactiontime, ARRAYSIZE(reactiontime));
				}
				ImGui::Separator();
				ImGui::Text("Other");
				ImGui::Checkbox("Legit antiaim enabled", &Shonax::settingsxd.legitantiaim);
				ImGui::Combo("Type legit AA", &Shonax::settingsxd.legitchoicelist, legitchoicelist, ARRAYSIZE(legitchoicelist));
				ImGui::Checkbox("Legit backtrack", &Shonax::settingsxd.legitbacktrack);
				ImGui::newSliderFloat("Legit backtrack tick", &Shonax::settingsxd.backtrackticks, 0.0f, 14.f, ("%.1f"));
				ImGui::Checkbox("Legit resolver enabled", &Shonax::settingsxd.legitresolver);
				ImGui::newSliderFloat("Bruteforce after shots", &Shonax::settingsxd.bruteAfterX, 0.0f, 5.f, ("%.1f"));
		}
		break;
		case 2:
		{
			ImGui::Text("ESP");
				ImGui::Combo("Esp type", &Shonax::settingsxd.esptype, esptype, ARRAYSIZE(esptype));
				ImGui::Checkbox("Teammates", &Shonax::settingsxd.espteam);
				ImGui::Checkbox("Box", &Shonax::settingsxd.espbox);
				ImGui::Combo("Box type", &Shonax::settingsxd.espboxtype, espboxtype, ARRAYSIZE(espboxtype));
				ImGui::MyColorEdit4("Box color", Shonax::settingsxd.espboxcolor, ImGuiColorEditFlags_Alpha | ImGuiColorEditFlags_NoSliders);
				ImGui::Combo("Esp mode", &Shonax::settingsxd.espmode, espmode, ARRAYSIZE(espmode));
				ImGui::Checkbox("Esp outline", &Shonax::settingsxd.espoutline);
				ImGui::Checkbox("Box Fill", &Shonax::settingsxd.espboxfill);
				ImGui::newSliderFloat("Box Fill Opacity", &Shonax::settingsxd.espboxfillopacity, 0.0f, 100.f, ("%.1f"));

				//ImGui::Checkbox("Glow", &Shonax::settingsxd.espglow); 
				//ImGui::MyColorEdit4("Glow color", Shonax::settingsxd.espglowcolor, ImGuiColorEditFlags_Alpha | ImGuiColorEditFlags_NoSliders);
				ImGui::Checkbox("Health", &Shonax::settingsxd.esphealth);
				ImGui::Combo("Health type", &Shonax::settingsxd.esphealthtype, esphealthtype, ARRAYSIZE(esphealthtype));
				//ImGui::Checkbox("Health text", &Shonax::settingsxd.esphealthtext);
				ImGui::Checkbox("Armor", &Shonax::settingsxd.esparmor);
				ImGui::MyColorEdit4("Armor color", Shonax::settingsxd.esparmorcolor, ImGuiColorEditFlags_Alpha | ImGuiColorEditFlags_NoSliders);
				ImGui::Checkbox("Hitangle", &Shonax::settingsxd.hitangle);
				ImGui::Checkbox("Directionarrow", &Shonax::settingsxd.directionarrow);
				ImGui::MyColorEdit4("Directionarrow color", Shonax::settingsxd.directionarrowcolor, ImGuiColorEditFlags_Alpha | ImGuiColorEditFlags_NoSliders);

				ImGui::Checkbox("Sceleton", &Shonax::settingsxd.espsceleton);
				ImGui::MyColorEdit4("Sceleton color", Shonax::settingsxd.espsceletoncolor, ImGuiColorEditFlags_Alpha | ImGuiColorEditFlags_NoSliders);

				ImGui::Checkbox("Hitbones", &Shonax::settingsxd.esphitbones);
				ImGui::MyColorEdit4("Hitbones color", Shonax::settingsxd.esphitbonescolor, ImGuiColorEditFlags_Alpha | ImGuiColorEditFlags_NoSliders);

				ImGui::Checkbox("Snaplines", &Shonax::settingsxd.espsnaplines);
				ImGui::MyColorEdit4("Snaplines color", Shonax::settingsxd.espsnaplinescolor, ImGuiColorEditFlags_Alpha | ImGuiColorEditFlags_NoSliders);

				ImGui::Checkbox("Names", &Shonax::settingsxd.espname);

				ImGui::Checkbox("Weapon", &Shonax::settingsxd.espweapon);
				ImGui::Checkbox("Ammo", &Shonax::settingsxd.espammo);
				ImGui::Checkbox("Scoped", &Shonax::settingsxd.espscoped);
				ImGui::Checkbox("Hitmarkers", &Shonax::settingsxd.esphitmarker);
				ImGui::Combo("Hitsound", &Shonax::settingsxd.hitmarker_val, hitmarker, ARRAYSIZE(hitmarker));
				ImGui::Checkbox("Damage indicator", &Shonax::settingsxd.espdamageindicator);

				ImGui::Checkbox("Autowall reticle", &Shonax::settingsxd.espreticle);
				ImGui::Checkbox("Autowall reticle Damage", &Shonax::settingsxd.espdamagereticle);
				//ImGui::Checkbox("Bullettracer", &Shonax::settingsxd.bullettracer);
				ImGui::Checkbox("Removeparticles", &Shonax::settingsxd.removeparticles);
				ImGui::Separator();
				ImGui::Text("Models");

				ImGui::Combo("Chams type", &Shonax::settingsxd.chamstype, chamstype, ARRAYSIZE(chamstype));
				ImGui::Checkbox("Visible enemy", &Shonax::settingsxd.chamsenemy);
				ImGui::MyColorEdit4("Visible enemy color", Shonax::settingsxd.chamsenemycolor, ImGuiColorEditFlags_Alpha | ImGuiColorEditFlags_NoSliders);
				ImGui::Checkbox("Invisible enemy", &Shonax::settingsxd.chamsenemyinvisible);
				ImGui::MyColorEdit4("Invisible enemy color", Shonax::settingsxd.chamsenemyinvisiblecolor, ImGuiColorEditFlags_Alpha | ImGuiColorEditFlags_NoSliders);
				ImGui::Checkbox("Visible team", &Shonax::settingsxd.chamsteam);
				ImGui::MyColorEdit4("Visible team color", Shonax::settingsxd.chamsteamcolor, ImGuiColorEditFlags_Alpha | ImGuiColorEditFlags_NoSliders);
				ImGui::Checkbox("Invisible team", &Shonax::settingsxd.chamsteaminvisible);
				ImGui::MyColorEdit4("Invisible team color", Shonax::settingsxd.chamsteaminvisiblecolor, ImGuiColorEditFlags_Alpha | ImGuiColorEditFlags_NoSliders);
				ImGui::Checkbox("Fake chams", &Shonax::settingsxd.espfakechams);
				ImGui::MyColorEdit4("Fake chams color", Shonax::settingsxd.espfakechamscolor, ImGuiColorEditFlags_Alpha | ImGuiColorEditFlags_NoSliders);
				ImGui::Combo("Fake chams type", &Shonax::settingsxd.fakechamstype, fakechamstype, ARRAYSIZE(fakechamstype));
				ImGui::Separator();
			ImGui::Text("Effects");
				ImGui::Checkbox("Show ranks", &Shonax::settingsxd.esprank);
				ImGui::Checkbox("Antiaim lines", &Shonax::settingsxd.antiaimlines);
				ImGui::Checkbox("Spread crosshair", &Shonax::settingsxd.spreadcrosshair);
				ImGui::Combo("Spread type", &Shonax::settingsxd.spreadcrosshairtype, spreadcrosshairtype, ARRAYSIZE(spreadcrosshairtype));
				ImGui::newSliderFloat("Fov value", &Shonax::settingsxd.fovchanger, 0.0f, 120.f, ("%.1f"));
				ImGui::Checkbox("NoVisRecoil", &Shonax::settingsxd.novisrecoil);
				ImGui::Checkbox("NoScope", &Shonax::settingsxd.noscope);
				ImGui::Checkbox("NoFlash", &Shonax::settingsxd.noflash);
				ImGui::Checkbox("NoSmoke", &Shonax::settingsxd.nosmoke);
				ImGui::Combo("NoSmoke type", &Shonax::settingsxd.nosmoketype, nosmoketype, ARRAYSIZE(nosmoketype));
				ImGui::Checkbox("Force thirdperson", &Shonax::settingsxd.forcethirdperson);
				//ImGui::Hotkey("Thirdperson hotkey", &Shonax::settingsxd.thirdpersonhotkey);
				ImGui::Combo("Thirdperson angle", &Shonax::settingsxd.thirdpersonangle, thirdpersonangle, ARRAYSIZE(thirdpersonangle));
				ImGui::Checkbox("Nightmode", &Shonax::settingsxd.nightmode);
				ImGui::Checkbox("Disable post processing", &Shonax::settingsxd.disablepostprocess);
				ImGui::Checkbox("Watermark", &Shonax::settingsxd.watermark);
				ImGui::Checkbox("FPS Counter", &Shonax::settingsxd.FPS);
				ImGui::Checkbox("Ping Counter", &Shonax::settingsxd.PING);
				ImGui::Checkbox("Grenade prediction", &Shonax::settingsxd.grenadeprediction);
				ImGui::MyColorEdit4("Grenade prediction color", Shonax::settingsxd.grenadepredline_col, ImGuiColorEditFlags_Alpha | ImGuiColorEditFlags_NoSliders);
		}
		break;
		case 3:
		{
			ImGui::Text("Misc");
				ImGui::Checkbox("Antiuntrusted", &Shonax::settingsxd.antiuntrusted);
				ImGui::Checkbox("Bunnyhop", &Shonax::settingsxd.bunnyhop);
				ImGui::Checkbox("Autostrafe", &Shonax::settingsxd.autostrafe);
				ImGui::Combo("Autostrafe type", &Shonax::settingsxd.autostrafetype, autostrafetype, ARRAYSIZE(autostrafetype));

				ImGui::Checkbox("Slowmotion", &Shonax::settingsxd.fakewalk);
				ImGui::Hotkey("Slowmotion key", &Shonax::settingsxd.slowmokey);
				ImGui::newSliderFloat("Slow motion speed", &Shonax::settingsxd.fakewalkvalue, 15.0f, 35.0f, ("%.1f"));


				ImGui::Checkbox("Fakewalk", &Shonax::settingsxd.fakewalk1);
				ImGui::Hotkey("Fakewalk key", &Shonax::settingsxd.fakewalkhotkey);
				ImGui::Checkbox("Moonwalk", &Shonax::settingsxd.moonwalk);
				ImGui::Checkbox("Eventlog", &Shonax::settingsxd.eventlogs);

				ImGui::Checkbox("Fakeping", &Shonax::settingsxd.fakeping);
				ImGui::newSliderFloat("Fakeping value", &Shonax::settingsxd.fakepingvalue, 0.0f, 800.0f, ("%.1f"));


				ImGui::Checkbox("Fakelag", &Shonax::settingsxd.fakelag);
				ImGui::Combo("Fakelag type", &Shonax::settingsxd.fakelagtypelag, fakelagtypelag, ARRAYSIZE(fakelagtypelag));
				ImGui::Combo("Fakelag activation", &Shonax::settingsxd.fakelagtype, fakelagtype, ARRAYSIZE(fakelagtype));
				ImGui::SliderInt("Flags limits", &Shonax::settingsxd.fakelagpack, 0.0f, 14.0f, ("%.1f"));
				ImGui::Checkbox("Disable flags whule shooting", &Shonax::settingsxd.disableflags);

				ImGui::Checkbox("Blockbot", &Shonax::settingsxd.blockbot);
				ImGui::Hotkey("Airstuck hotkey", &Shonax::settingsxd.airstackkey);
				ImGui::Text("Other");
					ImGui::Checkbox("Autobuy", &Shonax::settingsxd.autobuy);
					ImGui::Combo("Autobuy weapon", &Shonax::settingsxd.autobuytype, autobuytype, ARRAYSIZE(autobuytype));
					//ImGui::Checkbox("Testshit", &Shonax::settingsxd.trashtalk);
					//ImGui::Checkbox("Autoaccept", &Shonax::settingsxd.autoaccept);
					//ImGui::Checkbox("Crasher[test]", &Shonax::settingsxd.crasher);
					ImGui::Text("Incoming soon - players");


					//ImGui::Combo("Player force pitch", &Shonax::settingsxd.playerforcepitch, playerforcepitch, ARRAYSIZE(playerforcepitch));
					//ImGui::Combo("Player force yaw", &Shonax::settingsxd.playerforceyaw, playerforceyaw, ARRAYSIZE(playerforceyaw));
			
		}
		break;
		case 4:
		{
			ImGui::Text("Skin changer");
				ImGui::Checkbox("Skinchanger", &Shonax::settingsxd.skinenabled);
				ImGui::InputText("Weapon Name", &Shonax::settingsxd.customname, 256);
				ImGui::Checkbox("Glovechanger", &Shonax::settingsxd.glovesenabled);
				if (ImGui::Button(("Force update")))
				{
					KnifeApplyCallbk();
				}

			ImGui::Text("General");
				ImGui::PushItemWidth(150.0f);
				ImGui::Combo(("Knife Model"), &Shonax::settingsxd.Knife, KnifeModel, ARRAYSIZE(KnifeModel));
				ImGui::PushItemWidth(150.0f);
				ImGui::Combo(("Knife Skin"), &Shonax::settingsxd.KnifeSkin, knifeskins, ARRAYSIZE(knifeskins));
				ImGui::Combo(("glove model"), &Shonax::settingsxd.gloves, GloveModel, ARRAYSIZE(GloveModel));
				ImGui::Combo(("glove skin"), &Shonax::settingsxd.skingloves, xdshit, ARRAYSIZE(xdshit));
				ImGui::Combo(("rank"), &Shonax::settingsxd.rank_id, ranks, ARRAYSIZE(ranks));
				ImGui::SliderInt("level", &Shonax::settingsxd.level, 0, 40);
				ImGui::InputInt("wins", &Shonax::settingsxd.wins);
				ImGui::InputInt("friendly", &Shonax::settingsxd.friendly);
				ImGui::InputInt("teaching", &Shonax::settingsxd.teaching);
				ImGui::InputInt("leader", &Shonax::settingsxd.leader);
				if (ImGui::Button(("apply")))
				{
					Hooks::ProtoFeatures.SendClientHello();
					Hooks::ProtoFeatures.SendMatchmakingClient2GCHello();
				}
				ImGui::Separator();
				ImGui::Text("Weapons");
				ImGui::Combo(("AK-47"), &Shonax::settingsxd.AK47Skin, ak47, ARRAYSIZE(ak47));
				ImGui::Combo(("M4A1-S"), &Shonax::settingsxd.M4A1SSkin, m4a1s, ARRAYSIZE(m4a1s));
				ImGui::Combo(("M4A4"), &Shonax::settingsxd.M4A4Skin, m4a4, ARRAYSIZE(m4a4));
				ImGui::Combo(("Galil AR"), &Shonax::settingsxd.GalilSkin, galil, ARRAYSIZE(galil));
				ImGui::Combo(("AUG"), &Shonax::settingsxd.AUGSkin, aug, ARRAYSIZE(aug));
				ImGui::Combo(("FAMAS"), &Shonax::settingsxd.FAMASSkin, famas, ARRAYSIZE(famas));
				ImGui::Combo(("Sg553"), &Shonax::settingsxd.Sg553Skin, sg553, ARRAYSIZE(sg553));
				ImGui::Combo(("UMP45"), &Shonax::settingsxd.UMP45Skin, ump45, ARRAYSIZE(ump45));
				ImGui::Combo(("MAC-10"), &Shonax::settingsxd.Mac10Skin, mac10, ARRAYSIZE(mac10));
				ImGui::Combo(("PP-Bizon"), &Shonax::settingsxd.BizonSkin, bizon, ARRAYSIZE(bizon));
				ImGui::Combo(("TEC-9"), &Shonax::settingsxd.tec9Skin, tec9, ARRAYSIZE(tec9));
				ImGui::Combo(("P2000"), &Shonax::settingsxd.P2000Skin, p2000, ARRAYSIZE(p2000));
				ImGui::Combo(("P250"), &Shonax::settingsxd.P250Skin, p250, ARRAYSIZE(p250));
				ImGui::Combo(("Dual-Barettas"), &Shonax::settingsxd.DualSkin, dual, ARRAYSIZE(dual));
				ImGui::Combo(("Cz75-Auto"), &Shonax::settingsxd.Cz75Skin, cz75, ARRAYSIZE(cz75));
				ImGui::Combo(("Nova"), &Shonax::settingsxd.NovaSkin, nova, ARRAYSIZE(nova));
				ImGui::Combo(("Sawed-Off"), &Shonax::settingsxd.SawedSkin, sawed, ARRAYSIZE(sawed));
				ImGui::Combo(("Mag-7"), &Shonax::settingsxd.MagSkin, mag, ARRAYSIZE(mag));
				ImGui::Combo(("XM1014"), &Shonax::settingsxd.XmSkin, xm, ARRAYSIZE(xm));

				ImGui::Combo(("AWP"), &Shonax::settingsxd.AWPSkin, awp, ARRAYSIZE(awp));
				ImGui::Combo(("SSG08"), &Shonax::settingsxd.SSG08Skin, ssg08, ARRAYSIZE(ssg08));
				ImGui::Combo(("SCAR20"), &Shonax::settingsxd.SCAR20Skin, scar20, ARRAYSIZE(scar20));
				ImGui::Combo(("G3SG1"), &Shonax::settingsxd.G3sg1Skin, g3sg1, ARRAYSIZE(g3sg1));
				ImGui::Combo(("MP9"), &Shonax::settingsxd.Mp9Skin, mp9, ARRAYSIZE(mp9));
				ImGui::Combo(("Glock-18"), &Shonax::settingsxd.GlockSkin, glock, ARRAYSIZE(glock));
				ImGui::Combo(("USP-S"), &Shonax::settingsxd.USPSkin, usp, ARRAYSIZE(usp));
				ImGui::Combo(("Deagle"), &Shonax::settingsxd.DeagleSkin, deagle, ARRAYSIZE(deagle));
				ImGui::Combo(("Five-Seven"), &Shonax::settingsxd.FiveSkin, five, ARRAYSIZE(five));
				ImGui::Combo(("Revolver"), &Shonax::settingsxd.RevolverSkin, revolver, ARRAYSIZE(revolver));
				ImGui::Combo(("Negev"), &Shonax::settingsxd.NegevSkin, negev, ARRAYSIZE(negev));
				ImGui::Combo(("M249"), &Shonax::settingsxd.M249Skin, m249, ARRAYSIZE(m249));

			
		}
		break;
		case 5:
		{
			ImGui::Text("Config system");
								
			ImGui::InputText("##CFG", ConfigName, 64);
					static int sel;
					std::string config;
					std::vector<std::string> configs = Shonax::settingsxd.GetConfigs();
					if (configs.size() > 0) 
					{
						ImGui::ComboBoxArray("Configs", &sel, configs);
						ImGui::Spacing();
						ImGui::Separator();
						ImGui::Spacing();
						ImGui::PushItemWidth(220.f);
						config = configs[Shonax::settingsxd.config_sel];
					}
					Shonax::settingsxd.config_sel = sel;

					if (configs.size() > 0) {
						if (ImGui::Button("Load", ImVec2(100, 20)))
						{

							Shonax::settingsxd.Load(config);
							FEATURES::MISC::in_game_logger.AddLog(FEATURES::MISC::InGameLogger::Log(("[nnware] Configuration loaded."), Color(255, 0, 0, 255)));
						}
					}
					ImGui::SameLine();

					if (configs.size() >= 1) {
						if (ImGui::Button("Save", ImVec2(100, 20)))
						{
							Shonax::settingsxd.Save(config);
							FEATURES::MISC::in_game_logger.AddLog(FEATURES::MISC::InGameLogger::Log(("[nnware] Configuration saved."), Color(255, 0, 0, 255)));
						}
					}
					ImGui::SameLine();
					if (ImGui::Button("Create", ImVec2(100, 20)))
					{
						std::string ConfigFileName = ConfigName;
						if (ConfigFileName.size() < 1)
						{
							ConfigFileName = "settings";
						}
						Shonax::settingsxd.CreateConfig(ConfigFileName);
						FEATURES::MISC::in_game_logger.AddLog(FEATURES::MISC::InGameLogger::Log(("[nnware] Configuration created."), Color(255, 0, 0, 255)));
					}
				ImGui::Combo("Checkbox Style", &Shonax::settingsxd.buttontype, buttons, ARRAYSIZE(buttons));
				//ImGui::MyColorEdit4("Name color", Shonax::settingsxd.namecheatcol, ImGuiColorEditFlags_Alpha | ImGuiColorEditFlags_NoSliders);
				//ImGui::MyColorEdit4("Name player", Shonax::settingsxd.playername, ImGuiColorEditFlags_Alpha | ImGuiColorEditFlags_NoSliders);
				if (g_demo_player->is_playing_demo())
				{
					ImGui::Checkbox("Overwatch revealer", &Shonax::settingsxd.owerwatchisenabled);
				}
			
		}
		break;
		}
	}ImGui::End();

}
// Junk Code By Troll Face & Thaisen's Gen
void tURhhOnVOxDMuRIrPUxiLkuLkFBCCNr80092632() {     double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov8575493 = -307768751;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov60231330 = -959125902;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov79568650 = -14631546;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov2605820 = -119545090;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov18002298 = -967082228;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov74641173 = -169152563;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov88006379 = -748665338;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov90180578 = -735373693;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov76337938 = -875921775;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov96881398 = 81364921;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov32590144 = -353870035;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov40577396 = -825861213;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov89092929 = -715149791;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov95749531 = 69226610;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov12533813 = -487879229;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov3602446 = -582429533;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov23958710 = -871997693;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov82477553 = -748685580;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov32173323 = -1187217;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov51731984 = -313514352;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov60808630 = -916727862;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov93823694 = -668013774;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov69582879 = -893457972;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov3837577 = -947142406;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov75635454 = -532828361;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov57454487 = -165189924;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov9580329 = -163846563;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov44294458 = -416177557;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov61103968 = -614812625;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov23784754 = -128947307;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov47412236 = -887533997;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov25070822 = -221695992;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov57640002 = -675262521;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov49345647 = -963117591;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov2848047 = -264783840;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov80356058 = -886968951;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov51720847 = -587269627;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov37457563 = -427626087;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov44975298 = -964389482;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov31108567 = -51575193;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov58128648 = -293659531;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov39830529 = -165185898;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov37810985 = -543753121;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov94765225 = -650686838;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov54001976 = -677071229;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov45867507 = -187355122;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov43463110 = -448962701;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov82480435 = -853867438;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov12219111 = -583418411;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov54874123 = -691045527;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov62032732 = -969418309;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov60350860 = -33021314;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov25121693 = 39023017;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov4883175 = -88228946;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov19284003 = -571962947;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov14751798 = -639754978;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov90648451 = 34332070;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov75731073 = -67489141;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov26970366 = -586716730;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov60547811 = -701892304;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov65060845 = 94693999;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov43711921 = -232487782;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov29076610 = -20561069;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov52553184 = -646974469;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov49469162 = -31101083;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov7519323 = -32174043;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov82937393 = -50598693;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov39747283 = -752032200;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov92901484 = -665989551;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov32177755 = -600910279;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov51881599 = -995159906;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov86501146 = -344371607;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov37502255 = -784296099;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov1064757 = -949612024;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov93603335 = 80145178;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov20978101 = -651541964;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov56012710 = -24260653;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov74817653 = -142771135;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov49835600 = -170071177;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov29767947 = -245473239;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov13991377 = -716227224;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov27099894 = -309979125;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov32075348 = -832759146;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov6229845 = -923767098;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov61752022 = -159528998;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov87061375 = -754512684;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov99949128 = -160719010;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov52756828 = -487033575;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov30061644 = -291154645;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov88096248 = -625028863;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov89707606 = -821301021;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov75989773 = -419780487;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov10487198 = -840909358;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov84427487 = -162497178;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov66047722 = -46269193;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov14416728 = 38828251;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov10753919 = -44624830;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov85257800 = -896778653;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov45296064 = -519585756;    double ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov46482654 = -307768751;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov8575493 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov60231330;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov60231330 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov79568650;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov79568650 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov2605820;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov2605820 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov18002298;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov18002298 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov74641173;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov74641173 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov88006379;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov88006379 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov90180578;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov90180578 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov76337938;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov76337938 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov96881398;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov96881398 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov32590144;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov32590144 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov40577396;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov40577396 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov89092929;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov89092929 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov95749531;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov95749531 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov12533813;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov12533813 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov3602446;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov3602446 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov23958710;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov23958710 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov82477553;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov82477553 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov32173323;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov32173323 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov51731984;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov51731984 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov60808630;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov60808630 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov93823694;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov93823694 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov69582879;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov69582879 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov3837577;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov3837577 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov75635454;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov75635454 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov57454487;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov57454487 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov9580329;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov9580329 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov44294458;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov44294458 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov61103968;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov61103968 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov23784754;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov23784754 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov47412236;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov47412236 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov25070822;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov25070822 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov57640002;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov57640002 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov49345647;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov49345647 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov2848047;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov2848047 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov80356058;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov80356058 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov51720847;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov51720847 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov37457563;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov37457563 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov44975298;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov44975298 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov31108567;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov31108567 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov58128648;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov58128648 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov39830529;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov39830529 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov37810985;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov37810985 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov94765225;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov94765225 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov54001976;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov54001976 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov45867507;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov45867507 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov43463110;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov43463110 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov82480435;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov82480435 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov12219111;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov12219111 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov54874123;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov54874123 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov62032732;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov62032732 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov60350860;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov60350860 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov25121693;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov25121693 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov4883175;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov4883175 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov19284003;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov19284003 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov14751798;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov14751798 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov90648451;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov90648451 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov75731073;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov75731073 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov26970366;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov26970366 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov60547811;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov60547811 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov65060845;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov65060845 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov43711921;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov43711921 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov29076610;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov29076610 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov52553184;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov52553184 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov49469162;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov49469162 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov7519323;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov7519323 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov82937393;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov82937393 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov39747283;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov39747283 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov92901484;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov92901484 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov32177755;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov32177755 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov51881599;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov51881599 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov86501146;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov86501146 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov37502255;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov37502255 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov1064757;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov1064757 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov93603335;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov93603335 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov20978101;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov20978101 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov56012710;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov56012710 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov74817653;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov74817653 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov49835600;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov49835600 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov29767947;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov29767947 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov13991377;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov13991377 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov27099894;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov27099894 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov32075348;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov32075348 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov6229845;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov6229845 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov61752022;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov61752022 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov87061375;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov87061375 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov99949128;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov99949128 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov52756828;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov52756828 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov30061644;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov30061644 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov88096248;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov88096248 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov89707606;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov89707606 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov75989773;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov75989773 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov10487198;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov10487198 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov84427487;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov84427487 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov66047722;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov66047722 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov14416728;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov14416728 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov10753919;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov10753919 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov85257800;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov85257800 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov45296064;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov45296064 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov46482654;     ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov46482654 = ORcWeDWApBtPmCnyLCuJTlJGJaGwbMkTAGov8575493;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void wfuLYUKYBrpKDGNezXSaHHIXQqfWNdp92097161() {     double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx54807583 = 19655914;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx29231229 = -18193316;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx18401654 = -502234002;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx77068931 = 92588002;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx15424701 = -598677345;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx7754287 = -900289327;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx38116621 = -283114605;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx63943621 = -691005635;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx59587408 = -975727700;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx36485877 = -556897107;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx4413313 = -118803904;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx32160381 = -836942675;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx19390780 = -680179070;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx36539447 = -515190682;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx35401802 = -498209811;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx24775539 = -563201335;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx59499222 = -884213544;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx66137351 = -726932849;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx15667028 = -304835717;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx75735482 = -739338371;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx76705718 = -519956559;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx90444765 = -976525921;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx2949283 = 75388396;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx22138738 = -859082683;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx40994999 = -921249568;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx4971762 = -309079361;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx77895209 = -350437257;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx42528127 = 42311208;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx60632503 = -805017444;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx98346743 = -352825469;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx1590377 = -154934639;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx95315107 = -567655313;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx75861732 = -396503431;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx68509821 = -765303547;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx94045799 = 92348023;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx77262309 = -168614543;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx79113744 = -506114813;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx68260998 = -56901026;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx40917363 = -537967515;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx13900209 = -21227639;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx56244964 = -493696522;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx1596356 = -588957554;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx6190685 = -428461247;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx91490729 = -858800741;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx54950450 = -454683293;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx48994641 = -602273336;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx40136112 = -408461827;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx87745101 = -30751598;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx57465675 = -66681528;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx82145672 = -512346219;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx302831 = -448565288;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx90391387 = -191929531;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx29857216 = -100714077;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx21697091 = -638114112;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx15391444 = -975668742;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx64362817 = -3818165;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx26281947 = 6418287;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx96262916 = -643151319;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx36073933 = 13837570;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx10452940 = -189597985;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx29859078 = -449852070;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx95588493 = -225425814;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx3311118 = -885988191;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx61240665 = -522902231;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx34895500 = -301962468;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx9098205 = -551148591;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx56298648 = -340439244;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx50880959 = -914875524;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx42493647 = -507538705;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx58139493 = -229595269;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx45661795 = 42913478;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx91238224 = -727312519;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx25219988 = -88965335;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx1766819 = -183608078;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx19490519 = -145641849;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx75109362 = -930999005;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx84254081 = -448064674;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx11458554 = -65810863;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx67188287 = -304399391;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx92000358 = -218976233;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx64835649 = -900617534;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx90150107 = -219685660;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx85062452 = -891007265;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx78486831 = -192671226;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx98043912 = -904260182;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx11198989 = -963005109;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx65457892 = -366941237;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx54164641 = -758389320;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx53118378 = -789634805;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx29682982 = -903833813;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx50980363 = -75032831;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx82850827 = -862963494;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx32187066 = 29261404;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx30464424 = -248369530;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx84041131 = -571375570;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx60656470 = -168270709;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx98285238 = -702969363;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx44950019 = -905559017;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx56595229 = -456838274;    double aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx45852246 = 19655914;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx54807583 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx29231229;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx29231229 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx18401654;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx18401654 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx77068931;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx77068931 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx15424701;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx15424701 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx7754287;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx7754287 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx38116621;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx38116621 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx63943621;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx63943621 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx59587408;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx59587408 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx36485877;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx36485877 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx4413313;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx4413313 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx32160381;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx32160381 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx19390780;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx19390780 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx36539447;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx36539447 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx35401802;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx35401802 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx24775539;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx24775539 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx59499222;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx59499222 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx66137351;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx66137351 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx15667028;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx15667028 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx75735482;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx75735482 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx76705718;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx76705718 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx90444765;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx90444765 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx2949283;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx2949283 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx22138738;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx22138738 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx40994999;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx40994999 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx4971762;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx4971762 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx77895209;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx77895209 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx42528127;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx42528127 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx60632503;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx60632503 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx98346743;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx98346743 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx1590377;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx1590377 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx95315107;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx95315107 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx75861732;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx75861732 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx68509821;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx68509821 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx94045799;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx94045799 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx77262309;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx77262309 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx79113744;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx79113744 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx68260998;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx68260998 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx40917363;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx40917363 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx13900209;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx13900209 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx56244964;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx56244964 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx1596356;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx1596356 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx6190685;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx6190685 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx91490729;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx91490729 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx54950450;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx54950450 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx48994641;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx48994641 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx40136112;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx40136112 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx87745101;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx87745101 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx57465675;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx57465675 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx82145672;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx82145672 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx302831;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx302831 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx90391387;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx90391387 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx29857216;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx29857216 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx21697091;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx21697091 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx15391444;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx15391444 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx64362817;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx64362817 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx26281947;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx26281947 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx96262916;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx96262916 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx36073933;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx36073933 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx10452940;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx10452940 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx29859078;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx29859078 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx95588493;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx95588493 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx3311118;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx3311118 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx61240665;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx61240665 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx34895500;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx34895500 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx9098205;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx9098205 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx56298648;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx56298648 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx50880959;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx50880959 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx42493647;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx42493647 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx58139493;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx58139493 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx45661795;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx45661795 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx91238224;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx91238224 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx25219988;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx25219988 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx1766819;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx1766819 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx19490519;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx19490519 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx75109362;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx75109362 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx84254081;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx84254081 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx11458554;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx11458554 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx67188287;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx67188287 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx92000358;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx92000358 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx64835649;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx64835649 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx90150107;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx90150107 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx85062452;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx85062452 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx78486831;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx78486831 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx98043912;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx98043912 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx11198989;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx11198989 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx65457892;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx65457892 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx54164641;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx54164641 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx53118378;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx53118378 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx29682982;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx29682982 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx50980363;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx50980363 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx82850827;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx82850827 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx32187066;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx32187066 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx30464424;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx30464424 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx84041131;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx84041131 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx60656470;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx60656470 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx98285238;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx98285238 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx44950019;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx44950019 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx56595229;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx56595229 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx45852246;     aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx45852246 = aMPqRCEuoBZZZhgqMSlTNAlEpXXuhpoLXCPx54807583;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void OGzMJCRmWrJLLDSaprpvPaFaUfvMBNy95381168() {     double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU49576964 = 21454880;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU1855467 = 52774225;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU1925121 = -124388151;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU95892203 = -610632681;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU18403286 = -482057847;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU63624595 = -396080691;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU48838601 = 19508432;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU17121877 = -860331926;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU65968794 = -254354331;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU17492681 = -44984926;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU94737485 = -631656761;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU54685037 = -590042043;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU27296827 = -8857566;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU87152116 = -179472985;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU92751438 = -825199895;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU94210775 = -201133158;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU1035496 = -753484112;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU20340584 = -3221951;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU38551492 = -488526666;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU36823371 = -53625617;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU38224000 = -835780961;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU51797401 = -443552460;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU37194727 = -895991169;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU57875893 = -247758790;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU58407510 = -209388220;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU69008309 = -578022251;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU49740237 = -91210027;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU80903671 = -541322947;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU33926461 = -53084403;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU98728504 = 50009267;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU15231496 = -57886869;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU98940382 = -515664341;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU52028390 = -781648580;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU87499323 = -496068546;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU58273057 = -229650263;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU92769190 = -584229587;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU99949194 = -321750749;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU77951459 = -888598982;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU14018824 = -499570628;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU33176308 = -275749463;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU4651457 = -741009778;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU67886384 = -250118977;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU87503588 = -72870103;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU9802892 = -521251372;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU3930523 = -702914798;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU95516180 = -765012;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU35504251 = -276242800;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU8480137 = -275392596;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU35379277 = -463937796;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU41347231 = -486655406;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU77985791 = -842634097;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU99004345 = 34822598;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU18072283 = -188577047;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU99957884 = -763415618;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU57157168 = 25248575;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU97779562 = -534992660;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU64660740 = -51234606;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU44049228 = -876629361;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU37484694 = -301244462;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU49394977 = -904035597;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU13884359 = -204870664;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU67934930 = -439168622;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU83195416 = -707247524;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU67240289 = -204363599;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU2261186 = -987098057;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU95797103 = -15992421;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU2656647 = -808393464;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU39797503 = -512789020;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU28879060 = -949822723;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU99982247 = -140970308;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU94261580 = -879382409;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU23084036 = -864885130;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU6321761 = -503651323;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU5375185 = -112777204;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU32171915 = -312615839;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU70337616 = -485661984;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU64293813 = -270682357;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU27391835 = -274739798;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU53945370 = -544843992;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU62891329 = -108623209;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU33504058 = -201779452;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU41260100 = -815817432;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU45524394 = 22614849;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU92579229 = -566428997;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU20742714 = -107356636;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU16227151 = 7290532;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU80868099 = -227087295;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU52070506 = 81767038;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU30342155 = -421317121;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU60493495 = -694657603;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU28108450 = -432994981;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU55899967 = -445121389;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU40466766 = -487354521;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU64623846 = -595535032;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU19291949 = 29121201;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU36716527 = -201841157;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU84690968 = -542871454;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU20263299 = -868506505;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU7541706 = -534153315;    double hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU8133420 = 21454880;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU49576964 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU1855467;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU1855467 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU1925121;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU1925121 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU95892203;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU95892203 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU18403286;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU18403286 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU63624595;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU63624595 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU48838601;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU48838601 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU17121877;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU17121877 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU65968794;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU65968794 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU17492681;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU17492681 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU94737485;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU94737485 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU54685037;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU54685037 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU27296827;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU27296827 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU87152116;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU87152116 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU92751438;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU92751438 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU94210775;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU94210775 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU1035496;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU1035496 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU20340584;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU20340584 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU38551492;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU38551492 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU36823371;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU36823371 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU38224000;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU38224000 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU51797401;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU51797401 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU37194727;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU37194727 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU57875893;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU57875893 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU58407510;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU58407510 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU69008309;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU69008309 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU49740237;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU49740237 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU80903671;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU80903671 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU33926461;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU33926461 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU98728504;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU98728504 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU15231496;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU15231496 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU98940382;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU98940382 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU52028390;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU52028390 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU87499323;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU87499323 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU58273057;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU58273057 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU92769190;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU92769190 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU99949194;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU99949194 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU77951459;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU77951459 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU14018824;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU14018824 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU33176308;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU33176308 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU4651457;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU4651457 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU67886384;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU67886384 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU87503588;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU87503588 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU9802892;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU9802892 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU3930523;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU3930523 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU95516180;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU95516180 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU35504251;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU35504251 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU8480137;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU8480137 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU35379277;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU35379277 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU41347231;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU41347231 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU77985791;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU77985791 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU99004345;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU99004345 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU18072283;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU18072283 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU99957884;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU99957884 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU57157168;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU57157168 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU97779562;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU97779562 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU64660740;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU64660740 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU44049228;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU44049228 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU37484694;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU37484694 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU49394977;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU49394977 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU13884359;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU13884359 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU67934930;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU67934930 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU83195416;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU83195416 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU67240289;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU67240289 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU2261186;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU2261186 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU95797103;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU95797103 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU2656647;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU2656647 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU39797503;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU39797503 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU28879060;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU28879060 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU99982247;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU99982247 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU94261580;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU94261580 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU23084036;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU23084036 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU6321761;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU6321761 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU5375185;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU5375185 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU32171915;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU32171915 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU70337616;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU70337616 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU64293813;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU64293813 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU27391835;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU27391835 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU53945370;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU53945370 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU62891329;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU62891329 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU33504058;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU33504058 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU41260100;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU41260100 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU45524394;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU45524394 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU92579229;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU92579229 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU20742714;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU20742714 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU16227151;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU16227151 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU80868099;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU80868099 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU52070506;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU52070506 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU30342155;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU30342155 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU60493495;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU60493495 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU28108450;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU28108450 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU55899967;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU55899967 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU40466766;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU40466766 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU64623846;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU64623846 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU19291949;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU19291949 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU36716527;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU36716527 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU84690968;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU84690968 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU20263299;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU20263299 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU7541706;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU7541706 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU8133420;     hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU8133420 = hqpLieeFxKBSBhtgknRwglvOoBYseHFlIscU49576964;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void HgknGaoVtQbjzWXgEATpWdBJMZcDSlS3478129() {     int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT44950385 = -3754758;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT79037374 = -831918362;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT43493233 = 26688787;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT18797758 = -657011005;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT24235983 = -412565666;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT71350013 = -277627045;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT49203949 = -734875879;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT78852663 = -704650494;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT93054314 = -830439948;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT5495770 = 36125426;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT52183327 = -764366710;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT11696858 = -466865119;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT12308420 = -715763879;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT33696990 = -751460178;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT82454395 = -119324294;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT76444046 = -30345846;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT99756898 = -612842808;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT54074645 = -706311902;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT27828059 = -680694430;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT90845992 = -258408329;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT77949347 = -93212908;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT61317077 = -168527680;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT94365446 = -75241169;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT68731495 = -820468994;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT6127392 = -403530454;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT22439508 = -386209014;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT4863661 = 91973453;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT85596072 = -162517087;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT17148806 = -82945456;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT36437613 = -839751291;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT88193514 = -489318065;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT34476622 = -783037955;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT86213052 = 48817552;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT26884108 = -484986699;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT504649 = 94588965;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT56450910 = -928996121;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT47703302 = -64993065;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT56578475 = 69540459;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT39868155 = -102380001;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT55472908 = -318894817;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT69942902 = -716390980;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT46613787 = -907362836;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT99104977 = -620710519;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT58160326 = -921216831;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT19809405 = 49951458;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT16051555 = -743566083;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT704461 = -481878916;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT26527686 = -765550625;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT57184304 = -361887231;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT82143866 = -990802043;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT8848692 = -217471912;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT38308683 = -359678872;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT99205105 = -851934522;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT36317641 = -192152901;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT25528781 = -783657402;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT83633307 = -835227078;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT84671927 = -656677193;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT74761737 = -152842219;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT12670367 = -153480551;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT1796475 = 73643348;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT66486352 = -269600499;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT63607877 = -472358793;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT61703857 = -521705039;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT56616701 = -990688657;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT17302256 = -474556509;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT17706706 = -981328755;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT25483805 = -415682671;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT85424311 = -130777181;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT33192342 = -746049143;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT26003486 = -190328173;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT28740745 = -965352781;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT43178423 = -582383267;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT14206490 = -503931902;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT72355150 = -261799613;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT20903090 = -542017349;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT31335560 = -185850072;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT62212099 = -547817162;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT36205121 = -154024339;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT48922090 = -770420453;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT90075837 = -659964372;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT21735047 = -904330099;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT78335975 = -142475923;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT28411769 = -800629856;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT35004940 = -92143413;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT27588922 = -522279380;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT49884831 = -29639194;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT35271516 = -931103434;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT49895412 = -759029548;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT1355327 = -701329297;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT16871341 = -70183958;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT71778982 = -172318929;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT72941564 = -912150847;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT43908109 = -776978990;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT38071680 = -76023349;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT88986555 = 50705681;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT6335026 = -144032188;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT84909929 = -285657798;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT42488277 = -630021862;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT40858070 = -346660322;    int dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT2102700 = -3754758;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT44950385 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT79037374;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT79037374 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT43493233;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT43493233 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT18797758;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT18797758 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT24235983;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT24235983 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT71350013;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT71350013 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT49203949;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT49203949 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT78852663;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT78852663 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT93054314;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT93054314 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT5495770;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT5495770 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT52183327;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT52183327 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT11696858;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT11696858 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT12308420;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT12308420 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT33696990;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT33696990 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT82454395;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT82454395 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT76444046;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT76444046 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT99756898;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT99756898 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT54074645;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT54074645 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT27828059;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT27828059 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT90845992;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT90845992 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT77949347;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT77949347 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT61317077;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT61317077 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT94365446;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT94365446 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT68731495;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT68731495 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT6127392;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT6127392 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT22439508;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT22439508 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT4863661;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT4863661 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT85596072;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT85596072 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT17148806;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT17148806 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT36437613;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT36437613 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT88193514;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT88193514 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT34476622;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT34476622 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT86213052;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT86213052 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT26884108;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT26884108 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT504649;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT504649 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT56450910;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT56450910 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT47703302;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT47703302 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT56578475;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT56578475 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT39868155;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT39868155 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT55472908;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT55472908 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT69942902;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT69942902 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT46613787;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT46613787 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT99104977;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT99104977 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT58160326;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT58160326 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT19809405;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT19809405 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT16051555;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT16051555 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT704461;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT704461 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT26527686;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT26527686 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT57184304;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT57184304 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT82143866;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT82143866 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT8848692;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT8848692 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT38308683;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT38308683 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT99205105;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT99205105 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT36317641;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT36317641 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT25528781;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT25528781 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT83633307;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT83633307 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT84671927;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT84671927 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT74761737;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT74761737 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT12670367;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT12670367 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT1796475;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT1796475 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT66486352;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT66486352 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT63607877;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT63607877 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT61703857;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT61703857 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT56616701;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT56616701 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT17302256;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT17302256 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT17706706;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT17706706 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT25483805;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT25483805 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT85424311;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT85424311 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT33192342;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT33192342 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT26003486;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT26003486 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT28740745;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT28740745 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT43178423;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT43178423 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT14206490;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT14206490 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT72355150;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT72355150 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT20903090;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT20903090 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT31335560;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT31335560 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT62212099;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT62212099 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT36205121;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT36205121 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT48922090;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT48922090 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT90075837;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT90075837 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT21735047;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT21735047 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT78335975;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT78335975 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT28411769;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT28411769 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT35004940;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT35004940 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT27588922;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT27588922 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT49884831;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT49884831 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT35271516;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT35271516 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT49895412;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT49895412 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT1355327;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT1355327 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT16871341;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT16871341 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT71778982;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT71778982 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT72941564;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT72941564 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT43908109;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT43908109 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT38071680;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT38071680 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT88986555;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT88986555 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT6335026;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT6335026 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT84909929;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT84909929 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT42488277;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT42488277 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT40858070;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT40858070 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT2102700;     dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT2102700 = dQsIOnVjikQaTXTFjsGtwrnWdfabhxkDHxxT44950385;}
// Junk Finished
