// ImGui Win32 + DirectX9 binding
// In this binding, ImTextureID is used to store a 'LPDIRECT3DTEXTURE9' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#include "../imgui.h"
#include "imgui_dx9.h"

// DirectX
#include <d3d9.h>



#include <string>
#include <Windows.h>

// Data
static HWND                     g_hWnd = 0;
static INT64                    g_Time = 0;
static INT64                    g_TicksPerSecond = 0;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static LPDIRECT3DVERTEXBUFFER9  g_pVB = NULL;
static LPDIRECT3DINDEXBUFFER9   g_pIB = NULL;
static LPDIRECT3DTEXTURE9       g_FontTexture = NULL;
static int                      g_VertexBufferSize = 5000, g_IndexBufferSize = 10000;

struct CUSTOMVERTEX
{
	float    pos[3];
	D3DCOLOR col;
	float    uv[2];
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
// If text or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
void ImGui_ImplDX9_RenderDrawLists(ImDrawData* draw_data)
{
	// Avoid rendering when minimized
	ImGuiIO& io = ImGui::GetIO();
	if (io.DisplaySize.x <= 0.0f || io.DisplaySize.y <= 0.0f)
		return;

	// Create and grow buffers if needed
	if (!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount)
	{
		if (g_pVB) { g_pVB->Release(); g_pVB = NULL; }
		g_VertexBufferSize = draw_data->TotalVtxCount + 5000;
		if (g_pd3dDevice->CreateVertexBuffer(g_VertexBufferSize * sizeof(CUSTOMVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL) < 0)
			return;
	}
	if (!g_pIB || g_IndexBufferSize < draw_data->TotalIdxCount)
	{
		if (g_pIB) { g_pIB->Release(); g_pIB = NULL; }
		g_IndexBufferSize = draw_data->TotalIdxCount + 10000;
		if (g_pd3dDevice->CreateIndexBuffer(g_IndexBufferSize * sizeof(ImDrawIdx), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, sizeof(ImDrawIdx) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, D3DPOOL_DEFAULT, &g_pIB, NULL) < 0)
			return;
	}

	// Backup the DX9 state
	IDirect3DStateBlock9* d3d9_state_block = NULL;
	if (g_pd3dDevice->CreateStateBlock(D3DSBT_ALL, &d3d9_state_block) < 0)
		return;

	// Copy and convert all vertices into a single contiguous buffer
	CUSTOMVERTEX* vtx_dst;
	ImDrawIdx* idx_dst;
	if (g_pVB->Lock(0, (UINT)(draw_data->TotalVtxCount * sizeof(CUSTOMVERTEX)), (void**)&vtx_dst, D3DLOCK_DISCARD) < 0)
		return;
	if (g_pIB->Lock(0, (UINT)(draw_data->TotalIdxCount * sizeof(ImDrawIdx)), (void**)&idx_dst, D3DLOCK_DISCARD) < 0)
		return;
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawVert* vtx_src = cmd_list->VtxBuffer.Data;
		for (int i = 0; i < cmd_list->VtxBuffer.Size; i++)
		{
			vtx_dst->pos[0] = vtx_src->pos.x;
			vtx_dst->pos[1] = vtx_src->pos.y;
			vtx_dst->pos[2] = 0.0f;
			vtx_dst->col = (vtx_src->col & 0xFF00FF00) | ((vtx_src->col & 0xFF0000) >> 16) | ((vtx_src->col & 0xFF) << 16);     // RGBA --> ARGB for DirectX9
			vtx_dst->uv[0] = vtx_src->uv.x;
			vtx_dst->uv[1] = vtx_src->uv.y;
			vtx_dst++;
			vtx_src++;
		}
		memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		idx_dst += cmd_list->IdxBuffer.Size;
	}
	g_pVB->Unlock();
	g_pIB->Unlock();
	g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
	g_pd3dDevice->SetIndices(g_pIB);
	g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

	// Setup render state: fixed-pipeline, alpha-blending, no face culling, no depth testing
	g_pd3dDevice->SetPixelShader(NULL);
	g_pd3dDevice->SetVertexShader(NULL);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, true);
	g_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	// Setup orthographic projection matrix
	// Being agnostic of whether <d3dx9.h> or <DirectXMath.h> can be used, we aren't relying on D3DXMatrixIdentity()/D3DXMatrixOrthoOffCenterLH() or DirectX::XMMatrixIdentity()/DirectX::XMMatrixOrthographicOffCenterLH()
	{
		const float L = 0.5f, R = io.DisplaySize.x + 0.5f, T = 0.5f, B = io.DisplaySize.y + 0.5f;
		D3DMATRIX mat_identity = { { 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f } };
		D3DMATRIX mat_projection =
		{
			2.0f / (R - L),   0.0f,         0.0f,  0.0f,
			0.0f,         2.0f / (T - B),   0.0f,  0.0f,
			0.0f,         0.0f,         0.5f,  0.0f,
			(L + R) / (L - R),  (T + B) / (B - T),  0.5f,  1.0f,
		};
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &mat_identity);
		g_pd3dDevice->SetTransform(D3DTS_VIEW, &mat_identity);
		g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &mat_projection);
	}

	// Render command lists
	int vtx_offset = 0;
	int idx_offset = 0;
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				const RECT r = { (LONG)pcmd->ClipRect.x, (LONG)pcmd->ClipRect.y, (LONG)pcmd->ClipRect.z, (LONG)pcmd->ClipRect.w };
				g_pd3dDevice->SetTexture(0, (LPDIRECT3DTEXTURE9)pcmd->TextureId);
				g_pd3dDevice->SetScissorRect(&r);
				g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, vtx_offset, 0, (UINT)cmd_list->VtxBuffer.Size, idx_offset, pcmd->ElemCount / 3);
			}
			idx_offset += pcmd->ElemCount;
		}
		vtx_offset += cmd_list->VtxBuffer.Size;
	}

	// Restore the DX9 state
	d3d9_state_block->Apply();
	d3d9_state_block->Release();
}

