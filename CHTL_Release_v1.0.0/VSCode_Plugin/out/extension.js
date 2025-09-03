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
exports.deactivate = exports.activate = void 0;
const vscode = __importStar(require("vscode"));
const path = __importStar(require("path"));
const fs = __importStar(require("fs"));
const child_process_1 = require("child_process");
const CompilerManager_1 = require("./compiler/CompilerManager");
const PreviewProvider_1 = require("./preview/PreviewProvider");
const IntelliSenseProvider_1 = require("./intellisense/IntelliSenseProvider");
const FormatterProvider_1 = require("./formatter/FormatterProvider");
const ModuleManager_1 = require("./modules/ModuleManager");
function activate(context) {
    console.log('🚀 CHTL Language Support 插件已激活');
    // 初始化组件
    const compilerManager = new CompilerManager_1.CHTLCompilerManager(context);
    const previewProvider = new PreviewProvider_1.CHTLPreviewProvider(context);
    const intelliSenseProvider = new IntelliSenseProvider_1.CHTLIntelliSenseProvider(context);
    const formatterProvider = new FormatterProvider_1.CHTLFormatterProvider(context);
    const moduleManager = new ModuleManager_1.CHTLModuleManager(context);
    // 注册命令
    registerCommands(context, compilerManager, previewProvider, formatterProvider);
    // 注册语言功能
    registerLanguageFeatures(context, intelliSenseProvider, formatterProvider);
    // 注册预览功能
    registerPreviewFeatures(context, previewProvider);
    // 初始化模块管理器
    moduleManager.initialize();
    // 监听配置变化
    vscode.workspace.onDidChangeConfiguration(event => {
        if (event.affectsConfiguration('chtl')) {
            console.log('📝 CHTL配置已更新');
            // 重新加载相关组件
            moduleManager.reload();
        }
    });
    // 监听文件保存
    vscode.workspace.onDidSaveTextDocument(document => {
        if (document.languageId === 'chtl' || document.languageId === 'chtl-js') {
            // 自动编译和预览更新
            const config = vscode.workspace.getConfiguration('chtl');
            if (config.get('enableRealTimePreview', true)) {
                previewProvider.updatePreview(document);
            }
        }
    });
    console.log('✅ CHTL Language Support 插件初始化完成');
}
exports.activate = activate;
function registerCommands(context, compilerManager, previewProvider, formatterProvider) {
    // 编译命令
    const compileCommand = vscode.commands.registerCommand('chtl.compile', async (uri) => {
        const targetUri = uri || vscode.window.activeTextEditor?.document.uri;
        if (!targetUri) {
            vscode.window.showErrorMessage('❌ 没有选择要编译的文件');
            return;
        }
        try {
            vscode.window.showInformationMessage('🔄 正在编译CHTL文件...');
            const result = await compilerManager.compile(targetUri);
            if (result.success) {
                vscode.window.showInformationMessage('✅ 编译成功');
            }
            else {
                vscode.window.showErrorMessage(`❌ 编译失败: ${result.error}`);
            }
        }
        catch (error) {
            vscode.window.showErrorMessage(`❌ 编译错误: ${error}`);
        }
    });
    // 浏览器打开命令
    const openInBrowserCommand = vscode.commands.registerCommand('chtl.openInBrowser', async (uri) => {
        const targetUri = uri || vscode.window.activeTextEditor?.document.uri;
        if (!targetUri) {
            vscode.window.showErrorMessage('❌ 没有选择要打开的文件');
            return;
        }
        try {
            // 先编译文件
            const result = await compilerManager.compile(targetUri);
            if (!result.success) {
                vscode.window.showErrorMessage(`❌ 编译失败，无法在浏览器中打开: ${result.error}`);
                return;
            }
            // 在浏览器中打开生成的HTML文件
            const htmlPath = result.outputPath;
            if (htmlPath && fs.existsSync(htmlPath)) {
                const command = process.platform === 'win32' ? 'start' :
                    process.platform === 'darwin' ? 'open' : 'xdg-open';
                (0, child_process_1.exec)(`${command} "${htmlPath}"`);
                vscode.window.showInformationMessage('🌐 已在浏览器中打开');
            }
            else {
                vscode.window.showErrorMessage('❌ 找不到生成的HTML文件');
            }
        }
        catch (error) {
            vscode.window.showErrorMessage(`❌ 打开失败: ${error}`);
        }
    });
    // 格式化命令
    const formatCommand = vscode.commands.registerCommand('chtl.format', async () => {
        const editor = vscode.window.activeTextEditor;
        if (!editor) {
            vscode.window.showErrorMessage('❌ 没有活动的编辑器');
            return;
        }
        try {
            await formatterProvider.formatDocument(editor.document, editor);
            vscode.window.showInformationMessage('✅ 代码格式化完成');
        }
        catch (error) {
            vscode.window.showErrorMessage(`❌ 格式化失败: ${error}`);
        }
    });
    // 导出HTML命令
    const exportHTMLCommand = vscode.commands.registerCommand('chtl.exportHTML', async (uri) => {
        const targetUri = uri || vscode.window.activeTextEditor?.document.uri;
        if (!targetUri) {
            vscode.window.showErrorMessage('❌ 没有选择要导出的文件');
            return;
        }
        try {
            const result = await compilerManager.exportHTML(targetUri);
            if (result.success) {
                vscode.window.showInformationMessage(`✅ HTML导出成功: ${result.outputPath}`);
            }
            else {
                vscode.window.showErrorMessage(`❌ HTML导出失败: ${result.error}`);
            }
        }
        catch (error) {
            vscode.window.showErrorMessage(`❌ 导出错误: ${error}`);
        }
    });
    // 导出CSS命令
    const exportCSSCommand = vscode.commands.registerCommand('chtl.exportCSS', async (uri) => {
        const targetUri = uri || vscode.window.activeTextEditor?.document.uri;
        if (!targetUri) {
            vscode.window.showErrorMessage('❌ 没有选择要导出的文件');
            return;
        }
        try {
            const result = await compilerManager.exportCSS(targetUri);
            if (result.success) {
                vscode.window.showInformationMessage(`✅ CSS导出成功: ${result.outputPath}`);
            }
            else {
                vscode.window.showErrorMessage(`❌ CSS导出失败: ${result.error}`);
            }
        }
        catch (error) {
            vscode.window.showErrorMessage(`❌ 导出错误: ${error}`);
        }
    });
    // 导出JS命令
    const exportJSCommand = vscode.commands.registerCommand('chtl.exportJS', async (uri) => {
        const targetUri = uri || vscode.window.activeTextEditor?.document.uri;
        if (!targetUri) {
            vscode.window.showErrorMessage('❌ 没有选择要导出的文件');
            return;
        }
        try {
            const result = await compilerManager.exportJS(targetUri);
            if (result.success) {
                vscode.window.showInformationMessage(`✅ JavaScript导出成功: ${result.outputPath}`);
            }
            else {
                vscode.window.showErrorMessage(`❌ JavaScript导出失败: ${result.error}`);
            }
        }
        catch (error) {
            vscode.window.showErrorMessage(`❌ 导出错误: ${error}`);
        }
    });
    // 打开文档命令
    const openDocumentationCommand = vscode.commands.registerCommand('chtl.openDocumentation', () => {
        // 在新标签页中打开CHTL语法文档
        const panel = vscode.window.createWebviewPanel('chtlDocumentation', 'CHTL语法文档', vscode.ViewColumn.Beside, {
            enableScripts: true,
            localResourceRoots: [context.extensionUri]
        });
        // 加载文档内容
        const documentationPath = path.join(context.extensionPath, 'resources', 'documentation.html');
        if (fs.existsSync(documentationPath)) {
            panel.webview.html = fs.readFileSync(documentationPath, 'utf8');
        }
        else {
            panel.webview.html = `
                <html>
                <body>
                    <h1>CHTL语法文档</h1>
                    <p>正在加载文档内容...</p>
                    <p>如需完整文档，请访问官方网站。</p>
                </body>
                </html>
            `;
        }
    });
    // 切换预览命令
    const previewToggleCommand = vscode.commands.registerCommand('chtl.previewToggle', () => {
        previewProvider.togglePreview();
    });
    // 注册所有命令
    context.subscriptions.push(compileCommand, openInBrowserCommand, formatCommand, exportHTMLCommand, exportCSSCommand, exportJSCommand, openDocumentationCommand, previewToggleCommand);
}
function registerLanguageFeatures(context, intelliSenseProvider, formatterProvider) {
    // 注册自动完成提供程序
    const chtlCompletionProvider = vscode.languages.registerCompletionItemProvider('chtl', intelliSenseProvider, '{', '.', '#', '@', '[', ':');
    const chtlJSCompletionProvider = vscode.languages.registerCompletionItemProvider('chtl-js', intelliSenseProvider, '{', '.', '#', '@', '[', ':', '-', '>');
    // 注册格式化提供程序
    const chtlFormattingProvider = vscode.languages.registerDocumentFormattingEditProvider('chtl', formatterProvider);
    const chtlJSFormattingProvider = vscode.languages.registerDocumentFormattingEditProvider('chtl-js', formatterProvider);
    // 注册悬停提供程序
    const chtlHoverProvider = vscode.languages.registerHoverProvider(['chtl', 'chtl-js'], intelliSenseProvider);
    // 注册定义提供程序
    const chtlDefinitionProvider = vscode.languages.registerDefinitionProvider(['chtl', 'chtl-js'], intelliSenseProvider);
    context.subscriptions.push(chtlCompletionProvider, chtlJSCompletionProvider, chtlFormattingProvider, chtlJSFormattingProvider, chtlHoverProvider, chtlDefinitionProvider);
}
function registerPreviewFeatures(context, previewProvider) {
    // 注册预览提供程序
    const previewProvider_registration = vscode.window.registerWebviewPanelSerializer('chtlPreview', previewProvider);
    context.subscriptions.push(previewProvider_registration);
}
function deactivate() {
    console.log('👋 CHTL Language Support 插件已停用');
}
exports.deactivate = deactivate;
//# sourceMappingURL=extension.js.map