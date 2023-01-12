#include <stdio.h>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include <time.h>
using namespace std;

class constraint
{
protected:
    // 该约束所涉及的变量
    vector<int> elements;

public:
    /**
     * @brief 判断该约束是否满足
     * 功能：在判断约束是否满足的同时维护 remaining_value，即：某个未赋值变量因该约束可选取的值发生变化时
     *       要对 remianing_value 进行相应的修改
     * @param assignment 当前各个变量赋值的情况
     * @param remaining_value 未赋值变量的剩余值
     * @return int -1 当前还无法判断是否满足
     *              0 不满足
     *              1 满足
     */
    virtual int is_true(std::map<int, bool> assignment, map<pair<int, int>, set<bool>> &remaining_value) = 0;

    constraint(vector<int> list)
    {
        this->elements = list;
    }
};

// 约束 : 一周至少休息两天
class rest_days : public constraint
{
public:
    rest_days(vector<int> list) : constraint(list) {}
    int is_true(std::map<int, bool> assignment, map<pair<int, int>, set<bool>> &remaining_value) override
    {
        vector<int> assign_true, assign_false, not_assign;
        for (auto it : elements)
        {
            if (assignment.find(it) != assignment.end())
            {
                if (assignment[it])
                    assign_true.push_back(it);
                else
                    assign_false.push_back(it);
            }
            else
                not_assign.push_back(it);
        }
        if (assign_true.size() > 5)
            return 0;
        if (assign_true.size() == 5)
        {
            for (auto it : not_assign)
            {
                if (remaining_value.find(make_pair(2, it)) != remaining_value.end())
                {
                    remaining_value.erase(make_pair(2, it));
                    remaining_value.insert(make_pair(make_pair(1, it), set<bool>{false}));
                }
                else
                {
                    if (remaining_value.find(make_pair(1, it))->second.find(false) == remaining_value.find(make_pair(1, it))->second.end())
                    {
                        remaining_value.erase(make_pair(1, it));
                        remaining_value.insert(make_pair(make_pair(0, it), set<bool>{}));
                    }
                }
            }
        }
        if (assign_false.size() >= 2)
            return 1;
        else
            return -1;
    }
};

// 约束 : 不能连续休息三天
class continous_rest_days : public constraint
{
public:
    continous_rest_days(vector<int> list) : constraint(list) {}
    int is_true(std::map<int, bool> assignment, map<pair<int, int>, set<bool>> &remaining_value) override
    {
        vector<int> assign_true, assign_false, not_assign;
        for (auto it : elements)
        {
            if (assignment.find(it) != assignment.end())
            {
                if (assignment[it])
                    assign_true.push_back(it);
                else
                    assign_false.push_back(it);
            }
            else
                not_assign.push_back(it);
        }
        if (assign_true.size() > 0)
            return 1;
        if (assign_false.size() == 2)
        {
            if (not_assign.size() == 1)
            {
                if (remaining_value.find(make_pair(2, not_assign[0])) != remaining_value.end())
                {
                    remaining_value.erase(make_pair(2, not_assign[0]));
                    remaining_value.insert(make_pair(make_pair(1, not_assign[0]), set<bool>{true}));
                }
                else
                {
                    if (remaining_value.find(make_pair(1, not_assign[0]))->second.find(true) == remaining_value.find(make_pair(1, not_assign[0]))->second.end())
                    {
                        remaining_value.erase(make_pair(1, not_assign[0]));
                        remaining_value.insert(make_pair(make_pair(0, not_assign[0]), set<bool>{}));
                    }
                }
            }
        }
        if (assign_false.size() == 3)
            return 0;
        else
            return -1;
    }
};

