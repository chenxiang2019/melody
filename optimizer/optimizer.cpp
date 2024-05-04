#include "gurobi_c++.h"
#include <vector>
#include <fstream>
#include <ctime>

using namespace std;
inline pair<double, double> melody(vector<string> fields, int field_cnt, vector<int> field_width, int word_cnt, vector<int> word_width, vector<vector<int>> L, vector<vector<int>> E)
{
    GRBEnv env = GRBEnv();
    GRBModel model = GRBModel(env);
    vector<GRBVar> x;
    for (int i = 0; i < field_cnt; i++)
    {
        for (int j = 0; j < word_cnt; j++)
        {
            x.push_back(model.addVar(0.0, 1.0, 0.0, GRB_BINARY));
        }
    }
    cout << "1" << endl;
    vector<GRBVar> y;
    for (int j = 0; j < word_cnt; j++)
    {
        y.push_back(model.addVar(0.0, 1.0, 0.0, GRB_BINARY));
    }

    vector<vector<GRBVar>> z;
    for (int i = 0; i < field_cnt; i++)
    {
        vector<GRBVar> z_row;
        for (int j = 0; j < word_cnt; j++)
        {
            z_row.push_back(model.addVar(0.0, GRB_INFINITY, 0.0, GRB_INTEGER));
        }
        z.push_back(z_row);
    }
    cout << "2" << endl;
    vector<GRBVar> max_field_width;
    for (int j = 0; j < word_cnt; j++)
    {
        max_field_width.push_back(model.addVar(0.0, GRB_INFINITY, 0.0, GRB_INTEGER));
    }

    GRBVar obj = model.addVar(0.0, 255.0, 0.0, GRB_INTEGER);

    model.set(GRB_IntParam_SolutionLimit, 100);
    model.set(GRB_IntParam_OutputFlag, 0);
    // x与z的关系，也就是x与o的关系
    for (int i = 0; i < field_cnt; i++)
    {
        for (int j = 0; j < word_cnt; j++)
        {
            // 这种z[i][j]和x[i * word_cnt + j]与python版的m.addConstr(  (  x[i, j] == 1 ) >> (z[i, j] >= 1 )  )有什么区别

            // ，C++ 中的 x[i * word_cnt + j] 对应于 Python 中的 x[i, j]。这是因为在 C++ 中，x 是一个一维数组，你需要通过索引 i * word_cnt + j 来访问对应 (i, j) 位置的元素，
            // 而在 Python 中，x 是一个二维数组，你可以直接使用 x[i, j] 来访问。
            model.addGenConstrIndicator(x[i * word_cnt + j], true, z[i][j] >= 1);  // x 为 1 时，z >= 1
            model.addGenConstrIndicator(x[i * word_cnt + j], false, z[i][j] == 0); // x 为 0 时，z == 0
        }
    }
    cout << "公式3" << endl;
    for (int i = 0; i < field_cnt; i++)
    {
        GRBLinExpr expr = 0;
        for (int j = 0; j < word_cnt; j++)
        {
            expr += z[i][j];
        }
        model.addConstr(expr == field_width[i]);
    }
    cout << "互斥关系" << endl;
    for (int i = 0; i < field_cnt; i++)
    {
        for (int j = 0; j < field_cnt; j++)
        {
            if (L[i][j] == 0 && E[i][j] == 0 && i < j)
            {
                for (int k = 0; k < word_cnt; k++)
                {
                    // cout << "5" << endl;
                    model.addGenConstrIndicator(x[i * word_cnt + k], true, x[j * word_cnt + k] == 0);
                    model.addGenConstrIndicator(x[j * word_cnt + k], true, x[i * word_cnt + k] == 0);
                    // cout << i << " " << j << " " << k << endl;
                }
            }
        }
    }
    cout << "公式4" << endl;
    for (int j = 0; j < word_cnt; j++)
    {
        GRBLinExpr expr;
        for (int i = 0; i < field_cnt; i++)
        {
            expr += z[i][j];
        }
        model.addConstr(max_field_width[j] >= expr);
        // model.addGenConstrMax(max_field_width[j], expr);
    }
    cout << "宽度约束" << endl;
    for (int j = 0; j < word_cnt; j++)
    {
        model.addConstr(max_field_width[j] <= word_width[j]);
    }
    cout << "通过x求y" << endl;
    for (int j = 0; j < word_cnt; j++)
    {
        vector<GRBVar> vars;
        for (int i = 0; i < field_cnt; i++)
        { //??????????????????这里位置存疑
            // vars.push_back(x[j * word_cnt + i]);
            vars.push_back(x[i * word_cnt + j]);
        }

        GRBVar y_j_or = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "y_j_or");
        model.addConstr(y_j_or >= vars[0]);
        for (int i = 1; i < vars.size(); i++)
        {
            model.addConstr(y_j_or >= vars[i]);
        }

        model.addConstr(y[j] == y_j_or);
    }
    cout << "11" << endl;
    // for (int j = 0; j < word_cnt; j++)
    // {
    //     vector<GRBVar> or_vars;
    //     for (int i = 0; i < field_cnt; i++)
    //     {
    //         or_vars.push_back(x[j * word_cnt + i]);
    //     }
    //     model.addGenConstrOr(y[j], or_vars);
    // }
