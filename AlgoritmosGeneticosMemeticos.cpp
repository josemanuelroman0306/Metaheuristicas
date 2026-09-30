#include <cstdlib>
#include <iostream>
#include <fstream>
//#include <iterator>
//#include <list>
#include <ostream>
#include <string>
//#include <type_traits>
#include <utility>
#include <vector>
#include <algorithm>
#include "random.hpp"
#include <sstream>      //Se utiliza para crear un flujo de entrada que opera con un string

using namespace std;

using Random = effolkronium::random_static;

//**************************************************
//
//IMPORTANTE
//MANERA DE COMPILAR
// g++ -o QKP -O3 QKP.cpp 
//Con -O3 para que no de error al crear el vector de pares en la búsqueda local
//MANERA DE EJECUTAR
//  ./QKP nombre_archivo tamaño_mochila semilla_numeroAl
//EJEMPLO
// ./QKP data/jeu_100_25_1.txt 100 46
// 
//***************************************************

//***************************************************
/*
Función que calcula el fitness de una solución
*/
int calcularFitness(const vector<double>&pi,const vector<double>&wi, const vector<vector<double>>&pij, int w, vector<double> &VS){
    int mejorvalor = 0;
    int mejorvalorpares = 0;
    //Beneficio individual
    for(int i = 0; i < VS.size(); i++){
        if(VS[i] == 1){
            mejorvalor += pi[i];
        }
    }
    //Beneficio por pares
    for(int i = 0; i < pij.size(); i++){
        if(VS[i] == 1){
            for(int j = i; j < pij.size(); j++){
                if(VS[j] == 1){
                    mejorvalorpares += pij[i][j];
                }
            }
        }
    }
    //Multiplico el valor por pares ya que solo leo de la diagonal principal hacia arriba
    //Por lo tanto falta la otra mitad
    mejorvalorpares *=2;
    mejorvalor += mejorvalorpares;

    return mejorvalor;
}
//*********************************************************

//*********************************************************
/*
Función que genera los vecinos en la busqueda local
*/
void generarvecinos(const vector<double>&vectorSolucion, vector<pair<int, int>>&vecinos){
    //Si encuentra un objeto con valor 1, añade al vector el par con el objeto a 1 y todos los 0 que hay en el vector
    for(int i = 0; i < vectorSolucion.size(); i++){
        if(vectorSolucion[i] == 1){            
            for(int j  = 0; j < vectorSolucion.size(); j++){
                if(vectorSolucion[j] == 0){
                    vecinos.emplace_back(i, j);
                }
            }
        }
    }

}
//********************************************************

//********************************************************
/*
Función que comprueba los vecinos
Intercambia una pareja del vector de vecinos se calcula el fitness de la solucion y si es mejor se mantiene el cambio y se generan vecinos nuevos
*/
void comprobarvecinos(vector<double>&vectorSolucion, vector<pair<int, int>>&vecinos, int &mejorvalor, int &contSol, const vector<double>&pi, const vector<vector<double>>&pij, int &peso, const vector<double>&wi, int eval_total){
    int valornuevo = 0;
    int pesototal = 0;

    //Voy comprobando soluciones, si mejora el valor dejo la solucion, actualizo el valor y el contador
    while(vecinos.size() > 0 && contSol < eval_total){
        //Hago el primer intercambio, el primer valor es el objeto que esta metido y el segundo no
        vectorSolucion[vecinos[0].first] = 0;
        vectorSolucion[vecinos[0].second] = 1;
        contSol +=1;    //Incremento el contador
        
        pesototal = 0;
        for(int i = 0; i < wi.size(); i++){
            if(vectorSolucion[i] == 1){
                pesototal += wi[i];
            }
        }
        
        if(pesototal  < peso){
            
            valornuevo = calcularFitness(pi, wi, pij, peso,vectorSolucion);

            //Si el valor es mejor actualizo mejor mejorvalor, si no vuelvo a cambiar las soluciones
            if(valornuevo > mejorvalor){
                mejorvalor = valornuevo;
                valornuevo = 0;
                //Genero nuevos vecinos en el caso de que sea mejor
                generarvecinos(vectorSolucion, vecinos);
                Random::shuffle(vecinos);
            }
            else{
                //Vuelvo a la solucion original y elimino el movimiento evaluado
                vectorSolucion[vecinos[0].first] = 1;
                vectorSolucion[vecinos[0].second] = 0;  
                vecinos.erase(vecinos.begin()); 
                valornuevo = 0; 
            }
        }else{
            vectorSolucion[vecinos[0].first] = 1;
            vectorSolucion[vecinos[0].second] = 0;  
            vecinos.erase(vecinos.begin()); 
            valornuevo = 0;
        }
    }
}
//****************************************************************

