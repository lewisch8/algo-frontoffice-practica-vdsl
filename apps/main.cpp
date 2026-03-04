#include "quant/quant.hpp"

using namespace Quant;

int main() {
    try {
        // Mercado e Índice
        auto curve = make_curve("2016-04-01", {
            {"2016-10-03", 0.0474},
            {"2017-04-03", 0.0500},
            {"2017-10-02", 0.0510},
            {"2018-04-02", 0.0520}
        });
        auto euribor = make_index("EURIBOR_6M", curve, 6, "2016-04-01", 0.048);

        // Definición de patas y swap
        auto fija = make_fixed_leg("2016-04-01", 2, 2, 100e6, 0.05);
        auto variable = make_floating_leg("2016-04-01", 2, 2, 100e6, euribor);
        
        auto swap = make_swap(fija, variable, curve);

        // Reporte automático
        Reports::print_swap_report(*swap, *curve, "SWAP 100M EUR");

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}