#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <unordered_set>
#include <algorithm>
#include <map>
#include <set>
using namespace std;
inline unordered_map<int, vector<int>> flip_map(unordered_map<int, vector<int>> map)
{
    unordered_map<int, vector<int>> flipped_map1;
    for (auto pair : map)
    {
        int key = pair.first;
        vector<int> values = pair.second;
        for (auto value : values)
        {
            flipped_map1[value].push_back(key);
        }
    }
    return flipped_map1;
}
inline bool inter(const vector<string> &lifetime1, const vector<string> &lifetime2)
{
    for (const auto &elem1 : lifetime1)
    {
        for (const auto &elem2 : lifetime2)
        {
            if (elem1 == elem2)
            {
                return true;
            }
        }
    }
    return false;
}
inline int ffl(vector<string> fields, int field_cnt, vector<int> field_width, int word_cnt, vector<int> word_width, vector<vector<int>> L)
{
    unordered_map<int, vector<int>> assignments;
    unordered_map<int, int> remaining_width;
    unordered_map<int, int> field2word;
    vector<pair<int, int>> non_conflict;
    vector<int> used_words;
    vector<int> used_fields;
    unordered_map<int, vector<int>> reuse_words;
    unordered_map<int, vector<int>> reuse_fields;

    for (int i = 0; i < word_cnt; i++)
    {
        assignments[i] = vector<int>();
        remaining_width[i] = word_width[i];
        reuse_words[i] = vector<int>();
    }

    for (int i = 0; i < field_cnt; i++)
    {
        for (int j = 0; j < field_cnt; j++)
        {
            if (L[i][j] >= 1)
            {
                non_conflict.push_back(make_pair(i, j));
            }
        }
    }

    for (int field = 0; field < field_cnt; field++)
    {
        int flag = 0;
        for (int k : used_fields)
        {
            if (find(non_conflict.begin(), non_conflict.end(), make_pair(k, field)) != non_conflict.end())
            {
                if (field_width[field] <= field_width[k])
                {
                    vector<int> tem_words = reuse_words[field2word[k]];
                    unordered_map<int, unordered_set<int>> tem_words_set;
                    for (int i = 0; i < tem_words.size(); i++)
                    {
                        for (int j = i; j < tem_words.size(); j++)
                        {
                            if (L[tem_words[i]][tem_words[j]] == 1)
                            {
                                tem_words_set[tem_words[i]].insert(tem_words[j]);
                                tem_words_set[tem_words[j]].insert(tem_words[i]);
                            }
                        }
                    }
                    vector<vector<int>> tem_words_list;
                    for (auto &entry : tem_words_set)
                    {
                        tem_words_list.push_back(vector<int>(entry.second.begin(), entry.second.end()));
                    }
                    int max_sum = 0;
                    for (auto &v : tem_words_list)
                    {
                        int sum = 0;
                        for (int i : v)
                        {
                            sum += field_width[i];
                        }
                        max_sum = max(max_sum, sum);
                    }
                    if (max_sum >= field_width[k])
                    {
                        continue;
                    }
                    reuse_words[field2word[k]].push_back(field);
                    assignments[field2word[k]].push_back(field);
                    flag = 1;
                    break;
                }
            }
        }
        if (flag == 1)
        {
            continue;
        }

        for (int word = 0; word < word_cnt; word++)
        {
            int word_w = remaining_width[word];
            if (field_width[field] <= word_w)
            {
                field2word[field] = word;
                assignments[word].push_back(field);
                remaining_width[word] -= field_width[field];
                used_words.push_back(word);
                used_fields.push_back(field);
                break;
            }
        }
    }
    unordered_map<int, vector<int>> ass_result;
    for (auto &entry : assignments)
    {
        if (!entry.second.empty())
        {
            ass_result[entry.first] = entry.second;
        }
    }

    int ffl_res = 0;
    for (auto &entry : ass_result)
    {
        ffl_res += word_width[entry.first];
    }

    cout << "Assignment Result: " << endl;
    for (const auto &pair : ass_result)
    {
        cout << pair.first << ": ";
        for (int field_index : pair.second)
        {
            cout << field_index << " ";
        }
        cout << endl;
    }

    unordered_map<int, vector<int>> filped_map = flip_map(ass_result);

    ofstream outputFile("heuristic_res.txt"); // 打开输出文件

    if (outputFile.is_open())
    { // 检查文件是否成功打开

        // 打印 Assignment Result 的内容
        outputFile << "Assignment Result: " << endl;
        for (auto &pair : filped_map)
        {
            outputFile << fields[pair.first] << " (" << field_width[pair.first]<<"bit" << ")"
                       << ": ";
            for (int word_index : pair.second)
            {
                outputFile << word_index << " (" << word_width[word_index] << "bit)";
            }
            outputFile << endl;
        }
        outputFile.close(); // 关闭文件
        cout << "内容已成功写入 heristic_res.txt 文件" << endl;
    }
    else
    {
        cerr << "无法打开文件" << endl;
    }

    return ffl_res;
}