
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <regex>
#include <set>
#include <graphviz/gvc.h>
// #include <cgraph.h>
// #include <gvc.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

class Parser
{
    vector<vector<int>> branches_tables;

    vector<pair<int, int>> nodeID_of_edge;
    vector<string> edge_label;
    vector<int> ellipse_nodeID;
    vector<string> ellipse_node_label;
    vector<int> rectangle_nodeID;
    vector<string> rectangle_node_label;
    map<int, string> ellipse_nodeID_label_dict;
    map<int, string> rectangle_nodeID_label_dict;
    map<string, map<string, vector<string>>> controlBlock_table_keys_field_dict;
    // TO DO
    map<string, string> field_target_dict;
    // int last_refresh_branch_id;
    map<int, int> tables_in_which_branch;
    // TO DO
    map<string, int> field_length_mapper;
    map<string, int> All_field_length_mapper;
    map<string, int> field_length_for_opt;
    map<string, string> header_type_mapper;
    map<string, map<string, int>> table_actions_field_length_dict;

    int find_pre_node_id(vector<pair<int, int>> &nodeID_of_edge, int end_node_id)
    {
        for (const auto &edge : nodeID_of_edge)
        {
            // 检查pair.first是否以a结尾
            if (edge.second == end_node_id)
            {
                return edge.first;
            }
        }
    }

public:
    int last_refresh_branch_cnt = 0;
    //    Parser(vector<pair<int, int>> nodeID_of_edge, vector<string> edge_label,
    //           vector<int> ellipse_nodeID, vector<string> ellipse_node_label,
    //           vector<int> rectangle_nodeID, vector<string> rectangle_node_label,
    //           map<int, string> ellipse_nodeID_label_dict, map<int, string> rectangle_nodeID_label_dict,
    //           map<string, vector<string>> controlBlock_table_keys_field_dict) : nodeID_of_edge(nodeID_of_edge), edge_label(edge_label), ellipse_nodeID(ellipse_nodeID), ellipse_node_label(ellipse_node_label), rectangle_nodeID(rectangle_nodeID), rectangle_node_label(rectangle_node_label), ellipse_nodeID_label_dict(ellipse_nodeID_label_dict), rectangle_nodeID_label_dict(rectangle_nodeID_label_dict), controlBlock_table_keys_field_dict(controlBlock_table_keys_field_dict)
    //
    //    {
    //    }
    // Define the recursive function to find branch tables

    void recursion_find_branch_tables(int nodeid)
    {
        if (nodeid == 1)
            return;
        else if (find(rectangle_nodeID.begin(), rectangle_nodeID.end(), nodeid) != rectangle_nodeID.end())
        {
            for (size_t index = 0; index < nodeID_of_edge.size(); ++index)
            {
                if (nodeid == nodeID_of_edge[index].first)
                {
                    int end_node = nodeID_of_edge[index].second;
                    string label = edge_label[index];
                    if (end_node == 1)
                    {

                        return;
                    }

                    else if (find(ellipse_nodeID.begin(), ellipse_nodeID.end(), end_node) != ellipse_nodeID.end() && label == "TRUE")
                    {
                        branches_tables.push_back({end_node});
                        tables_in_which_branch[end_node] = last_refresh_branch_cnt;
                        last_refresh_branch_cnt++;
                        recursion_find_branch_tables(end_node);
                    }
                    else if (find(ellipse_nodeID.begin(), ellipse_nodeID.end(), end_node) != ellipse_nodeID.end() && label == "FALSE")
                    {
                        branches_tables.push_back({end_node});
                        tables_in_which_branch[end_node] = last_refresh_branch_cnt;
                        last_refresh_branch_cnt++;
                        recursion_find_branch_tables(end_node);
                    }
                    else if (find(rectangle_nodeID.begin(), rectangle_nodeID.end(), end_node) != rectangle_nodeID.end() && label == "FALSE")
                    {
                        recursion_find_branch_tables(end_node);
                    }
                }
                else if (nodeid < nodeID_of_edge[index].first)
                {
                    break;
                }
            }
        }
        else if (find(ellipse_nodeID.begin(), ellipse_nodeID.end(), nodeid) != ellipse_nodeID.end())
        {
            for (size_t index = 0; index < nodeID_of_edge.size(); ++index)
            {
                if (nodeid == nodeID_of_edge[index].first)
                {
                    int end_node = nodeID_of_edge[index].second;
                    string label = edge_label[index];
                    if (end_node == 1)
                    {
                        int pre_node_id = find_pre_node_id(nodeID_of_edge, nodeid);
                        branches_tables[tables_in_which_branch[pre_node_id]].push_back(nodeid);
                        tables_in_which_branch[nodeid] = tables_in_which_branch[pre_node_id];
                        return;
                    }
                    if (find(rectangle_nodeID.begin(), rectangle_nodeID.end(), end_node) != rectangle_nodeID.end() && label == "")
                    {
                        for (size_t i = 0; i < branches_tables.size(); ++i)
                        {
                            if (nodeID_of_edge[index].first == branches_tables[i][0] && nodeID_of_edge[index].first == nodeid)
                            {
                                recursion_find_branch_tables(end_node);
                            }
                        }
                    }
                    else if (find(ellipse_nodeID.begin(), ellipse_nodeID.end(), end_node) != ellipse_nodeID.end() && label == "")
                    {
                        for (size_t i = 0; i < branches_tables.size(); ++i)
                        {
                            if (find(branches_tables[i].begin(), branches_tables[i].end(), nodeid) != branches_tables[i].end() && nodeID_of_edge[index].first == nodeid)
                            {

                                branches_tables[i].push_back(end_node);
                                tables_in_which_branch[end_node] = i;
                                recursion_find_branch_tables(end_node);
                            }
                        }
                    }
                }
                else if (nodeid < nodeID_of_edge[index].first)
                {
                    break;
                }
            }
        }
        cout << "<<<<<<<<<<<<<<<<<<<<<<< branches_tables" << endl;
        for (size_t i = 0; i < branches_tables.size(); ++i)
        { // 外层循环遍历每个分支
            cout << "Branch " << i << ":" << endl;
            for (size_t j = 0; j < branches_tables[i].size(); ++j)
            {                                                  // 内层循环遍历分支中的每个整数
                cout << "  " << branches_tables[i][j] << endl; // 打印当前整数
            }
            cout << endl; // 在每个分支的末尾添加换行符
        }

        for (const auto &pair : tables_in_which_branch)
        {
            cout << "table id: " << pair.first << ", in branch: " << pair.second << endl;
        }
    }

