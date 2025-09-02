import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';
import * as http from 'http';
import * as WebSocket from 'ws';
import * as chokidar from 'chokidar';
import { CHTLCompilerManager } from '../compiler/CompilerManager';

export class CHTLPreviewProvider implements vscode.WebviewPanelSerializer {
    private context: vscode.ExtensionContext;
    private compilerManager: CHTLCompilerManager;
    private previewPanel: vscode.WebviewPanel | null = null;
    private server: http.Server | null = null;
    private wss: WebSocket.Server | null = null;
    private fileWatcher: chokidar.FSWatcher | null = null;
    private currentDocument: vscode.TextDocument | null = null;

    constructor(context: vscode.ExtensionContext) {
        this.context = context;
        this.compilerManager = new CHTLCompilerManager(context);
        this.initializePreviewServer();
    }

    private initializePreviewServer() {
        const config = vscode.workspace.getConfiguration('chtl');
        const port = config.get('previewPort', 3000);

        // 创建HTTP服务器
        this.server = http.createServer((req, res) => {
            res.setHeader('Access-Control-Allow-Origin', '*');
            res.setHeader('Access-Control-Allow-Methods', 'GET, POST, PUT, DELETE');
            res.setHeader('Access-Control-Allow-Headers', 'Content-Type');

            if (req.url === '/') {
                this.servePreviewHTML(res);
            } else if (req.url?.startsWith('/assets/')) {
                this.serveAsset(req.url, res);
            } else {
                res.writeHead(404);
                res.end('Not Found');
            }
        });

        // 创建WebSocket服务器用于实时更新
        this.wss = new WebSocket.Server({ server: this.server });
        this.wss.on('connection', (ws) => {
            console.log('🔌 预览客户端已连接');
            ws.on('close', () => {
                console.log('🔌 预览客户端已断开');
            });
        });

        this.server.listen(port, () => {
            console.log(`🌐 CHTL预览服务器启动在端口 ${port}`);
        });
    }

    private servePreviewHTML(res: http.ServerResponse) {
        const html = `
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CHTL实时预览</title>
    <style>
        body {
            margin: 0;
            padding: 20px;
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background-color: #f5f5f5;
        }
        .preview-container {
            background: white;
            border-radius: 8px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
            overflow: hidden;
        }
        .preview-header {
            background: #007acc;
            color: white;
            padding: 10px 20px;
            font-weight: bold;
        }
        .preview-content {
            padding: 20px;
            min-height: 400px;
        }
        .loading {
            text-align: center;
            color: #666;
            font-style: italic;
        }
        .error {
            color: #d73a49;
            background: #ffeef0;
            padding: 10px;
            border-radius: 4px;
            border-left: 4px solid #d73a49;
        }
    </style>
</head>
<body>
    <div class="preview-container">
        <div class="preview-header">
            📺 CHTL实时预览
        </div>
        <div class="preview-content" id="content">
            <div class="loading">等待CHTL文件内容...</div>
        </div>
    </div>

    <script>
        const ws = new WebSocket('ws://localhost:${this.getPreviewPort()}');
        const contentEl = document.getElementById('content');

        ws.onopen = function() {
            console.log('🔌 已连接到预览服务器');
        };

        ws.onmessage = function(event) {
            const data = JSON.parse(event.data);
            if (data.type === 'update') {
                if (data.success) {
                    contentEl.innerHTML = data.content;
                } else {
                    contentEl.innerHTML = \`<div class="error">❌ 编译错误: \${data.error}</div>\`;
                }
            }
        };

        ws.onclose = function() {
            console.log('🔌 预览服务器连接已断开');
            contentEl.innerHTML = '<div class="error">❌ 预览服务器连接已断开</div>';
        };

        ws.onerror = function(error) {
            console.error('预览服务器错误:', error);
            contentEl.innerHTML = '<div class="error">❌ 预览服务器错误</div>';
        };
    </script>
</body>
</html>`;
        
        res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
        res.end(html);
    }

