# Text Search Engine——一个.txt英文文档搜索工具
## 工具原理及作用
### 1.KMP算法
可参考b站（可以直接点击右侧链接跳转至b站视频页）[最浅显易懂的KMP算法详解](https://www.bilibili.com/video/BV1AY4y157yL)和acwing831对kmp算法的基本思想以及如何构造next数组进行理解
### 2.查找并返回查询单词在文本中出现的段落号及段落文本
### 3. Windows 文件选择对话框

>该代码实现了一个 Windows 文件选择对话框，允许用户选择一个文件，并返回该文件的路径。

#### 3.1 Windows API
使用了 Windows API 函数，特别是 `GetOpenFileName` 函数，用于显示一个文件选择对话框并返回用户选择的文件路径。

#### 3.2 涉及的库
- **`windows.h`**: 这是 Windows 编程的基础库，包含了与 Windows 操作系统交互的函数和类型定义。它提供了 `OPENFILENAME` 结构体、`GetOpenFileName` 函数等功能。
- **`wchar.h` 或 `tchar.h`**: 用于支持宽字符（`wchar_t`）的字符串操作，因为 Windows API 中许多函数（如 `GetOpenFileName`）使用的是宽字符类型。

#### 3.3 结构体初始化 (`OPENFILENAME ofn`)
`OPENFILENAME` 是一个 Windows API 结构体，包含了文件对话框的配置信息。通过设置这个结构体的成员，程序能够定制文件对话框的行为。

```cpp
ZeroMemory(&ofn, sizeof(ofn));  // 清空 OPENFILENAME 结构体的内容，防止使用未初始化的字段
ofn.lStructSize = sizeof(ofn);   // 指定结构体的大小，确保结构体与 Windows API 兼容
ofn.hwndOwner = NULL;            // 表示对话框的父窗口句柄，这里设为 NULL，意味着没有父窗口
ofn.lpstrFile = szFile;         // 指定用来存储用户选择文件路径的缓冲区 szFile
ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);  // 文件路径缓冲区 szFile 的大小
ofn.lpstrFilter = L"Text Files\0*.TXT\0All Files\0*.*\0";  // 文件过滤器，定义用户在对话框中看到的文件类型
ofn.nFilterIndex = 1;           // 过滤器的索引，1 表示默认选择 "Text Files" 过滤器
ofn.lpstrFile[0] = L'\0';       // 初始化 szFile 字符数组为空，确保没有默认的路径
ofn.lpstrTitle = L"Select a text file";  // 对话框标题
ofn.lpstrDefExt = L"txt";       // 默认扩展名，如果用户没有输入扩展名，将自动使用 .txt
```
`GetOpenFileName(&ofn)` 是 Windows API 提供的文件选择对话框函数。它会弹出一个文件选择窗口，允许用户选择文件。如果用户成功选择了文件并点击 "OK" 按钮，该函数返回 `TRUE`。如果用户取消选择或关闭对话框，则返回 `FALSE`。


## 测试
> 测试样例选用哈利波特与魔法石第一章原文
> 
> 下图仅为部分结果

![芝士结果](./测试.png)
