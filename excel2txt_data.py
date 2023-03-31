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

num = range(0, len(id))
print(len(id))
print()
for j in num:
    print("\""+str(file_name[j])+"\",")
print()
for i in num:
    # print("{", str(int(id[i]))+",", 88,154,59,256,214,166})
    print("{", str(round(id[i]))+",", str(round(min_x[i]))+",", str(round(min_y[i]))+",", str(round(min_z[i]))+",",
          str(round(max_x[i]))+",", str(round(max_y[i]))+",", str(round(max_z[i])), "},")
print()
for k in num:
    print("{", str(round(id[k]))+",0,0,0},")
print()
print(m_x, m_y, m_z)