#include "Hooks.h"
#include "Fonts.h"
#include "GameFunctions.h"
#include "vmt.h"
#include "NewMenu.h"
#include "vmthook.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/dx9/imgui_dx9.h"
#include "ImGui/dx9/imgui_impl_dx9.h"
#include "Font.h"
#include "Protobuffs.h"
#include <intrin.h>
#include "fatalwin.h"
#include <d3dx9tex.h>
#include "Sana.h"
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

using GCRetrieveMessage = EGCResult(__thiscall*)(void*, uint32_t *punMsgType, void *pubDest, uint32_t cubDest, uint32_t *pcubMsgSize);
using GCSendMessage = EGCResult(__thiscall*)(void*, uint32_t unMsgType, const void* pubData, uint32_t cubData);

ImFont* Name;
CPlist plist = CPlist();
double PlistArray[102][200];

RecvVarProxyFn oRecvLowerBodyTarget = NULL;
IDirect3DTexture9 *tImage = nullptr;
namespace INIT
{
	HMODULE Dll;
	HWND Window;
	WNDPROC OldWindow;
}

static bool menu_open = false;
static bool d3d_init = false;
bool PressedKeys[256] = {};


DWORD xdsignature(const char* szModule, const char* szSignature)
{
	MODULEINFO modInfo;
	GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(szModule), &modInfo, sizeof(MODULEINFO));
	DWORD startAddress = (DWORD)modInfo.lpBaseOfDll;
	DWORD endAddress = startAddress + modInfo.SizeOfImage;
	const char* pat = szSignature;
	DWORD firstMatch = 0;
	for (DWORD pCur = startAddress; pCur < endAddress; pCur++) {
		if (!*pat) return firstMatch;
		if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
			if (!firstMatch) firstMatch = pCur;
			if (!pat[2]) return firstMatch;
			if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?') pat += 3;
			else pat += 2;
		}
		else {
			pat = szSignature;
			firstMatch = 0;
		}
	}
	return NULL;
}

typedef void(__thiscall* LockCursor)(void*);

namespace Hooks
{
	CHookedEvents hooked_events;
	Protobuffs ProtoFeatures;
	VFTableHook* g_pClientModeHook;
	VFTableHook* g_pMaterialSystemHook;
	VFTableHook* g_pModelRenderHook;
	VFTableHook* g_pPredictionHook;
	VFTableHook* g_pMatSurfaceHook;
	VFTableHook* g_pPanelHook;
	VFTableHook* g_pClientHook;
	VFTableHook* g_pEngineHook;
	VFTableHook* g_pGameEventHook;
	VFTableHook* g_pSurfaceHook;
	VFTableHook* g_pRenderViewHook;
	IViewRenderBeams* g_pViewRenderBeams;
	vfunc_hook directz;
	vfunc_hook gc_hook;
	vfunc_hook demoplayback;
	GOO::Hook::VTable cursor;

	CreateMove_t g_fnOriginalCreateMove;
	PaintTraverse_t g_fnOriginalPaintTraverse;
	DrawModelExecute_t g_fnOriginalDrawModelExecute;
	FrameStageNotify_t g_fnOriginalFrameStageNotify;
	FireEventClientSide_t g_fnOriginalFireEventClientSide;
	OverrideView_t g_fnOriginalOverrideView;
	IsPlayingTimeDemo_t g_fnOriginalPlayingTimeDemo;
	PlaySound_t g_fnOriginalPlaySound;
	LockCursor olock;
	DoPostScreenEffects_t g_fnOriginalDoPostScreenEffects;
	SceneEnd_t g_fnOriginalSceneEnd;


	void Hook_LockCursor(void* xd)
	{
		cursor.UnHook();
		m_pSurface->lockcursor();
		cursor.ReHook();
		if (menu_open)
			m_pSurface->unlockcursor();
	}

	void OpenMenu()
	{
		static bool is_down = false;
		static bool is_clicked = false;
		if (GetAsyncKeyState(VK_INSERT))
		{
			is_clicked = false;
			is_down = true;
		}
		else if (!GetAsyncKeyState(VK_INSERT) && is_down)
		{
			is_clicked = true;
			is_down = false;
		}
		else
		{
			is_clicked = false;
			is_down = false;
		}

		if (is_clicked)
		{
			menu_open = !menu_open;

		}
	}

