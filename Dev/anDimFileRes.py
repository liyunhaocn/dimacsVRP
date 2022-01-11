
import os
import csv

def isFloat(x):
    try:
        float(x)
        return True
    except:
        return False

def getOneFileInfo(path):
    #使用r模式打开文件，做读取文件操作
    #打开文件的模式，默认就是r模式，如果只是读文件，可以不填写mode模式
    file_handle=open(path,mode='r')

    contents = file_handle.readlines()
    
    bks = 0.0
    area = 0.0
    for line in contents:
        arr = line.split(' ')
        # print(arr)
        firstPara = arr[0]
        # print(firstPara)
        if isFloat(firstPara):
            bks = float(firstPara)*10
            # print(float(firstPara))
        else:
            if firstPara == "Primal":
                area = float(arr[2])
                # print("area:",float(arr[2]))

    return [bks,area]

# 遍历文件夹
def getAllFilePath(file):
    
    ret = []
    for root, dirs, files in os.walk(file):

        # root 表示当前正在访问的文件夹路径
        # dirs 表示该文件夹下的子目录名list
        # files 表示该文件夹下的文件list
        # 遍历文件
        
        for f in files:
            # print(os.path.join(root, f))
            ret.append(os.path.join(root, f))
    return ret

    # 遍历所有的文件夹
    # for d in dirs:
    #     print(os.path.join(root, d))

def writecsv(path,data):

    # 1. 创建文件对象
    f = open(path,'a',encoding='utf-8',newline='')

    # 2. 基于文件对象构建 csv写入对象
    csv_writer = csv.writer(f)

    for row in data:
        csv_writer.writerow(row)

    # 5. 关闭文件
    f.close()

if __name__ == '__main__':
    
    paths = getAllFilePath(r"..\commitFile" )

    mp = {}

    for ph in paths:
        ins = ph.split("DIMACS-VRPTW-HustSmart-")[1].split(".out")[0]
        info = getOneFileInfo(ph)
        mp[ins] = info 
        # break
    
    for k,v in mp.items():
        print([k,str(v[0]),str(v[1])])
        writecsv("../Results/run10.csv",[[k,str(v[0]),str(v[1])]])


