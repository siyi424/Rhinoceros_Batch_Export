import pandas as pd
import numpy as np


ex = pd.read_excel("output.xlsx")
file_name = list(ex[ex.columns[0]])
id = list(ex[ex.columns[1]])
min_x = list(ex[ex.columns[2]])
min_y = list(ex[ex.columns[3]])
min_z = list(ex[ex.columns[4]])
max_x = list(ex[ex.columns[5]])
max_y = list(ex[ex.columns[6]])
max_z = list(ex[ex.columns[7]])

arr_x = np.full(len(id), min(min_x))
arr_y = np.full(len(id), min(min_y))
arr_z = np.full(len(id), min(min_z))

min_x = list(min_x - arr_x)
min_y = list(min_y - arr_y)
min_z = list(min_z - arr_z)
max_x = list(max_x - arr_x)
max_y = list(max_y - arr_y)
max_z = list(max_z - arr_z)

m_x = int(max(max_x))
m_y = int(max(max_y))
m_z = int(max(max_z))

num = len(id)

# "res" is the content going to be written into "wrldata.txt"
res = str(num) + '\n\n'

for j in range(0, num):
    res += '"' + str(file_name[j]) + '", \n'

res += '\n'
for i in range(0, num):
    # print("{", str(int(id[i]))+",", 88,154,59,256,214,166})
    res += "{" + str(round(id[i]))+", " + str(round(min_x[i]))+", " + str(round(min_y[i]))+", " + str(round(min_z[i]))+", " + str(round(max_x[i]))+", " + str(round(max_y[i]))+", " + str(round(max_z[i])) + "}, \n"

res += '\n'
for k in range(0, num):
    res += "{" + str(round(id[k])) + ", 0, 0, 0}," + "\n"

res += '\n'
res += str(m_x) + " " + str(m_y) + " " + str(m_z)


f = open('wrldata.txt','a')
f.write(res)
f.close()

