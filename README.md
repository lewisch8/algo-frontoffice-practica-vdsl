# Práctica Final – Bonos, Swaps y Calibración de Curvas

Proyecto desarrollado para la asignatura **Algoritmos de Front-Office**.

## 📌 Objetivo

Implementar en C++:

- Valoración de bonos
- Valoración de swaps de tipos de interés
- Construcción y calibración de curvas (bootstrap)
- Pruebas unitarias de validación

---

## 🐳 Entorno Docker

Para construir la imagen:

```bash
docker build --no-cache -t proyectofinal .
docker run -it --rm --name cont-cpp -v "$PWD":/workspace proyectofinal
```

## 🧪 Tests

Para ejecutar los tests unitarios:

```bash
./unit_tests --log_level=message
```
