#include "hk/hook/Trampoline.h"
#include "hk/gfx/DebugRenderer.h"
#include "hk/mem/BssHeap.h"
#include "nn/fs/fs_mount.h"
#include "textwritter/textwritter.hpp"
#include "utility/nya_log.hpp"
#include "utility/input_mgr.hpp"


HkTrampoline<void> init_hook = hk::hook::trampoline([]() {
    //initialize SD card
    nn::fs::MountSdCardForDebug("sd");
    nya::nya_log("[nya] hewwo wowld :3\n");

    nya::nya_log("[nya] total heap size %x\n", hk::mem::sMainHeap.getTotalSize());
    nya::nya_log("[nya] free memory %x\n", hk::mem::sMainHeap.getFreeSize());

    nya::hid::initKBM();
    nya::hid::setPort(0);
    nya::nya_log("[nya] input initialized\n");
});

extern "C" void hkMain() {
    //install hooks
    init_hook.installAtSym<"_ZN2nn7account12PopOpenUsersEPiPNS0_10UserHandleEi">();
    nya::hid::install_hooks();
    textwritter::install_hook();
    hk::gfx::DebugRenderer::instance()->installHooks();
}