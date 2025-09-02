#include <fstream>
#include <sstream>

namespace CHTL {
namespace Utils {

std::string ReadFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

} // namespace Utils
} // namespace CHTL