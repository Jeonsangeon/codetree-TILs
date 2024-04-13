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
    int damage = field[attacker.first][attacker.second]; // 공격 포탑의 공격력
    // 공격 대상 피해
    field[target.first][target.second] -= damage;
    if(field[target.first][target.second] < 0)
        field[target.first][target.second] = 0;
    // 주변 포탑 피해
    for(int i = 0; i < 8; ++i){
        int splash_row = target.first + splash_r[i];
        if(splash_row == 0)
            splash_row = N;
        else if(splash_row == N+1)
            splash_row = 1;

        int splash_col = target.second + splash_c[i];
        if(splash_col == 0)
            splash_col = M;
        else if(splash_col == M+1)
            splash_col = 1;
        // 부서진 포탑인 경우 및 공격자인 경우
        if(!field[splash_row][splash_col] || (splash_row == attacker.first && splash_col == attacker.second))
            continue;
        field[splash_row][splash_col] = ((field[splash_row][splash_col] - (damage/2)) < 0) ? 0 : (field[splash_row][splash_col] - (damage/2));
    }
}

bool laserAttack(pair<int, int> attacker, pair<int, int> target){
    pair<int, int> log[MAX_F][MAX_F] = {{make_pair(-1, -1)}};   //레이저 공격의 경로 저장
    int damage = field[attacker.first][attacker.second];        // 공격 포탑의 공격력
    bool visited[MAX_F][MAX_F] = {false};
    queue<pair<int, int>> q;
    q.push(attacker);
    visited[attacker.first][attacker.second] = true;

    while(!q.empty()){
        pair<int, int> cur = q.front(); q.pop();
        // 공격 대상에 도달한 경우
        if(cur.first == target.first && cur.second == target.second){
            // 공격 대상 피해
            field[target.first][target.second] -= damage;
            if(field[target.first][target.second] < 0)
                field[target.first][target.second] = 0;
            pair<int, int> pre = log[target.first][target.second];
            while(true){
                // 공격 포탑인 경우 정지
                if(pre.first == attacker.first && pre.second == attacker.second)
                    break;
                // 공격 경로에 존재하는 포탑 피해
                field[pre.first][pre.second] -= (damage/2);
                if(field[pre.first][pre.second] < 0)
                    field[pre.first][pre.second] = 0;
                pre = log[pre.first][pre.second];
            }
            return true;
        }
        //우-하-좌-상 경로로 추가
        for(int i = 0; i < 4; ++i){
            pair<int, int> next;
            next.first = (cur.first + dr[i]);
            if(next.first == 0)
                next.first = N;
            else if(next.first == N+1)
                next.first = 1;
            next.second = (cur.second + dc[i]);
            if(next.second == 0)
                next.second = M;
            else if(next.second == M+1)
                next.second = 1;
            // 이전에 방문하지 않았고 파괴된 포탑이 아닌 경우, 이동경로에 추가
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
            // 부서진 포탑인 경우
            if(!field[row][col])
                continue;
            // 1. 공격력이 가장 높은 포탑
            if(power < field[row][col]){
                target.first = row; target.second = col;
                power = field[row][col];
            }
            else if(power == field[row][col]){
                // 2. 공격한지 가장 오래된 포탑
                if(attack[row][col] < attack[target.first][target.second]){
                    target.first = row; target.second = col;
                    power = field[row][col];
                }
                else if(attack[target.first][target.second] == attack[row][col]){
                    // 3. 행과 열의 합이 가장 작은 포탑
                    if(row + col < target.first + target.second){
                        target.first = row; target.second = col;
                        power = field[row][col];
                    }
                    // 4. 열 값이 가장 작은 포탑
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
            // 부서진 포탑인 경우
            if(!field[row][col])
                continue;
            // 1. 공격력이 가장 낮은 포탑
            if(field[row][col] < power){
                attacker.first = row; attacker.second = col;
                power = field[row][col];
            }
            else if(field[row][col] == power){
                // 2. 가장 최근에 공격한 포탑
                if(attack[attacker.first][attacker.second] < attack[row][col]){
                    attacker.first = row; attacker.second = col;
                    power = field[row][col];
                }
                else if(attack[attacker.first][attacker.second] == attack[row][col]){
                    // 3. 행과 열의 합이 가장 큰 포탑
                    if(attacker.first + attacker.second < row + col){
                        attacker.first = row; attacker.second = col;
                        power = field[row][col];
                    }
                    //4. 행과 열의 합이 동일한 경우 - 열의 값이 가장 큰 포탑
                    else if(attacker.first + attacker.second == row+col && attacker.second < col){
                        attacker.first = row; attacker.second = col;
                        power = field[row][col];
                    }
                }
            }
        }
    }

    return attacker;
}

void copyFieldTo(int destination[][MAX_F]) {
    for (int i = 1; i <= N; ++i)
        for (int j = 1; j <= M; ++j)
            destination[i][j] = field[i][j];
}

bool fix(int compare[][MAX_F], pair<int, int> attacker){
    int count = 0;
    for (int i = 1; i <= N; ++i){
        for (int j = 1; j <= M; ++j){
            // 부서진 포탑인 경우 또는 공격자인 경우
            if(field[i][j] == 0 || (attacker.first == i && attacker.second == j))
                continue;
            // 공격과 무관했던 포탑 정비
            if(compare[i][j] == field[i][j])
                ++field[i][j];
            ++count;
        }
    }
    return count;
}

int main() {
    cin >> N >> M >> K;
    for(int i = 1; i <= N; ++i)
        for(int j = 1; j <= M; ++j)
            cin >> field[i][j];

    pair<int, int> attacker, target;
    int backup_field[MAX_F][MAX_F];
    for(int round = 1; round <= K; ++round){
        // 변화된 field 확인 하기 위해 backup_field copy
        copyFieldTo(backup_field);
        // 1. 공격자 선정
        attacker = findAttacter();
        // 2. 공격 대상자 선정
        target = findTarget();
        // 공격자 공격력 Upgrade
        field[attacker.first][attacker.second] += UPGRADE_POWER;
        // 최종 공격 ROUND 초기화
        attack[attacker.first][attacker.second] = round;
        // 3. 공격(레이저 공격) - 실패시 포탄공격
        if(!laserAttack(attacker, target)){
            cannonAttack(attacker, target);
        }
        // 4. 포탑정비
        if(!fix(backup_field, attacker))
            break;
    }
    // 5. 가장 강한 포탑 찾기
    target = findTarget();
    cout << field[target.first][target.second] << "\n";

    return 0;
}