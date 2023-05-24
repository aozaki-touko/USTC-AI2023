import os
dx = [0,1,-1,-1,1]
dy = [0,-1,-1,1,1]
def checkAstar() :
    def change(x):
        if(x=='0'):
            return False
        return True
    def checkValidTurn(scale,i,j,k):
        if(i+dy[k]>=0 and i+dy[k]<scale and j+dx[k]>=0 and j+dx[k]<scale):
            return True
        return False
    def turn(configuration:list,i,j,k):
        assert(checkValidTurn(len(configuration),i,j,k)==True)
        configuration[i][j] = not configuration[i][j]
        configuration[i+dy[k]][j] = not configuration[i+dy[k]][j]
        configuration[i][j+dx[k]] = not configuration[i][j+dx[k]]
        return configuration
        
    for i in range(0,10):
        inputPath = './astar/input/input'+str(i)+'.txt'
        outputPath = './astar/output/output'+str(i)+'.txt'

        #get problem
        file = open(inputPath)
        problemScale = int(file.readline().replace('\n',''))
        configuration = []
        for line in file:
            configuration.append(list(map(change,line.split())))
        file.close()
        #get solution
        file = open(outputPath)
        totalStep = int(file.readline().strip())
        cnt = 0
        for line in file:
            cnt += 1
            op=list(map(int,line.split(',')))
            configuration = turn(configuration,op[0],op[1],op[2])
        file.close()
        assert(totalStep == cnt)
        for x in range(problemScale):
            for y in range(problemScale):
                assert(configuration[x][y] == False)
        print("astar-test" + str(i) + ' : passed')


def checkCsp():
    def sol(x):
        return int(x)-1
    def change(x):
        if x=='0':
            return False
        return True
    def checkSameWork(table):
        for i in range(len(table)-1):
            if(table[i]==table[i+1]):
                return False
        return True
    def checkMinWork(table:list,cnt,minReq):
        for i in range(cnt):
            if(table.count(i)<minReq):
                return False
        return True
    def checkReqCnt(table:list,cnt,Req:list,totalJobs,expect):
        totalSAT = 0
        for i in range(totalJobs):
            if Req[i].count(table[i]) > 0:
                totalSAT += 1
        if(totalSAT == expect):
            return True
        return False
        
    for i in range(0,10):
        
        inputPath = './csp/input/input'+str(i)+'.txt'
        outputPath = './csp/output/output'+str(i)+'.txt'

        #get problem
        file = open(inputPath)
        problemScale = list(map(int,file.readline().strip().split(',')))
        request = [[] for _ in range(problemScale[1]*problemScale[2])]
        workerCnt = 0
        dayCnt = 0
        for line in file:
            req = list(map(change,line.split(',')))
            if(workerCnt>=problemScale[0]):
                break
            #print(req)
            for schedule in range(problemScale[2]):
                if req[schedule] == True:
                    request[dayCnt*problemScale[2]+schedule].append(workerCnt)
            dayCnt += 1
            if dayCnt == problemScale[1]:
                workerCnt += 1
                dayCnt = 0
        file.close()
        #print(request)

        #get solution
        table = []
        file = open(outputPath)
        cnt = 0
        SAT = 0
        for line in file:
            if(cnt == problemScale[1]*problemScale[2]):
                SAT = int(line.strip())
                break
            cnt += problemScale[2]
            op=list(map(sol,line.split(',')))
            table.extend(op)
        file.close()
        assert(checkSameWork(table) == True)
        assert(checkMinWork(table,problemScale[0],problemScale[1]*problemScale[2]//problemScale[0]) == True)
        assert(checkReqCnt(table,problemScale[0],request,problemScale[1]*problemScale[2],SAT) == True)
        print("csp-test" + str(i) + ' : passed')
#checkAstar()
print(os.curdir)
checkAstar()
checkCsp()

        