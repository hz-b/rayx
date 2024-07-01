#include "Strings.h"


namespace RAYX {

ElementType findElementString(std::string name) {
    for (const auto& pair : ElementStringMap) {
        if (pair.second == name) {
            return pair.first;
        }else{
            RAYX_ERR << "Error in findElementString(std::string)";
            return ElementType::ImagePlane;
        }
    }
}
} //namespace