# CHTL JS开发指南（严格按文档）

## 📝 CHTL JS概述

**重要说明**: 局部script严格上属于CHTL，由CHTL编译器处理，不是独立的CHTL JS语言。

---

## 📜 局部script语法

### 🔧 基本语法

CHTL允许在元素中使用script{}来编写JS代码。局部script会被添加到一个不会全局污染，具有高优先级的全局script块之中。

```chtl
div
{
    style
    {
        .box
        {
            width: 100px;
            height: 100px;
            background-color: red;
        }
    }

    script
    {
        {{box}}.addEventListener('click', () => {
            console.log('Box clicked!');
        });

        // 引用功能
        {{&}}->addEventListener('click', () => {
            console.log('Box clicked!');
        });
    }
}
```

---

## 🎯 增强选择器

### 🔍 选择器语法

你可以使用{{CSS选择器}}来创建一个DOM对象。

```chtl
button
{
    style
    {
        .box
        {
            
        }
    }
}

script
{
    {{box}}.textContent()  // 先判断是否为tag，然后查找类名 / id = box(id优先)的元素，并创建DOM对象
    {{.box}}  // 查找类名为box的元素，并创建DOM对象
    {{#box}}  // 查找id为box的元素，并创建DOM对象
    {{button}}  // 所有的button元素
    {{.box button}}  // 查找类名为box的元素的所有的button后代，并创建DOM对象  
    
    // 精确访问
    {{button[0]}}  // 第一个button元素
}
```

**重要限制**: 增强选择器仅支持上述的种类，这是出于性能与复杂性之间的考虑。.boxbutton这种交集选择器会消耗很多性能，因此这里不得不放弃支持。

---

## ⚡ -> 操作符

### 🔗 明确使用CHTL语法

使用到CHTL JS语法时，推荐使用->代替.，以便明确使用了CHTL JS语法。->与.是完全等价的，因此你可以直接使用->进行链式访问。

```chtl
button
{
    style
    {
        .box
        {
            
        }
    }
}

script
{
    {{box}}->textContent();
}
```

---

## 🎧 增强监听器

### 📡 listen语法

你现在可以使用listen来快捷绑定事件监听器。

```chtl
button
{
    style
    {
        .box
        {

        }
    }
}

script
{
    // 声明式
    {{box}}->listen {
        click: () => {

        },

        mouseenter: mouseEnterEvent,  // 已经存在的函数

        mousemove: function() {

        }
    };
}
```

---

## 🎭 事件委托

### 🔄 delegate语法

为了解决SPA页面中元素动态更新导致事件监听丢失的问题，提供了基于事件委托的增强语法。通过将事件绑定到不会销毁的父元素，监听冒泡阶段的事件，从而实现稳定的事件绑定。

```chtl
script
{
    {{父元素选择器}}->delegate {
        target: {{选择器}} | [{{选择器1}}, {{选择器2}},...], // 要代理的子元素对象 / 子元素对象数组
        click: 函数,  // 绑定的事件类型及对应回调函数
        mouseenter: 函数,
        mouseleave: 函数,
    };
}
```

需要创建一个全局注册表，管理所有事件委托的父元素，重复绑定父元素的子元素会作为分支合并在同一个事件委托之中，避免创建多个相同的事件委托。

---

## 🎬 动画系统

### ✨ animate语法

CHTL JS简化了动画的使用，封装了requestAnimationFrame。

```chtl
script
{
    const anim = animate {
        target: {{选择器}} || [{{选择器1}}, {{选择器2}}] || DOM对象
        duration: 100,  // 动画持续时间，ms
        easing: ease-in-out,  // 缓慢函数

        begin: {  // 起始状态，写css代码

        }

        when: [
            {
                at: 0.4;  // 动画播放到什么时刻

                // css代码
                opacity: 0.5,
                transform: 'scale(1.1)'
            },
            {
                at: 0.8;  // 动画播放到什么时刻
                // css代码
            }
        ]

        end: {  // 终止状态，写css代码

        }

        loop: -1,  // 循环次数
        direction: ,  // 播放的方向
        delay:  ,  // 开始播放的延迟，ms
        callback: function,  // 播放完毕后做什么
    };
}
```

