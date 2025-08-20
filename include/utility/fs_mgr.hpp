#pragma once 

#include <cstddef>

namespace nya { 
    namespace fs { 

        struct LoadData {
            const char* path;
            void* buffer;
            long bufSize;
        };

        void writeFileToPath(void* buf, size_t size, const char* path);

        void loadFileFromPath(LoadData& loadData, size_t alignment);

        long getFileSize(const char* path);

        bool isFileExist(const char* path); 
    } 
}