// 约束 : 一天最少需要 number_people_on_duty 个工人工作
class people_on_duty : public constraint
{
private:
    int number_people_on_duty;

public:
    people_on_duty(vector<int> list, int number) : constraint(list)
    {
        this->number_people_on_duty = number;
    }
    int is_true(std::map<int, bool> assignment, map<pair<int, int>, set<bool>> &remaining_value) override
    {
        vector<int> assign_true, assign_false, not_assign;
        for (auto it : elements)
        {
            if (assignment.find(it) != assignment.end())
            {
                if (assignment[it])
                    assign_true.push_back(it);
                else
                    assign_false.push_back(it);
            }
            else
                not_assign.push_back(it);
        }
        if (assign_true.size() >= number_people_on_duty)
            return 1;
        if (assign_false.size() > (int)elements.size() - number_people_on_duty)
        {
            for (auto it : not_assign)
            {
                if (remaining_value.find(make_pair(2, it)) != remaining_value.end())
                {
                    remaining_value.erase(make_pair(2, it));
                    remaining_value.insert(make_pair(make_pair(1, it), set<bool>{true}));
                }
                else
                {
                    if (remaining_value.find(make_pair(1, it))->second.find(true) == remaining_value.find(make_pair(1, it))->second.end())
                    {
                        remaining_value.erase(make_pair(1, it));
                        remaining_value.insert(make_pair(make_pair(0, it), set<bool>{}));
                    }
                }
            }
        }
        if (assign_false.size() > (int)elements.size() - number_people_on_duty)
            return 0;
        else
            return -1;
    }
};

// 约束 : 一天最少需要一名 senior 级别的工人
class senior_on_duty : public constraint
{
public:
    senior_on_duty(vector<int> list) : constraint(list) {}
    int is_true(std::map<int, bool> assignment, map<pair<int, int>, set<bool>> &remaining_value) override
    {
        vector<int> assign_true, assign_false, not_assign;
        for (auto it : elements)
        {
            if (assignment.find(it) != assignment.end())
            {
                if (assignment[it])
                    assign_true.push_back(it);
                else
                    assign_false.push_back(it);
            }
            else
                not_assign.push_back(it);
        }
        if (assign_true.size() > 0)
            return 1;
        if (assign_false.size() == elements.size() - 1)
        {
            if (remaining_value.find(make_pair(2, not_assign[0])) != remaining_value.end())
            {
                remaining_value.erase(make_pair(2, not_assign[0]));
                remaining_value.insert(make_pair(make_pair(1, not_assign[0]), set<bool>{true}));
            }
            else
            {
                if (remaining_value.find(make_pair(1, not_assign[0]))->second.find(true) == remaining_value.find(make_pair(1, not_assign[0]))->second.end())
                {
                    remaining_value.erase(make_pair(1, not_assign[0]));
                    remaining_value.insert(make_pair(make_pair(0, not_assign[0]), set<bool>{}));
                }
            }
        }
        if (assign_false.size() == elements.size())
            return 0;
        else
            return -1;
    }
};

class not_together_constraint : public constraint
{
public:
    not_together_constraint(vector<int> list) : constraint(list){};
    int is_true(std::map<int, bool> assignment, map<pair<int, int>, set<bool>> &remaining_value) override
    {
        vector<int> assign_true, assign_false, not_assign;
        for (auto it : elements)
        {
            if (assignment.find(it) != assignment.end())
            {
                if (assignment[it])
                    assign_true.push_back(it);
                else
                    assign_false.push_back(it);
            }
            else
                not_assign.push_back(it);
        }
        if (assign_true.size() == 2)
            return 0;
        if (assign_false.size() == 2)
            return 1;
        if (assign_true.size() == 1)
        {
            if (not_assign.size() == 1)
            {
                if (remaining_value.find(make_pair(2, not_assign[0])) != remaining_value.end())
                {
                    remaining_value.erase(make_pair(2, not_assign[0]));
                    remaining_value.insert(make_pair(make_pair(1, not_assign[0]), set<bool>{false}));
                }
                else
                {
                    if (remaining_value.find(make_pair(1, not_assign[0]))->second.find(false) == remaining_value.find(make_pair(1, not_assign[0]))->second.end())
                    {
                        remaining_value.erase(make_pair(1, not_assign[0]));
                        remaining_value.insert(make_pair(make_pair(0, not_assign[0]), set<bool>{}));
                    }
                }
            }
        }
        if (assign_true.size() == 1 && assign_false.size() == 1)
            return 1;
        else
            return -1;
    }
};

/**
 * @brief 判断当前赋值 assignment 是否完整（所有变量均被赋值且满足所有约束）
 * 
 * @param assignment 当前赋值
 * @param csp 约束集合
 * @param remaining_value 剩余值 
 * @return int -1 还有约束无法判断是否满足
 *              0 不满足
 *              1 满足
 */
