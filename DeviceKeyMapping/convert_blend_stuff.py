import bpy

def float_to_fixed_string(floater):
    output = str(round(floater,4))
    while len(output) < 6:
        output += "0"
    return output

def do_thing():
    print("...")
    print("running")
    print("...")
    collection = bpy.context.collection
    
    rows = []
    
    min_x = None
    max_x = None
    min_z = None
    max_z = None
    
    pos_collectons = []
    for subcollection in collection.children:
        # iterate objects
        origin_points_list = []
        for obj in subcollection.objects:
            if obj.type == 'MESH':
                x, _, z = obj.matrix_world.translation
                origin_points_list.append([x, z])
                if (min_x == None or x < min_x):
                    min_x = x
                if (max_x == None or x > max_x):
                    max_x = x
                if (min_z == None or z < min_z):
                    min_z = z
                if (max_z == None or z > max_z):
                    max_z = z
        # sort
        sorted_origin_points = sorted(origin_points_list, key=lambda point: point[0])
        pos_collectons.append(sorted_origin_points)
    
    hori_dist = max_x - min_x
    vert_dist = max_z - min_z
    print("const float height_multi = " + str(vert_dist/hori_dist) + "f;")
    
    max_x -= min_x
    max_z -= min_z
    # normalize data
    for col in pos_collectons:
        for pos in col:
            pos[0] -= min_x
            pos[0] /= max_x
            pos[1] -= min_z
            pos[1] /= max_z
    
    
    # parse & export
    row_index = 0
    for col in pos_collectons:
        output = "const float pos_row"+str(row_index)+"[] = {"
        is_first = True
        for pos in col:
            if not is_first:
                output += ", "
            output += float_to_fixed_string(pos[0]) + "," +  float_to_fixed_string(pos[1])
            is_first = False
            
        output += "};"
        print(output)
        row_index += 1
        
    # then build table for arrays
    last_output = "static const float* pos_rows["+str(len(pos_collectons))+"] = {"
    last_is_first = True
    row_index = 0
    for col in pos_collectons:
        if last_is_first == False:
            last_output += ","
        last_output += "pos_row"+str(row_index)
        last_is_first = False
        row_index += 1
    last_output += "};"
    print(last_output)
        
do_thing()