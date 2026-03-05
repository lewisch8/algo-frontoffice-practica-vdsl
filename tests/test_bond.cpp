#include <boost/test/unit_test.hpp>
#include "quant/factory/Factory.hpp"
#include "quant/factory/BondBuilder.hpp"
#include "quant/market/MarketCurve.hpp"
#include "quant/instruments/Bond.hpp"

BOOST_AUTO_TEST_CASE(test_bond_valuation) {
    using namespace Quant;
    BOOST_TEST_MESSAGE("=== Iniciando test de valoracion de un bono ===");
    boost::gregorian::date ref_date(2016, 4, 1);
    BOOST_TEST_MESSAGE("Fecha de referencia: " << ref_date);
    auto curva = std::make_shared<Market::MarketCurve>(ref_date);
    
    // Inyectar tasas para las fechas de pago
    curva->add_rate(boost::gregorian::from_string("2017-04-03"), 0.0500);
    curva->add_rate(boost::gregorian::from_string("2018-04-02"), 0.0520);
    
    // Configurar la pata del bono (Fijo, 2 años, frecuencia 1, 100M al 5%)
    Description::LegDescription bond_leg(
        Description::LegDescription::LegType::Fixed, 
        "2016-04-01", 2, 1, 100e6, 0.05, nullptr, "ACT_360");
    
    Description::InstrumentDescription desc(Description::InstrumentDescription::Type::bond, bond_leg, curva);
    
    // Construcción a través de la Factoría abstracta
    auto instrument = Factory::Factory::instance()(desc);
    
    auto* bond = dynamic_cast<Instruments::Bond*>(instrument.get());
    BOOST_REQUIRE(bond != nullptr);
    
    // Valoración
    double pv = bond->price();
    BOOST_TEST_MESSAGE("RESULTADOS DEL CÁLCULO:");
    BOOST_TEST_MESSAGE("  - Precio del bono (PV de cupones + principal): " << pv << " EUR");
    
    // Verificaciones
    BOOST_TEST_MESSAGE("VERIFICACIONES:");
    BOOST_TEST_MESSAGE("  - ¿PV > 0? " << (pv > 0.0 ? "SÍ" : "NO"));
    BOOST_CHECK(pv > 0.0);
    BOOST_TEST_MESSAGE("  - ¿PV > 90.000.000 EUR? " << (pv > 90000000.0 ? "SÍ" : "NO"));
    BOOST_CHECK_GT(pv, 90000000.0); 
    BOOST_TEST_MESSAGE("  - ¿PV < 110.000.000 EUR? " << (pv < 110000000.0 ? "SÍ" : "NO"));
    BOOST_CHECK_LT(pv, 110000000.0); 

    // Valor obtenido en Excel
    double expected_pv = 99372325.8845;
    BOOST_TEST_MESSAGE("  - ¿PV es similar al obtenido en Excel? " << (std::abs(pv - expected_pv) < 0.01 ? "SÍ" : "NO"));
    // Se compara la diferencia con 1 centimo de euro
    BOOST_CHECK_SMALL(std::abs(pv - expected_pv), 0.01);
    
    // Verificación de precisión: con tasas del 5% y cupón del 5%, el precio debe estar cerca de 100M
    BOOST_CHECK_CLOSE(pv, 100000000.0, 10.0); // Margen de tolerancia del 10%
    
    BOOST_TEST_MESSAGE(" =========== Test Bono Finalizado ============");
}