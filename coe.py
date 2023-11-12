import math

### VARIABLES #####
outfile = "sin_LUT.coe"
lut_width = 16
num_divisions = 512
stop_point = math.pi/2
#####        #######

lut_width_hex = math.ceil(lut_width/4)
bit_mask = (1 << lut_width) - 1
delta = stop_point/num_divisions

with open(outfile, "w") as f:
    f.write("memory_initialization_radix=16;\n")
    f.write("memory_initialization_vector=\n")

    for i in range(num_divisions):
        sin_value = math.sin(i*delta)
        value = round(sin_value*(2**(lut_width-1) - 1))

        delim = "" if i == num_divisions-1 else ","
        f.write("{0:0"f"{lut_width_hex}""X}{1}\n".format(value & bit_mask, delim))

    f.write(";\n")