//****************************************************************
/*
Función que realiza la BL
Adaptada para los algoritmos meméticos, se le pasa el vector solución ya inicializado
Parámetro extra eval_hechas para tener en cuenta estas evaluaciones después
*/
vector<double> busquedalocal(const vector<double>&pi,const vector<double>&wi, const vector<vector<double>>&pij, int w, vector<double> &VS, int &eval_hechas, int eval_total = 90000){
    //Inicializar el vector solucion a 0
    int tam = pi.size();
    vector<double> vectorSolucion = VS;   //Inicializo el vector con todo 0
    vector<int> pendientes;
    int indice_aux;
    vector<double> pendiente = wi;    
    vector<pair<int, int>> vecinos;
    int mejorvalor = 0;
    int contSol = 0;
    int peso = w;
    int pesoSolIni = 0;

    for(int i = 0; i < tam; i++){
        pendientes.push_back(i);
    }

    contSol += 1;   //Le sumo 1 por explorar una solucion

    //Genero los vecinos de la primera solucion
    generarvecinos(vectorSolucion, vecinos);

    mejorvalor = calcularFitness(pi, wi, pij, w,vectorSolucion);

    //Mezclo los vecinos
    Random::shuffle(vecinos);

    //Compruebo los vecinos
    comprobarvecinos(vectorSolucion, vecinos, mejorvalor, contSol, pi, pij, peso, wi, eval_total);

    int pesoSol = 0;
    //Calculo el peso de cada uno
    for(int j = 0; j < vectorSolucion.size(); j++){
        if(vectorSolucion[j] == 1){
            pesoSol += wi[j];
        }
    }
    
    //Mientras sea mayor elimino objetos aleatorios
    while(pesoSol >= peso){
        int indice = Random::get(0, (int)vectorSolucion.size() - 1);
        if(vectorSolucion[indice] == 1){
            vectorSolucion[indice] = 0;
            pesoSol -= wi[indice];
        }
    }
    //Copio vectorSolucion en el original
    VS = vectorSolucion;
    eval_hechas+=contSol;

    return VS;
}


//**********************************************************
//**********************************************************
//              ALGORITMOS GENÉTICOS
//**********************************************************
//**********************************************************

//**********************************************************
/*
Función para generar la población en algoritmos geneticos
Parámetros importantes población, peso w, tamaño de la mochila y el vector de pesos
*/
void generarSolIni(vector<vector<double>>&poblacion, int w, const int tam_mochila, const vector<double>&wi){
    int peso = 0;
    int contador = 50;
    vector<double> filas;

    //Genero un vector de 0 y 1 aleatorio con una probabilidad reducida 
    //Si el peso de ese vector es menor que el de la mochila lo introduzco como parte de la poblacion
    while(contador > 0){
        for(int i = 0; i < tam_mochila; i++){
            filas.push_back(Random::get<bool>(0.40));
            if(filas[i] == 1){
                peso += wi[i];
            }
        }
        
        if(peso <= w){
            poblacion.push_back(filas);
            //Resto al contador para que cuando llegue a 50 pare
            contador--;
            filas.clear();
        }
        //Borro filas y peso para que no tengan valores al volver a empezar
        filas.clear();
        peso = 0;
    }

}
//**********************************************************************************

//**********************************************************************************
/*
Función que realiza la selección de los padres
Parámetros importantes matriz de población
*/
vector<double> seleccion(const vector<vector<double>>&poblacion, const vector<vector<double>>&pij, int w, const vector<double>&wi, const vector<double>&pi){
    vector<double> mejor;
    vector<int> aleatorios;
    int mejorvalor = 0;

    //Genero 3 numeros aleatorios y selecciono dichos cromosomas de la población
    aleatorios = Random::get<std::vector>(0, 49, 3);

    vector<double> cand1 = poblacion[aleatorios[0]];
    vector<double> cand2 = poblacion[aleatorios[1]];
    vector<double> cand3 = poblacion[aleatorios[2]];
    
    //Calculo el valor de cada solución
    int valorCand1 = calcularFitness(pi, wi, pij, w, cand1);
    int valorCand2 = calcularFitness(pi, wi, pij, w, cand2);
    int valorCand3 = calcularFitness(pi, wi, pij, w, cand3);

    //Comparo los valores y me quedo con el mejor de ellos
    if(valorCand1 > valorCand2){
        mejorvalor = valorCand1;
        mejor = cand1;
    }else {
        mejorvalor = valorCand2;
        mejor = cand2;
    }

    if(mejorvalor < valorCand3){
        mejorvalor = valorCand3;
        mejor = cand3;
    }

    return mejor;
}
//**********************************************************************************