IMGUI_API LRESULT ImGui_ImplDX9_WndProcHandler(HWND, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGuiIO& io = ImGui::GetIO();
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		io.MouseDown[0] = true;
		return true;
	case WM_LBUTTONUP:
		io.MouseDown[0] = false;
		return true;
	case WM_RBUTTONDOWN:
		io.MouseDown[1] = true;
		return true;
	case WM_RBUTTONUP:
		io.MouseDown[1] = false;
		return true;
	case WM_MBUTTONDOWN:
		io.MouseDown[2] = true;
		return true;
	case WM_MBUTTONUP:
		io.MouseDown[2] = false;
		return true;
	case WM_MOUSEWHEEL:
		io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
		return true;
	case WM_MOUSEMOVE:
		io.MousePos.x = (signed short)(lParam);
		io.MousePos.y = (signed short)(lParam >> 16);
		return true;
	case WM_KEYDOWN:
		if (wParam < 256)
			io.KeysDown[wParam] = 1;
		return true;
	case WM_KEYUP:
		if (wParam < 256)
			io.KeysDown[wParam] = 0;
		return true;
	case WM_CHAR:
		// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
		if (wParam > 0 && wParam < 0x10000)
			io.AddInputCharacter((unsigned short)wParam);
		return true;
	}
	return 0;
}

bool ImGui_ImplDX9_Init(void* hwnd, IDirect3DDevice9* device)
{
	g_hWnd = (HWND)hwnd;
	g_pd3dDevice = device;

	if (!QueryPerformanceFrequency((LARGE_INTEGER *)&g_TicksPerSecond))
		return false;
	if (!QueryPerformanceCounter((LARGE_INTEGER *)&g_Time))
		return false;

	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = VK_TAB;                       // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will update during the application lifetime.
	io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
	io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
	io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
	io.KeyMap[ImGuiKey_Home] = VK_HOME;
	io.KeyMap[ImGuiKey_End] = VK_END;
	io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
	io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
	io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
	io.KeyMap[ImGuiKey_A] = 'A';
	io.KeyMap[ImGuiKey_C] = 'C';
	io.KeyMap[ImGuiKey_V] = 'V';
	io.KeyMap[ImGuiKey_X] = 'X';
	io.KeyMap[ImGuiKey_Y] = 'Y';
	io.KeyMap[ImGuiKey_Z] = 'Z';

	io.RenderDrawListsFn = ImGui_ImplDX9_RenderDrawLists;   // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
	io.ImeWindowHandle = g_hWnd;

	return true;
}

void ImGui_ImplDX9_Shutdown()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	ImGui::Shutdown();
	g_pd3dDevice = NULL;
	g_hWnd = 0;
}

