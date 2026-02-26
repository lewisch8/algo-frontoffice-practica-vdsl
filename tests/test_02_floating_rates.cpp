#include <boost/test/unit_test.hpp>
#include "quant/market/Index.hpp"
#include "quant/market/ExerciseCurve.hpp"

BOOST_AUTO_TEST_CASE(test_forward_rate_estimation) {
    // Usamos la curva de ejercicio definida en el proyecto
    auto curve = std::make_shared<Quant::Market::ExerciseCurve>(boost::gregorian::date(2016, 4, 1));
    Quant::Market::Index euribor("EURIBOR_6M", curve);

    // Periodo de 6 meses (aprox 182 días)
    double yf = 182.0 / 360.0;
    double forward_rate = euribor.get_rate(
        boost::gregorian::date(2016, 10, 3), 
        boost::gregorian::date(2017, 4, 3), 
        yf
    );

    // Verificamos que la tasa proyectada sea positiva y coherente con la curva (aprox 5.33%)
    BOOST_CHECK_GT(forward_rate, 0.0);
    BOOST_CHECK_CLOSE(forward_rate, 0.05334, 0.5); 
}