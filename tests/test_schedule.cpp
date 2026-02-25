#include <boost/test/unit_test.hpp>
#include "quant/time/Schedule.hpp"
#include "quant/daycount/DayCountFactory.hpp"
#include <iomanip>

using namespace Quant::Time;

BOOST_AUTO_TEST_SUITE(ScheduleSuite)

BOOST_AUTO_TEST_CASE(TestScheduleFullFlow)
{
    std::string start = "2016-04-01";
    int years = 2;
    int freq = 2; 
    auto calc = DayCountFactory::create(DayCountFactory::Convention::ACT_360);

    Schedule schedule(start, years, freq, *calc);

    const auto& dates = schedule.get_dates();
    const auto& fractions = schedule.get_year_fractions();

    // Mensaje de cabecera en el log
    BOOST_TEST_MESSAGE("--- Auditando Schedule: Start " << start << " ---");

    for (size_t i = 0; i < dates.size(); ++i) {
        BOOST_TEST_MESSAGE("Cupon " << (i + 1) 
            << " | Fecha: " << boost::gregorian::to_simple_string(dates[i]) 
            << " | YF: " << std::fixed << std::setprecision(6) << fractions[i]);
        
        // Verificación básica: cada fracción debe ser > 0
        BOOST_CHECK_GT(fractions[i], 0.0);
    }
}

BOOST_AUTO_TEST_SUITE_END()