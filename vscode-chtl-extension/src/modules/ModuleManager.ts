import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';

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

export class CHTLModuleManager {
    private context: vscode.ExtensionContext;
    private moduleCache: Map<string, ModuleInfo> = new Map();
    private queryTable: any = {};
    private officialModulesPath: string = '';

    constructor(context: vscode.ExtensionContext) {
        this.context = context;
        this.initialize();
    }

    async initialize() {
        // 获取官方模块路径
        const config = vscode.workspace.getConfiguration('chtl');
        this.officialModulesPath = config.get('officialModulesPath', '');
        
        if (!this.officialModulesPath) {
            // 尝试从编译器路径推断模块路径
            const compilerPath = config.get('compilerPath', '');
            if (compilerPath) {
                this.officialModulesPath = path.join(path.dirname(compilerPath), 'modules');
            }
        }

        // 扫描并加载模块
        await this.scanModules();
        
        // 生成查询表
        this.generateQueryTable();

        console.log(`📦 已加载 ${this.moduleCache.size} 个模块`);
    }

    private async scanModules() {
        this.moduleCache.clear();

        // 扫描官方模块
        if (this.officialModulesPath && fs.existsSync(this.officialModulesPath)) {
            await this.scanOfficialModules();
        }

        // 扫描工作区模块
        if (vscode.workspace.workspaceFolders) {
            for (const folder of vscode.workspace.workspaceFolders) {
                await this.scanWorkspaceModules(folder.uri.fsPath);
            }
        }
    }

    private async scanOfficialModules() {
        const modulesPath = this.officialModulesPath;
        
        try {
            // 扫描CMOD模块
            const cmodPath = path.join(modulesPath, 'CMOD');
            if (fs.existsSync(cmodPath)) {
                await this.scanCMODModules(cmodPath);
            }

            // 扫描CJMOD模块
            const cjmodPath = path.join(modulesPath, 'CJMOD');
            if (fs.existsSync(cjmodPath)) {
                await this.scanCJMODModules(cjmodPath);
            }

        } catch (error) {
            console.error('扫描官方模块失败:', error);
        }
    }

    private async scanWorkspaceModules(workspacePath: string) {
        const moduleDir = path.join(workspacePath, 'modules');
        if (!fs.existsSync(moduleDir)) {
            return;
        }

        try {
            // 扫描工作区CMOD模块
            const cmodPath = path.join(moduleDir, 'CMOD');
            if (fs.existsSync(cmodPath)) {
                await this.scanCMODModules(cmodPath);
            }

            // 扫描工作区CJMOD模块
            const cjmodPath = path.join(moduleDir, 'CJMOD');
            if (fs.existsSync(cjmodPath)) {
                await this.scanCJMODModules(cjmodPath);
            }

        } catch (error) {
            console.error('扫描工作区模块失败:', error);
        }
    }

    private async scanCMODModules(cmodPath: string) {
        const entries = fs.readdirSync(cmodPath, { withFileTypes: true });
        
        for (const entry of entries) {
            if (entry.isDirectory()) {
                const modulePath = path.join(cmodPath, entry.name);
                const infoPath = path.join(modulePath, 'info');
                
                if (fs.existsSync(infoPath)) {
                    const moduleInfo = await this.parseCMODInfo(entry.name, modulePath);
                    if (moduleInfo) {
                        this.moduleCache.set(entry.name, moduleInfo);
                    }
                }
            }
        }
    }

    private async scanCJMODModules(cjmodPath: string) {
        const entries = fs.readdirSync(cjmodPath, { withFileTypes: true });
        
        for (const entry of entries) {
            if (entry.isDirectory()) {
                const modulePath = path.join(cjmodPath, entry.name);
                const infoPath = path.join(modulePath, 'info');
                
                if (fs.existsSync(infoPath)) {
                    const moduleInfo = await this.parseCJMODInfo(entry.name, modulePath);
                    if (moduleInfo) {
                        this.moduleCache.set(entry.name, moduleInfo);
                    }
                }
            }
        }
    }

    private async parseCMODInfo(moduleName: string, modulePath: string): Promise<ModuleInfo | null> {
        try {
            const infoFile = path.join(modulePath, 'info', `${moduleName}.chtl`);
            if (!fs.existsSync(infoFile)) {
                return null;
            }

            const content = fs.readFileSync(infoFile, 'utf8');
            const exports = this.extractCMODExports(content);
            
            return {
                name: moduleName,
                type: 'CMOD',
                version: '1.0.0', // 从info文件中解析
                description: this.extractDescription(content),
                exports,
                path: modulePath
            };

        } catch (error) {
            console.error(`解析CMOD模块 ${moduleName} 失败:`, error);
            return null;
        }
    }

    private async parseCJMODInfo(moduleName: string, modulePath: string): Promise<ModuleInfo | null> {
        try {
            const infoFile = path.join(modulePath, 'info', `${moduleName}.chtl`);
            if (!fs.existsSync(infoFile)) {
                return null;
            }

            const content = fs.readFileSync(infoFile, 'utf8');
            
            // 解析CJMOD函数
            const functions = await this.extractCJMODFunctions(modulePath);
            
            return {
                name: moduleName,
                type: 'CJMOD',
                version: '1.0.0',
                description: this.extractDescription(content),
                exports: functions.map(f => f.name),
                functions,
                path: modulePath
            };

        } catch (error) {
            console.error(`解析CJMOD模块 ${moduleName} 失败:`, error);
            return null;
        }
    }

