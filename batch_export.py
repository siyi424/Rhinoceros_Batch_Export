import rhinoscriptsyntax as rs

# Get a list of layer names
layers = rs.GetLayers("Select layers to operate on", True)
if not layers: exit()

# For each layer name
for layer in layers:
    # Get a list of objects on the layer and select it
    objects = rs.ObjectsByLayer(layer, True)
    if not objects: continue

    # Export
    filename = str(layer) + ".wrl"
    # bcz "::" can't be used in a Windows filename, we need to check and change sublayers' layername
    if "::" in filename:
        filename = filename.split("::")[1]
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


