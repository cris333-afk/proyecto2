#include "../proyecto/src/estructuras/redservidores/RedServidores.h"

#include <iostream>
#include <string>

// Contador global para resumir al final sin usar librerías externas.
int fallos = 0;

// Imprime [OK] o [FALLO] según la condición evaluada.
void verificar(const std::string& nombrePrueba, bool condicion) {
    if (condicion) {
        std::cout << "[OK] " << nombrePrueba << std::endl;
    } else {
        std::cout << "[FALLO] " << nombrePrueba << std::endl;
        ++fallos;
    }
}

int main() {
    // Caso 1: ruta alternativa más corta que la conexión directa.
    {
        // Se usa una red pequeña para controlar el costo esperado a mano.
        RedServidores red(10);
        int a = red.agregarServidor("A");
        int b = red.agregarServidor("B");
        int c = red.agregarServidor("C");
        int d = red.agregarServidor("D");
        int e = red.agregarServidor("E");
        int f = red.agregarServidor("F");
        verificar("Caso1 ids secuenciales", a == 0 && b == 1 && c == 2 && d == 3 && e == 4 && f == 5);
        red.agregarConexion(a, b, 10);
        red.agregarConexion(b, c, 10);
        red.agregarConexion(a, c, 50);
        red.agregarConexion(c, d, 5);
        red.agregarConexion(d, e, 5);
        red.agregarConexion(e, f, 5);
        // La directa A-C cuesta 50 pero A-B-C cuesta 20, debe elegir la corta.
        verificar("Caso1 ruta alternativa mas corta", red.rutaMasCorta(a, c) == 20);

        // Caso 2: al quitar el tramo óptimo debe usar la siguiente mejor ruta.
        // Se elimina B-C que era parte del camino de costo 20.
        verificar("Caso2 eliminar tramo optimo", red.eliminarConexion(b, c) == true);
        verificar("Caso2 nuevo costo tras eliminar", red.rutaMasCorta(a, c) == 50);

        // Caso 3: rechazos de conexiones inválidas.
        // Se prueban en la misma red porque ya tiene aristas y topología conocida.
        verificar("Caso3 rechaza latencia cero", red.agregarConexion(a, d, 0) == false);
        verificar("Caso3 rechaza latencia negativa", red.agregarConexion(a, d, -5) == false);
        verificar("Caso3 rechaza duplicada", red.agregarConexion(a, b, 10) == false);
        verificar("Caso3 rechaza autoenlace", red.agregarConexion(a, a, 10) == false);
        verificar("Caso3 rechaza id inexistente", red.agregarConexion(a, 99, 10) == false);
        verificar("Caso3 rechaza id negativo", red.agregarConexion(-1, b, 10) == false);
        verificar("Caso3 eliminar inexistente", red.eliminarConexion(a, f) == false);
    }

    // Caso 4: borrado en cabeza y en medio de la lista.
    {
        // Red fresca para conocer el orden exacto de la lista (inserción al inicio).
        RedServidores red(10);
        red.agregarServidor("X");
        red.agregarServidor("Y");
        red.agregarServidor("Z");
        red.agregarServidor("W");
        red.agregarConexion(0, 1, 10);
        red.agregarConexion(0, 2, 20);
        red.agregarConexion(0, 3, 30);
        // La lista de 0 quedó 3 -> 2 -> 1, así 0-3 es la cabeza.
        verificar("Caso4 eliminar cabeza", red.eliminarConexion(0, 3) == true);
        verificar("Caso4 cabeza ya no existe", red.rutaMasCorta(0, 3) == -1);
        verificar("Caso4 resto intacto tras cabeza", red.rutaMasCorta(0, 1) == 10);
        // Ahora la lista de 0 es 2 -> 1, así 0-2 está en medio/cabeza con resto.
        verificar("Caso4 eliminar medio", red.eliminarConexion(0, 2) == true);
        verificar("Caso4 medio ya no existe", red.rutaMasCorta(0, 2) == -1);
        verificar("Caso4 resto intacto tras medio", red.rutaMasCorta(0, 1) == 10);
    }

    // Caso 5: ruta inexistente entre desconectados devuelve -1.
    {
        // Dos servidores sin aristas nunca se alcanzan entre sí.
        RedServidores red(5);
        red.agregarServidor("Solo0");
        red.agregarServidor("Solo1");
        verificar("Caso5 sin ruta devuelve -1", red.rutaMasCorta(0, 1) == -1);
    }

    // Caso 6: ping en red conexa y con un servidor aislado.
    {
        // Tres nodos encadenados forman una red conexa desde el 0.
        RedServidores red(5);
        red.agregarServidor("N0");
        red.agregarServidor("N1");
        red.agregarServidor("N2");
        red.agregarConexion(0, 1, 5);
        red.agregarConexion(1, 2, 5);
        verificar("Caso6 ping conexa true", red.pingGeneral() == true);
        // Al agregar un nodo sin aristas el ping debe fallar por aislado.
        red.agregarServidor("Aislado");
        verificar("Caso6 ping con aislado false", red.pingGeneral() == false);
    }

    // Caso 7: red partida en dos grupos devuelve false.
    {
        // Dos pares conectados entre sí pero sin puente forman dos componentes.
        RedServidores red(6);
        red.agregarServidor("G0");
        red.agregarServidor("G1");
        red.agregarServidor("G2");
        red.agregarServidor("G3");
        red.agregarConexion(0, 1, 7);
        red.agregarConexion(2, 3, 7);
        verificar("Caso7 ping partida false", red.pingGeneral() == false);
    }

    // Caso 8: el destructor libera todo y se valida con AddressSanitizer.
    if (fallos == 0) {
        std::cout << "Todas las pruebas pasaron." << std::endl;
    } else {
        std::cout << "Pruebas con fallos: " << fallos << std::endl;
    }
    return fallos == 0 ? 0 : 1;
}
