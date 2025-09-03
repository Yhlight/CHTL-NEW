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
exports.CHTLCompilerManager = void 0;
const vscode = __importStar(require("vscode"));
const path = __importStar(require("path"));
const fs = __importStar(require("fs"));
const child_process_1 = require("child_process");
const util_1 = require("util");
const execAsync = (0, util_1.promisify)(child_process_1.exec);
class CHTLCompilerManager {
    constructor(context) {
        this.compilerPath = '';
        this.builtinCompilerPath = '';
        this.context = context;
        this.initializeCompiler();
    }
    initializeCompiler() {
        // 获取配置的编译器路径
        const config = vscode.workspace.getConfiguration('chtl');
        this.compilerPath = config.get('compilerPath', '');
        // 设置内置编译器路径（包含CJMOD支持）
        this.builtinCompilerPath = path.join(this.context.extensionPath, 'bin', 'chtl-professional');
        // 设置内置编译器路径（使用工作区编译器）
        const workspaceRoot = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath || '';
        this.builtinCompilerPath = path.join(workspaceRoot, 'build', 'bin', 'chtl');
        // 如果没有配置外部编译器，使用内置编译器
        if (!this.compilerPath) {
            this.compilerPath = this.builtinCompilerPath;
        }
        console.log(`📦 CHTL编译器路径: ${this.compilerPath}`);
    }
    async compile(uri) {
        try {
            const filePath = uri.fsPath;
            const fileName = path.basename(filePath, path.extname(filePath));
            const outputDir = path.join(path.dirname(filePath), 'output');
            // 确保输出目录存在
            if (!fs.existsSync(outputDir)) {
                fs.mkdirSync(outputDir, { recursive: true });
            }
            const outputPath = path.join(outputDir, `${fileName}.html`);
            // 检查编译器是否存在
            if (!fs.existsSync(this.compilerPath)) {
                return {
                    success: false,
                    error: `编译器不存在: ${this.compilerPath}`
                };
            }
            // 执行编译命令（CHTL编译器格式：chtl input.chtl output.html）
            const command = `"${this.compilerPath}" "${filePath}" "${outputPath}"`;
            console.log(`🔧 执行编译命令: ${command}`);
            const { stdout, stderr } = await execAsync(command, {
                timeout: 30000 // 30秒超时
            });
            if (stderr && stderr.trim()) {
                console.warn(`⚠️ 编译警告: ${stderr}`);
            }
            // 检查输出文件是否生成
            if (fs.existsSync(outputPath)) {
                const htmlContent = fs.readFileSync(outputPath, 'utf8');
                return {
                    success: true,
                    outputPath,
                    htmlContent
                };
            }
            else {
                return {
                    success: false,
                    error: '编译完成但未生成输出文件'
                };
            }
        }
        catch (error) {
            console.error(`❌ 编译错误:`, error);
            return {
                success: false,
                error: error.message || String(error)
            };
        }
    }
    async exportHTML(uri) {
        const result = await this.compile(uri);
        if (result.success && result.outputPath) {
            // HTML已经在compile方法中生成
            return result;
        }
        return result;
    }
    async exportCSS(uri) {
        try {
            const filePath = uri.fsPath;
            const fileName = path.basename(filePath, path.extname(filePath));
            const outputDir = path.join(path.dirname(filePath), 'output');
            const cssOutputPath = path.join(outputDir, `${fileName}.css`);
            // 确保输出目录存在
            if (!fs.existsSync(outputDir)) {
                fs.mkdirSync(outputDir, { recursive: true });
            }
            // 执行CSS导出命令（先编译到HTML，然后提取CSS）
            const tempHtmlPath = path.join(outputDir, `${fileName}_temp.html`);
            const command = `"${this.compilerPath}" "${filePath}" "${tempHtmlPath}"`;
            console.log(`🎨 执行CSS导出命令: ${command}`);
            const { stdout, stderr } = await execAsync(command, {
                timeout: 30000
            });
            if (stderr && stderr.trim()) {
                console.warn(`⚠️ CSS导出警告: ${stderr}`);
            }
            if (fs.existsSync(tempHtmlPath)) {
                // 从HTML中提取CSS
                const htmlContent = fs.readFileSync(tempHtmlPath, 'utf8');
                const cssContent = this.extractCSSFromHTML(htmlContent);
                // 写入CSS文件
                fs.writeFileSync(cssOutputPath, cssContent);
                // 清理临时文件
                fs.unlinkSync(tempHtmlPath);
                return {
                    success: true,
                    outputPath: cssOutputPath,
                    cssContent
                };
            }
            else {
                return {
                    success: false,
                    error: 'CSS导出完成但未生成输出文件'
                };
            }
        }
        catch (error) {
            console.error(`❌ CSS导出错误:`, error);
            return {
                success: false,
                error: error.message || String(error)
            };
        }
    }
    async exportJS(uri) {
        try {
            const filePath = uri.fsPath;
            const fileName = path.basename(filePath, path.extname(filePath));
            const outputDir = path.join(path.dirname(filePath), 'output');
            const jsOutputPath = path.join(outputDir, `${fileName}.js`);
            // 确保输出目录存在
            if (!fs.existsSync(outputDir)) {
                fs.mkdirSync(outputDir, { recursive: true });
            }
            // 执行JS导出命令（先编译到HTML，然后提取JS）
            const tempHtmlPath = path.join(outputDir, `${fileName}_temp.html`);
            const command = `"${this.compilerPath}" "${filePath}" "${tempHtmlPath}"`;
            console.log(`⚡ 执行JS导出命令: ${command}`);
            const { stdout, stderr } = await execAsync(command, {
                timeout: 30000
            });
            if (stderr && stderr.trim()) {
                console.warn(`⚠️ JS导出警告: ${stderr}`);
            }
            if (fs.existsSync(tempHtmlPath)) {
                // 从HTML中提取JavaScript
                const htmlContent = fs.readFileSync(tempHtmlPath, 'utf8');
                const jsContent = this.extractJSFromHTML(htmlContent);
                // 写入JS文件
                fs.writeFileSync(jsOutputPath, jsContent);
                // 清理临时文件
                fs.unlinkSync(tempHtmlPath);
                return {
                    success: true,
                    outputPath: jsOutputPath,
                    jsContent
                };
            }
            else {
                return {
                    success: false,
                    error: 'JavaScript导出完成但未生成输出文件'
                };
            }
        }
        catch (error) {
            console.error(`❌ JS导出错误:`, error);
            return {
                success: false,
                error: error.message || String(error)
            };
        }
    }
    // 获取编译器版本信息
    async getCompilerVersion() {
        try {
            const { stdout } = await execAsync(`"${this.compilerPath}" --version`);
            return stdout.trim();
        }
        catch (error) {
            return '未知版本';
        }
    }
    // 检查编译器是否可用
    async isCompilerAvailable() {
        try {
            await execAsync(`"${this.compilerPath}" --help`);
            return true;
        }
        catch (error) {
            return false;
        }
    }
    // 重新加载编译器配置
    reload() {
        this.initializeCompiler();
    }
    // 从HTML中提取CSS
    extractCSSFromHTML(htmlContent) {
        const styleRegex = /<style[^>]*>([\s\S]*?)<\/style>/gi;
        let cssContent = '';
        let match;
        while ((match = styleRegex.exec(htmlContent)) !== null) {
            cssContent += match[1] + '\n';
        }
        return cssContent.trim();
    }
    // 从HTML中提取JavaScript
    extractJSFromHTML(htmlContent) {
        const scriptRegex = /<script[^>]*>([\s\S]*?)<\/script>/gi;
        let jsContent = '';
        let match;
        while ((match = scriptRegex.exec(htmlContent)) !== null) {
            // 跳过src属性的script标签
            if (!match[0].includes('src=')) {
                jsContent += match[1] + '\n';
            }
        }
        return jsContent.trim();
    }
}
exports.CHTLCompilerManager = CHTLCompilerManager;
//# sourceMappingURL=CompilerManager.js.map