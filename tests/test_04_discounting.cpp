#include <boost/test/unit_test.hpp>
#include "quant/instruments/FixedLeg.hpp"
#include "quant/market/ExerciseCurve.hpp"
#include "quant/daycount/DayCountFactory.hpp"

BOOST_AUTO_TEST_CASE(test_discount_factors) {
    using namespace Quant;
    auto curva = std::make_shared<Market::ExerciseCurve>(boost::gregorian::date(2016, 4, 1));
    auto calc = DayCountFactory::create(DayCountFactory::Convention::ACT_360);
    
    Time::Schedule sched("2016-04-01", 1, 2, *calc);
    Instruments::FixedLeg leg(std::move(sched), 100e6, 0.05);
    
    auto flujos = leg.get_cashflows(*curva);

    for (const auto& cf : flujos) {
        // PV debe ser igual a Amount * Factor de Descuento
        BOOST_CHECK_CLOSE(cf.pv, cf.amount * cf.discount, 0.000001);
        BOOST_CHECK_LT(cf.discount, 1.0); // ZC < 1
    }
}