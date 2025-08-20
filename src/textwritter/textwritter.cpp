#include "textwritter/textwritter.hpp"
#include "hk/hook/Trampoline.h"
#include "nvn/nvn_Cpp.h"
#include "utility/input_mgr.hpp"
#include "hk/gfx/DebugRenderer.h"

namespace textwritter {

    void draw(nvn::CommandBuffer* cmdbuf) {
        nya::hid::updatePadState();

        auto* renderer = hk::gfx::DebugRenderer::instance();

        renderer->clear();
        renderer->begin(cmdbuf);

        renderer->setGlyphSize(0.45);

        renderer->drawQuad(
            { { 30, 30 }, { 0, 0 }, 0xef000000 },
            { { 300, 30 }, { 1.0, 0 }, 0xef000000 },
            { { 300, 100 }, { 1.0, 1.0 }, 0xef000000 },
            { { 30, 100 }, { 0, 1.0 }, 0xef000000 });

        renderer->setCursor({ 50, 50 });

        renderer->printf("Hewwo wowld :3\n");
    }

    HkTrampoline<nvn::CommandHandle, nvn::CommandBuffer*> draw_hook = hk::hook::trampoline([](nvn::CommandBuffer* param_1) {
        //get call location
        uint64_t called_by;
        asm volatile("mov %0, lr" : "=r"(called_by));

        //draw if the function has been called from sead::GameFrameWorkNx::procDraw_
        if (called_by == hk::util::lookupSymbol<"procdraw_hook">()) {
            draw(param_1);
        }

        return draw_hook.orig(param_1);
    });

    void install_hook() {
        draw_hook.installAtSym<"nvnCommandBufferEndRecording">();
    }}