# 1.3.4 — Observaciones Críticas de Diseño

Este documento recoge los puntos de deuda técnica, inconsistencias y oportunidades de mejora identificadas durante el análisis del diseño del sistema. Complementa la sección de decisiones de diseño y nutre la valoración del grupo (1.3.4).

---

## OC-1 — `dynamic_cast` en `Bootstrapper` (acoplamiento por RTTI)

**Severidad:** Alta  
**Fichero:** `include/quant/instruments/Bootstraper.hpp`

```cpp
// Ejemplo del problema
auto* fixed = dynamic_cast<FixedLeg*>(&swap.get_payer_leg());
```

`Bootstrapper::bootstrapCurve` necesita acceder al annuity (suma de $\alpha_i \cdot Z(t_i)$) de la pierna fija de cada swap. Para ello usa `dynamic_cast<FixedLeg*>` sobre la referencia abstracta `Leg&`, creando un acoplamiento directo a la implementación concreta.

**Problema:**
- Viola el principio de sustitución de Liskov: el código asume que la pierna es necesariamente un `FixedLeg`.
- Si en el futuro se usan piernas fijas de otro tipo (amortizing, escalón), el cast falla en tiempo de ejecución.
- El compilador no puede detectar el error en tiempo de compilación.

**Mejora propuesta:**  
Añadir un método virtual puro `get_annuity(const ZeroCouponCurve&) const` en `Leg`. `FixedLeg` lo implementaría devolviendo $\sum \alpha_i \cdot Z(t_i)$; `FloatingLeg` podría devolver 0 o lanzar una excepción. Alternativamente, exponer `get_annuity()` directamente en `Swap::calculate_par_rate()` sin delegar al `Bootstrapper`.

---

## OC-2 — `DayCountCalculator` y `DayCountFactory` en namespace global

**Severidad:** Media  
**Ficheros:** `include/quant/daycount/DayCountCalculator.hpp`, `include/quant/daycount/DayCountFactory.hpp`

Todas las clases del dominio viven bajo `Quant::*` (e.g., `Quant::Time::Schedule`, `Quant::Market::ZeroCouponCurve`), pero `DayCountCalculator` y `DayCountFactory` se definen en el namespace global. Esto indica una refactorización de namespaces incompleta.

**Problema:**
- Contaminación del namespace global en un proyecto de librería.
- Dificulta encontrar las clases por convención de nombres.
- Inconsistente con el resto del diseño.

**Mejora propuesta:**  
Mover ambas clases a `Quant::Time`. Implica actualizar los `#include` y las declaraciones en todos los ficheros que las usen (`Schedule.hpp`, `MarketCurve.hpp`, `DayCountFactory.cpp`).

---

## OC-3 — Semántica ambigua de `Deposit::price()`

**Severidad:** Media  
**Fichero:** `include/quant/instruments/Deposit.hpp`

`Deposit::price()` devuelve $1 \cdot (1 + r \cdot T)$, es decir, el **valor futuro simple** del depósito, no su valor presente descontado. Esto es correcto para el algoritmo de bootstrapping (donde se usa como punto ancora de la curva), pero es semánticamente inconsistente con el contrato declarado en `Instrument::price()`, que se espera devuelva el valor presente.

**Problema:**
- Un cliente que use `Factory::instance()(desc).price()` sobre un depósito recibirá un valor futuro, no un PV.
- La inconsistencia no está documentada en el código.
- El test unitario del depósito debe verificar explícitamente este comportamiento especial.

**Mejora propuesta (opción A):** Que `Deposit::price()` devuelva el valor presente $\frac{N}{1 + r \cdot T}$ y añadir un método separado `future_value()` para el bootstrapping.  
**Mejora propuesta (opción B):** Documentar claramente con un comentario `// Returns future value for bootstrapping purposes` y excluir `Deposit` de la `Factory` para evitar uso accidental.

---

