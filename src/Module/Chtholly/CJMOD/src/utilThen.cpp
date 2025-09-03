#include "utilThen.h"
#include <sstream>
#include <regex>

namespace CJMOD {
namespace Chtholly {

std::string UtilThenProcessor::ProcessUtilThen(const std::string& chainCode) {
    auto operations = ParseChainOperations(chainCode);
    return GenerateAsyncChain(operations);
}

std::string UtilThenProcessor::GenerateAsyncChain(const std::vector<ChainOperation>& operations) {
    std::ostringstream chain;
    
    chain << "// 珂朵莉util...then异步链式操作\n";
    chain << "const util = {\n";
    chain << "    fadeIn: function(element) {\n";
    chain << "        return new Promise(resolve => {\n";
    chain << "            if (element) {\n";
    chain << "                element.style.transition = 'opacity 0.5s ease';\n";
    chain << "                element.style.opacity = '1';\n";
    chain << "                setTimeout(() => resolve(element), 500);\n";
    chain << "            } else {\n";
    chain << "                resolve(null);\n";
    chain << "            }\n";
    chain << "        });\n";
    chain << "    },\n";
    chain << "    \n";
    chain << "    slideDown: function() {\n";
    chain << "        return function(element) {\n";
    chain << "            return new Promise(resolve => {\n";
    chain << "                if (element) {\n";
    chain << "                    element.style.transform = 'translateY(0)';\n";
    chain << "                    setTimeout(() => resolve(element), 300);\n";
    chain << "                } else {\n";
    chain << "                    resolve(null);\n";
    chain << "                }\n";
    chain << "            });\n";
    chain << "        };\n";
    chain << "    },\n";
    chain << "    \n";
    chain << "    addSparkles: function() {\n";
    chain << "        return function(element) {\n";
    chain << "            return new Promise(resolve => {\n";
    chain << "                if (element) {\n";
    chain << "                    element.style.boxShadow = '0 0 20px rgba(255, 107, 107, 0.6)';\n";
    chain << "                    setTimeout(() => resolve(element), 200);\n";
    chain << "                } else {\n";
    chain << "                    resolve(null);\n";
    chain << "                }\n";
    chain << "            });\n";
    chain << "        };\n";
    chain << "    },\n";
    chain << "    \n";
    chain << "    then: function(nextOperation) {\n";
    chain << "        return function(element) {\n";
    chain << "            if (typeof nextOperation === 'function') {\n";
    chain << "                return nextOperation(element);\n";
    chain << "            }\n";
    chain << "            return Promise.resolve(element);\n";
    chain << "        };\n";
    chain << "    }\n";
    chain << "};\n";
    
    return chain.str();
}

std::vector<ChainOperation> UtilThenProcessor::ParseChainOperations(const std::string& code) {
    std::vector<ChainOperation> operations;
    
    // 解析util链式操作
    std::regex operationRegex(R"(util\.(\w+)\s*\([^)]*\))");
    std::sregex_iterator iter(code.begin(), code.end(), operationRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        ChainOperation op;
        op.operationType = (*iter)[1].str();
        operations.push_back(op);
    }
    
    return operations;
}

std::string UtilThenProcessor::CreateUtilObject() {
    return GenerateAsyncChain({});
}

} // namespace Chtholly
} // namespace CJMOD