#include "tcp_common.h"

bool is_directory_valid(const char *dir_path)
{
    // 检查目录是否存在并且是否可读
    if (access(dir_path, F_OK | R_OK) == 0)
    {
        // 如果目录存在并且可读，返回true表示有效
        return true;
    }
    else
    {
        // 如果目录不存在或者不可读，返回false表示无效
        return false;
    }
}