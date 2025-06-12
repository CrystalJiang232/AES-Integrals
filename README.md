# 项目简介

本项目是基于Δ-集和Δ'-集4轮AES-128积分攻击的C++实现。

# 背景

- **AES(Advanced Encryption Standard)**是基于SPN(Substitution-Permutation Network)的对称加密算法，而采用十轮加密的AES-128是目前被最广泛应用的对称加密算法之一。一般进行密码分析时会假定AES自身的块加密是安全的，既有针对AES的分析多针对其填充、链接、应用（校验）等阶段进行。  
- **积分攻击(Integral Attack)**是选择明文攻击的一种，攻击者对加密算法进行分析后，推断加密一组明文能够使该组明文的对应密文（或其中间量）存在某种线性关系，继而通过验证该线性关系是否成立来破解密钥，其爆破的计算量会远少于直接爆破整个密钥集。  
- SPN的一大缺点是在轮数较少时，由于对密文的混淆、扩散尚不够充分而易受积分攻击的影响。因此针对4轮AES-128的积分攻击被提出，明文选择上以Δ-集（明文的某一字节遍历`0x00`至`0xff`所有值，其余字节相同）和Δ'-集（Δ-集的一个子集）为主。  

# 成果概述

基于C++实现了对Δ-集和Δ'-集的4轮AES攻击，代码应用大量现代C++特性，保证效率的情况下达成了极高的可读性，并预留大量接口用于攻击的进一步拓展应用。  

> 下述二测试均采用生成的可执行文件`./gencp`随机生成的Δ-集（`./delta.txt`）和Δ'-集（`./pdelta.txt`）进行。  
  `./gencp`会随机生成一个AES密钥和Δ-集（第一个字节遍历`0x00`至`0xff`的明文），将4轮AES加密后的密文写入`./delta.txt`（以十六进制文本形式，下同），随后随机选取Δ-集中的66个进行加密，写入`./pdelta.txt`。  

## 测试配置

CPU：13th Gen Intel i7-13700H（内核数14，逻辑处理器数20）  
平台：WSL-Ubuntu 24.04  

## Δ-集测试
### 概述

- 验证了攻击正确性，程序能够正确完成对密钥的猜解。  
- 密钥破解的耗时在毫秒级。  
- 针对Δ-集攻击第一阶段只能选择每位密钥可能值的问题，通过逐个密钥验证完全解密后是否符合Δ-集特性来判定是否为正确密钥，在时间开销基本不变的情况下使解密正确率达到100%。  

## Δ'-集测试
### 概述

- 验证了攻击正确性，程序能够正确完成对密钥的猜解。  
- 相较Python实现，保持可读性的前提下大幅提升了对Δ-集和Δ'-集的积分攻击效率：在个人便携式计算机上，本程序可以在数分钟内完成密钥破解，具备高效地进行实践应用的可行性。  

### 结果  

以密文数n=66、20线程方式进行攻击，执行100次测试的结果如下：    

|计时点|最大值|最小值|平均值|
|:---:|:---:|:---:|:---:|
|线程组|3.51s|2.035s|2.372s|
|单组密钥|58.377s|2.224s|24.135s|
|完整密钥|101.186s|26.339s|64.081s|

- 进行的密钥枚举次数数量级为 $10^{27.5}$。  
- 解密正确率100%(100/100)。    
- 耗时超过100s的攻击占比为2%(2/100)，超过60s的攻击占比为58%(58/100)。  

### 算法性能及比较

密文集互异的角度来看，若单个位的密钥空间为 $q$，则该积分攻击方法的最坏、平均时间复杂度均为 $O(q^4)$，空间复杂度是 $O(q^4)$。  
对于基于n=66的Δ'-集的4轮AES-128的攻击，其密钥枚举次数的数学期望是 $2^{33}$，相较枚举全密钥空间的 $2^{127}$ 次大幅降低；验证每个密钥是否有效亦仅需经过三小步解密（轮密钥加、逆置换和逆列混淆），具备远高于直接爆破完整密钥的可行性。  

