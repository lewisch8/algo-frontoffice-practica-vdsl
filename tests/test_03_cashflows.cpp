#include <boost/test/unit_test.hpp>
#include "quant/market/ExerciseCurve.hpp"
#include "quant/instruments/FixedLeg.hpp"
#include "quant/daycount/DayCountFactory.hpp"

BOOST_AUTO_TEST_CASE(test_cashflow_amounts) {
    using namespace Quant;
    auto curva = std::make_shared<Market::ExerciseCurve>(boost::gregorian::date(2016, 4, 1));
    
    // DayCountFactory no lleva prefijo Quant::Time::
    auto calc = DayCountFactory::create(DayCountFactory::Convention::ACT_360);
    Time::Schedule sched("2016-04-01", 1, 2, *calc);
    
    Instruments::FixedLeg leg(std::move(sched), 100e6, 0.05);
    auto flujos = leg.get_cashflows(*curva);

    double expected = 100e6 * 0.05 * flujos[0].year_fraction;
    BOOST_CHECK_CLOSE(flujos[0].amount, expected, 0.01);
}