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
RAY_REWORKED = p_join(path, 'Tests\output')
print(RAY_REWORKED)

# where the results from old ray are
#RAY_UI = os.path.split(os.path.split(path)[0])[0]
#RAY_UI = p_join(RAY_UI, 'python')
#RAY_UI = p_join(RAY_UI, 'test_output')
#RAY_UI = p_join(RAY_UI, 'unittestData')
RAY_UI = p_join(RAY_REWORKED, 'oldray_files')
RAY_UI = p_join(RAY_UI, 'unittestData')

scatterplot_size = 0.5


def open_new_file(name, valid=0):
    df = pd.read_csv(p_join(RAY_REWORKED, name), sep=';')
    m_o = np.array([df['Xloc'].values, df['Yloc'].values, df['Zloc'].values,
                   df['Xdir'].values, df['Ydir'].values, df['Zdir'].values], dtype=np.double)
    weights = np.array(df['Weight'].values)
    # ignore rays that would have been thrown away in old ray (weight = 0)
    if valid == 1:
        valid_weights = np.where([weights == 1])[1]
        m_o = m_o[:, valid_weights]
        return m_o
    return m_o


def open_old_file(name, prefix):
    df = pd.read_csv(p_join(RAY_UI, name), decimal=',', sep=';')
    m = np.array([df[prefix+'OX'].values, df[prefix+'OY'].values, df[prefix+'OZ'].values,
                 df[prefix+'DX'].values, df[prefix+'DY'].values, df[prefix+'DZ'].values], dtype=np.double)
    return m


def plot_comparison(name, result, correct):
    fig, ax = plt.subplots(2, 3, figsize=(10, 6))
    ax[0][0].set_ylabel('New - y')
    ax[0][0].scatter(result[0], result[1], s=scatterplot_size)
    ax[0][0].set_xlabel("x")
    ax[0][1].scatter(result[0], result[2], s=scatterplot_size)
    ax[0][1].set_xlabel('x')
    ax[0][1].set_ylabel('z')
    ax[0][2].scatter(result[3], result[4], s=scatterplot_size)
    ax[0][2].set_xlabel('x-dir')
    ax[0][2].set_ylabel('y-dir')

    ax[1][0].set_ylabel('Old - y')
    ax[1][0].set_xlabel('x')
    ax[1][0].scatter(correct[0], correct[1], s=scatterplot_size)
    ax[1][1].set_xlabel('x')
    ax[1][1].set_ylabel('y')
    ax[1][1].scatter(correct[0], correct[2], s=scatterplot_size)
    ax[1][2].set_xlabel('x-dir')
    ax[1][2].set_ylabel('y-ddir')
    ax[1][2].scatter(correct[3], correct[4], s=scatterplot_size)
    fig.suptitle(name)
    fig.tight_layout()
    plt.show()


