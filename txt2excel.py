import xlsxwriter
import re

# create a new execl
workbook = xlsxwriter.Workbook('output.xlsx')
worksheet = workbook.add_worksheet()

# set col names
col_names = ['id', 'min_x', 'min_y', 'min_z', 'max_x', 'max_y', 'max_z', 'size_x', 'size_y', 'size_z']
worksheet.write_row(0, 0, col_names)

# data processing
row = 1
id, min_x, min_y, min_z, max_x, max_y, max_z, size_x, size_y, size_z = [0] * 10
data = open('record.txt', 'r')
linelist = data.readlines()
for line in linelist:
    # write a row
    if  line == "--------------------------------------- \n":
        newline = [id, min_x, min_y, min_z, max_x, max_y, max_z, size_x, size_y, size_z]
        worksheet.write_row(row, 0, newline)
        row += 1

    # get id
    elif '.wrl' in line:
        id = line.split('.')[0]
        if '_' in id:
            id = int(id.split('_')[0])
        else:
            id = int(id)
    
    # get min loc 
    elif line.startswith('min = '):
        minLocs = re.findall("[+-]?[0-9]+\.[0-9]+", line)
        minLocs = [round(float(x), 2) for x in minLocs]
        min_x, min_y, min_z = minLocs
        
    # get max loc 
    elif line.startswith('max = '):
        maxLocs = re.findall("[+-]?[0-9]+\.[0-9]+", line)
        maxLocs = [round(float(x), 2) for x in maxLocs]
        max_x, max_y, max_z = maxLocs
    
    # get size loc 
    elif line.startswith('dimensions = '):
        sizeLocs = re.findall("[+-]?[0-9]+\.[0-9]+", line)
        size_x, size_y, size_z = [round(float(x), 2) for x in sizeLocs]

workbook.close()