    private extractCMODExports(content: string): string[] {
        const exports: string[] = [];
        const exportMatch = content.match(/\[Export\]\s*\{([^}]*)\}/s);
        
        if (exportMatch) {
            const exportContent = exportMatch[1];
            const lines = exportContent.split('\n');
            
            for (const line of lines) {
                const trimmed = line.trim();
                if (trimmed && !trimmed.startsWith('//') && !trimmed.startsWith('/*')) {
                    const exportName = trimmed.split(':')[0].trim();
                    if (exportName) {
                        exports.push(exportName);
                    }
                }
            }
        }
        
        return exports;
    }

    private async extractCJMODFunctions(modulePath: string): Promise<CJMODFunction[]> {
        const functions: CJMODFunction[] = [];
        const srcPath = path.join(modulePath, 'src');
        
        if (!fs.existsSync(srcPath)) {
            return functions;
        }

        try {
            const entries = fs.readdirSync(srcPath, { withFileTypes: true });
            
            for (const entry of entries) {
                if (entry.isFile() && entry.name.endsWith('.cpp')) {
                    const filePath = path.join(srcPath, entry.name);
                    const content = fs.readFileSync(filePath, 'utf8');
                    
                    // 解析语法函数
                    const syntaxFunctions = this.parseCJMODSyntaxFunctions(content);
                    functions.push(...syntaxFunctions);
                }
            }

        } catch (error) {
            console.error(`提取CJMOD函数失败:`, error);
        }

        return functions;
    }

    private parseCJMODSyntaxFunctions(content: string): CJMODFunction[] {
        const functions: CJMODFunction[] = [];
        
        // 查找syntax方法
        const syntaxMatches = content.matchAll(/std::string\s+(\w+)\s*\([^)]*\)\s*\{[^}]*syntax\s*\([^)]*"([^"]+)"/g);
        
        for (const match of syntaxMatches) {
            const functionName = match[1];
            const syntax = match[2];
            
            functions.push({
                name: functionName,
                syntax: syntax,
                description: `CJMOD函数: ${functionName}`,
                parameters: this.extractParameters(syntax)
            });
        }

        return functions;
    }

    private extractParameters(syntax: string): string[] {
        const parameters: string[] = [];
        const paramMatches = syntax.matchAll(/\$(\w*)/g);
        
        for (const match of paramMatches) {
            const param = match[1] || '$';
            if (!parameters.includes(param)) {
                parameters.push(param);
            }
        }

        return parameters;
    }

    private extractDescription(content: string): string {
        // 从注释中提取描述
        const descMatch = content.match(/\/\*\*\s*([^*]*)\*\//);
        if (descMatch) {
            return descMatch[1].trim();
        }

        const lineDescMatch = content.match(/\/\/\s*(.+)/);
        if (lineDescMatch) {
            return lineDescMatch[1].trim();
        }

        return '';
    }

    private generateQueryTable() {
        this.queryTable = {
            modules: {},
            exports: {},
            functions: {}
        };

        for (const [name, module] of this.moduleCache) {
            this.queryTable.modules[name] = {
                type: module.type,
                version: module.version,
                description: module.description,
                path: module.path
            };

            // 索引导出项
            for (const exportName of module.exports) {
                if (!this.queryTable.exports[exportName]) {
                    this.queryTable.exports[exportName] = [];
                }
                this.queryTable.exports[exportName].push({
                    module: name,
                    type: module.type
                });
            }

            // 索引CJMOD函数
            if (module.functions) {
                for (const func of module.functions) {
                    this.queryTable.functions[func.name] = {
                        module: name,
                        syntax: func.syntax,
                        description: func.description,
                        parameters: func.parameters
                    };
                }
            }
        }

        console.log('🔍 模块查询表已生成');
    }

    async getAvailableModules(): Promise<ModuleInfo[]> {
        return Array.from(this.moduleCache.values());
    }

    async getModuleInfo(moduleName: string): Promise<ModuleInfo | null> {
        return this.moduleCache.get(moduleName) || null;
    }

    async findModuleDefinition(moduleName: string): Promise<string | null> {
        const module = this.moduleCache.get(moduleName);
        if (module) {
            return module.path;
        }
        return null;
    }

    getQueryTable(): any {
        return this.queryTable;
    }

    async reload() {
        console.log('🔄 重新加载模块...');
        await this.initialize();
    }

    // 获取模块的智能提示信息
    getModuleCompletions(moduleType?: 'CMOD' | 'CJMOD'): any[] {
        const completions: any[] = [];

        for (const [name, module] of this.moduleCache) {
            if (!moduleType || module.type === moduleType) {
                completions.push({
                    name: module.name,
                    type: module.type,
                    description: module.description,
                    exports: module.exports,
                    functions: module.functions
                });
            }
        }

        return completions;
    }
}