# 亮点

作为少有的涵盖了针对Δ'-集、基于现代C++的、能以较高效率实现的AES积分攻击，其大量应用现代C++语言特性以达成可读性、维护性与效率的“双成”。

C++语言技术栈：  

1. 基于`jthread`的多线程处理（TODO：线程间通信等实现效率的极限突破）  
2. `constexpr`修饰大量函数、变量；`consteval`函数阐述“打表”逻辑，极大提升表的可读性&维护性  
3. *range-*`for`，简洁高效的循环语义；`std::views::iota(0,x)`实现安全的范围迭代  
4. AES采用`template<size_t Rounds>`，预留不同轮数的开发接口，编译期常量提供高优化空间  
5. *ranges algorithm* 及利用`operator|`实现链式算法，类型&边界安全+高可读性&维护性  
6. 自定义轻量级`struct byte`，单向`explicit`“双收”，字面量运算符及其它重载使之易于复用拓展  
7. 字节块(block)输入/输出方面，向后兼容`iostream`提供流运算符重载，并通过特化`std::formatter`、及`block_fromhex`函数实现二进制和文本形式的块数据相互转换；相关组件及helper-functions等对于字节数据的处理、输入/输出等提供了复用参考  
8. 应用C++17/20引入的 *view-type*（如`std::string_view`、`std::span`）达成在不同函数间传递时，类型完整信息（如数组大小、`const`语义等）得到保留，同时通过其构造函数对传入参数进行编译期检查，使得函数间传参、内存访问更加安全；使用`using`取`typedef`来达成更好的可读性，同时视图应用能达到极接近原始指针的效率  
9. 在Δ-集攻击生成“潜在密钥集”时，采用了 *coroutines* 这一特性编写异步`co_yield`可能密钥的类，实现了极佳的可读性的同时，通过惰性迭代降低时间与空间开销，不失为为协程之较好实践  


密码学技术栈：  

1. 对AES加密的内部组件、活跃位置及线性变换性质分析出发，得出积分攻击所需验证的性质；  
2. 执行选择明文攻击，对密钥拓展函数逆向而得到原AES密钥。  

同时，在命令行应用方面：  

- 提供了输入、输出文件及方式（文本或二进制）选项  
- 提供指定是否使用多线程、回显模式、利用密文数等选项。  

# 密码学原理相关技术文档

