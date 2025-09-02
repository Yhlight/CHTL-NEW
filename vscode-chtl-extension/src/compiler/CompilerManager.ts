import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';
import { exec, spawn } from 'child_process';
import { promisify } from 'util';

const execAsync = promisify(exec);

export interface CompileResult {
    success: boolean;
    outputPath?: string;
    error?: string;
    htmlContent?: string;
    cssContent?: string;
    jsContent?: string;
}

export class CHTLCompilerManager {
    private context: vscode.ExtensionContext;
    private compilerPath: string = '';
    private builtinCompilerPath: string = '';

    constructor(context: vscode.ExtensionContext) {
        this.context = context;
        this.initializeCompiler();
    }

    private initializeCompiler() {
        // 获取配置的编译器路径
        const config = vscode.workspace.getConfiguration('chtl');
        this.compilerPath = config.get('compilerPath', '');

        // 设置内置编译器路径
        this.builtinCompilerPath = path.join(this.context.extensionPath, 'bin', 'chtl-compiler');
        
        // 如果没有配置外部编译器，使用内置编译器
        if (!this.compilerPath) {
            this.compilerPath = this.builtinCompilerPath;
        }

        console.log(`📦 CHTL编译器路径: ${this.compilerPath}`);
    }

    async compile(uri: vscode.Uri): Promise<CompileResult> {
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

            // 执行编译命令
            const command = `"${this.compilerPath}" "${filePath}" -o "${outputPath}"`;
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
            } else {
                return {
                    success: false,
                    error: '编译完成但未生成输出文件'
                };
            }

        } catch (error: any) {
            console.error(`❌ 编译错误:`, error);
            return {
                success: false,
                error: error.message || String(error)
            };
        }
    }

    async exportHTML(uri: vscode.Uri): Promise<CompileResult> {
        const result = await this.compile(uri);
        if (result.success && result.outputPath) {
            // HTML已经在compile方法中生成
            return result;
        }
        return result;
    }

    async exportCSS(uri: vscode.Uri): Promise<CompileResult> {
        try {
            const filePath = uri.fsPath;
            const fileName = path.basename(filePath, path.extname(filePath));
            const outputDir = path.join(path.dirname(filePath), 'output');
            const cssOutputPath = path.join(outputDir, `${fileName}.css`);

            // 确保输出目录存在
            if (!fs.existsSync(outputDir)) {
                fs.mkdirSync(outputDir, { recursive: true });
            }

            // 执行CSS导出命令
            const command = `"${this.compilerPath}" "${filePath}" --export-css -o "${cssOutputPath}"`;
            console.log(`🎨 执行CSS导出命令: ${command}`);

            const { stdout, stderr } = await execAsync(command, {
                timeout: 30000
            });

            if (stderr && stderr.trim()) {
                console.warn(`⚠️ CSS导出警告: ${stderr}`);
            }

            if (fs.existsSync(cssOutputPath)) {
                const cssContent = fs.readFileSync(cssOutputPath, 'utf8');
                return {
                    success: true,
                    outputPath: cssOutputPath,
                    cssContent
                };
            } else {
                return {
                    success: false,
                    error: 'CSS导出完成但未生成输出文件'
                };
            }

        } catch (error: any) {
            console.error(`❌ CSS导出错误:`, error);
            return {
                success: false,
                error: error.message || String(error)
            };
        }
    }

    async exportJS(uri: vscode.Uri): Promise<CompileResult> {
        try {
            const filePath = uri.fsPath;
            const fileName = path.basename(filePath, path.extname(filePath));
            const outputDir = path.join(path.dirname(filePath), 'output');
            const jsOutputPath = path.join(outputDir, `${fileName}.js`);

            // 确保输出目录存在
            if (!fs.existsSync(outputDir)) {
                fs.mkdirSync(outputDir, { recursive: true });
            }

            // 执行JS导出命令
            const command = `"${this.compilerPath}" "${filePath}" --export-js -o "${jsOutputPath}"`;
            console.log(`⚡ 执行JS导出命令: ${command}`);

            const { stdout, stderr } = await execAsync(command, {
                timeout: 30000
            });

            if (stderr && stderr.trim()) {
                console.warn(`⚠️ JS导出警告: ${stderr}`);
            }

            if (fs.existsSync(jsOutputPath)) {
                const jsContent = fs.readFileSync(jsOutputPath, 'utf8');
                return {
                    success: true,
                    outputPath: jsOutputPath,
                    jsContent
                };
            } else {
                return {
                    success: false,
                    error: 'JavaScript导出完成但未生成输出文件'
                };
            }

        } catch (error: any) {
            console.error(`❌ JS导出错误:`, error);
            return {
                success: false,
                error: error.message || String(error)
            };
        }
    }

    // 获取编译器版本信息
    async getCompilerVersion(): Promise<string> {
        try {
            const { stdout } = await execAsync(`"${this.compilerPath}" --version`);
            return stdout.trim();
        } catch (error) {
            return '未知版本';
        }
    }

    // 检查编译器是否可用
    async isCompilerAvailable(): Promise<boolean> {
        try {
            await execAsync(`"${this.compilerPath}" --help`);
            return true;
        } catch (error) {
            return false;
        }
    }

    // 重新加载编译器配置
    reload() {
        this.initializeCompiler();
    }
}