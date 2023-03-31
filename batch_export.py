import rhinoscriptsyntax as rs
import re

# Check whether objects in different layers have been grouped together
# set NotGrouped signal
NotGrouped = True
objects = rs.AllObjects()
gp_layers = set()
for obj in objects:
    # Check if object is in a group
    if rs.ObjectGroups(obj):
        # Get group name
        group_name = rs.ObjectGroups(obj)[0]
        group_objects = rs.ObjectsByGroup(group_name)

        # Check if object is in a different layer than other objects in the group
        for group_obj in group_objects:
            l1 = rs.ObjectLayer(obj)
            l2 = rs.ObjectLayer(group_obj)
            if l1 != l2 and l2 not in gp_layers:
                print("( " + l1 + " ) & ( " + l2 + " ) have been grouped together.")
                gp_layers.add(rs.ObjectLayer(obj))
                NotGrouped = False
                break

if not NotGrouped:
    print('Please ungroup prompted layers!')
    exit()
print('NotGrouped Check over!')


# Check whether selected layer has "id" numbers in its layername




# main 
# Get a list of layer names
layers = rs.GetLayers("Select layers to operate on", True)
if not layers: exit()

# record how many sublayers
record = {}
# For each layer name
for layer in layers:
    # Get a list of objects on the layer and select it
    objects = rs.ObjectsByLayer(layer, True)
    if not objects: continue

    # Export
    id = re.findall('\d+', layer)[0]
    if id not in record:
        record[id] = -1
    filename = id + '.wrl'
    # bcz "::" can't be used in a Windows filename, we need to check and change sublayers' layername
    if "::" in layer:
        record[id] += 1
        filename = id + '_' + str(record[id]) + '.wrl'
    # Set the export options for wrl format and version 2.0
    # bcz ParaView can only read wrl version 2.0 files!
    options = "_Version=2.0 _Enter"
    rs.Command("_-Export " + filename + " " + options)


    # BoundingBox
    options = "_CoordinateSystem=World _Enter"
    rs.Command("_BoundingBox " + options)


    # Log file
    text = rs.CommandHistory()
    # filter "min\max\dimensions" that we need
    res = filename + "\n"
    for line in text.splitlines():
        if line.startswith("min = "):
            res += line + "\n"
        elif line.startswith("max = "):
            res += line + "\n"
        elif line.startswith("dimensions = "):
            res += line + "\n"
    res += "--------------------------------------- \n"
    f = open('record.txt','a')
    f.write(res)
    f.close()
    rs.ClearCommandHistory()

    # Deselect the objects on the layer
    rs.UnselectObjects(objects)


