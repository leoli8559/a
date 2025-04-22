# encoding: utf-8
import os
import sys




fileName = sys.argv[1]
bridge_board_num = sys.argv[2]
name = os.path.basename(fileName)

#打开文件，读取所有行，储存到lines_list中，
#然后使用insert语句插入import board以及在文件结尾插入结束标志。
with open(fileName) as f1:
    lines_list = f1.readlines()
    i = 0       
    num = len(lines_list)
    while True:       
        if (lines_list[i].startswith('import')):
            lines_list.insert(i+1,'import board\nbridge_board_num = '+bridge_board_num+'\nboard.setId(bridge_board_num)\n')
            break
        i = i + 1
        if i >= num + 1:
            break;
    #lines_list.append('\nboard.WriteI2C(0,0,0)')        
    lines_list.append('\nboard.endI2C()')          
newName = name+'New.py'
#将处理过的lines_list写入新的文件中
with open(newName,'w') as f2:
    f2.writelines(lines_list)
    f2.close()

#运行新生成的文件    
os.system('python \"' + newName + '\"')
sys.exit(1)

