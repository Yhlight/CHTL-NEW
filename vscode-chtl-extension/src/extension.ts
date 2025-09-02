import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';
import { exec, spawn } from 'child_process';
import { CHTLCompilerManager } from './compiler/CompilerManager';
import { CHTLPreviewProvider } from './preview/PreviewProvider';
import { CHTLIntelliSenseProvider } from './intellisense/IntelliSenseProvider';
import { CHTLFormatterProvider } from './formatter/FormatterProvider';
import { CHTLModuleManager } from './modules/ModuleManager';

export function activate(context: vscode.ExtensionContext) {
    console.log('🚀 CHTL Language Support 插件已激活');

    // 初始化组件
    const compilerManager = new CHTLCompilerManager(context);
    const previewProvider = new CHTLPreviewProvider(context);
    const intelliSenseProvider = new CHTLIntelliSenseProvider(context);
    const formatterProvider = new CHTLFormatterProvider(context);
    const moduleManager = new CHTLModuleManager(context);

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

function registerCommands(
    context: vscode.ExtensionContext,
    compilerManager: CHTLCompilerManager,
    previewProvider: CHTLPreviewProvider,
    formatterProvider: CHTLFormatterProvider
) {
    // 编译命令
    const compileCommand = vscode.commands.registerCommand('chtl.compile', async (uri?: vscode.Uri) => {
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
            } else {
                vscode.window.showErrorMessage(`❌ 编译失败: ${result.error}`);
            }
        } catch (error) {
            vscode.window.showErrorMessage(`❌ 编译错误: ${error}`);
        }
    });

    // 浏览器打开命令
    const openInBrowserCommand = vscode.commands.registerCommand('chtl.openInBrowser', async (uri?: vscode.Uri) => {
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
                exec(`${command} "${htmlPath}"`);
                vscode.window.showInformationMessage('🌐 已在浏览器中打开');
            } else {
                vscode.window.showErrorMessage('❌ 找不到生成的HTML文件');
            }
        } catch (error) {
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
        } catch (error) {
            vscode.window.showErrorMessage(`❌ 格式化失败: ${error}`);
        }
    });

    // 导出HTML命令
    const exportHTMLCommand = vscode.commands.registerCommand('chtl.exportHTML', async (uri?: vscode.Uri) => {
        const targetUri = uri || vscode.window.activeTextEditor?.document.uri;
        if (!targetUri) {
            vscode.window.showErrorMessage('❌ 没有选择要导出的文件');
            return;
        }

        try {
            const result = await compilerManager.exportHTML(targetUri);
            if (result.success) {
                vscode.window.showInformationMessage(`✅ HTML导出成功: ${result.outputPath}`);
            } else {
                vscode.window.showErrorMessage(`❌ HTML导出失败: ${result.error}`);
            }
        } catch (error) {
            vscode.window.showErrorMessage(`❌ 导出错误: ${error}`);
        }
    });

    // 导出CSS命令
    const exportCSSCommand = vscode.commands.registerCommand('chtl.exportCSS', async (uri?: vscode.Uri) => {
        const targetUri = uri || vscode.window.activeTextEditor?.document.uri;
        if (!targetUri) {
            vscode.window.showErrorMessage('❌ 没有选择要导出的文件');
            return;
        }

        try {
            const result = await compilerManager.exportCSS(targetUri);
            if (result.success) {
                vscode.window.showInformationMessage(`✅ CSS导出成功: ${result.outputPath}`);
            } else {
                vscode.window.showErrorMessage(`❌ CSS导出失败: ${result.error}`);
            }
        } catch (error) {
            vscode.window.showErrorMessage(`❌ 导出错误: ${error}`);
        }
    });

    // 导出JS命令
    const exportJSCommand = vscode.commands.registerCommand('chtl.exportJS', async (uri?: vscode.Uri) => {
        const targetUri = uri || vscode.window.activeTextEditor?.document.uri;
        if (!targetUri) {
            vscode.window.showErrorMessage('❌ 没有选择要导出的文件');
            return;
        }

        try {
            const result = await compilerManager.exportJS(targetUri);
            if (result.success) {
                vscode.window.showInformationMessage(`✅ JavaScript导出成功: ${result.outputPath}`);
            } else {
                vscode.window.showErrorMessage(`❌ JavaScript导出失败: ${result.error}`);
            }
        } catch (error) {
            vscode.window.showErrorMessage(`❌ 导出错误: ${error}`);
        }
    });

    // 打开文档命令
    const openDocumentationCommand = vscode.commands.registerCommand('chtl.openDocumentation', () => {
        // 在新标签页中打开CHTL语法文档
        const panel = vscode.window.createWebviewPanel(
            'chtlDocumentation',
            'CHTL语法文档',
            vscode.ViewColumn.Beside,
            {
                enableScripts: true,
                localResourceRoots: [context.extensionUri]
            }
        );

        // 加载文档内容
        const documentationPath = path.join(context.extensionPath, 'resources', 'documentation.html');
        if (fs.existsSync(documentationPath)) {
            panel.webview.html = fs.readFileSync(documentationPath, 'utf8');
        } else {
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
    context.subscriptions.push(
        compileCommand,
        openInBrowserCommand,
        formatCommand,
        exportHTMLCommand,
        exportCSSCommand,
        exportJSCommand,
        openDocumentationCommand,
        previewToggleCommand
    );
}

function registerLanguageFeatures(
    context: vscode.ExtensionContext,
    intelliSenseProvider: CHTLIntelliSenseProvider,
    formatterProvider: CHTLFormatterProvider
) {
    // 注册自动完成提供程序
    const chtlCompletionProvider = vscode.languages.registerCompletionItemProvider(
        'chtl',
        intelliSenseProvider,
        '{', '.', '#', '@', '[', ':'
    );

    const chtlJSCompletionProvider = vscode.languages.registerCompletionItemProvider(
        'chtl-js',
        intelliSenseProvider,
        '{', '.', '#', '@', '[', ':', '-', '>'
    );

    // 注册格式化提供程序
    const chtlFormattingProvider = vscode.languages.registerDocumentFormattingEditProvider(
        'chtl',
        formatterProvider
    );

    const chtlJSFormattingProvider = vscode.languages.registerDocumentFormattingEditProvider(
        'chtl-js',
        formatterProvider
    );

    // 注册悬停提供程序
    const chtlHoverProvider = vscode.languages.registerHoverProvider(
        ['chtl', 'chtl-js'],
        intelliSenseProvider
    );

    // 注册定义提供程序
    const chtlDefinitionProvider = vscode.languages.registerDefinitionProvider(
        ['chtl', 'chtl-js'],
        intelliSenseProvider
    );

    context.subscriptions.push(
        chtlCompletionProvider,
        chtlJSCompletionProvider,
        chtlFormattingProvider,
        chtlJSFormattingProvider,
        chtlHoverProvider,
        chtlDefinitionProvider
    );
}

function registerPreviewFeatures(
    context: vscode.ExtensionContext,
    previewProvider: CHTLPreviewProvider
) {
    // 注册预览提供程序
    const previewProvider_registration = vscode.window.registerWebviewPanelSerializer(
        'chtlPreview',
        previewProvider
    );

    context.subscriptions.push(previewProvider_registration);
}

export function deactivate() {
    console.log('👋 CHTL Language Support 插件已停用');
}