import sys
import xml.etree.ElementTree as ET

input_file = sys.argv[1]

file = input_file.split("/")[-1]
outputname = input_file.split("/")[0] + "/" + file.split(".")[0] + "_Coatings.rml"

print(f"Processing file: {input_file}")
print(f"Output file: {outputname}")

tree = ET.parse(input_file)
root = tree.getroot()

for obj in root.findall(".//object"):
    reflectivity = obj.find("param[@id='reflectivityType']")
    if reflectivity is None or "Derived" not in (reflectivity.get("comment") or ""):
        continue

    # Alte Coating-Parameter sammeln
    coatings = []
    i = 1
    while True:
        mat = obj.find(f"param[@id='materialCoating{i}']")
        thick = obj.find(f"param[@id='thicknessCoating{i}']")
        rough = obj.find(f"param[@id='roughnessCoating{i}']")
        if mat is None or thick is None or rough is None:
            break
        coatings.append({
            "material": mat.text,
            "thickness": thick.text,
            "roughness": rough.text
        })
        i += 1

    # Toplayer prüfen und anhängen
    mat_top = obj.find("param[@id='materialTopLayer']")
    thick_top = obj.find("param[@id='thicknessTopLayer']")
    rough_top = obj.find("param[@id='roughnessTopLayer']")

    if mat_top is not None and thick_top is not None:
        if rough_top is None:
            rough_top.text = "0.0"
        coatings.append({
            "material": mat_top.text,
            "thickness": thick_top.text,
            "roughness": rough_top.text
        })

    for param in obj.findall("param"):
        if any(x in (param.get("id") or "") for x in
               ["surfaceCoating"]):
            obj.remove(param)

    if coatings:
        # Neue Coating-Struktur einfügen
        coating_param = ET.Element("param", {"id": "Coating", "enabled": "T"})
        for idx, c in enumerate(coatings):
            layer = ET.SubElement(coating_param, "layer", {
                "material": c["material"],
                "thickness": c["thickness"],
                "roughness": c["roughness"]
            })
            layer.text = f"layer{idx}" if f"layer{idx}" else ""
        obj.append(coating_param)

        num_layers = str(len(coatings))

        #surface_param = ET.Element("param", {
        #    "id": "surfaceCoating",
        #    "comment": "Multilayer Coating",
        #    "enabled": "T"
        #})
        #surface_param.text = "2"

        num_param = ET.Element("param", {
            "id": "NumberOfLayer",
            "enabled": "T"
        })
        num_param.text = num_layers

        #obj.append(surface_param)
        obj.append(num_param)

def indent(elem, level=0):
    i = "\n" + level * "  "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
        for e in elem:
            indent(e, level + 1)
            if not e.tail or not e.tail.strip():
                e.tail = i + "  "
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i
indent(root)
tree.write(outputname, encoding="UTF-8", xml_declaration=True, short_empty_elements=False)