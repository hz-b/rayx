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
    file: str
    numRZPs: int
    gamma: float


def getArgs():
    RMLParams.file = sys.argv[1]
    RMLParams.numRZPs = int(sys.argv[2])
    RMLParams.gamma = float(sys.argv[3])

    # Temporary dummy
    # RMLParams.file = "C:\Projects\HZB\RAY-X\Scripts\RZP.rml"
    # RMLParams.numRZPs = 6
    # RMLParams.gamma = 0.00203483


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


def getVec(RMLParams):
    origin = np.array([
        float(RMLParams.find("x").text),
        float(RMLParams.find("y").text),
        float(RMLParams.find("z").text)
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


def rotateAroundPoint(point: np.array, angle: float, origin: np.array):
    # Translate point to origin
    point = point - origin
    # Rotation matrix
    rotMat = np.array([
        [np.cos(angle), 0, np.sin(angle)],
        [0, 1, 0],
        [-np.sin(angle), 0, np.cos(angle)]
    ])
    rotatedPoint = np.matmul(rotMat, point)
    # Translate back
    rotatedPoint = rotatedPoint + origin
    return rotatedPoint


def rotateBasisY(mat, angle):
    # Rotation matrix
    rotMatY = np.array([
        [np.cos(angle), 0, np.sin(angle)],
        [0, 1, 0],
        [-np.sin(angle), 0, np.cos(angle)]
    ])
    return np.matmul(mat, rotMatY)


def getRZPParams(rmlRZP):
    params = rmlRZP.findall("param")
    paramsFound = 4
    for param in params:
        # rzp origin
        if param.attrib["id"] == "worldPosition":
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

    return rzpOrigin, rzpXDirection, rzpYDirection, rzpZDirection


# TODO(Jannis): implement for variable source position
def calculateRZP(rmlRZP):
    positions = []
    directions = []

    # Get RZP parameters
    rzpOrigin, rzpXDirection, rzpYDirection, rzpZDirection = getRZPParams(
        rmlRZP)

    # Calculate RZP positions and directions
    rzpDirMat = np.array([rzpXDirection, rzpYDirection, rzpZDirection])
    projectedSourceOrigin = projectPointOntoPlane(
        [0, 0, 0], rzpYDirection, rzpOrigin)
    # distanceProjection = np.linalg.norm(projectedSourceOrigin - rzpOrigin)

    if RMLParams.numRZPs % 2 == 0:
        relativeOrigin = projectedSourceOrigin - rzpOrigin

        # Calculate positions and directions of the left center RZP
        positions.append(rotateAroundPoint(
            [0, 0, 0], -RMLParams.gamma/2, relativeOrigin))
        directions.append(rotateBasisY(
            [[1, 0, 0], [0, 1, 0], [0, 0, 1]], RMLParams.gamma/2))

        # Calculate positions and directions of the right center RZP
        positions.append(rotateAroundPoint(
            [0, 0, 0], RMLParams.gamma/2, relativeOrigin))
        directions.append(rotateBasisY(
            [[1, 0, 0], [0, 1, 0], [0, 0, 1]], -RMLParams.gamma/2))

        # Now we calculate the positions and directions of the RZPs left and right of the central ones
        for i in range(2, RMLParams.numRZPs):
            # left
            if i % 2 == 0:
                angle = (1+i)/2 * -RMLParams.gamma
            # right
            else:
                angle = i/2 * RMLParams.gamma
            relativeOrigin = projectedSourceOrigin - rzpOrigin
            position = rotateAroundPoint(
                [0, 0, 0], -angle, relativeOrigin)
            positions.append(position)
            directions.append(rotateBasisY(
                [[1, 0, 0], [0, 1, 0], [0, 0, 1]], angle))

    else:
        positions.append([0, 0, 0])
        directions.append([[1, 0, 0], [0, 1, 0], [0, 0, 1]])
        # Now we calculate the positions and directions of the RZPs left and right of the central one
        for i in range(1, RMLParams.numRZPs):
            # left
            if i % 2 == 0:
                angle = i/2 * -RMLParams.gamma
            # right
            else:
                angle = (1 + int(i/2)) * RMLParams.gamma

            relativeOrigin = projectedSourceOrigin - rzpOrigin
            position = rotateAroundPoint(
                [0, 0, 0], -angle, relativeOrigin)
            positions.append(position)
            directions.append(rotateBasisY(
                [[1, 0, 0], [0, 1, 0], [0, 0, 1]], angle))

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
    rzpOrigin, rzpXDirection, rzpYDirection, rzpZDirection = getRZPParams(
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
    getArgs()
    if RMLParams.gamma <= 0:
        print("Gamma is less/equal 0. Nothing to do...")
        return
    if RMLParams.numRZPs <= 1:
        print("Number of RZPs is less/equal 1. Nothing to do...")
        return
    # Read RML file
    rmlRZP, root = readRML(RMLParams.file)
    # Calculate RZP positions and directions
    positions, directions = calculateRZP(rmlRZP)
    # Write RML file
    writeRML(root, rmlRZP, positions, directions)


# main
if __name__ == '__main__':
    main()
