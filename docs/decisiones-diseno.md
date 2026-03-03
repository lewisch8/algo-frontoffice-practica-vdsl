# 1.3.2 — Decisiones de Diseño

## Introducción

El diseño del sistema sigue una arquitectura en capas inspirada en los principios de **separación de responsabilidades** y **programación orientada a interfaces**. A continuación se detalla cada decisión arquitectónica significativa, el patrón aplicado, la motivación y las alternativas descartadas.

---

## Decisión 1 — Convenciones de día como Strategy

**Patrón:** Strategy  
**Clases implicadas:** `DayCountCalculator`, `Actual_360`, `Thirty_360`  
**Fichero:** `include/quant/daycount/DayCountCalculator.hpp`

`DayCountCalculator` define una interfaz abstracta con `operator()` puro virtual (dos sobrecargas: `string` y `boost::gregorian::date`). Las convenciones concretas (`Actual_360`, `Thirty_360`) la implementan de forma independiente.

**Motivación:**
- Permite añadir nuevas convenciones (`ACT/365`, `ACT/ACT`) sin modificar el código cliente.
- `Schedule` y `MarketCurve` dependen únicamente de la abstracción, nunca de una implementación concreta.
- Cumple el principio Open/Closed: abierto a extensión, cerrado a modificación.

**Alternativa descartada:** Un `enum` con `switch/case` en un único método global. Se descartó porque acopla el código cliente a la lista de convenciones y dificulta la extensión.

---

## Decisión 2 — Curva de descuento como Strategy

**Patrón:** Strategy  
**Clases implicadas:** `ZeroCouponCurve`, `MarketCurve`  
**Fichero:** `include/quant/market/ZeroCouponCurve.hpp`

`ZeroCouponCurve` define la interfaz de curva con `get_zc(date)` y `get_dcf(date, date)` virtuales puros. `MarketCurve` almacena pares `{fecha, tasa continua}` e implementa el cálculo $Z(t) = e^{-r \cdot \alpha}$.

**Motivación:**
- `Instrument`, `Index`, `Leg` y `Bootstrapper` trabajan contra la abstracción: se puede sustituir `MarketCurve` por una curva interpolada (Nelson-Siegel, Svensson) sin tocar ninguna clase de instrumentos.
- Facilita el testing mediante curvas mock.

**Alternativa descartada:** Pasar directamente un `MarketCurve` concreto por referencia en todos los constructores. Se descartó porque crearía dependencias hacia una implementación concreta y haría imposible extender a otros tipos de curva.

---

## Decisión 3 — Jerarquía de instrumentos con interfaz `Instrument`

**Patrón:** Template Method (interfaz abstracta)  
**Clases implicadas:** `Instrument`, `Bond`, `Swap`, `Deposit`  
**Fichero:** `include/quant/instruments/Instrument.hpp`

`Instrument` define el contrato único `price() const = 0` y guarda en `curve_` el puntero compartido a la curva de descuento. Las subclases (`Bond`, `Swap`, `Deposit`) implementan `price()` con su lógica específica.

**Motivación:**
- El código cliente puede trabajar con cualquier instrumento sin conocer el tipo concreto.
- La curva de descuento se inyecta por constructor (**Dependency Injection**), facilitando el testing y la reutilización.

**Alternativa descartada:** Clases independientes sin jerarquía. Se descartó porque impide tratar los instrumentos de forma polimórfica y obliga a duplicar la lógica de descuento.

---

## Decisión 4 — Piernas como jerarquía `Leg`

**Patrón:** Template Method + Polimorfismo  
**Clases implicadas:** `Leg`, `FixedLeg`, `FloatingLeg`  
**Fichero:** `include/quant/instruments/Leg.hpp`

`Leg` es una clase abstracta que declara `price(curve)` y `get_cashflows(curve)` como métodos virtuales puros. `FixedLeg` computa $PV = \sum N \cdot R \cdot \alpha_i \cdot Z(t_i)$; `FloatingLeg` obtiene el tipo forward del `Index` y computa $PV = \sum N \cdot (f_i + s) \cdot \alpha_i \cdot Z(t_i)$.

**Motivación:**
- Permite añadir nuevos tipos de pierna (amortizing, inflation-linked) sin modificar `Bond` ni `Swap`.
- `Swap` compone dos `Leg` sin saber si son fijas o flotantes, lo que simplifica `Swap::price()` a `receiver.price() - payer.price()`.

**Alternativa descartada:** Flags booleanos `is_fixed` dentro de una clase `Leg` monolítica. Se descartó porque aumenta la complejidad ciclomática y viola el principio de responsabilidad única.

---

## Decisión 5 — Factory con Singleton + Self-Registering Builders

**Patrón:** Abstract Factory + Singleton + Self-Registering Factory  
**Clases implicadas:** `Factory`, `FactoryRegistrator<T>`, `BondBuilder`, `SwapBuilder`  
**Ficheros:** `include/quant/factory/Factory.hpp`, `FactoryRegistrator.hpp`, `BondBuilder.hpp`, `SwapBuilder.hpp`

`Factory` es un Singleton (Meyers Singleton) que mantiene un `map<InstrumentDescription::Type, Builder>` donde `Builder = std::function<unique_ptr<Instrument>(const InstrumentDescription&)>`. El cliente construye un instrumento llamando a `Factory::instance()(description)`.

