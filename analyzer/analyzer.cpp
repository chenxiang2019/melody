#include <unordered_map>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;
class Analyzer
{
private:
    unordered_map<int, vector<string>> branchFileds_dict;
    unordered_map<string, vector<int>> lifetime_dist;
    vector<string> fields_list;

public:
    Analyzer(const unordered_map<int, vector<string>> &branchFileds_dict,
             const unordered_map<string, vector<int>> &lifetime_dist)
        : branchFileds_dict(branchFileds_dict), lifetime_dist(lifetime_dist) {}

    vector<pair<string, string>> return_combination()
    {
        vector<string> key_set;
        for (const auto &pair : branchFileds_dict)
        {
            const vector<string> &fields = pair.second;
            key_set.insert(key_set.end(), fields.begin(), fields.end());
        }
        sort(key_set.begin(), key_set.end());
        key_set.erase(unique(key_set.begin(), key_set.end()), key_set.end());

        vector<pair<string, string>> combination;
        for (size_t i = 0; i < key_set.size(); ++i)
        {
            for (size_t j = i + 1; j < key_set.size(); ++j)
            {
                combination.emplace_back(key_set[i], key_set[j]);
            }
        }
        return combination;
    }

    bool lifetime_analyzer(const string &field1, const string &field2)
    {
        const vector<int> &field1_lifetime = lifetime_dist[field1];
        const vector<int> &field2_lifetime = lifetime_dist[field2];

        int field1_start = field1_lifetime.front();
        int field1_end = field1_lifetime.back();
        int field2_start = field2_lifetime.front();
        int field2_end = field2_lifetime.back();

        if ((field1_start <= field2_start && field1_end >= field2_end) ||
            (field1_start >= field2_start && field1_end <= field2_end) ||
            (field1_start <= field2_start && field1_start >= field2_start) ||
            (field1_start <= field2_end && field1_end >= field2_end))
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    unordered_map<string, vector<int>> branchFields_to_filedsBranch()
    {
        unordered_map<string, vector<int>> filedsBranch_dict;
        for (const auto &pair : branchFileds_dict)
        {
            int branch = pair.first;
            const vector<string> &fields = pair.second;
            for (const string &field : fields)
            {
                filedsBranch_dict[field].push_back(branch);
            }
        }

        cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" << endl;
        cout << "branchFiledsDict:" << endl;
        for (const auto &entry : branchFileds_dict)
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
        for (const auto &entry : lifetime_dist)
        {
            cout << entry.first << ": ";
            for (const auto &value : entry.second)
            {
                cout << value << " ";
            }
            cout << endl;
        }

        cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" << endl;

        return filedsBranch_dict;
    }

    bool exclusive_analyzer(const string &field1, const string &field2)
    {
        unordered_map<string, vector<int>> filedsBranch_dict = branchFields_to_filedsBranch();
        const vector<int> &a = filedsBranch_dict[field1];
        const vector<int> &b = filedsBranch_dict[field2];

        return any_of(a.begin(), a.end(), [&](int branch)
                      { return find(b.begin(), b.end(), branch) != b.end(); });
    }

    vector<vector<int>> return_L()
    {
        unordered_map<string, vector<int>> filedsBranch_dict = branchFields_to_filedsBranch();
        cout << "<<<<<<<filedsBranch_dict>>>>>>" << endl;
        for (const auto &pair : filedsBranch_dict)
        {
            cout << pair.first << " ";
            for (const auto &field : pair.second)
            {
                cout << field << " ";
            }
            cout << endl; // 在每个键值对的末尾添加换行符
        }
        
        for (const auto &pair : filedsBranch_dict)
        {
            fields_list.push_back(pair.first);
        }
        sort(fields_list.begin(), fields_list.end());
        int keys = 0;
        cout << "LLLLLLLLLLLL----field_list-----LLLLLLLLLLLL~" << endl;
        for(auto item:fields_list){
            cout << keys << "    "<<item << endl; 
            keys++;
        }
        size_t length = fields_list.size();
        vector<vector<int>> L(length, vector<int>(length, 0));
        for (size_t i = 0; i < length; ++i)
        {
            for (size_t j = 0; j < length; ++j)
            {
                if (lifetime_analyzer(fields_list[i], fields_list[j]))
                {
                    L[i][j] = 1;
                }
            }
        }
        return L;
    }

    vector<vector<int>> return_E()
    {
        unordered_map<string, vector<int>> filedsBranch_dict = branchFields_to_filedsBranch();
        // vector<string> fields_list;
        // for (const auto &pair : filedsBranch_dict)
        // {
        //     fields_list.push_back(pair.first);
        // }
        // sort(fields_list.begin(), fields_list.end());
        // cout << "EEEEEEEEEEEE~~~~~~field_list~~EEEEEEEEEEEE" << endl;
        // int keys = 0;
        // for(auto item:fields_list){
        //     cout << keys << "    "<<item  << endl; 
        //     keys++;
        // }

        size_t length = fields_list.size();
        vector<vector<int>> E(length, vector<int>(length, 0));
        for (size_t i = 0; i < length; ++i)
        {
            for (size_t j = 0; j < length; ++j)
            {
                if (!exclusive_analyzer(fields_list[i], fields_list[j]))
                {
                    E[i][j] = 1;
                }
            }
        }
        return E;
    }
    vector<string> return_field_list(){
        return fields_list;
    }
};
