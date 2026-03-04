#include <boost/test/unit_test.hpp>
#include "quant/time/Calendar.hpp"

using Quant::Calendar::Calendar;

BOOST_AUTO_TEST_SUITE(CalendarSuite)

BOOST_AUTO_TEST_CASE(TestFirstBusinessDay_Scenarios)
{
    BOOST_TEST_MESSAGE(" =========== Inicio Test de Dia Util ===========");
    // Escenario 1
    // 01-Feb-2026 es Domingo -> Esperado: 02-Feb-2026 (Lunes)
    auto feb = Calendar::adjust_modified_following(2026, 2, 1);
    BOOST_TEST_MESSAGE("Escenario 1: 1 Febrero 2026 (empieza en domingo)");
    BOOST_TEST_MESSAGE("  Resultado esperado: 2026-Feb-02 (Lunes)  || Resultado obtenido: " << feb);
    BOOST_CHECK_EQUAL(feb.day(), 2);
    BOOST_CHECK_EQUAL(feb.day_of_week(), boost::date_time::Monday);

    // Escenario 2
    // 01-Aug-2026 es Sábado -> Esperado: 03-Aug-2026 (Lunes)
    auto aug = Calendar::adjust_modified_following(2026, 8, 1);
    BOOST_TEST_MESSAGE("Escenario 2: 1 Agosto 2026 (empieza en sábado)");
    BOOST_TEST_MESSAGE("  Resultado esperado: 2026-Feb-03 (Lunes)  || Resultado obtenido: " << feb);
    BOOST_CHECK_EQUAL(aug.day(), 3);

    // Escenario 3
    // 01-May-2026 es Viernes -> Esperado: 01-May-2026
    auto may = Calendar::adjust_modified_following(2026, 5, 1);
    BOOST_TEST_MESSAGE("Escenario 3: 1 Mayo 2026 (empieza en viernes, día hábil)");
    BOOST_TEST_MESSAGE("  Resultado esperado: 2026-May-01 (Viernes)  || Resultado obtenido: " << may);
    BOOST_CHECK_EQUAL(may.day(), 1);

    BOOST_TEST_MESSAGE(" =========== Fin del Test de Dia Util ===========");
}

BOOST_AUTO_TEST_SUITE_END()