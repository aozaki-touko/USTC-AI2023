#include<iostream>
#include<algorithm>
#include<string>
#include<vector>
#include<fstream>
#include<queue>
#include<unordered_set>
#include <chrono>
#include<set>
#define H h3
#define COST 1
using namespace std;
const long long P = 1000000007;  //
const long long MOD = 1000000009;  // 
int dx[5] = {0,1,-1,-1,1};
int dy[5] = {0,-1,-1,1,1};
struct operation{
    short _i,_j,_k;//k = 1,L 型,此后逆时针旋转
};
struct node{
    short _n;//n represent the size of lock max represent 64*64
    short _cost{};
    short _heuristic{};
    std::vector<std::vector<bool>>_blocks{};
    std::vector<operation>_path{};
    node():_n(0){}
    node(short n,vector<vector<bool>>blocks,vector<operation>path):_n(n),_blocks(blocks),_path(path){

    }
    bool operator<(const node &j)const{
        return _cost+_heuristic>j._cost+j._heuristic;
    }
};
bool is_goal(node &n){
    for(int i=0;i<n._n;i++){
        for (int j = 0; j < n._n; j++){
            if(n._blocks[i][j]!=false){
                return false;
            }
        }
    }
    return true;
}
inline bool is_valid_op(node &n,short i,short j,short k){
    //判断这次操作是否合理
    if(i+dy[k]>=0&&i+dy[k]<n._n&&j+dx[k]>=0&&j+dx[k]<n._n){// in bound
        if(n._blocks[i][j]!=0 || n._blocks[i+dy[k]][j]!=0 || n._blocks[i][j+dx[k]]!=0){
            return true;//没有1的旋转无意义
        }
    }
    return false;
}
short h1(node &n){
    //直接使用1的个数除以3
    short h{};
    for(int i=0;i<n._n;i++){
        for(int j=0;j<n._n;j++){
            h += n._blocks[i][j];
        }
    }
    if(n._n<7){
        return h;
    }
    return h*6;
}
short h2(node&n){
    return 0;
}
short h3(node &n){
    short cnt = 0;
    set<pair<int,int>>visited;
    //先统计4种L形状的
    for(int i = 0;i<n._n;i++){
        for(int j = 0;j<n._n;j++){
            //当前块已被访问，则跳过
            if(!n._blocks[i][j]){
                continue;
            }
            if(visited.find({i,j})!=visited.end()){
                continue;
            }
            for(int k = 1;k<5;k++){
                if(is_valid_op(n,i,j,k)){
                    bool isDyValid = n._blocks[i+dy[k]][j] && (visited.find({i+dy[k],j})==visited.end());
                    bool isDxValid = n._blocks[i][j+dx[k]] && (visited.find({i,j+dx[k]})==visited.end());
                    if(isDxValid && isDyValid){
                        visited.insert({i+dy[k],j});
                        visited.insert({i,j+dx[k]});
                        visited.insert({i,j});
                        cnt += 1;
                        break;
                    }
                }
            }
        }
    }
    //再统计两个连着的
    for(int i = 0;i<n._n;i++){
        for(int j = 0;j<n._n;j++){
            //当前块已被访问，则跳过
            if(!n._blocks[i][j]){
                continue;
            }
            if(visited.find({i,j})!=visited.end()){
                continue;
            }
            for(int k = 1;k<5;k++){
                if(is_valid_op(n,i,j,k)){
                    bool isDyValid = n._blocks[i+dy[k]][j] && (visited.find({i+dy[k],j})==visited.end());
                    if(isDyValid){
                        visited.insert({i+dy[k],j});
                        visited.insert({i,j});
                        cnt += 2;
                        break;
                    }
                    bool isDxValid = n._blocks[i][j+dx[k]] && (visited.find({i,j+dx[k]})==visited.end());
                    if(isDxValid){
                        visited.insert({i,j+dx[k]});
                        visited.insert({i,j});
                        cnt += 2;
                        break;
                    }
                }
            }
        }
    }
    //统计单独的
    for(int i = 0;i<n._n;i++){
        for(int j = 0;j<n._n;j++){
            //当前块已被访问，则跳过
            if(n._blocks[i][j] && visited.find({i,j})==visited.end()){
                cnt += 3;
            }

        }
    }
    return cnt;
}
node turn(node n,short i,short j, short k){
    node newState{n._n,n._blocks,n._path};
    newState._cost = n._cost+COST;
    newState._blocks[i][j] = !newState._blocks[i][j];
    newState._blocks[i+dy[k]][j] = !newState._blocks[i+dy[k]][j];
    newState._blocks[i][j+dx[k]] = !newState._blocks[i][j+dx[k]];
    newState._heuristic = H(newState);
    newState._path.push_back(operation{i,j,k});
    return newState;
}
long long getHashVal(node &n){
    long long hashValue = 0;
    long long base = 1;
    for (int i = 0; i < n._n; i++) {
        for (int j = 0; j < n._n; j++) {
            hashValue = (hashValue + base * n._blocks[i][j]) % MOD;
            base = (base * P) % MOD;
        }
    }
    return hashValue;
}
void astar(node &init, string savePath){
    size_t nodeCount = 0;
    auto start = chrono::high_resolution_clock::now();
    ofstream out;
    out.open(savePath);
    priority_queue<node> open_list{};
    unordered_set<long long>visited{};
    open_list.push(init);
    // cout<<init._heuristic;
    bool flag{};
        while(!open_list.empty()){
            node n = open_list.top();
            open_list.pop();
            if(is_goal(n)){
                flag = true;
                out<<n._path.size()<<'\n';
                for(auto &i:n._path){
                    out<<i._i<<','<<i._j<<','<<i._k<<'\n';
                }
                out.close();
                break;
            }
            long long hash = getHashVal(n);
            visited.emplace(hash);
            for(int i=0;i<n._n;i++){
                for(int j=0;j<n._n;j++){
                    for(int k=1;k<5;k++){
                        if(is_valid_op(n,i,j,k)){
                            node newState = turn(n,i,j,k);
                            if(n._path.size()>35){
                                continue;//防止规模过大
                            }
                            long long val = getHashVal(newState);
                            if(visited.find(val)==visited.end()){
                                open_list.push(newState);
                            }
                        }
                    }
                }
            }
            if(open_list.size()>nodeCount){
                nodeCount = open_list.size();
            }
        }
    auto end = chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    double seconds = duration.count();
    cout<<"time used:"<<seconds<<" second(s)"<<endl;
    cout<<"Max node count:"<<nodeCount<<endl;
    if(!flag){
        out<<"No valid solution.";
    }
}
int main(){
    string inPath = "../input/input";
    string outPath = "../output/output";
    char files[] = {'0','1','2','3','4','5','6','7','8','9'};
    for (short i = 0; i < 10; i++){
        string input = inPath + files[i] + ".txt";
        string output = outPath + files[i] + ".txt";
        ifstream in;
        in.open(input);
        bool s;
        int val;
        node init;
        in >> init._n;
        for(int j = 0; j<init._n;j++){
            vector<bool>line{};
            init._blocks.push_back(line);
            for(int k = 0; k<init._n;k++){
                in >> val;
                init._blocks[j].push_back(static_cast<bool>(val));
            }
        }
//        for(int j = 0; j<init._n;j++){
//            for(int k = 0; k<init._n;k++){
//                cout<<init._blocks[j][k]<<"\t";
//            }
//        }
        in.close();
        cout<<"Now Processing input"<<files[i]<<endl;
        init._heuristic = H(init);
        astar(init,output);
    }
}