---

## 👻 虚对象

### 🌟 vir语法

虚对象是CHTL JS重要的特征之一，虚对象提供了访问CHTL JS函数的元信息能力。虚对象能够获取CHTL JS函数的任意键的键值。

```chtl
vir test = listen {
    click: () => {

    }

    other: {

    }
};

test->click();  // 解析click为函数引用
test->other;  // 解析other为对象
```

**重要说明**: 
- vir是CHTL JS层面的语法糖，不涉及JS
- listen会按原样生成JS代码
- vir本身就不存在，是编译期间的语法糖

### 🔧 vir实现原理

```chtl
vir Test = listen {
    click: ()=>{

    }
};
```

编译器扫描到vir时，会创建一个C++对象，这个C++对象负责vir的解析。假设这个对象为View，View对象需要做两件事情：
1. 记录vir虚对象的名称
2. 解析CHTL JS函数中的键，并创建对应表

后续在解析时，遇到Test->click;时，会根据键值的类型，转换成不同的内容，比如函数引用，对象，数组等，并且这些结果会缓存在View之中，以便后续的解析。

---

## ⚡ 事件绑定操作符

### 🎮 &-> 操作符

现在你可以使用&->来绑定事件。

```chtl
script
{
    {{.box}} &-> click {
        console.log('Box clicked!');
    }
}
```

---

## 🎯 CJMOD功能

### 💝 printMylove

```chtl
const str = printMylove {
    url: ,
    mode: ,  // 模式可以选择ASCII或Pixel
    width: ,  // 宽度，支持的单位有CSS单位以及百分比，小数，纯数字(像素)
    height: ,  // 高度
    scale:  ,  // 缩放倍数，限定为等比缩放策略
};
```

### 🌟 iNeverAway

iNeverAway用于创建一组标记函数。iNeverAway与其他CHTL JS功能不同，它允许开发者定义键，而不是使用键，并可以使用状态区分同名的键。iNeverAway需要与虚对象共用。

```chtl
vir Test = iNeverAway {
    Void<A>: function(int, int) {

    },

    Void<B>: funtion(int, int) {  // 通过状态同名同参重载

    },

    Void: {

    },

    Ax: {

    }
};

Test->Void<A>();
```

Test是虚拟对象，是不存在的对象，这里并没有创建一个对象。

**实现原理**: 
- iNeverAway -> 创建一组JS全局函数，名称由CHTL编译器统一管理，在调用时才生成对应的JS函数代码
- vir对象本身不存在，最终转变成相对应的函数的引用

### ⚡ util...then表达式

```chtl
util 表达式 -> change { 条件变化时 } -> then { 条件完成时 }

util a > b -> change print("发生变化") -> then print("a > b");  // 单行语句情况下，change条件可以不写分号
util a > b -> change print("发生变化"); -> then print("a > b");
util a < b -> change {print("发生变化");} -> then {print("a < b");}  // 多行代码下，无论如何都要以分号结束
```

---

## 🎊 总结

CHTL JS的语法特征（严格按文档）：

- 🔥 **局部script** - 属于CHTL，由CHTL编译器处理
- 🔥 **增强选择器** - {{CSS选择器}}语法
- 🔥 **-> 操作符** - 明确CHTL语法使用
- 🔥 **listen监听器** - 声明式事件绑定
- 🔥 **delegate委托** - 事件委托机制
- 🔥 **animate动画** - 简化的动画API
- 🔥 **vir虚对象** - 编译期语法糖
- 🔥 **&-> 事件绑定操作符** - 快捷事件绑定

**重要**: 严格按照CHTL语法文档实现，不私自扩展语法！