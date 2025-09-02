#ifndef CHTL_CHTLJS_AST_NODES_H
#define CHTL_CHTLJS_AST_NODES_H

#include <memory>

namespace CHTL {
namespace JS {

class ModuleNode {
public:
    ModuleNode() {}
    ~ModuleNode() {}
};

class VirtualObjectNode {
public:
    VirtualObjectNode() {}
    ~VirtualObjectNode() {}
};

class AnimationNode {
public:
    AnimationNode() {}
    ~AnimationNode() {}
};

class ListenerNode {
public:
    ListenerNode() {}
    ~ListenerNode() {}
};

class DelegateNode {
public:
    DelegateNode() {}
    ~DelegateNode() {}
};

class SelectorCache {
public:
    SelectorCache() {}
    ~SelectorCache() {}
};

} // namespace JS
} // namespace CHTL

#endif // CHTL_CHTLJS_AST_NODES_H