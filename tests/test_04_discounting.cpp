#include <boost/test/unit_test.hpp>
#include "quant/instruments/FixedLeg.hpp"
#include "quant/market/MarketCurve.hpp"
#include "quant/daycount/DayCountFactory.hpp"

BOOST_AUTO_TEST_CASE(test_discount_factors) {
    using namespace Quant;
    boost::gregorian::date ref_date(2016, 4, 1);
    
    // 1. Instanciamos la MarketCurve dinámica
    auto curva = std::make_shared<Market::MarketCurve>(ref_date);
    
    // 2. INYECCIÓN MANUAL: Añadimos tasas para las dos fechas del cronograma
    // Si no las añadimos, curva.get_zc() lanzará una excepción
    curva->add_rate(boost::gregorian::from_string("2016-10-03"), 0.0474);
    curva->add_rate(boost::gregorian::from_string("2017-04-03"), 0.0500);
    
    auto calc = DayCountFactory::create(DayCountFactory::Convention::ACT_360);
    
    // 3. Generamos el cronograma de 1 año, semestral (2 cupones)
    Time::Schedule sched("2016-04-01", 1, 2, *calc);
    Instruments::FixedLeg leg(std::move(sched), 100e6, 0.05);
    
    // 4. get_cashflows consultará curva->get_zc() para cada fecha
    auto flujos = leg.get_cashflows(*curva);

    // 5. Verificación de la integridad del descuento
    for (const auto& cf : flujos) {
        // PV debe ser igual a Amount * Factor de Descuento (ZC)
        BOOST_CHECK_CLOSE(cf.pv, cf.amount * cf.discount, 0.000001);
        
        // Con tasas positivas, el factor de descuento debe ser menor a 1.0
        BOOST_CHECK_LT(cf.discount, 1.0); 
    }
}