#include "BeamlineObjectFactory.h"


namespace RAYX
{
    std::shared_ptr<BeamlineObjectFactory::TypeMap> BeamlineObjectFactory::m_TypeMap = std::shared_ptr<TypeMap>();

    std::shared_ptr<BeamlineObjectFactory::TypeMap> BeamlineObjectFactory::getMap()
    {
        // never delete'ed. (exist until program termination)
        // because we can't guarantee correct destruction order 
        if (!m_TypeMap) { m_TypeMap = std::shared_ptr<TypeMap>(); }
        return m_TypeMap;
    }

    BeamlineObject* BeamlineObjectFactory::createInstance(std::string const& s)
    {
        TypeMap::iterator it = getMap()->find(s);
        if (it == getMap()->end())
            return 0;
        return it->second();
    }

} // namespace RAYX
