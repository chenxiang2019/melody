#include "analyzer/analyzer.cpp"
#include "analyzer/parser.cpp"
#include "optimizer/optimizer.cpp"
#include "optimizer/heuristic.cpp"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;
int main(int argc,char* argv[])
{
    // string dot_file_dir = "2.dot";
    // string json_file_dir = "2.json";
    string dot_file_dir  = argv[1];
    string json_file_dir = argv[2];
    string phv_file = argv[3];//"analyzer/phv.txt"
    Parser parser;
    auto result = parser.run_parser(dot_file_dir, json_file_dir);
    unordered_map<int, vector<string>> branchFiledsDict = get<0>(result);
    unordered_map<string, vector<int>> lifetimeDist = get<1>(result);
    map<string, int> fields_length_dict = get<2>(result);
    // 打印 branchFiledsDict
    cout << "branchFiledsDict:" << endl;
    for (const auto &entry : branchFiledsDict)
    {
        cout << entry.first << ": ";
        for (const auto &value : entry.second)
        {
            cout << value << " ";
        }
        cout << endl;
    }

    // 打印 lifetimeDist
    cout << "lifetimeDist:" << endl;
    for (const auto &entry : lifetimeDist)
    {
        cout << entry.first << ": ";
        for (const auto &value : entry.second)
        {
            cout << value << " ";
        }
        cout << endl;
    }

    // 初始化参数
    // int field_cnt = branchFiledsDict.size();
    // int field_cnt = 3;
    // vector<int> field_width = {32, 16, 32};
    // int word_cnt = 5;
    // vector<int> word_width = {16, 16, 16, 16, 16};

    // 创建 analyzer 对象并调用相关方法
    Analyzer defineAnalyzer(branchFiledsDict, lifetimeDist);
    vector<vector<int>> L = defineAnalyzer.return_L();
    vector<vector<int>> E = defineAnalyzer.return_E();
    vector<string> fields_list = defineAnalyzer.return_field_list();
    int field_cnt = fields_list.size();
    vector<int> field_width;
    for (auto field : fields_list)
    {
        field_width.push_back(fields_length_dict[field]);
    }
    ifstream file(phv_file); // 打开文件
    vector<int> cnt;                   // 用于存储每行的第一个数
    vector<int> word_width;            // 用于存储每行的第二个数

    if (file.is_open())
    { // 检查文件是否成功打开
        string line;
        while (getline(file, line))
        { // 逐行读取文件内容
            istringstream iss(line);
            int num1, num2;
            if (iss >> num1 >> num2)
            {                        // 解析每行的两个数
                cnt.push_back(num1); // 将第一个数存入 cnt
                for (int i = 0; i < num1; i++)
                {
                    word_width.push_back(num2); // 将第二个数存入 word_width
                }
            }
        }
        file.close(); // 关闭文件
    }
    else
    {
        cerr << "无法打开文件" << endl;
        return 1;
    }
    int word_cnt = 0;
    for (int i = 0; i < cnt.size(); i++)
    {
        word_cnt += cnt[i];
    }
    cout << "   *************fields_list*************  " << endl;
    int index = 0;
    for (auto item : fields_list)
    {
        cout << index << "      " << item << endl;
        index++;
    }
    // 打印 L 的内容
    cout << "((((((((((((((((((((((L))))))))))))))))))))))" << endl;
    for (size_t i = 0; i < L.size(); ++i)
    { // 外层循环遍历每一行
        for (size_t j = 0; j < L[i].size(); ++j)
        {                           // 内层循环遍历每一行中的每个元素
            cout << L[i][j] << " "; // 打印当前元素，后面跟一个空格
        }
        cout << endl; // 每打印完一行后换行
    }
    // 打印 L 的内容
    cout << "((((((((((((((((((((((E))))))))))))))))))))))" << endl;
    for (size_t i = 0; i < E.size(); ++i)
    { // 外层循环遍历每一行
        for (size_t j = 0; j < E[i].size(); ++j)
        {                           // 内层循环遍历每一行中的每个元素
            cout << E[i][j] << " "; // 打印当前元素，后面跟一个空格
        }
        cout << endl; // 每打印完一行后换行
    }
    // 调用 melody 和ffl函数
    // 调用 melody 函数并获取返回对象
    int ffl_res = ffl(fields_list, field_cnt, field_width, word_cnt, word_width, L);
    pair<double, double> returnObject = melody(fields_list, field_cnt, field_width, word_cnt, word_width, L, E);

    for (auto item : field_width)
    {
        cout << item << ",";
    }
    cout << endl;
    for (auto item : word_width)
    {
        cout << item << ",";
    }
    cout << "L" << endl;
    for (auto item1 : L)
    {
        cout << "[";
        for (auto item2 : item1)
        {
            cout << item2 << ",";
        }
        cout << "]" << endl;
    }
    cout << "E" << endl;
    for (auto item1 : E)
    {
        cout << "[";
        for (auto item2 : item1)
        {
            cout << item2 << ",";
        }
        cout << "]" << endl;
    }
    cout << endl;
    // 提取返回对象的第一个元素并打印
    double returnObjectXVec = returnObject.first;
    printf("%lf\n", returnObjectXVec);

    // 提取返回对象的第二个元素并打印
    double returnTime = returnObject.second;
    printf("%lf\n", returnTime);

    // 输出结果
    cout << ffl_res << endl;
    return 0;
}
