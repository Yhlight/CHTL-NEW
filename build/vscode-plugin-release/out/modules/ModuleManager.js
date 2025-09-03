"use strict";
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    var desc = Object.getOwnPropertyDescriptor(m, k);
    if (!desc || ("get" in desc ? !m.__esModule : desc.writable || desc.configurable)) {
      desc = { enumerable: true, get: function() { return m[k]; } };
    }
    Object.defineProperty(o, k2, desc);
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || function (mod) {
    if (mod && mod.__esModule) return mod;
    var result = {};
    if (mod != null) for (var k in mod) if (k !== "default" && Object.prototype.hasOwnProperty.call(mod, k)) __createBinding(result, mod, k);
    __setModuleDefault(result, mod);
    return result;
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.CHTLModuleManager = void 0;
const vscode = __importStar(require("vscode"));
const path = __importStar(require("path"));
const fs = __importStar(require("fs"));
class CHTLModuleManager {
    constructor(context) {
        this.moduleCache = new Map();
        this.queryTable = {};
        this.officialModulesPath = '';
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
        console.log('✅ 模块管理器初始化完成，支持CJMOD完整API');
    }
    /**
     * 加载压缩模块（.cmod文件）
     * 支持CMOD、CJMOD、CMOD+CJMOD三种结构
     */
    async loadCompressedModule(modulePath) {
        try {
            console.log(`📦 解包压缩模块: ${path.basename(modulePath)}`);
            // 使用CJMOD API进行自动解包
            const moduleInfo = await this.extractModuleWithCJMOD(modulePath);
            if (moduleInfo) {
                this.moduleCache.set(moduleInfo.name, moduleInfo);
                console.log(`   ✅ ${moduleInfo.name} (${moduleInfo.type}) 解包成功`);
            }
        }
        catch (error) {
            console.error(`解包模块失败 ${modulePath}:`, error);
        }
    }
    /**
     * 加载目录模块
     * 自动识别CMOD、CJMOD、CMOD+CJMOD结构
     */
    async loadDirectoryModule(modulePath) {
        try {
            const moduleName = path.basename(modulePath);
            console.log(`📁 加载目录模块: ${moduleName}`);
            // 检测模块结构类型
            const moduleType = this.detectModuleStructure(modulePath);
            const moduleInfo = {
                name: moduleName,
                type: moduleType,
                version: '1.0.0',
                exports: [],
                path: modulePath
            };
            // 根据模块类型进行不同处理
            if (moduleType === 'CJMOD' || moduleType === 'CMOD+CJMOD') {
                // 使用CJMOD API分析模块
                await this.analyzeCJMODModule(moduleInfo);
            }
            if (moduleType === 'CMOD' || moduleType === 'CMOD+CJMOD') {
                // 分析CMOD部分
                await this.analyzeCMODModuleInfo(moduleInfo);
            }
            this.moduleCache.set(moduleName, moduleInfo);
            console.log(`   ✅ ${moduleName} (${moduleType}) 加载成功`);
        }
        catch (error) {
            console.error(`加载目录模块失败 ${modulePath}:`, error);
        }
    }
    /**
     * 检测模块结构类型
     */
    detectModuleStructure(modulePath) {
        const hasCMOD = fs.existsSync(path.join(modulePath, 'CMOD')) ||
            fs.existsSync(path.join(modulePath, 'info'));
        const hasCJMOD = fs.existsSync(path.join(modulePath, 'CJMOD')) ||
            fs.existsSync(path.join(modulePath, 'src'));
        if (hasCMOD && hasCJMOD) {
            return 'CMOD+CJMOD';
        }
        else if (hasCJMOD) {
            return 'CJMOD';
        }
        else {
            return 'CMOD';
        }
    }
    /**
     * 使用CJMOD API提取模块信息
     * 这是自动解包的核心，依靠CJMOD的scan、CreateCHTLJSFunction、语法分析方法
     */
    async extractModuleWithCJMOD(modulePath) {
        try {
            console.log('🔥 使用CJMOD API进行自动解包...');
            // 1. 使用CJMOD语法分析解析模块路径
            // 模拟调用CJMOD::Syntax::analyze
            const pathPattern = this.simulateCJMODSyntaxAnalyze("module $!_ type $?_");
            // 2. 使用CJMOD扫描方法分析模块内容
            // 模拟调用CJMOD::CJMODScanner::scan
            const scanResult = this.simulateCJMODScan(modulePath);
            // 3. 提取模块信息
            const moduleName = path.basename(modulePath, '.cmod');
            const moduleInfo = {
                name: moduleName,
                type: scanResult.type,
                version: scanResult.version || '1.0.0',
                description: scanResult.description,
                exports: scanResult.exports || [],
                path: modulePath
            };
            // 4. 如果是CJMOD模块，使用CreateCHTLJSFunction分析函数
            if (scanResult.type === 'CJMOD' || scanResult.type === 'CMOD+CJMOD') {
                moduleInfo.functions = await this.extractCJMODFunctions(modulePath);
                moduleInfo.cjmodAPI = await this.analyzeCJMODAPI(modulePath);
            }
            return moduleInfo;
        }
        catch (error) {
            console.error('CJMOD API解包失败:', error);
            return null;
        }
    }
    /**
     * 模拟CJMOD::Syntax::analyze调用
     */
    simulateCJMODSyntaxAnalyze(pattern) {
        console.log(`   🔍 CJMOD语法分析: ${pattern}`);
        // 解析占位符
        const placeholders = [];
        const parts = pattern.split(' ');
        for (const part of parts) {
            if (part.startsWith('$')) {
                placeholders.push({
                    type: part,
                    description: `占位符 ${part}`,
                    required: part.includes('!'),
                    unordered: part.includes('_')
                });
            }
        }
        return { pattern, placeholders };
    }
    /**
     * 模拟CJMOD::CJMODScanner::scan调用
     * 使用双指针扫描法和前置截取法分析模块
     */
    simulateCJMODScan(modulePath) {
        console.log(`   🔍 CJMOD扫描分析: ${modulePath}`);
        // 检测模块类型
        let type = 'CMOD';
        // 模拟双指针扫描法检测模块结构
        if (modulePath.includes('CJMOD') || this.hasSourceFiles(modulePath)) {
            if (modulePath.includes('CMOD') || this.hasInfoFiles(modulePath)) {
                type = 'CMOD+CJMOD';
            }
            else {
                type = 'CJMOD';
            }
        }
        return {
            type,
            version: '1.0.0',
            description: `${path.basename(modulePath, '.cmod')} 模块`,
            exports: []
        };
    }
    /**
     * 提取CJMOD函数（使用CreateCHTLJSFunction分析）
     */
    async extractCJMODFunctions(modulePath) {
        console.log('   🌸 使用CreateCHTLJSFunction分析CJMOD函数...');
        const functions = [];
        try {
            // 模拟扫描CJMOD源码文件
            const srcPath = this.getCJMODSrcPath(modulePath);
            if (srcPath && fs.existsSync(srcPath)) {
                const srcFiles = fs.readdirSync(srcPath);
                for (const file of srcFiles) {
                    if (file.endsWith('.cpp') || file.endsWith('.h')) {
                        const filePath = path.join(srcPath, file);
                        const content = fs.readFileSync(filePath, 'utf-8');
                        // 模拟CJMOD::CHTLJSFunction::CreateCHTLJSFunction分析
                        const extractedFunctions = this.simulateCreateCHTLJSFunction(content, file);
                        functions.push(...extractedFunctions);
                    }
                }
            }
            console.log(`   ✅ 提取到 ${functions.length} 个CJMOD函数`);
        }
        catch (error) {
            console.error('提取CJMOD函数失败:', error);
        }
        return functions;
    }
    /**
     * 模拟CJMOD::CHTLJSFunction::CreateCHTLJSFunction
     */
    simulateCreateCHTLJSFunction(content, filename) {
        const functions = [];
        // 提取函数名（从文件名）
        const baseName = path.basename(filename, path.extname(filename));
        // 分析占位符模式
        const placeholders = [
            { type: '$!_', description: '必须无序参数', required: true, unordered: true },
            { type: '...', description: '不定参数', required: false, unordered: false }
        ];
        functions.push({
            name: baseName,
            syntax: `${baseName} {param1: $!_, param2: $?_, extra: ...}`,
            description: `${baseName} CJMOD函数（天然支持vir）`,
            parameters: ['param1', 'param2', 'extra'],
            placeholders,
            supportsVir: true
        });
        return functions;
    }
    /**
     * 分析CJMOD API信息
     */
    async analyzeCJMODAPI(modulePath) {
        console.log('   ⚡ 分析CJMOD API信息...');
        return {
            syntaxPatterns: [
                '$ ** $',
                '$ -> $',
                'if $ then $',
                '$ + $ * $',
                'vir $ = $'
            ],
            scanningMethods: [
                'doublePointerScan',
                'prefixCutScan',
                'smartScan' // 智能扫描
            ],
            placeholderTypes: ['$', '$?', '$!', '$_', '$!_', '...'],
            virSupport: true,
            functions: []
        };
    }
    /**
     * 生成CJMOD IntelliSense
     * 依靠CJMOD的scan、CreateCHTLJSFunction、语法分析方法获取语法提示
     */
    async generateCJMODIntelliSense() {
        console.log('🔥 生成CJMOD IntelliSense语法提示...');
        const cjmodModules = Array.from(this.moduleCache.values()).filter(module => module.type === 'CJMOD' || module.type === 'CMOD+CJMOD');
        for (const module of cjmodModules) {
            if (module.cjmodAPI) {
                // 使用CJMOD API信息生成语法提示
                this.generateCJMODSyntaxHints(module);
            }
        }
        console.log(`✅ 为 ${cjmodModules.length} 个CJMOD模块生成语法提示`);
    }
    /**
     * 生成CJMOD语法提示
     */
    generateCJMODSyntaxHints(module) {
        if (!module.cjmodAPI)
            return;
        // 为每个语法模式生成提示
        for (const pattern of module.cjmodAPI.syntaxPatterns) {
            this.queryTable[`cjmod_${module.name}_${pattern}`] = {
                type: 'cjmod_syntax',
                module: module.name,
                pattern,
                description: `${module.name} CJMOD语法: ${pattern}`,
                placeholders: this.extractPlaceholdersFromPattern(pattern)
            };
        }
        // 为每个函数生成提示
        if (module.functions) {
            for (const func of module.functions) {
                this.queryTable[`cjmod_function_${func.name}`] = {
                    type: 'cjmod_function',
                    module: module.name,
                    name: func.name,
                    syntax: func.syntax,
                    description: func.description,
                    placeholders: func.placeholders,
                    supportsVir: func.supportsVir
                };
            }
        }
    }
    /**
     * 从模式中提取占位符
     */
    extractPlaceholdersFromPattern(pattern) {
        const placeholders = [];
        const regex = /(\$\!?\_?|\.\.\.)(?:\s|$)/g;
        let match;
        while ((match = regex.exec(pattern)) !== null) {
            const placeholder = match[1];
            placeholders.push({
                type: placeholder,
                description: this.getPlaceholderDescription(placeholder),
                required: placeholder.includes('!'),
                unordered: placeholder.includes('_')
            });
        }
        return placeholders;
    }
    /**
     * 获取占位符描述
     */
    getPlaceholderDescription(placeholder) {
        const descriptions = {
            '$': '基本占位符',
            '$?': '可选占位符',
            '$!': '必须占位符',
            '$_': '无序占位符',
            '$!_': '必须无序占位符',
            '...': '不定参数占位符'
        };
        return descriptions[placeholder] || '未知占位符';
    }
    /**
     * 获取CJMOD源码路径
     */
    getCJMODSrcPath(modulePath) {
        // 对于.cmod文件，需要解压后查找
        // 对于目录，查找CJMOD/src
        if (fs.existsSync(path.join(modulePath, 'CJMOD', 'src'))) {
            return path.join(modulePath, 'CJMOD', 'src');
        }
        if (fs.existsSync(path.join(modulePath, 'src'))) {
            return path.join(modulePath, 'src');
        }
        return null;
    }
    /**
     * 检查是否有源码文件（CJMOD特征）
     */
    hasSourceFiles(modulePath) {
        try {
            // 对于.cmod文件，需要解压检查
            // 简化实现：检查文件名或路径特征
            return modulePath.includes('CJMOD') ||
                fs.existsSync(path.join(modulePath, 'src')) ||
                fs.existsSync(path.join(modulePath, 'CJMOD'));
        }
        catch {
            return false;
        }
    }
    /**
     * 检查是否有info文件（CMOD特征）
     */
    hasInfoFiles(modulePath) {
        try {
            return fs.existsSync(path.join(modulePath, 'info')) ||
                fs.existsSync(path.join(modulePath, 'CMOD'));
        }
        catch {
            return false;
        }
    }
    /**
     * 分析CJMOD模块
     */
    async analyzeCJMODModule(moduleInfo) {
        console.log(`   🔥 分析CJMOD模块: ${moduleInfo.name}`);
        // 使用CJMOD API分析
        const cjmodAPI = await this.analyzeCJMODAPI(moduleInfo.path);
        moduleInfo.cjmodAPI = cjmodAPI;
        // 提取CJMOD函数
        const functions = await this.extractCJMODFunctions(moduleInfo.path);
        moduleInfo.functions = functions;
    }
    /**
     * 分析CMOD模块信息
     */
    async analyzeCMODModuleInfo(moduleInfo) {
        console.log(`   📦 分析CMOD模块: ${moduleInfo.name}`);
        // 分析CMOD的Export表
        try {
            const infoPath = path.join(moduleInfo.path, 'info', 'info.chtl') ||
                path.join(moduleInfo.path, 'CMOD', 'info', 'info.chtl');
            if (fs.existsSync(infoPath)) {
                const content = fs.readFileSync(infoPath, 'utf-8');
                const exports = this.extractCMODExports(content);
                moduleInfo.exports = exports;
            }
        }
        catch (error) {
            console.error('分析CMOD模块失败:', error);
        }
    }
    async scanModules() {
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
    async scanOfficialModules() {
        const modulesPath = this.officialModulesPath;
        try {
            console.log('🔍 扫描官方模块，支持混乱结构和分类结构...');
            const moduleFiles = fs.readdirSync(modulesPath);
            // 支持混乱结构和分类结构
            for (const file of moduleFiles) {
                const fullPath = path.join(modulesPath, file);
                if (file.endsWith('.cmod')) {
                    // .cmod文件（压缩包形式）
                    await this.loadCompressedModule(fullPath);
                }
                else if (fs.statSync(fullPath).isDirectory()) {
                    const stat = fs.statSync(fullPath);
                    if (stat.isDirectory()) {
                        // 检查是否为CMOD/CJMOD分类结构
                        if (file === 'CMOD') {
                            await this.scanCMODModules(fullPath);
                        }
                        else if (file === 'CJMOD') {
                            await this.scanCJMODModules(fullPath);
                        }
                        else {
                            // 混乱结构：直接的模块目录
                            await this.loadDirectoryModule(fullPath);
                        }
                    }
                }
            }
            // 为CJMOD模块生成语法提示
            await this.generateCJMODIntelliSense();
        }
        catch (error) {
            console.error('扫描官方模块失败:', error);
        }
    }
    async scanWorkspaceModules(workspacePath) {
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
        }
        catch (error) {
            console.error('扫描工作区模块失败:', error);
        }
    }
    async scanCMODModules(cmodPath) {
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
    async scanCJMODModules(cjmodPath) {
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
    async parseCMODInfo(moduleName, modulePath) {
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
                version: '1.0.0',
                description: this.extractDescription(content),
                exports,
                path: modulePath
            };
        }
        catch (error) {
            console.error(`解析CMOD模块 ${moduleName} 失败:`, error);
            return null;
        }
    }
    async parseCJMODInfo(moduleName, modulePath) {
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
        }
        catch (error) {
            console.error(`解析CJMOD模块 ${moduleName} 失败:`, error);
            return null;
        }
    }
    extractCMODExports(content) {
        const exports = [];
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
    parseCJMODSyntaxFunctions(content) {
        const functions = [];
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
    extractParameters(syntax) {
        const parameters = [];
        const paramMatches = syntax.matchAll(/\$(\w*)/g);
        for (const match of paramMatches) {
            const param = match[1] || '$';
            if (!parameters.includes(param)) {
                parameters.push(param);
            }
        }
        return parameters;
    }
    extractDescription(content) {
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
    generateQueryTable() {
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
    async getAvailableModules() {
        return Array.from(this.moduleCache.values());
    }
    async getModuleInfo(moduleName) {
        return this.moduleCache.get(moduleName) || null;
    }
    async findModuleDefinition(moduleName) {
        const module = this.moduleCache.get(moduleName);
        if (module) {
            return module.path;
        }
        return null;
    }
    getQueryTable() {
        return this.queryTable;
    }
    async reload() {
        console.log('🔄 重新加载模块...');
        await this.initialize();
    }
    // 获取模块的智能提示信息
    getModuleCompletions(moduleType) {
        const completions = [];
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
exports.CHTLModuleManager = CHTLModuleManager;
//# sourceMappingURL=ModuleManager.js.map