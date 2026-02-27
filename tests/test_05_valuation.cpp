#include <boost/test/unit_test.hpp>
#include "quant/factory/Factory.hpp"
#include "quant/instruments/Swap.hpp"
#include "quant/factory/SwapBuilder.hpp"
#include "quant/market/MarketCurve.hpp"

BOOST_AUTO_TEST_CASE(test_full_swap_valuation_and_par_rate) {
    using namespace Quant;
    // 1. Configuración de Mercado (1 de Abril de 2016)
    boost::gregorian::date ref_date(2016, 4, 1);
    auto my_curve = std::make_shared<Market::MarketCurve>(ref_date);
    
    // Añades los puntos que quieras dinámicamente
    my_curve->add_rate(boost::gregorian::from_string("2016-10-03"), 0.0474);
    my_curve->add_rate(boost::gregorian::from_string("2017-04-03"), 0.0500);
    my_curve->add_rate(boost::gregorian::from_string("2017-10-02"), 0.0510);
    my_curve->add_rate(boost::gregorian::from_string("2018-04-02"), 0.0520);

    // Definición estándar de un Swap de 100M al 5%
    Description::LegDescription fixed_p(Description::LegDescription::LegType::Fixed, "2016-04-01", 2, 2, 100e6, 0.05, "", "ACT_360");
    Description::LegDescription float_r(Description::LegDescription::LegType::Floating, "2016-04-01", 2, 2, 100e6, 0.0, "EURIBOR_6M", "ACT_360");
    Description::InstrumentDescription desc(Description::InstrumentDescription::Type::swap, fixed_p, float_r, my_curve);

    auto irs_ptr = Factory::Factory::instance()(desc);
    auto* swap = dynamic_cast<Instruments::Swap*>(irs_ptr.get());

    BOOST_REQUIRE(swap != nullptr);

    double npv = swap->price();
    double par_rate = swap->calculate_par_rate();

    // El NPV no debe ser cero si la tasa fija (5%) no es la tasa par del mercado
    BOOST_CHECK(std::abs(npv) > 0.0);
    
    // La tasa par debe estar en un rango razonable (ej. entre 4.5% y 5.5%)
    BOOST_CHECK_GT(par_rate, 0.045);
    BOOST_CHECK_LT(par_rate, 0.055);
}