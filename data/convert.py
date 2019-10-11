import numpy as np
import h5py

def read_section(lines, section_name):
    """
    Read a section entry of an Icarus-formatted gas mixture database file. If the
    section is not found in the file, then return negative values.

    Args:
        lines (list) : A line-by-line list of strings
        section_name (str) : The name of the section.

    Returns:
        nx (int) : The number of independent variable entries.
        ny (int) : The number of dependent variable entries.
        minx (float) : The minimum value of the independent variable.
        maxx (float) : The maximum value of the independent variable.
        pressures (list) : A list of pressure values. The length should correspond to ny.
        start (int) : The index of the line beginning the section of interest.
        stop (int) :  The index of the line ending the section of interest.

    """
    start = -1
    for k in range(len(lines)):
        if lines[k].find(section_name) >= 0:
            start = k
            break

    if start > 0:
        ny = int(lines[start+2].strip())
        nx = int(lines[start+3].strip())
        minx = float(lines[start+4].strip())
        miny = float(lines[start+5].strip())
        pressures = list(float(i.strip()) for i in lines[start+6].split())
        start = start + 7
        stop = start + nx
        return nx, ny, minx, miny, pressures, start, stop
    else:
        return -1, -1, -1, -1, [], -1, -1

def find_scale(x):
    scale = "log10"
    for i in range(len(x)):
        if (x[0] * 10**i != x[i]):
            scale = "linear"
            break
    if (scale == "linear"):
        dx = x[1] - x[0]
        for i in range(len(x)-1):
            if (x[i] + dx != x[i+1]):
                return scale
        scale = "constant_linear"
    return scale

def similar_scales(x1, y1, x2, y2):
    xscale = False
    yscale = False
    count = 0
    if len(x1) == len(x2):
       for i in range(len(x1)):
           count += 1
           if x1[i] != x2[i]:
               break
       if count == len(x1):
           xscale = True
    count = 0
    if len(y1) == len(y2):
       for i in range(len(y1)):
           count += 1
           if y1[i] != y2[i]:
               break
       if count == len(y1):
           yscale = True

    if (xscale and yscale):
        return True
    else:
        return False

def parse_data(data, pressures):
    """
    Return the tabularized data from a line-by-line list.

    Args:
        data (list) :
        pressures (list) :

    Returns:
        x (list) :
        y (dict) :

    """
    x = []
    y = pressures
    z = dict((key, []) for key in pressures)
    for line in data:
        x_data, *z_data = line.split()
        x.append(float(x_data.strip()))
        for p in range(len(pressures)):
            z[pressures[p]].append(float(z_data[p].strip()))
    return x, y, z

def write_hdf5_database(table_data, filename="gas_table.h5", gas_mixture="24sp-tacot-pyro"):
    """
    Write the gas mixture equilibrium thermodynamic data to an HDF5 database file.

    Args:
        data (dict):
        filename (str) :
    """
    uv_tables = []
    tp_tables = []

    f = h5py.File(filename, 'w')
    gas = f.create_group(gas_mixture)
    for prop, data in table_data.items():
        grp = gas.create_group(prop)

        grp.attrs["x_variable"] = data["xy_variables"][0]
        grp.attrs["y_variable"] = data["xy_variables"][1]
        grp.attrs["x_min"] = data["min_x"]
        grp.attrs["x_max"] = data["max_x"]
        grp.attrs["y_min"] = data["min_y"]
        grp.attrs["y_max"] = data["max_y"]
        grp.attrs["x_scale"] = data["x_scale"]
        grp.attrs["y_scale"] = data["y_scale"]

        nx = len(data['x'])
        ny = len(data['y'])
        grp.attrs["nx"] = nx
        grp.attrs["ny"] = ny

        dset_x = grp.create_dataset("x", (nx,), dtype='f')
        dset_x[:] = data['x'][:]

        dset_y = grp.create_dataset("y", (ny,), dtype='f')
        dset_y[:] = data['y'][:]

        # Write the two-dimensional table data, z = f(x,y), to file where each y-entry
        # is written as a separate dataset. Each of those datasets contain nx-entries.
        #
        y_keys = [k for k in data['z'].keys()]

        for i in range(ny):
            dset_name = "z_" + str(i)
            dset_z = grp.create_dataset(dset_name, (nx,), dtype='f')
            dset_z.attrs['y'] = data['y'][i]
            dset_z[:] = data['z'][y_keys[i]][:]

        #if (data["xy_variables"][0] == "energy" and data["xy_variables"][1] == "density"):
        #    print("uv", prop)
        #    uv_tables.append(data)
        #if (data["xy_variables"][0] == "temperature" and data["xy_variables"][1] == "pressure"):
        #    print("tp", prop)
        #    tp_tables.append(data)

    #same_uv_scale = True
    #for i in range(1,1): #len(uv_tables)):
    #    if not similar_scales(uv_tables[0]['x'][:], uv_tables[0]['y'][:],uv_tables[i]['x'][:], uv_tables[i]['y'][:]):
    #        same_uv_scale = False
    #same_tp_scale = True
    #for i in range(1,len(tp_tables)):
    #    if not similar_scales(uv_tables[0]['x'][:], uv_tables[0]['y'][:],uv_tables[i]['x'][:], uv_tables[i]['y'][:]):
    #        same_tp_scale = False
    #if (same_uv_scale):
    #    gas.attrs["smae_uv_scale"] = 1
    #else:
    #    gas.attrs["smae_uv_scale"] = 0
    #if (same_tp_scale):
    #    gas.attrs["smae_tp_scale"] = 1
    #else:
    #    gas.attrs["smae_tp_scale"] = 0

    f.close()

