import numpy as np
import os
import subprocess
import h5py
import pandas as pd
import xml.etree.ElementTree as ET
from tqdm import trange
import schwimmbad
import shutil

cwd = os.getcwd()
NAME = 'Spec1-first_rzp4mm_rm4_run2'

num_of_workers = 100
max_size = 1000  # number of traces until quit Ray-UI (memory leak)
simulations = 2000000
print('Used workers: ', num_of_workers)

# appended to directory name
prefix = 'd'

# intervals for parameters to set in rml
conditions = {
    "Point Source": {
        'translationXerror': [-5.0, 5.0],
        'translationYerror': [-5.0, 5.0],
    },
    "Reflection Zoneplate": {
        'distancePreceding': [85.0, 95.0],
    },
}

conditions2 = {
    "Point Source": {
        'rotationXerror': 0,
        'rotationYerror': 0,
    },
}

# original beamlinefile
filec = os.path.join(cwd, 'Spec1-first_rzp4mm_rm4.rml')

# app directory
app_dir = os.path.join(cwd, 'RAY-UI-development')

# export directory
export_dir = os.path.join(cwd, 'output2')
export_dir_h5 = os.path.join(cwd, 'output2/h5/')

# create workload for multiprocessing
list_of_jobs = np.arange(simulations)
jobs_per_worker = np.array_split(list_of_jobs, num_of_workers)
limited_jobs_per_worker = [np.array_split(job_ids, len(job_ids) // max_size) for job_ids in jobs_per_worker]
workload = [item for sublist in limited_jobs_per_worker for item in sublist]


def roll_dice(element_name, param_id):
    """
    return random value in interval
    """
    interval = conditions[element_name][param_id]
    return np.random.rand() * (interval[1] - interval[0]) + interval[0]


def rotation(param_id, params):
    """
    return correct rotation of point source dependant Translations TX, Ty, Tz
    """
    Tx = params[0]
    Ty = params[1]
    Z = params[2]
    if param_id == 'rotationXerror':
        R = (Ty / Z) * 1000  #Rx
    else:
        R = (-Tx / Z) * 1000  #Ry
    return R


#
def createRayInputParameters(simulations=simulations):
    """
    creates the input parameters for Ray-UI randomly
    """
    print("Start creation of", simulations, "input parameters")

    xmlTree = ET.parse(filec)
    rootElement = xmlTree.getroot()

    for jobID in trange(simulations):

        directory = export_dir + '/' + str(jobID) + '_' + prefix + '/'
        if not os.path.exists(directory):
            os.makedirs(directory)

        # copy .dat Energy file to directory
        original = export_dir + '/' + 'RAC_HV_1zu1.DAT'
        target = directory + 'RAC_HV_1zu1.DAT'
        shutil.copyfile(original, target)

        params = []
        rotations = []
        # translations
        for element in rootElement.findall("./beamline/object"):
            if element.attrib['name'] in conditions:
                element_condition = conditions[element.attrib['name']]
                for param in element.findall("./param"):
                    if param.attrib['id'] in element_condition:
                        random = roll_dice(element.attrib['name'], param.attrib['id'])
                        params.append(random)
                        param.text = str(random)

        # rotations
        for element in rootElement.findall("./beamline/object"):
            if element.attrib['name'] in conditions2:
                element_condition2 = conditions2[element.attrib['name']]
                for param in element.findall("./param"):
                    if param.attrib['id'] in element_condition2:
                        rot = rotation(param.attrib['id'], params)
                        rotations.append(rot)
                        param.text = str(rot)

        xmlTree.write(directory + 'BL_file.rml', encoding='UTF-8', xml_declaration=True)
        np.savetxt(directory + 'parameters.txt', np.array(params))
        np.savetxt(directory + 'rotations.txt', np.array(rotations))


def query(command, proc):
    proc.stdin.write(bytes(command + "\n", encoding='utf-8'))
    proc.stdin.flush()
    output = proc.stdout.readline()
    print(output.rstrip())


def exec_ray_trace(list_of_jobs):
    """
    executes RAY-UI for all jobs within the list_of_jobs
    exports csv's of Ray and rewrites BL_file.rml
    """

    print("execute RAY traces")
    print(app_dir)

    proc = subprocess.Popen(app_dir + '/rayui.sh -b', shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE)

    for jobID in list_of_jobs:
        directory = export_dir + '/' + str(jobID) + '_' + prefix + '/'

        query('load ' + directory + 'BL_file.rml', proc)
        query("trace noanalyze", proc)
        query("export \"CCD 350-450\" \"RawRaysOutgoing\" " + directory, proc)
        query("export \"CCD 350-450 2\" \"RawRaysOutgoing\" " + directory, proc)

    query("quit", proc)
    proc.stdin.close()
    proc.wait()
    create_h5_files(list_of_jobs)


def create_h5_files(list_of_jobs):
    for jobID in list_of_jobs:
        directory = export_dir + '/' + str(jobID) + '_' + prefix + '/'
        csv_path_raw = directory + 'CCD 350-450-RawRaysOutgoing.csv'

        # read raw csv
        dfr = pd.read_csv(csv_path_raw, sep='\t', header=0, skiprows=1)  # tab deliminator
        OX = dfr.values[:, 3].astype('float64')
        OY = dfr.values[:, 4].astype('float64')
        EN = dfr.values[:, 9].astype('float64')

        # read parameters.txt and rotations.txt
        params = np.loadtxt(directory + 'parameters.txt')
        rotations = np.loadtxt(directory + 'rotations.txt')

        # create h5 files, datasets x and y, parameters
        with h5py.File(os.path.join(directory, 'data.h5'), 'w') as f:
            f.create_dataset("parameters", data=params, compression="gzip")
            f.create_dataset("rotations", data=rotations, compression="gzip")
            f.create_dataset("OX", data=OX, compression="gzip")
            f.create_dataset("OY", data=OY, compression="gzip")
            f.create_dataset("EN", data=EN, compression="gzip")

        csv_path_raw_2 = directory + 'CCD 350-450 2-RawRaysOutgoing.csv'

        # read raw csv
        dfr2 = pd.read_csv(csv_path_raw_2, sep='\t', header=0, skiprows=1)  # tab deliminator
        OX2 = dfr2.values[:, 3].astype('float64')
        OY2 = dfr2.values[:, 4].astype('float64')
        EN2 = dfr2.values[:, 9].astype('float64')

        # create h5 files, datasets x and y, parameters
        with h5py.File(os.path.join(directory, 'data2.h5'), 'w') as f:
            f.create_dataset("parameters", data=params, compression="gzip")
            f.create_dataset("rotations", data=rotations, compression="gzip")
            f.create_dataset("OX", data=OX2, compression="gzip")
            f.create_dataset("OY", data=OY2, compression="gzip")
            f.create_dataset("EN", data=EN2, compression="gzip")

        # if h5 exists, remove csv file
        filePath_h5 = directory + 'data.h5'
        if os.path.exists(filePath_h5):
            try:
                os.remove(csv_path_raw)
            except:
                pass



def createDeepRayFile():
    """
    Saves the RAY images as DeepRay.h5 file
    """
    h5 = h5py.File(os.path.join(export_dir_h5, 'DeepRay_' + NAME + '.h5'), 'w')

    for nr in trange(simulations):
        try:
            directory = export_dir + '/' + str(nr) + '_' + prefix + '/'
            h5_data = h5py.File(os.path.join(directory, 'data.h5'), 'r')
            g = h5.create_group(str(nr))
            for key in h5_data.keys():
                g.create_dataset(key, data=h5_data[key], compression="gzip")
            h5_data.close()

        except:
            print("Something went wrong in deep ray file creation! In folder ", nr)
            continue
    h5.close()

    h5 = h5py.File(os.path.join(export_dir_h5, 'DeepRay_' + NAME + '_2.h5'), 'w')

    for nr in trange(simulations):
        try:
            directory = export_dir + '/' + str(nr) + '_' + prefix + '/'
            h5_data = h5py.File(os.path.join(directory, 'data2.h5'), 'r')
            g = h5.create_group(str(nr))
            for key in h5_data.keys():
                g.create_dataset(key, data=h5_data[key], compression="gzip")
            h5_data.close()

        except:
            print("Something went wrong in deep ray file creation! In folder ", nr)
            continue
    h5.close()


if __name__ == '__main__':
    # generate the rml files
    createRayInputParameters()

    # trace using RAY-UI with number of workers
    with schwimmbad.JoblibPool(num_of_workers) as pool:
        pool.map(exec_ray_trace, workload)

    # create a H5 file of traces
    createDeepRayFile()
