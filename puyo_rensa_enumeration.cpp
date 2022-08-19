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
using board = vector<vector<uint8_t>>;
using vanish_puyos = vector<pair<uint8_t, uint8_t>>;

uint8_t ROW_SIZE = 9;
uint8_t COLUMN_SIZE = 3;
uint8_t MAX_RENSA = 5;
const uint8_t NONE = 0;
const uint8_t R = 1;
const uint8_t G = 2;
const uint8_t B = 3;
const uint8_t Y = 4;
const vector<int> COLORS = {
    R, G, B, Y};
const vector<pair<uint8_t, uint8_t>> NEIGHBORS = {
    {0, 1},
    // {0, -1},
    {1, 0}
    // {-1, 0}
};

// 繋がりを管理するデータ構造
struct UnionFind
{
    vector<int8_t> data;

    UnionFind(uint8_t sz)
    {
        data.assign(sz, -1);
    }
    void clear()
    {
        fill(data.begin(), data.end(), -1);
    }

    bool unite(uint8_t x, uint8_t y)
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

    bool same(uint8_t x, uint8_t y)
    {
        x = find(x), y = find(y);
        return x == y;
    }
    uint8_t find(uint8_t k)
    {
        if (data[k] < 0)
            return (k);
        return (data[k] = find(data[k]));
    }

    uint8_t size(uint8_t k)
    {
        return (-data[find(k)]);
    }
};

UnionFind uf(ROW_SIZE *COLUMN_SIZE);

// 盤面を出力
void print_board(board &b)
{
    for (uint8_t j = 0; j < COLUMN_SIZE; j++)
    {
        for (uint8_t i = 0; i < b[j].size(); i++)
        {
            cout << (int)b[j][i];
        }
        cout << "\n";
    }
    cout << flush;
}

// 消えるぷよを挿入する。正しく挿入できたらtrue
bool insert_vanish_puyos(board &b, uint8_t base_row, uint8_t base_column, vanish_puyos &vanishes, uint8_t color)
{
    for (auto &p : vanishes)
    {
        uint8_t r = base_row + p.first;
        uint8_t c = base_column + p.second;
        if (0 <= r && r < ROW_SIZE &&
            0 <= c && c < COLUMN_SIZE &&
            (b[c].size() >= r) &&
            b[c].size() < ROW_SIZE)
        {
            b[c].insert(b[c].begin() + r, color);
        }
        else
        {
            return false;
        }
    }
    return true;
}

//  連鎖が正しく消えるかどうか。挿入したぷよ以外が消えていたらfalse
bool vanish_correctly(board &b, uint8_t max_vanish_size, uint8_t max_vanish_count)
{
    uf.clear();
    uint8_t count = 0;

    for (uint8_t c = 0; c < COLUMN_SIZE; c++)
    {
        for (uint8_t r = 0; r < b[c].size(); r++)
        {
            for (auto &neighbor : NEIGHBORS)
            {
                uint8_t neighbor_r = r + neighbor.first;
                uint8_t neighbor_c = c + neighbor.second;
                if (0 <= neighbor_c && neighbor_c < COLUMN_SIZE &&
                    0 <= neighbor_r && neighbor_r < b[neighbor_c].size() &&
                    b[c][r] == b[neighbor_c][neighbor_r])
                {
                    uf.unite(COLUMN_SIZE * r + c, COLUMN_SIZE * neighbor_r + neighbor_c);
                }
            }
        }
    }

    for (uint8_t c = 0; c < COLUMN_SIZE; c++)
    {
        for (uint8_t r = 0; r < b[c].size(); r++)
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
uint8_t used_color_count(board b)
{
    set<uint8_t> used_colors;
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
    vector<vector<uint8_t>> initial_board(COLUMN_SIZE);
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
    for (uint8_t i = 0; i < vanish_puyo_patterns.size(); i++)
    {
        sort(vanish_puyo_patterns[i].begin(), vanish_puyo_patterns[i].end());
    }
    vector<vector<board>> rensas(MAX_RENSA + 1);
    rensas[0].push_back(initial_board);
    for (uint8_t i = 0; i < MAX_RENSA; i++)
    {
        cout << (int)i << ":" << rensas[i].size() << endl;
        for (auto &b : rensas[i])
        {
            // 色は既存の色か新しい色。
            uint8_t next_colors = min(4, used_color_count(b) + 1);
            for (auto &v : vanish_puyo_patterns)
            {
                for (uint8_t c = 0; c < COLUMN_SIZE; c++)
                {
                    for (uint8_t r = 0; r <= b[c].size() && r < ROW_SIZE; r++)
                    {

                        for (uint8_t j = 0; j < next_colors; j++)
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
    for (uint8_t i = 0; i < 10; i++)
    {
        print_board(rensas[MAX_RENSA][i]);
        cout << "=====" << endl;
    }
    cout << rensas[MAX_RENSA].size() << endl;
    return 0;
}