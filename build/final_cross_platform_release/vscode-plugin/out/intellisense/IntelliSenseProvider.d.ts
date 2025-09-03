import * as vscode from 'vscode';
export declare class CHTLIntelliSenseProvider implements vscode.CompletionItemProvider, vscode.HoverProvider, vscode.DefinitionProvider {
    private context;
    private moduleManager;
    private chtlKeywords;
    private chtljsKeywords;
    private cjmodKeywords;
    private cjmodOperators;
    private cjmodPlaceholders;
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
    /**
     * 检查是否导入了CJMOD
     */
    private hasCJMODImport;
    /**
     * 获取CJMOD补全项
     * 依靠CJMOD的scan、CreateCHTLJSFunction、语法分析方法获取语法提示
     */
    private getCJMODCompletions;
    /**
     * 获取CJMOD函数补全（使用CreateCHTLJSFunction分析的结果）
     */
    private getCJMODFunctionCompletions;
    /**
     * 获取CJMOD语法模式补全
     */
    private getCJMODSyntaxPatterns;
}
//# sourceMappingURL=IntelliSenseProvider.d.ts.map