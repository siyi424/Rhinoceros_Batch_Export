import pandas as pd
from collections import OrderedDict

ID_Dens = pd.read_excel('AllPhantoms_OrganID.xlsx', sheet_name='UnitedID')
orgs = pd.read_excel('output.xlsx', sheet_name='Sheet1')

OrganNum = 0
OrganFlag = ""
Dens = ""

ids = {}
for row, data in orgs.iterrows():
    id = data['id']
    if id in ids:
        continue

    r = ID_Dens[ID_Dens['OrganID'] == id].index[0]
    Density = ID_Dens.iloc[r,3]
    ids[id] = Density
    OrganNum += 1

ids = OrderedDict(sorted(ids.items(), key=lambda x: int(x[0])))
for key, value in ids.items():
    OrganFlag += str(key) + ","
    Dens += str(value) + ","


with open('info.txt','w+') as f:
    res = ""
    res += str(OrganNum) + "\n\n"
    res += "{" + OrganFlag + "};\n\n"
    res += "{" + Dens + "};"
    f.write(res)


