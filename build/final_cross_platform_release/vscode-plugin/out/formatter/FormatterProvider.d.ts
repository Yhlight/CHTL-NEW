import * as vscode from 'vscode';
export declare class CHTLFormatterProvider implements vscode.DocumentFormattingEditProvider {
    private context;
    constructor(context: vscode.ExtensionContext);
    provideDocumentFormattingEdits(document: vscode.TextDocument, options: vscode.FormattingOptions, token: vscode.CancellationToken): Promise<vscode.TextEdit[]>;
    formatDocument(document: vscode.TextDocument, editor: vscode.TextEditor): Promise<void>;
    private formatCHTL;
    private formatCHTLJS;
    private formatCHTLLine;
    private formatCppStyle;
    private formatCSSLine;
    private formatJSLine;
    static configureBracketPairs(): void;
}
//# sourceMappingURL=FormatterProvider.d.ts.map