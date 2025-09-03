import * as vscode from 'vscode';
export declare class CHTLPreviewProvider implements vscode.WebviewPanelSerializer {
    private context;
    private compilerManager;
    private previewPanel;
    private server;
    private wss;
    private fileWatcher;
    private currentDocument;
    constructor(context: vscode.ExtensionContext);
    private initializePreviewServer;
    private servePreviewHTML;
    private serveAsset;
    private getPreviewPort;
    togglePreview(): Promise<void>;
    showPreview(): Promise<void>;
    private getWebviewContent;
    private startFileWatching;
    updatePreview(document: vscode.TextDocument): Promise<void>;
    deserializeWebviewPanel(webviewPanel: vscode.WebviewPanel, state: any): Promise<void>;
    dispose(): void;
}
//# sourceMappingURL=PreviewProvider.d.ts.map