int is_complete(map<int, bool> assignment, vector<constraint *> csp, map<pair<int, int>, set<bool>> &remaining_value)
{
    int flag = 1;
    for (auto constr : csp)
    {
        if (constr->is_true(assignment, remaining_value) == 0)
            return 0;
        if (constr->is_true(assignment, remaining_value) == -1)
            flag = -1;
    }
    return flag;
}

/**
 * @brief 某个变量被赋值后，判断该变量相关的约束是否满足
 * 
 * @param assignment 当前赋值
 * @param csp 该变量所涉及的所有约束
 * @param remaining_value 剩余值
 * @return true 满足
 * @return false 不满足
 */
bool is_consistent(map<int, bool> assignment, vector<constraint *> csp, map<pair<int, int>, set<bool>> &remaining_value)
{
    for (auto constr : csp)
    {
        if (constr->is_true(assignment, remaining_value) == 0)
            return false;
    }
    return true;
}

/**
 * @brief 约束传播，如果还有变量的剩余值减少，则继续传播
 * 
 * @param assignment 当前赋值
 * @param value_to_csp 各个变量所对应的各自的约束（当有变量值确定时，并不需要检测所有约束，只需要检测该变量所涉及的约束）
 * @param remaining_value 剩余值
 * @param remaining_1 只剩一个剩余值的变量集合（返回值）
 * @return true 约束传播过程中未发现错误
 * @return false 约束传播过程中发现错误
 */
bool constraint_propagation(std::map<int, bool> &assignment,
                            map<int, vector<constraint *>> value_to_csp,
                            map<pair<int, int>, set<bool>> &remaining_value,
                            vector<int> &remaining_1)
{
    bool flag = false;
    while (!flag)
    {
        flag = true;
        vector<int> temp = {};
        while (!remaining_value.empty() && remaining_value.begin()->first.first != 2)
        {
            flag = false;
            temp.push_back(remaining_value.begin()->first.second);
            remaining_1.push_back(remaining_value.begin()->first.second);
            assignment.insert(make_pair(remaining_value.begin()->first.second, *remaining_value.begin()->second.begin()));
            remaining_value.erase(remaining_value.begin());
        }
        if (flag)
            break;
        for (auto it : temp)
        {
            if (is_consistent(assignment, value_to_csp[it], remaining_value) == 0)
                return false;
        }
    }
    return true;
}

/**
 * @brief 在当前赋值和剩余值下，接着选取一个变量进行赋值，如果成功则返回；否则，回退！
 * 
 * @param assignment 当前赋值
 * @param value_to_csp 各个变量对应的约束
 * @param remaining_value 剩余值
 * @param csp 所有约束
 * @param count 记录变量赋值次数
 * @return true 成功
 * @return false 失败，回退！
 */
bool backtracking_search(std::map<int, bool> &assignment,
                         map<int, vector<constraint *>> value_to_csp,
                         map<pair<int, int>, set<bool>> &remaining_value,
                         vector<constraint *> csp, int &count)
{
    if (is_complete(assignment, csp, remaining_value) == 1)
        return true;
    if (is_complete(assignment, csp, remaining_value) == 0)
        return false;
    if (remaining_value.begin()->first.first == 0)
        return false;
    vector<int> remaining_1;
    auto var = remaining_value.begin();
    pair<pair<int, int>, set<bool>> variable = make_pair(var->first, var->second);
    remaining_value.erase(remaining_value.begin());
    for (auto it : variable.second)
    {
        count++;
        assignment.insert(make_pair(variable.first.second, it));
        if (is_consistent(assignment, value_to_csp[variable.first.second], remaining_value))
        {
            if (!remaining_value.empty() && remaining_value.begin()->first.first == 0)
                assignment.erase(variable.first.second);
            else
            {
                if (constraint_propagation(assignment, value_to_csp, remaining_value, remaining_1))
                {
                    if (backtracking_search(assignment, value_to_csp, remaining_value, csp, count) == true)
                        return true;
                }
                for (auto it : remaining_1)
                {
                    assignment.erase(it);
                    remaining_value.insert(make_pair(make_pair(2, it), set<bool>{false, true}));
                }
                assignment.erase(variable.first.second);
            }
        }
        else
            assignment.erase(variable.first.second);
    }

    // 回退，各个变量剩余值需要恢复
    while (!remaining_value.empty() && remaining_value.begin()->first.first != 2)
    {
        int it = remaining_value.begin()->first.second;
        remaining_value.erase(remaining_value.begin());
        remaining_value.insert(make_pair(make_pair(2, it), set<bool>{false, true}));
    }
    remaining_value.insert(make_pair(make_pair(2, variable.first.second), set<bool>{false, true}));
    return false;
}

