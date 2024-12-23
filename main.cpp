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

// ���� TextQuery �࣬����ִ���ı���ѯ����
class TextQuery {
public:
    typedef vector<string>::size_type line_no;  // �����к�����

    // ��ȡ�ļ����ݵ��ڲ��洢
    void read_file(wifstream& is) {
        store_file(is);  // ���ļ��ж�ȡ����
        build_map();     // �����������кŵ�ӳ���ϵ
    }

    // ִ�в�ѯ�����ذ�����ѯ���ʳ����кŵļ���
    set<line_no> run_query(const string&) const;

    // ��ȡĳһ�е��ı�����
    string text_line(line_no) const;

private:
    // ˽�г�Ա����
    void store_file(wifstream&);  // �洢�ļ�����
    void build_map();            // �����������кŵ�ӳ���ϵ

    vector<string> lines_of_text;  // �洢�ı��ļ���ÿһ��
    map<string, set<line_no>> word_map;  // �洢�������кŵ�ӳ��
};

// ��ȡ�ļ����ݲ��洢�� lines_of_text ��
void TextQuery::store_file(wifstream& is) {
    wstring textline;
    while (getline(is, textline)) {  // ���ж�ȡ�ļ�����
        // �����ַ�(wchar_t)�ַ���ת��ΪUTF-8�������ͨ�ַ���
        string utf8_line(textline.begin(), textline.end());
        lines_of_text.push_back(utf8_line);  // ��ÿ�����ݴ��� vector ��
    }
}

// �����������кŵ�ӳ��
void TextQuery::build_map() {
    for (line_no line_num = 0; line_num != lines_of_text.size(); line_num++) {
        istringstream line(lines_of_text[line_num]);  // ʹ�� stringstream ����ÿ��
        string word;
        while (line >> word) {  // ��ÿ�е�ÿ�����ʲ��� word_map
            word_map[word].insert(line_num);  // word_map �洢�������кŵĶ�Ӧ��ϵ
        }
    }
}

// ��ѯָ�����ʣ����ظõ��ʳ��ֵ������к�
set<TextQuery::line_no> TextQuery::run_query(const string& query_word) const {
    auto loc = word_map.find(query_word);  // ���ҵ����� map �е�λ��
    if (loc != word_map.end()) {
        return loc->second;  // ���ظõ��ʵ��кż���
    }
    return set<line_no>();  // ���û���ҵ������ؿռ���
}

// ��ȡָ���кŵ��ı�����
string TextQuery::text_line(line_no line) const {
    if (line < lines_of_text.size()) {
        return lines_of_text[line];  // ����ָ���кŵ�����
    }
    throw out_of_range("line number out of range");  // ����кų�����Χ���׳��쳣
}

// Windows �ļ�ѡ��Ի���
string open_file_dialog() {
    OPENFILENAME ofn;       // �ļ�ѡ���ṹ��
    wchar_t szFile[260];    // ʹ�ÿ��ַ����飨wchar_t��

    // ��ʼ�� OPENFILENAME �ṹ��
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
    ofn.lpstrFilter = L"Text Files\0*.TXT\0All Files\0*.*\0";  // ʹ�ÿ��ַ�����
    ofn.nFilterIndex = 1;
    ofn.lpstrFile[0] = L'\0';  // ��ʼ���ļ�·��Ϊ��
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = L"Select a text file";  // ʹ�ÿ��ַ�����
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = L"txt";  // ʹ�ÿ��ַ�����

    // ��ʾ�ļ�ѡ��Ի���
    if (GetOpenFileName(&ofn) == TRUE) {
        return string(szFile, szFile + wcslen(szFile));  // �����ַ�����ת��Ϊ std::string
    }
    return "";  // ���û��ѡ���ļ������ؿ��ַ���
}

// ���ɸ�����ʽ�ĵ��ʣ����ݵ��ʳ��ִ����жϣ�
string make_plural(size_t cnt, const string& word, const string& words) {
    return (cnt == 1) ? word : word + words;  // ������ִ���Ϊ1�����Ӹ�����ʽ��������ϸ�����ʽ�� "s"
}

// ��ӡ��ѯ���
void print_results(const set<TextQuery::line_no>& locs, const string& sought, const TextQuery& file) {
    typedef set<TextQuery::line_no> line_nums;  // �����кż�������
    line_nums::size_type size = locs.size();  // ��ȡ���ʳ��ֵ�����
    cout << sought << " occurs " << size << " " << make_plural(size, "time", "s") << endl;  // �����ѯ�ĵ��ʼ�����ִ���
    line_nums::const_iterator it = locs.begin();
    for (; it != locs.end(); ++it) {  // ���ÿ�е�����
        cout << "\t(line" << ((*it) + 1) << ") " << file.text_line(*it) << endl;
    }
}

// ������
int main() {
    string filename = open_file_dialog();  // ���ļ�ѡ��Ի��򣬻�ȡ�ļ�·��
    if (filename.empty()) {  // ���û��ѡ���ļ������������Ϣ
        cerr << "No file selected!" << endl;
        return EXIT_FAILURE;
    }

    wifstream infile(filename);  // ʹ�� wifstream ��ȡ�ļ�
    infile.imbue(locale("en_US.UTF-8"));  // �����ļ�Ϊ UTF-8 ����
    if (!infile) {  // ����ļ���ʧ�ܣ����������Ϣ
        cerr << "Failed to open file!" << endl;
        return EXIT_FAILURE;
    }

    TextQuery tq;  // ���� TextQuery ����
    tq.read_file(infile);  // ��ȡ�ļ�����

    while (true) {
        cout << "Enter word to look for, or q to quit: ";
        string s;
        cin >> s;  // ��ȡ�û�����Ĳ�ѯ����
        if (!cin || s == "q") break;  // ����û����� "q" �������������������˳�
        set<TextQuery::line_no> locs = tq.run_query(s);  // ���ҵ��ʳ��ֵ��к�
        print_results(locs, s, tq);  // ��ӡ��ѯ���
    }

    return 0;  // ���������˳�
}
