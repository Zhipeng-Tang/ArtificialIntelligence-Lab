#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <set>
#include <map>
#include <string.h>
using namespace std;
#define spacesize 5
#define column 2
#define row 2

typedef struct State
{
    int f, g, h;
    int x, y;
    char action;
    State *parent;
    vector<vector<int>> pos;
} State;

/**
 * @brief 启发式函数h1(n)
 * 
 * @param position 当前各个星球的位置
 * @param target 各个请求的目标位置
 * @return int 
 */
int h1(vector<vector<int>> position, vector<vector<int>> target)
{
    int count = 0;
    for (int i = 0; i < spacesize; i++)
    {
        for (int j = 0; j < spacesize; j++)
        {
            if (position[i][j] > 0 && position[i][j] != target[i][j])
                count++;
        }
    }
    return count;
}

/**
 * @brief 启发式函数h2(n)
 * 
 * @param position 当前各个星球的位置
 * @param target 各个星球的目标位置
 * @return int 
 */
int h2(vector<vector<int>> position, map<int, pair<int, int>> target)
{
    int i, j;
    int x, y = 0;
    int count = 0;
    for (i = 0; i < spacesize; i++)
    {
        for (j = 0; j < spacesize; j++)
        {
            if (position[i][j] > 0)
            {
                x = target[position[i][j]].first;
                y = target[position[i][j]].second;
                if (abs(x - i) > 2)
                    count += 5 - abs(x - i);
                else
                    count += abs(x - i);
                if (abs(y - j) > 2)
                    count += 5 - abs(y - j);
                else
                    count += abs(y - j);
            }
        }
    }
    return count;
}

/**
 * @brief 判断当前状态是否已经到达目标状态
 * 
 * @param position 当前各个星球的位置
 * @param target 各个星球的目标位置
 * @return true 到达目标状态
 * @return false 未到达目标状态
 */
bool is_goal(vector<vector<int>> position, vector<vector<int>> target)
{
    for (int i = 0; i < spacesize; i++)
    {
        for (int j = 0; j < spacesize; j++)
        {
            if (position[i][j] != target[i][j])
                return false;
        }
    }
    return true;
}

/**
 * @brief 输出从开始状态到当前状态state的每一步
 * 
 * @param state 当前状态
 */
void print_path_action(State * state)
{
    // 如果stat是开始状态，则直接返回
    if (state->parent == NULL)
    {
        printf("\n");
        return;
    }
    else
    {
        // 输出从开始状态到state->parent的每一步 和 state->parent 到 state的行动即可
        print_path_action(state->parent);
        printf("%c", state->action);
        return;
    }
}

/**
 * @brief 以h1(n)为启发函数的A*算法实现
 * 
 * @param start 开始状态
 * @param target 目标状态
 */
