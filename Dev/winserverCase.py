
import sys

all53Cases = [
	
"C1_10_5", "C1_10_6", "C1_10_7", "C1_10_8", "C1_8_5", "C1_8_6", "C1_8_7", 
"C1_8_8", "C1_6_5", "C1_6_6", "C1_6_7", "C1_4_5", "C1_4_6", "C1_4_7", 
"C1_4_8", "C1_2_5", "C1_2_6", "C1_2_7", "C1_2_8", 
	
"C2_10_1", "C2_10_2", "C2_10_5", "C2_10_6", "C2_10_7", "C2_10_9", "C2_8_1", "C2_8_2", "C2_8_3",
"C2_8_5", "C2_8_6", "C2_8_7", "C2_8_8", "C2_8_9", "C2_8_10", "C2_6_1", "C2_6_5", "C2_6_6",
"C2_6_7", "C2_4_1", "C2_4_2", "C2_4_5", "C2_4_6", "C2_4_7", "C2_4_9",

		"RC2_10_1", "RC2_8_1", "RC2_8_2", "RC2_6_1", "RC2_6_2", "RC2_4_1", "RC2_4_2", "RC2_2_1", "RC2_2_2"
]

EP012Cases = [
"RC2_2_1",
"RC2_4_1",
"RC2_8_2",
"RC2_10_1",
"C2_8_7",
"C1_2_8",
"C1_4_7",
"C2_10_9",

"RC2_2_2",
"RC2_6_2",
"C2_4_2",
"C1_2_7",
"C1_4_8",
"C1_10_7",
"C2_6_7",
"C2_8_10",
"C2_10_5",
"C2_10_2",
"C1_2_6",
"C1_4_6",
"C1_8_7"]

EP0Cases = [
"RC2_2_1",
"RC2_4_1",
"RC2_8_2",
"RC2_10_1",
"C2_8_7",
"C1_2_8",
"C1_4_7",
"C2_10_9",
]

print(len(all53Cases))
print(all53Cases)

cases = EP0Cases

print(len(cases))
# strBase = "start cmd /k \"dimacsVRP.exe 1 C2_10_2 36000\""
path = "./ep0_2h.bat"

print(str(sys.argv))
print(len(sys.argv))

part = 11
part = min(part,len(cases))
runTime = "7200"
# runTime = "86400"
isBreak = "1"

if len(sys.argv) > 1:
	isBreak = sys.argv[1]

if len(sys.argv) > 2:
	part = int(sys.argv[2])

if len(sys.argv) > 3:
	runTime = sys.argv[3]

print(part)
print(runTime)

ret = ["" ] * part

print( ret )

for i in range(0,len(cases),part):
	for j in range(0,part,1):
		if i+j < len(cases):
			print("dimacsVRP.exe " + isBreak + " " + cases[i+j] + " " + runTime + "&&")
			ret[j] += "dimacsVRP.exe " + isBreak + " " + cases[i+j] + " " + runTime + "&&"
	print('\n')


file_write_obj = open(path, 'w')

    

for i in range(len(ret)):

	str = "start cmd /k \"{0}\"".format(ret[i])[0:-3] + "\""
	print(str )

	file_write_obj.writelines(str+"\n")
	file_write_obj.writelines("timeout /t 3"+"\n")

# for line in open(path,"r"): #设置文件对象并读取每一行文件
#     print(line)               #将每一行文件加入到list中