if __name__ == "__main__":

    with open("gas_table.db", 'r') as f:
        lines = f.readlines()

    nx, ny, min_x, max_x, pressures, start, stop = read_section(lines, "[ENTHALPY]")
    x, y, data = parse_data(lines[start:stop], pressures)
    min_y = y[0]
    max_y = y[-1]
    x_scale = find_scale(x)
    y_scale = find_scale(y)

    enthalpy = {'x' : x,
                'y' : y,
                'z' : data,
                'xy_variables' : ['temperature', 'pressure'],
                'x_scale' : x_scale,
                'y_scale' : y_scale,
                'min_x' : min_x,
                'max_x' : max_x,
                'min_y' : min_y,
                'max_y' : max_y}

    nx, ny, min_x, max_x, pressures, start, stop = read_section(lines, "[EINT]")
    x, y, data = parse_data(lines[start:stop], pressures)
    min_y = y[0]
    max_y = y[-1]
    x_scale = find_scale(x)
    y_scale = find_scale(y)

    eint = {'x' : x,
            'y' : y,
            'z' : data,
            'xy_variables' : ['temperature', 'pressure'],
            'x_scale' : x_scale,
            'y_scale' : y_scale,
            'min_x' : min_x,
            'max_x' : max_x,
            'min_y' : min_y,
            'max_y' : max_y}

    nx, ny, min_x, max_x, pressures, start, stop = read_section(lines, "[CP]")
    x, y, data = parse_data(lines[start:stop], pressures)
    min_y = y[0]
    max_y = y[-1]
    x_scale = find_scale(x)
    y_scale = find_scale(y)

    cp = {'x' : x,
          'y' : y,
          'z' : data,
          'xy_variables' : ['energy', 'density'],
          'x_scale' : x_scale,
          'y_scale' : y_scale,
          'min_x' : min_x,
          'max_x' : max_x,
          'min_y' : min_y,
          'max_y' : max_y}

    nx, ny, min_x, max_x, pressures, start, stop = read_section(lines, "[CV]")
    x, y, data = parse_data(lines[start:stop], pressures)
    min_y = y[0]
    max_y = y[-1]
    x_scale = find_scale(x)
    y_scale = find_scale(y)

    cv = {'x' : x,
          'y' : y,
          'z' : data,
          'xy_variables' : ['energy', 'density'],
          'x_scale' : x_scale,
          'y_scale' : y_scale,
          'min_x' : min_x,
          'max_x' : max_x,
          'min_y' : min_y,
          'max_y' : max_y}

    nx, ny, min_x, max_x, pressures, start, stop = read_section(lines, "[DENSITY]")
    x, y, data = parse_data(lines[start:stop], pressures)
    min_y = y[0]
    max_y = y[-1]
    x_scale = find_scale(x)
    y_scale = find_scale(y)

    density = {'x' : x,
               'y' : y,
               'z' : data,
               'xy_variables' : ['temperature', 'pressure'],
               'x_scale' : x_scale,
               'y_scale' : y_scale,
               'min_x' : min_x,
               'max_x' : max_x,
               'min_y' : min_y,
               'max_y' : max_y}

    nx, ny, min_x, max_x, pressures, start, stop = read_section(lines, "[PRESSURE]")
    x, y, data = parse_data(lines[start:stop], pressures)
    min_y = y[0]
    max_y = y[-1]
    x_scale = find_scale(x)
    y_scale = find_scale(y)

    pressure = {'x' : x,
                'y' : y,
                'z' : data,
                'xy_variables' : ['energy', 'density'],
                'x_scale' : x_scale,
                'y_scale' : y_scale,
                'min_x' : min_x,
                'max_x' : max_x,
                'min_y' : min_y,
                'max_y' : max_y}

    nx, ny, min_x, max_x, pressures, start, stop = read_section(lines, "[TEMPERATURE]")
    x, y, data = parse_data(lines[start:stop], pressures)
    min_y = y[0]
    max_y = y[-1]
    x_scale = find_scale(x)
    y_scale = find_scale(y)

    temperature = {'x' : x,
                   'y' : y,
                   'z' : data,
                   'xy_variables' : ['energy', 'density'],
                   'x_scale' : x_scale,
                   'y_scale' : y_scale,
                   'min_x' : min_x,
                   'max_x' : max_x,
                   'min_y' : min_y,
                   'max_y' : max_y}

    nx, ny, min_x, max_x, pressures, start, stop = read_section(lines, "[VISCOSITY]")
    x, y, data = parse_data(lines[start:stop], pressures)
    min_y = y[0]
    max_y = y[-1]
    x_scale = find_scale(x)
    y_scale = find_scale(y)

    viscosity = {'x' : x,
                 'y' : y,
                 'z' : data,
                 'xy_variables' : ['temperature', 'pressure'],
                 'x_scale' : x_scale,
                 'y_scale' : y_scale,
                 'min_x' : min_x,
                 'max_x' : max_x,
                 'min_y' : min_y,
                 'max_y' : max_y}

    table_data = {'enthalpy' : enthalpy,
                  'eint' : eint,
                  'cp' : cp,
                  'cv' : cv,
                  'density' : density,
                  'pressure' : pressure,
                  'temperature' : temperature,
                  'viscosity' : viscosity
    }
    write_hdf5_database(table_data)
