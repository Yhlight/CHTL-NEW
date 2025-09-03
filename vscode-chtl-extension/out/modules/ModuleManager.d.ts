import * as vscode from 'vscode';
export interface ModuleInfo {
    name: string;
    type: 'CMOD' | 'CJMOD';
    version: string;
    description?: string;
    documentation?: string;
    exports: string[];
    functions?: CJMODFunction[];
    path: string;
}
export interface CJMODFunction {
    name: string;
    syntax: string;
    description: string;
    parameters: string[];
}
export declare class CHTLModuleManager {
    private context;
    private moduleCache;
    private queryTable;
    private officialModulesPath;
    constructor(context: vscode.ExtensionContext);
    initialize(): Promise<void>;
    private scanModules;
    private scanOfficialModules;
    private scanWorkspaceModules;
    private scanCMODModules;
    private scanCJMODModules;
    private parseCMODInfo;
    private parseCJMODInfo;
    private extractCMODExports;
    private extractCJMODFunctions;
    private parseCJMODSyntaxFunctions;
    private extractParameters;
    private extractDescription;
    private generateQueryTable;
    getAvailableModules(): Promise<ModuleInfo[]>;
    getModuleInfo(moduleName: string): Promise<ModuleInfo | null>;
    findModuleDefinition(moduleName: string): Promise<string | null>;
    getQueryTable(): any;
    reload(): Promise<void>;
    getModuleCompletions(moduleType?: 'CMOD' | 'CJMOD'): any[];
}
//# sourceMappingURL=ModuleManager.d.ts.map