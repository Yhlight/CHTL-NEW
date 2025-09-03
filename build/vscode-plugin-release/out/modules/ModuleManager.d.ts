import * as vscode from 'vscode';
export interface ModuleInfo {
    name: string;
    type: 'CMOD' | 'CJMOD' | 'CMOD+CJMOD';
    version: string;
    description?: string;
    documentation?: string;
    exports: string[];
    functions?: CJMODFunction[];
    cjmodAPI?: CJMODAPIInfo;
    path: string;
}
export interface CJMODFunction {
    name: string;
    syntax: string;
    description: string;
    parameters: string[];
    placeholders?: PlaceholderInfo[];
    supportsVir?: boolean;
}
export interface CJMODAPIInfo {
    syntaxPatterns: string[];
    scanningMethods: string[];
    placeholderTypes: string[];
    virSupport: boolean;
    functions: CJMODFunction[];
}
export interface PlaceholderInfo {
    type: '$' | '$?' | '$!' | '$_' | '$!_' | '...';
    description: string;
    required: boolean;
    unordered: boolean;
}
export declare class CHTLModuleManager {
    private context;
    private moduleCache;
    private queryTable;
    private officialModulesPath;
    constructor(context: vscode.ExtensionContext);
    initialize(): Promise<void>;
    /**
     * 加载压缩模块（.cmod文件）
     * 支持CMOD、CJMOD、CMOD+CJMOD三种结构
     */
    private loadCompressedModule;
    /**
     * 加载目录模块
     * 自动识别CMOD、CJMOD、CMOD+CJMOD结构
     */
    private loadDirectoryModule;
    /**
     * 检测模块结构类型
     */
    private detectModuleStructure;
    /**
     * 使用CJMOD API提取模块信息
     * 这是自动解包的核心，依靠CJMOD的scan、CreateCHTLJSFunction、语法分析方法
     */
    private extractModuleWithCJMOD;
    /**
     * 模拟CJMOD::Syntax::analyze调用
     */
    private simulateCJMODSyntaxAnalyze;
    /**
     * 模拟CJMOD::CJMODScanner::scan调用
     * 使用双指针扫描法和前置截取法分析模块
     */
    private simulateCJMODScan;
    /**
     * 提取CJMOD函数（使用CreateCHTLJSFunction分析）
     */
    private extractCJMODFunctions;
    /**
     * 模拟CJMOD::CHTLJSFunction::CreateCHTLJSFunction
     */
    private simulateCreateCHTLJSFunction;
    /**
     * 分析CJMOD API信息
     */
    private analyzeCJMODAPI;
    /**
     * 生成CJMOD IntelliSense
     * 依靠CJMOD的scan、CreateCHTLJSFunction、语法分析方法获取语法提示
     */
    private generateCJMODIntelliSense;
    /**
     * 生成CJMOD语法提示
     */
    private generateCJMODSyntaxHints;
    /**
     * 从模式中提取占位符
     */
    private extractPlaceholdersFromPattern;
    /**
     * 获取占位符描述
     */
    private getPlaceholderDescription;
    /**
     * 获取CJMOD源码路径
     */
    private getCJMODSrcPath;
    /**
     * 检查是否有源码文件（CJMOD特征）
     */
    private hasSourceFiles;
    /**
     * 检查是否有info文件（CMOD特征）
     */
    private hasInfoFiles;
    /**
     * 分析CJMOD模块
     */
    private analyzeCJMODModule;
    /**
     * 分析CMOD模块信息
     */
    private analyzeCMODModuleInfo;
    private scanModules;
    private scanOfficialModules;
    private scanWorkspaceModules;
    private scanCMODModules;
    private scanCJMODModules;
    private parseCMODInfo;
    private parseCJMODInfo;
    private extractCMODExports;
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