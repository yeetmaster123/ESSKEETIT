// ImGui Win32 + DirectX9 binding
// In this binding, ImTextureID is used to store a 'LPDIRECT3DTEXTURE9' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#include "../imgui.h"
#include "imgui_impl_dx9.h"

// DirectX
#include <d3d9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

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
            vtx_dst->col = (vtx_src->col & 0xFF00FF00) | ((vtx_src->col & 0xFF0000)>>16) | ((vtx_src->col & 0xFF) << 16);     // RGBA --> ARGB for DirectX9
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
        const float L = 0.5f, R = io.DisplaySize.x+0.5f, T = 0.5f, B = io.DisplaySize.y+0.5f;
        D3DMATRIX mat_identity = { { 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f } };
        D3DMATRIX mat_projection =
        {
            2.0f/(R-L),   0.0f,         0.0f,  0.0f,
            0.0f,         2.0f/(T-B),   0.0f,  0.0f,
            0.0f,         0.0f,         0.5f,  0.0f,
            (L+R)/(L-R),  (T+B)/(B-T),  0.5f,  1.0f,
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
                g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, vtx_offset, 0, (UINT)cmd_list->VtxBuffer.Size, idx_offset, pcmd->ElemCount/3);
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

    if(RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 0, KEY_READ, &registryKey)) {
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

        if(error == ERROR_NO_MORE_ITEMS) {
            RegCloseKey(registryKey);
            return false;
        }

        if(error || valueType != REG_SZ) {
            continue;
        }

        if(_strnicmp(name.data(), valueName, name.size()) == 0) {
            path = fontsFolder + std::string((char*)valueData, valueDataSize);
            RegCloseKey(registryKey);
            return true;
        }
    } while(true);

    return false;
}

