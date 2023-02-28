import sys
import os
from dataclasses import dataclass
import xml.etree.ElementTree as ET
import numpy as np
import copy

# Parameters
# 1. RML file to edit (XML format)
# 2. Number of RZPs
# 3. Angle of rotation (gamma, in radians)


@dataclass
class RMLParams:
    file: str = ""
    numRZPs: int = 1
    gamma: float = 0.0


def getArgs():
    params = RMLParams()
    # params.file = sys.argv[1]
    # params.numRZPs = int(sys.argv[2])
    # params.gamma = float(sys.argv[3])

    # Temporary dummy
    # TODO: get rml file from Pete
    params.file = "C:\Projects\HZB\RAY-X\Scripts\RZP.rml"
    params.numRZPs = 5
    params.gamma = 0.1
    return params


def readRML(file):
    tree = ET.parse(file)
    root = tree.getroot()
    elements = root.find("beamline").findall("object")
    # Get object with name attribute "Reflection Zoneplate"
    for element in elements:
        if element.attrib["name"] == "Reflection Zoneplate":
            return element, root
    print("Error: Could not find object with name 'Reflection Zoneplate'")
    exit(1)


def getVec(rmlParam):
    origin = np.array([
        float(rmlParam.find("x").text),
        float(rmlParam.find("y").text),
        float(rmlParam.find("z").text)
    ])
    return origin


# Project point onto plane
def projectPointOntoPlane(point: np.array, planeNormal: np.array, planeOrigin: np.array):
    # Make sure planeNormal is a unit vector
    planeNormal = planeNormal / np.linalg.norm(planeNormal) 
    v = point - planeOrigin
    d = np.dot(v, planeNormal)
    projectedPoint = point - d * planeNormal
    return projectedPoint


def rotateAroundPoint(origin: np.array, angle: float, point: np.array):
    # Rotate point around origin and y-axis by angle
    s = np.sin(angle)
    c = np.cos(angle)

    # Translate point back to origin
    point = point - origin

    # Rotate point
    xnew = point[0] * c - point[2] * s
    znew = point[0] * s + point[2] * c

    # Translate point back
    point = np.array([xnew, point[1], znew])
    point = point + origin

    return point


def rotateMatrix(mat, angle):
    # Rotation matrix
    rotMatY = np.array([
        [np.cos(angle), 0, np.sin(angle)],
        [0, 1, 0],
        [-np.sin(angle), 0, np.cos(angle)]
    ])
    return np.matmul(mat, rotMatY)


def getRZPParams(rmlRZP):
    params = rmlRZP.findall("param")
    paramsFound = 6
    for param in params:
        if param.attrib["id"] == "distancePreceding":
            distancePreceding = float(param.text)
            paramsFound -= 1
        # grazingIncAngle
        elif param.attrib["id"] == "grazingIncAngle":
            grazingIncAngle = float(param.text)
            paramsFound -= 1
        # rzp origin
        elif param.attrib["id"] == "worldPosition":
            rzpOrigin = getVec(param)
            paramsFound -= 1
        # rzp source
        elif param.attrib["id"] == "worldXdirection":
            rzpXDirection = getVec(param)
            paramsFound -= 1
        elif param.attrib["id"] == "worldYdirection":
            rzpYDirection = getVec(param)
            paramsFound -= 1
        elif param.attrib["id"] == "worldZdirection":
            rzpZDirection = getVec(param)
            paramsFound -= 1

    if paramsFound != 0:
        print("Error: Not all RZP parameters found")
        exit(1)

    return distancePreceding, grazingIncAngle, rzpOrigin, rzpXDirection, rzpYDirection, rzpZDirection


