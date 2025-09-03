import * as vscode from 'vscode';
export interface CompileResult {
    success: boolean;
    outputPath?: string;
    error?: string;
    htmlContent?: string;
    cssContent?: string;
    jsContent?: string;
}
export declare class CHTLCompilerManager {
    private context;
    private compilerPath;
    private builtinCompilerPath;
    constructor(context: vscode.ExtensionContext);
    private initializeCompiler;
    compile(uri: vscode.Uri): Promise<CompileResult>;
    exportHTML(uri: vscode.Uri): Promise<CompileResult>;
    exportCSS(uri: vscode.Uri): Promise<CompileResult>;
    exportJS(uri: vscode.Uri): Promise<CompileResult>;
    getCompilerVersion(): Promise<string>;
    isCompilerAvailable(): Promise<boolean>;
    reload(): void;
    private extractCSSFromHTML;
    private extractJSFromHTML;
}
//# sourceMappingURL=CompilerManager.d.ts.map