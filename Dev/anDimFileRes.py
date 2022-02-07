
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
    
    bks = "999999999"
    area = "999999999"
    timeStr = "999999999"

    for line in contents:
        arr = line.split(' ')
        # print(arr)
        firstPara = arr[0]
        
        # print(firstPara)
        if isFloat(firstPara):
            bks = firstPara.replace(".","")
            timeStr = arr[1].replace("\n","")
            # print(float(firstPara))
        else:
            if firstPara == "Primal":
                area = arr[2].replace("\n","")
                # print("area:",float(arr[2]))

    return {"bks":bks,"area":area,"time":timeStr,"path":path}

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

def writecsv(path,data):

    # 1. 创建文件对象
    f = open(path,'a',encoding='utf-8',newline='')

    # 2. 基于文件对象构建 csv写入对象
    csv_writer = csv.writer(f)

    csv_writer.writerow(data)

    # 5. 关闭文件
    f.close()

import shutil
def solverOneInsbestToZip(ins,data):
    
    min1 = 999999999
    mindr = "-1"
    for one in data:
        if float(one["area"]) < min1:
            min1 = float(one["area"])
            mindr = one["path"]

    try:
        print("mindr:",mindr)
        shutil.copy(mindr, r"..\Final\\{0}".format(mindr.split('\\')[3]))
    except IOError as e:
        print("Unable to copy file. %s" % e)
        exit(1)
    except:
        print("Unexpected error:")
        exit(1) 
    finally:
        print("Unexpected error:")


def solverOneInsWriteCsv(ins,data):

    allrun = ["run10","run11","run12","run13","run1401","run1402test","run1402","run1403","run2601","run2701","run3101","run0602"]
    order = []
    for i in range(len(allrun)):
        dr = allrun[i]
        ord1 = {'bks': '999999999', 'area': '999999999', 'time': '999999999', 'path': '-1'}
        for one in data:
            if one["path"].find(dr) != -1:
                one["path"] = str(i)
                ord1 = one
                break
        order.append(ord1)

    writeline = []
    min1 = 999999999
    mindr = "-1"
    for one in order:
        if float(one["area"]) < min1:
            min1 = float(one["area"])
            mindr = one["path"]
        print(one)
        writeline.append(one["bks"])
        writeline.append(one["area"])
        writeline.append(one["time"])
        writeline.append(one["path"])
    
    print("mindr:",mindr)
    writeline.append(mindr)
    writeline.append(min1)
    writeline = [ins] + writeline
    print("writeline:",writeline)
    writecsv("../Results/runall.csv",writeline)

if __name__ == '__main__':
    
    paths = getAllFilePath(r"..\commitFile")
    
    mp = {}

    # for root, dirs, files in os.walk(r"..\commitFile"):
    #     print(dirs)

    for ph in paths:
        ins = ph.split("DIMACS-VRPTW-HustSmart-")[1].split(".out")[0]
        info = getOneFileInfo(ph)
        if ins not in mp:
            mp[ins] = []
        mp[ins].append(info) 
        # print(info)
        # break
    for ins,data in mp.items():
        print(ins)
        solverOneInsWriteCsv(ins,data)
        # solverOneInsbestToZip(ins,data)



