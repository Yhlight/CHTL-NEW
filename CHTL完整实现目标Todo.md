# CHTL编译器完整实现目标Todo

## 核心架构要求
- 使用C++17编程语言
- CHTL和CHTL JS完全分离，各自独立的文件体系
- 每个编译器拥有独立的Token、GlobalMap、State、Context、Lexer等组件
- 大驼峰命名法
- UTF-8支持
- 模块化开发，高质量高标准

## 第一部分：核心基础架构（8个主要Todo）

### Todo 1: CHTLUnifiedScanner精准代码切割器
- [ ] 实现可变长度切片机制
- [ ] 实现最小语法单元切割
- [ ] 支持CHTL片段、CHTL JS片段、CSS片段、JS片段识别
- [ ] 实现片段完整性检查和扩增机制
- [ ] 确保{{box}}->click切割为{{box}}->和click两个单元

### Todo 2: CHTL编译器核心组件
- [ ] CHTLToken：支持所有CHTL语法令牌
- [ ] CHTLGlobalMap：全局映射表管理
- [ ] CHTLState：状态机管理
- [ ] CHTLContext：上下文管理
- [ ] CHTLLexer：词法分析器

### Todo 3: CHTL JS编译器核心组件（完全独立）
- [ ] CHTLJSToken：支持所有CHTL JS语法令牌
- [ ] CHTLJSGlobalMap：独立的全局映射表
- [ ] CHTLJSState：独立的状态机
- [ ] CHTLJSContext：独立的上下文管理
- [ ] CHTLJSLexer：独立的词法分析器

### Todo 4: CHTL AST节点定义
- [ ] 元素节点（支持所有HTML元素）
- [ ] 文本节点（text{}）
- [ ] 属性节点（支持CE对等式：:和=等价）
- [ ] 局部样式块节点
- [ ] 模板节点（@Style、@Element、@Var）
- [ ] 自定义节点（[Custom]）
- [ ] 原始嵌入节点（[Origin]）
- [ ] 配置组节点（[Configuration]）
- [ ] 导入节点（[Import]）
- [ ] 命名空间节点（[Namespace]）
- [ ] 约束节点（except）

### Todo 5: CHTL JS AST节点定义（完全独立）
- [ ] 模块导入节点（module{}）
- [ ] 局部script节点
- [ ] 增强选择器节点（{{选择器}}）
- [ ] 事件监听器节点（listen）
- [ ] 事件委托节点（delegate）
- [ ] 动画节点（animate）
- [ ] 虚对象节点（vir）
- [ ] 事件绑定操作符节点（&->）
- [ ] util...then表达式节点

### Todo 6: CompilerDispatcher编译器调度器
- [ ] 协调四个编译器工作
- [ ] 片段分发机制
- [ ] 编译结果合并
- [ ] HTML输出生成

### Todo 7: CHTL解析器和生成器
- [ ] 支持所有CHTL语法特征的解析
- [ ] 模板继承和特例化
- [ ] 自动化类名/id生成
- [ ] 上下文推导（&引用）
- [ ] 局部样式块处理

### Todo 8: CHTL JS解析器和生成器（完全独立）
- [ ] 增强选择器解析
- [ ] 虚对象vir处理
- [ ] 事件委托全局注册表
- [ ] 动画封装requestAnimationFrame
- [ ] CJMOD API接口支持

## 第二部分：高级功能实现

### Todo 9: 模板系统完整实现
- [ ] 样式组模板（[Template] @Style）
- [ ] 元素模板（[Template] @Element）
- [ ] 变量组模板（[Template] @Var）
- [ ] 模板继承（组合式和显性inherit）
- [ ] 全缀名访问

### Todo 10: 自定义系统完整实现
- [ ] 自定义样式组（[Custom] @Style）
- [ ] 无值样式组
- [ ] 样式组特例化（delete属性、delete继承）
- [ ] 自定义元素（[Custom] @Element）
- [ ] 元素特例化（索引访问、插入、删除）
- [ ] 自定义变量组（[Custom] @Var）
- [ ] 变量组特例化

