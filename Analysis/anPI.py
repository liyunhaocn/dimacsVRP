
# coding:utf-8
import csv
import os


def writecsv(path,data):

    # 1. 创建文件对象
    f = open(path,'a',encoding='utf-8',newline='')
    # 2. 基于文件对象构建 csv写入对象
    csv_writer = csv.writer(f)
    csv_writer.writerow(data)
    # 5. 关闭文件
    f.close()

def isFloat(x):
    try:
        float(x)
        return True
    except:
        return False

def readcsv(path):
    f = csv.reader(open(path,'r',encoding='utf-8'))
    ret = []
    for i in f:
        ret.append(i)
    return ret

# 遍历文件夹
def getAllFilePath(file):
    
    ret = []
    print(file)
    for root, dirs, files in os.walk(file):

        # root 表示当前正在访问的文件夹路径
        # dirs 表示该文件夹下的子目录名list
        # files 表示该文件夹下的文件list
        # 遍历文件
        for f in files:
            print(os.path.join(root, f))
            ret.append(os.path.join(root, f))
    return ret

if __name__ == '__main__':
    
    path = r"../PICom/PI.csv"
    
    cont = readcsv(path)
    
    ins = []
    
    for i in range(len(cont)):
        if i%2 == 1:
            ins.append(cont[i][1])

    for i in range(len(cont)):
        cont[i] = cont[i][3::]

    bks = []
    pi = []

    for i in range(1,len(cont)):
        if i %2 == 1:
            pi.append(cont[i])
        else:
            bks.append(cont[i])

    resPath = r"../Results/ph2teams.csv";
    head = ["ins"]
    for i in cont[0]:
        head.append(i)
        head.append("bks")
        
    writecsv(resPath,head)

    for i in range(len(pi)):
        
        arr = [ins[i]]

        for j in range(len(pi[i])):
            arr.append(pi[i][j])
            arr.append(bks[i][j])
        print(arr)
        writecsv(resPath,arr)