	LRESULT __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg) {
		case WM_LBUTTONDOWN:
			PressedKeys[VK_LBUTTON] = true;
			break;
		case WM_LBUTTONUP:
			PressedKeys[VK_LBUTTON] = false;
			break;
		case WM_RBUTTONDOWN:
			PressedKeys[VK_RBUTTON] = true;
			break;
		case WM_RBUTTONUP:
			PressedKeys[VK_RBUTTON] = false;
			break;
		case WM_MBUTTONDOWN:
			PressedKeys[VK_MBUTTON] = true;
			break;
		case WM_MBUTTONUP:
			PressedKeys[VK_MBUTTON] = false;
			break;
		case WM_XBUTTONDOWN:
		{
			UINT button = GET_XBUTTON_WPARAM(wParam);
			if (button == XBUTTON1)
			{
				PressedKeys[VK_XBUTTON1] = true;
			}
			else if (button == XBUTTON2)
			{
				PressedKeys[VK_XBUTTON2] = true;
			}
			break;
		}
		case WM_XBUTTONUP:
		{
			UINT button = GET_XBUTTON_WPARAM(wParam);
			if (button == XBUTTON1)
			{
				PressedKeys[VK_XBUTTON1] = false;
			}
			else if (button == XBUTTON2)
			{
				PressedKeys[VK_XBUTTON2] = false;
			}
			break;
		}
		case WM_KEYDOWN:
			PressedKeys[wParam] = true;
			break;
		case WM_KEYUP:
			PressedKeys[wParam] = false;
			break;
		default: break;
		}

		OpenMenu();

		if (d3d_init && menu_open && ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true;

		return CallWindowProc(INIT::OldWindow, hWnd, uMsg, wParam, lParam);
	}

	void GUI_Init(IDirect3DDevice9* pDevice)
	{
		ImGui_ImplDX9_Init(INIT::Window, pDevice);


		


		ImGuiIO& io = ImGui::GetIO();



		
		Fontgui::Tabs =  io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Calibri.ttf", 18.0f);

		

		
		Fontgui::Text = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\tahoma.ttf", 12.0f);

		
		Fontgui::Name = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\square_sans_serif.ttf", 16.0f);



		Fontgui::wep = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\undefeated.ttf", 18.0f);
		


			ImGuiStyle &style = ImGui::GetStyle();
			style.Alpha = 1.0f;
			style.WindowPadding = ImVec2(8, 8);
			style.WindowMinSize = ImVec2(32, 32);
			style.WindowRounding = 0.0f;
			style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
			style.ChildWindowRounding = 0.0f;
			style.FramePadding = ImVec2(4, 1);
			style.FrameRounding = 0.0f;
			style.ItemSpacing = ImVec2(15, 4);
			style.ItemInnerSpacing = ImVec2(4, 4);
			style.TouchExtraPadding = ImVec2(0, 0);
			style.IndentSpacing = 21.0f;
			style.ColumnsMinSpacing = 3.0f;
			style.ScrollbarSize = 12.0f;
			style.ScrollbarRounding = 0.0f;
			style.GrabMinSize = 0.1f;
			style.GrabRounding = 16.0f;
			style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
			style.DisplayWindowPadding = ImVec2(22, 22);
			style.DisplaySafeAreaPadding = ImVec2(4, 4);
			style.AntiAliasedLines = true;
			style.AntiAliasedShapes = true;
			style.CurveTessellationTol = 1.25f;


			ImVec4 Color_Title_Text = ImColor(165, 75, 100);

			int MenuRed, MenuGreen, MenuBlue;

			ImColor mainColor = ImColor(54, 54, 54, 255);
			ImColor bodyColor = ImColor(54, 54, 54, 255);
			ImColor fontColor = ImColor(255, 255, 255, 255);

			ImVec4 mainColorHovered = ImVec4(mainColor.Value.x + 0.1f, mainColor.Value.y + 0.1f, mainColor.Value.z + 0.1f, mainColor.Value.w);
			ImVec4 mainColorActive = ImVec4(mainColor.Value.x + 0.2f, mainColor.Value.y + 0.2f, mainColor.Value.z + 0.2f, mainColor.Value.w);
			ImVec4 menubarColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w - 0.8f);
			ImVec4 frameBgColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w + .1f);
			ImVec4 tooltipBgColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w + .05f);

			style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f); //white
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.85f, 0.85f, 0.85f, 0.85f); //main quarter
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.70f); //main quarter
			style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.07f, 0.60f); //main bg
			style.Colors[ImGuiCol_Border] = ImVec4(0.14f, 0.16f, 0.19f, 0.60f); //main border
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f); //dark
			style.Colors[ImGuiCol_FrameBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f); //main bg
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(1.00f, 0.30f, 0.10f, 0.50f); //main colored
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(1.00f, 0.30f, 0.10f, 1.00f); //main colored
			style.Colors[ImGuiCol_TitleBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f); //main bg
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.24f, 0.28f, 0.75f); //collapsed
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f); //main bg
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.11f, 0.11f, 0.11f, 0.70f); //main bg
			style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f); //main bg
			style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f); //main half
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.50f, 0.50f, 0.50f, 0.70f); //main half
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.00f, 0.30f, 0.10f, 1.00f); //main colored
			style.Colors[ImGuiCol_ComboBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f); //main bg
			style.Colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.30f, 0.10f, 1.00f); //main colored
			style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f); //main half
			style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.30f, 0.10f, 1.00f); //main colored
			style.Colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f); //main
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f); //main
			style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.30f, 0.10f, 1.00f); //main colored
			style.Colors[ImGuiCol_Header] = ImVec4(1.00f, 0.30f, 0.10f, 1.00f); //main colored
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f); //main
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f); //main
			style.Colors[ImGuiCol_Column] = ImVec4(0.14f, 0.16f, 0.19f, 1.00f); //main border  
			style.Colors[ImGuiCol_ColumnHovered] = ImVec4(1.00f, 0.30f, 0.10f, 0.50f); //main colored
			style.Colors[ImGuiCol_ColumnActive] = ImVec4(1.00f, 0.30f, 0.10f, 1.00f); //main colored
			style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.90f, 0.90f, 0.90f, 0.75f); //main white
			style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f); //main white
			style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f); //main white
			style.Colors[ImGuiCol_CloseButton] = ImVec4(0.86f, 0.93f, 0.89f, 0.00f); //dark
			style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.86f, 0.93f, 0.89f, 0.40f); //close button
			style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.86f, 0.93f, 0.89f, 0.90f); //close button
			style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.30f, 0.10f, 1.00f); //main colored
			style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.30f, 0.10f, 0.50f); //main colored
			style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.30f, 0.10f, 1.00f); //main colored
			style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.30f, 0.10f, 0.50f); //main colored
			style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.30f, 0.10f, 1.00f); //main colored
			style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.07f, 0.70f); //main bg
			style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.24f, 0.28f, 0.60f); //collapsed

			style.Alpha = 1.f;
			style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
			style.FramePadding = ImVec2(1, 1);
			style.ScrollbarSize = 10.f;
			style.ScrollbarRounding = 0.f;
			style.GrabMinSize = 5.f;

			d3d_init = true;
	}

	static bool _xd = true;
	long __stdcall Hooked_EndScene(IDirect3DDevice9* pDevice)
	{
		static auto ofunc = directz.get_original<EndSceneFn>(42);
		ImGuiStyle& style = ImGui::GetStyle();
		if (menu_open)
		{
			D3DCOLOR rectColor = D3DCOLOR_XRGB(255, 0, 0);
			D3DRECT BarRect = { 1, 1, 1, 1 };
			
			pDevice->Clear(1, &BarRect, D3DCLEAR_TARGET | D3DCLEAR_TARGET, rectColor, 0, 0);
			pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
			pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
			pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
		}
		

		if (Shonax::settingsxd.spreadcrosshair && Shonax::settingsxd.spreadcrosshairtype == 1)
		{
			drawfatalpricel(pDevice);
		}

		if (!d3d_init)
		{
			GUI_Init(pDevice);
		}
		ImGui::GetIO().MouseDrawCursor = menu_open;

		ImGui_ImplDX9_NewFrame();
		POINT mp;

		GetCursorPos(&mp);

		ImGuiIO& io = ImGui::GetIO();

		io.MousePos.x = mp.x;
		io.MousePos.y = mp.y;


		//if (tImage == nullptr)D3DXCreateTextureFromFileInMemoryEx(pDevice, &NameArry, sizeof(NameArry), 564, 845, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &tImage);


		if (menu_open)
		{
			ImGui::SetNextWindowSize(ImVec2(650, 569));
			ImGui::Begin("nnware RECODE", &menu_open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
			{
				DrawMenu();
			}
			ImGui::End();
		}
		/*
		ImGui::SetNextWindowSize(ImVec2(564, 845));
		int screen_width, screen_height;
		m_pEngine->GetScreenSize(screen_width, screen_height);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.0f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		ImGui::Begin("##PharxdamcyTabsxdxd", &_xd, ImVec2(screen_width - 564, screen_height), -1.0f, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
		{
			ImGui::Image(tImage, ImVec2(564, 845));
		}
		ImGui::End();
		*/



		ImGui::Render();

		return ofunc(pDevice);
	}

	long __stdcall Hooked_EndScene_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		static auto ofunc = directz.get_original<EndSceneResetFn>(16);
		//	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DRS_COLORWRITEENABLE);
		//	pDevice->Release();

		if (!d3d_init)
			return ofunc(pDevice, pPresentationParameters);

		ImGui_ImplDX9_InvalidateDeviceObjects();

		auto hr = ofunc(pDevice, pPresentationParameters);

		ImGui_ImplDX9_CreateDeviceObjects();

		return hr;
	}



	EGCResult __fastcall hkGCRetrieveMessage(void* ecx, void*, uint32_t *punMsgType, void *pubDest, uint32_t cubDest, uint32_t *pcubMsgSize)
	{
		static auto oGCRetrieveMessage = gc_hook.get_original<GCRetrieveMessage>(2);
		auto status = oGCRetrieveMessage(ecx, punMsgType, pubDest, cubDest, pcubMsgSize);

		if (status == k_EGCResultOK)
		{

			void* thisPtr = nullptr;
			__asm mov thisPtr, ebx;
			auto oldEBP = *reinterpret_cast<void**>((uint32_t)_AddressOfReturnAddress() - 4);

			uint32_t messageType = *punMsgType & 0x7FFFFFFF;
			ProtoFeatures.ReceiveMessage(thisPtr, oldEBP, messageType, pubDest, cubDest, pcubMsgSize);
		}

		return status;
	}

	EGCResult __fastcall hkGCSendMessage(void* ecx, void*, uint32_t unMsgType, const void* pubData, uint32_t cubData)
	{
		static auto oGCSendMessage = gc_hook.get_original<GCSendMessage>(0);

		EGCResult status;

		bool sendMessage = ProtoFeatures.PreSendMessage(unMsgType, const_cast<void*>(pubData), cubData);

		if (!sendMessage)
			return k_EGCResultOK;

		return oGCSendMessage(ecx, unMsgType, const_cast<void*>(pubData), cubData);
	}



	void* __fastcall hk_read_packet(i_demo_player* _this, void* edx) {
		using read_packet_t = void*(__thiscall*)(i_demo_player*);
		static auto orig_fn = demoplayback.get_original< read_packet_t >(17);
		
		if (!Shonax::settingsxd.owerwatchisenabled)
			return orig_fn(_this);

		g_demo_player->set_overwatch_state(true);

		auto ret = orig_fn(_this);

		g_demo_player->set_overwatch_state(false);

		return ret;
	}

	void __fastcall hk_hud_update(IBaseClientDLL* _this, void* edx, bool active) {
		using hud_update_t = void(__thiscall*)(IBaseClientDLL*, bool);
		static auto orig_fn = demoplayback.get_original< hud_update_t >(11);

		// without this check the game will crash when joining a normal match
		if (Shonax::settingsxd.owerwatchisenabled && g_demo_player->is_playing_demo()) {
			g_demo_player->set_overwatch_state(true);

			orig_fn(_this, active);

			g_demo_player->set_overwatch_state(false);
		}
		else {
			orig_fn(_this, active);
		}
	}


	void Initialise()
	{
		g_pClientModeHook = new VFTableHook(m_pClientMode);
		g_pModelRenderHook = new VFTableHook(m_pModelRender);
		g_pMaterialSystemHook = new VFTableHook(m_pMaterialSystem);
		g_pPredictionHook = new VFTableHook(m_pPrediction);
		g_pPanelHook = new VFTableHook(m_pPanels);
		g_pClientHook = new VFTableHook(m_pClient);
		g_pEngineHook = new VFTableHook(m_pEngine);
		g_pGameEventHook = new VFTableHook(m_pGameEventManager);
		g_pSurfaceHook = new VFTableHook(m_pSurface);
		g_pRenderViewHook = new VFTableHook(m_pRenderView);
	
		


		

		enum FontFlags
		{
			FONTFLAG_NONE,
			FONTFLAG_ITALIC = 0x001,
			FONTFLAG_UNDERLINE = 0x002,
			FONTFLAG_STRIKEOUT = 0x004,
			FONTFLAG_SYMBOL = 0x008,
			FONTFLAG_ANTIALIAS = 0x010,
			FONTFLAG_GAUSSIANBLUR = 0x020,
			FONTFLAG_ROTARY = 0x040,
			FONTFLAG_DROPSHADOW = 0x080,
			FONTFLAG_ADDITIVE = 0x100,
			FONTFLAG_OUTLINE = 0x200,
			FONTFLAG_CUSTOM = 0x400,
			FONTFLAG_BITMAP = 0x800,
		};
		

		draw.fonts.menu = m_pSurface->FontCreate();
		m_pSurface->SetFontGlyphSet(draw.fonts.menu, XorStr("Verdana"), 12, 400, 0, 0, (int)FONTFLAG_ANTIALIAS);

		draw.fonts.menu_bold = m_pSurface->FontCreate();
		m_pSurface->SetFontGlyphSet(draw.fonts.menu_bold, XorStr("Verdana"), 12, 700, 0, 0, (int)FONTFLAG_ANTIALIAS | (int)FONTFLAG_OUTLINE | (int)FONTFLAG_DROPSHADOW);

		draw.fonts.Legitbot = m_pSurface->FontCreate();
		m_pSurface->SetFontGlyphSet(draw.fonts.Legitbot, XorStr("Undefeated"), 12, 400, 0, 0, (int)FONTFLAG_ANTIALIAS);

		draw.fonts.font_icons = m_pSurface->FontCreate();
		m_pSurface->SetFontGlyphSet(draw.fonts.font_icons, XorStr("Untitled1"), 50, 400, 0, 0, (int)FONTFLAG_ANTIALIAS | (int)FONTFLAG_DROPSHADOW);

		draw.fonts.esp = m_pSurface->FontCreate( );
		m_pSurface->SetFontGlyphSet(draw.fonts.esp, XorStr( "Arial" ), 11, 700, 0, 0, FONTFLAG_OUTLINE );

		draw.fonts.esp_extra = m_pSurface->FontCreate();
		m_pSurface->SetFontGlyphSet(draw.fonts.esp_extra, XorStr("Arial"), 10, 700, 0, 0, (int)FONTFLAG_OUTLINE );

		draw.fonts.esp_small = m_pSurface->FontCreate();
		m_pSurface->SetFontGlyphSet(draw.fonts.esp_small, XorStr("Verdana"), 11, 230, 0, 0, FONTFLAG_OUTLINE);

		draw.fonts.indicator = m_pSurface->FontCreate();
		m_pSurface->SetFontGlyphSet(draw.fonts.indicator, XorStr("Verdana"), 24, 700, 0, 0, FONTFLAG_OUTLINE | FONTFLAG_DROPSHADOW);

		draw.fonts.aaindicator = m_pSurface->FontCreate();
		m_pSurface->SetFontGlyphSet(draw.fonts.aaindicator, XorStr("Arial"), 60, 500, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);


		draw.fonts.in_game_logging_font = m_pSurface->FontCreate();
		m_pSurface->SetFontGlyphSet(draw.fonts.in_game_logging_font, XorStr("Quantum"), 20, 500, 0, 0, FONTFLAG_OUTLINE);


		draw.fonts.visuals_grenade_pred_font = m_pSurface->FontCreate();
		m_pSurface->SetFontGlyphSet(draw.fonts.visuals_grenade_pred_font, XorStr("AstriumWep"), 16, 400, 0, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);


		draw.fonts.in_game_logging_font_1 = m_pSurface->FontCreate();
		m_pSurface->SetFontGlyphSet(draw.fonts.in_game_logging_font_1, XorStr("font"), 20, 500, 0, 0, FONTFLAG_OUTLINE);

		draw.fonts.in_game_logging_font_2 = m_pSurface->FontCreate();
		m_pSurface->SetFontGlyphSet(draw.fonts.in_game_logging_font_2, XorStr("12082"), 20, 500, 0, 0, FONTFLAG_OUTLINE);

		draw.fonts.in_game_logging_font_3 = m_pSurface->FontCreate();
		m_pSurface->SetFontGlyphSet(draw.fonts.in_game_logging_font_3, XorStr("11676"), 20, 500, 0, 0, FONTFLAG_OUTLINE);


		draw.fonts.in_game_logging_font_4 = m_pSurface->FontCreate();
		m_pSurface->SetFontGlyphSet(draw.fonts.in_game_logging_font_4, XorStr("ANVYL"), 20, 500, 0, 0, FONTFLAG_OUTLINE);

		
		chams = new Chams;

	
		g_fnOriginalPaintTraverse = g_pPanelHook->Hook(41, (PaintTraverse_t)Hooked_PaintTraverse);
		g_fnOriginalCreateMove = g_pClientModeHook->Hook(24, (CreateMove_t)Hooked_CreateMove);
		g_fnOriginalOverrideView = g_pClientModeHook->Hook(18, (OverrideView_t)Hooked_OverrideView);
		g_fnOriginalDoPostScreenEffects = g_pClientModeHook->Hook(44, (DoPostScreenEffects_t)Hooked_DoPostScreenEffects);
		g_fnOriginalFireEventClientSide = g_pGameEventHook->Hook(9, (FireEventClientSide_t)Hooked_FireEventClientSide);
		g_fnOriginalFrameStageNotify = g_pClientHook->Hook(37, (FrameStageNotify_t)Hooked_FrameStageNotify);
		g_fnOriginalPlayingTimeDemo = g_pEngineHook->Hook(85, (IsPlayingTimeDemo_t)Hooked_IsPlayingTimeDemo);
		g_fnOriginalPlaySound = g_pSurfaceHook->Hook(82, (PlaySound_t)Hooked_PlaySound);

		

		cursor.InitTable(m_pSurface);
		cursor.HookIndex(67, Hook_LockCursor);

		g_fnOriginalSceneEnd = g_pRenderViewHook->Hook(9, (SceneEnd_t)Hooked_SceneEnd);
		g_pModelRenderHook->Hook(21, (DrawModelExecute_t)Hooked_DrawModelExecute);
		g_pClientModeHook->Hook(35, (GetViewmodelFov_t)Hooked_GetViewmodelFov);
		g_pMaterialSystemHook->Hook(84, Hooked_FindMaterial);
		g_pPredictionHook->Hook(19, (RunCommand_t)Hooked_RunCommand);



		demoplayback.setup(g_demo_player);
		demoplayback.hook_index(17, hk_read_packet);
		demoplayback.hook_index(11, hk_hud_update);




		gc_hook.setup(g_SteamGameCoordinator);
		gc_hook.hook_index(0, hkGCSendMessage);
		gc_hook.hook_index(2, hkGCRetrieveMessage);
		ProtoFeatures.SendClientHello();
		ProtoFeatures.SendMatchmakingClient2GCHello();


		while (!(INIT::Window = FindWindowA("Valve001", nullptr)))
			Sleep(100);
		if (INIT::Window)
			INIT::OldWindow = (WNDPROC)SetWindowLongPtr(INIT::Window, GWL_WNDPROC, (LONG_PTR)Hooked_WndProc);
		DWORD DeviceStructureAddress = **(DWORD**)(xdsignature("shaderapidx9.dll", "A1 ?? ?? ?? ?? 50 8B 08 FF 51 0C") + 1);
		if (DeviceStructureAddress) {
			directz.setup((DWORD**)DeviceStructureAddress);
			directz.hook_index(16, Hooked_EndScene_Reset);
			directz.hook_index(42, Hooked_EndScene);

		}



		initialize_events();



		for (ClientClass* pClass = m_pClient->GetAllClasses(); pClass; pClass = pClass->m_pNext) {
			if (!strcmp(pClass->m_pNetworkName, "DT_CSPlayer")) {
				RecvTable* pClassTable = pClass->m_pRecvTable;

				for (int nIndex = 0; nIndex < pClassTable->m_nProps; nIndex++) {
					RecvProp* pProp = &pClassTable->m_pProps[nIndex];

					if (!pProp || strcmp(pProp->m_pVarName, "m_flLowerBodyYawTarget"))
						continue;

					pProp->m_ProxyFn = (RecvVarProxyFn)LowerbodyProxy;

					break;
				}

				break;
			}
		}
	}
	void UndoHooks()
	{
	}	
}

