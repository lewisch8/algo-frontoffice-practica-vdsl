#include <boost/test/unit_test.hpp>
#include "quant/market/Index.hpp"
#include "quant/market/MarketCurve.hpp"

BOOST_AUTO_TEST_CASE(test_forward_rate_estimation) {
    BOOST_TEST_MESSAGE("=== Iniciando test de tipos forward ===");
    using namespace Quant;
     boost::gregorian::date ref_date(2016, 4, 1);
    auto curve = std::make_shared<Market::MarketCurve>(ref_date);

    curve->add_rate(boost::gregorian::date(2016, 10, 3), 0.0474);
    curve->add_rate(boost::gregorian::date(2017, 4, 3), 0.0500);  

    Market::Index euribor("EURIBOR_6M", curve);

    // Periodo de 6 meses (aprox 183 días)
    double yf = 182.0 / 360.0;
    boost::gregorian::date start_date(2016, 10, 3);
    boost::gregorian::date end_date(2017, 4, 3);
    
    BOOST_TEST_MESSAGE("Calculando tipo forward para el periodo:");
    BOOST_TEST_MESSAGE(" - Fecha inicio: " << start_date);
    BOOST_TEST_MESSAGE(" - Fecha fin: " << end_date);
    BOOST_TEST_MESSAGE(" - Fracción de año (ACT/360): " << yf);
    
    double forward_rate = euribor.get_rate(start_date, end_date, yf);
    
    BOOST_TEST_MESSAGE("RESULTADOS:");
    BOOST_TEST_MESSAGE(" - Tipo forward calculado: " << forward_rate * 100 << "%");
    BOOST_TEST_MESSAGE(" - Tipo forward esperado: 5.334%");
    BOOST_TEST_MESSAGE(" - Diferencia: " << (forward_rate - 0.05334) * 100 << "%");
    
    // Verificaciones
    BOOST_CHECK_GT(forward_rate, 0.0);
    BOOST_CHECK_CLOSE(forward_rate, 0.05334, 0.05);
}