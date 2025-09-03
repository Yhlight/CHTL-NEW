# CHTL JS开发指南

## 🌟 CHTL JS简介

**CHTL JS** 是CHTL项目中的脚本语言部分，采用**双语言分离架构**设计，与CHTL完全独立。CHTL JS不是JavaScript的超集，而是一种专门为CHTL设计的增强脚本语言，最终编译为JavaScript。

### 🎯 设计理念

- **完全分离** - 与CHTL语言完全独立的编译体系
- **增强功能** - 提供比传统JavaScript更强大的Web开发功能
- **优雅语法** - 更直观、更易用的脚本编写方式
- **性能优化** - 编译时优化，运行时高效

---

## 🏗️ 核心架构

### 🔧 独立编译体系

CHTL JS拥有完全独立的编译组件：
- **CHTLJSToken** - 专用词法单元
- **CHTLJSGlobalMap** - 独立全局映射
- **CHTLJSState** - 专用状态机
- **CHTLJSContext** - 独立上下文管理
- **CHTLJSLexer** - 专用词法分析器
- **CHTLJSParser** - 专用语法解析器
- **CHTLJSGenerator** - 专用代码生成器

### 📁 文件扩展名

```
.cjjs - CHTL JS源文件扩展名
```

---

## 📝 基础语法

### 🎵 模块系统 (AMD风格)

```chtljs
// module{}块用于AMD风格模块加载
module {
    name: "MyModule",
    version: "1.0.0",
    dependencies: ["chtl::Chtholly", "jquery"],
    exports: {
        myFunction: myFunction,
        MyClass: MyClass
    }
}
```

### 📜 局部脚本块

```chtljs
// script{}块属于CHTL，由CHTL编译器处理
script {
    console.log('这是局部脚本');
    // 这部分代码由CHTL编译器处理，不是CHTL JS
}
```

**注意**: `script{}`块虽然在CHTL JS文档中提到，但**严格上属于CHTL**，由CHTL编译器处理。

---

## 🎯 增强选择器

### 🔍 CSS选择器增强

```chtljs
// 使用{{}}包围CSS选择器进行增强
{{.button[data-active="true"]}} -> textContent -> "激活状态";

// 索引访问
{{.item}}[0] -> style.color -> "red";
{{.item}}[1] -> style.color -> "blue";

// 智能推导
{{.container .item}} -> forEach(item => {
    item.style.transition = "all 0.3s ease";
});
```

### 🎨 选择器自动化

```chtljs
// 自动推导上下文
{{.current-page .navigation}} -> addClass("active");

// 智能索引
{{.gallery img}}[currentIndex] -> src -> newImageUrl;
```

---

## ⚡ 操作符系统

### 🔗 -> 操作符

```chtljs
// 属性设置
{{.title}} -> textContent -> "新标题";
{{.image}} -> src -> "new-image.jpg";

// 样式设置
{{.element}} -> style.background -> "linear-gradient(45deg, red, blue)";

// 方法调用
{{.animation}} -> classList.add("fadeIn");
```

### 🎮 &-> 事件绑定操作符

```chtljs
// 事件绑定
{{.button}} &-> 'click' &-> function(event) {
    console.log('按钮被点击');
    event.target.style.background = 'green';
};

// 多事件绑定
{{.input}} &-> 'focus' &-> handleFocus
           &-> 'blur' &-> handleBlur
           &-> 'change' &-> handleChange;
```

---

## 🎧 增强监听器

### 📡 listen功能

```chtljs
// 基础监听
listen {
    selector: ".button",
    event: "click",
    callback: function(event) {
        console.log('监听器触发');
    }
}

// 高级配置
listen {
    selector: ".dynamic-element",
    event: "custom:chtl-event",
    once: true,
    passive: true,
    capture: false,
    callback: handleCustomEvent
}

// 条件监听
listen {
    selector: ".conditional",
    event: "click",
    condition: function(element) {
        return element.dataset.enabled === "true";
    },
    callback: conditionalHandler
}
```

### 🎭 delegate事件委托

```chtljs
// 基础委托
delegate {
    parent: ".container",
    target: ".dynamic-button",
    event: "click",
    callback: function(event) {
        console.log('委托事件触发');
    }
}

// 高级委托
delegate {
    parent: document.body,
    target: "[data-action]",
    event: "click",
    filter: function(target) {
        return target.dataset.action !== "disabled";
    },
    callback: function(event, target) {
        const action = target.dataset.action;
        executeAction(action, target);
    }
}
```

---

## 🎬 动画系统

### ✨ animate功能

