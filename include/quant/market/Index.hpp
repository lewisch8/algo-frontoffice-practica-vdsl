#ifndef INDEX_HPP
#define INDEX_HPP

#include <string>
#include <memory>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "ZeroCouponCurve.hpp"

namespace Quant::Market {

class Index {
protected:
    std::string name_;
    std::shared_ptr<ZeroCouponCurve> curve_;

public:
    Index(std::string name, std::shared_ptr<ZeroCouponCurve> c) 
        : name_(std::move(name)), curve_(std::move(c)) {}

    virtual ~Index() = default;

    // Usamos get_rate con 3 parámetros para consistencia con tu FloatingLeg
    virtual double get_rate(const boost::gregorian::date& d1, 
                        const boost::gregorian::date& d2,
                        double yf) const {
        // 1. Obtenemos factores de descuento para d1 y d2
        double zc1 = curve_->get_zc(d1);
        double zc2 = curve_->get_zc(d2);
        
        // 2. Calculamos el tipo forward continuamente compuesto (Rc o RFi)
        // Matemáticamente: ZC = exp(-R*T), por lo que Rc = ln(ZC1/ZC2) / Delta
        double rc = std::log(zc1 / zc2) / yf; 

        // 3. Transformamos Rc a tipo semianual (Rm) con m=2
        // Esta es la clave para que coincida con tu columna "Rm" de Excel
        double m = 2.0; 
        double rm = m * (std::exp(rc / m) - 1.0);
        
        return rm;
    }
};

} // namespace Quant::Market
#endif