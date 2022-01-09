
import os

#使用r模式打开文件，做读取文件操作
#打开文件的模式，默认就是r模式，如果只是读文件，可以不填写mode模式
file_handle=open(r'../commitFile/DIMACS-VRPTW-HustSmart-C1_2_1.out',mode='r')

contents = file_handle.readlines()
for line in contents:
    print(line)
    print(line.find("Primal Integral: "))

