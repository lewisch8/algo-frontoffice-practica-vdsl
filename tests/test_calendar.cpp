#include <boost/test/unit_test.hpp>
#include "quant/time/Calendar.hpp"

using Quant::Calendar::Calendar;

BOOST_AUTO_TEST_SUITE(CalendarSuite)

BOOST_AUTO_TEST_CASE(TestFirstBusinessDay_Scenarios)
{
    // Escenario 1: El mes empieza en Domingo (Febrero 2026)
    // 01-Feb-2026 es Domingo -> Esperado: 02-Feb-2026 (Lunes)
    auto feb = Calendar::first_business_day(2026, 2);
    BOOST_TEST_MESSAGE("Escenario 1: Febrero 2026 (empieza en domingo)");
    BOOST_TEST_MESSAGE("  Resultado esperado: 2026-Feb-02 (Lunes)  || Resultado obtenido: " << feb);
    BOOST_CHECK_EQUAL(feb.day(), 2);
    BOOST_CHECK_EQUAL(feb.day_of_week(), boost::date_time::Monday);

    // Escenario 2: El mes empieza en Sábado (Agosto 2026)
    // 01-Aug-2026 es Sábado -> Esperado: 03-Aug-2026 (Lunes)
    auto aug = Calendar::first_business_day(2026, 8);
    BOOST_TEST_MESSAGE("Escenario 1: Agosto 2026 (empieza en sábado)");
    BOOST_TEST_MESSAGE("  Resultado esperado: 2026-Feb-03 (Lunes)  || Resultado obtenido: " << feb);
    BOOST_CHECK_EQUAL(aug.day(), 3);

    // Escenario 3: El mes empieza en día útil (Mayo 2026)
    // 01-May-2026 es Viernes -> Esperado: 01-May-2026
    auto may = Calendar::first_business_day(2026, 5);
    BOOST_TEST_MESSAGE("Escenario 3: Mayo 2026 (empieza en viernes, día hábil)");
    BOOST_TEST_MESSAGE("  Resultado esperado: 2026-May-01 (Viernes)  || Resultado obtenido: " << may);
    BOOST_CHECK_EQUAL(may.day(), 1);
}

BOOST_AUTO_TEST_SUITE_END()