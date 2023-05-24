#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<memory>
#include<random>
#include<stack>
#include<map>
#include<algorithm>
#include <chrono>
using namespace std;
struct cspSolver{
    int N{};
    int D{};
    int S{};
    size_t exchangeTimes{};
    string outPath;
    vector<vector<int>>job_aunt{};//统计每一班想去的
    map<int,int>aunt_count{};//统计每个阿姨请求个数
    vector<int>table{};//排班表
    vector<int>notBestNum{};//代表第i班的分配不满足请求
    void initTable();
    void csp(); 
    void print();
    void localSearch();//爬山法搜索最优解
    bool CheckSameWork();//检查重复工作,由于分配策略,公平性是绝对满足的
    inline void exchangeJob(int index1,int index2){
        table[index1]^=table[index2];
        table[index2]^=table[index1];
        table[index1]^=table[index2];
    }
};
void cspSolver::initTable(){
    for(int i=0;i<N;i++){
        aunt_count[i] = 0;
    }
    for(int i=0;i<D*S;i++){
        for(auto &x:job_aunt[i]){
            aunt_count[x] += 1;
        }
    }


    //初始化排班表,直接轮流分配
    vector<int>initList{};
    for (int i = 0; i < N; i++)
    {
        initList.push_back(i);
    }
    sort(initList.begin(),initList.end(),[this](int aunt1,int aunt2){return aunt_count[aunt1]>aunt_count[aunt2];});
    auto x = initList.begin();
    for (int i = 0; i < D*S; i++)
    {
        table.push_back(*x);
        if(++x == initList.end()){
            x = initList.begin();
        }
    }
    //统计不好的分配
    for(int i = 0; i < D*S; i++){
        //在请求这班的人找不到
        if(find(job_aunt[i].begin(),job_aunt[i].end(),table[i])==job_aunt[i].end()){
            notBestNum.push_back(i);
        }
    }
}
bool cspSolver::CheckSameWork(){
    for(int i = 0;i<D*S-1;i++){
        if(table[i] == table[i+1]){
            return false;
        }
    }
    return true;
}
void cspSolver::localSearch(){
    int maxAttempt = 100;//最大失败尝试搜索次数
    int attempt = 0;

    
    random_device seed;
    std::mt19937 engine{seed()};
    while (attempt<maxAttempt)
    {
        if(notBestNum.empty()){
            //已经达到最优
            break;
        }
        //采用随机交换结点的方式!
        //如果采用修改结点的方式,规模将会是无法承受的
        uniform_int_distribution<> distrubution(0,notBestNum.size()-1);
    
        //随机选一个不能满足的班次
        int failedIndex = notBestNum[distrubution(engine)];
        if(!job_aunt[failedIndex].size()){
            attempt+=1;
        }
        //遍历所有排班,尝试交换,是否有更优的分配
        for(int i=0;i<D*S;i++){
            if(i==failedIndex || table[i]==table[failedIndex]){
                //不能自己换自己,也不能别人
                continue;
            }else{
                exchangeJob(failedIndex,i);
                if(!CheckSameWork()){
                    //破坏约束
                    exchangeJob(failedIndex,i);
                    continue;
                }else{
                    //不破坏约束,则判断是否会使得当前状态更优,是则保留
                    //现在i是原来的failed,failed是原来的i

                    //表明原来的第i个是满足的
                    int checkAuntISatifaction = static_cast<int>(find(job_aunt[i].begin(),job_aunt[i].end(),table[failedIndex])!=job_aunt[i].end());

                    //检查现在的满足情况
                    //原来的failed情况
                    int checkFailedO = static_cast<int>(find(job_aunt[failedIndex].begin(),job_aunt[failedIndex].end(),table[failedIndex])!=job_aunt[failedIndex].end());
                    //选取的i满足情况
                    int checkI = static_cast<int>(find(job_aunt[i].begin(),job_aunt[i].end(),table[i])!=job_aunt[i].end());

                    if(checkI+checkFailedO>checkAuntISatifaction){
                        //达到了一种更优的情况
                        exchangeTimes++;
                        if(checkFailedO){
                            auto it = find(notBestNum.begin(),notBestNum.end(),failedIndex);
                            if(it == notBestNum.end()){
                                cerr<<(failedIndex)<<"error1\n";
                                for(auto i:notBestNum){
                                    cerr<<i<<"\t";
                                }
                            }
                            notBestNum.erase(it);
                        }
                        if(checkI && !checkAuntISatifaction){
                            auto it = find(notBestNum.begin(),notBestNum.end(),i);
                            if(it == notBestNum.end()){
                                cerr<<"error2";
                            }
                            notBestNum.erase(it);
                        }
                        attempt=0;
                        break;
                    }else{
                        //搜不到就回退
                        exchangeJob(failedIndex,i);
                    }
                }
            }
            
            
        }
        attempt++;

    }
    
}

void cspSolver::print(){
    auto cnt = 0;
    ofstream out;
    out.open(outPath);  
    for(int day = 0; day < D; day++){
        for(int schedule = 0; schedule < S; schedule++){
            if(find(job_aunt[day*S+schedule].begin(),job_aunt[day*S+schedule].end(),table[day*S+schedule])!=job_aunt[day*S+schedule].end()){
                //统计满足的请求数
                cnt++;
            }
            out<<static_cast<int>(table[day*S+schedule]+1);
            if(schedule!=S-1){
                out<<',';
            }else{
                out<<'\n';
            }
        }
    }
    out<<cnt;
    out.close();
}
void cspSolver::csp(){
    auto start = chrono::high_resolution_clock::now();
    initTable();
    localSearch();
    print();
    auto end = chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    double seconds = duration.count();
    cout<<"time used:"<<seconds<<" second(s)"<<endl;
    cout<<"Total exchange times:"<<exchangeTimes<<endl;
}


int main(){
    string base_in{"../input/input"};
    string base_out{"../output/output"};
    char fileNames[] = {'0','1','2','3','4','5','6','7','8','9'}; 
    char comma;
    auto start = chrono::high_resolution_clock::now();
    for(int i = 0; i<10;i++){
        ifstream in;
        in.open(base_in+fileNames[i]+".txt");
        cspSolver solver{};
        solver.outPath = base_out+fileNames[i]+".txt";
        in>>solver.N>>comma>>solver.D>>comma>>solver.S;
        for (int schedule  = 0; schedule < solver.D*solver.S; schedule++)
        {
            vector<int>eachJob{};
            solver.job_aunt.push_back(eachJob);
        }
        for(int auntNo = 0;auntNo<solver.N;auntNo++){
            solver.aunt_count[auntNo] = 0;
        }
        //读入文件数据
        bool willing;
        
        for(int worker = 0;worker<solver.N;worker++){
            for(int day = 0;day<solver.D;day++){
                for(int jobNo = 0;jobNo<solver.S;jobNo++){
                    in>>willing;
                    if(willing){
                        solver.job_aunt[day*solver.S+jobNo].push_back(worker);
                    }
                    if(jobNo!=solver.S-1){
                        in>>comma;
                    }
                }
            }
        }
        in.close();
        cout<<"Now processing input"<<fileNames[i]<<endl;
        solver.csp();
    }
    auto end = chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    double seconds = duration.count();
    cout<<"All inputs used:"<<seconds<<" second(s)"<<endl;
}

