#define _CRT_SECURE_NO_WARNINGS
#include "ui.hh"
#include "../globals.hh"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

void ui::render() {
    if (!globals.active) return;

    ImGui::SetNextWindowPos(ImVec2(window_pos.x, window_pos.y), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(window_size.x, window_size.y));
    ImGui::SetNextWindowBgAlpha(1.0f);

    ImGui::Begin(window_title, &globals.active, window_flags);
    {
        if (ImGui::BeginTabBar("MainTabs")) {
            if (ImGui::BeginTabItem("Clicker")) {
                ImGui::Spacing();
                ImGui::Checkbox("LeftClicker", &clicker::LeftClicker);
                if (clicker::LeftClicker) {
                    ImGui::SliderInt("minCPS", &clicker::minCPS, 1, 20, "%d Min");
                    ImGui::SliderInt("maxCPS", &clicker::maxCPS, 1, 20, "%d Max");
                }
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Checkbox("RightClicker", &clicker::RightClicker);
                if (clicker::RightClicker) {
                    ImGui::SliderInt("minRCPS", &clicker::minRCPS, 1, 20, "%d Min");
                    ImGui::SliderInt("maxRCPS", &clicker::maxRCPS, 1, 20, "%d Max");
                }
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Checkbox("Only MC", &clicker::OnlyMinecraft);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Reach")) {
                ImGui::Spacing();
                ImGui::Checkbox("Reach", &reach::C_reach);
                if (reach::C_reach) {
                    ImGui::SliderFloat("Value", &reach::reachValue, 3.0f, 6.0f, "%.02f Distance", ImGuiSliderFlags_None);
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Fake Lag")) {
                ImGui::Spacing();
                ImGui::Checkbox("Fake Lag", &lag::lagrange);
                if (lag::lagrange) {
                    ImGui::SliderInt("ms", &lag::cooldown, 100, 1000, "%d ms");
                    ImGui::SliderInt("every", &lag::everyms, 100, 1000, "Every %d ms");
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Utilities")) {
                ImGui::Spacing();
                ImGui::Checkbox("NoFall (Vanilla)", &utilities::nofall);
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Checkbox("Velocity", &utilities::velocity);
                if (utilities::velocity) {
                    ImGui::Checkbox("Velocity X & Z", &utilities::velocityz);
                    ImGui::Checkbox("Velocity Y", &utilities::velocityy);
                }
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Checkbox("Blink", &utilities::blink);
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Checkbox("Glide", &utilities::glide);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Settings")) {
                ImGui::Spacing();

                if (ImGui::InputText("Your DID", buffer, IM_ARRAYSIZE(buffer))) {
                    //user typing
                }
                if (ImGui::Button("Spoof DeviceID")) {
                    MessageBoxA(NULL, "We are now spoofing your DID. Please do not touch minecraft until this is finished!", "Spoofing...", MB_OK | MB_ICONINFORMATION);
                    std::string originalID(buffer);
                    if (!originalID.empty()) {
                        if (!memoWriter.AttachProcess()) {
                            MessageBoxA(NULL, "Failed to attach to process!", "Error", MB_OK | MB_ICONERROR);
                            return;
                        }

                        std::string newDeviceID = GenerateRandomDeviceID(originalID);

                        if (memoWriter.SpoofMemoryString(originalID, newDeviceID)) {
                            MessageBoxA(NULL, ("Successfully spoofed Device ID to: " + newDeviceID).c_str(), "Spoof Success", MB_OK | MB_ICONINFORMATION);
                            strncpy(buffer, newDeviceID.c_str(), IM_ARRAYSIZE(buffer)); // Unsafe but works
                        }
                        else {
                            MessageBoxA(NULL, "Failed to spoof DID!", "Error", MB_OK | MB_ICONERROR);
                        }
                    }
                }
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }
    ImGui::End();

    if (clicker::minCPS > clicker::maxCPS) {
        clicker::maxCPS = clicker::minCPS;
    }
    if (clicker::minRCPS > clicker::maxRCPS) {
        clicker::maxRCPS = clicker::minRCPS;
    }
}


void ui::init(LPDIRECT3DDEVICE9 device) {
    dev = device;

    // Colors
    ImGui::StyleColorsDark();

    if (window_pos.x == 0) {
        RECT screen_rect{};
        GetWindowRect(GetDesktopWindow(), &screen_rect);
        screen_res = ImVec2(float(screen_rect.right), float(screen_rect.bottom));
        window_pos = (screen_res - window_size) * 0.5f;
    }
}