    pair<unordered_map<string, vector<int>>, unordered_map<int, vector<string>>> return_dict()
    {

        vector<int> hash_nodeID_list(2 * nodeID_of_edge.size(), 0);
        for (auto i : nodeID_of_edge)
        {
            hash_nodeID_list[i.first] = 1;
            hash_nodeID_list[i.second] = 1;
        }

        int len_range_min = 1;
        int len_range_max = nodeID_of_edge.size();
        for (int i = 0; i < hash_nodeID_list.size(); i++)
        {
            if (hash_nodeID_list[i] == 1)
            {
                continue;
            }
            else
            {
                len_range_max = i;
                break;
            }
        }
        //---------------------------------------------------------------
        vector<int> in_vertex(len_range_max, 0);
        vector<int> out_vertex(len_range_max, 0);
        for (auto i : nodeID_of_edge)
        {
            in_vertex[i.second]++;
            out_vertex[i.first]++;
        }
        cout << "nodeID_of_edge:" << endl;
        for (const auto &p : nodeID_of_edge)
        {

            cout << "(" << p.first << ", " << p.second << ")" << endl;

            cout << endl;
        }
        int start = nodeID_of_edge[0].first;
        vector<vector<int>> nodeid_of_each_stage;
        vector<int> queue;
        int end_index = 0;
        int tmp_len = 0;
        int last_len = 0;
        while (true)
        {
            for (int index = 0; index < in_vertex.size(); index++)
            {
                if (in_vertex[index] == 0 && find(queue.begin(), queue.end(), index) == queue.end())
                {
                    queue.push_back(index);
                    tmp_len++;
                    end_index++;
                }
            }
            if (tmp_len == 0)
            {
                break;
            }
            nodeid_of_each_stage.push_back(vector<int>(queue.end() - tmp_len, queue.end()));
            for (auto pair : nodeID_of_edge)
            {
                if (find(queue.end() - tmp_len, queue.end(), pair.first) != queue.end())
                {
                    in_vertex[pair.second]--;
                }
            }
            tmp_len = 0;
        }

        map<int, string> tableID_label;
        for (int i = 0; i < ellipse_nodeID.size(); i++)
        {
            tableID_label[ellipse_nodeID[i]] = ellipse_node_label[i];
        }

        // vector<vector<int>> branches_tables;
        int start_nodeid = 0;
        for (auto i : nodeID_of_edge)
        {
            if (find(rectangle_nodeID.begin(), rectangle_nodeID.end(), i.first) != rectangle_nodeID.end() && rectangle_nodeID_label_dict[i.first] != "__START__")
            {
                start_nodeid = i.first;
            }
        }
        recursion_find_branch_tables(start_nodeid);

        vector<vector<string>> new_branches_tables;
        for (auto item1 : branches_tables)
        {
            vector<string> new_branches_tables_item;
            for (auto item2 : item1)
            {
                string name = ellipse_nodeID_label_dict[item2];
                new_branches_tables_item.push_back(name);
            }
            new_branches_tables.push_back(new_branches_tables_item);
        }
        // 打印new_branches_tables的内容
        cout << "///////////////////////////new_branches_tables" << endl;
        for (size_t i = 0; i < new_branches_tables.size(); ++i)
        { // 外层循环遍历每个分支
            cout << "Branch " << i << ":" << endl;
            for (const auto &table_name : new_branches_tables[i])
            { // 内层循环遍历分支中的每个表名
                cout << "  " << table_name << endl;
            }
            // 在每个分支的末尾添加额外的换行符以增加可读性
            cout << endl;
        }
        cout << "///////////////////////////branches_tables" << endl;
        for (size_t i = 0; i < branches_tables.size(); ++i)
        { // 外层循环遍历每个分支
            cout << "Branch " << i << ":" << endl;
            for (size_t j = 0; j < branches_tables[i].size(); ++j)
            {                                                  // 内层循环遍历分支中的每个整数
                cout << "  " << branches_tables[i][j] << endl; // 打印当前整数
            }
            cout << endl; // 在每个分支的末尾添加换行符
        }

        vector<int> pop_index_list;
        for (int i = 0; i < new_branches_tables.size(); i++)
        {
            for (int j = i + 1; j < new_branches_tables.size(); j++)
            {
                if (new_branches_tables[i] == new_branches_tables[j])
                {
                    if (find(pop_index_list.begin(), pop_index_list.end(), j) == pop_index_list.end())
                    {
                        pop_index_list.push_back(j);
                    }
                }
            }
        }

        vector<vector<int>> branches_tables_after_handle;
        for (int i = 0; i < branches_tables.size(); i++)
        {
            if (find(pop_index_list.begin(), pop_index_list.end(), i) == pop_index_list.end())
            {
                branches_tables_after_handle.push_back(branches_tables[i]);
            }
        }

        cout << "Printing branches_tables_after_handle:" << endl;
        for (size_t i = 0; i < branches_tables_after_handle.size(); ++i)
        { // 外层循环遍历每个分支
            cout << "Branch " << i << ":" << endl;
            for (const auto &value : branches_tables_after_handle[i])
            { // 内层循环遍历分支中的每个整数
                cout << value << " ";
            }
            cout << endl; // 在每个分支的末尾添加换行符
        }

        vector<pair<int, int>> dependency_between_branches;
        for (int i = 0; i < branches_tables_after_handle.size(); i++)
        {
            for (int j = i + 1; j < branches_tables_after_handle.size(); j++)
            {
                for (auto item1 : branches_tables_after_handle[i])
                {
                    for (auto item2 : branches_tables_after_handle[j])
                    {
                        if (has_dependency(item1, item2, nodeID_of_edge))
                        {
                            if (find(dependency_between_branches.begin(), dependency_between_branches.end(), make_pair(i, j)) == dependency_between_branches.end())
                            {
                                dependency_between_branches.push_back(make_pair(i, j));
                                break;
                            }
                        }
                    }
                }
            }
        }

        vector<int> new_division(branches_tables_after_handle.size());
        iota(new_division.begin(), new_division.end(), 0);
        for (int i = dependency_between_branches.size() - 1; i >= 0; i--)
        {
            new_division[dependency_between_branches[i].second] = new_division[dependency_between_branches[i].first];
        }

        unordered_set<int> unique_values(new_division.begin(), new_division.end());
        vector<vector<int>> unique_values_index_list;
        for (auto i : unique_values)
        {
            vector<int> tmp_list;
            for (int index = 0; index < new_division.size(); index++)
            {
                if (new_division[index] == i)
                {
                    tmp_list.push_back(index);
                }
            }
            unique_values_index_list.push_back(tmp_list);
        }

        vector<vector<int>> new_branches_tables_after_handle;
        for (auto item : unique_values_index_list)
        {
            vector<int> tmp_list;
            for (auto i : item)
            {
                tmp_list.insert(tmp_list.end(), branches_tables_after_handle[i].begin(), branches_tables_after_handle[i].end());
            }
            new_branches_tables_after_handle.push_back(tmp_list);
        }

        set<int> node_in_branches;
        set<int> node_notIn_branches;
        // 打印new_branches_tables_after_handle的内容
        cout << "Printing new_branches_tables_after_handle:" << endl;
        for (size_t i = 0; i < new_branches_tables_after_handle.size(); ++i)
        { // 外层循环遍历每个分支
            cout << "Branch " << i << ":" << endl;
            for (const auto &table : new_branches_tables_after_handle[i])
            { // 内层循环遍历分支中的每个表
                cout << table << " ";
                node_in_branches.insert(table);
            }
            cout << endl; // 在每个分支的末尾添加换行符
        }

        int key_id = 0;

        unordered_map<int, vector<int>> branchTables_dict;
        for (auto item : new_branches_tables_after_handle)
        {
            branchTables_dict[key_id] = new_branches_tables_after_handle[key_id];
            key_id++;
        }
        // 补充公共主干节点到所有分支中
        // 先看哪些节点属于主干--没出现在任何分支中的椭圆节点

        for (const auto &pair : ellipse_nodeID_label_dict)
        {
            if (node_in_branches.find(pair.first) == node_in_branches.end() && pair.second != "")
            {
                node_notIn_branches.insert(pair.first);
            }
        }
        for (auto item1 : branchTables_dict)
        {
            for (auto item2 : node_notIn_branches)
            {
                branchTables_dict[item1.first].push_back(item2);
            }
        }

        unordered_map<int, vector<string>> branchFields_dict;
        cout << "Printing branchTables_dict:" << endl;
        for (const auto &pair : branchTables_dict)
        {
            cout << "Key: " << pair.first << ", Tables: ";
            for (const auto &table : pair.second)
            {
                cout << table << " ";
            }
            cout << endl; // 在每个键值对的末尾添加换行符
        }
        for (auto it = branchTables_dict.begin(); it != branchTables_dict.end(); it++)
        {
            vector<string> tmp_list;
            cout << "******************ellipse_nodeID_label_dict *****************" << endl;
            cout << ellipse_nodeID_label_dict[it->first] << endl;
            // 打印ellipse_nodeID_label_dict的内容
            cout << "Printing ellipse_nodeID_label_dict:" << endl;
            for (const auto &pair : ellipse_nodeID_label_dict)
            {
                cout << "Node ID: " << pair.first << ", Label: " << pair.second << endl;
            }

            vector<string> field_list;
            cout << "tttttttttttttttttcontrolBloctttttttttttttttttttttttt" << endl;
            for (const auto &controlBlock_pair : controlBlock_table_keys_field_dict)
            {
                cout << "Control Block: " << controlBlock_pair.first << endl;
                for (const auto &table_pair : controlBlock_pair.second)
                {
                    cout << "  Table: " << table_pair.first << ", Fields: ";
                    for (const auto &field : table_pair.second)
                    {
                        cout << field << " ";
                    }
                    cout << endl;
                }
            }
            cout << "tttttttttttttttttcontrolBloctttttttttttttttttttttttt" << endl;
            for (auto item : branchTables_dict[it->first])
            {
                // auto outer_map = controlBlock_table_keys_field_dict["ingress"];
                cout << "[[[[[[[[[[[[[[[[[[[[[[[[**************]]]]]]]]]]]]]]]]]]]]]]]]" << endl;
                auto outer_map = controlBlock_table_keys_field_dict["ingress"];
                cout << item << endl;
                cout << "Printing ellipse_nodeID_label_dict:" << endl;
                for (const auto &pair : ellipse_nodeID_label_dict)
                {
                    cout << "Node ID: " << pair.first << ", Label: " << pair.second << endl;
                }
                vector<string> tmp = outer_map[ellipse_nodeID_label_dict[item]];
                cout << "%%%%%%%%%%%%%%%%%%%%%%%" << endl;
                for (auto &item : tmp)
                {
                    cout << item << endl;
                }
                cout << "%%%%%%%%%%%%%%%%%%%%%%%" << endl;
                field_list.insert(field_list.end(), tmp.begin(), tmp.end());
            }

            // 打印field_list的内容
            cout << it->first << endl;

            cout << "******************field list *****************" << endl;
            for (const auto &field : field_list)
            {
                cout << field << endl; // 打印当前字符串，并在每个字符串后添加换行符
            }
            for (auto j : field_list)
            {
                if (find(tmp_list.begin(), tmp_list.end(), j) == tmp_list.end())
                {
                    tmp_list.push_back(j);
                }
            }
            branchFields_dict[it->first] = tmp_list;
        }
        cout << "Printing branchFields_dict:" << endl;
        for (const auto &pair : branchFields_dict)
        {
            cout << "Key: " << pair.first << ", Values: ";
            for (const auto &value : pair.second)
            {
                cout << value << " ";
            }
            cout << endl;
        }

        vector<vector<string>> table_of_each_stage;
        vector<string> tables_list;
        for (auto item : nodeid_of_each_stage)
        {
            vector<string> table_list;
            for (auto i : item)
            {
                if (find(ellipse_nodeID.begin(), ellipse_nodeID.end(), i) != ellipse_nodeID.end())
                {
                    table_list.push_back(ellipse_nodeID_label_dict[i]);
                    tables_list.push_back(ellipse_nodeID_label_dict[i]);
                }
            }
            table_of_each_stage.push_back(table_list);
        }
        // 打印table_of_each_stage的内容
        cout << "Printing table_of_each_stage:" << endl;
        for (size_t i = 0; i < table_of_each_stage.size(); ++i)
        { // 外层循环遍历每个阶段
            cout << "Stage " << i << ":" << endl;
            for (const auto &table_name : table_of_each_stage[i])
            { // 内层循环遍历该阶段中的每个表名
                cout << "  " << table_name << endl;
            }
            cout << endl; // 在每个阶段的末尾添加额外的换行符以增加可读性
        }

        unordered_map<string, vector<int>> tables_lifetime_dict;
        unordered_map<string, vector<int>> fields_lifetime_dict;
        vector<string> fields_list;
        for (auto i : tables_list)
        {
            tables_lifetime_dict[i] = vector<int>();
            fields_list.insert(fields_list.end(), controlBlock_table_keys_field_dict["ingress"][i].begin(), controlBlock_table_keys_field_dict["ingress"][i].end());
        }
        for (auto i : fields_list)
        {
            fields_lifetime_dict[i] = vector<int>();
        }
        for (int index = 0; index < nodeid_of_each_stage.size(); index++)
        {
            for (auto item : nodeid_of_each_stage[index])
            {
                if (find(ellipse_nodeID.begin(), ellipse_nodeID.end(), item) != ellipse_nodeID.end())
                {
                    string node_name = ellipse_nodeID_label_dict[item];
                    for (auto field : controlBlock_table_keys_field_dict["ingress"][node_name])
                    {
                        if (find(fields_lifetime_dict[field].begin(), fields_lifetime_dict[field].end(), index) == fields_lifetime_dict[field].end())
                        {
                            fields_lifetime_dict[field].push_back(index);
                        }
                    }
                }
            }
        }

        return make_pair(fields_lifetime_dict, branchFields_dict);
    }

