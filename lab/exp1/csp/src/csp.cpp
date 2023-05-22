#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<memory>
#include<random>
#include<stack>
#include<map>
#include<algorithm>
using namespace std;
int N;
int D;
int S;
const unsigned long long maxIterLimit = 100000000;
vector<vector<vector<bool>>>request{};//第i天,j个阿姨,第k天的请求
vector<vector<vector<uint8_t>>>job_aunt{};//第i天,j班,阿姨的一个列表
vector<vector<uint8_t>>table{};//排班表

/*
    用于记录第i天j班的分配,如果这一次分配符合约束,则进入栈中,
*/
struct step{
    int _day; //第day天
    int _schedule;//第schedule班
    vector<vector<uint8_t>>_table;//当前已经分配好的班次
    vector<uint8_t>_chooseRecord{};//记录本次操作已经尝试选择过的阿姨
    bool checkSameWork() const;//检查该操作是否满足不重复工作
    // bool checkEveryoneHasJob();//检查每一个人是否都有工作
    bool checkMinWorkCount() const;//检查是否还能满足每一个阿姨的最少工作数
    step(int day,int schedule,vector<vector<uint8_t>>table);
};


/*
    检查到第i天,第j班(不包括j班)是否满足不连续排班的硬约束;
*/
bool step::checkSameWork() const {
    //检查同一天的
    for(int day=0;day<_day;day++){
        for(int schedule=0;schedule<S-1;schedule++){
            if(_table[day][schedule]==_table[day][schedule+1]){
                return false;
            }
        }
    }
    //检查最后一天的
    for(int schedule=0;schedule<_schedule-1;schedule++){
        if(_table[_day][schedule]==_table[_day][schedule+1]){
            return false;
        }
    }
    //检查跨天的,但最后一天不检查
    for(int day=0;day<_day;day++){
        if(day==_day-1 && _schedule == 0){
            continue;
        }
        if(_table[day][S-1]==_table[day+1][0]){
            return false;
        }
    }
    return true;
}
/*
    检查是否满足至少工作的数量
*/
bool step::checkMinWorkCount() const {
    map<uint8_t,int>record{};
    for(uint8_t i=0;i<N;i++){
        int cnt = 0;
        for(int day=0;day<=_day;day++){
            cnt += count(_table[day].begin(),_table[day].end(),i);
        }
        record[i] = cnt;
    }
    //统计已经分配的数量
    int req = 0;
    int minRequest = D*S/N;
    for(auto &eachRecord:record){
        if(eachRecord.second<minRequest){
            req += (minRequest - eachRecord.second);
        }
    }
    if(req>D*S-(_day*S)-_schedule){
        return false;
    }
    return true;
}
/*


*/
step::step(int day,int schedule,vector<vector<uint8_t>>table){
    _day = day;
    _schedule = schedule;
    _table = table;
}


void csp(string outPath){
    //
    vector<vector<uint8_t>>init_table{D,vector<uint8_t>(S,0xff)};
    shared_ptr<step>initStep = make_shared<step>(0,0,init_table);
    
    //此结构用于回溯backtrack
    stack<shared_ptr<step>>backTrackStack;
    backTrackStack.push(initStep);

    //随机化选择值
    random_device seed;
    ranlux48 engine{seed};
    uniform_int_distribution<> distrib(0,N-1);

    //打开文件
    ofstream out;
    out.open(outPath);

    while(!backTrackStack.empty()){
        auto top = backTrackStack.top();
        if(top->_day==D){
            /*
                TODO:
                这里统计满足的请求数并且输出排班表
            */
           int cnt = 0;
           for(int day = 0;day<D;day++){
            for(int schedule = 0;schedule<S;schedule++){
                if(find(job_aunt[day][schedule].begin(),job_aunt[day][schedule].end(),top->_table[day][schedule])!=job_aunt[day][schedule].end()){
                    cnt++;
                }
                out<<top->_table[day][schedule];
                if(schedule!=S-1){
                    out<<',';
                }else{
                    out<<'\n';
                }
            }
            out<<cnt;
            out.close();
           }
           break;
        }
        int n = 10 + job_aunt[top->_day][top->_schedule].size();
        if(top->_chooseRecord.size()>=min({n,N})){
            /*
                左边的常数可以根据实践调整
                尝试次数过多,直接放弃
            */
            backTrackStack.pop();
            continue;
        }


        //根据最上面的状态进行扩展
        bool flag = false;//判断能否进栈，如果不能进栈就维持循环
        bool findReq = true;//表示在想申请这一班的人中选
        

        //
        if(top->_chooseRecord.size() >= job_aunt[top->_day][top->_schedule].size()){
            findReq = false;
        }

        //统计每一班已经分配的
        map<uint8_t,int>schedule_count;
        for(int i = 0;i < N;i++){
            int cnt = 0;
            for(int day = 0;day < D;day++){
                cnt += count(top->_table.begin(),top->_table.end(),i);
            }
            schedule_count[i] = cnt;
        }
        shared_ptr<step>nextStep = make_shared<step>(top->_day+(top->_schedule+1==S),(top->_schedule+1==S)?0:top->_schedule+1,top->_schedule);
        while(!flag){
            if(findReq){
                //需要在申请这一班的人中找
                uint8_t minAunt = 0;
                int minCount = 1000000;
                
            }
            


        }

    }


    //表示搜不到解
    if(backTrackStack.empty()){
        out<<"No valid schedule found.";
        out.close();
    }
}
int main(){
    string base_in{"../input/input"};
    string base_out{"../output/output"};
    char fileNames[] = {'0','1','2','3','4','5','6','7','8','9'}; 
    char comma;
    for(int i = 0; i<10;i++){
        ifstream in;
        in.open(base_in+fileNames[i]+".txt");
        in>>N>>comma>>D>>comma>>S;
        //初始化各种表;
        for (int day  = 0; day < D; day++)
        {
            vector<vector<uint8_t>> each_day{S};
            job_aunt.push_back(each_day);
            vector<uint8_t>each_day_job{S};
            table.push_back(each_day_job);
        }
        //读入文件数据
        for(int day = 0;day<D;day++){
            vector<vector<bool>>dayJob{};
            for(int worker = 0;worker<N;worker++){
                vector<bool>workerJob{};
                for(int jobNo = 0;jobNo<2*S-1;jobNo++){
                    if(jobNo%2==0){
                        bool choose;
                        in>>choose;
                        workerJob.push_back(choose);
                        if(choose){
                            job_aunt[day][jobNo/2].push_back(worker);
                        }
                    }else{
                        in>>comma;
                    }
                }
                dayJob.push_back(workerJob);
            }
            request.push_back(dayJob);
        }
    }

}

/*
    local search废案
    先对排班表初始化,为了防止重叠,用0xff初始化,如果有阿姨请求这一班则随意选取一个想去的阿姨
    没有阿姨想去就随便挑一个阿姨去
*/
// void init_table(){
    // random_device seed;
    // ranlux48 engine{seed};
    // uniform_int_distribution<> distrib(0,999);
//     for(int i=0;i<D;i++){
//         for(int j=0;j<S;j++){
//             if(job_aunt[i][j].size()){
//                 table[i].push_back(job_aunt[i][j][distrib(engine)%job_aunt[i][j].size()]);
//             }
//             else{
//                 table[i].push_back(distrib(engine)%N);
//             }
//         }
//     } 
// }