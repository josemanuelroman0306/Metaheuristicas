````markdown
# Problema de la Mochila Cuadrática – Optimización Metaheurística

Implementación y comparación experimental de diferentes técnicas de optimización metaheurística aplicadas al **Problema de la Mochila Cuadrática (Quadratic Knapsack Problem, QKP)**.

El proyecto fue desarrollado como parte de la asignatura de **Computación Inteligente** en la Universidad de Granada y se centra en el análisis del comportamiento de diferentes estrategias de optimización en términos de calidad de las soluciones y coste computacional.

## Problema

El Problema de la Mochila Cuadrática es un problema de optimización combinatoria en el que se debe seleccionar un subconjunto de elementos respetando una restricción de capacidad.

A diferencia del Problema de la Mochila clásico, el QKP considera no solo el beneficio individual de cada elemento, sino también las interacciones entre pares de elementos seleccionados. Esto incrementa la complejidad del problema y lo convierte en un escenario adecuado para el estudio de técnicas de optimización metaheurística.

## Algoritmos implementados

El proyecto incluye diferentes estrategias de optimización:

### Algoritmos genéticos y meméticos

- **AGG** – Algoritmo Genético Generacional
  - Cruce de 2 puntos
  - Cruce propuesto

- **AGE** – Algoritmo Genético Estacionario
  - Cruce de 2 puntos
  - Cruce propuesto

- **AM-All** – Algoritmo Memético con búsqueda local aplicada a toda la población

- **AM-Rand** – Algoritmo Memético con búsqueda local aplicada a individuos seleccionados aleatoriamente

- **AM-Best** – Algoritmo Memético con búsqueda local aplicada a los mejores individuos

### Otras metaheurísticas

- **BMB** – Búsqueda Multiarranque Básica
- **ILS** – Iterated Local Search
- **ES** – Evolution Strategy
- **ILS-ES** – Estrategia híbrida de Iterated Local Search y Evolution Strategy

## Evaluación experimental

Los algoritmos fueron evaluados utilizando instancias de diferentes tamaños y densidades.

Se analizaron tres tamaños de instancia:

- **100 elementos**
- **200 elementos**
- **300 elementos**

Para cada algoritmo se analizaron diferentes métricas:

- Fitness medio
- Tiempo medio de ejecución
- Número de evaluaciones

Los experimentos permiten comparar el comportamiento de las diferentes estrategias a medida que aumenta el tamaño y la complejidad del problema.

## Resultados

Los experimentos muestran que las diferentes estrategias presentan distintos compromisos entre la calidad de las soluciones obtenidas y el coste computacional.

En general, las variantes **AGG** obtuvieron los valores medios de fitness más elevados en los tamaños de instancia analizados. El cruce propuesto mejoró los resultados del cruce de 2 puntos en AGG, mientras que en AGE se observó el comportamiento contrario.

Entre los algoritmos meméticos, **AM-Best** obtuvo valores medios de fitness superiores a AM-Rand y AM-All en los tamaños de instancia analizados, manteniendo además tiempos de ejecución relativamente bajos.

El aumento del tamaño de las instancias de 100 a 300 elementos produjo un incremento significativo del tiempo de ejecución en la mayoría de los algoritmos, mostrando el impacto de la complejidad del problema sobre el coste computacional.

Los resultados permiten analizar el equilibrio existente entre obtener soluciones de mayor calidad y reducir el tiempo de ejecución.

## Tecnologías

- **C++**
- Algoritmos genéticos
- Algoritmos meméticos
- Búsqueda local
- Iterated Local Search
- Evolution Strategies
- Técnicas de optimización metaheurística

## Estructura del proyecto

.
├── AlgoritmosGeneticosMemeticos.cpp
├── Metaheuristicas.cpp
└── README.md


### `AlgoritmosGeneticosMemeticos.cpp`

Contiene las implementaciones de los algoritmos genéticos y meméticos:

* AGG
* AGE
* AM-All
* AM-Rand
* AM-Best

### `Metaheuristicas.cpp`

Contiene las implementaciones del resto de metaheurísticas:

* BMB
* ILS
* ES
* ILS-ES

## Contexto

Proyecto académico desarrollado en la **Universidad de Granada (UGR)** 
