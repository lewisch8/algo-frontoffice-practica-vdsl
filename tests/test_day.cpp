#define BOOST_TEST_MODULE Tests
#include <boost/test/unit_test.hpp>
#include "quant/daycount/Actual_360.hpp"
#include "quant/daycount/Thirty_360.hpp"

using Quant::Time::Actual_360;
using Quant::Time::Thirty_360;

// Usamos un margen de error para comparar doubles (tolerancia)
const double TOLERANCIA = 0.000001;

BOOST_AUTO_TEST_SUITE(DayCountCalculators)

BOOST_AUTO_TEST_CASE(TestActual360)
{
    Actual_360 calc;
    std::string start = "2023-01-01";
    std::string end = "2023-03-01"; // 59 días reales
    
    double resultado = calc(start, end);
    double esperado = 59.0 / 360.0;
    double diferencia = std::abs(resultado - esperado);
    
    BOOST_TEST_MESSAGE("--- Test Actual/360 ---");
    BOOST_TEST_MESSAGE("Input: " << start << " a " << end);
    BOOST_TEST_MESSAGE("Resultado: " << std::fixed << std::setprecision(8) << resultado);
    BOOST_TEST_MESSAGE("Esperado:  " << esperado);
    BOOST_TEST_MESSAGE("Diferencia: " << diferencia);

    BOOST_CHECK_CLOSE(resultado, esperado, TOLERANCIA);
}

BOOST_AUTO_TEST_CASE(TestThirty360_Standard)
{
    Thirty_360 calc;
    std::string start = "2023-01-01";
    std::string end = "2023-03-01"; // 2 meses de 30 días = 60 días

    double resultado = calc(start, end);
    double esperado = 60.0 / 360.0;
    double diferencia = std::abs(resultado - esperado);

    BOOST_TEST_MESSAGE("--- Test 30/360 ---");
    BOOST_TEST_MESSAGE("Input: " << start << " a " << end);
    BOOST_TEST_MESSAGE("Resultado: " << std::fixed << std::setprecision(8) << resultado);
    BOOST_TEST_MESSAGE("Esperado:  " << esperado);
    BOOST_TEST_MESSAGE("Diferencia: " << diferencia);

    BOOST_CHECK_SMALL(diferencia, TOLERANCIA);
}

BOOST_AUTO_TEST_SUITE_END()