/*
Función que resta peso si la solución está pasada de peso
Parámetros importantes una matriz con las soluciones, el peso de la mochila y el vector de pesos
*/
//**********************************************************************************
void restarPeso(vector<vector<double>>&matriz, int w, const vector<double>&wi){
    int peso = 0;

    for(int i = 0; i < matriz.size(); i++){
        peso = 0;
        //Calculo el peso de cada uno
        for(int j = 0; j < matriz[0].size(); j++){
            if(matriz[i][j] == 1){
                peso += wi[j];
            }
        }
        //Mientras sea mayor elimino objetos aleatorios
        while(peso > w){
            int indice = Random::get(0, (int)matriz[0].size() - 1);
            if(matriz[i][indice] == 1){
                matriz[i][indice] = 0;
                peso -= wi[indice];
            }
        }
    }
}
//**********************************************************************************

/*
Función que calcula la mejor solución a una población dada
Parámetros importantes poblacion y costeIni
*/
//**********************************************************************************
void calcularMejorSol(vector<vector<double>>&poblacion, vector<double>&costesIni, const vector<double>&pi,const vector<double>&wi, const vector<vector<double>>&pij, int w){
    int mejorValor = 0;
    int posMejor = 0;

    for(int i = 0; i < poblacion.size(); i++){
        costesIni.push_back(calcularFitness(pi, wi, pij, w, poblacion[i]));
    }
    for(int i = 0; i < poblacion.size(); i++){
        int aux = costesIni[i];
        if(aux > mejorValor){
            mejorValor = aux;
            posMejor = i;
        }
    }
    for(int i = 0; i < poblacion[0].size(); i++){
        cout << poblacion[posMejor][i] << " ";
    }
    cout << endl;
    cout << "Fitness: " << calcularFitness(pi, wi, pij, w, poblacion[posMejor]) << endl;

}
//**********************************************************************************

//**********************************************************************************
/*
Función que realiza el cruce en 2 puntos
Los padres ya están elegidos por lo que los cruces son primero con el segundo...
Parámetros importantes padres y probabilidad de cruce
Los demás son el vector de pesos y el peso de la mochila
*/
vector<vector<double>> cruce2Puntos(vector<vector<double>>&padres, double probabilidad, int w, const vector<double>&wi){
    int crucesEsp = probabilidad * (padres.size()/2.0);
    int cruces = 0;
    int posIni;
    int posFin;
    int tamSubcadena;
    int peso = 0;
   
    //Generamos los hijos e intercambio las subcadenas
    vector<vector<double>> hijos = padres;

    //Compruebo que no se pasen de los cruces esperados
    //Los cruces son primero y segundo, tercer y cuarto...
    //No hace falta que sean aleatorios porque ya están seleccionados  
    for(int i = 0; i < padres.size(); i+=2){
        tamSubcadena = Random::get(0, (int)padres[0].size()/2);
        posIni = Random::get(0, (int)padres[0].size());

        //Controlo que no se pase de rango
        while(posIni > padres[0].size()/2.0){
            posIni = Random::get(0, (int)padres[0].size());
        }
        //Posiciń final, inicial más duración
        posFin = posIni + tamSubcadena;

        if(cruces < crucesEsp){
            for(int j = posIni; j < posFin; j++){
                hijos[i][j] = padres[i+1][j];
                hijos[i+1][j] = padres[i][j];
            }
        }
        cruces++;
    }

    restarPeso(hijos, w, wi);

    return hijos;
}
//**********************************************************************************

//**********************************************************************************
/*
Función que realiza un cruce propuesto
Para este cruce dejo los genes que sean iguales en ambos padres en los hijos y los demás los añado aleatorios
Parámetros importantes padres y probabilidad de cruce
Los demás son el vector de pesos y el peso de la mochila
*/
vector<vector<double>> crucePropuesto(vector<vector<double>>&padres, double probabilidad, int w, const vector<double>&wi){
    int crucesEsp = probabilidad * (padres.size()/2.0);
    int cruces = 0;

    vector<vector<double>> hijos = padres;
    for(int i = 0; i < padres.size(); i+=2){
        if(cruces < crucesEsp){
            for(int j = 0; j < padres[0].size(); j++){
                //Si los genes son iguales los mantengo en los hijos
                if(padres[i][j] == padres[i+1][j]){
                    hijos[i][j] = padres[i][j];
                    hijos[i+1][j] = padres[i+1][j];
                }else{
                    //Si son distintos los añado automáticamente
                    hijos[i][j] = Random::get<bool>();
                    hijos[i+1][j] = Random::get<bool>();
                }
            }    
        }
        cruces++;
    }

    restarPeso(hijos, w, wi);

    return hijos;
}
//**********************************************************************************

