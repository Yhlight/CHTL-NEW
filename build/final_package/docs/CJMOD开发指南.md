# CJMOD开发指南（严格按文档）

## 🔥 CJMOD简介

CJMOD是CHTL项目CHTL JS模块的扩展组件，用于提供模块化分发CHTL JS代码。CHTL提供了CJMOD API，**CJMOD API极其强大，能够高效创建CHTL JS语法**。

---

## 🛠️ CJMOD API详解

### 📊 Syntax类

语法分析类，负责对语法进行解析。

#### analyze方法
分析语法，返回一个Arg对象，Arg对象包含了解析出的参数列表。

```cpp
Arg args = Syntax::analyze("$ ** $");  // 语法分析
args.print();  // 输出-> ["$", "**", "$"]
```

#### 类型判断方法

```cpp
// 判断是否是JS对象
Syntax::isObject("{b: 1}");  // 输出-> true

// 判断是否是JS函数
Syntax::isFunction("function a(){}");  // 输出-> true

// 判断是否是JS数组
Syntax::isArray("[1, 2, 3]");  // 输出-> true

// 判断是否是CHTL JS函数
Syntax::isCHTLJSFunction("test {test: 1, test2: 2};");  // 输出-> true
```

---

## 📋 Arg类

参数列表类，包含了解析出的参数列表，并且提供了参数的绑定、填充、转换等操作。

### bind方法
让一个原子Arg绑定获取值的函数。

```cpp
Arg args = Syntax::analyze("$ ** $");  // 语法分析

args.bind("$", [](const std::string& value) {
    return value;
});

args.bind("**", [](const std::string& value) {
    return Syntax::isCHTLJSFunction(value) ? "" : value;
});
```

### fillValue方法
填充参数列表的值。

```cpp
Arg result = CJMODScanner::scan(args);
args.fillValue(result);
args.fillValue(Arg(["3", "**", "4"]));
```

### transform方法
参数最终输出什么JS代码。

```cpp
args.transform("pow(" + arg[0].value + arg[2].value + ")");
```

---

## 🔍 CJMODScanner类

统一扫描器用于CJMOD API的接口。

### scan方法
扫描语法片段，第二参数为扫描的关键字。

```cpp
Arg result = CJMODScanner::scan(args, "**");
```

---

## ⚙️ CJMODGenerator类

生成器用于CJMOD API的接口。

### exportResult方法
导出最终的JS代码。

```cpp
CJMODGenerator::exportResult(args);
```

---

## 🔧 AtomArg原子参数

AtomArg是原子参数，Arg封装此。

### 占位符类型

#### $ - 占位符
基础占位符

#### $? - 可选占位符
可选的参数占位符

#### $! - 必须占位符
必须提供的参数占位符

#### $_ - 无序占位符
无序参数占位符

**上述占位符可以组合，例如$!_**

#### ... - 不定参数占位符
支持可变参数

### bind方法
绑定获取值的函数。

```cpp
args[0].bind([](const std::string& value) {return value;});
```

### fillValue方法
填充参数值。

```cpp
args[0].fillValue("3");
args[0].fillValue(3);
```

---

## 🎯 CHTLJSFunction类

CHTL JS函数，用于CJMOD API的接口。

### CreateCHTLJSFunction方法
封装了原始API构建语法的流程，能够快速构建CHTL JS函数，这些CHTL JS函数天然支持虚对象vir以及无修饰字符串。

```cpp
CHTLJSFunction func;
CHTLJSFunction::CreateCHTLJSFunction("printMyLove {url: $!_, mode: $?_}");
```

对应的CHTL JS使用：
```chtl
script
{
    printMyLove({url: "https://www.baidu.com", mode: "auto"});
}

// 天然支持vir
script
{
    vir test = printMyLove({url: "https://www.baidu.com", mode: "auto"});
}
```

### bindVirtualObject方法
绑定虚对象vir。对于不使用CreateCHTLJSFunction创建的，但是符合CHTL JS函数的语法，可以使用bindVirtualObject手动绑定虚对象vir，获得虚对象的支持。

```cpp
Syntax::isCHTLJSFunction("printMyLove {url: $!_, mode: $?_}");  // 输出-> true
CHTLJSFunction::bindVirtualObject("printMyLove");  // 写函数名称
```

---

## 💡 完整使用案例

### 📝 按文档的完整示例

```cpp
// 完整的CJMOD API使用案例
Arg args = Syntax::analyze("$ ** $");  // 语法分析
args.print();  // 输出-> ["$", "**", "$"]

args.bind("$", [](const std::string& value) {
    return value;
});

args.bind("**", [](const std::string& value) {
    return value;
});

args.bind("$", [](const std::string& value) {
    return value;
});

Arg result = CJMODScanner::scan(args, "**");
result.print();  // 输出-> ["3", "**", "4"]

args.fillValue(result);
std::cout << arg[0].value << std::endl;  // 输出-> 3
std::cout << arg[1].value << std::endl;  // 输出-> **
std::cout << arg[2].value << std::endl;  // 输出-> 4

args.transform("pow(" + arg[0].value + arg[2].value + ")");

CJMODGenerator::exportResult(args);
```

对应的转换结果：
```chtl
script
{
    console.log(3 ** 4);
}

->

<script>
    console.log(pow(3, 4));
</script>
```

---

## 🎯 CJMOD开发最佳实践

### 📋 严格按API开发

1. **使用Syntax::analyze进行语法分析**
2. **使用bind方法绑定参数处理函数**
3. **使用CJMODScanner::scan进行扫描**
4. **使用fillValue填充参数值**
5. **使用transform定义输出转换**
6. **使用CJMODGenerator::exportResult导出结果**

### ⚠️ 重要提醒

**不要私自扩展CJMOD API！** CJMOD.md已经把这个系统说得通透，不仅好用还强大。严格按照文档提供的API进行开发。

---

## 🎊 总结

CJMOD API极其强大，能够高效创建CHTL JS语法。通过严格按照文档使用这些API，可以实现强大的CHTL JS语法扩展功能。

**🔥 严格按照CJMOD.md文档使用API，不要私自扩展！**