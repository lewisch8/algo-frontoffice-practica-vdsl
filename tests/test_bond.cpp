#include <boost/test/unit_test.hpp>
#include "quant/factory/Factory.hpp"
#include "quant/factory/BondBuilder.hpp"
#include "quant/market/MarketCurve.hpp"
#include "quant/instruments/Bond.hpp"

BOOST_AUTO_TEST_CASE(test_bond_valuation) {
    using namespace Quant;
    
    // Curva dinámica
    boost::gregorian::date ref_date(2016, 4, 1);
    auto curva = std::make_shared<Market::MarketCurve>(ref_date);
    
    // Inyectar tasas para las fechas de pago
    curva->add_rate(boost::gregorian::from_string("2017-04-03"), 0.0500);
    curva->add_rate(boost::gregorian::from_string("2018-04-02"), 0.0520);
    
    // Configurar la pata del bono (Fijo, 2 años, frecuencia 1, 100M al 5%)
    Description::LegDescription bond_leg(
        Description::LegDescription::LegType::Fixed, 
        "2016-04-01", 2, 1, 100e6, 0.05, nullptr, "ACT_360"
    );
    
    Description::InstrumentDescription desc(Description::InstrumentDescription::Type::bond, bond_leg, curva);
    
    // Construcción a través de la Factoría abstracta
    auto instrument = Factory::Factory::instance()(desc);
    
    auto* bond = dynamic_cast<Instruments::Bond*>(instrument.get());
    BOOST_REQUIRE(bond != nullptr);
    
    // Valoración
    double pv = bond->price();
    
    // Verificaciones
    BOOST_CHECK(pv > 0.0);
    BOOST_CHECK_GT(pv, 9000000.0);
    BOOST_CHECK_LT(pv, 10000000.0);
    
    BOOST_TEST_MESSAGE("Precio del Bono (PV de cupones): " << std::fixed << std::setprecision(2) << pv << " EUR");
}