//**********************************************************************************
/*
función que realiza las mutaciones en la población
Parámetros importantes hijos y probabilidad
Resto de parámetros son el peso w y el vectorde pesos wi
*/
void mutacion(vector<vector<double>>&hijos, double probabilidad, int w, const vector<double>&wi){
    int mutacionesTotal = probabilidad * hijos[0].size()/2;
    int mutaciones = 0;

    while(mutaciones < mutacionesTotal){
        
        //Genero dos números aleatorios y si el objeto esta a 1 lo pongo a 0 y del revés
        int indiceI = Random::get(1, (int)hijos.size() - 1);
        int indiceJ = Random::get(1, (int)hijos[0].size() - 1);

        if(hijos[indiceI][indiceJ] == 1){
            hijos[indiceI][indiceJ] = 0;
            mutaciones++;
        }else{
            hijos[indiceI][indiceJ] = 1;
            mutaciones++;
        }
    }
    //Calculo que no se pase de peso
    restarPeso(hijos, w, wi);
}
//**********************************************************************************

/////////////////////////////////////
/////////////////////////////////////
//      ALGORITMOS GENÉTICOS
/////////////////////////////////////
/////////////////////////////////////

/*
ESQUEMA GENERACIONAL (AGG)
INCLUYE ESQUEMA DE REEMPLAZO
Si modo == 0, se usa cruce2Puntos
Si modo == 1, se usa cucePropuesto
*/
//**********************************************************************************
vector<vector<double>> AGG(vector<vector<double>>&poblacion_ini, int w, int modo, const vector<double>&wi, const vector<double>&pi, const vector<vector<double>>&pij, int max_eval = 90000){
    vector<vector<double>> poblacion = poblacion_ini;
    int evaluaciones = 0;
    double probabilidad_cruce = 0.68;
    double probabilidad_muta = 0.08;
    vector<double> costesIni;

    //Calculo los costes de las soluciones iniciales
    for(int i = 0; i < poblacion_ini.size(); i++){
        costesIni.push_back(calcularFitness(pi, wi, pij, w, poblacion_ini[i]));
        evaluaciones++;
    }

    while(evaluaciones < max_eval){
        //Padres
        vector<vector<double>> padres = poblacion;

        //Seleccionamos los padres
        for(int i = 0; i < poblacion.size(); i++){
            padres[i] = seleccion(poblacion, pij, w, wi, pi);
        }

        //Cruzamos los padres
        //Modo 0, cruce2Puntos
        //Modo 1, crucePropuesto
        vector<vector<double>> hijos = padres;
        if(modo == 0){
            hijos = cruce2Puntos(padres, probabilidad_cruce, w, wi);
        }else{
            hijos = crucePropuesto(padres, probabilidad_cruce, w, wi);
        }

        //Mutamos a los hijos
        mutacion(hijos, probabilidad_muta, w, wi);

        //Esquema de reemplazo (elitismo)
        //Buscamos la mejor solucion antigua
        int mejorValor = 0;
        int posMejor = 0;
        for(int i = 0; i < poblacion.size(); i++){
            int aux = costesIni[i];
            if(aux > mejorValor){
                mejorValor = aux;
                posMejor = i;
            }
        }
        vector<double> mejorSol = poblacion[posMejor];
        int valorMejorSol = calcularFitness(pi, wi, pij, w, mejorSol);
        
        //Buscamos la mejor solucion de los hijos
        vector<double> costeHijos;
        for(int i = 0; i < hijos.size(); i++){
            costeHijos.push_back(calcularFitness(pi, wi, pij, w, hijos[i]));
            evaluaciones++;
        }
        int mejorValorHijos = 0;
        int posMejorhijos = 0;
        for(int i = 0; i < hijos.size(); i++){
            int aux = costeHijos[i];
            if(aux > mejorValorHijos){
                mejorValorHijos = aux;
                posMejorhijos = i;
            }
        }
        vector<double> mejorSolHijos = hijos[posMejorhijos];
        int valorMejorSolHijos = calcularFitness(pi, wi, pij, w, mejorSolHijos);
        //cout << "Valor: " << valorMejorSol << " Valor hijos: " <<valorMejorSolHijos <<endl;
       
        if(valorMejorSol > valorMejorSolHijos){ //Si la solucion antigua es peor
            bool salvaMejor = false;
            //Comprobamos si se salva la mejor solución
            for(int i = 0; i < hijos.size() && !salvaMejor; i++){
                salvaMejor = true;
                for(int j = 0; j < hijos[0].size(); j++){
                    salvaMejor = (mejorSol[j] == hijos[i][j]);
                }
            }
            //No se salva
            if(!salvaMejor){
                //Buscamos la peor solución actual
                vector<double> nuevosCostes;
                for(int i = 0; i < hijos.size(); i++){
                    nuevosCostes.push_back(calcularFitness(pi, wi, pij, w, hijos[i]));
                    evaluaciones++;
                }

                int peorValor = 0;
                int posPeor = 0;
                for(int i = 0; i < hijos.size(); i++){
                    int aux = costesIni[i];
                    posPeor = i;
                    if(aux < peorValor){
                        peorValor = aux;
                        posPeor = i;
                    }
                }

                hijos[posPeor] = mejorSol;
                nuevosCostes[posPeor] = calcularFitness(pi, wi, pij, w, hijos[posPeor]); 

                //Actualizo la población y costes
                poblacion = hijos;
                costesIni = nuevosCostes;
            }else{  //Se salva
                vector<double> nuevosCostes;
                for(int i = 0; i < hijos.size(); i++){
                    nuevosCostes.push_back(calcularFitness(pi, wi, pij, w, hijos[i]));
                    evaluaciones++;
                }

                //Actualizo la población y costes
                poblacion = hijos;
                costesIni = nuevosCostes;
            }
        }else{  //Si la solución nueva es mejor
            vector<double> nuevosCostes;
                for(int i = 0; i < hijos.size(); i++){
                    nuevosCostes.push_back(calcularFitness(pi, wi, pij, w, hijos[i]));
                    evaluaciones++;
                }

                //Actualizo la población y costes
                poblacion = hijos;
                costesIni = nuevosCostes;
        }
    }

    return poblacion;
}
//**********************************************************************************