    private serveAsset(url: string, res: http.ServerResponse) {
        // 处理静态资源请求
        res.writeHead(404);
        res.end('Asset not found');
    }

    private getPreviewPort(): number {
        const config = vscode.workspace.getConfiguration('chtl');
        return config.get('previewPort', 3000);
    }

    async togglePreview() {
        if (this.previewPanel) {
            this.previewPanel.dispose();
            this.previewPanel = null;
        } else {
            await this.showPreview();
        }
    }

    async showPreview() {
        const editor = vscode.window.activeTextEditor;
        if (!editor || (editor.document.languageId !== 'chtl' && editor.document.languageId !== 'chtl-js')) {
            vscode.window.showErrorMessage('❌ 请打开一个CHTL文件进行预览');
            return;
        }

        this.currentDocument = editor.document;

        // 创建预览面板
        this.previewPanel = vscode.window.createWebviewPanel(
            'chtlPreview',
            `预览: ${path.basename(editor.document.fileName)}`,
            vscode.ViewColumn.Beside,
            {
                enableScripts: true,
                localResourceRoots: [this.context.extensionUri]
            }
        );

        // 设置预览内容
        this.previewPanel.webview.html = this.getWebviewContent();

        // 监听面板关闭事件
        this.previewPanel.onDidDispose(() => {
            this.previewPanel = null;
            if (this.fileWatcher) {
                this.fileWatcher.close();
                this.fileWatcher = null;
            }
        });

        // 启动文件监听
        this.startFileWatching(editor.document.uri);

        // 初始编译和预览
        await this.updatePreview(editor.document);
    }

    private getWebviewContent(): string {
        const port = this.getPreviewPort();
        return `
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body, html {
            margin: 0;
            padding: 0;
            width: 100%;
            height: 100%;
            overflow: hidden;
        }
        iframe {
            width: 100%;
            height: 100%;
            border: none;
        }
    </style>
</head>
<body>
    <iframe src="http://localhost:${port}" id="preview-frame"></iframe>
</body>
</html>`;
    }

    private startFileWatching(uri: vscode.Uri) {
        if (this.fileWatcher) {
            this.fileWatcher.close();
        }

        this.fileWatcher = chokidar.watch(uri.fsPath, {
            persistent: true,
            ignoreInitial: true
        });

        this.fileWatcher.on('change', async () => {
            if (this.currentDocument) {
                await this.updatePreview(this.currentDocument);
            }
        });
    }

    async updatePreview(document: vscode.TextDocument) {
        if (!this.wss) {
            return;
        }

        try {
            // 编译文档
            const result = await this.compilerManager.compile(document.uri);
            
            // 向所有连接的客户端发送更新
            const message = {
                type: 'update',
                success: result.success,
                content: result.htmlContent || '',
                error: result.error
            };

            this.wss.clients.forEach(client => {
                if (client.readyState === WebSocket.OPEN) {
                    client.send(JSON.stringify(message));
                }
            });

        } catch (error) {
            console.error('❌ 预览更新错误:', error);
            
            const message = {
                type: 'update',
                success: false,
                content: '',
                error: String(error)
            };

            this.wss.clients.forEach(client => {
                if (client.readyState === WebSocket.OPEN) {
                    client.send(JSON.stringify(message));
                }
            });
        }
    }

    async deserializeWebviewPanel(
        webviewPanel: vscode.WebviewPanel,
        state: any
    ): Promise<void> {
        this.previewPanel = webviewPanel;
        webviewPanel.webview.html = this.getWebviewContent();

        // 重新设置事件处理
        webviewPanel.onDidDispose(() => {
            this.previewPanel = null;
            if (this.fileWatcher) {
                this.fileWatcher.close();
                this.fileWatcher = null;
            }
        });
    }

    dispose() {
        if (this.server) {
            this.server.close();
        }
        if (this.wss) {
            this.wss.close();
        }
        if (this.fileWatcher) {
            this.fileWatcher.close();
        }
        if (this.previewPanel) {
            this.previewPanel.dispose();
        }
    }
}