void A_h1(const vector<vector<int>> &start, const vector<vector<int>> &target)
{
    int i, j;

    // 找到所有黑洞的位置，并记录
    std::set<int> black_holes;
    for (i = 0; i < spacesize; i++)
    {
        for (j = 0; j < spacesize; j++)
        {
            if (start[i][j] < 0)
                black_holes.insert(spacesize * i + j);
        }
    }

    // 初始化开始状态
    State s0;
    s0.parent = NULL;
    s0.pos = start;
    for (i = 0; i < spacesize; i++)
    {
        for(j = 0; j < spacesize; j++)
        {
            if (start[i][j] == 0)
            {
                s0.x = i;
                s0.y = j;
            }
        }
    }
    s0.g = 0;
    s0.h = h1(s0.pos, target);
    s0.f = s0.g + s0.h;

    // 用 open_list 记录已经到达的状态，以f=g+h为键
    std::multimap<int, State *> open_list;
    open_list.insert(pair<int, State *>(s0.f, &s0));

    State *new_state = NULL;
    // 每次都取出 open_list 中的首个状态，如果该状态是目标状态，则结束！
    // 否则，继续搜索
    while (!is_goal(open_list.begin()->second->pos, target))
    {
        // 取出 open_list 中的首个状态 curr
        auto curr = open_list.begin()->second;
        open_list.erase(open_list.begin());

        // 向上下左右四个方向做出试探，新状态都加入 open_list 中
        // 其实这部分代码可以复用，但是写完就不想改了，所以后面相同部分就不做详细描述了
        // 同时控制飞船不往回走，即：如果上一步是Up，那么这一步就不会是Down
        if ((curr->x - 1 >= 0 || curr->y == column) && (curr->action != 'D'))
        {
            if (black_holes.find(spacesize * ((curr->x - 1 + spacesize) % spacesize) + curr->y) == black_holes.end())
            {
                new_state = new State;
                new_state->pos = curr->pos;
                new_state->pos[curr->x][curr->y] = curr->pos[(curr->x - 1 + spacesize) % spacesize][curr->y];
                new_state->pos[(curr->x - 1 + spacesize) % spacesize][curr->y] = curr->pos[curr->x][curr->y];
                new_state->x = (curr->x - 1 + spacesize) % spacesize;
                new_state->y = curr->y;
                new_state->parent = curr;
                new_state->action = 'U';
                new_state->g = curr->g + 1;
                new_state->h = h1(new_state->pos, target);
                new_state->f = new_state->g + new_state->h;
                open_list.insert(pair<int, State *>(new_state->f, new_state));
            }
        }

        if ((curr->x <= spacesize - 2 || curr->y == column) && (curr->action != 'U'))
        {
            if (black_holes.find(spacesize * ((curr->x + 1) % spacesize) + curr->y) == black_holes.end())
            {
                new_state = new State;
                new_state->pos = curr->pos;
                new_state->pos[curr->x][curr->y] = curr->pos[(curr->x + 1) % spacesize][curr->y];
                new_state->pos[(curr->x + 1) % spacesize][curr->y] = curr->pos[curr->x][curr->y];
                new_state->x = (curr->x + 1) % spacesize;
                new_state->y = curr->y;
                new_state->parent = curr;
                new_state->action = 'D';
                new_state->g = curr->g + 1;
                new_state->h = h1(new_state->pos, target);
                new_state->f = new_state->g + new_state->h;
                open_list.insert(pair<int, State *>(new_state->f, new_state));
            }
        }

        if ((curr->y - 1 >= 0 || curr->x == row) && (curr-> action != 'R'))
        {
            if (black_holes.find((curr->y - 1 + spacesize) % spacesize + spacesize * curr->x) == black_holes.end())
            {
                new_state = new State;
                new_state->pos = curr->pos;
                new_state->pos[curr->x][curr->y] = curr->pos[curr->x][(curr->y - 1 + spacesize) % spacesize];
                new_state->pos[curr->x][(curr->y - 1 + spacesize) % spacesize] = curr->pos[curr->x][curr->y];
                new_state->x = curr->x;
                new_state->y = (curr->y - 1 + spacesize) % spacesize;
                new_state->parent = curr;
                new_state->action = 'L';
                new_state->g = curr->g + 1;
                new_state->h = h1(new_state->pos, target);
                new_state->f = new_state->g + new_state->h;
                open_list.insert(pair<int, State *>(new_state->f, new_state));
            }
        }

        if ((curr->y <= spacesize - 2 || curr->x == row) && (curr->action != 'L'))
        {
            if (black_holes.find((curr->y + 1) % spacesize + spacesize * curr->x) == black_holes.end())
            {
                new_state = new State;
                new_state->pos = curr->pos;
                new_state->pos[curr->x][curr->y] = curr->pos[curr->x][(curr->y + 1) % spacesize];
                new_state->pos[curr->x][(curr->y + 1) % spacesize] = curr->pos[curr->x][curr->y];
                new_state->x = curr->x;
                new_state->y = (curr->y + 1) % spacesize;
                new_state->parent = curr;
                new_state->action = 'R';
                new_state->g = curr->g + 1;
                new_state->h = h1(new_state->pos, target);
                new_state->f = new_state->g + new_state->h;
                open_list.insert(pair<int, State *>(new_state->f, new_state));
            }
        }
    }

    // 取出目标状态，并输出路径
    State *target_state = open_list.begin()->second;
    print_path_action(target_state);
    
    return;
}

/**
 * @brief 以h2(n)为启发函数的A*算法
 * 
 * @param start 开始状态
 * @param target 目标状态
 */