    pair<string, json> read_files(const string &dot_file, const string &json_file)
    {
        ifstream file(dot_file);
        if (!file.is_open())
        {
            cerr << "Failed to open DOT file." << endl;
            exit(1);
        }

        string dot_content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

        file.close();

        json json_info;
        ifstream json_fs(json_file);
        if (!json_fs.is_open())
        {
            cerr << "Error: Unable to open JSON file." << endl;
            exit(EXIT_FAILURE);
        }
        json_fs >> json_info;

        return make_pair(dot_content, json_info);
    }

    tuple<vector<pair<int, int>>, vector<string>, vector<int>, map<int, string>, vector<string>, vector<int>, vector<string>, map<int, string>> parse_dot_file(string dot_content)
    {

        vector<string> lines;
        vector<string> select;
        vector<string> node;
        // vector<int> ellipse_nodeID;
        vector<string> ellipse_node;
        // vector<string> ellipse_node_label;
        // map<int, string> ellipse_nodeID_label_dict;
        // vector<int> rectangle_nodeID;
        vector<string> rectangle_node;
        // vector<string> rectangle_node_label;
        // map<int, string> rectangle_nodeID_label_dict;
        //  nodeID_of_edge;
        // vector<string> edge_label;
        vector<string> edge;
        // Splitting dot_content into lines
        size_t pos = 0;
        while ((pos = dot_content.find("\n")) != string::npos)
        {
            lines.push_back(dot_content.substr(0, pos));
            dot_content.erase(0, pos + 1);
        }

        // Selecting lines starting with numbers
        for (const auto &line : lines)
        {
            if (line[0] >= '0' && line[0] <= '9')
            {
                select.push_back(line);
            }
        }

        // Filtering nodes and edges
        for (const auto &line : select)
        {
            if (line[1] == '[' || line[2] == '[' || line[3] == '[')
            {
                node.push_back(line);
            }
            else
            {
                edge.push_back(line);
            }
        }

        // Processing ellipse nodes
        for (size_t index = 0; index < node.size(); ++index)
        {
            const auto &x = node[index];
            for (int i = x.size() - 3; i >= 2; --i)
            {
                if (x[i] == ',')
                {
                    for (int j = i - 1; j >= 1; --j)
                    {
                        if (x[j] == 'e' && x[j - 2] == 'a' && x[j - 4] == 's')
                        {
                            if (x.substr(j + 2, i - j - 2) == "ellipse")
                            {
                                ellipse_nodeID.push_back(index);
                                ellipse_node.push_back(x);
                                break;
                            }
                        }
                    }
                }
            }
        }

        // Extracting labels for ellipse nodes
        for (size_t index = 0; index < ellipse_node.size(); ++index)
        {
            const auto &x = ellipse_node[index];
            size_t start = 0, end = 0;
            for (size_t i = 6; i < x.size(); ++i)
            {
                if (x[i - 2] == 'e' && x[i - 1] == 'l' && x[i] == '=')
                {
                    start = (x[i + 1] == '"') ? i + 2 : i + 1;
                    for (size_t j = start; j < x.size(); ++j)
                    {
                        if (x[j] != ',' && x[j + 1] == ',')
                        {
                            end = (x[j] != '"') ? j + 1 : j;
                            ellipse_node_label.push_back(x.substr(start, end - start));
                            break;
                        }
                    }
                }
            }
        }

        // Mapping ellipse node IDs to labels
        for (size_t i = 0; i < ellipse_nodeID.size(); ++i)
        {
            ellipse_nodeID_label_dict[ellipse_nodeID[i]] = ellipse_node_label[i];
        }

        // Processing rectangle nodes
        for (size_t index = 0; index < node.size(); ++index)
        {
            const auto &x = node[index];
            for (int i = x.size() - 3; i >= 2; --i)
            {
                if (x[i] == ',')
                {
                    for (int j = i - 1; j >= 1; --j)
                    {
                        if (x[j] == 'e' && x[j - 2] == 'a' && x[j - 4] == 's')
                        {
                            if (x.substr(j + 2, i - j - 2) == "rectangle")
                            {
                                rectangle_nodeID.push_back(index);
                                rectangle_node.push_back(x);
                                break;
                            }
                        }
                    }
                }
            }
        }

        // Extracting labels for rectangle nodes
        for (size_t index = 0; index < rectangle_node.size(); ++index)
        {
            const auto &x = rectangle_node[index];
            size_t start = 0, end = 0;
            for (size_t i = 6; i < x.size(); ++i)
            {
                if (x[i - 2] == 'e' && x[i - 1] == 'l' && x[i] == '=')
                {
                    start = (x[i + 1] == '"') ? i + 2 : i + 1;
                    for (size_t j = start; j < x.size(); ++j)
                    {
                        if (x[j] != ',' && x[j + 1] == ',')
                        {
                            end = (x[j] != '"') ? j + 1 : j;
                            rectangle_node_label.push_back(x.substr(start, end - start));
                            break;
                        }
                    }
                }
            }
        }

        // Mapping rectangle node IDs to labels
        for (size_t i = 0; i < rectangle_nodeID.size(); ++i)
        {
            rectangle_nodeID_label_dict[rectangle_nodeID[i]] = rectangle_node_label[i];
        }

        // Extracting edge information
        for (const auto &x : edge)
        {
            int starID, endID;
            size_t i = 2;
            while (i < x.size() && !(x[i] == '-' && x[i + 1] == '>'))
            {
                ++i;
            }
            starID = stoi(x.substr(0, i - 1));
            size_t j = i + 2;
            while (j < x.size() && x[j] != '[')
            {
                ++j;
            }
            endID = stoi(x.substr(i + 2, j - i - 2));
            nodeID_of_edge.push_back({starID, endID});
        }

        // Extracting edge labels
        for (const auto &x : edge)
        {
            for (size_t i = 2; i < x.length(); ++i)
            {
                if (x[i] == '=')
                {
                    for (size_t j = i + 1; j < x.length(); ++j)
                    {
                        if (x[j] == ']')
                        {
                            if (x[j - 1] == '"')
                            {
                                edge_label.push_back(x.substr(i + 2, j - i - 3));
                            }
                            else
                            {
                                edge_label.push_back(x.substr(i + 1, j - i - 1));
                            }
                            break;
                        }
                    }
                }
            }
        }
        cout << "nodeID_of_edge:" << endl;
        for (const auto &edge : nodeID_of_edge)
        {
            cout << "(" << edge.first << ", " << edge.second << ")" << endl;
        }

        cout << "edge_label:" << endl;
        for (const auto &label : edge_label)
        {
            cout << label << endl;
        }

        cout << "ellipse_nodeID:" << endl;
        for (const auto &id : ellipse_nodeID)
        {
            cout << id << endl;
        }

        cout << "ellipse_nodeID_label_dict:" << endl;
        for (const auto &pair : ellipse_nodeID_label_dict)
        {
            cout << "Node ID: " << pair.first << ", Label: " << pair.second << endl;
        }

        cout << "ellipse_node_label:" << endl;
        for (const auto &label : ellipse_node_label)
        {
            cout << label << endl;
        }

        cout << "rectangle_nodeID:" << endl;
        for (const auto &id : rectangle_nodeID)
        {
            cout << id << endl;
        }

        cout << "rectangle_node_label:" << endl;
        for (const auto &label : rectangle_node_label)
        {
            cout << label << endl;
        }

        cout << "rectangle_nodeID_label_dict:" << endl;
        for (const auto &pair : rectangle_nodeID_label_dict)
        {
            cout << "Node ID: " << pair.first << ", Label: " << pair.second << endl;
        }

        return make_tuple(nodeID_of_edge, edge_label, ellipse_nodeID, ellipse_nodeID_label_dict, ellipse_node_label, rectangle_nodeID, rectangle_node_label, rectangle_nodeID_label_dict);
    }
    // 对metadata的field name进行处理
    // 也对普通的field_name加上属性前缀
    std::string processString(const std::string &str)
    {
        // 替换 "scalars.userMetadata" 为 "meta"
        std::string processedStr = std::regex_replace(str, std::regex("scalars\\.userMetadata"), "meta");

        // 移除 "ingress_metadata" 前的下划线并在后面加点
        size_t ingressMetadataPos = processedStr.find("_ingress_metadata_");
        if (ingressMetadataPos != std::string::npos)
        {
            processedStr.erase(ingressMetadataPos, 1);                                                   // 移除前面的下划线
            processedStr.replace(ingressMetadataPos + std::string("ingress_metadata").length(), 1, "."); // 替换后面的下划线为点
        }

        // 移除末尾数字
        size_t lastDigitPos = processedStr.find_last_not_of("0123456789");
        if (lastDigitPos != std::string::npos)
        {
            if (lastDigitPos > 0 && processedStr[lastDigitPos] == '5' && processedStr[lastDigitPos - 1] == '4')
            {
                // 如果倒数第二个数字是4且最后一个数字是5，则只去掉最后一个5
                processedStr.erase(lastDigitPos);
            }
            else
            {
                // 否则去掉末尾的数字
                processedStr.erase(lastDigitPos + 1);
            }
        }

        // 替换 "_ipv45" 为 "_ipv4"
        processedStr = std::regex_replace(processedStr, std::regex("_ipv45"), "_ipv4");

        return processedStr;
    }

