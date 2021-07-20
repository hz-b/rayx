#pragma once

#include <map>
#include <memory>
#include <string>

#include "BeamlineObject.h"
#include "Core.h"

#define REGISTER_DEC_TYPE(NAME) \
    static BeamlineObjectRegister<NAME> reg

#define REGISTER_DEF_TYPE(NAME) \
    BeamlineObjectRegister<NAME> NAME::reg(#NAME)


namespace RAYX
{
    class RAY_API BeamlineObjectFactory {
    public:
        typedef std::map<std::string, BeamlineObject* (*)()> TypeMap;

        // Factory
        static BeamlineObjectFactory getInstance();
        ~BeamlineObjectFactory();

        // For creating objects
        static BeamlineObject* createInstance(std::string const& s);

    protected:
        static std::shared_ptr<TypeMap> getMap();

    private:
        static std::shared_ptr<TypeMap> m_TypeMap;
        BeamlineObjectFactory();

    };


    template<typename T>
    BeamlineObject* createT() { return new T; }

    template<typename T>
    struct BeamlineObjectRegister : BeamlineObjectFactory {
        BeamlineObjectRegister(std::string const& s) {
            getMap()->insert(
                std::make_pair(s, &createT<T>));
        }
    };

} // namespace RAYX