// Junk Code By Troll Face & Thaisen's Gen
void DWgzmBlXqWJTxRJCGfYVcFAaGyQfVYq21529383() {     double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH52180791 = -483379440;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH1488966 = -334607677;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH96020553 = -74734009;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH44905372 = -987579159;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH73456771 = -288262826;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH61687496 = -755549109;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH19539334 = -676963420;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH44124535 = -439158894;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH49320574 = -548454246;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH42931924 = -200792981;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH73926604 = -286395045;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH78175062 = 43935604;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH11984507 = -493673081;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH9398547 = -400854054;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH12173564 = 24935473;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH83031048 = -115182751;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH10045991 = -445014102;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH71448443 = -795348334;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH11700076 = -200948186;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH18423177 = -166898396;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH40638958 = -598686323;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH73873851 = -985582571;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH59422330 = -217695486;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH92496538 = -737200030;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH54798997 = -259280862;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH45938684 = -297025807;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH46427956 = -472193351;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH19468877 = 68479748;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH5965054 = -944221584;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH41431664 = -232862073;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH83547408 = -846098931;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH69592374 = -330291594;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH5980134 = 37178818;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH60878170 = -704910915;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH81907290 = -24596381;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH43451048 = -187628835;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH61324159 = -442228296;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH26084972 = -466631711;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH916005 = -396634547;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH60944425 = -856724692;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH24376911 = -402908508;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH32403716 = -814478251;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH60440090 = -671818962;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH12258932 = -919341828;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH35002244 = -574001099;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH19160435 = -928707070;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH11596549 = -870525330;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH11606348 = -103381387;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH69851683 = -117558389;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH86410269 = -282283517;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH6183828 = -187614906;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH99899401 = -167965082;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH1741206 = -120988844;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH35095880 = -151483038;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH80890425 = 36034477;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH78306939 = -497796870;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH42066636 = -16912192;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH3524015 = -337533979;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH90106375 = -628298298;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH27518088 = -991237020;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH15259541 = -183355759;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH70458 = -645443169;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH38159481 = -494937311;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH7888911 = -215592174;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH59384516 = -354694050;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH4334231 = -956103452;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH72194929 = -993243214;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH51106337 = -788762166;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH27491257 = -276257674;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH68722516 = -787435692;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH21706890 = -672954455;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH83961019 = -978382392;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH70532439 = -298713788;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH50755650 = -344223494;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH94046265 = -763989888;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH8235242 = -784208072;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH13433762 = -213763609;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH47163399 = -298353658;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH57494294 = -63198932;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH35638562 = -330573792;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH34342136 = -426500477;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH34821609 = -268811964;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH49617194 = -813961863;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH19554785 = -561938067;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH35247837 = 54752833;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH83648006 = -578133849;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH67851169 = -109302750;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH70884254 = -811338145;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH79987744 = -640945393;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH3600351 = -526799511;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH1384413 = -70716644;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH57800144 = -4694318;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH35978596 = -838333414;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH73397917 = -405397528;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH45684885 = -573368933;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH24306454 = -757465340;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH94244235 = -219540941;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH52551179 = -356226788;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH52874415 = -464647778;    double WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH30668014 = -483379440;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH52180791 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH1488966;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH1488966 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH96020553;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH96020553 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH44905372;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH44905372 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH73456771;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH73456771 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH61687496;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH61687496 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH19539334;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH19539334 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH44124535;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH44124535 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH49320574;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH49320574 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH42931924;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH42931924 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH73926604;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH73926604 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH78175062;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH78175062 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH11984507;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH11984507 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH9398547;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH9398547 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH12173564;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH12173564 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH83031048;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH83031048 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH10045991;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH10045991 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH71448443;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH71448443 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH11700076;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH11700076 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH18423177;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH18423177 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH40638958;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH40638958 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH73873851;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH73873851 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH59422330;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH59422330 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH92496538;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH92496538 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH54798997;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH54798997 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH45938684;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH45938684 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH46427956;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH46427956 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH19468877;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH19468877 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH5965054;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH5965054 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH41431664;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH41431664 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH83547408;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH83547408 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH69592374;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH69592374 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH5980134;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH5980134 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH60878170;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH60878170 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH81907290;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH81907290 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH43451048;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH43451048 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH61324159;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH61324159 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH26084972;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH26084972 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH916005;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH916005 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH60944425;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH60944425 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH24376911;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH24376911 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH32403716;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH32403716 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH60440090;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH60440090 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH12258932;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH12258932 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH35002244;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH35002244 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH19160435;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH19160435 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH11596549;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH11596549 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH11606348;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH11606348 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH69851683;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH69851683 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH86410269;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH86410269 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH6183828;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH6183828 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH99899401;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH99899401 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH1741206;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH1741206 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH35095880;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH35095880 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH80890425;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH80890425 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH78306939;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH78306939 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH42066636;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH42066636 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH3524015;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH3524015 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH90106375;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH90106375 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH27518088;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH27518088 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH15259541;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH15259541 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH70458;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH70458 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH38159481;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH38159481 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH7888911;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH7888911 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH59384516;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH59384516 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH4334231;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH4334231 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH72194929;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH72194929 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH51106337;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH51106337 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH27491257;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH27491257 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH68722516;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH68722516 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH21706890;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH21706890 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH83961019;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH83961019 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH70532439;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH70532439 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH50755650;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH50755650 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH94046265;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH94046265 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH8235242;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH8235242 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH13433762;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH13433762 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH47163399;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH47163399 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH57494294;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH57494294 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH35638562;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH35638562 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH34342136;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH34342136 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH34821609;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH34821609 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH49617194;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH49617194 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH19554785;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH19554785 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH35247837;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH35247837 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH83648006;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH83648006 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH67851169;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH67851169 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH70884254;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH70884254 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH79987744;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH79987744 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH3600351;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH3600351 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH1384413;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH1384413 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH57800144;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH57800144 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH35978596;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH35978596 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH73397917;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH73397917 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH45684885;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH45684885 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH24306454;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH24306454 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH94244235;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH94244235 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH52551179;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH52551179 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH52874415;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH52874415 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH30668014;     WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH30668014 = WMXjvXPzFidEkMLCFQTvvfhTyWFtokGCSfmH52180791;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void vdYvtAlxWCPLzsxNcgRaYhqtZHygKtN49565742() {     double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi12945824 = -905333054;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi30432181 = -666367396;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi74108595 = -568080157;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi28494955 = 95028969;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi25644033 = -949703258;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi2084529 = -848628992;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi69676340 = -822357536;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi92273579 = -930778357;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi84477644 = -76986353;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi63433082 = -445376599;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi32968795 = -748661276;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi87054495 = -322373049;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi93863854 = -758762949;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi39352875 = -890349251;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi83312172 = -535361177;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi88868525 = -51137509;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi59566517 = -254773613;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi59098716 = -921507066;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi95178787 = -410511097;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi88681659 = -381191913;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi43035963 = -732723303;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi89943729 = -469948278;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi68361350 = 90085765;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi21567390 = -539466357;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi22693953 = -332084200;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi28475383 = -87595843;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi17099240 = -128499546;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi21228528 = -614468054;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi24673433 = 7080522;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi80572579 = -16522282;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi60908165 = -595385629;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi82918463 = -843056699;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi68799381 = -613896383;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi50647464 = -13255223;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi60244137 = -40506670;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi42331693 = -729416286;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi29231949 = -345944165;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi43070103 = -107329421;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi60609503 = -385188062;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi44305651 = -597904200;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi73861203 = -668676459;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi86926492 = -785944698;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi89790610 = -52259118;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi67892969 = 30671125;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi90956825 = -841676253;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi26861200 = -794757472;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi73546627 = -397638874;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi18374178 = -699690648;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi40528568 = -766789428;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi64209007 = -196338506;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi30257415 = -365679087;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi14638528 = -865903133;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi57166014 = -644747897;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi98730787 = -899759518;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi6529780 = -817305264;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi23002094 = -335384777;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi62070831 = -656453162;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi52541206 = 71386199;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi5801003 = -572886831;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi97168649 = -762107416;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi84985288 = -620129447;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi48447812 = -107889483;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi67600147 = -837858879;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi3905065 = 39535929;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi2524918 = -849990970;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi50050331 = -905604578;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi18255114 = -708476667;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi43216390 = -645507726;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi79108737 = -749842581;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi40980480 = -805944891;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi59636576 = -705193345;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi16496415 = -47444193;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi98489213 = -436319005;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi50873137 = -812606898;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi14820457 = -712515454;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi56109470 = -946778605;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi153119 = -317689160;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi468381 = -940585361;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi30610565 = -697790105;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi95832752 = -537326729;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi54928756 = -689956970;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi98725061 = -428808898;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi80699959 = -847678627;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi60464425 = -796580973;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi50315164 = -650843196;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi46269637 = -729482496;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi25752450 = -98308802;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi70068593 = -714136865;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi44117684 = -195949959;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi37242043 = -705121894;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi80260862 = 27036875;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi76690743 = -317330365;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi37269100 = -534442590;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi63440854 = -623080647;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi59320362 = -977774753;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi25413391 = -460786977;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi19326346 = -948085820;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi85885546 = 8204953;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi65368052 = -119337905;    double BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi40906494 = -905333054;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi12945824 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi30432181;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi30432181 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi74108595;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi74108595 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi28494955;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi28494955 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi25644033;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi25644033 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi2084529;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi2084529 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi69676340;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi69676340 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi92273579;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi92273579 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi84477644;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi84477644 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi63433082;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi63433082 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi32968795;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi32968795 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi87054495;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi87054495 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi93863854;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi93863854 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi39352875;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi39352875 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi83312172;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi83312172 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi88868525;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi88868525 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi59566517;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi59566517 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi59098716;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi59098716 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi95178787;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi95178787 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi88681659;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi88681659 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi43035963;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi43035963 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi89943729;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi89943729 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi68361350;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi68361350 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi21567390;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi21567390 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi22693953;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi22693953 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi28475383;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi28475383 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi17099240;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi17099240 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi21228528;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi21228528 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi24673433;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi24673433 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi80572579;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi80572579 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi60908165;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi60908165 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi82918463;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi82918463 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi68799381;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi68799381 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi50647464;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi50647464 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi60244137;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi60244137 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi42331693;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi42331693 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi29231949;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi29231949 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi43070103;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi43070103 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi60609503;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi60609503 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi44305651;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi44305651 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi73861203;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi73861203 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi86926492;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi86926492 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi89790610;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi89790610 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi67892969;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi67892969 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi90956825;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi90956825 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi26861200;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi26861200 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi73546627;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi73546627 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi18374178;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi18374178 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi40528568;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi40528568 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi64209007;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi64209007 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi30257415;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi30257415 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi14638528;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi14638528 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi57166014;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi57166014 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi98730787;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi98730787 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi6529780;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi6529780 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi23002094;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi23002094 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi62070831;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi62070831 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi52541206;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi52541206 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi5801003;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi5801003 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi97168649;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi97168649 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi84985288;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi84985288 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi48447812;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi48447812 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi67600147;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi67600147 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi3905065;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi3905065 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi2524918;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi2524918 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi50050331;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi50050331 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi18255114;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi18255114 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi43216390;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi43216390 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi79108737;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi79108737 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi40980480;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi40980480 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi59636576;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi59636576 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi16496415;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi16496415 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi98489213;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi98489213 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi50873137;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi50873137 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi14820457;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi14820457 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi56109470;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi56109470 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi153119;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi153119 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi468381;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi468381 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi30610565;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi30610565 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi95832752;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi95832752 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi54928756;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi54928756 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi98725061;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi98725061 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi80699959;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi80699959 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi60464425;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi60464425 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi50315164;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi50315164 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi46269637;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi46269637 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi25752450;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi25752450 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi70068593;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi70068593 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi44117684;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi44117684 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi37242043;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi37242043 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi80260862;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi80260862 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi76690743;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi76690743 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi37269100;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi37269100 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi63440854;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi63440854 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi59320362;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi59320362 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi25413391;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi25413391 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi19326346;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi19326346 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi85885546;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi85885546 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi65368052;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi65368052 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi40906494;     BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi40906494 = BdudULjnDzXntBTurIQIPNXaNneVmBFmcpOi12945824;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void yQSaprZpIUMrlZgCGdscfRExKUDoaFZ16969248() {     double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK38245417 = -733533908;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK84675634 = -60170629;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK16123373 = -252887844;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK24941044 = 78247336;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK30386127 = -873900158;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK41064120 = -378793791;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK77703275 = -827562386;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK47505114 = -382340981;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK857274 = 69166879;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK65671042 = -662080090;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK74149855 = -268391849;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK9657457 = -560039284;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK69361470 = -493498786;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK70010559 = -648634090;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK41428375 = -554945645;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK85729248 = -684076311;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK9103867 = -457173141;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK69989330 = -271309347;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK98535439 = -921492854;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK37176688 = -780948815;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK12015529 = -919951963;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK80888349 = -377669575;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK43653387 = -423458643;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK495404 = -811828602;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK11013647 = -807596181;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK16887988 = -97795005;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK97571531 = -279321312;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK17663278 = -636610664;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK9392045 = -213592901;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK90269954 = -541106694;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK53098369 = -179785075;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK43145392 = -874672155;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK4853043 = -219319825;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK10951169 = -233587449;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK55130568 = -364630637;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK32479683 = -36404195;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK59669291 = -484617371;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK78757509 = -860634225;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK25226038 = -335546729;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK35268237 = -483252848;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK77749556 = -153189394;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK37781803 = -256657957;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK90041113 = -438648744;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK42627568 = -171947956;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK30913000 = -656099648;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK29028605 = -433928931;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK43849335 = 77953321;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK82799278 = -638234673;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK6971177 = -216047472;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK37523580 = -306391560;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK86819648 = -248021717;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK60439439 = -632334590;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK19418023 = -204515402;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK56624121 = -888447351;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK4953719 = 18945857;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK57357067 = -255864334;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK41022248 = -636711986;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK15627970 = -441059242;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK13927397 = -114156484;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK13498140 = -676105154;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK43492589 = 527521;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK60039997 = -90951722;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK38113070 = -68748081;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK10587319 = -389726427;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK12572674 = -382295015;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK31004464 = -393719695;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK4804415 = -240719460;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK58410301 = -159911337;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK14879992 = -184003453;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK8948692 = -418541451;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK26059958 = -99458940;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK30346357 = -596538917;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK44763293 = -935762619;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK63267203 = -338240007;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK59427132 = -527759421;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK74233726 = -563294006;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK90847235 = -939020831;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK1025820 = -151510688;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK69582404 = -55728955;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK81985041 = -273667250;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK73038652 = -75748327;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK14772254 = -641086640;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK10692101 = -320563193;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK71868465 = -907201341;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK3450306 = -193084978;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK92658929 = -547450485;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK23727370 = -570156753;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK48228922 = -330872474;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK5997451 = -152533307;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK97773500 = -8766304;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK91457435 = -399692209;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK44041322 = 56441871;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK64830112 = -646477741;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK11727899 = -659441576;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK91775648 = -383780369;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK17709559 = 37762327;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK99668733 = -87909877;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK79453795 = 51077682;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK30054894 = -789652941;    double ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK99908535 = -733533908;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK38245417 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK84675634;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK84675634 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK16123373;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK16123373 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK24941044;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK24941044 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK30386127;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK30386127 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK41064120;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK41064120 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK77703275;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK77703275 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK47505114;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK47505114 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK857274;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK857274 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK65671042;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK65671042 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK74149855;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK74149855 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK9657457;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK9657457 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK69361470;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK69361470 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK70010559;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK70010559 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK41428375;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK41428375 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK85729248;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK85729248 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK9103867;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK9103867 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK69989330;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK69989330 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK98535439;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK98535439 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK37176688;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK37176688 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK12015529;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK12015529 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK80888349;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK80888349 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK43653387;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK43653387 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK495404;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK495404 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK11013647;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK11013647 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK16887988;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK16887988 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK97571531;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK97571531 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK17663278;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK17663278 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK9392045;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK9392045 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK90269954;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK90269954 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK53098369;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK53098369 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK43145392;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK43145392 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK4853043;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK4853043 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK10951169;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK10951169 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK55130568;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK55130568 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK32479683;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK32479683 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK59669291;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK59669291 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK78757509;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK78757509 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK25226038;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK25226038 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK35268237;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK35268237 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK77749556;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK77749556 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK37781803;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK37781803 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK90041113;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK90041113 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK42627568;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK42627568 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK30913000;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK30913000 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK29028605;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK29028605 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK43849335;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK43849335 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK82799278;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK82799278 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK6971177;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK6971177 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK37523580;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK37523580 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK86819648;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK86819648 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK60439439;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK60439439 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK19418023;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK19418023 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK56624121;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK56624121 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK4953719;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK4953719 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK57357067;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK57357067 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK41022248;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK41022248 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK15627970;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK15627970 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK13927397;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK13927397 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK13498140;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK13498140 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK43492589;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK43492589 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK60039997;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK60039997 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK38113070;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK38113070 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK10587319;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK10587319 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK12572674;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK12572674 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK31004464;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK31004464 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK4804415;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK4804415 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK58410301;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK58410301 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK14879992;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK14879992 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK8948692;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK8948692 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK26059958;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK26059958 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK30346357;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK30346357 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK44763293;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK44763293 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK63267203;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK63267203 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK59427132;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK59427132 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK74233726;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK74233726 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK90847235;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK90847235 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK1025820;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK1025820 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK69582404;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK69582404 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK81985041;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK81985041 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK73038652;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK73038652 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK14772254;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK14772254 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK10692101;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK10692101 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK71868465;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK71868465 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK3450306;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK3450306 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK92658929;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK92658929 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK23727370;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK23727370 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK48228922;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK48228922 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK5997451;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK5997451 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK97773500;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK97773500 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK91457435;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK91457435 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK44041322;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK44041322 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK64830112;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK64830112 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK11727899;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK11727899 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK91775648;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK91775648 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK17709559;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK17709559 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK99668733;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK99668733 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK79453795;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK79453795 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK30054894;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK30054894 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK99908535;     ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK99908535 = ygNfGkxzBKzFqqyFIlsOfiDShLUPVokODuOK38245417;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ncOLxmwYRJmeBczFOWahHBNnJqMEmsd60946709() {     int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB3088627 = -928743724;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB80238326 = -380092436;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB99200174 = -39157367;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB70223781 = -654570041;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB34455315 = -763591570;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB65680254 = -225966711;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB80763668 = -174118810;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB7182622 = -944423201;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB17944550 = 68301401;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB32442976 = -952354066;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB80738809 = -294224081;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB66590972 = 47704517;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB86781493 = -794347758;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB36510418 = -26618747;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB30364766 = -156475641;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB40537032 = -618282020;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB99824192 = 16597124;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB47036010 = -900886116;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB7339819 = -786369811;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB3792171 = 99738130;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB44279592 = -305979647;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB60816042 = -761950037;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB59777514 = -60543801;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB68160148 = -500852668;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB87826345 = -914365074;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB45943129 = -164725500;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB44067691 = -786088835;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB64296472 = -819296342;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB81189734 = -370847506;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB18663450 = -503448104;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB47511302 = -929769055;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB22079977 = 41560656;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB79150702 = -168575402;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB9021751 = -832938375;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB66702986 = -38265731;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB21520293 = -389797883;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB97821506 = 95177583;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB31387580 = 19112068;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB59560295 = 50399440;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB85878350 = -895571378;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB87559142 = -891370917;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB31943924 = -4350001;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB82704904 = -244508392;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB34562566 = -31744965;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB55815780 = -337041503;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB93918113 = -936050237;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB34114976 = -471055962;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB57156762 = -334489676;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB40247197 = 38004844;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB64207201 = -674794325;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB38803276 = -134585711;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB62555823 = 66347521;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB26513905 = -295968339;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB13351338 = -453798307;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB16667117 = -625293930;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB42272584 = -66793688;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB20460813 = -219548636;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB31040027 = -538304700;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB82397436 = -740204968;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB88512185 = -498866071;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB21612564 = -439877876;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB16467196 = -354822468;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB25992887 = -473575696;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB99281100 = -428250496;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB84931673 = 77414988;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB58658832 = -235784737;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB87440270 = -783720082;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB77759742 = -961409383;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB69807432 = -988353016;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB8844474 = -766677759;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB42715526 = -613459604;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB68436613 = 97485056;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB87475714 = -851285556;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB21461469 = -890798434;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB16233029 = -8890954;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB12335669 = -201629647;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB78111137 = -417441645;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB25214948 = 71201163;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB12344368 = -63811166;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB93908231 = -978314837;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB11828153 = -693669539;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB86910928 = -351599160;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB24049275 = -757301187;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB16982534 = -696053181;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB79860174 = -268862393;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB84955479 = -896116577;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB95566072 = -662471005;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB65799364 = -714777096;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB92354633 = -107644446;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB24430402 = -971472044;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB1059481 = -70249248;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB66781480 = -366517717;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB48990143 = -240682965;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB71048109 = -450734490;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB64265787 = -355693502;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB71091947 = -436548450;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB5951037 = -530774306;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB83423804 = -816257896;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB49630893 = -9159953;    int KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB97156947 = -928743724;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB3088627 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB80238326;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB80238326 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB99200174;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB99200174 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB70223781;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB70223781 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB34455315;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB34455315 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB65680254;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB65680254 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB80763668;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB80763668 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB7182622;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB7182622 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB17944550;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB17944550 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB32442976;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB32442976 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB80738809;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB80738809 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB66590972;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB66590972 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB86781493;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB86781493 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB36510418;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB36510418 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB30364766;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB30364766 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB40537032;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB40537032 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB99824192;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB99824192 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB47036010;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB47036010 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB7339819;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB7339819 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB3792171;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB3792171 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB44279592;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB44279592 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB60816042;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB60816042 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB59777514;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB59777514 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB68160148;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB68160148 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB87826345;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB87826345 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB45943129;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB45943129 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB44067691;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB44067691 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB64296472;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB64296472 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB81189734;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB81189734 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB18663450;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB18663450 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB47511302;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB47511302 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB22079977;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB22079977 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB79150702;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB79150702 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB9021751;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB9021751 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB66702986;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB66702986 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB21520293;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB21520293 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB97821506;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB97821506 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB31387580;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB31387580 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB59560295;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB59560295 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB85878350;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB85878350 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB87559142;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB87559142 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB31943924;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB31943924 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB82704904;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB82704904 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB34562566;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB34562566 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB55815780;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB55815780 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB93918113;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB93918113 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB34114976;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB34114976 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB57156762;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB57156762 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB40247197;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB40247197 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB64207201;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB64207201 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB38803276;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB38803276 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB62555823;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB62555823 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB26513905;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB26513905 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB13351338;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB13351338 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB16667117;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB16667117 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB42272584;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB42272584 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB20460813;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB20460813 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB31040027;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB31040027 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB82397436;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB82397436 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB88512185;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB88512185 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB21612564;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB21612564 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB16467196;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB16467196 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB25992887;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB25992887 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB99281100;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB99281100 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB84931673;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB84931673 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB58658832;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB58658832 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB87440270;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB87440270 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB77759742;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB77759742 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB69807432;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB69807432 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB8844474;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB8844474 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB42715526;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB42715526 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB68436613;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB68436613 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB87475714;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB87475714 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB21461469;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB21461469 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB16233029;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB16233029 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB12335669;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB12335669 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB78111137;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB78111137 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB25214948;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB25214948 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB12344368;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB12344368 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB93908231;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB93908231 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB11828153;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB11828153 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB86910928;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB86910928 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB24049275;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB24049275 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB16982534;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB16982534 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB79860174;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB79860174 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB84955479;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB84955479 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB95566072;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB95566072 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB65799364;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB65799364 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB92354633;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB92354633 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB24430402;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB24430402 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB1059481;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB1059481 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB66781480;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB66781480 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB48990143;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB48990143 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB71048109;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB71048109 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB64265787;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB64265787 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB71091947;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB71091947 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB5951037;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB5951037 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB83423804;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB83423804 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB49630893;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB49630893 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB97156947;     KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB97156947 = KdUZOJJyjEjMCEKqrDQxTfECmObCGasmZfbB3088627;}
// Junk Finished
