#ifndef FACTORY_HPP
#define FACTORY_HPP

#include <map>
#include <memory>
#include <functional>
#include <stdexcept>
#include "../description/InstrumentDescription.hpp"
#include "../instruments/Instrument.hpp"

namespace Quant::Factory {

class Factory {
public:
    // Firma del constructor especializado
    typedef std::function<std::unique_ptr<Instruments::Instrument>(const Description::InstrumentDescription&)> Builder;

    // Singleton: instancia única
    static Factory& instance() {
        static Factory factory;
        return factory;
    }

    // Registro de nuevos constructores (ej. SwapBuilder)
    void register_constructor(Description::InstrumentDescription::Type id, const Builder& builder) {
        buildersMap_.insert(std::make_pair(id, builder));
    }

    // El método principal: crea instrumentos a partir de una descripción
    std::unique_ptr<Instruments::Instrument> operator()(const Description::InstrumentDescription& description) const {
        auto it = buildersMap_.find(description.type);
        if (it == buildersMap_.end()) {
            throw std::runtime_error("Error: Tipo de instrumento no registrado en la Factory.");
        }
        return (it->second)(description);
    }

private:
    Factory() : buildersMap_() {} // effc++: inicialización de lista
    std::map<Description::InstrumentDescription::Type, Builder> buildersMap_;
};

} // namespace Quant::Factory
#endif