void A_h2(const vector<vector<int>> &start, const vector<vector<int>> &target)
{
    int i, j;
    map <int, pair<int, int>> target_pos;
    for (i = 0; i < spacesize; i++)
    {
        for (j = 0; j < spacesize; j++)
        {
            if (target[i][j] > 0)
                target_pos.insert(pair<int, pair<int, int>>(target[i][j], pair<int, int>(i, j)));
        }
    }

    std::set<int> black_holes;
    for (i = 0; i < spacesize; i++)
    {
        for (j = 0; j < spacesize; j++)
        {
            if (start[i][j] < 0)
                black_holes.insert(spacesize * i + j);
        }
    }

    // initial state s0
    State s0;
    s0.parent = NULL;
    s0.pos = start;
    for (i = 0; i < spacesize; i++)
    {
        for(j = 0; j < spacesize; j++)
        {
            if (start[i][j] == 0)
            {
                s0.x = i;
                s0.y = j;
            }
        }
    }
    s0.g = 0;
    s0.h = h2(s0.pos, target_pos);
    s0.f = s0.g + s0.h;

    std::multimap<int, State *> open_list;
    open_list.insert(pair<int, State *>(s0.f, &s0));

    State *new_state = NULL;
    while (!is_goal(open_list.begin()->second->pos, target))
    {
        auto curr = open_list.begin()->second;
        open_list.erase(open_list.begin());

        if ((curr->x - 1 >= 0 || curr->y == column) && (curr->action != 'D'))
        {
            if (black_holes.find(spacesize * ((curr->x - 1 + spacesize) % spacesize) + curr->y) == black_holes.end())
            {
                new_state = new State;
                new_state->pos = curr->pos;
                new_state->pos[curr->x][curr->y] = curr->pos[(curr->x - 1 + spacesize) % spacesize][curr->y];
                new_state->pos[(curr->x - 1 + spacesize) % spacesize][curr->y] = curr->pos[curr->x][curr->y];
                new_state->x = (curr->x - 1 + spacesize) % spacesize;
                new_state->y = curr->y;
                new_state->parent = curr;
                new_state->action = 'U';
                new_state->g = curr->g + 1;
                new_state->h = h2(new_state->pos, target_pos);
                new_state->f = new_state->g + new_state->h;
                open_list.insert(pair<int, State *>(new_state->f, new_state));
            }
        }

        if ((curr->x <= spacesize - 2 || curr->y == column) && (curr->action != 'U'))
        {
            if (black_holes.find(spacesize * ((curr->x + 1) % spacesize) + curr->y) == black_holes.end())
            {
                new_state = new State;
                new_state->pos = curr->pos;
                new_state->pos[curr->x][curr->y] = curr->pos[(curr->x + 1) % spacesize][curr->y];
                new_state->pos[(curr->x + 1) % spacesize][curr->y] = curr->pos[curr->x][curr->y];
                new_state->x = (curr->x + 1) % spacesize;
                new_state->y = curr->y;
                new_state->parent = curr;
                new_state->action = 'D';
                new_state->g = curr->g + 1;
                new_state->h = h2(new_state->pos, target_pos);
                new_state->f = new_state->g + new_state->h;
                open_list.insert(pair<int, State *>(new_state->f, new_state));
            }
        }

        if ((curr->y - 1 >= 0 || curr->x == row) && (curr-> action != 'R'))
        {
            if (black_holes.find((curr->y - 1 + spacesize) % spacesize + spacesize * curr->x) == black_holes.end())
            {
                new_state = new State;
                new_state->pos = curr->pos;
                new_state->pos[curr->x][curr->y] = curr->pos[curr->x][(curr->y - 1 + spacesize) % spacesize];
                new_state->pos[curr->x][(curr->y - 1 + spacesize) % spacesize] = curr->pos[curr->x][curr->y];
                new_state->x = curr->x;
                new_state->y = (curr->y - 1 + spacesize) % spacesize;
                new_state->parent = curr;
                new_state->action = 'L';
                new_state->g = curr->g + 1;
                new_state->h = h2(new_state->pos, target_pos);
                new_state->f = new_state->g + new_state->h;
                open_list.insert(pair<int, State *>(new_state->f, new_state));
            }
        }

        if ((curr->y <= spacesize - 2 || curr->x == row) && (curr->action != 'L'))
        {
            if (black_holes.find((curr->y + 1) % spacesize + spacesize * curr->x) == black_holes.end())
            {
                new_state = new State;
                new_state->pos = curr->pos;
                new_state->pos[curr->x][curr->y] = curr->pos[curr->x][(curr->y + 1) % spacesize];
                new_state->pos[curr->x][(curr->y + 1) % spacesize] = curr->pos[curr->x][curr->y];
                new_state->x = curr->x;
                new_state->y = (curr->y + 1) % spacesize;
                new_state->parent = curr;
                new_state->action = 'R';
                new_state->g = curr->g + 1;
                new_state->h = h2(new_state->pos, target_pos);
                new_state->f = new_state->g + new_state->h;
                open_list.insert(pair<int, State *>(new_state->f, new_state));
            }
        }
    }

    State *target_state = open_list.begin()->second;
    print_path_action(target_state);
    
    return;
}