    string field_modify(string name)
    {
        if (name.size() >= 20 && name.substr(0, 20) == "scalars.userMetadata")
        {
            // scalars.userMetadata._ingress_metadata_flow_ipg0
            string res = processString(name);
            if (res.substr(res.size() - 3, res.size()) == "ipv")
            {
                return res + "4";
            }
            else
            {
                return res;
            }
        }
        else if (name.size() >= 17 && name.substr(0, 17) == "standard_metadata")
        {
            return name;
        }
        else
        {
            return "hdr." + name;
        }
    }
    void extract(string table_name, string name, const json &data)
    {

        if (data.contains("type") && data["type"] == "expression")
        {
            extract(table_name, name, data["value"]);
        }
        else
        {
            if (data.contains("left") && data.contains("right"))
            {
                extract(table_name, name, data["left"]);
                extract(table_name, name, data["right"]);
            }
            else
            {
                // cout << data["value"] << data["type"] << endl;
                if (data["type"] == "field")
                {
                    string field_name = "";
                    for (auto &value_split : data["value"])
                    {
                        field_name += string(value_split);
                        field_name += '.';
                    }
                    field_name.erase(field_name.end() - 1);
                    // TO DO
                    if (field_name.substr(0, 7) != "scalars" || (field_name.size() >= 20 && field_name.substr(0, 20) == "scalars.userMetadata"))
                    {
                        
                        if(field_name.size()<17 || field_name.size() >=17 && field_name.substr(0,17)!="standard_metadata"){
                            string field_name_after_modify = field_modify(field_name);
                            if(field_name_after_modify == "hdr.ethernet.dstAddr" || field_name_after_modify=="hdr.ethernet.srcAddr"){
                                string field_name_1 = field_name_after_modify+"_1"; 
                                string field_name_2 = field_name_after_modify+"_2";
                                // action_field_length[name][field_name_1] = 32;
                                // action_field_length[name][field_name_2] = 16;
                                table_actions_field_length_dict[table_name][field_name_1] = 32;
                                table_actions_field_length_dict[table_name][field_name_2] = 16;
                            }
                            else{
                                table_actions_field_length_dict[table_name][field_name_after_modify] = All_field_length_mapper[field_name];
                            }
                            
                        }
                       
                    }
                }
            }
        }
    }
    map<string, int> parse_json_and_return_AllTable_actions_field_length_dict(json json_info)
    {
        json header_types_fields = json_info["header_types"];
        json headers_mapper_json = json_info["headers"];
        map<string, int> field_length_mapper_1;

        for (auto head_mapper_json : headers_mapper_json)
        {
            header_type_mapper[head_mapper_json["header_type"]] = head_mapper_json["name"];
        }
        for (auto &head_type : header_types_fields)
        {
            string name = header_type_mapper[head_type["name"]];

            auto head_type_field_list = head_type["fields"];

            // for (tuple<string, int, bool> &field_item : head_type_field_list)
            for (auto &field_item : head_type_field_list)
            {

                string tmp_name = name + '.' + string(field_item[0]);
                if (tmp_name.substr(0, 7) != "scalars" || (tmp_name.size() >= 20 && tmp_name.substr(0, 20) == "scalars.userMetadata"))
                {
                    if(tmp_name.size()<17 || tmp_name.size()>=17 && tmp_name.substr(0,17)!="standard_metadata"){
                            string field_name_after_modify = field_modify(tmp_name);
                            if(field_name_after_modify == "hdr.ethernet.dstAddr" || field_name_after_modify=="hdr.ethernet.srcAddr"){
                                string field_name_1 = field_name_after_modify+"_1"; 
                                string field_name_2 = field_name_after_modify+"_2";
                                // action_field_length[name][field_name_1] = 32;
                                // action_field_length[name][field_name_2] = 16;
                                field_length_mapper_1[field_name_1] = 32;
                                field_length_mapper_1[field_name_2] = 16;
                            }
                            else{
                                field_length_mapper_1[field_name_after_modify] = field_item[1];
                            }
                    }
                   
                }
            }
        }
        int standard_metadata_length = 384;
        // for (auto &item : json_info["header_types"][1]["fields"])
        // {
        //     standard_metadata_length+=
        // }
        field_length_mapper_1["standard_metadata"] = standard_metadata_length;
        for (const auto &pair : field_length_mapper_1)
        {
            cout << "键: " << pair.first << ", 值: " << pair.second << endl;
        }
        return field_length_mapper_1;
    }
    void parse_json_and_return_OneTable_actions_field_length_dict(string table_name, vector<string> actions, json json_info)
    {
        json list_actions_json_origin = json_info["actions"];
        map<string, map<string, int>> action_field_length;
        vector<json> list_actions_json;
        for (auto &tmp : list_actions_json_origin)
        {
            if (find(actions.begin(), actions.end(), string(tmp["name"])) != actions.end())
            {
                list_actions_json.push_back(tmp);
            }
        }

        for (auto &action : list_actions_json)
        {
            string name = action["name"];

            // 取消临时变量纳入field了
            //  json runtime_datas = action["runtime_data"];
            //  for (auto &runtime_data : runtime_datas)
            //  {
            //    action_field_length[name][runtime_data["name"]] = runtime_data["bitwidth"];
            //  }
            //  extract(json_info_item);
            json primatives = action["primitives"];
            for (auto &primative : primatives)
            {
                if (primative["op"] == "assign")
                {
                    json parameters = primative["parameters"];
                    for (auto &parameter : parameters)
                    {
                        if (parameter["type"] == "field")
                        {
                            string field_name = "";
                            for (auto &value_split : parameter["value"])
                            {
                                field_name += string(value_split);
                                field_name += '.';
                            }
                            field_name.erase(field_name.end() - 1);
                            if (field_name.substr(0, 7) != "scalars" || (field_name.size() >= 20 && field_name.substr(0, 20) == "scalars.userMetadata"))
                            {
                                if(field_name.size()<17 || field_name.size() >=17 && field_name.substr(0,17)!="standard_metadata"){
                                    string field_name_after_modify = field_modify(field_name);
                                    if(field_name_after_modify == "hdr.ethernet.dstAddr" || field_name_after_modify=="hdr.ethernet.srcAddr"){
                                        string field_name_1 = field_name_after_modify+"_1"; 
                                        string field_name_2 = field_name_after_modify+"_2";
                                        // action_field_length[name][field_name_1] = 32;
                                        // action_field_length[name][field_name_2] = 16;
                                        table_actions_field_length_dict[table_name][field_name_1] = 32;
                                        table_actions_field_length_dict[table_name][field_name_2] = 16;
                                    }
                                    else{
                                        table_actions_field_length_dict[table_name][field_name_after_modify] = All_field_length_mapper[field_name];
                                    }
                                }
                            
                            }
                                
                            
                            cout << "222" << endl;
                        }
                        else if (parameter["type"] == "expression")
                        {
                            extract(table_name, name, parameter);
                            cout << "111" << endl;
                        }
                    }
                }
                else if (primative["op"] == "mark_to_drop")
                {
                    json parameters = primative["parameters"];
                    for (auto &parameter : parameters)
                    {
                        if (parameter["type"] == "header")
                        {
                            string field_name = "";
                            for (auto &value_split : parameter["value"])
                            {
                                field_name += string(value_split);
                                field_name += '.';
                            }
                            field_name.erase(field_name.end() - 1);
                            if (field_name.substr(0, 7) != "scalars" || (field_name.size() >= 20 && field_name.substr(0, 20) == "scalars.userMetadata"))
                            {

                                if(field_name.size()<17 || field_name.size() >=17 && field_name.substr(0,17)!="standard_metadata"){
                                    string field_name_after_modify = field_modify(field_name);
                                    if(field_name_after_modify.size()>0 && (field_name_after_modify == "hdr.ethernet.dstAddr" || field_name_after_modify=="hdr.ethernet.srcAddr")){
                                        string field_name_1 = field_name_after_modify+"_1"; 
                                        string field_name_2 = field_name_after_modify+"_2";
                                        // action_field_length[name][field_name_1] = 32;
                                        // action_field_length[name][field_name_2] = 16;
                                        table_actions_field_length_dict[table_name][field_name_1] = 32;
                                        table_actions_field_length_dict[table_name][field_name_2] = 16;
                                    }
                                    else{
                                        table_actions_field_length_dict[table_name][field_name_after_modify] = All_field_length_mapper[field_name];
                                    }
                                }
                               
                            }
                            cout << "333" << endl;
                        }
                    }
                }
                // else if (primative["op"] == "register_read")
                // {
                // }
            }
        }

        cout << "~~~~~~~~~~~~~~~~~~^^^^^^^^^^^^^~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
        cout << table_name << "Tables Action Field Length" << endl;

        for (const auto &action : action_field_length)
        {
            cout << "Action: " << action.first << endl;
            for (const auto &field_length : action.second)
            {
                cout << "  Field: " << field_length.first << ", Length: " << field_length.second << endl;
            }
        }

        for (const auto &table_fields : table_actions_field_length_dict)
        {
            cout << "table name: " << table_fields.first << "actions field length dict" << endl;
            for (const auto &field_length : table_fields.second)
            {
                cout << "  Field: " << field_length.first << ", Length: " << field_length.second << endl;
            }
        }
        cout << "~~~~~~~~~~~~~~~~~~^^^^^^^^^^^^^~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    }
    std::vector<std::string> mergeAndRemoveDuplicates(const std::vector<std::string> &vec1, const std::vector<std::string> &vec2)
    {
        // 创建一个新的向量，容量为两个输入向量的总和
        std::vector<std::string> mergedVec(vec1.size() + vec2.size());

        // 使用std::merge将两个向量合并到新的向量中
        auto it = std::merge(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(), mergedVec.begin());

        // 使用std::sort和std::unique去除重复元素
        std::sort(mergedVec.begin(), it);
        auto last = std::unique(mergedVec.begin(), it);
        mergedVec.erase(last, mergedVec.end());

        return mergedVec;
    }

    tuple<map<string, map<string, vector<string>>>, map<string, string>, map<string, int>> parse_json(json json_info)
    {
        map<string, map<string, vector<string>>> controlBlock_table_keys_field_dict_1;
        map<string, string> field_target_dict_1;
        map<string, int> field_length_mapper_1;
        json pipelines = json_info["pipelines"];
        map<string, int> field_length_mapper_2;
        All_field_length_mapper = parse_json_and_return_AllTable_actions_field_length_dict(json_info);
        cout << "全体的field 长度如下" << endl;
        for (auto &tmp_mapper : All_field_length_mapper)
        {
            cout << tmp_mapper.first << ":" << tmp_mapper.second << endl;
        }
        for (const auto &pipeline : pipelines)
        {
            string controlBlock_name = pipeline["source_info"]["source_fragment"];
            map<string, vector<string>> tmp_table_actions_dict;

            map<string, vector<string>> tmp_table_keys_dict;
            map<string, vector<string>> tmp_table_actions_field_dict;
            map<string, vector<string>> tmp_table_all_fields_dict;

            for (const auto &table_item : pipeline["tables"])
            {
                string table_name = table_item["name"];
                vector<string> keys_field_name;
                vector<string> action_names;
                for (auto &action : table_item["actions"])
                {
                    if (action != "NoAction")
                    {
                        tmp_table_actions_dict[table_name].push_back(action);
                        action_names.push_back(action);
                    }
                }
                parse_json_and_return_OneTable_actions_field_length_dict(table_name, action_names, json_info);

                for (const auto &key_item : table_item["key"])
                {
                    string key_name = key_item["name"];
                    // 获取field length
                    string target_name = key_item["target"][1];
                    field_target_dict_1[key_name] = target_name;
                    keys_field_name.push_back(key_name);
                }

                tmp_table_keys_dict[table_name] = keys_field_name;
                // 下面来按table收集actions和keys中的field

                auto tmp_map_taf = table_actions_field_length_dict[table_name];
                for (auto &taf : tmp_map_taf)
                {
                    tmp_table_actions_field_dict[table_name].push_back(taf.first);
                }
                // 合并两部分并去重，保序
                vector<string> mergerVec = mergeAndRemoveDuplicates(tmp_table_actions_field_dict[table_name], tmp_table_keys_dict[table_name]);
                tmp_table_all_fields_dict[table_name] = mergerVec;
            }
            cout << "---------------------------After merge---------------------------" << endl;
            cout << "---------------------------After merge---------------------------" << endl;
            cout << "---------------------------After merge---------------------------" << endl;
            for (const auto &pair : tmp_table_all_fields_dict)
            {
                std::cout << "table: " << pair.first << std::endl;
                std::cout << "fields: ";

                // 遍历每个键对应的 vector
                for (const std::string &value : pair.second)
                {
                    std::cout << value << " ";
                }

                std::cout << std::endl;
            }

            // controlBlock_table_keys_field_dict_1[controlBlock_name] = tmp_table_keys_dict + tmp_table_action_field_dict;
            // table_actions_field_length_dict[table_name]
            controlBlock_table_keys_field_dict_1[controlBlock_name] = tmp_table_all_fields_dict;
        }
        cout << "$$$$$$$$$$$$$$$$$$$$controlBlock$$$$$$$$$$$$$$$$$$$$ " << endl;
        for (const auto &controlBlock_pair : controlBlock_table_keys_field_dict_1)
        {
            cout << "Control Block: " << controlBlock_pair.first << endl;
            for (const auto &table_pair : controlBlock_pair.second)
            {
                cout << "  Table: " << table_pair.first << ", Fields: ";
                for (const auto &field : table_pair.second)
                {
                    cout << field << " ";
                }
                cout << endl;
            }
        }
        // for head_type in header_types:
        //     head_type_field_list = head_type['fields']
        //     for field_item in head_type_field_list:
        //         field_length_mapper[field_item[0]] = field_item[1]

        // 打印 field_target_dict_1 的内容
        cout << "field_target_dict_1:" << endl;
        for (const auto &pair : field_target_dict_1)
        {
            cout << pair.first << " -> " << pair.second << endl;
        }
        cout << endl; // 打印一个空行以分隔两个map的输出

        // 打印 field_length_mapper_1 的内容
        cout << "field_length_mapper_1:" << endl;
        for (const auto &pair : field_length_mapper_1)
        {
            cout << pair.first << " -> " << pair.second << endl;
        }

        return make_tuple(controlBlock_table_keys_field_dict_1, field_target_dict_1, field_length_mapper_1);
    }

    bool has_dependency(int index1, int index2, vector<pair<int, int>> &nodeID_of_edge)
    {
        vector<int> stack;

        for (const auto &item_edge : nodeID_of_edge)
        {
            if (item_edge.second == index2)
            {
                return true;
            }
            else if (item_edge.first == index1)
            {
                stack.push_back(item_edge.second);
            }
        }

        while (!stack.empty())
        {
            for (int i = 0; i < stack.size(); i++)
            {
                for (const auto &item_edge : nodeID_of_edge)
                {
                    if (item_edge.second == index2)
                    {
                        return true;
                    }
                    else if (stack[i] == item_edge.first)
                    {
                        stack.push_back(item_edge.second);
                    }
                }
                stack.erase(stack.begin() + i);
            }
        }

        return false;
    }
    tuple<unordered_map<int, vector<string>>, unordered_map<string, vector<int>>, map<string, int>> run_parser(const string &dot_file, const string &json_file)
    {
        auto files_content = read_files(dot_file, json_file);
        string dot_content = files_content.first;
        json json_info;
        json_info = files_content.second;
        // 调用 parse_dot_file 函数并接收返回值
        auto result = parse_dot_file(dot_content);

        // 解构返回值
        nodeID_of_edge = get<0>(result);
        auto &edge_label = get<1>(result);
        auto &ellipse_nodeID = get<2>(result);
        auto &ellipse_nodeID_label_dict = get<3>(result);
        auto &ellipse_node_label = get<4>(result);
        auto &rectangle_nodeID = get<5>(result);
        auto &rectangle_node_label = get<6>(result);
        auto &rectangle_nodeID_label_dict = get<7>(result);

        cout << "------------------------------------------" << endl;

        cout << "nodeID_of_edge:" << endl;
        for (const auto &edge : nodeID_of_edge)
        {
            cout << "(" << edge.first << ", " << edge.second << ")" << endl;
        }

        cout << "edge_label:" << endl;
        for (const auto &label : edge_label)
        {
            cout << label << endl;
        }

        cout << "ellipse_nodeID:" << endl;
        for (const auto &id : ellipse_nodeID)
        {
            cout << id << endl;
        }

        cout << "ellipse_nodeID_label_dict:" << endl;
        for (const auto &pair : ellipse_nodeID_label_dict)
        {
            cout << "Node ID: " << pair.first << ", Label: " << pair.second << endl;
        }

        cout << "ellipse_node_label:" << endl;
        for (const auto &label : ellipse_node_label)
        {
            cout << label << endl;
        }

        cout << "rectangle_nodeID:" << endl;
        for (const auto &id : rectangle_nodeID)
        {
            cout << id << endl;
        }

        cout << "rectangle_node_label:" << endl;
        for (const auto &label : rectangle_node_label)
        {
            cout << label << endl;
        }

        cout << "rectangle_nodeID_label_dict:" << endl;
        for (const auto &pair : rectangle_nodeID_label_dict)
        {
            cout << "Node ID: " << pair.first << ", Label: " << pair.second << endl;
        }
        auto res_of_json = parse_json(json_info);
        controlBlock_table_keys_field_dict = get<0>(res_of_json);
        field_target_dict = get<1>(res_of_json);
        field_length_mapper = get<2>(res_of_json);

        auto parsed_results = return_dict();
        auto branchFields_dict = parsed_results.second;
        auto fileds_lifetime_dict = parsed_results.first;
        vector<string> fields_for_opt;
        // for (auto &item : fileds_lifetime_dict)
        // {
        //     fields_for_opt.push_back(item.first);
        // }
        // for (auto &item : fields_for_opt)
        // {
        //     field_length_for_opt[item] = field_length_mapper[field_target_dict[item]];
        // }
        // 打印输出结果
        cout << "^^^^^^^^^^^^^^Fields Lifetime Dictionary:^^^^^^^^^^^^^^" << endl;
        for (const auto &entry : fileds_lifetime_dict)
        {
            cout << entry.first << ": ";
            for (const auto &value : entry.second)
            {
                cout << value << " ";
            }
            cout << endl;
        }

        cout << "Branch Fields Dictionary:" << endl;
        for (const auto &entry : branchFields_dict)
        {
            cout << entry.first << ": ";
            for (const auto &value : entry.second)
            {
                cout << value << endl;
            }
            cout << "---------------------" << endl;
        }

        return make_tuple(branchFields_dict, fileds_lifetime_dict, All_field_length_mapper);
    }
};
