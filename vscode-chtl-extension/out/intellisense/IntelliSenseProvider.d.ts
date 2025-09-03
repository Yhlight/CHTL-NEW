import * as vscode from 'vscode';
export declare class CHTLIntelliSenseProvider implements vscode.CompletionItemProvider, vscode.HoverProvider, vscode.DefinitionProvider {
    private context;
    private moduleManager;
    private chtlKeywords;
    private chtljsKeywords;
    private chtlAttributes;
    private cssProperties;
    constructor(context: vscode.ExtensionContext);
    provideCompletionItems(document: vscode.TextDocument, position: vscode.Position, token: vscode.CancellationToken, context: vscode.CompletionContext): Promise<vscode.CompletionItem[]>;
    private isInStyleBlock;
    private isInScriptBlock;
    private getCHTLCompletions;
    private getCSSCompletions;
    private getCHTLJSCompletions;
    private getImportCompletions;
    private getSpecialSyntaxCompletions;
    private getModuleCompletions;
    provideHover(document: vscode.TextDocument, position: vscode.Position, token: vscode.CancellationToken): Promise<vscode.Hover | null>;
    provideDefinition(document: vscode.TextDocument, position: vscode.Position, token: vscode.CancellationToken): Promise<vscode.Definition | null>;
    private getElementDescription;
    private getCSSPropertyDescription;
}
//# sourceMappingURL=IntelliSenseProvider.d.ts.map