bool ImGui_ImplDX9_GetFontPath(const std::string& name, std::string& path)
{
	//
	// This code is not as safe as it should be.
	// Assumptions we make:
	//  -> GetWindowsDirectoryA does not fail.
	//  -> The registry key exists.
	//  -> The subkeys are ordered alphabetically
	//  -> The subkeys name and data are no longer than 260 (MAX_PATH) chars.
	//

	char buffer[MAX_PATH];
	HKEY registryKey;

	GetWindowsDirectoryA(buffer, MAX_PATH);
	std::string fontsFolder = buffer + std::string("\\Fonts\\");

	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 0, KEY_READ, &registryKey)) {
		return false;
	}

	uint32_t valueIndex = 0;
	char valueName[MAX_PATH];
	uint8_t valueData[MAX_PATH];
	std::wstring wsFontFile;

	do {
		uint32_t valueNameSize = MAX_PATH;
		uint32_t valueDataSize = MAX_PATH;
		uint32_t valueType;

		auto error = RegEnumValueA(
			registryKey,
			valueIndex,
			valueName,
			reinterpret_cast<DWORD*>(&valueNameSize),
			0,
			reinterpret_cast<DWORD*>(&valueType),
			valueData,
			reinterpret_cast<DWORD*>(&valueDataSize));

		valueIndex++;

		if (error == ERROR_NO_MORE_ITEMS) {
			RegCloseKey(registryKey);
			return false;
		}

		if (error || valueType != REG_SZ) {
			continue;
		}

		if (_strnicmp(name.data(), valueName, name.size()) == 0) {
			path = fontsFolder + std::string((char*)valueData, valueDataSize);
			RegCloseKey(registryKey);
			return true;
		}
	} while (true);

	return false;
}

static bool ImGui_ImplDX9_CreateFontsTexture()
{
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height, bytes_per_pixel;
	std::string path;

	if (ImGui_ImplDX9_GetFontPath("Tahoma", path))
		io.Fonts->AddFontFromFileTTF(std::data(path), 14.0f);
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);

	// Upload texture to graphics system
	g_FontTexture = NULL;
	if (g_pd3dDevice->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_FontTexture, NULL) < 0)
		return false;
	D3DLOCKED_RECT tex_locked_rect;
	if (g_FontTexture->LockRect(0, &tex_locked_rect, NULL, 0) != D3D_OK)
		return false;
	for (int y = 0; y < height; y++)
		memcpy((unsigned char *)tex_locked_rect.pBits + tex_locked_rect.Pitch * y, pixels + (width * bytes_per_pixel) * y, (width * bytes_per_pixel));
	g_FontTexture->UnlockRect(0);

	// Store our identifier
	io.Fonts->TexID = (void *)g_FontTexture;

	return true;
}

bool ImGui_ImplDX9_CreateDeviceObjects()
{
	if (!g_pd3dDevice)
		return false;
	if (!ImGui_ImplDX9_CreateFontsTexture())
		return false;
	return true;
}

void ImGui_ImplDX9_InvalidateDeviceObjects()
{
	if (!g_pd3dDevice)
		return;
	if (g_pVB)
	{
		g_pVB->Release();
		g_pVB = NULL;
	}
	if (g_pIB)
	{
		g_pIB->Release();
		g_pIB = NULL;
	}
	if (g_FontTexture)
	{
		g_FontTexture->Release();
		g_FontTexture = NULL;
	}
}

void ImGui_ImplDX9_NewFrame()
{
	if (!g_FontTexture)
		ImGui_ImplDX9_CreateDeviceObjects();

	ImGuiIO& io = ImGui::GetIO();

	// Setup display size (every frame to accommodate for window resizing)
	RECT rect;
	GetClientRect(g_hWnd, &rect);
	io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

	// Setup time step
	INT64 current_time;
	QueryPerformanceCounter((LARGE_INTEGER *)&current_time);
	io.DeltaTime = (float)(current_time - g_Time) / g_TicksPerSecond;
	g_Time = current_time;

	// Read keyboard modifiers inputs
	io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
	io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
	io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
	io.KeySuper = false;
	// io.KeysDown : filled by WM_KEYDOWN/WM_KEYUP events
	// io.MousePos : filled by WM_MOUSEMOVE events
	// io.MouseDown : filled by WM_*BUTTON* events
	// io.MouseWheel : filled by WM_MOUSEWHEEL events

	// Hide OS mouse cursor if ImGui is drawing it
	if (io.MouseDrawCursor)
		SetCursor(NULL);

	// Start the frame
	ImGui::NewFrame();
}

