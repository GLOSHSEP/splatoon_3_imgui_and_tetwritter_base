#include "utility/fs_mgr.hpp"
#include "nn/fs.h"
#include <cstdlib>

namespace nya { 
    namespace fs { 
        void writeFileToPath(void *buf, size_t size, const char *path) {
            nn::fs::FileHandle handle;

            if (isFileExist(path)) {
                nn::fs::DeleteFile(path);
            }

            if (nn::fs::CreateFile(path, size).IsFailure()) {
                return;
            }

            if (nn::fs::OpenFile(&handle, path, nn::fs::OpenMode_Write).IsFailure()) {
                return;
            }

            if (nn::fs::WriteFile(handle, 0, buf, size, nn::fs::WriteOption::CreateOption(nn::fs::WriteOptionFlag_Flush)).IsFailure()) {
                return;
            } 

            nn::fs::CloseFile(handle);
        }

        // make sure to free buffer after usage is done
        void loadFileFromPath(LoadData &loadData, size_t alignment) {
            nn::fs::FileHandle handle;

            nn::fs::OpenFile(&handle, loadData.path, nn::fs::OpenMode_Read); 

            long size = 0;
            nn::fs::GetFileSize(&size, handle);

            loadData.buffer = aligned_alloc(alignment, size); 
            loadData.bufSize = size;

            nn::fs::ReadFile(handle, 0, loadData.buffer, size);

            nn::fs::CloseFile(handle);
        }


        long getFileSize(const char *path) {
            nn::fs::FileHandle handle;
            long result = -1;

            nn::Result openResult = nn::fs::OpenFile(&handle, path, nn::fs::OpenMode::OpenMode_Read);

            if (openResult.IsSuccess()) {
                nn::fs::GetFileSize(&result, handle);
                nn::fs::CloseFile(handle);
            }

            return result;
        }

        bool isFileExist(const char *path) {
            nn::fs::DirectoryEntryType type;
            nn::fs::GetEntryType(&type, path); 
            return type == nn::fs::DirectoryEntryType_File;
        } 
    }
}