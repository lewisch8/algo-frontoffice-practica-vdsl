#include "quant/market/ZeroCouponCurve.hpp"
#include <map>
#include <cmath>

namespace Quant::Market {

class ExerciseCurve : public ZeroCouponCurve {
private:
    boost::gregorian::date ref_date_;
    std::map<boost::gregorian::date, double> rates_;
    std::shared_ptr<DayCountCalculator> dcf_calc_;

public:
    ExerciseCurve(boost::gregorian::date ref) : ref_date_(ref), rates_(), dcf_calc_(DayCountFactory::create(DayCountFactory::Convention::ACT_360)) {
        rates_[boost::gregorian::from_string("2016-10-03")] = 0.0474;
        rates_[boost::gregorian::from_string("2017-04-03")] = 0.0500;
        rates_[boost::gregorian::from_string("2017-10-02")] = 0.0510;
        rates_[boost::gregorian::from_string("2018-04-02")] = 0.0520;
    }

    double get_zc(const boost::gregorian::date& d) const override {
        double r = rates_.at(d);
        double t = get_dcf(ref_date_, d);
        return std::exp(-r * t); // Capitalización continua
    }

    double get_dcf(const boost::gregorian::date& d1, const boost::gregorian::date& d2) const override {
        // Para la curva ZC suele usarse Act/365 para el factor de tiempo 't'
        return (*dcf_calc_)(d1, d2);
    }
};

} // namespace Quant::Market