/*
ESQUEMA ESTACIONARIO (AGE)
INCLUYE ESQUEMA DE REEMPLAZO
Si modo == 0, se usa cruce2Puntos
Si modo == 1, se usa cucePropuesto
*/
//**********************************************************************************
void AGE(vector<vector<double>>&poblacion_ini, int w, int modo, const vector<double>&wi, const vector<double>&pi, const vector<vector<double>>&pij, int max_eval = 90000){
    vector<vector<double>> poblacion = poblacion_ini;
    int evaluaciones = 0;
    double probabilidad_cruce = 1;
    double probabilidad_muta = 0.08;
    vector<double> costesIni;
    int tam = poblacion[0].size();

    //Calculo los costes de las soluciones iniciales
    for(int i = 0; i < poblacion_ini.size(); i++){
        costesIni.push_back(calcularFitness(pi, wi, pij, w, poblacion_ini[i]));
        evaluaciones++;
    }

    while(evaluaciones < max_eval){
        //Padres
        vector<vector<double>> padres(2, vector<double>(tam));
        //Seleccionamos los padres
        for(int i = 0; i < 2; i++){
            padres[i] = seleccion(poblacion, pij, w, wi, pi);
        }

        //Cruzamos los padres
        //Modo 0, cruce2Puntos
        //Modo 1, crucePropuesto
        vector<vector<double>> hijos = padres;
        if(modo == 0){
            hijos = cruce2Puntos(padres, probabilidad_cruce, w, wi);
        }else{
            hijos = crucePropuesto(padres, probabilidad_cruce, w, wi);
        }

        //Mutamos a los hijos
        mutacion(hijos, probabilidad_muta, w, wi);

        //Esquema de reemplazo
        //Competición entre las dos peores de la población actual y las dos nuevas
        vector<double> costeHijos;

        for(int i = 0; i < 2; i++){
            costeHijos.push_back(calcularFitness(pi, wi, pij, w, hijos[i]));
            evaluaciones++;
        }

        //Reemplazamos
        //Encontramos la peor solución de la población actual
        int peorValor = 0;
        int posPeor = 0;
        for(int i = 0; i < poblacion.size(); i++){
            int aux = costesIni[i];
            posPeor = i;
            if(aux < peorValor){
                peorValor = aux;
                posPeor = i;
            }
        }

        int costePeor = calcularFitness(pi, wi, pij, w, poblacion[posPeor]);

        //Compruebo que si el primer hijo es mejor los sustituyo
        //En el caso de que si actualizo el coste también
        if(costeHijos[0] > costePeor){
            poblacion[posPeor] = hijos[0];
            costesIni[posPeor] = costeHijos[0];
        }

        //Encontramos la segunda peor
        int peorValor2 = 0;
        int posPeor2 = 0;
        for(int i = 0; i < poblacion.size(); i++){
            int aux = costesIni[i];
            posPeor2 = i;
            if(aux < peorValor2){
                peorValor2 = aux;
                posPeor2 = i;
            }
        }

        int costePeor2 = calcularFitness(pi, wi, pij, w, poblacion[posPeor2]);
        
        //Compruebo que si el segundo hijo es mejor los sustituyo
        //En el caso de que si actualizo el coste también
        if(costeHijos[1] > costePeor2){
            poblacion[posPeor2] = hijos[1];
            costesIni[posPeor2] = costeHijos[1];
        }
    }

    calcularMejorSol(poblacion, costesIni, pi, wi, pij, w);
    
}
//**********************************************************************************