# TODO(Jannis): implement for variable source position
def calculateRZP(rmlRZP, numRZPs, gamma):
    positions = []
    directions = []

    # Get RZP parameters
    distancePreceding, grazingIncAngle, rzpOrigin, rzpXDirection, rzpYDirection, rzpZDirection = getRZPParams(
        rmlRZP)

    # Calculate RZP positions and directions
    rzpDirMat = np.array([rzpXDirection, rzpYDirection, rzpZDirection])
    projectedSourceOrigin = projectPointOntoPlane(
        [0, 0, 0], rzpYDirection, rzpOrigin)
    distanceProjection = np.linalg.norm(projectedSourceOrigin - rzpOrigin)

    if RMLParams.numRZPs % 2 == 0:
        print("Error: Number of RZPs must be odd")
        exit(1)
    else:
        positions.append([0, 0, 0])
        directions.append([[1, 0, 0], [0, 1, 0], [0, 0, 1]])
        # Now we calculate the positions and directions of the RZPs left and right of the central one
        for i in range(1, numRZPs):
            # left
            if i % 2 == 0:
                angle = (1 + int(i/2)) * -gamma
                positions.append(rotateAroundPoint(
                    rzpOrigin, angle, projectedSourceOrigin))
                directions.append(rotateMatrix(rzpDirMat, angle))
            # right
            else:
                angle = (1 + int(i/2)) * gamma
                positions.append(rotateAroundPoint(
                    rzpOrigin, angle, projectedSourceOrigin))
                directions.append(rotateMatrix(rzpDirMat, angle))

    return positions, directions


def appendVecToElement(element, name, vec):
    rmlVec = ET.Element("param")
    rmlVec.attrib["id"] = name
    rmlVec.attrib["enabled"] = "true"
    x = ET.Element("x")
    x.text = str(vec[0])
    y = ET.Element("y")
    y.text = str(vec[1])
    z = ET.Element("z")
    z.text = str(vec[2])
    rmlVec.append(x)
    rmlVec.append(y)
    rmlVec.append(z)
    element.append(rmlVec)


def indent(elem, level=0):
    i = "\n" + level*"  "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
        for elem in elem:
            indent(elem, level+1)
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i


# Replaces original RZP with group of RZPs
def writeRML(root, rmlRZP, positions, directions):
    assert len(positions) == len(directions)

    beamline = root.find("beamline")
    _, _, rzpOrigin, rzpXDirection, rzpYDirection, rzpZDirection = getRZPParams(
        rmlRZP)

    # Add group for rzps
    group = ET.Element("group")
    beamline.insert(1, group)
    group.attrib["name"] = "RZP Group"

    # Add group position and direction
    appendVecToElement(group, "worldPosition", rzpOrigin)
    appendVecToElement(group, "worldXdirection", rzpXDirection)
    appendVecToElement(group, "worldYdirection", rzpYDirection)
    appendVecToElement(group, "worldZdirection", rzpZDirection)

    # Add RZPs
    for i in range(len(positions)):
        # Copy original RZP
        newRZP = copy.deepcopy(rmlRZP)
        # Set new name and id
        newRZP.attrib["name"] = "Reflection Zoneplate " + str(i)
        # Set new position and direction
        for param in newRZP.findall("param"):
            if param.attrib["id"] == "worldPosition":
                param.find("x").text = str(positions[i][0])
                param.find("y").text = str(positions[i][1])
                param.find("z").text = str(positions[i][2])
            elif param.attrib["id"] == "worldXdirection":
                param.find("x").text = str(directions[i][0][0])
                param.find("y").text = str(directions[i][0][1])
                param.find("z").text = str(directions[i][0][2])
            elif param.attrib["id"] == "worldYdirection":
                param.find("x").text = str(directions[i][1][0])
                param.find("y").text = str(directions[i][1][1])
                param.find("z").text = str(directions[i][1][2])
            elif param.attrib["id"] == "worldZdirection":
                param.find("x").text = str(directions[i][2][0])
                param.find("y").text = str(directions[i][2][1])
                param.find("z").text = str(directions[i][2][2])
        # Add RZP to group
        group.append(newRZP)

    # Remove original RZP
    beamline.remove(rmlRZP)

    # Write RML file
    indent(root)
    tree = ET.ElementTree(root)
    name = "MultiRZP" + str(len(positions)) + ".rml"
    tree.write(name, encoding="utf-8", xml_declaration=True)


def main():
    # Get parameters
    params = getArgs()
    if params.gamma <= 0:
        print("Gamma is less/equal 0. Nothing to do...")
        return
    if params.numRZPs <= 1:
        print("Number of RZPs is less/equal 1. Nothing to do...")
        return
    # Read RML file
    rmlRZP, root = readRML(params.file)
    # Calculate RZP positions and directions
    positions, directions = calculateRZP(rmlRZP, params.numRZPs, params.gamma)
    # Write RML file
    writeRML(root, rmlRZP, positions, directions)


# main
if __name__ == '__main__':
    main()