//添加约束,一对一映射
    for (int i = 0; i < field_cnt; ++i) {
        GRBLinExpr expr = 0.0;
        for (int j = 0; j < word_cnt; ++j) {
            expr += x[i * word_cnt + j];
        }
        model.addConstr(expr == 1);
    }

    GRBLinExpr obj_expr = 0;
    cout << "优化的目标式" << endl;
    for (int j = 0; j < word_cnt; j++)
    {
        obj_expr += y[j] * word_width[j];
    }







    model.setObjective(obj_expr, GRB_MINIMIZE);

    model.optimize();
    int status = model.get(GRB_IntAttr_Status);
    cout << "14" << endl;

    if ((status == GRB_INF_OR_UNBD) || (status == GRB_INFEASIBLE) ||
        (status == GRB_UNBOUNDED))
    {
        cout << "The model cannot be solved because it is "
             << "infeasible or unbounded" << endl;
    }

    if (status != GRB_OPTIMAL)
    {
        cout << "Optimization was stopped with status " << status << endl;
    }
    auto res = make_pair(obj.get(GRB_DoubleAttr_X), model.get(GRB_DoubleAttr_Runtime) * 1000);
    // auto res = make_pair(model.get(GRB_DoubleAttr_X), model.get(GRB_DoubleAttr_Runtime) * 1000);
    int index = 0;
    int line_index = 0;
    vector<int> notZero_value_of_a_line;
    ofstream outputFile("ilp_res.txt"); // 打开输出文件

    if (outputFile.is_open())
    { // 检查文件是否成功打开

        // 打印内容并写入文件
        for (auto &item : x)
        {
            // outputFile << item.get(GRB_DoubleAttr_X);
            if (item.get(GRB_DoubleAttr_X) == 1)
            {
                notZero_value_of_a_line.push_back(index + 1);
            }
            index++;
            if (index % word_cnt == 0)
            {
                outputFile << endl;
                outputFile << " " << fields[line_index] << " (" << field_width[line_index] << "bit) : ";
                for (auto item2 : notZero_value_of_a_line)
                {
                    outputFile << item2 << " (" << word_width[item2-1] << "bit)"
                               << " ";
                }
                notZero_value_of_a_line.clear();
                line_index++;
                // outputFile << endl;
                index = 0;
            }
        }

        outputFile.close(); // 关闭文件
        cout << "内容已成功写入 ilp_res.txt 文件" << endl;
    }
    else
    {
        cerr << "无法打开文件" << endl;
    }

    cout << " " << endl;
    cout << "15" << endl;
    return res;
}
