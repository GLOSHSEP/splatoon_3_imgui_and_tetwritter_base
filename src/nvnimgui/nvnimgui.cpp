#include "nvnimgui/nvnimgui.hpp"
#include "hk/hook/Trampoline.h"
#include "nvnimgui/imgui_hid_mappings.hpp"
#include "imgui.h"
#include "nn/hid.h"
#include "nvn/nvn_Cpp.h"
#include "hk/gfx/ImGuiBackendNvn.h"
#include "utility/input_mgr.hpp"
#include "utility/nya_log.hpp"

namespace nvnimgui {
    void update_mouse(ImGuiIO& io) {
        ImVec2 mousePos(0, 0);
        nya::hid::getMouseCoords(&mousePos.x, &mousePos.y);

        mousePos = ImVec2((mousePos.x / 1280.f) * io.DisplaySize.x,
            (mousePos.y / 720.f) * io.DisplaySize.y);

        io.AddMousePosEvent(mousePos.x, mousePos.y);

        ImVec2 scrollDelta(0, 0);
        nya::hid::getScrollDelta(&scrollDelta.x, &scrollDelta.y);

        if (scrollDelta.x != 0.0f)
            io.AddMouseWheelEvent(0.0f, scrollDelta.x > 0.0f ? 0.5f : -0.5f);

        for (auto [im_k, nx_k] : mouse_mapping) {
            if (nya::hid::isMousePress((nn::hid::MouseButton)nx_k)) {
                io.AddMouseButtonEvent((ImGuiMouseButton)im_k, true);
            } else if (nya::hid::isMouseRelease((nn::hid::MouseButton)nx_k)) {
                io.AddMouseButtonEvent((ImGuiMouseButton)im_k, false);
            }
        }
    }

    void update_keyboard(ImGuiIO& io) {
        io.AddKeyEvent(ImGuiMod_Shift, nya::hid::isModifierActive(nn::hid::KeyboardModifier::Shift));
        io.AddKeyEvent(ImGuiMod_Ctrl, nya::hid::isModifierActive(nn::hid::KeyboardModifier::Control));
        io.AddKeyEvent(ImGuiMod_Super, nya::hid::isModifierActive(nn::hid::KeyboardModifier::Gui));
        io.AddKeyEvent(ImGuiMod_Alt, nya::hid::isModifierActive(nn::hid::KeyboardModifier::LeftAlt) || nya::hid::isModifierActive(nn::hid::KeyboardModifier::RightAlt));

        bool isAltCode = nya::hid::isModifierActive(nn::hid::KeyboardModifier::CapsLock) || nya::hid::isModifierActive(nn::hid::KeyboardModifier::Shift);
        bool isNumLock = nya::hid::isModifierActive(nn::hid::KeyboardModifier::NumLock);

        for (auto [im_int, nx_int] : key_mapping) {
            auto nx_k = (nn::hid::KeyboardKey)nx_int;
            auto im_k = (ImGuiKey)im_int;

            if (nya::hid::isKeyPress(nx_k)) {
                io.AddKeyEvent(im_k, true);

                char keyCode = getKeyCode(im_k, isAltCode, isNumLock);
                if (keyCode != 0)
                    io.AddInputCharacter(keyCode);
            } else if (nya::hid::isKeyRelease(nx_k)) {
                io.AddKeyEvent(im_k, false);
            }
        }
    }

    void update_gamepad(ImGuiIO& io) {
        for (auto [im_k, nx_k] : npad_mapping) {
            if (nya::hid::isButtonPress((nn::hid::NpadButton)nx_k)) {
                io.AddKeyEvent((ImGuiKey)im_k, true);
            } else if (nya::hid::isButtonRelease((nn::hid::NpadButton)nx_k)) {
                io.AddKeyEvent((ImGuiKey)im_k, false);
            }
        }
    }

    void update_input() {
        ImGuiIO& io = ImGui::GetIO();
        io.MouseDrawCursor = nya::hid::isMouseConnected();
        update_keyboard(io);
        if (io.MouseDrawCursor) update_mouse(io);
        update_gamepad(io);
    }

    void draw(nvn::CommandBuffer* cmdbuf) {
        //make sure imgui is initialized before doing anything
        if (hk::gfx::ImGuiBackendNvn::instance()->isInitialized()) {
            nya::hid::updatePadState();
            update_input();

            ImGui::NewFrame();
            ImGui::ShowDemoWindow();
            ImGui::Render();

            hk::gfx::ImGuiBackendNvn::instance()->draw(ImGui::GetDrawData(), cmdbuf);
        }
    }

    void init() {
        //get backend instance
        auto* imgui = hk::gfx::ImGuiBackendNvn::instance();

        //set allocators
        imgui->setAllocator({ 
            [](size allocSize, size alignment) -> void* {
                return aligned_alloc(alignment, allocSize);
            },
            [](void* ptr) -> void {
                free(ptr);
            } 
        });

        //try to initialize the backend
        bool result = imgui->tryInitialize();

        nya::nya_log("[nya] imgui initialized %d\n", result);
    }

    HkTrampoline<nvn::CommandHandle, nvn::CommandBuffer*> draw_hook = hk::hook::trampoline([](nvn::CommandBuffer* param_1) {
        //get call location
        uint64_t called_by;
        asm volatile("mov %0, lr" : "=r"(called_by));

        //draw iif the function has been called from sead::GameFrameWorkNx::procDraw_
        if (called_by == hk::util::lookupSymbol<"procdraw_hook">()) {
            draw(param_1);
        }

        return draw_hook.orig(param_1);
    });

    void install_hook() {
        draw_hook.installAtOffset(hk::ro::getSdkModule(), 0x4B7E80);
    }}