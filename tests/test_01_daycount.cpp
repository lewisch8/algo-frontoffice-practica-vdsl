#include <boost/test/unit_test.hpp>
#include "quant/daycount/Actual_360.hpp"

BOOST_AUTO_TEST_CASE(test_year_fractions_calculation) {
    Quant::Time::Actual_360 act360;
    
    // Entre el 2016-04-01 y el 2016-10-03 hay exactamente 185 días
    // La fracción debe ser 185 / 360
    double yf = act360("2016-04-01", "2016-10-03");
    double expected = 185.0 / 360.0;
    
    BOOST_CHECK_CLOSE(yf, expected, 0.0001);
}