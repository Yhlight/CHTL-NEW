import * as vscode from 'vscode';

export class CHTLFormatterProvider implements vscode.DocumentFormattingEditProvider {
    private context: vscode.ExtensionContext;

    constructor(context: vscode.ExtensionContext) {
        this.context = context;
    }

    async provideDocumentFormattingEdits(
        document: vscode.TextDocument,
        options: vscode.FormattingOptions,
        token: vscode.CancellationToken
    ): Promise<vscode.TextEdit[]> {
        
        const config = vscode.workspace.getConfiguration('chtl');
        const formatStyle = config.get('formatStyle', 'cpp');

        const text = document.getText();
        let formattedText: string;

        if (document.languageId === 'chtl') {
            formattedText = this.formatCHTL(text, options, formatStyle);
        } else if (document.languageId === 'chtl-js') {
            formattedText = this.formatCHTLJS(text, options);
        } else {
            return [];
        }

        // 返回替换整个文档的编辑
        const fullRange = new vscode.Range(
            document.positionAt(0),
            document.positionAt(text.length)
        );

        return [vscode.TextEdit.replace(fullRange, formattedText)];
    }

    async formatDocument(document: vscode.TextDocument, editor: vscode.TextEditor): Promise<void> {
        const edits = await this.provideDocumentFormattingEdits(
            document,
            {
                tabSize: editor.options.tabSize as number || 4,
                insertSpaces: editor.options.insertSpaces as boolean || true
            },
            new vscode.CancellationTokenSource().token
        );

        if (edits.length > 0) {
            const workspaceEdit = new vscode.WorkspaceEdit();
            workspaceEdit.set(document.uri, edits);
            await vscode.workspace.applyEdit(workspaceEdit);
        }
    }

    private formatCHTL(text: string, options: vscode.FormattingOptions, style: string): string {
        const lines = text.split('\n');
        const formattedLines: string[] = [];
        let indentLevel = 0;
        let inStyleBlock = false;
        let inScriptBlock = false;

        const indent = options.insertSpaces ? ' '.repeat(options.tabSize) : '\t';

        for (let i = 0; i < lines.length; i++) {
            const line = lines[i];
            const trimmedLine = line.trim();

            if (trimmedLine === '') {
                formattedLines.push('');
                continue;
            }

            // 检测块的开始和结束
            const hasOpenBrace = trimmedLine.includes('{');
            const hasCloseBrace = trimmedLine.includes('}');

            // 检测是否在特殊块中
            if (trimmedLine.includes('style') && hasOpenBrace) {
                inStyleBlock = true;
            } else if (trimmedLine.includes('script') && hasOpenBrace) {
                inScriptBlock = true;
            }

            // 处理缩进
            if (hasCloseBrace && !hasOpenBrace) {
                indentLevel = Math.max(0, indentLevel - 1);
                if (trimmedLine === '}') {
                    inStyleBlock = false;
                    inScriptBlock = false;
                }
            }

            // 格式化当前行
            let formattedLine = '';
            if (inStyleBlock) {
                formattedLine = this.formatCSSLine(trimmedLine, indentLevel, indent);
            } else if (inScriptBlock) {
                formattedLine = this.formatJSLine(trimmedLine, indentLevel, indent);
            } else {
                formattedLine = this.formatCHTLLine(trimmedLine, indentLevel, indent, style);
            }

            formattedLines.push(formattedLine);

            // 更新缩进级别
            if (hasOpenBrace && !hasCloseBrace) {
                indentLevel++;
            }
        }

        return formattedLines.join('\n');
    }

    private formatCHTLJS(text: string, options: vscode.FormattingOptions): string {
        const lines = text.split('\n');
        const formattedLines: string[] = [];
        let indentLevel = 0;

        const indent = options.insertSpaces ? ' '.repeat(options.tabSize) : '\t';

        for (const line of lines) {
            const trimmedLine = line.trim();

            if (trimmedLine === '') {
                formattedLines.push('');
                continue;
            }

            // 处理大括号缩进
            const hasOpenBrace = trimmedLine.includes('{');
            const hasCloseBrace = trimmedLine.includes('}');

            if (hasCloseBrace && !hasOpenBrace) {
                indentLevel = Math.max(0, indentLevel - 1);
            }

            // 格式化JavaScript风格
            const formattedLine = this.formatJSLine(trimmedLine, indentLevel, indent);
            formattedLines.push(formattedLine);

            if (hasOpenBrace && !hasCloseBrace) {
                indentLevel++;
            }
        }

        return formattedLines.join('\n');
    }