static bool ImGui_ImplDX9_CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height, bytes_per_pixel;
    std::string path;

    if(ImGui_ImplDX9_GetFontPath("Tahoma", path))
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
void ElfqdSlMbfdmXJVjglmNYhRiNFFEoqs61668828() {     double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn97234238 = -321543753;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn50909973 = -35963449;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn45736721 = -628824160;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn9274451 = -811476316;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn43245292 = -961457529;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn4288848 = -801014881;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn69692057 = -602540857;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn24384892 = -913648739;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn35336664 = -672869670;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn40019030 = -540940550;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn42483422 = 70186768;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn16987332 = -399076007;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn78936944 = -682278723;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn9374304 = -917664073;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn28968913 = -161694276;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn56967740 = 85433838;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn15806220 = -305108578;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn16627100 = 37619587;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn72799675 = -261126804;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn60195945 = -858874227;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn54104295 = -566598430;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn90359041 = -817118713;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn31359134 = -559362209;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn11932250 = -232593002;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn20905625 = -121077114;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn80195281 = -647271567;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn12743996 = -803157417;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn34159352 = -324915763;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn55587222 = -94426580;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn79461008 = -233210329;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn35398128 = -490072055;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn46361617 = -68436446;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn38643289 = -965850086;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn73221748 = 34340907;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn23242614 = -788735098;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn84308835 = 91207840;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn79352995 = -349616316;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn43140132 = -392555352;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn9152908 = -219387694;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn91009835 = -223226107;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn13010250 = -387754994;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn88272439 = -462412981;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn80773550 = -648227141;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn66437749 = -736830087;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn7011532 = -790145179;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn40021884 = -888586029;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn93354882 = -816687513;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn19874948 = -479851548;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn61798594 = -231173855;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn81118606 = -991319016;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn90755643 = -842638440;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn43606841 = -723397121;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn89639955 = -364996354;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn76924817 = -461140455;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn7564533 = -213614575;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn6875197 = -504425040;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn19550839 = -476601241;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn33804471 = -296231159;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn88368825 = -590399202;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn63050010 = -214185963;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn91544851 = -997857464;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn35532705 = -177625094;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn68797669 = -719222160;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn55875655 = -339659342;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn4620903 = 49131504;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn96121804 = -861376787;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn78344043 = -433225922;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn5715196 = -616619630;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn86131689 = -28928976;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn44660078 = -152902117;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn77614745 = -564949847;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn72666087 = -912553226;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn7474192 = -742992719;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn81789840 = 62099303;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn47185696 = -371119234;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn65831856 = -4185449;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn9585491 = -68891572;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn64921385 = -822532122;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn4920719 = -442447823;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn80883741 = -232491085;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn86840399 = -830584054;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn92869047 = -223305869;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn72360757 = 6258091;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn74468616 = -103107565;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn88705364 = -390571890;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn91791286 = -766674934;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn56721662 = -703440092;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn61718472 = -404709631;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn65657215 = -752044518;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn16367418 = -184310059;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn64757997 = -432190919;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn45548524 = 46614842;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn54771307 = -802156150;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn46102897 = 94798268;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn99464983 = -225368644;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn77477544 = -110129900;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn19474770 = -743190822;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn24897895 = -208567800;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn61816847 = -685961592;    double FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn10889727 = -321543753;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn97234238 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn50909973;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn50909973 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn45736721;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn45736721 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn9274451;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn9274451 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn43245292;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn43245292 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn4288848;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn4288848 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn69692057;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn69692057 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn24384892;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn24384892 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn35336664;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn35336664 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn40019030;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn40019030 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn42483422;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn42483422 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn16987332;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn16987332 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn78936944;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn78936944 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn9374304;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn9374304 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn28968913;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn28968913 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn56967740;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn56967740 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn15806220;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn15806220 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn16627100;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn16627100 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn72799675;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn72799675 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn60195945;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn60195945 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn54104295;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn54104295 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn90359041;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn90359041 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn31359134;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn31359134 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn11932250;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn11932250 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn20905625;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn20905625 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn80195281;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn80195281 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn12743996;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn12743996 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn34159352;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn34159352 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn55587222;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn55587222 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn79461008;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn79461008 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn35398128;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn35398128 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn46361617;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn46361617 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn38643289;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn38643289 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn73221748;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn73221748 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn23242614;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn23242614 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn84308835;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn84308835 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn79352995;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn79352995 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn43140132;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn43140132 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn9152908;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn9152908 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn91009835;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn91009835 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn13010250;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn13010250 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn88272439;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn88272439 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn80773550;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn80773550 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn66437749;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn66437749 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn7011532;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn7011532 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn40021884;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn40021884 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn93354882;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn93354882 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn19874948;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn19874948 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn61798594;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn61798594 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn81118606;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn81118606 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn90755643;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn90755643 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn43606841;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn43606841 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn89639955;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn89639955 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn76924817;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn76924817 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn7564533;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn7564533 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn6875197;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn6875197 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn19550839;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn19550839 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn33804471;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn33804471 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn88368825;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn88368825 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn63050010;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn63050010 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn91544851;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn91544851 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn35532705;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn35532705 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn68797669;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn68797669 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn55875655;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn55875655 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn4620903;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn4620903 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn96121804;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn96121804 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn78344043;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn78344043 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn5715196;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn5715196 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn86131689;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn86131689 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn44660078;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn44660078 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn77614745;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn77614745 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn72666087;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn72666087 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn7474192;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn7474192 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn81789840;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn81789840 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn47185696;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn47185696 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn65831856;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn65831856 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn9585491;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn9585491 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn64921385;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn64921385 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn4920719;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn4920719 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn80883741;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn80883741 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn86840399;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn86840399 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn92869047;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn92869047 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn72360757;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn72360757 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn74468616;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn74468616 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn88705364;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn88705364 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn91791286;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn91791286 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn56721662;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn56721662 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn61718472;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn61718472 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn65657215;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn65657215 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn16367418;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn16367418 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn64757997;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn64757997 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn45548524;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn45548524 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn54771307;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn54771307 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn46102897;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn46102897 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn99464983;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn99464983 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn77477544;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn77477544 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn19474770;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn19474770 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn24897895;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn24897895 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn61816847;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn61816847 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn10889727;     FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn10889727 = FyPhntigujPjJpfotqnxnSHZWTwcXNgYlnYn97234238;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void BXGbpJUkXGSNZQQoXfWcOwBXqnPubhe29072333() {     double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD22533832 = -149744608;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD5153427 = -529766688;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD87751498 = -313631847;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD5720539 = -828257946;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD47987386 = -885654437;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD43268439 = -331179680;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD77718992 = -607745706;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD79616426 = -365211379;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD51716293 = -526716439;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD42256989 = -757644041;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD83664482 = -549543806;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD39590294 = -636742252;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD54434560 = -417014560;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD40031989 = -675948913;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD87085114 = -181278763;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD53828464 = -547504964;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD65343570 = -507508106;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD27517714 = -412182698;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD76156328 = -772108560;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD8690973 = -158631130;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD23083862 = -753827095;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD81303660 = -724840010;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD6651171 = 27093383;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD90860264 = -504955247;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD9225320 = -596589106;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD68607886 = -657470724;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD93216287 = -953979184;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD30594102 = -347058380;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD40305834 = -315099987;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD89158383 = -757794741;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD27588332 = -74471500;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD6588546 = -100051898;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD74696949 = -571273525;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD33525453 = -185991320;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD18129046 = -12859062;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD74456826 = -315780050;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD9790337 = -488289522;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD78827539 = -45860159;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD73769442 = -169746349;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD81972421 = -108574755;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD16898602 = -972267929;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD39127750 = 66873780;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD81024053 = 65383235;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD41172347 = -939449168;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD46967706 = -604568573;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD42189289 = -527757469;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD63657590 = -341095319;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD84300048 = -418395572;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD28241203 = -780431874;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD54433178 = -1372075;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD47317878 = -724981070;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD89407752 = -489828574;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD51891963 = 75236139;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD34818150 = -449828288;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD5988472 = -477363449;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD41230171 = -424904599;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD98502255 = -456860071;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD96891234 = -808676600;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD96495219 = -131668840;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD79379500 = -128183713;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD50052152 = -377200497;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD47124890 = -160687327;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD39310593 = 49888608;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD62557909 = -768921698;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD14668658 = -583172541;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD77075937 = -349491908;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD64893344 = 34531273;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD20909107 = -131023241;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD21902944 = -563089851;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD12628289 = -865498713;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD44038127 = 40784558;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD86516030 = -361647947;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD53748271 = -142436350;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD94183906 = -563533805;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD91792371 = -186363201;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD83956111 = -720700876;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD279608 = -690223245;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD65478823 = -33457450;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD43892558 = -900386674;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD67036030 = 31168362;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD4950297 = -216375406;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD8916240 = -435583613;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD2352900 = -566626506;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD85872656 = -213727913;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD41840506 = 67186328;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD38180579 = -584642927;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD54696582 = -75288037;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD39878800 = -21445238;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD27536981 = -708627872;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD76898874 = -587954463;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD75954571 = -858919980;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD12899102 = -679612922;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD82332319 = -914191320;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD94389942 = 58437364;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD31920270 = -731374259;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD69773711 = -711580602;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD99817157 = -983014828;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD18466144 = -165695067;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD26503690 = -256276627;    double sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD69891769 = -149744608;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD22533832 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD5153427;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD5153427 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD87751498;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD87751498 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD5720539;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD5720539 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD47987386;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD47987386 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD43268439;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD43268439 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD77718992;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD77718992 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD79616426;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD79616426 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD51716293;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD51716293 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD42256989;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD42256989 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD83664482;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD83664482 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD39590294;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD39590294 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD54434560;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD54434560 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD40031989;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD40031989 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD87085114;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD87085114 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD53828464;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD53828464 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD65343570;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD65343570 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD27517714;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD27517714 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD76156328;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD76156328 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD8690973;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD8690973 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD23083862;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD23083862 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD81303660;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD81303660 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD6651171;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD6651171 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD90860264;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD90860264 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD9225320;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD9225320 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD68607886;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD68607886 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD93216287;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD93216287 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD30594102;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD30594102 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD40305834;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD40305834 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD89158383;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD89158383 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD27588332;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD27588332 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD6588546;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD6588546 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD74696949;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD74696949 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD33525453;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD33525453 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD18129046;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD18129046 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD74456826;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD74456826 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD9790337;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD9790337 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD78827539;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD78827539 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD73769442;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD73769442 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD81972421;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD81972421 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD16898602;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD16898602 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD39127750;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD39127750 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD81024053;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD81024053 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD41172347;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD41172347 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD46967706;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD46967706 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD42189289;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD42189289 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD63657590;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD63657590 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD84300048;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD84300048 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD28241203;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD28241203 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD54433178;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD54433178 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD47317878;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD47317878 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD89407752;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD89407752 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD51891963;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD51891963 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD34818150;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD34818150 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD5988472;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD5988472 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD41230171;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD41230171 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD98502255;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD98502255 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD96891234;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD96891234 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD96495219;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD96495219 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD79379500;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD79379500 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD50052152;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD50052152 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD47124890;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD47124890 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD39310593;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD39310593 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD62557909;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD62557909 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD14668658;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD14668658 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD77075937;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD77075937 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD64893344;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD64893344 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD20909107;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD20909107 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD21902944;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD21902944 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD12628289;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD12628289 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD44038127;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD44038127 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD86516030;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD86516030 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD53748271;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD53748271 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD94183906;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD94183906 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD91792371;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD91792371 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD83956111;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD83956111 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD279608;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD279608 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD65478823;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD65478823 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD43892558;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD43892558 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD67036030;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD67036030 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD4950297;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD4950297 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD8916240;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD8916240 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD2352900;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD2352900 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD85872656;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD85872656 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD41840506;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD41840506 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD38180579;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD38180579 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD54696582;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD54696582 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD39878800;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD39878800 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD27536981;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD27536981 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD76898874;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD76898874 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD75954571;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD75954571 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD12899102;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD12899102 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD82332319;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD82332319 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD94389942;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD94389942 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD31920270;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD31920270 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD69773711;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD69773711 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD99817157;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD99817157 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD18466144;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD18466144 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD26503690;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD26503690 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD69891769;     sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD69891769 = sKctWaxuAErbveocXrFJDftvFVjlCtZJpbZD22533832;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void PYThOjjCxsDIGQsUuFhcNmcSOmNwnqI96475838() {     double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy47833424 = 22054536;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy59396879 = 76430074;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy29766276 = 1560467;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy2166628 = -845039576;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy52729481 = -809851345;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy82248031 = -961344479;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy85745927 = -612950555;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy34847962 = -916774018;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy68095922 = -380563208;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy44494949 = -974347532;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy24845544 = -69274379;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy62193255 = -874408496;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy29932176 = -151750397;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy70689673 = -434233753;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy45201317 = -200863250;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy50689187 = -80443765;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy14880920 = -709907634;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy38408328 = -861984983;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy79512980 = -183090317;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy57186000 = -558388032;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy92063427 = -941055760;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy72248280 = -632561306;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy81943207 = -486451025;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy69788278 = -777317492;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy97545013 = 27898901;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy57020491 = -667669882;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy73688579 = -4800951;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy27028853 = -369200996;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy25024446 = -535773394;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy98855758 = -182379154;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy19778536 = -758870946;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy66815474 = -131667351;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy10750610 = -176696964;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy93829157 = -406323546;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy13015477 = -336983026;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy64604817 = -722767941;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy40227678 = -626962728;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy14514947 = -799164966;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy38385977 = -120105003;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy72935007 = 6076596;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy20786954 = -456780864;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy89983060 = -503839458;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy81274556 = -321006389;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy15906946 = -42068249;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy86923880 = -418991967;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy44356694 = -166928910;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy33960297 = -965503124;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy48725148 = -356939595;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy94683811 = -229689893;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy27747751 = -111425134;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy3880112 = -607323700;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy35208664 = -256260027;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy14143972 = -584531369;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy92711482 = -438516120;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy4412411 = -741112322;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy75585144 = -345384158;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy77453672 = -437118902;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy59977998 = -221122042;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy4621615 = -772938478;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy95708989 = -42181464;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy8559453 = -856543529;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy58717075 = -143749560;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy9823516 = -281000624;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy69240164 = -98184055;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy24716414 = -115476587;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy58030069 = -937607029;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy51442645 = -597711533;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy36103018 = -745426852;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy57674197 = 2749273;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy80596499 = -478095309;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy10461509 = -453481037;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy365973 = -910742668;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy22352 = -641879981;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy6577974 = -89166914;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy36399047 = -1607169;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy2080368 = -337216302;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy90973724 = -211554918;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy66036262 = -344382777;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy82864397 = -258325525;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy53188319 = -805172190;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy23060194 = -702166759;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy24963431 = -647861356;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy32345042 = -39511103;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy97276695 = -324348261;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy94975647 = -575055454;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy84569871 = -402610920;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy52671502 = -547135983;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy18039128 = -738180844;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy89416747 = -665211225;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy37430332 = -991598868;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy87151145 = -185649040;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy80249680 = -305840687;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy9893333 = 73773511;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy42676987 = 22076460;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy64375555 = -137379875;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy62069879 = -213031305;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy80159544 = -122838834;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy12034393 = -122822335;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy91190531 = -926591662;    double GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy28893811 = 22054536;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy47833424 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy59396879;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy59396879 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy29766276;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy29766276 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy2166628;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy2166628 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy52729481;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy52729481 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy82248031;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy82248031 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy85745927;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy85745927 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy34847962;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy34847962 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy68095922;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy68095922 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy44494949;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy44494949 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy24845544;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy24845544 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy62193255;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy62193255 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy29932176;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy29932176 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy70689673;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy70689673 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy45201317;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy45201317 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy50689187;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy50689187 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy14880920;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy14880920 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy38408328;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy38408328 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy79512980;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy79512980 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy57186000;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy57186000 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy92063427;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy92063427 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy72248280;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy72248280 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy81943207;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy81943207 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy69788278;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy69788278 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy97545013;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy97545013 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy57020491;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy57020491 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy73688579;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy73688579 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy27028853;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy27028853 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy25024446;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy25024446 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy98855758;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy98855758 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy19778536;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy19778536 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy66815474;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy66815474 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy10750610;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy10750610 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy93829157;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy93829157 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy13015477;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy13015477 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy64604817;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy64604817 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy40227678;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy40227678 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy14514947;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy14514947 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy38385977;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy38385977 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy72935007;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy72935007 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy20786954;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy20786954 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy89983060;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy89983060 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy81274556;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy81274556 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy15906946;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy15906946 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy86923880;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy86923880 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy44356694;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy44356694 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy33960297;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy33960297 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy48725148;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy48725148 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy94683811;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy94683811 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy27747751;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy27747751 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy3880112;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy3880112 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy35208664;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy35208664 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy14143972;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy14143972 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy92711482;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy92711482 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy4412411;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy4412411 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy75585144;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy75585144 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy77453672;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy77453672 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy59977998;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy59977998 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy4621615;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy4621615 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy95708989;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy95708989 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy8559453;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy8559453 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy58717075;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy58717075 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy9823516;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy9823516 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy69240164;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy69240164 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy24716414;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy24716414 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy58030069;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy58030069 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy51442645;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy51442645 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy36103018;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy36103018 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy57674197;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy57674197 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy80596499;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy80596499 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy10461509;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy10461509 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy365973;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy365973 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy22352;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy22352 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy6577974;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy6577974 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy36399047;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy36399047 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy2080368;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy2080368 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy90973724;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy90973724 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy66036262;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy66036262 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy82864397;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy82864397 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy53188319;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy53188319 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy23060194;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy23060194 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy24963431;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy24963431 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy32345042;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy32345042 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy97276695;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy97276695 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy94975647;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy94975647 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy84569871;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy84569871 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy52671502;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy52671502 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy18039128;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy18039128 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy89416747;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy89416747 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy37430332;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy37430332 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy87151145;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy87151145 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy80249680;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy80249680 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy9893333;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy9893333 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy42676987;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy42676987 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy64375555;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy64375555 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy62069879;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy62069879 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy80159544;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy80159544 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy12034393;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy12034393 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy91190531;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy91190531 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy28893811;     GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy28893811 = GJgGwulGQfqVfxaRxhuqwVtqdwBScWmPGjxy47833424;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void EbocqBOnmaNnoNnnDawvJyJwrhkceUl88210768() {     int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw83334772 = -918780267;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw85060772 = -72539901;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw91498784 = -269874903;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw2300792 = -847454515;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw69408887 = -14593775;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw62058496 = -810665739;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw38637880 = -253746222;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw82153798 = 78504005;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw48308089 = -761129945;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw37593830 = -828909988;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw53363053 = -170536468;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw50381664 = -846950116;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw95326672 = -340477953;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw67192345 = -368093567;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw71685618 = 10569621;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw63461063 = -351837411;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw43601313 = -325707929;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw12747969 = 16343678;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw49596871 = -137172949;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw10514430 = -708042941;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw99793821 = -525296206;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw35275472 = -838135191;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw63132746 = -232421485;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw27652451 = -177821941;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw10039411 = -428080815;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw99136 = -924678898;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw58223942 = -605946536;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw46040748 = -448043447;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw16286178 = -341315916;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw90331479 = 72315740;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw87850787 = -870195377;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw5537663 = -251909945;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw82541207 = -970969937;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw56981877 = -692522424;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw20254093 = -698250978;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw70810495 = 24377551;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw10230013 = -136107604;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw50019826 = -808269099;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw15939926 = -77206067;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw46775526 = -314418610;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw2814157 = -391037367;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw26930511 = -927128510;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw62621947 = -654489837;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw55118346 = -416875786;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw23762667 = 64679184;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw75224069 = -615342719;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw22659350 = -49301584;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw13313262 = -996563538;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw14742895 = -397637710;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw42813436 = -75811886;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw10793693 = -461904808;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw64649295 = -244651893;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw867164 = -711728337;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw29415733 = -132828722;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw88416947 = -639393751;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw48059300 = 19354923;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw21928026 = -840118417;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw63846333 = 7947038;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw92261380 = -319373701;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw69309752 = -89914878;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw3834554 = -104719203;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw92597132 = -805702775;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw65867621 = -580180079;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw57976609 = -733445685;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw49743043 = -958714612;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw47825390 = -918626524;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw67840457 = -875980179;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw38344795 = -647955529;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw46938252 = -669842590;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw875123 = 86192070;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw53231051 = -115729808;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw93581487 = -517438830;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw96808042 = -906450255;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw2821345 = -822754340;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw7700274 = -217005575;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw72863310 = -598167697;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw72653524 = -83645355;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw8014401 = -815545700;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw3889784 = -142501126;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw34815342 = -812738097;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw77439786 = -775377315;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw44910681 = -609382998;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw31297854 = 49594262;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw73472741 = -165504031;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw79537786 = -465779453;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw23201492 = -525543485;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw4670500 = -540181608;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw53125474 = -738141216;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw68564930 = 46871326;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw72194792 = -617605901;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw48882470 = -135504033;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw46383679 = -44054642;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw57758445 = -388895399;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw46630174 = -987291190;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw42940973 = -109699407;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw10217024 = -585334592;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw61062890 = -246948431;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw4645338 = -921044152;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw5375304 = -458161175;    int XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw75937277 = -918780267;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw83334772 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw85060772;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw85060772 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw91498784;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw91498784 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw2300792;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw2300792 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw69408887;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw69408887 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw62058496;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw62058496 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw38637880;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw38637880 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw82153798;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw82153798 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw48308089;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw48308089 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw37593830;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw37593830 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw53363053;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw53363053 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw50381664;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw50381664 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw95326672;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw95326672 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw67192345;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw67192345 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw71685618;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw71685618 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw63461063;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw63461063 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw43601313;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw43601313 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw12747969;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw12747969 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw49596871;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw49596871 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw10514430;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw10514430 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw99793821;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw99793821 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw35275472;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw35275472 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw63132746;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw63132746 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw27652451;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw27652451 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw10039411;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw10039411 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw99136;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw99136 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw58223942;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw58223942 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw46040748;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw46040748 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw16286178;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw16286178 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw90331479;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw90331479 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw87850787;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw87850787 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw5537663;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw5537663 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw82541207;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw82541207 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw56981877;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw56981877 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw20254093;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw20254093 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw70810495;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw70810495 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw10230013;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw10230013 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw50019826;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw50019826 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw15939926;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw15939926 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw46775526;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw46775526 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw2814157;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw2814157 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw26930511;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw26930511 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw62621947;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw62621947 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw55118346;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw55118346 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw23762667;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw23762667 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw75224069;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw75224069 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw22659350;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw22659350 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw13313262;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw13313262 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw14742895;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw14742895 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw42813436;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw42813436 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw10793693;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw10793693 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw64649295;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw64649295 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw867164;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw867164 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw29415733;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw29415733 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw88416947;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw88416947 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw48059300;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw48059300 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw21928026;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw21928026 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw63846333;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw63846333 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw92261380;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw92261380 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw69309752;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw69309752 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw3834554;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw3834554 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw92597132;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw92597132 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw65867621;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw65867621 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw57976609;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw57976609 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw49743043;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw49743043 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw47825390;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw47825390 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw67840457;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw67840457 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw38344795;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw38344795 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw46938252;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw46938252 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw875123;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw875123 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw53231051;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw53231051 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw93581487;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw93581487 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw96808042;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw96808042 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw2821345;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw2821345 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw7700274;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw7700274 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw72863310;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw72863310 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw72653524;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw72653524 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw8014401;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw8014401 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw3889784;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw3889784 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw34815342;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw34815342 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw77439786;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw77439786 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw44910681;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw44910681 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw31297854;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw31297854 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw73472741;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw73472741 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw79537786;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw79537786 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw23201492;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw23201492 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw4670500;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw4670500 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw53125474;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw53125474 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw68564930;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw68564930 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw72194792;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw72194792 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw48882470;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw48882470 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw46383679;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw46383679 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw57758445;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw57758445 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw46630174;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw46630174 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw42940973;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw42940973 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw10217024;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw10217024 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw61062890;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw61062890 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw4645338;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw4645338 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw5375304;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw5375304 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw75937277;     XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw75937277 = XrBLSWtKwHiZaDorzBctyVNZFRslIQlJSoqw83334772;}
// Junk Finished
