#include <boost/test/unit_test.hpp>
#include "quant/market/Index.hpp"
#include "quant/market/ExerciseCurve.hpp"

BOOST_AUTO_TEST_CASE(test_forward_rate_estimation) {
    using namespace Quant;
    auto curve = std::make_shared<Market::ExerciseCurve>(boost::gregorian::date(2016, 4, 1));
    Market::Index index("EURIBOR_6M", curve);

    // Datos de Excel: Periodo 2->3
    double yf = 182.0 / 360.0; 
    double rate = index.get_rate(boost::gregorian::date(2016, 10, 3), 
                                 boost::gregorian::date(2017, 4, 3), yf);

    // Esperado Rm = 5.334%
    BOOST_CHECK_CLOSE(rate, 0.05334, 0.1); 
}