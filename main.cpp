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

// 定义 TextQuery 类，用于执行文本查询操作
class TextQuery {
public:
    typedef vector<string>::size_type line_no;  // 定义行号类型

    // 读取文件内容到内部存储
    void read_file(wifstream& is) {
        store_file(is);  // 从文件中读取内容
        build_map();     // 构建单词与行号的映射关系
    }

    // 执行查询，返回包含查询单词出现行号的集合
    set<line_no> run_query(const string&) const;

    // 获取某一行的文本内容
    string text_line(line_no) const;

private:
    // 私有成员函数
    void store_file(wifstream&);  // 存储文件内容
    void build_map();            // 构建单词与行号的映射关系

    vector<string> lines_of_text;  // 存储文本文件的每一行
    map<string, set<line_no>> word_map;  // 存储单词与行号的映射
};

// 读取文件内容并存储到 lines_of_text 中
void TextQuery::store_file(wifstream& is) {
    wstring textline;
    while (getline(is, textline)) {  // 逐行读取文件内容
        // 将宽字符(wchar_t)字符串转换为UTF-8编码的普通字符串
        string utf8_line(textline.begin(), textline.end());
        lines_of_text.push_back(utf8_line);  // 将每行内容存入 vector 中
    }
}

// 构建单词与行号的映射
void TextQuery::build_map() {
    for (line_no line_num = 0; line_num != lines_of_text.size(); line_num++) {
        istringstream line(lines_of_text[line_num]);  // 使用 stringstream 解析每行
        string word;
        while (line >> word) {  // 将每行的每个单词插入 word_map
            word_map[word].insert(line_num);  // word_map 存储单词与行号的对应关系
        }
    }
}

// 查询指定单词，返回该单词出现的所有行号
set<TextQuery::line_no> TextQuery::run_query(const string& query_word) const {
    auto loc = word_map.find(query_word);  // 查找单词在 map 中的位置
    if (loc != word_map.end()) {
        return loc->second;  // 返回该单词的行号集合
    }
    return set<line_no>();  // 如果没有找到，返回空集合
}

// 获取指定行号的文本内容
string TextQuery::text_line(line_no line) const {
    if (line < lines_of_text.size()) {
        return lines_of_text[line];  // 返回指定行号的内容
    }
    throw out_of_range("line number out of range");  // 如果行号超出范围，抛出异常
}

// Windows 文件选择对话框
string open_file_dialog() {
    OPENFILENAME ofn;       // 文件选择框结构体
    wchar_t szFile[260];    // 使用宽字符数组（wchar_t）

    // 初始化 OPENFILENAME 结构体
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
    ofn.lpstrFilter = L"Text Files\0*.TXT\0All Files\0*.*\0";  // 使用宽字符常量
    ofn.nFilterIndex = 1;
    ofn.lpstrFile[0] = L'\0';  // 初始化文件路径为空
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = L"Select a text file";  // 使用宽字符常量
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = L"txt";  // 使用宽字符常量

    // 显示文件选择对话框
    if (GetOpenFileName(&ofn) == TRUE) {
        return string(szFile, szFile + wcslen(szFile));  // 将宽字符数组转换为 std::string
    }
    return "";  // 如果没有选择文件，返回空字符串
}

// 生成复数形式的单词（根据单词出现次数判断）
string make_plural(size_t cnt, const string& word, const string& words) {
    return (cnt == 1) ? word : word + words;  // 如果出现次数为1，不加复数形式；否则加上复数形式的 "s"
}

// 打印查询结果
void print_results(const set<TextQuery::line_no>& locs, const string& sought, const TextQuery& file) {
    typedef set<TextQuery::line_no> line_nums;  // 定义行号集合类型
    line_nums::size_type size = locs.size();  // 获取单词出现的行数
    cout << sought << " occurs " << size << " " << make_plural(size, "time", "s") << endl;  // 输出查询的单词及其出现次数
    line_nums::const_iterator it = locs.begin();
    for (; it != locs.end(); ++it) {  // 输出每行的内容
        cout << "\t(line" << ((*it) + 1) << ") " << file.text_line(*it) << endl;
    }
}

// 主函数
int main() {
    string filename = open_file_dialog();  // 打开文件选择对话框，获取文件路径
    if (filename.empty()) {  // 如果没有选择文件，输出错误信息
        cerr << "No file selected!" << endl;
        return EXIT_FAILURE;
    }

    wifstream infile(filename);  // 使用 wifstream 读取文件
    infile.imbue(locale("en_US.UTF-8"));  // 设置文件为 UTF-8 编码
    if (!infile) {  // 如果文件打开失败，输出错误信息
        cerr << "Failed to open file!" << endl;
        return EXIT_FAILURE;
    }

    TextQuery tq;  // 创建 TextQuery 对象
    tq.read_file(infile);  // 读取文件内容

    while (true) {
        cout << "Enter word to look for, or q to quit: ";
        string s;
        cin >> s;  // 获取用户输入的查询单词
        if (!cin || s == "q") break;  // 如果用户输入 "q" 或遇到输入流错误，则退出
        set<TextQuery::line_no> locs = tq.run_query(s);  // 查找单词出现的行号
        print_results(locs, s, tq);  // 打印查询结果
    }

    return 0;  // 程序正常退出
}
