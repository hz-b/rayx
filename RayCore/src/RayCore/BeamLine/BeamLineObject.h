#pragma once

#include "Core.h"
#include <vector>

namespace RAY {
    /**
     *  TODO: A comprehensive description for the BeamLineObject class
     */
    class RAY_API BeamLineObject
    {
    public:
        /**
         * @brief Different types of beamline objects as enum.
         *  These values can be combined using "+" or "|"
         */
        enum Type {
            NONE = 0,
            SOURCE = 1,
            OPTICAL_ELEMENT = 2,
            IMAGE_PLANE = 4,
            ALL = 255
        };

    private:
        BeamLineObject();
        ~BeamLineObject();


    };

}