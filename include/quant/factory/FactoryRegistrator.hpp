#ifndef FACTORY_REGISTRATOR_HPP
#define FACTORY_REGISTRATOR_HPP

#include "Factory.hpp"

namespace Quant::Factory {

template <class BuilderClass>
class FactoryRegistrator {
public:
    FactoryRegistrator() {
        // Al instanciar este objeto, se registra el constructor en la Factory
        Factory::instance().register_constructor(BuilderClass::getId(), &BuilderClass::build);
    }
};

} // namespace Quant::Factory
#endif