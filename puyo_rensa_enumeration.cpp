#include <iostream>
#include <algorithm>
#include <vector>
#include <deque>
#include <queue>
#include <stack>
#include <map>
#include <iomanip>
#include <cmath>
#include <set>
#include <numeric>
#include <unordered_map>
using namespace std;
using ll = long long;

using board = vector<vector<int>>;
using vanish_puyos = vector<pair<int, int>>;

int ROW_SIZE = 9;
int COLUMN_SIZE = 3;
int MAX_RENSA = 4;
const ll NONE = 0;
const ll R = 1;
const ll G = 2;
const ll B = 3;
const ll Y = 4;
const vector<ll> COLORS = {
    R, G, B, Y};
const vector<pair<int, int>> NEIGHBORS = {
    {0, 1},
    {0, -1},
    {1, 0},
    {-1, 0}};

// 繋がりを管理するデータ構造
struct UnionFind
{
    vector<int> data;

    UnionFind(int sz)
    {
        data.assign(sz, -1);
    }
    void clear()
    {
        fill(data.begin(), data.end(), -1);
    }

    bool unite(int x, int y)
    {
        x = find(x), y = find(y);
        if (x == y)
            return (false);
        if (data[x] > data[y])
            swap(x, y);
        data[x] += data[y];
        data[y] = x;
        return (true);
    }

    bool same(int x, int y)
    {
        x = find(x), y = find(y);
        return x == y;
    }
    int find(int k)
    {
        if (data[k] < 0)
            return (k);
        return (data[k] = find(data[k]));
    }

    int size(int k)
    {
        return (-data[find(k)]);
    }
};

UnionFind uf(ROW_SIZE *COLUMN_SIZE);

// 盤面を出力
void print_board(board &b)
{
    for (int i = ROW_SIZE - 1; i >= 0; i--)
    {
        for (int j = 0; j < COLUMN_SIZE; j++)
        {
            cout << b[i][j];
        }
        cout << "\n";
    }
    cout << flush;
}

// 消えるぷよを挿入する。正しく挿入できたらtrue
bool insert_vanish_puyos(board &b, int base_row, int base_column, vanish_puyos &vanishes, int color)
{
    for (auto &p : vanishes)
    {
        ll r = base_row + p.first;
        ll c = base_column + p.second;
        if (0 <= r && r < ROW_SIZE &&
            0 <= c && c < COLUMN_SIZE &&
            (r == 0 || b[r - 1][c] != NONE) &&
            b[ROW_SIZE - 1][c] == NONE)
        {
            for (int i = ROW_SIZE - 1; i > r; i--)
            {
                b[i][c] = b[i - 1][c];
            }
            b[r][c] = color;
        }
        else
        {
            return false;
        }
    }
    return true;
}

//  連鎖が正しく消えるかどうか。挿入したぷよ以外が消えていたらfalse
bool vanish_correctly(board &b, ll max_vanish_size, ll max_vanish_count)
{
    uf.clear();
    ll count = 0;

    for (int r = 0; r < ROW_SIZE; r++)
    {
        for (int c = 0; c < COLUMN_SIZE; c++)
        {
            if (b[r][c] != NONE)
            {
                for (auto &neighbor : NEIGHBORS)
                {
                    int neighbor_r = r + neighbor.first;
                    int neighbor_c = c + neighbor.second;
                    if (0 <= neighbor_r && neighbor_r < ROW_SIZE &&
                        0 <= neighbor_c && neighbor_c < COLUMN_SIZE &&
                        b[r][c] == b[neighbor_r][neighbor_c])
                    {
                        uf.unite(COLUMN_SIZE * r + c, COLUMN_SIZE * neighbor_r + neighbor_c);
                    }
                }
            }
        }
    }

    for (int r = 0; r < ROW_SIZE; r++)
    {
        for (int c = 0; c < COLUMN_SIZE; c++)
        {
            if (uf.find(COLUMN_SIZE * r + c) == COLUMN_SIZE * r + c)
            {
                if (uf.size(COLUMN_SIZE * r + c) > max_vanish_size)
                {
                    return false;
                }
                else if (uf.size(COLUMN_SIZE * r + c) == max_vanish_size)
                {
                    count++;
                }
            }
        }
    }
    return count == max_vanish_count;
}

