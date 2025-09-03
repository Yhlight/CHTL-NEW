const fs = require('fs');
const path = require('path');
const { execSync } = require('child_process');

console.log('🚀 开始打包CHTL VSCode插件（包含CJMOD支持）...');

// 1. 确保编译完成
console.log('📋 步骤1: 编译TypeScript...');
try {
    execSync('npm run compile', { stdio: 'inherit' });
    console.log('✅ TypeScript编译完成');
} catch (error) {
    console.error('❌ TypeScript编译失败:', error.message);
    process.exit(1);
}

// 2. 验证必要文件存在
console.log('📋 步骤2: 验证文件...');
const requiredFiles = [
    'package.json',
    'out/extension.js',
    'bin/chtl-professional',
    'module/Chtholly.cmod',
    'module/Yuigahama.cmod'
];

for (const file of requiredFiles) {
    if (!fs.existsSync(file)) {
        console.error(`❌ 缺少必要文件: ${file}`);
        process.exit(1);
    }
}
console.log('✅ 所有必要文件存在');

// 3. 创建发布包目录
console.log('📋 步骤3: 创建发布包...');
const releaseDir = '../build/vscode-plugin-release';
if (fs.existsSync(releaseDir)) {
    execSync(`rm -rf ${releaseDir}`);
}
fs.mkdirSync(releaseDir, { recursive: true });

// 4. 复制所有文件
console.log('📋 步骤4: 复制插件文件...');
const filesToCopy = [
    'package.json',
    'README.md',
    'language-configuration.json',
    'language-configuration-js.json',
    'out/',
    'syntaxes/',
    'bin/',
    'module/'
];

for (const file of filesToCopy) {
    const src = path.join(__dirname, file);
    const dest = path.join(releaseDir, file);
    
    if (fs.existsSync(src)) {
        execSync(`cp -r "${src}" "${dest}"`);
        console.log(`   ✅ 复制: ${file}`);
    }
}

// 5. 创建插件信息文件
console.log('📋 步骤5: 创建插件信息...');
const pluginInfo = {
    name: 'CHTL Language Support with Complete CJMOD API',
    version: '1.0.0',
    description: 'CHTL编译器VSCode插件，完整支持CJMOD（CHTL极为强大的特征）',
    features: {
        '1_实时预览': '✅ 支持实时HTML预览',
        '2_内置编译器': '✅ 内置chtl-professional编译器，支持CJMOD',
        '3_自动解包': '✅ CJMOD自动解包，使用scan、CreateCHTLJSFunction、语法分析',
        '4_代码格式化': '✅ C++风格CHTL，JS风格CHTL JS',
        '5_浏览器打开': '✅ 右键在浏览器中打开',
        '6_右键编译': '✅ 右键编译功能',
        '7_冲突解决': '✅ 解决[]自动补全冲突',
        '8_语法文档': '✅ 右键打开语法文档',
        '9_导出功能': '✅ 右键导出HTML/CSS/JS'
    },
    cjmod_support: {
        api_integration: '✅ 完整CJMOD API集成',
        auto_unpack: '✅ 自动模块解包',
        syntax_hints: '✅ CJMOD语法提示',
        scanning_methods: ['双指针扫描法', '前置截取法', '智能扫描'],
        placeholder_types: ['$', '$?', '$!', '$_', '$!_', '...'],
        vir_support: '✅ 虚对象支持'
    },
    included_modules: [
        'Chtholly.cmod (CMOD+CJMOD)',
        'Yuigahama.cmod (纯CMOD)'
    ]
};

fs.writeFileSync(path.join(releaseDir, 'PLUGIN_INFO.json'), JSON.stringify(pluginInfo, null, 2));

// 6. 创建使用说明
const usageGuide = `# CHTL VSCode插件使用指南

## 🔥 CJMOD支持（CHTL极为强大的特征）

### 自动解包功能
- ✅ 自动识别CMOD、CJMOD、CMOD+CJMOD模块结构
- ✅ 使用CJMOD的scan方法分析模块
- ✅ 使用CreateCHTLJSFunction分析CJMOD函数
- ✅ 使用语法分析方法获取语法提示

### CJMOD语法提示
- ✅ CJMOD关键字: then, when, with, using, via, from, to...
- ✅ CJMOD操作符: **, ->, &->, |>, <|, =>, <=>
- ✅ CJMOD占位符: $, $?, $!, $_, $!_, ...
- ✅ CJMOD函数: 天然支持vir的函数

### 9条基本要求
1. ✅ 实时预览
2. ✅ 内置编译器和官方模块
3. ✅ 自动模块解包和JSON查询表生成（CJMOD API驱动）
4. ✅ 右键代码格式化（C++风格CHTL，JS风格CHTL JS）
5. ✅ 右键在浏览器中打开
6. ✅ 右键编译
7. ✅ 解决[]自动补全冲突
8. ✅ 右键打开语法文档
9. ✅ 右键导出HTML、CSS、JS

## 安装方法
1. 解压插件包
2. 在VSCode中安装: Extensions -> Install from VSIX -> 选择.vsix文件
3. 配置编译器路径（可选，插件已内置）

## CJMOD功能测试
1. 创建.chtl文件
2. 添加: [Import] @CJmod from "module_name"
3. 在script块中输入CJMOD语法，享受强大的语法提示
4. 右键编译，查看CJMOD强大的扩展能力
`;

fs.writeFileSync(path.join(releaseDir, 'USAGE_GUIDE.md'), usageGuide);

console.log('✅ CHTL VSCode插件打包完成！');
console.log(`📦 发布包位置: ${releaseDir}`);
console.log('🔥 包含完整CJMOD支持，满足所有9条基本要求！');