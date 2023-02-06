
def isxclosedepot():

    return 0
    
def getOneFileInfo(path):
    #使用r模式打开文件，做读取文件操作
    #打开文件的模式，默认就是r模式，如果只是读文件，可以不填写mode模式
    file_handle=open(path,mode='r')

    contents = file_handle.readlines()
    
    bks = "999999999"
    area = "999999999"
    timeStr = "999999999"

    lineNo = 0
    for line in contents:
        lineNo = lineNo+1
        # print("lineNo:",lineNo,"line:",line)
        if lineNo < 10:
            continue
        
        arr = line.split('  ')
        arr = [i for i in arr if len(i) > 0]
        
        for i in range(len(arr)):
            arr[i] = arr[i].replace("\n","")
            arr[i] = int(arr[i])
        print(arr)

        # firstPara = arr[0]
        
        # # print(firstPara)
        # if isFloat(firstPara):
        #     bks = firstPara.replace(".","")
        #     timeStr = arr[1].replace("\n","")
        #     # print(float(firstPara))
        # else:
        #     if firstPara == "Primal":
        #         area = arr[2].replace("\n","")


getOneFileInfo(r"../Instances/Homberger/C1_2_1.txt")