    private formatCHTLLine(line: string, indentLevel: number, indent: string, style: string): string {
        if (style === 'cpp') {
            return this.formatCppStyle(line, indentLevel, indent);
        } else {
            return this.formatJSLine(line, indentLevel, indent);
        }
    }

    private formatCppStyle(line: string, indentLevel: number, indent: string): string {
        const indentation = indent.repeat(indentLevel);
        
        // C++风格格式化规则
        let formatted = line;

        // 处理大括号
        if (line === '{') {
            return indentation + '{';
        }
        if (line === '}') {
            return indentation + '}';
        }

        // 处理属性赋值
        if (line.includes(':') && !line.includes('//')) {
            const parts = line.split(':');
            if (parts.length === 2) {
                const property = parts[0].trim();
                const value = parts[1].trim();
                formatted = `${property}: ${value}`;
            }
        }

        // 处理特殊语法块
        if (line.startsWith('[') && line.endsWith(']')) {
            formatted = line;
        }

        return indentation + formatted;
    }

    private formatCSSLine(line: string, indentLevel: number, indent: string): string {
        const indentation = indent.repeat(indentLevel);
        
        // CSS格式化规则
        let formatted = line;

        // 处理选择器
        if (line.includes('{') && !line.includes('}')) {
            const selector = line.replace('{', '').trim();
            return indentation + selector + ' {';
        }

        // 处理属性
        if (line.includes(':') && line.endsWith(';')) {
            const parts = line.split(':');
            if (parts.length === 2) {
                const property = parts[0].trim();
                const value = parts[1].replace(';', '').trim();
                formatted = `${property}: ${value};`;
            }
        }

        // 处理大括号
        if (line === '{' || line === '}') {
            return indentation + line;
        }

        return indentation + formatted;
    }

    private formatJSLine(line: string, indentLevel: number, indent: string): string {
        const indentation = indent.repeat(indentLevel);
        
        // JavaScript格式化规则
        let formatted = line;

        // 处理函数声明
        if (line.includes('function') || line.includes('=>')) {
            // 保持函数声明格式
            formatted = line;
        }

        // 处理对象字面量
        if (line.includes(':') && !line.includes('//') && !line.includes('http')) {
            const parts = line.split(':');
            if (parts.length === 2) {
                const key = parts[0].trim();
                const value = parts[1].trim();
                formatted = `${key}: ${value}`;
            }
        }

        // 处理大括号
        if (line === '{' || line === '}') {
            return indentation + line;
        }

        // 处理CHTL JS特殊语法
        if (line.includes('{{') && line.includes('}}')) {
            // 保持选择器引用格式
            formatted = line;
        }

        if (line.includes('->')) {
            // 保持箭头操作符格式
            formatted = line;
        }

        return indentation + formatted;
    }

    // 解决VSCode自动补齐[]冲突的方法
    static configureBracketPairs(): void {
        // 这个方法将在插件激活时调用，配置CHTL特定的括号行为
        const config = vscode.workspace.getConfiguration();
        
        // 获取当前的括号配置
        const currentBrackets = config.get('editor.autoClosingBrackets', 'languageDefined');
        
        // 为CHTL语言禁用自动括号补齐
        config.update(
            '[chtl]',
            {
                'editor.autoClosingBrackets': 'never',
                'editor.autoClosingQuotes': 'never',
                'editor.autoSurround': 'never'
            },
            vscode.ConfigurationTarget.Global
        );

        config.update(
            '[chtl-js]',
            {
                'editor.autoClosingBrackets': 'languageDefined',
                'editor.autoClosingQuotes': 'languageDefined',
                'editor.autoSurround': 'languageDefined'
            },
            vscode.ConfigurationTarget.Global
        );

        console.log('🔧 已配置CHTL括号行为');
    }
}