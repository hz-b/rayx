#include "DesignElement.h"

#include "Debug/Debug.h"

namespace RAYX{
    Element DesignElement::compile() const {
        RAYX_ERR << "TODO!";
        Element e;
        if ( v["type"].as_string() == "ImagePlane") {
            //makeImagePlane(*this);
        }
        return e;
    }
}


