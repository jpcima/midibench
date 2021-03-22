#pragma once
#include <imgui.h>

bool ToggleButtonEx(const char* label, bool* value, const ImVec2& size_arg = ImVec2(0, 0), ImGuiButtonFlags flags = 0);
bool ToggleButton(const char* label, bool* value, const ImVec2& size_arg = ImVec2(0, 0));
bool KickButton(const char* label, bool* value, const ImVec2& size_arg = ImVec2(0, 0));
