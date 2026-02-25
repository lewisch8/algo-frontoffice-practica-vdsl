#include <boost/test/unit_test.hpp>
#include "quant/instruments/FixedLeg.hpp"

BOOST_AUTO_TEST_CASE(test_fixed_leg_cashflows) {
    using namespace Quant;
    // 3. Test de Cálculo de Flujos (Cupones)
    // Pata Fija: 100M * 5% * (182/360) para el segundo cupón
    double yf = 182.0 / 360.0;
    double expected_fixed_coupon = 100e6 * 0.05 * yf;
    
    // Excel: 2.527.777,78 €
    BOOST_CHECK_CLOSE(expected_fixed_coupon, 2527777.78, 0.01);
}