Δ-集攻击原理：[AES积分攻击——云帆](https://crystaljiang232.github.io/crypto/aesatk/4-1/)   
Δ'-集攻击原理：[AES积分攻击——霜天](https://crystaljiang232.github.io/crypto/aesatk/4-2/)  

# Build指令

```sh
rm -rf build && mkdir build && cd build  
cmake .. && cmake --build .  # Add `--config Release` for MSVC  
```

> 亦可直接调用`./AES/onekey_build.sh`。  

# 命令行用法

## `./aes4-1`
```
参数：  -i <inputfile>

-i            十六进制文本形式读 <inputfile> 作为输入。必须明确此选项，inputfile应能提供长度为256的密文集。  

```

## `./aes4-2`
```
参数:  -i|--it|--ib <inputfile> [-o|--ot|--ob <outputfile>] [--count xxx] [--echo xxx] [--nothread]
       -h

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
               默认值为1。
--nothread     禁用多线程并行解密。若如此做，解密时间将大幅增加。  

-h             显示帮助页面。
```

## `./gencp`

```
参数: -a [-q]
      -d -p -k [-q]
-a             生成全部选项（等价于'-d -p -k'）。
-d             生成Δ-集，即对单个随机明文块，令其某字节遍历0x00-0xff，经4轮AES-128加密的密文集。
-p             生成Δ'-集，即Δ-集的大小为66的子集。
-k             输出密钥。
注意：当同时指定-d和-p时，二者生成密文集所用的加密密钥是相同的（可用-k输出之），但所用的随机明文块并不相同（故实际生成的文件中，Δ'-集的内容并非Δ-集的子集）。
-q             安静模式。
``` 

# 正确性验证

```sh
./gencp -a && ./aes4-1 -i delta.txt && ./aes4-2 -i pdelta.txt --echo 3 && cat ./key.txt
```

# 展望

1. 进程间通信，多线程调度效率进一步突破  
2. 攻击拓展至五轮AES-128（可能涉及`key_expansion`改动）  
3. `std::formatter<block>`的`parse`格式字符串选项待开发（提供含大小写、是否含空格、每行个数等的选项）  

# 参与者

- [Hibiscus](https://github.com/CrystalJiang232/)，E-mail:crystaljiang232@126.com

---

# Project Overview  

This project is a **C++ implementation of a 4-round AES-128 integral attack** (also known as a **Square attack**) based on **Δ-sets** and **Δ'-sets**.  

# Background  

- **AES (Advanced Encryption Standard)** is a symmetric encryption algorithm based on the **SPN (Substitution-Permutation Network)** structure. **AES-128**, with its 10-round encryption, is one of the most widely used symmetric ciphers. Most cryptanalysis assumes AES’s core - block encryption is secure; existing attacks often target its padding, chaining, or application (e.g., verification) stages.  
- **Integral Attack**: A chosen-plaintext attack where the attacker analyzes the cipher to identify linear relationships in intermediate values (or ciphertexts) of a carefully constructed plaintext set. By verifying these relationships, the attacker can recover the key with far fewer computations than brute-forcing the entire key space.  
- **SPN Weakness**: With fewer rounds, insufficient diffusion/confusion makes AES vulnerable to integral attacks. Hence, **4-round AES-128 attacks** were proposed, using **Δ-sets** (plaintexts with one byte varying over `0x00`–`0xff` while others remain constant) and **Δ'-sets** (a subset of Δ-sets).  

# Key Achievements  

- Implemented Δ-set and Δ'-set attacks in **Modern C++**, balancing **efficiency** and **readability**, with extensible interfaces for further research.  
- **Test Setup**:  
  - CPU: 13th Gen Intel i7-13700H (14 cores, 20 threads)  
  - Platform: WSL-Ubuntu 24.04  

## Δ-Set Test  
- Validated attack correctness; the program successfully recovers the key.  
- The program can complete its execution in miliseconds.
- Improved key-guessing by verifying Δ-set properties for each candidate key, achieving **100% accuracy** with negligible time overhead.  

## Δ'-Set Test  
- Correctness: Key recovery validated.  
- Performance: Completed in **minutes** , significantly faster than slower Python implementation(~hours or even more).  
- **Benchmarks (n=66, 20 threads, 100 tests)**:  

  | Metric          | Max    | Min    | Avg     |  
  |-----------------|--------|--------|---------|  
  | Thread Group    | 3.51s  | 2.035s | 2.372s  |  
  | Single Key      | 58.377s| 2.224s | 24.135s |  
  | Full Key        | 101.186s| 26.339s| 64.081s |  

  - **Key Enumeration**: ~ $10^{27.5}$ attempts.  
  - **Success Rate**: 100% (100/100).    
  - **Efficiency**: 58% of tests completed under 60s; only 2% exceeded 100s.  

### Algorithmic Complexity  
- **Time/Space Complexity**: *O(q⁴)* for a *q*-sized key subspace.  
- For **n=66 Δ'-sets**, the expected key attempts are $2^{33}$ , vastly lower than brute-force ( $2^{127}$ ). Each key verification requires only **three lightweight steps** (AddRoundKey, InvSubBytes, InvMixColumns).  

# Highlights  

One of the few **Δ'-set-capable**, **modern C++** implementations of AES integral attacks, combining **readability**, **maintainability**, and **performance**.  

## C++ Techniques  
1. **Multithreading**: `jthread` for parallel processing (TODO: optimize inter-thread communication).  
2. **Compile-Time Optimization**: `constexpr`/`consteval` for precomputed tables.  
3. **Ranges & Algorithms**: `std::views::iota`, range-`for`, and pipeline operators (`|`) for safe, readable loops.  
4. **Templated AES**: `template<size_t Rounds>` for compile-time flexibility.  
5. **Custom `byte` Type**: Lightweight `struct` with `explicit` conversions and overloaded operators.  
6. **I/O Flexibility**: Stream operators, `std::formatter` specialization, and hex/binary converters (`block_fromhex`).  
7. **Memory Safety**: `std::span`/`std::string_view` for bounds-checked data passing.  
8. **Coroutines**: Lazy key-generation via `co_yield` to reduce overhead.  

## Cryptography Stack  
1. Analyzed AES internals (active bytes, linear transformations) to derive integral properties.  
2. Executed chosen-plaintext attacks, reverse-engineered key expansion to recover the original key.  

# Cryptographic Documentation  
- Δ-set attack: [AES Integral Attack — Yunfan](https://crystaljiang232.github.io/crypto/aesatk/4-1/)  
- Δ'-set attack: [AES Integral Attack — Shuangtian](https://crystaljiang232.github.io/crypto/aesatk/4-2/)  

# Build Instructions  

```sh
rm -rf build && mkdir build && cd build  
cmake .. && cmake --build .  # Add `--config Release` for MSVC  
```  

> Shell scripts also available at `./AES/onekey_build.sh`.  

# CLI Usage

## `./aes4-1`

```
Usage：  -i <inputfile>

-i            Use <inputfile> as input in hexidecimal-text format.
              This option must be specified, and <inputfile> shall provide a ciphertext for delta-set with size of 256.
```

## `./aes4-2`

```
Usage: -i|--it|--ib <inputfile> [-o|--ot|--ob <outputfile>] [--count xxx] [--echo xxx] [--nothread]
       -g|-h

-i,--it        Use <inputfile> as input in hexidecimal-text format.
--ib           Use <inputfile> as input in binary format.
               One of the input options must be specified.
-o,--ot        Use <outputfile> as output in hexidecimal-text format.
--ob           Use <outputfile> as output in binary format.
               The key in hex format will be printed to the console disregarding this option unless -echo=0.
--count xxx    Read at most xxx valid ciphertexts from input file. Valid range for xxx is 30-256. Default value is 66.
               Note that ciphertext count lower than 50 can cause significant loss in decryption accuracy.
--echo xxx     Specify the echo(output to the terminal) options:
               3 - Display time elapsed for every round of decryption attempt.
               2 - Display time elapsed for every group of key deciphered(Default option).
               1 - Display time elapsed after completing the entire decryption process.
               0 - Disable echo to the terminal.
               Default value is 2.
--nothread     Disable multithreading. This may significantly prolong the execution time.

-h             Display help screen.
```

## `./gencp`

```
Usage: -a [-q]
       -d -p -k [-q]
-a             Generate all; equivalent to '-d -p -k'.
-d             Generate delta-set, that is, the 4-round AES-128 encrypted ciphertext of a random plaintext, with one particular byte traversing from 0x00 to 0x0f.
-p             Generate partial delta-set, a size=66 subset of delta-set by definition.
-k             Output the answer key.
NOTE: The delta-set and partial delta-set, when generated simotaneously, are guaranteed of been encrypted by the same key(output via '-k' option, if designated), yet the sample plaintext used in generating the ciphertext isn't identical(Thus the partial delta-set generated IS NOT a subset of the delta-set generated).
-q             Quiet mode.
```

# Validation  
```sh
./gencp -a && ./aes4-1 -i delta.txt && ./aes4-2 -i pdelta.txt --echo 3 && cat ./key.txt  
```  

# Future Work  
1. Optimize inter-thread communication.  
2. Extend to **5-round AES-128** (may require `key_expansion` modifications).  
3. Enhance `std::formatter<block>` with formatting options (case, spacing, etc.).  

# Contributor  
- [Hibiscus](https://github.com/CrystalJiang232/), Email: crystaljiang232@126.com  