/**
 * @brief 以h1(n)为启发函数的迭代A*算法
 * 
 * @param start 开始状态
 * @param target 目标状态
 */
void IDA_h1(const vector<vector<int>> &start, const vector<vector<int>> &target)
{
    int i, j;

    // find the position of black hole
    std::set<int> black_holes;
    for (i = 0; i < spacesize; i++)
    {
        for (j = 0; j < spacesize; j++)
        {
            if (start[i][j] < 0)
                black_holes.insert(spacesize * i + j);
        }
    }

    // initial state s0
    State s0;
    s0.parent = NULL;
    s0.pos = start;
    for (i = 0; i < spacesize; i++)
    {
        for(j = 0; j < spacesize; j++)
        {
            if (start[i][j] == 0)
            {
                s0.x = i;
                s0.y = j;
            }
        }
    }
    s0.g = 0;
    s0.h = h1(s0.pos, target);
    s0.f = s0.g + s0.h;

    int f_limit = s0.f;
    int next_f_limit = 0;

    std::multimap<int, State *> open_list;

    State *new_state = NULL;
    State *final_state = NULL;
    while(f_limit < INT_MAX)
    {
        next_f_limit = INT_MAX;
        open_list.insert(pair<int, State *>(s0.f, &s0));
        while (!open_list.empty())
        {
            auto curr = open_list.begin()->second;
            open_list.erase(open_list.begin());

            if (curr->f > f_limit)
            {
                if (next_f_limit > curr->f)
                    next_f_limit = curr->f;
            }
            else
            {
                if (is_goal(curr->pos, target))
                {
                    final_state = curr;
                    break;
                }
                if ((curr->x - 1 >= 0 || curr->y == column) && (curr->action != 'D'))
                {
                    if (black_holes.find(spacesize * ((curr->x - 1 + spacesize) % spacesize) + curr->y) == black_holes.end())
                    {
                        new_state = new State;
                        new_state->pos = curr->pos;
                        new_state->pos[curr->x][curr->y] = curr->pos[(curr->x - 1 + spacesize) % spacesize][curr->y];
                        new_state->pos[(curr->x - 1 + spacesize) % spacesize][curr->y] = curr->pos[curr->x][curr->y];
                        new_state->x = (curr->x - 1 + spacesize) % spacesize;
                        new_state->y = curr->y;
                        new_state->parent = curr;
                        new_state->action = 'U';
                        new_state->g = curr->g + 1;
                        new_state->h = h1(new_state->pos, target);
                        new_state->f = new_state->g + new_state->h;
                        open_list.insert(pair<int, State *>(new_state->f, new_state));
                    }
                }

                if ((curr->x <= spacesize - 2 || curr->y == column) && (curr->action != 'U'))
                {
                    if (black_holes.find(spacesize * ((curr->x + 1) % spacesize) + curr->y) == black_holes.end())
                    {
                        new_state = new State;
                        new_state->pos = curr->pos;
                        new_state->pos[curr->x][curr->y] = curr->pos[(curr->x + 1) % spacesize][curr->y];
                        new_state->pos[(curr->x + 1) % spacesize][curr->y] = curr->pos[curr->x][curr->y];
                        new_state->x = (curr->x + 1) % spacesize;
                        new_state->y = curr->y;
                        new_state->parent = curr;
                        new_state->action = 'D';
                        new_state->g = curr->g + 1;
                        new_state->h = h1(new_state->pos, target);
                        new_state->f = new_state->g + new_state->h;
                        open_list.insert(pair<int, State *>(new_state->f, new_state));
                    }
                }

                if ((curr->y - 1 >= 0 || curr->x == row) && (curr-> action != 'R'))
                {
                    if (black_holes.find((curr->y - 1 + spacesize) % spacesize + spacesize * curr->x) == black_holes.end())
                    {
                        new_state = new State;
                        new_state->pos = curr->pos;
                        new_state->pos[curr->x][curr->y] = curr->pos[curr->x][(curr->y - 1 + spacesize) % spacesize];
                        new_state->pos[curr->x][(curr->y - 1 + spacesize) % spacesize] = curr->pos[curr->x][curr->y];
                        new_state->x = curr->x;
                        new_state->y = (curr->y - 1 + spacesize) % spacesize;
                        new_state->parent = curr;
                        new_state->action = 'L';
                        new_state->g = curr->g + 1;
                        new_state->h = h1(new_state->pos, target);
                        new_state->f = new_state->g + new_state->h;
                        open_list.insert(pair<int, State *>(new_state->f, new_state));
                    }
                }

                if ((curr->y <= spacesize - 2 || curr->x == row) && (curr->action != 'L'))
                {
                    if (black_holes.find((curr->y + 1) % spacesize + spacesize * curr->x) == black_holes.end())
                    {
                        new_state = new State;
                        new_state->pos = curr->pos;
                        new_state->pos[curr->x][curr->y] = curr->pos[curr->x][(curr->y + 1) % spacesize];
                        new_state->pos[curr->x][(curr->y + 1) % spacesize] = curr->pos[curr->x][curr->y];
                        new_state->x = curr->x;
                        new_state->y = (curr->y + 1) % spacesize;
                        new_state->parent = curr;
                        new_state->action = 'R';
                        new_state->g = curr->g + 1;
                        new_state->h = h1(new_state->pos, target);
                        new_state->f = new_state->g + new_state->h;
                        open_list.insert(pair<int, State *>(new_state->f, new_state));
                    }
                }
            }
        }
        f_limit = next_f_limit;
        if (final_state)
            break;
    }

    print_path_action(final_state);
    
    return;
}

