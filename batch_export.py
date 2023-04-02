import rhinoscriptsyntax as rs
import re


# Get a list of layer names & Select
layers = rs.GetLayers("Select layers to operate on", True)
if not layers: exit()


# Check whether objects in different layers have been grouped together: accomplished
# Check whether selected layer has "id" numbers in its layername: waiting
# Check whether selected layers are visible: accomplished
NotGrouped = True
Visible = True
gp_layers = set()

for layer in layers:
    # Check if object is in a group
    objects = rs.ObjectsByLayer(layer)
    for obj in objects:
        if rs.ObjectGroups(obj):
            # Get group name
            group_name = rs.ObjectGroups(obj)[0]
            group_objects = rs.ObjectsByGroup(group_name)

            # Check if object is in a different layer than other objects in the group
            for group_obj in group_objects:
                l1 = layer
                l2 = rs.ObjectLayer(group_obj)
                if l1 != l2 and l2 not in gp_layers:
                    print("( " + l1 + " ) & ( " + l2 + " ) have been grouped together.")
                    gp_layers.add(layer)
                    NotGrouped = False
                    break
    
    if Visible:
        vis = rs.IsLayerVisible(layer)
        if not vis:
            Visible = False

if not NotGrouped:
    print('Please ungroup prompted layers!')
if not Visible:
    print('Please make all the selected layers visible!')
if not NotGrouped or not Visible:
    exit()
print('NotGrouped Check over!')
print('Visible Check over!')


# Saved path: must be the full path!
path = "C:\\Users\\crthx\\Desktop\\Fudan\\Rhinoceros_Batch_Export_wrl\\wrl\\"

# Record how many sublayers
record = {}

# For each layer name
for layer in layers:
    # Get a list of objects on the layer and select it
    objects = rs.ObjectsByLayer(layer, True)
    if not objects: continue

    # Export
    # saved into ".\generated" folder
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
    # Double "_Enter": to solve prompting "Mesh setting (...)" automatically
    options = "_Version=2.0 _Enter _Enter"
    fullname = path + filename
    rs.Command("_-Export " + fullname + " " + options)


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


