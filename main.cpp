#include <iostream>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <windows.h>
#include <locale>

using namespace std;

// KMP算法：构建部分匹配表（LPS数组）
// LPS（Longest Prefix Suffix）数组用于记录模式串每个位置的最长前缀和后缀的匹配长度
vector<int> build_kmp_table(const string& pattern) {
    vector<int> lps(pattern.size(), 0);  // 存储每个位置的前缀后缀匹配长度
    int j = 0;  // 模式串的当前匹配位置

    // 从第二个字符开始计算LPS数组
    for (int i = 1; i < pattern.size(); ++i) {
        // 如果字符不匹配，回溯到lps数组中的前一个位置
        while (j > 0 && pattern[i] != pattern[j]) {
            j = lps[j - 1];  // 通过回退，尝试找到新的匹配位置
        }
        if (pattern[i] == pattern[j]) {
            ++j;  // 匹配成功，扩展匹配长度
        }
        lps[i] = j;  // 更新当前字符的前缀后缀匹配长度
    }
    return lps;
}

// KMP算法：在文本中查找模式串的所有位置
vector<int> kmp_search(const string& text, const string& pattern) {
    vector<int> lps = build_kmp_table(pattern);  // 获取模式串的LPS数组
    vector<int> positions;  // 存储模式串匹配到的位置

    int j = 0;  // 模式串的当前匹配位置

    // 遍历文本字符串
    for (int i = 0; i < text.size(); ++i) {
        // 如果字符不匹配，回退到LPS数组中的前一个位置
        while (j > 0 && text[i] != pattern[j]) {
            j = lps[j - 1];  // 通过回退，尝试找到新的匹配位置
        }
        if (text[i] == pattern[j]) {
            ++j;  // 匹配成功，扩展匹配长度
        }
        // 如果匹配成功并且模式串已经完全匹配
        if (j == pattern.size()) {
            positions.push_back(i - j + 1);  // 记录模式串匹配的起始位置
            j = lps[j - 1];  // 继续搜索下一个可能的匹配
        }
    }
    return positions;  // 返回所有匹配的位置
}

class TextQuery {
public:
    typedef vector<string>::size_type line_no;  // 行号类型别名

    // 读取文件内容并构建文件的文本数据
    void read_file(wifstream& is) {
        store_file(is);  // 从文件读取内容
    }

    // 执行查询，返回包含查询单词出现行号的集合
    set<line_no> run_query(const string& query_word) const;

    // 获取某一行的文本内容
    string text_line(line_no) const;

private:
    void store_file(wifstream&);  // 存储文件内容

    vector<string> lines_of_text;  // 存储文本文件的每一行
};

// 存储文件内容到 lines_of_text 中
void TextQuery::store_file(wifstream& is) {
    wstring textline;
    // 逐行读取文件内容
    while (getline(is, textline)) {
        // 将宽字符（wchar_t）字符串转换为UTF-8编码的普通字符串
        string utf8_line(textline.begin(), textline.end());
        lines_of_text.push_back(utf8_line);  // 将每行内容存入 vector 中
    }
}

// 查找并返回查询单词在文本中出现的行号
set<TextQuery::line_no> TextQuery::run_query(const string& query_word) const {
    set<line_no> matching_lines;  // 存储包含查询单词的行号集合

    // 遍历每一行，使用 KMP 算法查找查询单词
    for (line_no line_num = 0; line_num != lines_of_text.size(); ++line_num) {
        const string& line = lines_of_text[line_num];  // 当前行的文本
        vector<int> positions = kmp_search(line, query_word);  // 使用 KMP 算法查找单词
        if (!positions.empty()) {
            matching_lines.insert(line_num);  // 如果找到了匹配，将行号插入集合
        }
    }
    return matching_lines;  // 返回所有包含查询单词的行号
}

// 获取指定行号的文本内容
string TextQuery::text_line(line_no line) const {
    if (line < lines_of_text.size()) {
        return lines_of_text[line];  // 返回指定行号的文本内容
    }
    throw out_of_range("line number out of range");  // 如果行号超出范围，抛出异常
}

// 打开文件选择对话框，返回用户选择的文件路径
string open_file_dialog() {
    OPENFILENAME ofn;
    wchar_t szFile[260];

    // 初始化文件选择框结构体
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
    ofn.lpstrFilter = L"Text Files\0*.TXT\0All Files\0*.*\0";  // 文件过滤器
    ofn.nFilterIndex = 1;
    ofn.lpstrFile[0] = L'\0';  // 初始化文件路径为空
    ofn.lpstrTitle = L"Select a text file";  // 对话框标题
    ofn.lpstrDefExt = L"txt";  // 默认文件扩展名

    // 显示文件选择对话框
    if (GetOpenFileName(&ofn) == TRUE) {
        return string(szFile, szFile + wcslen(szFile));  // 返回文件路径
    }
    return "";  // 如果未选择文件，返回空字符串
}

// 根据单词出现次数生成复数形式
string make_plural(size_t cnt, const string& word, const string& words) {
    return (cnt == 1) ? word : word + words;  // 如果出现次数为1，不加复数形式；否则加上复数形式的 "s"
}

// 打印查询结果
void print_results(const set<TextQuery::line_no>& locs, const string& sought, const TextQuery& file) {
    typedef set<TextQuery::line_no> line_nums;  // 行号集合类型
    line_nums::size_type size = locs.size();  // 获取单词出现的行数
    cout << sought << " occurs " << size << " " << make_plural(size, "time", "s") << endl;  // 输出查询的单词及其出现次数

    // 打印包含该单词的每一行内容
    for (auto it = locs.begin(); it != locs.end(); ++it) {
        cout << "\t(line" << ((*it) + 1) << ") " << file.text_line(*it) << endl;
    }
}

// 主函数
int main() {
    string filename = open_file_dialog();  // 打开文件选择对话框，获取文件路径
    if (filename.empty()) {  // 如果没有选择文件，输出错误信息并退出
        cerr << "No file selected!" << endl;
        return EXIT_FAILURE;
    }

    wifstream infile(filename);  // 使用 wifstream 读取文件
    infile.imbue(locale("en_US.UTF-8"));  // 设置文件为 UTF-8 编码
    if (!infile) {  // 如果文件打开失败，输出错误信息并退出
        cerr << "Failed to open file!" << endl;
        return EXIT_FAILURE;
    }

    TextQuery tq;  // 创建 TextQuery 对象
    tq.read_file(infile);  // 读取文件内容

    while (true) {
        cout << "Enter word to look for, or q to quit: ";  // 提示用户输入查询单词
        string s;
        cin >> s;  // 获取用户输入的查询单词
        if (!cin || s == "q") break;  // 如果用户输入 "q" 或遇到输入流错误，则退出
        set<TextQuery::line_no> locs = tq.run_query(s);  // 查找单词出现的行号
        print_results(locs, s, tq);  // 打印查询结果
    }

    return 0;  // 程序正常退出
}