/////////////////////////////////////
/////////////////////////////////////
//      ALGORITMOS MEMÉTICOS
/////////////////////////////////////
/////////////////////////////////////

/*
AM_ALL
Se aplica a todos los individuos de la población, cada 10 generaciones
*/
//**********************************************************************************
void AM_ALL(vector<vector<double>>&poblacion_ini, int w, const vector<double>&wi, const vector<double>&pi, const vector<vector<double>>&pij, int max_eval = 90000, int max_eval_BL = 650){
    int evaluaciones = 0;
    int eval_BL = 0;
    vector<vector<double>> poblacion = poblacion_ini;
    vector<double> costesIni;
    vector<double> vecAux;
    int numPob = 50;
    int numGen = 10;

    while(evaluaciones < max_eval){
        //Le decimos que haga numPob*numGen, ya que eso es lo que equivale a 10 generaciones
        poblacion = AGG(poblacion, w, 0, wi, pi, pij, numPob*numGen);
        evaluaciones += numPob*numGen;

        for(int i = 0; i < poblacion.size(); i++){
            vecAux = busquedalocal(pi, wi, pij, w, poblacion[i], eval_BL, max_eval_BL);
            poblacion[i] = vecAux;
        }

        for(int i = 0; i < poblacion.size(); i++){
            costesIni.push_back(calcularFitness(pi, wi, pij, w, poblacion[i]));
            evaluaciones++;
        }
        evaluaciones += eval_BL;
        eval_BL = 0;
    }

    calcularMejorSol(poblacion, costesIni, pi, wi, pij, w);

}

/*
AM_Rand
Se aplica búsqueda local a probabilidad_BL*numeroPob, en nuestro caso 5 elementos
Los elementos se escogen aleatoriamente 
*/
//**********************************************************************************
void AM_Rand(vector<vector<double>>&poblacion_ini, int w, const vector<double>&wi, const vector<double>&pi, const vector<vector<double>>&pij, int max_eval = 90000, int max_eval_BL = 650){
    int evaluaciones = 0;
    int eval_BL = 0;
    vector<vector<double>> poblacion = poblacion_ini;
    vector<double> costesIni;
    vector<double> vecAux;
    int numPob = 50;
    int numGen = 10;
    int ejemplos_BL = 0.1*numPob;
    vector<double> indices;

    for(int i = 0; i < numPob; i++){
        indices.push_back(Random::get(0, numPob-1))   ; 
    }

    while(evaluaciones < max_eval){
        //Le decimos que haga numPob*numGen, ya que eso es lo que equivale a 10 generaciones
        poblacion = AGG(poblacion, w, 0, wi, pi, pij, numPob*numGen);
        evaluaciones += numPob*numGen;
        
        for(int i = 0; i < ejemplos_BL; i++){
            vecAux = busquedalocal(pi, wi, pij, w, poblacion[indices[i]], eval_BL, max_eval_BL);
            poblacion[indices[i]] = vecAux;
        }
        Random::shuffle(indices);

        for(int i = 0; i < poblacion.size(); i++){
            costesIni.push_back(calcularFitness(pi, wi, pij, w, poblacion[i]));
            evaluaciones++;
        }
        evaluaciones += eval_BL;
        eval_BL = 0;
    }

    calcularMejorSol(poblacion, costesIni, pi, wi, pij, w);
    
}
//**********************************************************************************