/**
 * @brief 以h2(n)为启发函数的迭代A*算法
 * 
 * @param start 开始状态
 * @param target 目标状态
 */
void IDA_h2(const vector<vector<int>> &start, const vector<vector<int>> &target)
{
    int i, j;
    map <int, pair<int, int>> target_pos;
    for (i = 0; i < spacesize; i++)
    {
        for (j = 0; j < spacesize; j++)
        {
            if (target[i][j] > 0)
                target_pos.insert(pair<int, pair<int, int>>(target[i][j], pair<int, int>(i, j)));
        }
    }

    // find the position of black hole
    std::set<int> black_holes;
    for (i = 0; i < spacesize; i++)
    {
        for (j = 0; j < spacesize; j++)
        {
            if (start[i][j] < 0)
                black_holes.insert(spacesize * i + j);
        }
    }

    // initial state s0
    State s0;
    s0.parent = NULL;
    s0.pos = start;
    for (i = 0; i < spacesize; i++)
    {
        for(j = 0; j < spacesize; j++)
        {
            if (start[i][j] == 0)
            {
                s0.x = i;
                s0.y = j;
            }
        }
    }
    s0.g = 0;
    s0.h = h2(s0.pos, target_pos);
    s0.f = s0.g + s0.h;

    int f_limit = s0.f;
    int next_f_limit = 0;

    std::multimap<int, State *> open_list;

    State *new_state = NULL;
    State *final_state = NULL;
    while(f_limit < INT_MAX)
    {
        next_f_limit = INT_MAX;
        open_list.insert(pair<int, State *>(s0.f, &s0));
        while (!open_list.empty())
        {
            auto curr = open_list.begin()->second;
            open_list.erase(open_list.begin());

            if (curr->f > f_limit)
            {
                if (next_f_limit > curr->f)
                    next_f_limit = curr->f;
            }
            else
            {
                if (is_goal(curr->pos, target))
                {
                    final_state = curr;
                    break;
                }
                if ((curr->x - 1 >= 0 || curr->y == column) && (curr->action != 'D'))
                {
                    if (black_holes.find(spacesize * ((curr->x - 1 + spacesize) % spacesize) + curr->y) == black_holes.end())
                    {
                        new_state = new State;
                        new_state->pos = curr->pos;
                        new_state->pos[curr->x][curr->y] = curr->pos[(curr->x - 1 + spacesize) % spacesize][curr->y];
                        new_state->pos[(curr->x - 1 + spacesize) % spacesize][curr->y] = curr->pos[curr->x][curr->y];
                        new_state->x = (curr->x - 1 + spacesize) % spacesize;
                        new_state->y = curr->y;
                        new_state->parent = curr;
                        new_state->action = 'U';
                        new_state->g = curr->g + 1;
                        new_state->h = h2(new_state->pos, target_pos);
                        new_state->f = new_state->g + new_state->h;
                        open_list.insert(pair<int, State *>(new_state->f, new_state));
                    }
                }

                if ((curr->x <= spacesize - 2 || curr->y == column) && (curr->action != 'U'))
                {
                    if (black_holes.find(spacesize * ((curr->x + 1) % spacesize) + curr->y) == black_holes.end())
                    {
                        new_state = new State;
                        new_state->pos = curr->pos;
                        new_state->pos[curr->x][curr->y] = curr->pos[(curr->x + 1) % spacesize][curr->y];
                        new_state->pos[(curr->x + 1) % spacesize][curr->y] = curr->pos[curr->x][curr->y];
                        new_state->x = (curr->x + 1) % spacesize;
                        new_state->y = curr->y;
                        new_state->parent = curr;
                        new_state->action = 'D';
                        new_state->g = curr->g + 1;
                        new_state->h = h2(new_state->pos, target_pos);
                        new_state->f = new_state->g + new_state->h;
                        open_list.insert(pair<int, State *>(new_state->f, new_state));
                    }
                }

                if ((curr->y - 1 >= 0 || curr->x == row) && (curr-> action != 'R'))
                {
                    if (black_holes.find((curr->y - 1 + spacesize) % spacesize + spacesize * curr->x) == black_holes.end())
                    {
                        new_state = new State;
                        new_state->pos = curr->pos;
                        new_state->pos[curr->x][curr->y] = curr->pos[curr->x][(curr->y - 1 + spacesize) % spacesize];
                        new_state->pos[curr->x][(curr->y - 1 + spacesize) % spacesize] = curr->pos[curr->x][curr->y];
                        new_state->x = curr->x;
                        new_state->y = (curr->y - 1 + spacesize) % spacesize;
                        new_state->parent = curr;
                        new_state->action = 'L';
                        new_state->g = curr->g + 1;
                        new_state->h = h2(new_state->pos, target_pos);
                        new_state->f = new_state->g + new_state->h;
                        open_list.insert(pair<int, State *>(new_state->f, new_state));
                    }
                }

                if ((curr->y <= spacesize - 2 || curr->x == row) && (curr->action != 'L'))
                {
                    if (black_holes.find((curr->y + 1) % spacesize + spacesize * curr->x) == black_holes.end())
                    {
                        new_state = new State;
                        new_state->pos = curr->pos;
                        new_state->pos[curr->x][curr->y] = curr->pos[curr->x][(curr->y + 1) % spacesize];
                        new_state->pos[curr->x][(curr->y + 1) % spacesize] = curr->pos[curr->x][curr->y];
                        new_state->x = curr->x;
                        new_state->y = (curr->y + 1) % spacesize;
                        new_state->parent = curr;
                        new_state->action = 'R';
                        new_state->g = curr->g + 1;
                        new_state->h = h2(new_state->pos, target_pos);
                        new_state->f = new_state->g + new_state->h;
                        open_list.insert(pair<int, State *>(new_state->f, new_state));
                    }
                }
            }
        }
        f_limit = next_f_limit;
        if (final_state)
            break;
    }

    print_path_action(final_state);
    
    return;
}

