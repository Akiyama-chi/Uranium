#include "Main.h"
#include "ui/ui.hh"
#include "globals.hh"
#include "modules/Clicker/Autoclicker.h"
#include "modules/reach/reach.h"
#include "modules/nofall/nofall.h"
#include "modules/nokb/velocityy.h"
#include "modules/nokb/velocityz.h"
#include "modules/blink/blink.h"
#include "modules/nodown/nodown.h"
#include "modules/lagrange/lagrange.h"
#include "Utils/hider.h"  // Adjust the path if necessary
#include <thread>

Reach reachHandler;
NoFall noFallHandler;
VelocityY velocityYHandler;
VelocityZ velocityZHandler;
Blink blinkHandler;
Glide glideHandler;
LagRange lagHandler;

void startReachThread() {
    reachHandler.startReachThread();
}
void startVelocityYThread() {
    velocityYHandler.startVelocityYThread();
}
void startVelocityZThread() {
    velocityZHandler.startVelocityZThread();
}

void startNoFallThread() {
    noFallHandler.startNoFallThread();
}

void startClickerThread() {
    clickerthread();
}
void startBlinkThread() {
    blinkHandler.startBlinkThread();
}
void startGlideThread() {
    glideHandler.startGlideThread();
}
void startLagRangeThread() {
    lagHandler.startLagRangeThread();
}

void startHiderThread() {
    while (globals.active) {  // globals.active should be false when the app exits
        if (GetAsyncKeyState(VK_INSERT) & 1) { // Detect INSERT key press
            Hider::ToggleHide();
        }
        Sleep(100); // Reduce CPU usage
    }
}



// Main code
int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // Create application window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, ui::window_title, NULL };
    RegisterClassEx(&wc);
    main_hwnd = CreateWindow(wc.lpszClassName, ui::window_title, WS_POPUP, 0, 0, 5, 5, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(main_hwnd)) {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ShowWindow(main_hwnd, SW_HIDE);
    UpdateWindow(main_hwnd);

    // Setup Dear ImGui context
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr; // Prevents creating imgui.ini file
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport

    // Setup ImGui styles
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 5.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(main_hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    std::thread reachThread(startReachThread);
    std::thread clickerThread(startClickerThread);
    std::thread noFallThread(startNoFallThread);
    std::thread velocityYThread(startVelocityYThread);
    std::thread velocityZThread(startVelocityZThread);
    std::thread blinkThread(startBlinkThread);
    std::thread glideThread(startGlideThread);
    std::thread lagThread(startLagRangeThread);
    std::thread hiderThread(startHiderThread);

    // Detach threads so they run independently
    reachThread.detach();
    clickerThread.detach();
    noFallThread.detach();
    velocityYThread.detach();
    velocityZThread.detach();
    blinkThread.detach();
    glideThread.detach();
    lagThread.detach();
    hiderThread.detach();  // Run independently

    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {
            static int init = false;
            if (!init) {
                ui::init(g_pd3dDevice);
                init = true;
            }
            else {
                ui::render();
            }
        }
        ImGui::EndFrame();

        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
            ResetDevice();
        }
        if (!globals.active) {
            msg.message = WM_QUIT;
        }
    }

    // Cleanup
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    DestroyWindow(main_hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Window Procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
