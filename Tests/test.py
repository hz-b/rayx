# -*- coding: utf-8 -*-
"""
Created on Tue Mar 23 13:42:35 2021

@author: Theresa
"""
import numpy as np
import pandas as pd
from os.path import join as p_join
import os
import unittest
import matplotlib.pyplot as plt

PROJDIR = os.path.dirname(__file__)
path = os.path.split(PROJDIR)[0]
OUTPUT_FILE = p_join(path, 'build/bin/output.csv')
TESTS = p_join(path, 'Tests')
RAY_REWORKED = p_join(TESTS, 'output')
print("new ray files in folder:",RAY_REWORKED)

# where the results from old ray are
#RAY_UI = os.path.split(os.path.split(path)[0])[0]
#RAY_UI = p_join(RAY_UI, 'python')
#RAY_UI = p_join(RAY_UI, 'test_output')
#RAY_UI = p_join(RAY_UI, 'unittestData')
RAY_UI = p_join(TESTS, 'oldray_files')
RAY_UI = p_join(RAY_UI, 'unittestData')
print("old ray files in folder:",RAY_UI)
scatterplot_size = 0.5

def load_output():
    data = pd.read_csv(OUTPUT_FILE, sep=';', header=0)
    filtered = data[data['Weight']==1]
    plt.scatter(filtered['Xloc'], filtered['Yloc'], s =0.2)

def open_new_file(name, valid=0, energy=0):
    df = pd.read_csv(p_join(RAY_REWORKED, name), sep=';')
    m_o = np.array([df['Xloc'].values, df['Yloc'].values, df['Zloc'].values,
                   df['Xdir'].values, df['Ydir'].values, df['Zdir'].values,
                   df['Energy'].values, df['pathLength'].values, df['order'].values, df['lastElement'].values],
                   dtype=np.double)
    weights = np.array(df['Weight'].values)
    # m_o = np.append(m_o, df['Energy'].values[None], axis=0)
    # ignore rays that would have been thrown away in old ray (weight = 0)
    if valid == 1:
        valid_weights = np.where([weights == 1])[1]
        m_o = m_o[:, valid_weights]
        return m_o
    return m_o

# returns 2d array with 6 or 7 rows [origin, direction(, energy)]
def open_old_file(name, prefix, energy=0):
    df = pd.read_csv(p_join(RAY_UI, name), decimal=',', sep=';')
    m = np.array([df[prefix+'OX'].values, df[prefix+'OY'].values, df[prefix+'OZ'].values,
                 df[prefix+'DX'].values, df[prefix+'DY'].values, df[prefix+'DZ'].values], dtype=np.double)
    if energy == 1:
        m = np.append(m, df[prefix+'EN'].values[None],axis=0)
        print(m.shape)
    return m


def plot_comparison(name, result, correct, mean=0):
    fig, ax = plt.subplots(2, 3, figsize=(10, 6))
    ax[0][0].set_ylabel('New - y')
    ax[0][0].set_xlabel("x")
    if mean == 1:
        ax[0][0].axhline(np.mean(result[1]), ls='--', c='r', alpha=0.7) 
        ax[0][0].axvline(np.mean(result[0]), ls='--', c='r', alpha=0.7) 
    ax[0][0].scatter(result[0], result[1], s=scatterplot_size)
    ax[0][1].set_xlabel('x')
    ax[0][1].set_ylabel('z')
    ax[0][1].scatter(result[0], result[2], s=scatterplot_size)
    ax[0][2].set_xlabel('x-dir')
    ax[0][2].set_ylabel('y-dir')
    ax[0][2].scatter(result[3], result[4], s=scatterplot_size)
    if mean == 1:
        ax[0][2].axhline(np.mean(result[4]), ls='--', c='r', alpha=0.7) 
        ax[0][2].axvline(np.mean(result[3]), ls='--', c='r', alpha=0.7) 
        
    ax[1][0].set_ylabel('Old - y')
    ax[1][0].set_xlabel('x')
    if mean == 1:
        ax[1][0].axhline(np.mean(correct[1]), ls='--', c='r', alpha=0.7) 
        ax[1][0].axvline(np.mean(correct[0]), ls='--', c='r', alpha=0.7) 
    ax[1][0].scatter(correct[0], correct[1], s=scatterplot_size)
    ax[1][1].set_xlabel('x')
    ax[1][1].set_ylabel('z')
    ax[1][1].scatter(correct[0], correct[2], s=scatterplot_size)
    ax[1][2].set_xlabel('x-dir')
    ax[1][2].set_ylabel('y-ddir')
    ax[1][2].scatter(correct[3], correct[4], s=scatterplot_size)
    if mean == 1:
        ax[1][2].axhline(np.mean(correct[4]), ls='--', c='r', alpha=0.7) 
        ax[1][2].axvline(np.mean(correct[3]), ls='--', c='r', alpha=0.7) 
    fig.suptitle(name)
    fig.tight_layout()
    plt.show()