### Todo 11: 原始嵌入系统
- [ ] 基本类型（@Html、@Style、@JavaScript）
- [ ] 带名原始嵌入
- [ ] 自定义类型系统（@Vue等）
- [ ] 隐式配置块创建

### Todo 12: 配置组系统
- [ ] 基础配置项（INDEX_INITIAL_COUNT、DEBUG_MODE等）
- [ ] Name配置块（关键字自定义）
- [ ] OriginType配置块
- [ ] 命名配置组
- [ ] 选择器自动化配置

## 第三部分：导入和模块系统

### Todo 13: Import导入系统完整实现
- [ ] HTML/CSS/JS文件导入
- [ ] CHTL文件导入（模板、自定义、原始嵌入）
- [ ] CMOD模块导入
- [ ] CJMOD模块导入
- [ ] 配置组导入
- [ ] 路径搜索策略（官方模块目录、当前目录module、当前目录）
- [ ] 循环依赖检测
- [ ] 重复导入处理

### Todo 14: 命名空间系统
- [ ] 命名空间创建（[Namespace]）
- [ ] 嵌套命名空间
- [ ] 同名命名空间自动合并
- [ ] 命名空间冲突检测
- [ ] 默认文件名命名空间
- [ ] from语法支持

### Todo 15: use语法系统
- [ ] HTML5类型声明
- [ ] 命名配置组使用
- [ ] 文件开头位置验证
- [ ] 唯一性检查

## 第四部分：约束和验证系统

### Todo 16: 约束系统（except）
- [ ] 精确约束（HTML元素、自定义、模板对象）
- [ ] 类型约束（@Html、[Custom]、[Template]）
- [ ] 全局约束（命名空间内）
- [ ] 约束验证和错误报告

### Todo 17: 选择器自动化系统
- [ ] 局部样式块自动添加类/id选择器
- [ ] 局部脚本自动添加类/id选择器
- [ ] &引用选择器处理（style优先class，script优先id）
- [ ] 配置开关支持

## 第五部分：CHTL JS高级功能

### Todo 18: 增强选择器系统
- [ ] {{CSS选择器}}DOM对象创建
- [ ] 标签判断和类名/id查找
- [ ] 精确访问（[index]）
- [ ] 复合选择器支持
- [ ] 性能优化考虑

### Todo 19: 事件系统
- [ ] 增强监听器（listen）
- [ ] 事件委托（delegate）
- [ ] 全局注册表管理
- [ ] 事件绑定操作符（&->）
- [ ] 重复绑定优化

### Todo 20: 动画系统
- [ ] animate函数实现
- [ ] requestAnimationFrame封装
- [ ] 缓动函数支持
- [ ] 关键帧（when）处理
- [ ] 循环和回调支持

### Todo 21: 虚对象系统
- [ ] vir关键字处理
- [ ] View对象创建和管理
- [ ] 键值类型识别
- [ ] 函数引用和对象解析
- [ ] 结果缓存机制

### Todo 22: CHTL JS特殊语法
- [ ] 模块导入（module{}）
- [ ] AMD风格加载器
- [ ] ->操作符（等价于.）
- [ ] iNeverAway函数重载
- [ ] util...then表达式

## 第六部分：模块化和扩展

### Todo 23: CMOD系统
- [ ] 标准格式支持（src/info结构）
- [ ] 子模块支持
- [ ] 打包脚本实现
- [ ] 解包功能
- [ ] 信息文件处理（[Info]、[Export]）

### Todo 24: CJMOD系统
- [ ] C++扩展接口
- [ ] Syntax类实现
- [ ] Arg和AtomArg类
- [ ] CJMODScanner接口
- [ ] CJMODGenerator接口
- [ ] CHTLJSFunction支持

### Todo 25: 第三方集成
- [ ] ANTLR4集成（CSS和JS编译器）
- [ ] Zip库实现（手写或集成）
- [ ] 跨平台支持（Linux和Windows）

## 第七部分：字面量和注释系统

