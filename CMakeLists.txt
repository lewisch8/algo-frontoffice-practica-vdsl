# Requerimientos mínimos y nombre del proyecto
cmake_minimum_required(VERSION 3.10)
project(pj_final LANGUAGES CXX)

# Configuración de C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Opciones de compilación estrictas
add_compile_options(
    -Wall 
    -Wextra 
    -Weffc++ 
    -Werror 
    -pedantic 
    -pedantic-errors 
    -Wconversion 
    -Wsign-conversion
)

# Buscar Boost
find_package(Boost 1.83 REQUIRED CONFIG COMPONENTS system date_time unit_test_framework)

set (SOURCES
    apps/main.cpp
    src/daycount/DayCountFactory.cpp
    src/factory/Swapbuilder.cpp)

# Definir test de Boost por ahora
add_executable(proyecto ${SOURCES})

# Enlace de librerías
target_include_directories(proyecto SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(proyecto PRIVATE include)
target_link_libraries(proyecto PRIVATE 
    Boost::system
    Boost::date_time)

# Mensaje para confirmar que todo va bien durante la configuración
message(STATUS "Boost version: ${Boost_VERSION}")
message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost libraries: ${Boost_LIBRARIES}")

# Pruebas unitarias
set(TEST_SOURCES
    tests/test_day.cpp
    tests/test_calendar.cpp
    tests/test_schedule.cpp
    src/daycount/DayCountFactory.cpp
    tests/test_01_daycount.cpp
    tests/test_02_floating_rates.cpp
    tests/test_03_cashflows.cpp
    tests/test_04_discounting.cpp
    tests/test_05_valuation.cpp
    src/daycount/DayCountFactory.cpp
    src/factory/SwapBuilder.cpp
)

# 2. Creamos el ejecutable de pruebas
add_executable(unit_tests ${TEST_SOURCES})

# 3. Configuramos las carpetas de inclusión para los tests
target_include_directories(unit_tests SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(unit_tests PRIVATE include)

# 4. Enlazamos las librerías necesarias
# Añadimos unit_test_framework para que Boost gestione los tests
target_link_libraries(unit_tests PRIVATE 
    Boost::system
    Boost::date_time
    Boost::unit_test_framework)

# Opcional: Esto permite que al ejecutar "ctest" se lancen las pruebas automáticamente
enable_testing()
add_test(NAME AllTests COMMAND unit_tests)