`FactoryRegistrator<T>` es una clase template cuyo constructor registra automáticamente `T::build` en la `Factory`. Cada `BondBuilder.hpp` y `SwapBuilder.hpp` declara un objeto estático `FactoryRegistrator<XBuilder>` en un namespace anónimo: la mera inclusión del header dispara el registro antes de que comience `main`.

**Motivación:**
- El cliente construye instrumentos sin incluir `Bond.hpp` ni `Swap.hpp`: desacoplamiento total.
- El registro es automático y no requiere ningún código de inicialización explícito.
- Añadir un nuevo instrumento (`Option`, `Future`) solo requiere: crear `OptionBuilder.hpp` con su `FactoryRegistrator<OptionBuilder>` estático.

**Alternativa descartada:** Un `if/else` o `switch` en una función `createInstrument()` centralizada. Se descartó porque el registro cerrado de tipos requiere modificar el código existente cada vez que se añade un nuevo instrumento.

---

## Decisión 6 — Separación entre descripción y construcción (DTO)

**Patrón:** Value Object / Data Transfer Object (DTO)  
**Clases implicadas:** `InstrumentDescription`, `LegDescription`, `CashFlow`, `Bootstrapper::DiscountFactor`  
**Ficheros:** `include/quant/description/`

`InstrumentDescription` y `LegDescription` son `struct`s planos que llevan toda la información necesaria para construir un instrumento. Los builders las leen; los instrumentos construidos nunca las conocen.

**Motivación:**
- Separa la declaración (qué quiero construir) de la implementación (cómo se construye).
- Facilita la serialización/deserialización y el testing parametrizado.
- Hace explícitas todas las dependencias de construcción en un único objeto.

**Alternativa descartada:** Pasar parámetros separados al constructor de la `Factory`. Se descartó porque el número de parámetros crece con la complejidad del instrumento y dificulta la extensión.

---

## Decisión 7 — Modelo de propiedad: `unique_ptr` vs `shared_ptr`

**Patrón:** RAII / Ownership semantics  
**Clases implicadas:** `Bond`, `Swap`, `Instrument`, `FloatingLeg`, `Index`

| Relación | Tipo de puntero | Justificación |
|---|---|---|
| `Bond::coupons_` → `Leg` | `unique_ptr` | La pierna es propiedad exclusiva del bono |
| `Swap::payer_leg_` / `receiver_leg_` → `Leg` | `unique_ptr` | Cada pierna pertenece a un único swap |
| `Instrument::curve_` → `ZeroCouponCurve` | `shared_ptr` | La curva puede ser compartida entre varios instrumentos |
| `FloatingLeg::index_` → `Index` | `shared_ptr` | El índice puede compartirse entre varias piernas flotantes |
| `Index::curve_` → `ZeroCouponCurve` | `shared_ptr` | El índice y los instrumentos pueden apuntar a la misma curva |
| `DayCountFactory::create()` devuelve | `unique_ptr` | Transferencia de propiedad al llamador |

**Motivación:**
- El uso de `unique_ptr` en las piernas garantiza que no haya copias accidentales y que la destrucción del instrumento libere automáticamente las piernas.
- El uso de `shared_ptr` en curvas e índices refleja el hecho de que múltiples entidades necesitan acceso de lectura a los mismos datos de mercado.

---

## Decisión 8 — `Bootstrapper` como fachada estática

**Patrón:** Facade  
**Clase implicada:** `Bootstrapper`  
**Fichero:** `include/quant/instruments/Bootstraper.hpp`

`Bootstrapper` expone un único método estático `bootstrapCurve(deposit, vector<Swap>)` que encapsula el algoritmo completo de calibración de la curva de factores de descuento. El cliente no necesita conocer los detalles del algoritmo iterativo.

**Motivación:**
- Oculta la complejidad del bootstrapping (iteración sobre swaps, resolución de $Z_N$ por la ecuación de par-swap) detrás de una API de una sola línea.
- El método estático expresa que `Bootstrapper` no mantiene estado propio: es un algoritmo puro.

**Alternativa descartada:** Exponer el algoritmo como un constructor de `MarketCurve`. Se descartó porque mezclaría responsabilidades: la curva sabe almacenar tasas, pero no debería saber cómo calibrarlas.

---

## Resumen de patrones

| Patrón | Clases principales | Beneficio clave |
|---|---|---|
| Strategy | `DayCountCalculator`, `ZeroCouponCurve` | Intercambiabilidad de algoritmos sin modificar clientes |
| Template Method | `Instrument`, `Leg` | Contrato común + implementaciones especializadas |
| Abstract Factory + Registry | `Factory`, `FactoryRegistrator<T>` | Creación desacoplada de instrumentos |
| Self-Registering Factory | `BondBuilder`, `SwapBuilder` | Registro automático sin código de inicialización |
| Builder | `BondBuilder`, `SwapBuilder` | Construcción paso a paso de objetos complejos |
| Singleton | `Factory` | Registro global único y accesible |
| DTO / Value Object | `InstrumentDescription`, `LegDescription` | Separación descripción / construcción |
| Facade | `Bootstrapper` | API simple sobre algoritmo complejo |
| RAII / Ownership | `unique_ptr`, `shared_ptr` | Gestión automática y explícita del ciclo de vida |