//  盤面で使われている色の数
int used_color_count(board b)
{
    set<int> used_colors;
    for (auto &row : b)
    {
        for (auto &cell : row)
        {
            if (cell != NONE)
            {
                used_colors.insert(cell);
            }
        }
    }
    return used_colors.size();
}

int main()
{
    vector<vector<int>> initial_board(ROW_SIZE, vector<int>(COLUMN_SIZE, NONE));
    // 今の所、4個消しのみ.
    vector<vanish_puyos> vanish_puyo_patterns = {
        {
            {0, 0}, {1, 0}, {2, 0}, {3, 0} // I
        },
        {
            {0, 0}, {1, 0}, {2, 0}, {0, 1} // L
        },
        {
            {0, 0}, {1, 0}, {2, 0}, {1, 1} // ト
        },
        {
            {0, 0}, {1, 0}, {2, 0}, {2, 1} // Γ
        },
        {
            {0, 1}, {1, 1}, {2, 1}, {0, 0} //  逆L
        },
        {
            {0, 1}, {1, 1}, {2, 1}, {1, 0} // 逆ト
        },
        {
            {0, 1}, {1, 1}, {2, 1}, {2, 0} // 逆Γ
        },
        {
            {0, 0}, {1, 0}, {0, 1}, {1, 1} // □
        },
        {
            {1, 0}, {2, 0}, {0, 1}, {1, 1} // S
        },
        {
            {0, 0}, {1, 0}, {1, 1}, {2, 1} // Z
        },
        {
            {0, 1}, {0, 2}, {1, 0}, {1, 1} // 横S
        },
        {
            {0, 0}, {0, 1}, {1, 1}, {1, 2} // 横Z
        },

        {
            {0, 0}, {0, 1}, {0, 2}, {1, 0} // 横L
        },
        {
            {0, 0}, {0, 1}, {0, 2}, {1, 1} // 横ト
        },
        {
            {0, 0}, {0, 1}, {0, 2}, {1, 2} // 横Γ
        },
        {
            {1, 0}, {1, 1}, {1, 2}, {0, 0} //  横逆L
        },
        {
            {1, 0}, {1, 1}, {1, 2}, {0, 1} // 横逆ト
        },
        {
            {1, 0}, {1, 1}, {1, 2}, {0, 2} // 横逆Γ
        },
        {
            {0, 0}, {0, 1}, {0, 2}, {0, 3} // 横I
        }};
    for (int i = 0; i < vanish_puyo_patterns.size(); i++)
    {
        sort(vanish_puyo_patterns[i].begin(), vanish_puyo_patterns[i].end());
    }
    vector<vector<board>> rensas(MAX_RENSA + 1);
    rensas[0].push_back(initial_board);
    for (int i = 0; i < MAX_RENSA; i++)
    {
        cout << i << ":" << rensas[i].size() << endl;
        for (auto &b : rensas[i])
        {
            ll next_colors = min(4, used_color_count(b) + 1);
            for (auto &v : vanish_puyo_patterns)
            {
                for (int r = 0; r < ROW_SIZE; r++)
                {
                    for (int c = 0; c < COLUMN_SIZE; c++)
                    {
                        if (i == 0)
                        {
                            board new_board(b);
                            // 一連鎖目の色はR
                            bool inserted = insert_vanish_puyos(new_board, r, c, v, COLORS[0]);
                            if (inserted && vanish_correctly(new_board, v.size(), 1))
                            {
                                rensas[i + 1].push_back(new_board);
                            }
                        }
                        else if (i == 1)
                        {
                            board new_board(b);
                            // 二連鎖目の色はG
                            bool inserted = insert_vanish_puyos(new_board, r, c, v, COLORS[1]);
                            if (inserted && vanish_correctly(new_board, v.size(), 1))
                            {
                                rensas[i + 1].push_back(new_board);
                            }
                        }
                        else
                        {
                            // 三連鎖目以降の色は既存の色か新しい色。
                            for (int j = 0; j < next_colors; j++)
                            {
                                board new_board(b);
                                bool inserted = insert_vanish_puyos(new_board, r, c, v, COLORS[j]);
                                if (inserted && vanish_correctly(new_board, v.size(), 1))
                                {
                                    rensas[i + 1].push_back(new_board);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    for (int i = 0; i < 10; i++)
    {
        print_board(rensas[MAX_RENSA][i]);
        cout << "=====" << endl;
    }
    cout << rensas[MAX_RENSA].size() << endl;
    return 0;
}