```chtljs
// 基础动画
animate {
    target: ".element",
    keyframes: {
        "0%": { opacity: 0, transform: "translateY(-20px)" },
        "100%": { opacity: 1, transform: "translateY(0)" }
    },
    duration: 500,
    easing: "ease-out"
}

// 高级动画配置
animate {
    target: ".complex-animation",
    keyframes: [
        { offset: 0, opacity: 0, transform: "scale(0.8)" },
        { offset: 0.5, opacity: 0.7, transform: "scale(1.1)" },
        { offset: 1, opacity: 1, transform: "scale(1)" }
    ],
    duration: 1000,
    easing: "cubic-bezier(0.25, 0.46, 0.45, 0.94)",
    delay: 200,
    iterations: 1,
    direction: "normal",
    fill: "forwards",
    callback: function() {
        console.log('动画完成');
    }
}

// 链式动画
animate {
    target: ".first",
    keyframes: { /* ... */ },
    duration: 500
} -> then(animate {
    target: ".second", 
    keyframes: { /* ... */ },
    duration: 300
});
```

---

## 🌟 虚拟对象系统

### 👻 vir虚对象

```chtljs
// 创建虚拟对象
vir MyHandler = listen {
    selector: ".interactive",
    event: "click",
    callback: function(event) {
        this.handleClick(event);
    },
    
    methods: {
        handleClick: function(event) {
            console.log('虚拟对象方法调用');
        },
        
        initialize: function() {
            console.log('虚拟对象初始化');
        }
    }
};

// 使用虚拟对象
MyHandler.initialize();

// 虚拟对象继承
vir ExtendedHandler = MyHandler {
    extend: true,
    methods: {
        handleClick: function(event) {
            super.handleClick(event);
            console.log('扩展处理');
        }
    }
};
```

---

## 🔍 键类型分析

### 🔑 类型检测

```chtljs
// 自动类型分析
let data = {
    string: "text",
    number: 42,
    boolean: true,
    array: [1, 2, 3],
    object: { key: "value" },
    function: function() { return "result"; }
};

// CHTL JS会自动分析键类型
analyzeTypes(data) -> {
    string: "string",
    number: "number", 
    boolean: "boolean",
    array: "array",
    object: "object",
    function: "function"
};
```

### 🗺️ 函数引用映射

```chtljs
// 函数引用映射
const functionMap = {
    "handleClick": handleClick,
    "handleSubmit": handleSubmit,
    "handleChange": handleChange
};

// 动态函数调用
executeFunction("handleClick", event) -> functionMap["handleClick"](event);
```

---

## 🚀 异步操作

### ⏰ Promise增强

```chtljs
// 增强的Promise语法
async function loadData() {
    const result = await fetch("/api/data") 
                  -> json() 
                  -> processData()
                  -> validateData();
    
    return result;
}

// 并行处理
parallel {
    task1: loadUserData(),
    task2: loadConfigData(),
    task3: loadModuleData()
} -> then(results => {
    console.log('所有任务完成:', results);
});
```

### 🔄 异步链式

```chtljs
// 异步链式操作
chain()
    .step(loadInitialData)
    .step(processData)
    .step(validateResults)
    .step(updateUI)
    .catch(handleError)
    .finally(cleanup);
```

---

## 🎮 事件系统

### 📢 自定义事件

```chtljs
// 创建自定义事件
createEvent {
    name: "chtl:dataLoaded",
    detail: { timestamp: Date.now(), source: "api" },
    bubbles: true,
    cancelable: true
} -> dispatchOn(document);

// 监听自定义事件
listen {
    selector: document,
    event: "chtl:dataLoaded",
    callback: function(event) {
        console.log('数据加载事件:', event.detail);
    }
}
```

### 🎯 事件流控制

```chtljs
// 事件流控制
eventFlow {
    capture: listen {
        selector: ".parent",
        event: "click",
        phase: "capture",
        callback: captureHandler
    },
    
    target: listen {
        selector: ".target",
        event: "click", 
        phase: "target",
        callback: targetHandler
    },
    
    bubble: listen {
        selector: ".parent",
        event: "click",
        phase: "bubble", 
        callback: bubbleHandler
    }
}
```

---

## 🔧 DOM操作增强

### 🎨 样式操作

```chtljs
// 批量样式操作
{{.elements}} -> applyStyles({
    background: "linear-gradient(45deg, red, blue)",
    transform: "translateY(0)",
    opacity: 1,
    transition: "all 0.3s ease"
});

// 条件样式
{{.conditional}} -> when(element => element.dataset.active === "true")
                 -> applyStyles({ background: "green" })
                 -> else()
                 -> applyStyles({ background: "gray" });
```

### 🏗️ 元素操作

