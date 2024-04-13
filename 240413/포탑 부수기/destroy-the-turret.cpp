#include <iostream>
#include <queue>

using namespace std;

#define MAX_F 11
#define MAX_POWER 5000
#define UPGRADE_POWER N+M

int N, M, K;
int field[MAX_F][MAX_F];
int attack[MAX_F][MAX_F] = {0};

const int dr[] = {0, 1, 0, -1};
const int dc[] = {1, 0, -1, 0};
const int splash_r[] = {-1, -1, 0, 1, 1, 1, 0, -1};
const int splash_c[] = {0, 1, 1, 1, 0, -1, -1, -1};

void cannonAttack(pair<int, int> attacker, pair<int, int> target){
    int damage = field[attacker.first][attacker.second];
    field[target.first][target.second] -= damage;
    if(field[target.first][target.second] < 0)
        field[target.first][target.second] = 0;
    for(int i = 0; i < 8; ++i){
        int splash_row = (target.first+splash_r[i])%5 ? (target.first+splash_r[i])%5 : 1;
        int splash_col = (target.second+splash_c[i])%5 ? (target.second+splash_c[i])%5 : 1;
        if(!field[splash_row][splash_col])
            continue;
        field[splash_row][splash_col] = ((field[splash_row][splash_col] - (damage/2)) < 0) ? 0 : (field[splash_row][splash_col] - (damage/2));
    }
}

bool laserAttack(pair<int, int> attacker, pair<int, int> target){
    pair<int, int> log[MAX_F][MAX_F];
    bool visited[MAX_F][MAX_F] = {false};
    int damage = field[attacker.first][attacker.second];
    queue<pair<int, int>> q;
    q.push(attacker);
    visited[attacker.first][attacker.second] = true;
    while(!q.empty()){
        pair<int, int> cur = q.front(); q.pop();
        if(cur.first == target.first && cur.second == target.second){
            field[target.first][target.second] -= damage;
            if(field[target.first][target.second] < 0)
                field[target.first][target.second] = 0;
            pair<int, int> pre = log[target.first][target.second];
            while(true){
                if(pre.first == attacker.first && pre.second == attacker.second)
                    break;
                field[pre.first][pre.second] -= (damage/2);
                if(field[pre.first][pre.second] < 0)
                    field[pre.first][pre.second] = 0;
                pre = log[pre.first][pre.second];
            }

            return true;
        }
        for(int i = 0; i < 4; ++i){
            pair<int, int> next;
            next.first = (cur.first + dr[i]) % 5 ? (cur.first + dr[i]) % 5 : 1;
            next.second = (cur.second + dc[i]) % 5 ? (cur.second + dc[i]) % 5 : 1;
            if(!visited[next.first][next.second] && field[next.first][next.second]){
                q.push(next);
                visited[next.first][next.second] = true;
                log[next.first][next.second] = cur;
            }
        }
    }

    return false;
}

pair<int, int> findTarget(){
    pair<int, int> target(MAX_F+1, MAX_F+1);
    int power = 0;
    for(int row = 1; row <= N; ++row){
        for(int col = 1; col <= M; ++col){
            if(field[row][col] < 1)
                continue;
            if(power < field[row][col]){
                target.first = row; target.second = col;
                power = field[row][col];
            }
            else if(power == field[row][col]){
                if(attack[row][col] < attack[target.first][target.second]){
                    target.first = row; target.second = col;
                    power = field[row][col];
                }
                else if(attack[target.first][target.second] == attack[row][col]){
                    if(row + col < target.first + target.second){
                        target.first = row; target.second = col;
                        power = field[row][col];
                    }
                    else if(target.first + target.second == row+col && col < target.second){
                        target.first = row; target.second = col;
                        power = field[row][col];
                    }
                }
            }
        }
    }

    return target;
}

pair<int, int> findAttacter(){
    pair<int, int> attacker(-1, -1);
    int power = MAX_POWER;
    for(int row = 1; row <= N; ++row){
        for(int col = 1; col <= M; ++col){
            if(field[row][col] < 1)
                continue;
            if(field[row][col] < power){
                attacker.first = row; attacker.second = col;
                power = field[row][col];
            }
            else if(field[row][col] == power){
                if(attack[attacker.first][attacker.second] < attack[row][col]){
                    attacker.first = row; attacker.second = col;
                    power = field[row][col];
                }
                else if(attack[attacker.first][attacker.second] == attack[row][col]){
                    if(attacker.first + attacker.second < row + col){
                        attacker.first = row; attacker.second = col;
                        power = field[row][col];
                    }
                    else if(attacker.first + attacker.second == row+col && attacker.second < col){
                        attacker.first = row; attacker.second = col;
                        power = field[row][col];
                    }
                }
            }
        }
    }
    field[attacker.first][attacker.second] += (UPGRADE_POWER);
    return attacker;
}

void copyField(int destination[][MAX_F]) {
    for (int i = 1; i <= N; ++i)
        for (int j = 1; j <= M; ++j)
            destination[i][j] = field[i][j];
}

void fix(int compare[][MAX_F]){
    for (int i = 1; i <= N; ++i){
        for (int j = 1; j <= M; ++j){
            if(field[i][j] == 0)
                continue;
            if(compare[i][j] == field[i][j])
                field[i][j] += 1;
        }
    }
}

int main() {
    cin >> N >> M >> K;
    for(int i = 1; i <= N; ++i)
        for(int j = 1; j <= M; ++j)
            cin >> field[i][j];

    int backup_field[MAX_F][MAX_F];
    copyField(backup_field);

    pair<int, int> attacker, target;
    for(int round = 1; round <= K; ++round){
        copyField(backup_field);
        
        attacker = findAttacter();
        attack[attacker.first][attacker.second] = round;
        target = findTarget();

        if(!laserAttack(attacker, target))
            cannonAttack(attacker, target);
        fix(backup_field);
        target = findTarget();
    }
    cout << field[target.first][target.second] << "\n";

    return 0;
}