photonEnergy_def = 260
energySpread_def = 80

class Tests(unittest.TestCase):
    
    def test_planeMirrorDefault(self):
        correct = open_old_file(
            'ImagePlane-RawRaysOutgoing_PlaneMirrorDef.csv', 'ImagePlane_')
        result = open_new_file('testFile_PlaneMirrorDef.csv')
        self.assertTrue(np.alltrue(result[7] <= 11001)) # check that path length makes sense
        self.assertTrue(np.alltrue(result[7] >= 11000))
        self.assertTrue(np.allclose(correct, result[:6]))
        
    
    def test_planeMirrorMis(self):
        correct = open_old_file(
            'ImagePlane-RawRaysOutgoing_PlaneMirrorMis.csv', 'ImagePlane_')
        result = open_new_file('testFile_PlaneMirrorMis.csv', 0)
        self.assertTrue(np.allclose(correct, result[:6]))

    def test_sphereMirrorDefault(self):
        correct = open_old_file('ImagePlane-RawRaysOutgoing_SphereMirrorDef.csv', 'ImagePlane_')
        result = open_new_file('testFile_SphereMirrorDefault.csv')
        self.assertTrue(correct.shape == result[:6].shape)
        self.assertTrue(np.allclose(correct, result[:6], atol=1e-00))

    def test_planeGratingDevDefault(self):
        correct = open_old_file(
            'ImagePlane-RawRaysOutgoing_PlaneGratingDef.csv', 'ImagePlane_')
        result = open_new_file('testFile_PlaneGratingDeviationDefault.csv')
        self.assertTrue(np.allclose(correct, result[:6]))

    def test_planeGratingDevAzimuthal(self):
        correct = open_old_file('ImagePlane-RawRaysOutgoing_PlaneGratingDevAz.csv', 'ImagePlane_')
        result = open_new_file('testFile_PlaneGratingDeviationAz.csv')
        self.assertTrue(correct.shape == result[:6].shape)
        self.assertTrue(np.allclose(correct, result[:6]))

    def test_planeGratingDevMis(self):
        correct = open_old_file(
            'ImagePlane-RawRaysOutgoing_PlaneGratingDevAzMis.csv', 'ImagePlane_')
        result = open_new_file('testFile_PlaneGratingDeviationAzMis.csv',valid=1)
        self.assertTrue(correct.shape == result[:6].shape)
        self.assertTrue(np.allclose(correct, result[:6]))

    
    #difference between ray-ui and ray-ui code??
    def test_planeGratingDevAzMisVLS(self):
        correct = open_old_file(
            'ImagePlane-RawRaysOutgoing_PlaneGratingDevAzMisVLS.csv', 'ImagePlane_')
        result = open_new_file('testFile_PlaneGratingDeviationMis.csv',valid=1)
        self.assertTrue(np.allclose(correct, result[:6]))
    

    def test_planeGratingIncAzMis(self):
        correct = open_old_file(
            'ImagePlane-RawRaysOutgoing_PlaneGratingIncAzMis.csv', 'ImagePlane_')
        result = open_new_file('testFile_PlaneGratingIncAzMis.csv')
        self.assertTrue(correct.shape == result[:6].shape)
        self.assertTrue(np.allclose(correct, result[:6]))
    
    def test_RZPdefault20(self):
        correct = open_old_file(
            'ImagePlane-RawRaysOutgoing_RZPDef.csv', 'ImagePlane_')
        result = open_new_file(
            'testFile_ReflectionZonePlateDefault.csv', valid=1)
        self.assertTrue(correct.shape == result[:6].shape)
        self.assertTrue(np.allclose(correct, result[:6]))
        
    def test_RZPdefault200(self):
        correct = open_old_file(
            'ImagePlane-RawRaysOutgoing_RZPDef200.csv', 'ImagePlane_')
        result = open_new_file(
            'testFile_ReflectionZonePlateDefault200.csv', valid=1)
        self.assertTrue(correct.shape == result[:6].shape)
        self.assertTrue(np.allclose(correct, result[:6]))

    def test_RZPazimuthal(self):
        correct = open_old_file(
            'ImagePlane-RawRaysOutgoing_RZPAz200.csv', 'ImagePlane_')
        result = open_new_file(
            'testFile_ReflectionZonePlateAzim200.csv', valid=1)
        self.assertTrue(correct.shape == result[:6].shape)
        self.assertTrue(np.allclose(correct, result[:6]))
        
    def test_RZPazimuthal(self):
        correct = open_old_file(
            'ImagePlane-RawRaysBeam_RZPtoroidal4020_200.csv', 'ImagePlane_')
        result = open_new_file(
            'testFile_ReflectionZonePlateDefault200Toroid.csv', valid=1)
        df = pd.read_csv(p_join(RAY_REWORKED, 'testFile_ReflectionZonePlateDefault200Toroid.csv'), sep=';')
        extraParam = np.array(df['extraParam'].values)
        result = result[:, np.where([extraParam == 21])[1]]
        self.assertTrue(correct.shape == result[:6].shape)
        self.assertTrue(np.allclose(correct, result[:6]))
    
    def test_ImagePlaneMatrixSource(self):
        correct = open_old_file(
            'ImagePlane-RawRaysOutgoing.csv', 'ImagePlane_')
        result = open_new_file('testFile_PlaneMirror_ImagePlane.csv', valid=1)
        self.assertTrue(correct.shape == result[:6].shape)
        self.assertTrue(np.allclose(correct, result[:6]))

    def test_RZPmis(self):
        correct = open_old_file(
            'ImagePlane-RawRaysOutgoing_RZPMisN.csv', 'ImagePlane_')
        result = open_new_file('testFile_ReflectionZonePlateMis.csv', valid=1)
        self.assertTrue(correct.shape == result[:6].shape)
        self.assertTrue(np.allclose(correct, result[:6]))
    
    def test_pointSourceHardEdge(self):
        sourceEnergy = 120.97
        energySpread = 12.1
        correct = open_old_file(
            'Point Source-RawRaysBeam_HE_s.csv', 'Point Source_')
        result = open_new_file('pointSourceHE.csv')
        # check that same amount of rays
        self.assertTrue(correct.shape[1] == result.shape[1])
        # z-direction
        self.assertTrue(np.isclose(np.mean(correct[5]), np.mean(result[5])))
        self.assertTrue(np.all(result[6] <= sourceEnergy+energySpread))
        self.assertTrue(np.all(result[6] >= sourceEnergy-energySpread))
        plot_comparison('Hard Edge', result, correct)

    def test_pointSourceSoftEdge(self):
        sourceEnergy = 120
        energySpread = 0
        correct = open_old_file(
            'Point Source-RawRaysBeam_SE_s.csv', 'Point Source_')
        result = open_new_file('pointSourceSE.csv')
        self.assertTrue(correct.shape[1] == result.shape[1])
        self.assertTrue(np.isclose(np.mean(correct[5]), np.mean(result[5])))
        self.assertTrue(np.all(result[6] <= sourceEnergy+energySpread))
        self.assertTrue(np.all(result[6] >= sourceEnergy-energySpread))
        plot_comparison('Soft Edge', result, correct, mean=1)

    def test_pointSourceSoftEdgeMis(self):
        sourceEnergy = 130
        energySpread = 10
        correct = open_old_file(
            'Point Source-RawRaysBeam_SE_mis_s.csv', 'Point Source_')
        result = open_new_file('pointSourceSE_mis.csv')
        self.assertTrue(correct.shape[1] == result.shape[1])
        self.assertTrue(np.isclose(np.mean(correct[5]), np.mean(result[5])))
        self.assertTrue(np.all(result[6] <= sourceEnergy+energySpread))
        self.assertTrue(np.all(result[6] >= sourceEnergy-energySpread))
        plot_comparison('Soft Edge Mis', result, correct, mean=1)

    def test_pointSourceHardEdgeMis(self):
        sourceEnergy = 151
        energySpread = 6
        correct = open_old_file(
            'Point Source-RawRaysBeam_HE_mis_s.csv', 'Point Source_')
        result = open_new_file('pointSourceHE_mis.csv')
        self.assertTrue(correct.shape[1] == result.shape[1])
        self.assertTrue(np.isclose(np.mean(correct[5]), np.mean(result[5])))
        self.assertTrue(np.all(result[6] <= sourceEnergy+energySpread))
        self.assertTrue(np.all(result[6] >= sourceEnergy-energySpread))
        plot_comparison('Hard Edge Mis', result, correct)

    def test_matrixSource20000(self):
        sourceEnergy = 120
        energySpread = 20
        correct = open_old_file(
            'Matrix Source-RawRaysBeam_20000.csv', 'Matrix Source_')
        result = open_new_file('matrixsource20000.csv')
        self.assertTrue(correct.shape[1] == result.shape[1])
        self.assertTrue(np.isclose(np.mean(correct[5]), np.mean(result[5])))
        self.assertTrue(np.all(result[6] <= sourceEnergy+energySpread))
        self.assertTrue(np.all(result[6] >= sourceEnergy-energySpread))
        #plot_comparison('Hard Edge Mis', result, correct)
    
    def test_globalCoord_9rays(self):
        correct = open_old_file(
            'ImagePlane-RawRaysOutgoing_9rays.csv', 'ImagePlane_')
        result = open_new_file('testFile_globalCoordinates_9rays.csv', valid=1)
        self.assertTrue(correct.shape == result[:6].shape)
        self.assertTrue(np.allclose(correct, result[:6]))
    
    def test_globalCoord_20rays(self):
        correct = open_old_file(
            'ImagePlane-RawRaysOutgoing_20rays.csv', 'ImagePlane_')
        result = open_new_file('testFile_globalCoordinates_20rays.csv', valid=1)
        self.assertTrue(correct.shape == result[:6].shape)
        self.assertTrue(np.allclose(correct, result[:6]))
        
    def test_ellipsoid_mirror_default200(self):
        correct = open_old_file(
            'Ellipsoid-RawRaysBeam_default200.csv', 'Ellipsoid_')
        result = open_new_file('testFile_ellipsoid_200default.csv', valid=1)
        self.assertTrue(correct.shape == result[:6].shape)
        self.assertTrue(np.allclose(correct, result[:6]))
    
    def test_ellipsoid_mirror_mirrormis200(self):
        correct = open_old_file(
            'Ellipsoid-RawRaysBeam_200mirrormis.csv', 'Ellipsoid_')
        result = open_new_file('testFile_ellipsoid_200mirrormis.csv', valid=1)
        self.assertTrue(correct.shape == result[:6].shape)
        self.assertTrue(np.allclose(correct, result[:6]))
        
    def test_ellipsoid_mirror_imageplane_default200(self):
        correct = open_old_file(
            'Ellipsoid-ImagePlane-RawRaysOutgoing_default200.csv', 'ImagePlane_')
        result = open_new_file('testFile_ellipsoid_ip_200default.csv', valid=1)
        self.assertTrue(correct.shape == result[:6].shape)
        self.assertTrue(np.allclose(correct, result[:6]))    
    
    def test_ellipsoid_mirror_imageplane_ellipsmis200(self):
        correct = open_old_file(
            'ImagePlane-RawRaysOutgoing_ellipsmis200.csv', 'ImagePlane_')
        result = open_new_file('testFile_ellipsoid_ip_200ellipsmis.csv', valid=1)
        self.assertTrue(correct.shape == result[:6].shape)
        self.assertTrue(np.allclose(correct, result[:6]))    
        
    def test_ellipsoid_mirror_imageplane_mirrormis200(self):
        correct = open_old_file(
            'ImagePlane-RawRaysOutgoing_mirrormis200.csv', 'ImagePlane_')
        result = open_new_file('testFile_ellipsoid_ip_200mirrormis.csv', valid=1)
        self.assertTrue(correct.shape == result[:6].shape)
        self.assertTrue(np.allclose(result[3:6,:], correct[3:6,:])) # check if direction is correct
        self.assertTrue(np.allclose(correct, result[:6]))    
    
    def test_pm_ellipsoid_mirror_imageplane_mirrormis200(self):
        correct = open_old_file(
            'PMmis-ELLmirrormis-IP-200RawRaysOutgoing.csv', 'ImagePlane_')
        result = open_new_file('testFile_pm_ell_ip_200mirrormis.csv', valid=1)
        self.assertTrue(correct.shape == result[:6].shape)
        self.assertTrue(np.allclose(correct, result[:6]))    
        
    def test_cylinder(self):
        correct = open_old_file(
            'ImagePlane-RawRaysBeam.csv','ImagePlane_')
        result = open_new_file('testFile_Cylinder.csv', valid=1) 
        self.assertTrue(correct.shape == result[:6].shape)
        self.assertTrue(np.allclose(correct, result[:6]))
        
    
if __name__ == '__main__':
    unittest.main()