### Todo 26: 字面量系统
- [ ] 无修饰字面量支持
- [ ] 双引号字符串("")
- [ ] 单引号字符串('')
- [ ] CE对等式（:和=等价）

### Todo 27: 注释系统
- [ ] //注释（不被生成器识别）
- [ ] /**/多行注释（不被生成器识别）
- [ ] --生成器注释（根据上下文生成不同语言注释）

## 第八部分：测试和验证

### Todo 28: 测试系统
- [ ] 单元测试覆盖所有语法特征
- [ ] 集成测试验证编译流程
- [ ] 性能测试
- [ ] 错误处理测试
- [ ] UTF-8字符测试

### Todo 29: 文档和示例
- [ ] API文档生成
- [ ] 使用示例创建
- [ ] 错误消息本地化
- [ ] 开发者指南

### Todo 30: 质量保证
- [ ] 代码规范检查
- [ ] 内存泄漏检测
- [ ] 性能优化
- [ ] 跨平台兼容性测试
- [ ] 最终集成验证

## 语法特征完整清单

### CHTL核心语法特征
1. 注释：//、/**/、--
2. 文本节点：text{}
3. 字面量：无修饰、""、''
4. CE对等式：:和=等价
5. 元素节点：所有HTML元素
6. 属性：属性名:属性值;
7. 局部样式块：style{}
8. 内联样式
9. 自动化类名/id
10. 上下文推导：&引用
11. 模板：[Template] @Style/@Element/@Var
12. 模板继承：组合式和inherit
13. 自定义：[Custom] @Style/@Element/@Var
14. 无值样式组
15. 特例化：delete属性、delete继承
16. 索引访问：[index]
17. 插入元素：insert after/before/replace/at top/at bottom
18. 删除元素：delete
19. 变量组特例化
20. 全缀名访问
21. 原始嵌入：[Origin] @Html/@Style/@JavaScript
22. 带名原始嵌入
23. 自定义类型系统
24. 配置组：[Configuration]
25. Name配置块
26. OriginType配置块
27. 命名配置组
28. 导入：[Import]各种类型
29. use语法：html5、@Config
30. 命名空间：[Namespace]
31. 嵌套命名空间
32. 约束：except精确/类型/全局约束
33. 选择器自动化配置

### CHTL JS核心语法特征
1. 文件后缀：*.cjjs
2. 模块导入：module{}
3. AMD风格加载器
4. 局部script：script{}
5. 增强选择器：{{CSS选择器}}
6. 标签/类名/id查找优先级
7. 精确访问：{{button[0]}}
8. ->操作符（等价于.）
9. 增强监听器：listen{}
10. 事件委托：delegate{}
11. 全局注册表管理
12. 动画：animate{}
13. requestAnimationFrame封装
14. 虚对象：vir
15. View对象管理
16. 键值类型识别
17. 事件绑定操作符：&->
18. iNeverAway函数重载
19. 状态区分同名键
20. util...then表达式

### CJMOD API特征
1. Syntax类：analyze、isObject、isFunction、isArray、isCHTLJSFunction
2. Arg类：bind、fillValue、transform
3. AtomArg类：$、$?、$!、$_、...占位符
4. CJMODScanner：scan接口
5. CJMODGenerator：exportResult接口
6. CHTLJSFunction：CreateCHTLJSFunction、bindVirtualObject

### 模块系统特征
1. CMOD标准格式：src/info结构
2. 子模块支持
3. 信息文件：[Info]、[Export]
4. CJMOD结构：src/info C++文件
5. 混合模块：CMOD+CJMOD
6. 路径搜索策略
7. 官方模块前缀：chtl::

### 配置和约束特征
1. 索引起始计数配置
2. Name配置组禁用
3. 自定义原始嵌入类型配置
4. DEBUG模式
5. 选择器自动化配置
6. 默认命名空间配置
7. 约束验证机制

## 实现优先级
1. 核心架构（Todo 1-8）
2. 基础语法（Todo 9-16）
3. 高级功能（Todo 17-24）
4. 集成测试（Todo 25-30）