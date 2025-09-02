#ifndef PRINTMYLOVE_H
#define PRINTMYLOVE_H

#include <string>
#include <vector>
#include <functional>
#include "CHTLJS/CJMOD/CJMODAPI.h"

namespace CHTL {
namespace CJMOD {

class PrintMyLove {
public:
    PrintMyLove();
    ~PrintMyLove();

    // 注册printMylove函数到CHTL JS
    static void Register();

private:
    // 转换图片为字符艺术
    static std::string ConvertToArt(const std::string& url, 
                                   const std::string& mode,
                                   const std::string& width,
                                   const std::string& height,
                                   double scale);

    // ASCII字符集
    static const char* ASCII_CHARS;
    
    // 像素块字符
    static const char* PIXEL_CHARS;
};

} // namespace CJMOD
} // namespace CHTL

#endif // PRINTMYLOVE_H