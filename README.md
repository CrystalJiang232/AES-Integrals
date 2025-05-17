# 项目简介

本项目是基于Δ-集和Δ'-集4轮AES-128积分攻击的C++实现。

# 背景

AES(Advanced Encryption Standard)是基于SPN(Substitution-Permutation Network)的对称加密算法，而采用十轮加密的AES-128是目前被最广泛应用的对称加密算法之一。一般进行密码分析时会假定AES自身的块加密是安全的，既有针对AES的分析多针对其填充、链接、应用（校验）等阶段进行。
积分攻击(Integral Attack)是选择明文攻击的一种，攻击者对加密算法进行分析后，推断加密一组明文能够使该组明文的对应密文（或其中间量）存在某种线性关系，继而通过验证该线性关系是否成立来破解密钥，其爆破的计算量会远少于直接爆破整个密钥集。  
SPN的一大缺点是在轮数较少时，由于对密文的混淆、扩散尚不够充分而易受积分攻击的影响。因此针对4轮AES-128的积分攻击被提出，明文选择上以Δ-集（明文的某一字节遍历`0x00`至`0xff`所有值，其余字节相同）和Δ'-集（Δ-集的一个子集）为主。  

# 成果

基于C++实现了对Δ-集和Δ'-集的4轮AES攻击，代码应用大量现代C++特性，保证效率的情况下达成了极高的可读性，并预留大量接口用于攻击的进一步拓展应用。  

## 测试集相关

使用C++生成随机密钥，对随机明文块生成密文的Δ-集和Δ'-集，交由C++编写、MSVC 19.4编译的程序进行攻击。  
成果如下：  

- 验证了攻击正确性，编译程序能够正确完成对密钥的猜解；  
- 相较Python实现，保持可读性的前提下大幅提升了对Δ-集和Δ'-集的积分攻击效率：在个人便携式计算机上，Δ-集可以在数十毫秒内完成密钥破解，Δ'-集的耗时数量级亦在分钟级，具备实践应用的可行性（具体测试设备、数据等见下）。  

### 测试详情

CPU：13th Gen Intel i7-13700H（内核数14，逻辑处理器数20）  
平台：WSL-Ubuntu 24.04  
`build`及编译指令等：参考`./CMakeLists.txt`  
命令行参数：`./aes-attack -i pdelta.txt -o key.txt --echo 1`  
结果：  
100次测试中，  
单组解密耗时平均值ms，最大值ms，最小值ms  
密钥获取正确率 %(/100)  
完整解密耗时平均值ms，最大值ms，最小值ms  

# 亮点

作为少有的涵盖了针对Δ'-集、基于现代C++的、能以较高效率实现的AES积分攻击，其大量应用现代C++语言特性以达成可读性、维护性与效率的“双成”。
涉及到的C++语言技术栈如下：  

1. 基于`jthread`的多线程处理（TODO：线程间通信等实现效率的极限突破）  
2. `constexpr`修饰大量函数、变量；`consteval`函数阐述“打表”逻辑，极大提升表的可读性&维护性  
3. *range-*`for`，简洁高效的循环语义；`std::views::iota(0,x)`实现安全的范围迭代  
4. AES采用`template<size_t Rounds>`，预留不同轮数的开发接口，编译期常量提供高优化空间  
5. *ranges algorithm* 及利用`operator|`实现链式算法，类型&边界安全+高可读性&维护性  
6. 自定义轻量级`struct byte`，单向`explicit`“双收”，字面量运算符及其它重载使之易于复用拓展  
7. 字节块(block)输入/输出方面，向后兼容`iostream`提供流运算符重载，并通过特化`std::formatter`、及`block_fromhex`函数实现二进制和文本形式的块数据相互转换；相关组件及helper-functions等对于字节数据的处理、输入/输出等提供了复用参考  
8. 应用C++17/20引入的 *view-type*（如`std::string_view`、`std::span`）达成在不同函数间传递时，类型完整信息（如数组大小、`const`语义等）得到保留，同时通过其构造函数对传入参数进行编译期检查，使得函数间传参、内存访问更加安全；使用`using`取`typedef`来达成更好的可读性，同时视图应用能达到极接近原始指针的效率  

密码学技术栈：  

1. 对AES加密的内部组件、活跃位置及线性变换性质分析出发，得出积分攻击所需验证的性质；  
2. 执行选择明文攻击，对密钥拓展函数逆向而得到原AES密钥。  

同时，在命令行应用方面：  

- 提供了输入、输出文件及方式（文本或二进制）选项  
- 提供指定是否使用多线程、回显模式、利用密文数等  

基于Δ-集的4轮AES-128积分攻击的密码学技术文档，请参阅：[AES积分攻击——“启卷”章](https://crystaljiang232.github.io/crypto/aesatk/4-1/)

# Build指令

```sh
rm -rf build
mkdir build
cd build
cmake ..
cmake --build .
```

# 命令行用法

```
参数:  -i|--it|--ib <inputfile> [-o|--ot|--ob <outputfile>] [--count xxx] [--echo xxx] [--nothread]
       -g|-h

-i,--it        十六进制文本形式读 <inputfile> 作为输入。  
--ib           二进制方式读 <inputfile> 作为输入。   
               调用时必须在输入选项中二选一（除非指定 -h 或 -g）。  
-o,--ot        十六进制文本形式输出密钥至 <outputfile>。  
--ob           二进制方式输出密钥至 <outputfile>。  
               除非指定 "--echo 0"，否则解出的密钥都会同时被输出至控制台。  
--count xxx    从输入文件中至多读取 count 个密文，有效范围为30-256。默认值为66。  
               注意，当输入密文数小于50时会导致解密正确率大幅下降。
--echo xxx     选择输出格式值，有效值为0-3：  
               3 - 每轮并行解密结束后输出耗时；  
               2 - 每组密钥解密完成后输出耗时；  
               1 - 完成密钥破解后输出耗时；
               0 - 不向控制台输出内容。
               默认值为2。
--nothread     禁用多线程并行解密。若如此做，解密时间将大幅增加。  

-g             在当前目录下的pdelta.txt中生成一个Δ'-集（长度为66）供攻击测试。
-h             显示帮助页面。
```


# 展望

1. 进程间通信，多线程调度效率进一步突破  
2. 攻击拓展至五轮AES-128（可能涉及`key_expansion`改动）  
3. Δ-集应用部分解密校验，进一步提升确定密钥速度  
4. `std::formatter<block>`的`parse`格式字符串选项待开发（提供含大小写、是否含空格、每行个数等的选项）  

# 参与者

- [Hibiscus](https://github.com/CrystalJiang232/)，E-mail:crystaljiang232@126.com

---

# Project Briefing

This project is an C++ implementation of 4-round AES-128 integral attack based on delta-set and delta'-set.

# Background Information



# Features

Being one of the few efficient 4-round AES Integral Attack implementation in modern C++ that covers both towards delta-set and delta'-set, modern C++ features are applied proactively to achieve maintainability, readability while obtaining maximum efficiency.  

Featuring C++'s technical stack are as follows:  

- Multithreading support based on `jthread`  
- `constexpr` functions and variables enabling compile-time computation to the uttermost  
- `consteval` functions defined to further elaborate the process of pre-computed table, enabling further maintainance and modification as required  
- *range-based* `for` loop and `std::views::iota` substituting traditional loops, ensuring type and bound safety during iterations  
- custom `struct byte` applied with exquisite application of `explicit` keyword to gain equilibarium between 


# Contributors

- [Hibiscus](https://github.com/CrystalJiang232/)，E-mail:crystaljiang232@126.com