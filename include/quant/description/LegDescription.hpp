#ifndef LEG_DESCRIPTION_HPP
#define LEG_DESCRIPTION_HPP

#include <string>
#include <memory>
#include "../market/Index.hpp" // Incluimos el Index

namespace Quant::Description {

struct LegDescription {
    enum class LegType { Fixed, Floating };

    LegType type;
    std::string start_date;
    int years;
    int frequency;
    double notional;
    double rate_or_spread;  
    
    std::shared_ptr<Market::Index> index_obj; 
    
    std::string day_count;  

    // Constructor
    LegDescription(LegType t, std::string start, int y, int freq, 
                   double notional_val, double rate, 
                   std::shared_ptr<Market::Index> idx, std::string dc)
        : type(t), start_date(std::move(start)), years(y), frequency(freq),
          notional(notional_val), rate_or_spread(rate), 
          index_obj(std::move(idx)), day_count(std::move(dc)) 
    {}

    // Constructor por defecto
    LegDescription() 
        : type(LegType::Fixed), start_date(""), years(0), frequency(1), 
          notional(0.0), rate_or_spread(0.0), index_obj(nullptr), day_count("") 
    {}
};

} // namespace Quant::Description
#endif