// Junk Code By Troll Face & Thaisen's Gen
void kHXwyGekUDDzxrhUuaBXxwnFkvuIFvy70389348() {     double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy48696947 = 4081944;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy47285633 = -265998410;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy1046259 = -394272467;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy64914290 = -996122538;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy37689110 = -709672153;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy81531652 = -936360280;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy9080320 = -439613161;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy44969680 = -699954404;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy12204749 = -394048964;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy98616703 = -591114758;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy23982417 = -281894246;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy86045661 = -657058111;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy1328748 = -218629507;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy99551549 = -737799062;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy94487266 = -945034793;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy8705599 = -257406141;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy9810460 = -448053862;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy46083665 = -664338584;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy33408916 = -381084354;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy23111555 = -870411000;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy8483101 = -954002729;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy25627476 = -558604322;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy30480094 = -819136275;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy94496255 = -755857173;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy68852659 = -121359682;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy63676009 = -522218109;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy9213850 = -148975341;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy94017477 = -382792850;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy81821801 = 63435576;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy53641236 = -859923228;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy75935148 = -954520467;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy12980629 = -466386736;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy80698360 = -301945845;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy73396419 = -37080049;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy50213109 = -109604947;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy65708206 = -974822689;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy85910442 = -452825565;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy64253106 = -290132337;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy31993513 = -931362602;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy54525379 = 61643269;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy62720072 = -340478730;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy83748238 = -125023193;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy67840346 = -888526409;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy44851090 = -182493360;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy58979933 = -319525737;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy96627477 = -805012549;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy94659745 = -908405667;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy4404580 = -512094709;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy29131557 = -417180678;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy49188597 = -838310524;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy51342783 = 72283391;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy65034411 = -9057462;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy6160410 = -416870481;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy15477940 = -885724116;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy61906248 = -518237682;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy23069472 = -437313735;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy16805539 = -446862136;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy6550004 = -638415295;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy96061630 = -774762856;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy74013101 = -87454044;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy72317802 = -687384939;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy15062843 = 43179688;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy63147878 = -663389980;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy58563512 = -534125737;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy22681556 = -636594292;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy11001789 = -815507510;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy5347301 = -255112266;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy27932328 = -81549459;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy49338441 = -528194115;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy28779060 = -970212104;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy22795156 = -804580576;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy45557354 = -57921525;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy14090153 = -732975983;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy78883537 = -342727623;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy60391482 = -429932271;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy24734863 = -728979537;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy57787130 = -670077913;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy85629003 = -536642916;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy35516322 = -336331437;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy72225181 = -316347134;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy69016264 = -613812442;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy4809270 = -636880632;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy64885920 = -965612172;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy32633204 = -98253901;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy2298454 = -832206620;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy10900738 = -845463005;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy6820219 = 50483745;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy65220421 = -416221730;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy11490172 = -518842368;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy27143638 = -672291213;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy48902667 = -427960554;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy79360438 = -814410271;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy68191475 = -515369482;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy57980412 = -743908558;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy82207576 = -250971792;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy47657230 = -283658419;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy20600360 = -461633213;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy9276834 = -254400673;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy22169535 = -545899069;    double iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy47978145 = 4081944;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy48696947 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy47285633;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy47285633 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy1046259;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy1046259 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy64914290;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy64914290 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy37689110;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy37689110 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy81531652;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy81531652 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy9080320;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy9080320 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy44969680;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy44969680 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy12204749;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy12204749 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy98616703;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy98616703 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy23982417;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy23982417 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy86045661;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy86045661 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy1328748;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy1328748 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy99551549;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy99551549 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy94487266;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy94487266 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy8705599;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy8705599 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy9810460;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy9810460 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy46083665;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy46083665 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy33408916;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy33408916 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy23111555;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy23111555 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy8483101;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy8483101 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy25627476;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy25627476 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy30480094;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy30480094 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy94496255;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy94496255 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy68852659;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy68852659 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy63676009;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy63676009 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy9213850;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy9213850 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy94017477;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy94017477 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy81821801;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy81821801 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy53641236;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy53641236 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy75935148;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy75935148 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy12980629;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy12980629 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy80698360;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy80698360 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy73396419;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy73396419 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy50213109;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy50213109 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy65708206;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy65708206 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy85910442;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy85910442 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy64253106;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy64253106 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy31993513;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy31993513 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy54525379;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy54525379 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy62720072;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy62720072 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy83748238;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy83748238 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy67840346;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy67840346 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy44851090;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy44851090 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy58979933;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy58979933 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy96627477;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy96627477 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy94659745;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy94659745 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy4404580;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy4404580 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy29131557;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy29131557 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy49188597;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy49188597 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy51342783;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy51342783 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy65034411;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy65034411 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy6160410;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy6160410 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy15477940;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy15477940 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy61906248;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy61906248 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy23069472;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy23069472 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy16805539;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy16805539 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy6550004;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy6550004 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy96061630;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy96061630 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy74013101;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy74013101 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy72317802;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy72317802 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy15062843;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy15062843 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy63147878;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy63147878 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy58563512;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy58563512 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy22681556;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy22681556 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy11001789;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy11001789 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy5347301;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy5347301 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy27932328;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy27932328 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy49338441;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy49338441 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy28779060;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy28779060 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy22795156;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy22795156 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy45557354;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy45557354 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy14090153;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy14090153 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy78883537;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy78883537 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy60391482;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy60391482 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy24734863;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy24734863 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy57787130;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy57787130 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy85629003;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy85629003 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy35516322;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy35516322 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy72225181;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy72225181 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy69016264;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy69016264 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy4809270;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy4809270 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy64885920;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy64885920 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy32633204;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy32633204 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy2298454;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy2298454 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy10900738;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy10900738 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy6820219;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy6820219 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy65220421;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy65220421 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy11490172;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy11490172 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy27143638;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy27143638 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy48902667;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy48902667 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy79360438;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy79360438 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy68191475;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy68191475 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy57980412;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy57980412 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy82207576;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy82207576 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy47657230;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy47657230 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy20600360;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy20600360 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy9276834;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy9276834 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy22169535;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy22169535 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy47978145;     iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy47978145 = iBsGCeRIQKVRKTNlYYZJMUcLoTRLDMWeZEVy48696947;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void VJxMyTQcyiXzwaaUeMWFlUMuLKKWwPg37792854() {     double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ73996540 = -924118913;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ1529087 = -759801655;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ43061036 = -79080154;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ61360378 = 87095835;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ42431204 = -633869069;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ20511245 = -466525079;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ17107255 = -444818011;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ201215 = -151517059;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ28584378 = -247895734;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ854664 = -807818249;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ65163478 = -901624820;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ8648623 = -894724364;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ76826363 = 46634656;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ30209235 = -496083903;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ52603468 = -964619298;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ5566322 = -890344942;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ59347809 = -650453390;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ56974280 = -14140874;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ36765569 = -892066109;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ71606582 = -170167903;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ77462667 = -41231400;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ16572096 = -466325618;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ5772132 = -232680683;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ73424269 = 71780582;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ57172353 = -596871688;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ52088615 = -532417263;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ89686141 = -299797108;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ90452227 = -404935473;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ66540413 = -157237816;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ63338611 = -284507641;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ68125352 = -538919913;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ73207557 = -498002186;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ16752022 = 92630719;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ33700124 = -257412275;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ45099541 = -433728908;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ55856197 = -281810561;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ16347784 = -591498771;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ99940513 = 56562852;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ96610047 = -881721244;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ45487964 = -923705379;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ66608424 = -924991665;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ34603548 = -695736411;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ68090848 = -174916031;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ19585689 = -385112440;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ98936107 = -133949130;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ98794882 = -444183971;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ64962452 = -432813472;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ68829680 = -450638732;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ95574164 = -966438673;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ22503169 = -948363589;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ7905017 = -910059239;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ10835323 = -875488911;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ68412418 = 23362008;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ73371272 = -874411948;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ60330187 = -781986550;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ57424445 = -357793296;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ95756955 = -427120972;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ69636766 = -50860737;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ4188026 = -316032478;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ90342589 = -1451807;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ30825103 = -66727971;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ26655027 = 60117462;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ33660801 = -994279243;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ65245767 = -963388094;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ32729311 = -168898337;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ91955921 = -303622635;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ91896601 = -887355084;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ43126239 = -695953070;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ85109694 = 37645005;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ96747270 = -582808737;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ89218538 = -198846172;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ59407296 = -607016243;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ60364232 = -132419630;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ91277605 = -968360731;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ4998158 = -245176238;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ42859119 = -345494989;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ48481247 = -191409588;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ86186442 = -847568243;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ74488161 = -794270289;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ58377471 = -52687717;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ87126162 = 396209;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ20856462 = -849158377;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ94878062 = -438496800;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ44037244 = -208874228;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ55433595 = -374448402;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ57290030 = -663431002;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ4795140 = -421364194;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ43380749 = -32957333;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ73369937 = -475425726;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ87675095 = 24064387;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ60099242 = -854689590;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ46711017 = -440638035;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ95752487 = -627404670;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ6267458 = -780269438;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ14662861 = -756977408;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ39953398 = -885109127;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ942747 = -701457168;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ2845082 = -211527938;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ86856377 = -116214104;    double BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ6980187 = -924118913;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ73996540 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ1529087;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ1529087 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ43061036;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ43061036 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ61360378;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ61360378 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ42431204;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ42431204 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ20511245;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ20511245 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ17107255;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ17107255 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ201215;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ201215 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ28584378;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ28584378 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ854664;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ854664 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ65163478;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ65163478 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ8648623;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ8648623 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ76826363;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ76826363 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ30209235;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ30209235 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ52603468;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ52603468 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ5566322;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ5566322 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ59347809;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ59347809 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ56974280;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ56974280 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ36765569;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ36765569 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ71606582;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ71606582 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ77462667;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ77462667 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ16572096;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ16572096 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ5772132;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ5772132 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ73424269;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ73424269 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ57172353;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ57172353 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ52088615;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ52088615 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ89686141;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ89686141 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ90452227;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ90452227 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ66540413;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ66540413 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ63338611;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ63338611 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ68125352;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ68125352 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ73207557;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ73207557 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ16752022;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ16752022 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ33700124;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ33700124 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ45099541;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ45099541 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ55856197;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ55856197 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ16347784;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ16347784 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ99940513;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ99940513 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ96610047;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ96610047 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ45487964;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ45487964 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ66608424;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ66608424 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ34603548;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ34603548 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ68090848;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ68090848 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ19585689;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ19585689 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ98936107;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ98936107 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ98794882;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ98794882 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ64962452;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ64962452 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ68829680;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ68829680 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ95574164;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ95574164 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ22503169;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ22503169 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ7905017;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ7905017 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ10835323;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ10835323 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ68412418;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ68412418 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ73371272;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ73371272 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ60330187;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ60330187 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ57424445;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ57424445 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ95756955;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ95756955 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ69636766;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ69636766 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ4188026;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ4188026 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ90342589;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ90342589 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ30825103;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ30825103 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ26655027;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ26655027 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ33660801;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ33660801 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ65245767;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ65245767 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ32729311;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ32729311 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ91955921;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ91955921 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ91896601;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ91896601 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ43126239;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ43126239 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ85109694;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ85109694 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ96747270;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ96747270 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ89218538;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ89218538 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ59407296;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ59407296 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ60364232;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ60364232 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ91277605;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ91277605 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ4998158;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ4998158 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ42859119;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ42859119 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ48481247;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ48481247 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ86186442;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ86186442 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ74488161;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ74488161 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ58377471;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ58377471 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ87126162;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ87126162 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ20856462;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ20856462 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ94878062;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ94878062 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ44037244;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ44037244 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ55433595;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ55433595 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ57290030;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ57290030 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ4795140;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ4795140 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ43380749;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ43380749 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ73369937;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ73369937 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ87675095;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ87675095 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ60099242;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ60099242 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ46711017;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ46711017 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ95752487;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ95752487 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ6267458;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ6267458 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ14662861;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ14662861 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ39953398;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ39953398 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ942747;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ942747 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ2845082;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ2845082 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ86856377;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ86856377 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ6980187;     BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ6980187 = BpvCUtpDCUNOuhmuuhYkKYbzBwCBnItgjCWJ73996540;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void aNWOAnvyblNKUYlfYKGEZCNnqmNhNZh5196359() {     double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD99296133 = -752319769;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD55772539 = -153604893;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD85075813 = -863887841;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD57806467 = 70314204;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD47173299 = -558065977;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD59490836 = 3310122;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD25134190 = -450022860;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD55432749 = -703079699;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD44964007 = -101742503;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD3092624 = 75478260;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD6344539 = -421355394;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD31251585 = -32390609;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD52323979 = -788101182;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD60866919 = -254368742;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD10719670 = -984203784;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD2427045 = -423283744;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD8885160 = -852852918;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD67864894 = -463943159;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD40122221 = -303047866;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD20101610 = -569924805;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD46442233 = -228460065;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD7516715 = -374046915;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD81064168 = -746225090;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD52352283 = -200581663;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD45492047 = 27616320;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD40501219 = -542616421;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD70158433 = -450618875;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD86886977 = -427078089;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD51259025 = -377911224;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD73035986 = -809092053;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD60315556 = -123319358;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD33434486 = -529617638;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD52805682 = -612792720;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD94003829 = -477744502;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD39985972 = -757852872;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD46004188 = -688798452;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD46785125 = -730171977;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD35627920 = -696741955;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD61226581 = -832079899;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD36450550 = -809054027;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD70496777 = -409504600;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD85458858 = -166449650;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD68341351 = -561305655;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD94320286 = -587731521;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD38892282 = 51627476;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD962288 = -83355411;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD35265160 = 42778722;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD33254780 = -389182756;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD62016773 = -415696693;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD95817740 = 41583352;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD64467250 = -792401870;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD56636234 = -641920364;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD30664426 = -636405499;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD31264605 = -863099781;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD58754125 = 54264577;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD91779418 = -278272855;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD74708371 = -407379803;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD32723530 = -563306178;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD12314420 = -957302116;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD6672080 = 84550443;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD89332403 = -546071004;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD38247212 = 77055229;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD4173725 = -225168476;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD71928021 = -292650451;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD42777067 = -801202382;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD72910053 = -891737756;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD78445902 = -419597889;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD58320150 = -210356680;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD20880948 = -496515871;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD64715482 = -195405333;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD55641920 = -693111767;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD73257239 = -56110964;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD6638313 = -631863261;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD3671672 = -493993839;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD49604833 = -60420206;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD60983374 = 37989585;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD39175364 = -812741260;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD86743881 = -58493570;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD13460001 = -152209140;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD44529760 = -889028270;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD5236060 = -485395144;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD36903653 = 38563880;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD24870205 = 88618603;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD55441284 = -319494576;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD8568736 = 83309816;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD3679322 = -481398995;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD2770060 = -893212140;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD21541077 = -749692940;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD35249704 = -432009079;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD48206553 = -379580017;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD71295816 = -181418650;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD14061595 = -66865800;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD23313501 = -739439840;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD54554502 = -816630342;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD47118146 = -162983024;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD32249565 = -386559829;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD81285134 = -941281174;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD96413330 = -168655205;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD51543220 = -786529139;    double rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD65982228 = -752319769;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD99296133 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD55772539;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD55772539 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD85075813;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD85075813 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD57806467;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD57806467 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD47173299;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD47173299 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD59490836;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD59490836 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD25134190;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD25134190 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD55432749;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD55432749 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD44964007;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD44964007 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD3092624;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD3092624 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD6344539;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD6344539 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD31251585;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD31251585 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD52323979;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD52323979 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD60866919;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD60866919 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD10719670;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD10719670 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD2427045;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD2427045 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD8885160;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD8885160 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD67864894;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD67864894 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD40122221;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD40122221 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD20101610;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD20101610 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD46442233;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD46442233 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD7516715;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD7516715 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD81064168;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD81064168 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD52352283;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD52352283 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD45492047;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD45492047 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD40501219;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD40501219 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD70158433;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD70158433 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD86886977;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD86886977 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD51259025;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD51259025 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD73035986;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD73035986 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD60315556;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD60315556 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD33434486;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD33434486 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD52805682;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD52805682 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD94003829;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD94003829 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD39985972;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD39985972 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD46004188;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD46004188 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD46785125;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD46785125 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD35627920;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD35627920 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD61226581;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD61226581 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD36450550;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD36450550 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD70496777;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD70496777 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD85458858;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD85458858 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD68341351;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD68341351 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD94320286;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD94320286 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD38892282;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD38892282 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD962288;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD962288 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD35265160;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD35265160 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD33254780;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD33254780 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD62016773;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD62016773 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD95817740;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD95817740 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD64467250;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD64467250 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD56636234;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD56636234 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD30664426;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD30664426 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD31264605;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD31264605 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD58754125;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD58754125 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD91779418;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD91779418 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD74708371;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD74708371 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD32723530;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD32723530 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD12314420;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD12314420 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD6672080;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD6672080 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD89332403;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD89332403 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD38247212;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD38247212 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD4173725;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD4173725 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD71928021;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD71928021 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD42777067;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD42777067 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD72910053;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD72910053 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD78445902;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD78445902 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD58320150;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD58320150 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD20880948;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD20880948 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD64715482;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD64715482 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD55641920;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD55641920 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD73257239;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD73257239 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD6638313;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD6638313 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD3671672;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD3671672 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD49604833;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD49604833 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD60983374;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD60983374 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD39175364;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD39175364 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD86743881;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD86743881 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD13460001;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD13460001 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD44529760;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD44529760 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD5236060;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD5236060 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD36903653;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD36903653 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD24870205;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD24870205 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD55441284;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD55441284 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD8568736;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD8568736 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD3679322;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD3679322 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD2770060;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD2770060 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD21541077;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD21541077 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD35249704;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD35249704 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD48206553;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD48206553 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD71295816;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD71295816 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD14061595;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD14061595 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD23313501;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD23313501 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD54554502;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD54554502 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD47118146;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD47118146 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD32249565;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD32249565 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD81285134;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD81285134 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD96413330;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD96413330 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD51543220;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD51543220 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD65982228;     rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD65982228 = rGEzSxJbWBbKJfkWVNSUzPtSXAiXOZOlBDWD99296133;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void lUPEqVPXzNaPPlCwEzgiEkDnZdgzaav49173821() {     int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj64139343 = -947529581;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj51335232 = -473526688;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj68152614 = -650157365;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj3089206 = -662503178;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj51242486 = -447757373;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj84106970 = -943862798;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj28194583 = -896579284;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj15110257 = -165161888;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj62051283 = -102607980;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj69864556 = -214795716;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj12933493 = -447187624;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj88185099 = -524646789;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj69744002 = 11049847;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj27366778 = -732353397;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj99656061 = -585733744;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj57234828 = -357489454;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj99605485 = -379082653;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj44911573 = 6480081;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj48926599 = -167924824;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj86717092 = -789237860;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj78706296 = -714487738;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj87444407 = -758327377;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj97188294 = -383310248;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj20017027 = -989605729;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj22304746 = -79152549;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj69556359 = -609546924;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj16654593 = -957386398;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj33520173 = -609763755;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj23056715 = -535165859;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj1429482 = -771433462;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj54728490 = -873303338;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj12369071 = -713384834;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj27103342 = -562048304;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj92074411 = 22904572;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj51558390 = -431487972;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj35044797 = 57807823;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj84937340 = -150377023;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj88257990 = -916995657;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj95560838 = -446133754;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj87060664 = -121372557;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj80306363 = -47686123;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj79620979 = 85858266;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj61005143 = -367165307;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj86255285 = -447528530;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj63795062 = -729314381;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj65851796 = -585476755;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj25530802 = -506230561;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj7612264 = -85437761;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj95292793 = -161644426;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj22501363 = -326819403;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj16450877 = -678965863;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj58752618 = 56761739;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj37760309 = -727858431;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj87991822 = -428450737;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj70467524 = -589975220;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj76694935 = -89202204;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj54146937 = 9783560;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj48135587 = -660551636;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj80784459 = -483350630;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj81686126 = -838210449;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj67452378 = -986476401;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj94674410 = -186815529;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj92053541 = -629996029;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj60621802 = -331174518;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj15136067 = -341492379;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj564422 = -733802790;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj61081758 = -962598486;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj77669591 = 88145274;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj75808388 = -200865426;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj64611264 = -543541568;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj72297487 = -107112431;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj11347495 = -462086997;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj49350734 = -547386165;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj61865935 = 53447732;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj6410730 = -641551738;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj99085316 = -700346005;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj26439265 = -291162070;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj10933010 = -935781718;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj56221965 = -160291349;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj56452950 = -493675795;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj44025558 = -3316364;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj9042329 = -771948638;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj38227379 = -348119330;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj555353 = -108346457;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj84978604 = 7532401;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj95975872 = -830065078;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj74608761 = -985526404;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj39111520 = -33597567;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj21606887 = -387120208;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj74863454 = -242285768;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj80897859 = -951975737;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj36801753 = -489825388;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj7473532 = -333645027;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj13874712 = -607923305;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj19608286 = -134896156;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj85631953 = -860870594;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj87567438 = -284145705;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj383341 = 64009211;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj71119218 = -6036152;    int RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj63230640 = -947529581;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj64139343 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj51335232;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj51335232 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj68152614;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj68152614 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj3089206;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj3089206 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj51242486;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj51242486 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj84106970;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj84106970 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj28194583;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj28194583 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj15110257;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj15110257 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj62051283;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj62051283 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj69864556;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj69864556 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj12933493;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj12933493 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj88185099;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj88185099 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj69744002;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj69744002 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj27366778;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj27366778 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj99656061;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj99656061 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj57234828;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj57234828 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj99605485;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj99605485 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj44911573;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj44911573 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj48926599;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj48926599 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj86717092;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj86717092 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj78706296;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj78706296 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj87444407;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj87444407 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj97188294;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj97188294 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj20017027;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj20017027 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj22304746;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj22304746 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj69556359;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj69556359 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj16654593;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj16654593 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj33520173;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj33520173 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj23056715;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj23056715 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj1429482;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj1429482 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj54728490;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj54728490 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj12369071;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj12369071 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj27103342;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj27103342 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj92074411;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj92074411 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj51558390;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj51558390 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj35044797;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj35044797 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj84937340;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj84937340 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj88257990;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj88257990 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj95560838;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj95560838 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj87060664;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj87060664 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj80306363;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj80306363 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj79620979;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj79620979 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj61005143;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj61005143 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj86255285;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj86255285 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj63795062;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj63795062 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj65851796;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj65851796 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj25530802;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj25530802 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj7612264;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj7612264 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj95292793;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj95292793 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj22501363;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj22501363 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj16450877;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj16450877 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj58752618;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj58752618 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj37760309;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj37760309 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj87991822;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj87991822 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj70467524;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj70467524 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj76694935;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj76694935 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj54146937;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj54146937 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj48135587;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj48135587 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj80784459;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj80784459 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj81686126;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj81686126 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj67452378;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj67452378 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj94674410;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj94674410 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj92053541;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj92053541 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj60621802;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj60621802 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj15136067;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj15136067 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj564422;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj564422 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj61081758;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj61081758 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj77669591;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj77669591 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj75808388;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj75808388 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj64611264;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj64611264 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj72297487;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj72297487 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj11347495;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj11347495 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj49350734;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj49350734 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj61865935;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj61865935 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj6410730;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj6410730 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj99085316;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj99085316 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj26439265;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj26439265 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj10933010;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj10933010 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj56221965;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj56221965 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj56452950;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj56452950 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj44025558;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj44025558 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj9042329;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj9042329 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj38227379;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj38227379 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj555353;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj555353 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj84978604;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj84978604 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj95975872;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj95975872 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj74608761;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj74608761 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj39111520;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj39111520 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj21606887;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj21606887 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj74863454;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj74863454 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj80897859;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj80897859 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj36801753;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj36801753 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj7473532;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj7473532 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj13874712;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj13874712 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj19608286;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj19608286 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj85631953;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj85631953 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj87567438;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj87567438 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj383341;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj383341 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj71119218;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj71119218 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj63230640;     RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj63230640 = RfLKNTNxRuRKpAkQtLWAyVWMAQcfMpQxYSJj64139343;}
// Junk Finished