## OC-4 — `Bootstrapper` usa `dynamic_cast` pero no gestiona el caso `nullptr`

**Severidad:** Media  
**Fichero:** `include/quant/instruments/Bootstraper.hpp`

Relacionado con OC-1: si el `dynamic_cast` falla (devuelve `nullptr`), el código accede a un puntero nulo, causando **comportamiento indefinido** (segfault). No hay comprobación de resultado ni excepción lanzada.

**Mejora propuesta:**  
Añadir una comprobación explícita:
```cpp
auto* fixed = dynamic_cast<FixedLeg*>(&swap.get_payer_leg());
if (!fixed) {
    throw std::invalid_argument("Bootstrapper: payer leg must be a FixedLeg");
}
```

---

## OC-5 — `Schedule` no valida la consistencia fecha/frecuencia

**Severidad:** Baja  
**Fichero:** `include/quant/time/Schedule.hpp`

El constructor de `Schedule` acepta cualquier combinación de `years` y `frequency` sin validar que `frequency > 0`, que `years > 0`, o que la fecha de inicio sea válida. Una frecuencia de 0 causaría una división por cero al calcular el step mensual (`12 / frequency`).

**Mejora propuesta:**  
Añadir precondiciones con `assert` o lanzar `std::invalid_argument` en el constructor:
```cpp
if (frequency <= 0) throw std::invalid_argument("frequency must be > 0");
if (years <= 0)     throw std::invalid_argument("years must be > 0");
```

---

## OC-6 — `Factory` no comprueba si el tipo está registrado antes de construir

**Severidad:** Baja  
**Fichero:** `include/quant/factory/Factory.hpp`

Si se llama a `Factory::instance()(desc)` con un `InstrumentDescription::Type` para el que no existe ningún builder registrado, `buildersMap_.at(desc.type)` lanzará `std::out_of_range`. El mensaje de error de esta excepción estándar no es informativo para el usuario de la librería.

**Mejora propuesta:**  
```cpp
auto it = buildersMap_.find(description.type);
if (it == buildersMap_.end()) {
    throw std::runtime_error("Factory: no builder registered for type "
                             + std::to_string(static_cast<int>(description.type)));
}
return it->second(description);
```

---

## OC-7 — Ausencia de const-correctness en `Index::get_rate()`

**Severidad:** Baja  
**Fichero:** `include/quant/market/Index.hpp`

`Index::get_rate()` está marcado como `virtual const` pero accede a `fixings_` con `find`, que en algunos compiladores puede requerir una sobrecarga no-`const` del mapa si los tipos no son estándar. Se recomienda revisar que todos los accesos a miembros proteged en métodos `const` utilicen la versión `const` del accessor.

---

## Resumen de observaciones

| ID | Descripción | Severidad | Esfuerzo estimado |
|---|---|---|---|
| OC-1 | `dynamic_cast` en `Bootstrapper` (RTTI coupling) | Alta | Media |
| OC-2 | Namespace global para `DayCountCalculator` / `DayCountFactory` | Media | Baja |
| OC-3 | Semántica ambigua de `Deposit::price()` | Media | Baja |
| OC-4 | `dynamic_cast` sin comprobación de `nullptr` | Media | Muy baja |
| OC-5 | `Schedule` sin validación de precondiciones | Baja | Muy baja |
| OC-6 | `Factory` sin mensaje de error descriptivo | Baja | Muy baja |
| OC-7 | `const`-correctness en `Index::get_rate()` | Baja | Muy baja |

---

## Trabajo pendiente de mayor impacto

1. **Refactor OC-1:** Añadir `get_annuity()` a `Leg` para eliminar el `dynamic_cast` en `Bootstrapper`.
2. **Migrar namespace (OC-2):** Mover `DayCountCalculator` y `DayCountFactory` a `Quant::Time`.
3. **Clarificar contrato de `Deposit` (OC-3):** Documentar o separar `price()` de `future_value()`.
