#ifndef MARKET_CURVE_HPP
#define MARKET_CURVE_HPP

#include "quant/market/ZeroCouponCurve.hpp"
#include "quant/daycount/DayCountFactory.hpp"
#include <map>
#include <cmath>
#include <memory>

namespace Quant::Market {

class MarketCurve : public ZeroCouponCurve {
private:
    boost::gregorian::date ref_date_;
    std::map<boost::gregorian::date, double> rates_; // Tasas continuas
    std::shared_ptr<DayCountCalculator> dcf_calc_;

public:
    MarketCurve(boost::gregorian::date ref) 
        : ref_date_(ref), 
          rates_(), 
          dcf_calc_(DayCountFactory::create(DayCountFactory::Convention::ACT_360)) 
    {}

    void add_rate(const boost::gregorian::date& d, double r) {
        rates_[d] = r;
    }

    double get_zc(const boost::gregorian::date& d) const override {
        if (d == ref_date_) return 1.0;
        
        // Buscamos la tasa para esa fecha
        auto it = rates_.find(d);
        if (it == rates_.end()) {
            throw std::runtime_error("Fecha no encontrada en la curva: " + boost::gregorian::to_iso_extended_string(d));
        }

        double r = it->second;
        double t = get_dcf(ref_date_, d);
        return std::exp(-r * t); // Capitalización continua
    }

    double get_dcf(const boost::gregorian::date& d1, const boost::gregorian::date& d2) const override {
        return (*dcf_calc_)(d1, d2);
    }
};

} // namespace Quant::Market
#endif