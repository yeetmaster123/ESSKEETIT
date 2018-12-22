#pragma once
class Protobuffs;
// It's actually in DLLMain but w/e
extern bool DoUnload;

#include <Windows.h>
#include <memory>
#include <d3d9.h>
#include "Utilities.h"
#include "Settings.h"
#include "EnginePrediction.h"
#include "VFTableHook.h"
#include "Hitmarkers.h"
#include "viewsetup.h"
#include "WallModulations.h"

#include "Menu.h"
#include "RageBot.h"
#include "Crosshair.h"

#include "Interfaces.h"
#include "DrawManager.h"
#include "Resolver.h"
#include "QAngle.hpp"

#include "Playerlist.h"
#include "DrawManager.h"
#include "Backtracking.h"
#include "Game.h""
#include "Spammers.h"
#include "Chams.h"

#include "steam.h"
#include "IMemAlloc.h"


#define TIME_TO_TICKS( dt )	( ( int )( 0.5f + ( float )( dt ) / m_pGlobals->interval_per_tick ) )
#define TICKS_TO_TIME( t ) ( m_pGlobals->interval_per_tick *( t ) )

struct DrawModelState_t {
	studiohdr_t*			m_pStudioHdr;
	void*					m_pStudioHWData;
	IClientRenderable*		m_pRenderable;
	const matrix3x4_t			*m_pModelToWorld;
	unsigned short			m_decals;
	int						m_drawFlags;
	int						m_lod;
};

class CHookedEvents : public IGameEventListener
{
public:
	void FireGameEvent(IGameEvent *event);
	void RegisterSelf();
};

namespace Hooks
{
	void Initialise();
	void DrawBeamd(Vector src, Vector end, Color color);
	void UndoHooks();

	typedef void(__stdcall* OverrideView_t)(CViewSetup*);
	typedef float(__stdcall* GetViewmodelFov_t)();
	typedef void(__stdcall* FrameStageNotify_t)(ClientFrameStage_t);
	typedef bool(__thiscall* IsPlayingTimeDemo_t)(void*);
	typedef bool(__thiscall* CreateMove_t)(IClientMode*, float, CUserCmd*);
	typedef void(__thiscall* PaintTraverse_t)(PVOID, unsigned int, bool, bool);
	typedef void(__thiscall* DrawModelExecute_t)(IVModelRender*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);
	typedef bool(__thiscall* FireEventClientSide_t)(void*, IGameEvent*);
	typedef void(__thiscall* RunCommand_t)(PVOID, IClientEntity*, CUserCmd*, IMoveHelper*);
	typedef void(__thiscall* PlaySound_t)(ISurface*, const char* name);
	typedef int(__thiscall* DoPostScreenEffects_t)(IClientMode*, int);
	typedef void(__fastcall* SceneEnd_t)(void*, void*);

	

	

	

	extern CreateMove_t g_fnOriginalCreateMove;
	extern PaintTraverse_t g_fnOriginalPaintTraverse;
	extern DrawModelExecute_t g_fnOriginalDrawModelExecute;
	extern FrameStageNotify_t g_fnOriginalFrameStageNotify;
	extern FireEventClientSide_t g_fnOriginalFireEventClientSide;
	extern OverrideView_t g_fnOriginalOverrideView;
	extern IsPlayingTimeDemo_t g_fnOriginalPlayingTimeDemo;
	extern PlaySound_t g_fnOriginalPlaySound;
	extern DoPostScreenEffects_t g_fnOriginalDoPostScreenEffects;
	extern SceneEnd_t g_fnOriginalSceneEnd;
	

	extern VFTableHook* g_pClientModeHook;
	extern VFTableHook* g_pModelRenderHook;
	extern VFTableHook* g_pMaterialSystemHook;
	extern VFTableHook* g_pPredictionHook;
	extern IViewRenderBeams* g_pViewRenderBeams;

	typedef long(__stdcall *EndSceneFn)(IDirect3DDevice9* device);
	typedef long(__stdcall *EndSceneResetFn)(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	bool __fastcall Hooked_FireEventClientSide(void *ecx, void* edx, IGameEvent* pEvent);
	bool __fastcall Hooked_IsPlayingTimeDemo(void* thisptr, void* edx);
	bool __stdcall Hooked_CreateMove(float sample_input_frametime, CUserCmd* pCmd);
	IMaterial* __fastcall Hooked_FindMaterial(void* pThis, void*, const char* pMaterialName, const char* pTextureGroup, bool complain, const char* pComplainPrefix);
	void __stdcall Hooked_PlaySound(const char* name);
	float __stdcall Hooked_GetViewmodelFov();
	void  __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage);
	void __fastcall Hooked_PaintTraverse(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce);
	void __stdcall Hooked_DrawModelExecute(IMatRenderContext* CTX, const DrawModelState_t &state, const ModelRenderInfo_t &renderInfo, matrix3x4_t *pCustomBoneToWorld);
	void __stdcall Hooked_RunCommand(IClientEntity* pPlayer, CUserCmd* pCmd, IMoveHelper* pMoveHelper);
	void __stdcall Hooked_OverrideView(CViewSetup* vsView);
	int  __stdcall Hooked_DoPostScreenEffects(int a1);
	void __fastcall Hooked_SceneEnd(void *ecx, void *edx);
	void __stdcall touring_csgo_CHudDeathNotice_FireGameEvent(DWORD *this_ptr, IGameEvent * event);

	void LowerbodyProxy(const CRecvProxyData *pData, void *pStruct, void *pOut);

	extern CHookedEvents hooked_events;
	extern Protobuffs ProtoFeatures;
	void initialize_events();
};


extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern CGameSearch gg;