#include <string>
#include <algorithm>
#include <cctype>

namespace CHTL {
namespace Utils {

std::string Trim(const std::string& str) {
    auto start = str.find_first_not_of(" \t\n\r");
    auto end = str.find_last_not_of(" \t\n\r");
    
    if (start == std::string::npos) {
        return "";
    }
    
    return str.substr(start, end - start + 1);
}

} // namespace Utils
} // namespace CHTL