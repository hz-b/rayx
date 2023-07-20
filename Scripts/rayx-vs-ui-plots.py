from asyncore import write
from logging import RootLogger
import xml.etree.ElementTree as ET
import os
import subprocess
from tqdm import trange
import shutil
import random

cwd = os.getcwd()


# This Script works only when you set the correct paths to RAY-UI and rayx

# add RML Files for testing here
# These are the original RML Files to generate the randoms from.
rmls = [tuple(['Ellips/test_0.rml', 'Ellips']),
        tuple(['Toroid/test_0.rml', 'Toroid'])]

# MAKE SURE THAT rayx IS CORRECTLY BUILT
rayxdir = os.path.join(cwd, 'build/bin')

# CHANGE HERE
# SET THE PATH TO RAY-UI executable (Absolute e.g /home/Documents/Ray-UI)
RAY_UI = ''

# Popen needs this env variable to find the library qwt for dynamic linking
LD_LIBRARY_PATH = ''
# (e.g /home/usr/Documents/qwt-6.2.0/lib/)

# END CHANGE

rayuidir = os.path.join(RAY_UI)

export_dir = os.path.join(cwd)

RAD_INCREMENT = 10
MM_INCREMENT = 250
TRIALS = 10  # SET THE AMOUNT OF TESTS TO CHECK

"""
Randomly change the values 
returns the new value to be changed
"""


def increment_random(amount_type: int, element_name, param_id, value):
    amount = random.randint(0, 5) * amount_type

    if (amount >= 360 and amount_type == 10):  # Only in 2 Pi
        return increment_random(amount_type, element_name, param_id)
    new_value = float(value) + amount
    # if (amount_type == MM_INCREMENT): return int(new_value)
    return new_value


"""
Conditions to randomly change. 
"""
conditions = {
    'Ellipsoid': {
        'totalWidth': MM_INCREMENT,
        'totalLength': MM_INCREMENT,
        'grazingIncAngle': RAD_INCREMENT,
        'designGrazingIncAngle': RAD_INCREMENT,
        'longHalfAxisA': MM_INCREMENT,
        'shortHalfAxisB': MM_INCREMENT,
        'azimuthalAngle': RAD_INCREMENT,
    }
}

"""
Change the value in an RML file
"""


def change_value(root, xmlTree: ET.ElementTree):
    params = []
    _random = 0
    for element in root.findall("./beamline/object"):
        if element.attrib['name'] in conditions:
            element_properties_to_change = conditions[element.attrib['name']]
            _random_to_change = random.choice(
                list(element_properties_to_change))
            for param in element.findall("./param"):
                if param.attrib['id'] == _random_to_change:
                    _random = increment_random(
                        element_properties_to_change[param.attrib['id']], element.attrib['name'], param.attrib['id'], param.text)
                    params.append(_random)
                    param.text = str(_random)
    new_rml = rayxdir+'/'+rmls[0][1]+'/auto' + \
        str(int(_random))+str(_random_to_change)+'.rml'
    xmlTree.write(new_rml, encoding='UTF-8', xml_declaration=True)
    return new_rml


if __name__ == '__main__':

    if RAY_UI == '' or LD_LIBRARY_PATH == '':
        raise Exception('Missing arguments RAY_UI/LD_LIBRARY_PATH')

    xmlTree = ET.parse(os.path.join(rayxdir, rmls[0][0]))
    root = xmlTree.getroot()

    # Delete the generated files from last run
    for _, dirs, files in os.walk(rayxdir+'/'+rmls[0][1]):
        for file in files:
            if 'auto' in str(file):
                os.remove(rayxdir+'/'+rmls[0][1]+'/'+file)

    my_env = os.environ.copy()
    my_env["LD_LIBRARY_PATH"] = LD_LIBRARY_PATH

    for i in trange(TRIALS):
        print(f'Generating new values..')
        _rml = change_value(root, xmlTree)
        print(f'Running rayx...')
        ray_x_proc = subprocess.Popen(
            rayxdir+'/TerminalApp -i ' + _rml+' -p', shell=True, cwd=rayxdir, stdin=subprocess.PIPE)
        # ray_x_proc.wait()
        print(f'Running RAY-UI...')
        ray_ui_proc = subprocess.Popen(
            rayuidir + '/Ray-UI -t ' + _rml, shell=True, env=my_env, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        wait_res = input("Finished. Press any key for next trial.")