```chtljs
// 元素创建和操作
createElement {
    tag: "div",
    className: "dynamic-element",
    attributes: {
        "data-id": generateId(),
        "data-created": Date.now()
    },
    styles: {
        background: "white",
        padding: "20px"
    },
    children: [
        createElement {
            tag: "h3",
            textContent: "动态标题"
        },
        createElement {
            tag: "p", 
            textContent: "动态内容"
        }
    ]
} -> appendTo(".container");
```

---

## 📦 模块导入导出

### 📥 导入模块

```chtljs
// 导入CHTL JS模块
import { MyFunction, MyClass } from "./myModule.cjjs";

// 导入官方模块
import { printMylove, iNeverAway } from "chtl::Chtholly";

// 条件导入
import("./conditionalModule.cjjs")
    .then(module => {
        module.initialize();
    });
```

### 📤 导出功能

```chtljs
// 导出函数
export function myUtility(param) {
    return processParam(param);
}

// 导出类
export class MyComponent {
    constructor(config) {
        this.config = config;
    }
    
    render() {
        return this.generateHTML();
    }
}

// 默认导出
export default {
    version: "1.0.0",
    name: "MyModule",
    utilities: { myUtility },
    components: { MyComponent }
};
```

---

## 🎭 高级特征

### 🎪 函数重载

```chtljs
// 基于参数类型的函数重载
function process(data) {
    switch(typeof data) {
        case "string":
            return processString(data);
        case "number":
            return processNumber(data);
        case "object":
            return processObject(data);
        default:
            return processDefault(data);
    }
}

// 参数个数重载
function create(...args) {
    if (args.length === 1) {
        return createSingle(args[0]);
    } else if (args.length === 2) {
        return createPair(args[0], args[1]);
    } else {
        return createMultiple(args);
    }
}
```

### 🔮 装饰器模式

```chtljs
// 函数装饰器
@memoize
@validate
function expensiveFunction(param) {
    // 昂贵的计算
    return complexCalculation(param);
}

// 类装饰器
@component
@singleton
class MyService {
    @inject("dataService")
    private dataService;
    
    @async
    @retry(3)
    async fetchData() {
        return await this.dataService.getData();
    }
}
```

### 🌊 响应式编程

```chtljs
// 响应式数据
reactive {
    data: {
        count: 0,
        message: "Hello"
    },
    
    computed: {
        displayMessage() {
            return `${this.message} (${this.count})`;
        }
    },
    
    watch: {
        count(newValue, oldValue) {
            console.log(`计数从 ${oldValue} 变为 ${newValue}`);
        }
    },
    
    methods: {
        increment() {
            this.count++;
        }
    }
}
```

---

## 🎨 实际应用示例

### 🌸 珂朵莉主题应用

```chtljs
// 使用珂朵莉模块功能
import { printMylove, iNeverAway } from "chtl::Chtholly";

// 爱的表达功能
vir LoveExpression = printMylove {
    target: "世界",
    message: "珂朵莉的爱意",
    style: "romantic",
    animation: "heartbeat"
};

// 永恒守护功能
vir EternalGuardian = iNeverAway {
    Void<Promise>: function(target, duration) {
        console.log(`💝 永恒的承诺: 我将永远守护${target}`);
        return new Promise(resolve => {
            setTimeout(() => {
                console.log(`⭐ 守护时间: ${duration}ms 完成`);
                resolve(`守护${target}的承诺已实现`);
            }, duration);
        });
    },
    
    Void<Protect>: function(target, threat) {
        console.log(`🛡️ 保护${target}免受${threat}的伤害`);
        return {
            status: "protected",
            target: target,
            threat: threat,
            guardian: "珂朵莉"
        };
    }
};

// 异步链式操作
util.fadeIn({{.chtholly-card}})
    .then(util.slideDown())
    .then(util.addSparkles())
    .then(() => {
        console.log('✨ 珂朵莉特效序列完成');
    });

// 综合应用
listen {
    selector: {{.chtholly-button[data-action="love"]}},
    event: "click",
    callback: function(event) {
        const target = event.target.dataset.target || "你";
        
        // 使用printMylove功能
        LoveExpression.express(target).then(result => {
            console.log('💕 爱的表达结果:', result);
            
            // 触发永恒守护
            return EternalGuardian.Promise(target, 2000);
        }).then(guardResult => {
            console.log('🌟 守护完成:', guardResult);
            
            // 显示特效
            event.target -> style.background -> 'linear-gradient(45deg, #ff6b6b, #ffd93d)';
            event.target -> textContent -> '💝 爱已传达';
        });
    }
}
```

### 🎀 由比滨结衣应用

