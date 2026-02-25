#ifndef ZERO_COUPON_CURVE_HPP
#define ZERO_COUPON_CURVE_HPP

#include <boost/date_time/gregorian/gregorian.hpp>

namespace Quant::Market {

class ZeroCouponCurve {
public:
    virtual ~ZeroCouponCurve() = default;

    // Métodos definidos en tu diagrama
    virtual double get_zc(const boost::gregorian::date& d) const = 0;
    virtual double get_dcf(const boost::gregorian::date& d1, const boost::gregorian::date& d2) const = 0;
};

} // namespace Quant::Market
#endif