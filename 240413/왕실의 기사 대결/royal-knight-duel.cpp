#include <iostream>
#include <queue>
#include <vector>

using namespace std;

#define MAX_B 41
#define MAX_N 31

struct Knight
{
    int r, c;
    int h, w;
    int k;
    int damage = 0;
    bool survive = true;
};

const int dr[] = {-1, 0, 1, 0};
const int dc[] = {0, 1, 0, -1};

int L, N, Q;
int board[MAX_B][MAX_B];
int board_K[MAX_B][MAX_B] = {0,};
Knight knights[MAX_N];

bool is_inrange(int row, int col)
{
    return 1 <= row && row <= L && 1 <= col && col <= L;
}

void update_knightLoc()
{
    for(int i = 1; i <= L; ++i)
        for(int j = 1; j <= L; ++j)
            board_K[i][j] = 0;
    for(int i = 1; i <= N; ++i){
        if(!knights[i].survive)
            continue;
        for(int row = knights[i].r; row < knights[i].r + knights[i].h; ++row){
            for(int col = knights[i].c; col < knights[i].c + knights[i].w; ++col){
                board_K[row][col] = i;
            }
        }
    }
}

bool checkValidation(int id, int direction, vector<int> &moving)
{
    bool check[MAX_N] = {false};
    queue<pair<int, int>> q;
    for(int row = knights[id].r; row < knights[id].r + knights[id].h; ++row){
        for(int col = knights[id].c; col < knights[id].c + knights[id].w; ++col){
            pair<int, int> tmp;
            tmp.first = row + dr[direction];
            tmp.second = col + dc[direction];
            q.push(tmp);     
        }
    }
    check[id] = true;
    while(!q.empty()){
        pair<int, int> cur = q.front(); q.pop();
        if(!is_inrange(cur.first, cur.second) || board[cur.first][cur.second] == 2)
            return false;
        if(board_K[cur.first][cur.second]){
            int tmp_id = board_K[cur.first][cur.second];
            if(!check[tmp_id]){
                for(int row = knights[tmp_id].r; row < knights[tmp_id].r + knights[tmp_id].h; ++row){
                    for(int col = knights[tmp_id].c; col < knights[tmp_id].c + knights[tmp_id].w; ++col){
                        pair<int, int> tmp;
                        tmp.first = row + dr[direction];
                        tmp.second = col + dc[direction];
                        q.push(tmp);
                    }
                }
                check[tmp_id] = true;
                moving.push_back(tmp_id);
            }
        }
    }
    return true;
}

void move(int id, int direction){
    if(!knights[id].survive)
        return;
    vector<int> moving;
    if(checkValidation(id, direction, moving)){
        knights[id].r += dr[direction];
        knights[id].c += dc[direction];
        for(int moveId: moving){
            knights[moveId].r += dr[direction];
            knights[moveId].c += dc[direction];
            for(int row = knights[moveId].r; row < knights[moveId].r + knights[moveId].h; ++row){
                for(int col = knights[moveId].c; col < knights[moveId].c + knights[moveId].w; ++col){
                    if(board[row][col] == 1)
                        knights[moveId].damage += 1;
                }
            }
            if(knights[moveId].k <= knights[moveId].damage)
                knights[moveId].survive = false;
        }
        update_knightLoc();
    }
}

int main() {
    cin >> L >> N >> Q;
    for(int i = 1; i <= L; ++i){
        for(int j = 1; j <= L; ++j){
            cin >> board[i][j];
        }
    }
    for(int i = 1; i <= N; ++i)
        cin >> knights[i].r >> knights[i].c >> knights[i].h >> knights[i].w >> knights[i].k;
    update_knightLoc();

    for(int i = 1; i <= Q; ++i){
        int id, direction;
        cin >> id >> direction;
        move(id, direction);
    }

    int total_damage = 0;
    for(int i = 1; i <= N; ++i){
        if(knights[i].survive)
            total_damage += knights[i].damage;
    }

    cout << total_damage << "\n";

    return 0;
}