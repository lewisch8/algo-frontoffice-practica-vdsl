#ifndef LEG_DESCRIPTION_HPP
#define LEG_DESCRIPTION_HPP

#include <string>

namespace Quant::Description {

struct LegDescription {
    // Usamos 'enum class' para mayor seguridad de tipos
    enum class LegType { Fixed, Floating };

    LegType type;
    std::string start_date;
    int years;
    int frequency;
    double notional;
    double rate_or_spread;  // Si es Fixed: la tasa. Si es Floating: el spread.
    std::string index_name; // Solo se usa si es Floating (ej. "LIBOR_6M")
    std::string day_count;  // Para tu Factory: "ACT_360", "30_360", etc.

    // Constructor completo cumpliendo effc++ (inicialización en lista)
    LegDescription(LegType t, std::string start, int y, int freq, 
                   double notional_val, double rate, std::string idx, std::string dc)
        : type(t), 
          start_date(std::move(start)), 
          years(y), 
          frequency(freq),
          notional(notional_val), 
          rate_or_spread(rate), 
          index_name(std::move(idx)), 
          day_count(std::move(dc)) 
    {}

    // Constructor por defecto (necesario para cuando se anida en otra clase)
    LegDescription() 
        : type(LegType::Fixed), 
          start_date(""), 
          years(0), 
          frequency(1), 
          notional(0.0), 
          rate_or_spread(0.0), 
          index_name(""), 
          day_count("") 
    {}
};

} // namespace Quant::Description

#endif