class Tests(unittest.TestCase):
    """
    def test_planeMirrorDefault(self):
        correct = open_old_file('Plane Mirror-RawRaysBeam_default.csv', 'Plane Mirror_')
        result = open_new_file('testFile_PlaneMirrorDef.csv')
        self.assertTrue(np.allclose(correct, result))

    def test_planeMirrorMis(self):
        correct = open_old_file('Plane Mirror-RawRaysBeam_20mis.csv', 'Plane Mirror_')
        result = open_new_file('testFile_PlaneMirrorMis.csv')
        self.assertTrue(np.allclose(correct, result))

    def test_sphereMirrorDefault(self):
        correct = open_old_file('Sphere-RawRaysBeam_default.csv', 'Sphere_')
        result = open_new_file('testFile_SphereMirrorDefault.csv')
        self.assertTrue(np.allclose(correct, result))

    def test_planeGratingDevDefault(self):
        correct = open_old_file('Plane Grating_div_default.csv', 'Plane Grating_')
        result = open_new_file('testFile_PlaneGratingDeviationDefault.csv')
        self.assertTrue(np.allclose(correct, result))

    def test_planeGratingDevAzimuthal(self):
        correct = open_old_file('Plane Grating_div_az.csv', 'Plane Grating_')
        result = open_new_file('testFile_PlaneGratingDeviationAz.csv')
        self.assertTrue(np.allclose(correct, result))

    def test_planeGratingDevMis(self):
        correct = open_old_file('Plane Grating_div_az_mis.csv', 'Plane Grating_')
        result = open_new_file('testFile_PlaneGratingDeviationMis.csv')
        self.assertTrue(np.allclose(correct, result))

    def test_planeGratingIncAzMis(self):
        correct = open_old_file('Plane Grating_inc_az_mis.csv', 'Plane Grating_')
        result = open_new_file('testFile_PlaneGratingIncAzMis.csv')
        self.assertTrue(np.allclose(correct, result))


    def test_planeGratingVLSnoMIS(self):
        correct = open_old_file('Plane Grating_default_azimuthal.csv', 'Plane Grating_')
        result = open_new_file('output.csv')
        self.assertTrue(np.allclose(correct, result))


    def test_RZPdefault(self):
        correct = open_old_file('Reflection Zoneplate-RawRaysBeam_def20_s.csv', 'Reflection Zoneplate_')
        result = open_new_file('testFile_ReflectionZonePlateDefault.csv', valid=1)
        self.assertTrue(np.allclose(correct, result))

    def test_RZPazimuthal(self):
        correct = open_old_file('Reflection Zoneplate-RawRaysBeam_azim200.csv', 'Reflection Zoneplate_')
        result = open_new_file('testFile_ReflectionZonePlateAzim200.csv', valid=1)
        self.assertTrue(np.allclose(correct, result))

    def test_ImagePlaneMatrixSource(self):
        correct = open_old_file('ImagePlane-RawRaysOutgoing.csv', 'ImagePlane_')
        result = open_new_file('testFile_PlaneMirror_ImagePlane.csv', valid=1)
        self.assertTrue(np.allclose(correct, result))

    def test_RZPmis(self):
        correct = open_old_file('Reflection Zoneplate-RawRaysBeam_mis200_s.csv', 'Reflection Zoneplate_')
        result = open_new_file('testFile_ReflectionZonePlateMis.csv', valid=1)
        self.assertTrue(np.allclose(correct, result))
    """

    def test_pointSourceHardEdge(self):
        correct = open_old_file(
            'Point Source-RawRaysBeam_HE_s.csv', 'Point Source_')
        result = open_new_file('pointSourceHE.csv')
        self.assertTrue(correct.shape == result.shape)
        self.assertTrue(np.isclose(np.mean(correct[5]), np.mean(result[5])))
        #plot_comparison('Hard Edge', result, correct)

    def test_pointSourceSoftEdge(self):
        correct = open_old_file(
            'Point Source-RawRaysBeam_SE_s.csv', 'Point Source_')
        result = open_new_file('pointSourceSE.csv')
        self.assertTrue(correct.shape == result.shape)
        self.assertTrue(np.isclose(np.mean(correct[5]), np.mean(result[5])))
        #plot_comparison('Soft Edge', result, correct)

    def test_pointSourceSoftEdgeMis(self):
        correct = open_old_file(
            'Point Source-RawRaysBeam_SE_mis_s.csv', 'Point Source_')
        result = open_new_file('pointSourceSE_mis.csv')
        self.assertTrue(correct.shape == result.shape)
        self.assertTrue(np.isclose(np.mean(correct[5]), np.mean(result[5])))
        #plot_comparison('Soft Edge Mis', result, correct)

    def test_pointSourceHardEdgeMis(self):
        correct = open_old_file(
            'Point Source-RawRaysBeam_HE_mis_s.csv', 'Point Source_')
        result = open_new_file('pointSourceHE_mis.csv')
        self.assertTrue(correct.shape == result.shape)
        self.assertTrue(np.isclose(np.mean(correct[5]), np.mean(result[5])))
        #plot_comparison('Hard Edge Mis', result, correct)
    
    def test_matrixSource20000(self):
        correct = open_old_file(
            'Matrix Source-RawRaysBeam_20000.csv', 'Matrix Source_')
        result = open_new_file('matrixsource20000.csv')
        self.assertTrue(correct.shape == result.shape)
        self.assertTrue(np.isclose(np.mean(correct[5]), np.mean(result[5])))
        #plot_comparison('Hard Edge Mis', result, correct)
    
    """
    def test_globalCoord_9rays(self):
        correct = open_old_file('Plane Mirror 3-RawRaysOutgoing_9rays.csv', 'Plane Mirror 3_')
        result = open_new_file('testFile_globalCoordinates_9rays.csv', valid=1)
        self.assertTrue(np.allclose(correct, result))
    
    def test_globalCoord_20rays(self):
        correct = open_old_file('Plane Mirror 3-RawRaysOutgoing_20Rays_mis.csv', 'Plane Mirror 3_')
        result = open_new_file('output.csv', valid=1)
        self.assertTrue(np.allclose(correct, result))
    """


if __name__ == '__main__':
    unittest.main()