int main()
{
    /* 输入格式：
       line 1 : N , number of workers
       line 2 : N 个正整数, 0 : junior, 1 : senior
       line 3 : num_people_on_duty, 一天最少工作人数
       line 4 : n, 不想再同一天工作的组数
       line 5 ~ n + 4, 不想再同一天工作的所有组合
    */
    int N; // number of workers
    int i = 0, j = 0, k = 0;
    // 一天最少工作人数
    int num_people_on_duty;
    vector<int> junior, senior;

    map<int, bool> assignment;

    scanf("%d", &N);

    map<pair<int, int>, set<bool>> remaining_value;
    for (i = 1; i <= N * 7; i++)
        remaining_value.insert(make_pair(make_pair(2, i), set<bool>{false, true}));

    map<int, vector<constraint *>> value_to_csp;
    for (i = 1; i <= N * 7; i++)
        value_to_csp.insert(make_pair(i, vector<constraint *>{}));
    vector<constraint *> csp;

    for (i = 0; i < N; i++)
    {
        int worker_type; // 0 : junior, 1 : senior
        scanf("%d", &worker_type);
        if (worker_type == 0)
            junior.push_back(i + 1);
        else
            senior.push_back(i + 1);
    }
    scanf("%d", &num_people_on_duty);

    int n; // 不想再同一天工作的组数
    vector<pair<int, int>> not_together;
    scanf("%d", &n);
    for (i = 0; i < n; i++)
    {
        int a, b;
        scanf("%d%d", &a, &b);
        not_together.push_back(make_pair(a, b));
    }

    for (i = 1; i <= N; i++)
    {
        vector<int> list;
        for (j = 0; j < 7; j++)
            list.push_back(i + j * N);
        rest_days *new_rest_days = new rest_days(list);
        for (j = 0; j < 7; j++)
            value_to_csp[i + j * N].push_back(new_rest_days);
        csp.push_back(new_rest_days);
    }

    for (i = 1; i <= N; i++)
    {
        for (j = 0; j < 5; j++)
        {
            vector<int> list = {i + j * N, i + (j + 1) * N, i + (j + 2) * N};
            continous_rest_days *new_continous_rest_days = new continous_rest_days(list);
            for (k = 0; k < 3; k++)
                value_to_csp[i + (j + k) * N].push_back(new_continous_rest_days);
            csp.push_back(new_continous_rest_days);
        }
    }

    for (i = 0; i < 7; i++)
    {
        vector<int> list;
        for (j = 1; j <= N; j++)
            list.push_back(j + i * N);
        people_on_duty *new_people_on_duty = new people_on_duty(list, num_people_on_duty);
        for (j = 1; j <= N; j++)
            value_to_csp[j + i * N].push_back(new_people_on_duty);
        csp.push_back(new_people_on_duty);
    }

    for (i = 0; i < 7; i++)
    {
        vector<int> list;
        for (auto it : senior)
            list.push_back(it + i * N);
        senior_on_duty *new_senior_on_duty = new senior_on_duty(list);
        for (auto it : senior)
            value_to_csp[it + i * N].push_back(new_senior_on_duty);
        csp.push_back(new_senior_on_duty);
    }

    for (i = 0; i < 7; i++)
    {
        for (auto it : not_together)
        {
            vector<int> list = {it.first + i * N, it.second + i * N};
            not_together_constraint *new_not_together = new not_together_constraint(list);
            value_to_csp[it.first + i * N].push_back(new_not_together);
            value_to_csp[it.second + i * N].push_back(new_not_together);
            csp.push_back(new_not_together);
        }
    }

    int count = 0;
    // clock_t start_time = clock();
    if (backtracking_search(assignment, value_to_csp, remaining_value, csp, count) == true)
    {
        for (i = 0; i < 7; i++)
        {
            for (j = 0; j < N; j++)
            {
                if (assignment[j + 1 + i * N])
                    printf("%d ", j + 1);
            }
            printf("\n");
        }
    }
    else
        printf("error!\n");

    // clock_t end_time = clock();
    // printf("%lfs\n", (double)(end_time - start_time) / CLOCKS_PER_SEC);
    // printf("%d\n", count);
    return 0;
}