int main(int argc, char *argv[])
{
    // if (argc == 4)
    // {
    //     int i, j;
    //     vector<vector<int>> start(spacesize);
    //     vector<vector<int>> target(spacesize);
    //     freopen(argv[2], "r", stdin);
    //     for (i = 0; i < spacesize; i++)
    //     {
    //         start[i].resize(spacesize);
    //         for (j = 0; j < spacesize; j++)
    //             scanf("%d", &start[i][j]);
    //     }
    //     freopen(argv[3], "r", stdin);
    //     for (i = 0; i < spacesize; i++)
    //     {
    //         target[i].resize(spacesize);
    //         for (j = 0; j < spacesize; j++)
    //             scanf("%d", &target[i][j]);
    //     }

    //     if (strcmp(argv[1], "A_h1") == 0)
    //         A_h1(start, target);
    //     else if (strcmp(argv[1], "A_h2") == 0)
    //         A_h2(start, target);
    //     else if (strcmp(argv[1], "IDA_h1") == 0)
    //         IDA_h1(start, target);
    //     else if (strcmp(argv[1], "IDA_h2") == 0)
    //         IDA_h2(start, target);
    //     else
    //     {
    //         printf("error!\n");
    //     }
    // }

    int i, j;
    vector<vector<int>> start(spacesize);
    vector<vector<int>> target(spacesize);
    for (i = 0; i < spacesize; i++)
    {
        start[i].resize(spacesize);
        for (j = 0; j < spacesize; j++)
            scanf("%d", &start[i][j]);
    }
    for (i = 0; i < spacesize; i++)
    {
        target[i].resize(spacesize);
        for (j = 0; j < spacesize; j++)
            scanf("%d", &target[i][j]);
    }
    A_h2(start, target);

    return 0;
}