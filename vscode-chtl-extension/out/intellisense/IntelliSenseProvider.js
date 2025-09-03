"use strict";
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    var desc = Object.getOwnPropertyDescriptor(m, k);
    if (!desc || ("get" in desc ? !m.__esModule : desc.writable || desc.configurable)) {
      desc = { enumerable: true, get: function() { return m[k]; } };
    }
    Object.defineProperty(o, k2, desc);
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || function (mod) {
    if (mod && mod.__esModule) return mod;
    var result = {};
    if (mod != null) for (var k in mod) if (k !== "default" && Object.prototype.hasOwnProperty.call(mod, k)) __createBinding(result, mod, k);
    __setModuleDefault(result, mod);
    return result;
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.CHTLIntelliSenseProvider = void 0;
const vscode = __importStar(require("vscode"));
const fs = __importStar(require("fs"));
const ModuleManager_1 = require("../modules/ModuleManager");
class CHTLIntelliSenseProvider {
    constructor(context) {
        // CHTL基础语法关键字
        this.chtlKeywords = [
            'text', 'style', 'script', 'html', 'head', 'body', 'div', 'span', 'p', 'a', 'img',
            'ul', 'ol', 'li', 'table', 'tr', 'td', 'th', 'form', 'input', 'button', 'textarea',
            'select', 'option', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6', 'section', 'article',
            'header', 'footer', 'nav', 'aside', 'main', 'figure', 'figcaption'
        ];
        // CHTL JS语法关键字
        this.chtljsKeywords = [
            'module', 'script', 'listen', 'delegate', 'animate', 'vir', 'function', 'var', 'let', 'const',
            'if', 'else', 'for', 'while', 'do', 'switch', 'case', 'default', 'break', 'continue',
            'return', 'try', 'catch', 'finally', 'throw', 'new', 'this', 'super', 'class', 'extends'
        ];
        // CHTL属性
        this.chtlAttributes = [
            'id', 'class', 'style', 'src', 'href', 'alt', 'title', 'width', 'height', 'type',
            'name', 'value', 'placeholder', 'disabled', 'readonly', 'required', 'checked',
            'selected', 'multiple', 'data-*', 'aria-*'
        ];
        // CSS属性
        this.cssProperties = [
            'color', 'background-color', 'background', 'font-size', 'font-family', 'font-weight',
            'margin', 'padding', 'border', 'width', 'height', 'display', 'position', 'top', 'left',
            'right', 'bottom', 'z-index', 'opacity', 'visibility', 'overflow', 'text-align',
            'line-height', 'letter-spacing', 'text-decoration', 'transform', 'transition', 'animation'
        ];
        this.context = context;
        this.moduleManager = new ModuleManager_1.CHTLModuleManager(context);
    }
    async provideCompletionItems(document, position, token, context) {
        const line = document.lineAt(position.line);
        const lineText = line.text;
        const beforeCursor = lineText.substring(0, position.character);
        const completions = [];
        // 根据上下文提供不同的补全
        if (this.isInStyleBlock(document, position)) {
            // 在样式块中提供CSS属性补全
            completions.push(...this.getCSSCompletions());
        }
        else if (this.isInScriptBlock(document, position)) {
            // 在脚本块中提供CHTL JS补全
            completions.push(...this.getCHTLJSCompletions());
        }
        else if (beforeCursor.includes('[Import]')) {
            // Import语句补全
            completions.push(...this.getImportCompletions());
        }
        else if (beforeCursor.includes('[')) {
            // 其他特殊语法补全
            completions.push(...this.getSpecialSyntaxCompletions());
        }
        else {
            // 基础CHTL语法补全
            completions.push(...this.getCHTLCompletions());
        }
        // 添加模块补全
        const moduleCompletions = await this.getModuleCompletions(document, position);
        completions.push(...moduleCompletions);
        return completions;
    }
    isInStyleBlock(document, position) {
        // 检查当前位置是否在style块内
        for (let i = position.line; i >= 0; i--) {
            const line = document.lineAt(i).text.trim();
            if (line.includes('style') && line.includes('{')) {
                return true;
            }
            if (line.includes('}') && i < position.line) {
                break;
            }
        }
        return false;
    }
    isInScriptBlock(document, position) {
        // 检查当前位置是否在script块内
        for (let i = position.line; i >= 0; i--) {
            const line = document.lineAt(i).text.trim();
            if (line.includes('script') && line.includes('{')) {
                return true;
            }
            if (line.includes('}') && i < position.line) {
                break;
            }
        }
        return false;
    }
    getCHTLCompletions() {
        const completions = [];
        // HTML元素补全
        this.chtlKeywords.forEach(keyword => {
            const item = new vscode.CompletionItem(keyword, vscode.CompletionItemKind.Keyword);
            item.detail = `CHTL ${keyword} 元素`;
            item.insertText = new vscode.SnippetString(`${keyword}\n{\n\t$0\n}`);
            completions.push(item);
        });
        // 属性补全
        this.chtlAttributes.forEach(attr => {
            const item = new vscode.CompletionItem(attr, vscode.CompletionItemKind.Property);
            item.detail = `CHTL ${attr} 属性`;
            item.insertText = new vscode.SnippetString(`${attr}: $0;`);
            completions.push(item);
        });
        return completions;
    }
    getCSSCompletions() {
        const completions = [];
        this.cssProperties.forEach(prop => {
            const item = new vscode.CompletionItem(prop, vscode.CompletionItemKind.Property);
            item.detail = `CSS ${prop} 属性`;
            item.insertText = new vscode.SnippetString(`${prop}: $0;`);
            completions.push(item);
        });
        // 选择器补全
        const selectors = ['.class', '#id', '&', '::before', '::after', ':hover', ':active', ':focus'];
        selectors.forEach(selector => {
            const item = new vscode.CompletionItem(selector, vscode.CompletionItemKind.Class);
            item.detail = `CSS ${selector} 选择器`;
            item.insertText = new vscode.SnippetString(`${selector}\n{\n\t$0\n}`);
            completions.push(item);
        });
        return completions;
    }
    getCHTLJSCompletions() {
        const completions = [];
        this.chtljsKeywords.forEach(keyword => {
            const item = new vscode.CompletionItem(keyword, vscode.CompletionItemKind.Keyword);
            item.detail = `CHTL JS ${keyword} 关键字`;
            completions.push(item);
        });
        // CHTL JS特殊语法
        const specialSyntax = [
            { name: '{{selector}}', detail: 'CSS选择器引用' },
            { name: '{{&}}', detail: '当前元素引用' },
            { name: '->', detail: '箭头操作符' },
            { name: '&->', detail: '事件绑定操作符' },
            { name: 'listen', detail: '增强监听器' },
            { name: 'delegate', detail: '事件委托' },
            { name: 'animate', detail: '动画API' },
            { name: 'vir', detail: '虚拟对象' }
        ];
        specialSyntax.forEach(syntax => {
            const item = new vscode.CompletionItem(syntax.name, vscode.CompletionItemKind.Function);
            item.detail = syntax.detail;
            completions.push(item);
        });
        return completions;
    }
    getImportCompletions() {
        const completions = [];
        const importTypes = [
            { name: '@Chtl', detail: 'CHTL模块导入' },
            { name: '@CJmod', detail: 'CJMOD模块导入' },
            { name: '@Html', detail: 'HTML文件导入' },
            { name: '@Style', detail: 'CSS文件导入' },
            { name: '@JavaScript', detail: 'JavaScript文件导入' },
            { name: '[Origin]', detail: '原始嵌入导入' }
        ];
        importTypes.forEach(type => {
            const item = new vscode.CompletionItem(type.name, vscode.CompletionItemKind.Module);
            item.detail = type.detail;
            item.insertText = new vscode.SnippetString(`${type.name} from "$1" as $2`);
            completions.push(item);
        });
        return completions;
    }
    getSpecialSyntaxCompletions() {
        const completions = [];
        const specialBlocks = [
            { name: '[Import]', detail: '导入语句块' },
            { name: '[Namespace]', detail: '命名空间块' },
            { name: '[Configuration]', detail: '配置块' },
            { name: '[Export]', detail: '导出表块' },
            { name: '[Template]', detail: '模板块' },
            { name: '[Custom]', detail: '自定义块' },
            { name: '[Origin]', detail: '原始嵌入块' }
        ];
        specialBlocks.forEach(block => {
            const item = new vscode.CompletionItem(block.name, vscode.CompletionItemKind.Struct);
            item.detail = block.detail;
            item.insertText = new vscode.SnippetString(`${block.name}\n{\n\t$0\n}`);
            completions.push(item);
        });
        return completions;
    }
    async getModuleCompletions(document, position) {
        const completions = [];
        try {
            // 获取可用的模块
            const modules = await this.moduleManager.getAvailableModules();
            modules.forEach(module => {
                const item = new vscode.CompletionItem(module.name, vscode.CompletionItemKind.Module);
                item.detail = `${module.type} 模块: ${module.description || ''}`;
                item.documentation = new vscode.MarkdownString(module.documentation || '');
                // 根据模块类型设置不同的插入文本
                if (module.type === 'CMOD') {
                    item.insertText = new vscode.SnippetString(`[Import] @Chtl from "${module.name}" as $1`);
                }
                else if (module.type === 'CJMOD') {
                    item.insertText = new vscode.SnippetString(`[Import] @CJmod from "${module.name}" as $1`);
                }
                completions.push(item);
            });
        }
        catch (error) {
            console.error('获取模块补全失败:', error);
        }
        return completions;
    }
    async provideHover(document, position, token) {
        const range = document.getWordRangeAtPosition(position);
        if (!range) {
            return null;
        }
        const word = document.getText(range);
        // 检查是否是CHTL关键字
        if (this.chtlKeywords.includes(word)) {
            return new vscode.Hover(new vscode.MarkdownString(`**CHTL元素**: \`${word}\`\n\n${this.getElementDescription(word)}`), range);
        }
        // 检查是否是CSS属性
        if (this.cssProperties.includes(word)) {
            return new vscode.Hover(new vscode.MarkdownString(`**CSS属性**: \`${word}\`\n\n${this.getCSSPropertyDescription(word)}`), range);
        }
        // 检查是否是模块相关
        try {
            const moduleInfo = await this.moduleManager.getModuleInfo(word);
            if (moduleInfo) {
                return new vscode.Hover(new vscode.MarkdownString(`**${moduleInfo.type}模块**: \`${word}\`\n\n${moduleInfo.description || ''}`), range);
            }
        }
        catch (error) {
            // 忽略模块查询错误
        }
        return null;
    }
    async provideDefinition(document, position, token) {
        const range = document.getWordRangeAtPosition(position);
        if (!range) {
            return null;
        }
        const word = document.getText(range);
        // 查找模块定义
        try {
            const modulePath = await this.moduleManager.findModuleDefinition(word);
            if (modulePath && fs.existsSync(modulePath)) {
                return new vscode.Location(vscode.Uri.file(modulePath), new vscode.Position(0, 0));
            }
        }
        catch (error) {
            console.error('查找模块定义失败:', error);
        }
        return null;
    }
    getElementDescription(element) {
        const descriptions = {
            'text': '文本节点，用于显示文本内容',
            'style': '样式块，用于定义CSS样式',
            'script': '脚本块，用于编写JavaScript代码',
            'div': 'HTML div元素，用于布局和分组',
            'span': 'HTML span元素，用于内联文本',
            'p': 'HTML段落元素',
            'a': 'HTML链接元素',
            'img': 'HTML图像元素'
        };
        return descriptions[element] || `HTML ${element} 元素`;
    }
    getCSSPropertyDescription(property) {
        const descriptions = {
            'color': '设置文本颜色',
            'background-color': '设置背景颜色',
            'font-size': '设置字体大小',
            'margin': '设置外边距',
            'padding': '设置内边距',
            'border': '设置边框',
            'width': '设置宽度',
            'height': '设置高度'
        };
        return descriptions[property] || `CSS ${property} 属性`;
    }
}
exports.CHTLIntelliSenseProvider = CHTLIntelliSenseProvider;
//# sourceMappingURL=IntelliSenseProvider.js.map