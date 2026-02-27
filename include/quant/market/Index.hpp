#ifndef INDEX_HPP
#define INDEX_HPP

#include <string>
#include <memory>
#include <map>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "ZeroCouponCurve.hpp"

namespace Quant::Market {

class Index {
protected:
    std::string name_;
    std::shared_ptr<ZeroCouponCurve> curve_;
    std::map<boost::gregorian::date, double> fixings_;

public:
    Index(std::string name, std::shared_ptr<ZeroCouponCurve> c) 
        : name_(std::move(name)), curve_(std::move(c)), fixings_() {}

    virtual ~Index() = default;

    void add_fixing(const boost::gregorian::date& date, double rate) {
        fixings_[date] = rate;
    }

    virtual double get_rate(const boost::gregorian::date& d1, 
                            const boost::gregorian::date& d2,
                            double yf) const {
        
        // Fixing histórico para la fecha de inicio
        auto it = fixings_.find(d1);
        if (it != fixings_.end()) {
            return it->second; // Si existe, devolvemos el fixing exacto
        }

        // Si no existe, proyectamos el forward usando la curva
        double zc1 = curve_->get_zc(d1);
        double zc2 = curve_->get_zc(d2);
        
        double rc = std::log(zc1 / zc2) / yf; 
        double m = 2.0; 
        return m * (std::exp(rc / m) - 1.0);
    }
};

} // namespace Quant::Market
#endif