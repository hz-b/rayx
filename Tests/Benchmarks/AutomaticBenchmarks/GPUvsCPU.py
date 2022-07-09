from cProfile import label
from sys import argv, exit, path
import matplotlib
import matplotlib.pyplot as plt
import xml.etree.ElementTree as ET
import numpy as np
import subprocess
import time
import os
from tqdm import trange

"""
////////////////////////
ONLY LAUNCH FROM CURRENT DIR
This might take a while... (Even on a powerful CPU, it's only running on 1 thread!)
///////////////////////
"""

cwd = os.getcwd()
test_rml = ['']
settings = {
    'RemoveExportDir': False,
    'RmlFileName': "",
    'AMOUNT': 1,
}

# CHANGE HERE
STEPS = 500
TRIALS = 10

rayx_cpu_times = []
rayx_gpu_times = []

# MAKE SURE THAT RAY-X IS CORRECTLY BUILT
rayxdir = os.path.join(cwd, "build/bin")


def increment_rays():
    """
    Increment Rays by amount
    """
    amount = settings['AMOUNT']
    settings['AMOUNT'] = settings['AMOUNT'] + STEPS
    return str(amount)


def change_rml(root, xmlTree: ET.ElementTree):
    """
    Change the Ray Count in an RML file
    """
    for element in root.findall("./beamline/object"):
        if element.attrib['type'] == 'Point Source':
            prop_to_change = 'numberRays'

            for param in element.findall("./param"):
                if param.attrib['id'] == prop_to_change:  # If numberRays found
                    param.text = increment_rays()  # Increment Rays

                    try:
                        os.mkdir(os.path.join(rayxdir, "Benchmark"))
                        settings['RemoveExportDir'] = True
                    except FileExistsError:
                        settings['RemoveExportDir'] = True

                    new_rml = rayxdir + '/Benchmark/' + \
                        settings['RmlFileName']
                    xmlTree.write(new_rml, encoding='UTF-8',
                                  xml_declaration=True)
                    return new_rml


def plot(cpu_times, gpu_times):
    """
    Plot the Beamline time; x, for RAY-X. ui, for RAY-UI
    beamlines is also the chaning variable throughout the beamline
    """
    assert(len(cpu_times) == len(gpu_times))

    N = len(cpu_times)
    assert(N == TRIALS)

    ind = np.arange(1, N*STEPS+1, step=STEPS)

    plt.scatter(ind, cpu_times, label='CPU')
    plt.scatter(ind, gpu_times, label='GPU')

    plt.xlabel('Number of Rays')
    plt.xticks(ind)

    # Grid display
    plt.grid(linewidth=.25)

    plt.ylabel('time (s)')
    plt.title("RAY-X: CPU vs GPU (Vulkan) Tracer")
    plt.legend()
    plt.show()


def run_rayx_cpu(rml):
    start = time.time()
    ray_x_proc = subprocess.Popen(
        rayxdir+'/TerminalApp -i ' + rml + ' -x', shell=True, cwd=rayxdir, stdin=subprocess.PIPE, stdout=subprocess.DEVNULL)
    ray_x_proc.wait()
    rayx_cpu_times.append(time.time() - start)


def run_rayx_gpu(rml):
    start = time.time()
    ray_x_proc = subprocess.Popen(
        rayxdir+'/TerminalApp -i ' + rml, shell=True, cwd=rayxdir, stdin=subprocess.PIPE, stdout=subprocess.DEVNULL,)
    ray_x_proc.wait()
    rayx_gpu_times.append(time.time() - start)


if __name__ == "__main__":
    if (len(argv) <= 1):
        exit("Missing RML file?")
    # if (argv[1].find(" ") == -1):
    #     exit("Rml file should not contain spaces")
    if (argv[1][-4:] != ".rml"):
        exit("Given file should be of .rml extension")

    # Get settings:
    settings['RmlFileName'] = argv[1]

    xmlTree = ET.parse(rayxdir + "/" + settings['RmlFileName'])
    root = xmlTree.getroot()

    print(f'Starting...')
    for i in trange(TRIALS):
        _rml = change_rml(root, xmlTree)
        run_rayx_gpu(_rml)
        run_rayx_cpu(_rml)

    # cleanup
    if (settings['RemoveExportDir']):
        for root, dirs, files in os.walk(os.path.join(rayxdir, "Benchmark"), topdown=False):
            for name in files:
                os.remove(os.path.join(root, name))
            for name in dirs:
                os.rmdir(os.path.join(root, name))

    # plot
    plot(rayx_cpu_times, rayx_gpu_times)
    exit()