```chtljs
// 使用由比滨结衣模块
import { FourLeafAlbum, Memo } from "chtl::Yuigahama";

// 相册组件
vir PhotoGallery = FourLeafAlbum {
    images: [
        "spring-sakura.jpg",
        "summer-beach.jpg", 
        "autumn-leaves.jpg",
        "winter-snow.jpg"
    ],
    layout: "clover",
    transition: "fade",
    autoplay: true,
    interval: 3000
};

// 备忘录系统
vir PersonalMemo = Memo {
    entries: [],
    
    add: function(content, mood) {
        const entry = {
            id: generateId(),
            content: content,
            mood: mood,
            timestamp: new Date(),
            tags: extractTags(content)
        };
        
        this.entries.push(entry);
        this.save();
        return entry;
    },
    
    filter: function(criteria) {
        return this.entries.filter(entry => 
            criteria.mood ? entry.mood === criteria.mood : true &&
            criteria.tags ? criteria.tags.some(tag => entry.tags.includes(tag)) : true
        );
    }
};

// 交互系统
delegate {
    parent: ".yuigahama-container",
    target: ".memo-item",
    event: "click",
    callback: function(event, target) {
        const memoId = target.dataset.memoId;
        const memo = PersonalMemo.findById(memoId);
        
        if (memo) {
            displayMemoDetail(memo);
        }
    }
}
```

---

## 🔧 调试和优化

### 🐛 调试技巧

```chtljs
// 调试模式
debug {
    enabled: true,
    level: "verbose",
    output: "console"
}

// 性能监控
performance {
    monitor: [
        "listen.callback.duration",
        "animate.frame.time",
        "delegate.lookup.time"
    ],
    
    threshold: {
        warning: 16, // 16ms (60fps)
        error: 33    // 33ms (30fps)
    },
    
    callback: function(metrics) {
        if (metrics.duration > this.threshold.error) {
            console.warn('性能警告:', metrics);
        }
    }
}
```

### ⚡ 性能优化

```chtljs
// 批量DOM操作
batch {
    operations: [
        () => {{.item1}} -> style.color -> "red",
        () => {{.item2}} -> style.color -> "blue", 
        () => {{.item3}} -> style.color -> "green"
    ],
    
    // 使用requestAnimationFrame批量执行
    strategy: "raf"
}

// 虚拟滚动
virtualScroll {
    container: ".large-list",
    itemHeight: 50,
    buffer: 10,
    renderItem: function(data, index) {
        return createElement {
            tag: "div",
            className: "list-item",
            textContent: data.title
        };
    }
}
```

---

## 📚 最佳实践

### 🎯 代码组织

```chtljs
// 推荐的文件结构
// main.cjjs - 主入口文件
module {
    name: "MyApp",
    dependencies: ["chtl::Chtholly"],
    
    initialize: function() {
        this.setupEventListeners();
        this.loadModules();
        this.startApplication();
    },
    
    setupEventListeners: function() {
        // 设置全局事件监听
    },
    
    loadModules: function() {
        // 加载必要模块
    }
}

// components/ - 组件文件
// utils/ - 工具函数
// services/ - 服务类
```

### 🔒 错误处理

```chtljs
// 全局错误处理
errorHandler {
    global: true,
    
    catch: function(error, context) {
        console.error('CHTL JS错误:', error);
        
        // 发送错误报告
        reportError(error, context);
        
        // 用户友好的错误提示
        showUserError("操作失败，请稍后重试");
    },
    
    types: {
        "TypeError": handleTypeError,
        "ReferenceError": handleReferenceError,
        "SyntaxError": handleSyntaxError
    }
}
```

### 🎪 性能最佳实践

```chtljs
// 1. 使用事件委托而不是大量监听器
delegate {
    parent: ".app",
    target: "[data-action]",
    event: "click",
    callback: handleAction
}

// 2. 批量DOM操作
batch {
    operations: domOperations,
    strategy: "raf"
}

// 3. 虚拟对象复用
vir ReusableComponent = {
    pool: [],
    
    acquire: function() {
        return this.pool.pop() || this.create();
    },
    
    release: function(instance) {
        instance.reset();
        this.pool.push(instance);
    }
}
```

---

## 🎊 总结

CHTL JS作为CHTL项目的脚本语言部分，提供了：

- 🔥 **完全独立的编译体系** - 与CHTL完全分离
- 🔥 **增强的选择器系统** - 更强大的DOM操作
- 🔥 **丰富的操作符** - 简化常见操作
- 🔥 **强大的事件系统** - 监听器和委托
- 🔥 **完整的动画支持** - 声明式动画
- 🔥 **虚拟对象系统** - 面向对象增强
- 🔥 **异步操作支持** - 现代异步编程
- 🔥 **模块化设计** - AMD风格模块系统

通过CHTL JS，开发者可以用更优雅、更直观的方式编写Web应用的交互逻辑，同时享受现代JavaScript的所有优势。

**🌟 开始您的CHTL JS开发之旅，体验增强的脚本编程体验！**