/*
AM_Best
Se aplica búsqueda local a los probabilidad_BL * numeroPob mejores, en nuestro caso los 5 mejores
*/
//**********************************************************************************
void AM_Best(vector<vector<double>>&poblacion_ini, int w, const vector<double>&wi, const vector<double>&pi, const vector<vector<double>>&pij, int max_eval = 90000, int max_eval_BL = 650){
    int evaluaciones = 0;
    int eval_BL = 0;
    vector<vector<double>> poblacion = poblacion_ini;
    vector<double> costesIni;
    vector<double> vecAux;
    int numPob = 50;
    int numGen = 10;
    int ejemplos_BL = 0.1*numPob;

    for(int i = 0; i < poblacion.size(); i++){
        costesIni.push_back(calcularFitness(pi, wi, pij, w, poblacion[i]));
        evaluaciones++;
    }

    while(evaluaciones < max_eval){
        //Le decimos que haga numPob*numGen, ya que eso es lo que equivale a 10 generaciones
        poblacion = AGG(poblacion, w, 0, wi, pi, pij, numPob*numGen);
        evaluaciones += numPob*numGen;
        
        //Ordeno de menor a mayor
        sort(poblacion.begin(), poblacion.end());

        for(int i = poblacion.size() - 1; i > poblacion.size()-ejemplos_BL; i--){
            vecAux = busquedalocal(pi, wi, pij, w, poblacion[i], eval_BL, max_eval_BL);
            poblacion[i] = vecAux;
        }

        for(int i = 0; i < poblacion.size(); i++){
            costesIni.push_back(calcularFitness(pi, wi, pij, w, poblacion[i]));
            evaluaciones++;
        }
        evaluaciones += eval_BL;
        eval_BL = 0;
    }

    calcularMejorSol(poblacion, costesIni, pi, wi, pij, w);
    
}
//**********************************************************************************

