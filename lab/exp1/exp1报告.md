# exp1报告

## Astar搜索

### 问题描述

⼆进制迷锁具有⼀个大小为 $N\times N$的拨轮锁盘, 每⼀个格点上具有⼀个可转动的拨轮，上面刻着 数字 0和1（表示非锁定）和 （锁定）。由于拨轮之间相互链接的关系，拨轮切换锁定的规则如下： 只能同时转动相邻呈“L”字形（四个方向的朝向均可）的三个拨轮，将它们同时由各⾃的锁定切换为非锁定状态，或从非锁定切换为锁定状态。

​	![image-20230523103443539](C:\Users\77089\AppData\Roaming\Typora\typora-user-images\image-20230523103443539.png)

- 为这个问题设计⼀个合适的启发式函数，并证明它是 admissible 的，并论证其是否满⾜ consistent 性质。 
- 根据上述启发式函数，开发对应的 A* 算法找到⼀个解法，将它恢复为全 状态以解开这个迷锁。 
- 设置启发式函数为 0，此时 A* 退化为 Dijkstra 算法，⽐较并分析使⽤ A* ⽅法带来的优化效果。

### 实验过程

----



1. 启发式函数设计

   首先是把单步cost设置为3

   一共设置了4种启发式

   - 可采纳的启发式$h(lock) = n$，其中n是1的个数。

     可采纳性证明:

     ​		任意一个1，至少都需要一步解锁，即cost为3去复原，而一次解锁最多复原3个1，故无论如何，解锁的代价都会大于等于估计的代价。因此$h(n)<=h'(n)$，因此这个启发式是可采纳的

     一致性证明：

     ​		即证$h(n)\le 3+h(n')$

     因为每一步至多减少3个1，所以$h(n')\ge h(n)-3$，证明显然成立

     至于把每一步cost设置为3，是因为这样对距离的刻画粒度更细，可以更好的区分结点间的好坏

   - 类似于贪心算法，$h(n) = 6*number\_of\_one(n)$

     这是非可采纳的启发式

     设计这个启发式的思路是问题规模变大后，会导致储存空间指数级上升，因此，减少深度和扩展的结点是极为重要的

     这个启发式把1的个数乘以6，在下一步扩展时就会优先扩展那些消去更多1的结点，从而节约了空间

   - 退化为Dijkstra算法，直接令$h(n) = 0$

   - 依据结构的启发式

     

2. 算法设计

   - 结点设计
   
        ```cpp
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
        ```
   
        这里用了一个2维的vector储存锁的格局，同时，每一个结点保存了之前的路径(带来约2倍的空间开销)
   
        同时，重载了node间的比较，以放入优先队列中
   
   - open list和close list的设计
   
     ```cpp
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
     priority_queue<node> open_list{};
     unordered_set<long long>visited{};
     ```
   
     `openlist`用一个优先队列存放，以扩展最优的结点
   
     `closelist`用一个哈希表存放，以节约空间
   
   - A star核心算法
   
     参考ppt上算法
   
     ![image-20230523111535068](C:\Users\77089\AppData\Roaming\Typora\typora-user-images\image-20230523111535068.png)
   
     
   
     ```cpp
     open_list.push(init);
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
         }
     if(!flag){
             out<<"No valid solution.";
     }
     ```
   
     首先取出顶上结点，判断是否解锁，是，则终止搜索，并打印解
   
     反之则将顶上结点做哈希放入closeList，并且以当前结点为基础，扩展出后面的结点
   
     并且提前做出剪枝和限制搜索深度
   
   - 剪枝算法
   
     主要是判断了此次操作是否有意义
   
     ```cpp
     inline bool is_valid_op(node &n,short i,short j,short k){
         //判断这次操作是否合理
         if(i+dy[k]>=0&&i+dy[k]<n._n&&j+dx[k]>=0&&j+dx[k]<n._n){// in bound
             if(n._blocks[i][j]!=0 || n._blocks[i+dy[k]][j]!=0 || n._blocks[i][j+dx[k]]!=0){
                 return true;//没有1的旋转无意义
             }
         }
         return false;
     }
     ```
   
     判断是否越界。以及对3个0的反转无意义
   
   ### 实验结果
   
   ----
   
   1. 使用A*启发式算法
   
      在输入规模为7*7及以下时，采用可采纳的启发式，即$n$，得到以下结果
      
      ```shell
      D:\ai2023\USTC-AI2023\lab\exp1\astar\src> .\a.exe
      Now Processing input0
      time used:0.377293 second(s)
      Max node count:128629
      Now Processing input1
      time used:0.004987 second(s)
      Max node count:1957
      Now Processing input2
      time used:0.104047 second(s)
      Max node count:41759
      Now Processing input3
      time used:5.25061 second(s)
      Max node count:2032496
      Now Processing input4
      time used:11.1568 second(s)
      Max node count:4000202
      Now Processing input5
      time used:5.5467 second(s)
      Max node count:2056150
      ```
      
      在input 4时扩展出了400万个结点，此时消耗内存为1.2GB
      
      在输入规模更大时，使用可采纳的启发式会导致内存的不足，16GB的内存完全无法承受。
      
      因此我只能选择求解出次优解，使用了
      
      
      
      
      
   2. 不使用启发式函数
   
      ![image-20230524192022219](C:\Users\77089\AppData\Roaming\Typora\typora-user-images\image-20230524192022219.png)
   
      



   

   

