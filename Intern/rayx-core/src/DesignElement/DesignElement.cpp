#include "DesignElement.h"

#include "Debug/Debug.h"

namespace RAYX{
    Element DesignElement::compile() const {
        RAYX_ERR << "TODO!";
        Element e;
        if (v["type"].as_string() == "ImagePlane") {
            makeImagePlane(*this);
        } else if( v["type"].as_string() == "Slit") {
            //makeSlit();
        }
        return e;
    }

    void DesignElement::setName(std::string s) {
        v["name"] = s;
    }

    std::string DesignElement::getName(){
        return v["name"].as_string();
    }

    void DesignElement::setWorldPosition(glm::dvec4 p) {
        v["worldPosition"] = Map();
        v["worldPosition"]["x"] = p.x;
        v["worldPosition"]["y"] = p.y;
        v["worldPosition"]["z"] = p.z;
    }

    void DesignElement::setWorldOrientation(glm::dmat4x4 o) {
        v["worldXDirection"] = Map();
        v["worldXDirection"]["x"] = o[1][1];
        v["worldXDirection"]["y"] = o[1][2];
        v["worldXDirection"]["z"] = o[1][3];

        v["worldYDirection"] = Map();
        v["worldYDirection"]["x"] = o[2][1];
        v["worldYDirection"]["y"] = o[2][2];
        v["worldYDirection"]["z"] = o[2][3];

        v["worldZDirection"] = Map();
        v["worldZDirection"]["x"] = o[3][1];
        v["worldZDirection"]["y"] = o[3][2];
        v["worldZDirection"]["z"] = o[3][3];

    }


}