//**********************************************************************************
/*
Función main
Leo el archivo y llamo a los algortimos
*/
int main(int argc, char *argv[]){
    //**********************************************************************************************
    //**********************************************************************************************
    //Compruebo que el numero de parmetros es correcto
    if(argc != 4){
        cerr << "Uso: " << argv[0] << " Nombre del archivo: " << "Tamanio de la mochila: " << " Semilla:" << endl;
        return 1;
    }

    int seed = atoi(argv[3]);
    Random::seed(seed);
    cout <<"Using seed: " <<seed <<endl;

    //Abro el archivo
    ifstream archivo(argv[1]);
    if(!archivo){
        cerr << "Error al abir el archivo" << endl;
        return 1;
    }
    //**********************************************************************************************
    //**********************************************************************************************

    //**********************************************************************************************
    //Variables necesarios para inicializar todo
    int num_linea = 0;
    string nombre_archivo;
    int tam_mochila = 0;
    int tam_matriz = atoi(argv[2]);
    vector<double> pi;
    int pijaux[tam_matriz][tam_matriz];     //Creo esta matriz y luego la copio en la buena para pasarsela a la funcion
    int w = 0;
    vector<double> wi;
    vector<vector<double>> pij;
    vector<double> VS;                         //Vector solucion
    string linea;
    clock_t tantes;
    clock_t tdespues;
    int fitness;

    //************************************************************************
    //************************************************************************
    //Leo el archivo
    while(getline(archivo, linea)){
        //La linea 0 corresponde al nombre del archivo
        if(num_linea == 0){
            nombre_archivo = linea;
        }

        //La linea 1 corresponde al tamaño de la mochila
        if(num_linea == 1){
            tam_mochila = stoi(linea);
        }

        if(num_linea == 2){
            //Funcion recomendada por ChatGPT
            istringstream ss(linea);    //Crea un flujo de cadena a  partir de una line(string)
            int num;
            while (ss >> num){              //Extrae los numeros uno por uno del flujo
                pi.push_back(num);
            }    
        }
        
        // A partir de esta linea empieza la matriz
        if(num_linea >= 3 && num_linea < (3 + tam_matriz - 1)){
            //Cada vez que lee una linea se inicializan a num_linea - 3 
            // Asi empiezan las 2 variables iguales y se pone a 0 el valor porque es parte de la diagonal
            int i = num_linea - 3;
            int j= num_linea - 3;
            int num;
            istringstream ss(linea);
            if(i <= tam_matriz){
                //Si los indeces son iguales esta en la diagonal por tanto su valor 0
                if(i == j){
                        pijaux[i][j] = 0;
                }
                while(ss >> num && j <= tam_matriz){
                    //Actualizo la variable j para que escriba el primer elemento bien
                    j++;
                    pijaux[i][j] = num;
                    pijaux[j][i] = num;
                }
            }
            //El ultimo valor nunca llega ya que por ejemplo si es una matriz 5x5 solo hay 4 lineas en el fichero
            //Se inicializa manual
            pijaux[tam_matriz-1][tam_matriz-1] = 0;
        }

        // En el caso de tamanio 100 seria 104 - (100 + 4)
        // Con tamanio de caso 200 y 300 es lo mismo
        if((num_linea - (tam_mochila + 4)) == 0){
            w = stoi(linea);
        }

        //Lo mismo que para el peso total, pero una linea debajo
        if((num_linea - (tam_mochila + 5)) == 0){
            istringstream ss(linea);    //Crea un flujo de cadena a  partir de una line(string)
            int num;
            while (ss >> num){              //Extrae los numeros uno por uno del flujo
                wi.push_back(num);
            }  
        }
        num_linea++;

    }

    //Inicializo pij, matriz buena
    for(int i = 0; i < tam_matriz; i++){
        vector<double> fila;
        for(int j = 0; j < tam_matriz; j++){
            fila.push_back(pijaux[i][j]);
        }
        pij.push_back(fila);
    }
    
    //Cierro el archivo para liberar la memoria
    archivo.close();
    //******************************************************************
    //******************************************************************
    
    //Genero una poblacion de 50 cromosomas
    vector<vector<double>> poblacion;
    generarSolIni(poblacion, w, tam_mochila, wi);
    //Copio la poblacion inicial en otras matrices diferentes para que no modifiquen la original
    vector<vector<double>> poblacionAGG2 = poblacion;
    vector<vector<double>> poblacionAGGP = poblacion;
    vector<vector<double>> poblacionAGE2 = poblacion;
    vector<vector<double>> poblacionAGEP = poblacion;
    vector<vector<double>> poblacionAM_ALL = poblacion;
    vector<vector<double>> poblacionAM_Rand = poblacion;
    vector<vector<double>> poblacionAM_Best = poblacion;
    vector<double> costesIni;

    cout << "AGG 2Puntos" <<endl;
    tantes = clock();
    poblacionAGG2 = AGG(poblacionAGG2, w, 0, wi, pi, pij);
    tdespues = clock();
    cout <<"Tiempo: " << (double)(tdespues - tantes)/1000000 << endl;
    calcularMejorSol(poblacionAGG2, costesIni, pi, wi, pij, w);
   
    cout << "AGG Propuesto" <<endl;
    tantes = clock();
    poblacionAGGP = AGG(poblacionAGGP, w, 1, wi, pi, pij);
    tdespues = clock();
    cout <<"Tiempo: " << (double)(tdespues - tantes)/1000000 << endl;
    calcularMejorSol(poblacionAGGP, costesIni, pi, wi, pij, w);
   

    cout << "AGE 2Puntos" <<endl;
    tantes = clock();
    AGE(poblacionAGE2, w, 0, wi, pi, pij);
    tdespues = clock();
    cout <<"Tiempo: " << (double)(tdespues - tantes)/1000000 << endl;

    cout << "AGE Propuesto" <<endl;
    tantes = clock();
    AGE(poblacionAGEP, w, 1, wi, pi, pij);
    tdespues = clock();
    cout <<"Tiempo: " << (double)(tdespues - tantes)/1000000 << endl;
    
    cout << "AM_ALL" <<endl;
    tantes = clock();
    AM_ALL(poblacionAM_ALL, w, wi, pi, pij);
    tdespues = clock();
    cout <<"Tiempo: " << (double)(tdespues - tantes)/1000000 << endl;

    cout << "AM_Rand" <<endl;
    tantes = clock();
    AM_Rand(poblacionAM_Rand, w, wi, pi, pij);
    tdespues = clock();
    cout <<"Tiempo: " << (double)(tdespues - tantes)/1000000 << endl;

    cout << "AM_Best" <<endl;
    tantes = clock();
    AM_Best(poblacionAM_Best, w, wi, pi, pij);
    tdespues = clock();
    cout <<"Tiempo: " << (double)(tdespues - tantes)/1000000 << endl;
    
    return 0;

    //Para el analisis de los resultados importante decir por qué es mejor en base a los algoritmos
    //Es decir por como están hechos los algoritmos
}
//***********************************************************************
//***********************************************************************