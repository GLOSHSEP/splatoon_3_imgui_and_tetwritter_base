#include "utility/nya_log.hpp"
#include "utility/fs_mgr.hpp"
#include "nn/fs.h"

namespace nya {
    void nya_log(const char* message, ...) {
        va_list args;
        va_start(args, message);
        nn::fs::FileHandle handle;
        static long position = 0;
        long add_size;
        static bool initialized = false;
        char buffer[128];

        if (!initialized) {
            //TODO
            // if (!nya_sd_avalible)
            //     return;
            if (nya::fs::isFileExist("sd:/nyalog.txt"))
                nn::fs::DeleteFile("sd:/nyalog.txt");
            initialized = true;
        }

        if (!nya::fs::isFileExist("sd:/nyalog.txt")) {
            nn::fs::CreateFile("sd:/nyalog.txt", 0);
            position = 0;
        }

        int writen = nn::util::VSNPrintf(buffer, 128, message, args);
        va_end(args);

        nn::fs::OpenFile(&handle, "sd:/nyalog.txt", nn::fs::OpenMode_Write);
        nn::fs::GetFileSize(&add_size, handle);
        nn::fs::SetFileSize(handle, add_size + writen);
        nn::fs::WriteFile(handle, position, buffer, writen, nn::fs::WriteOption::CreateOption(nn::fs::WriteOptionFlag_Flush));
        nn::fs::CloseFile(handle);
        position += long(writen);
    }
}