#pragma once

#include <string>
#include <vector>

namespace CJMOD {
namespace Chtholly {

/**
 * util...then功能头文件
 * 异步链式操作功能
 */

struct ChainOperation {
    std::string operationType;
    std::string target;
    std::unordered_map<std::string, std::string> parameters;
};

class UtilThenProcessor {
public:
    static std::string ProcessUtilThen(const std::string& chainCode);
    static std::string GenerateAsyncChain(const std::vector<ChainOperation>& operations);
    static std::vector<ChainOperation> ParseChainOperations(const std::string& code);
    static std::string CreateUtilObject();
};

} // namespace Chtholly
} // namespace CJMOD