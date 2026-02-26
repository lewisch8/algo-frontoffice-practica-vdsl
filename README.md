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

# Flujo para subir cambios a la rama
```bash
# para subir todos los cambios a stage
git add .

# para subir solo un fichero a stage
git add {mifichero}

# para hacer el commit
git commit -m "mensaje descriptivo"

# Subir al repo de github
git push origin main
```

# Como traer la info del repo de github
```bash
git pull origin main
```
