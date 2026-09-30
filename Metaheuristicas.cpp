#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
//#include <iterator>
//#include <list>
//#include <iterator>
#include <ostream>
#include <string>
//#include <type_traits>
#include <utility>
#include <vector>
//#include <algorithm>
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

/*
Función para generar la población en algoritmos geneticos
Parámetros importantes población, peso w, tamaño de la mochila y el vector de pesos
*/
void generarSolIni(vector<double>&VS, int w, const int tam_mochila, const vector<double>&wi){
    int peso = 0;
    int contador = 1;
    vector<double> filas;

    //Genero un vector de 0 y 1 aleatorio con una probabilidad reducida 
    //Si el peso de ese vector es menor que el de la mochila lo introduzco como parte de la poblacion
    while(contador > 0){
        for(int i = 0; i < tam_mochila; i++){
            filas.push_back(Random::get<bool>());
            if(filas[i] == 1){
                peso += wi[i];
            }
        }
            
        if(peso <= w){
            VS = filas;
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

//*********************************************************
/*
Función que genera los vecinos en la busqueda local
*/
void generarvecinos(vector<double>&vectorSolucion, vector<pair<int, int>>&vecinos){
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
Adaptada para los algoritmos de la práctica 3, se le pasa el vector solución ya inicializado
Parámetro extra eval_hechas para tener en cuenta estas evaluaciones después
*/
vector<double> BLP3(const vector<double>&pi,const vector<double>&wi, const vector<vector<double>>&pij, int w, vector<double> &VS, int &eval_hechas, int eval_total = 90000){
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

/*
BMB
Búsqueda multiarranque básico
*/
//**********************************************************************************
void BMB(vector<double>&VS, const vector<double>&pi,const vector<double>&wi, const vector<vector<double>>&pij, int w, int max_iter = 20, int max_eval_BL = 4500){
    int cont = 0;
    int aux_BL = 0;
    int eval_hechas = 0;

    int coste_optimo = calcularFitness(pi, wi, pij, w, VS);
    cont++;

    while(cont < max_iter){
        vector<double> vec_aux;
        generarSolIni(vec_aux, w, pi.size(), wi);

        vec_aux = BLP3(pi, wi, pij, w, vec_aux, eval_hechas, 4500);
        int coste_nuevo = calcularFitness(pi, wi, pij, w, vec_aux);

        //Si es mejor que el nuevo
        if(coste_nuevo > coste_optimo){
            VS = vec_aux;
            coste_optimo  = coste_nuevo;
        }
        cont++;
    }

    cout << "Fitness: " << coste_optimo << endl;
    for(int i = 0; i < VS.size(); i++){
        cout << VS[i] << " " ;
    } 
    cout << endl;
}
//**********************************************************************************


vector<double> mutacion(vector<double>&VS, const vector<vector<double>>&pij, int w, const vector<double>&wi){
    int tam_VS = VS.size();
    int t = 20;

    //Rango de posiciones a barajar
    int posIni = Random::get(1, tam_VS - 1);
    int posFin =(posIni + t) % tam_VS;

    //Obtenemos los valores a mutar
    vector<double> posMutar;
    for(int i = 0; i < t; i++){
        posMutar.push_back(VS[(posIni + i) % tam_VS]);
    }

    //Barajamos el subvector
    Random::shuffle(posMutar);

    //Actualizamos en el vector original
    for(int i = 0; i < t; i++){
        VS[(posIni + i) % tam_VS] = posMutar[i];
    }

    int pesoSol = 0;
    //Calculo el peso de cada uno
    for(int j = 0; j < VS.size(); j++){
        if(VS[j] == 1){
            pesoSol += wi[j];
        }
    }
    
    //Mientras sea mayor elimino objetos aleatorios
    while(pesoSol >= w){
        int indice = Random::get(0, (int)VS.size() - 1);
        if(VS[indice] == 1){
            VS[indice] = 0;
            pesoSol -= wi[indice];
        }
    }

    return VS;
}

/*
ILS
Búsqueda Local Reiterada
*/
//**********************************************************************************
void ILS(vector<double>&VS, const vector<double>&pi, const vector<double>&wi, const vector<vector<double>>&pij, int w, int max_iter = 20, int max_eval_BL = 4500){
    int cont = 0;
    int aux_BL = 0;
    int eval_hechas = 0;

    VS = BLP3(pi, wi, pij, w, VS, eval_hechas, 4500);
    cont++;

    vector<double> vec_aux = VS;
    int coste_optimo = calcularFitness(pi, wi, pij, w, vec_aux);

    while(cont < max_iter){
        vec_aux = mutacion(vec_aux, pij, w, wi);

        vec_aux = BLP3(pi, wi, pij, w, vec_aux, eval_hechas, 4500);
        int coste_nuevo = calcularFitness(pi, wi, pij, w, vec_aux);

        //Si es mejor que el nuevo
        if(coste_nuevo > coste_optimo){
            VS = vec_aux;
            coste_optimo  = coste_nuevo;
        }
        cont++;
    }

    cout << "Fitness: " << coste_optimo << endl;
    for(int i = 0; i < VS.size(); i++){
        cout << VS[i] << " " ;
    } 
    cout << endl;
}
//**********************************************************************************

/*
ES
Enfriamiento simulado
*/
//**********************************************************************************
vector<double> ES(vector<double>&VS, const vector<double>&pi, const vector<double>&wi, const vector<vector<double>>&pij, int w, double phi = 0.3, double mu = 0.1, int max_eval = 90000){
    int coste_optimo = calcularFitness(pi, wi, pij, w, VS);
    int coste_mejor = coste_optimo;
    vector<double> vec_optimo = VS;
    vector<double> vec_aux = VS;
    vector<double> vec_mejor = VS;
    vector<pair<int, int>> vecinos_pares;
    int tam = VS.size();

    double t_fin = 0.001;
    double t_ini = (mu * coste_optimo) / (-log(phi));
    double t_aux = t_ini;

    int max_vecinos = 5*VS.size();
    int max_exitos = 0.1*max_vecinos;

    double M = (double)max_eval/max_vecinos;

    //Comprobamos que la temperatura final es menor que la inicial
    if(t_ini <= t_fin){
        t_fin = t_ini * 0.1;
    }

    double beta = (t_ini - t_fin)/ (M*t_ini*t_fin);

    int exitos = 1; //Tiene que ser distinto de cero
    int evals = 0;

    //cout <<t_ini <<endl;
    while(exitos != 0 && evals < max_eval){
        int vecinos = 0;
        exitos = 0;

        //Genero los vecinos
        generarvecinos(vec_aux, vecinos_pares);
        Random::shuffle(vecinos_pares);
        int i = 0;

        while(vecinos < max_vecinos && exitos < max_exitos){

            // Hacemos el cambio
            vec_aux[vecinos_pares[i].first] = 0;
            vec_aux[vecinos_pares[i].second] = 1;

            int coste_aux = calcularFitness(pi, wi, pij, w, vec_aux);
            int coste_optimo_aux = calcularFitness(pi, wi, pij, w, vec_optimo);
            int delta = coste_optimo_aux - coste_aux;
            
            evals++;    //Evaluacion hecha
            vecinos++;  //Hemos generado un vecino

            //La solución nueva es mejor o probabilidad por enfriamiento
            if(delta <= 0 || (Random::get(0.0, 1.0) <= exp(-delta / t_aux))){
                vec_optimo = vec_aux;
                coste_optimo = calcularFitness(pi, wi, pij, w, vec_aux);
                exitos++;   //Nueva solución

                vecinos_pares.clear();
                generarvecinos(vec_optimo, vecinos_pares);
                Random::shuffle(vecinos_pares);
                i = 0;
            }else{
                vec_aux[vecinos_pares[i].first] = 1;
                vec_aux[vecinos_pares[i].second] = 0;
                i+=2;
            }

            if(coste_optimo > coste_mejor){
                coste_mejor = coste_optimo;
                vec_mejor = vec_optimo;
            }
        }
        t_aux = t_aux / (1+beta*t_aux);
        vecinos_pares.clear();

    }
    VS = vec_mejor;
    int pesoSol = 0;
    //Calculo el peso de cada uno
    for(int j = 0; j < VS.size(); j++){
        if(VS[j] == 1){
            pesoSol += wi[j];
        }
    }
    
    //Mientras sea mayor elimino objetos aleatorios
    while(pesoSol >= w){
        int indice = Random::get(0, (int)VS.size() - 1);
        if(VS[indice] == 1){
            VS[indice] = 0;
            pesoSol -= wi[indice];
        }
    }

    return VS;

}
//**********************************************************************************

/*
ILS_ES
Búsqueda Local Reiterada Con ES
*/
//**********************************************************************************
void ILS_ES(vector<double>&VS, const vector<double>&pi, const vector<double>&wi, const vector<vector<double>>&pij, int w, int max_iter = 20, int max_eval_BL = 4500){
    int cont = 0;
    int aux_BL = 0;
    int eval_hechas = 0;

    VS = ES(VS, pi, wi, pij, w, 0.3, 0.1, 4500);
    cont++;

    vector<double> vec_aux = VS;
    int coste_optimo = calcularFitness(pi, wi, pij, w, vec_aux);

    while(cont < max_iter){
        vec_aux = mutacion(vec_aux, pij, w, wi);

        vec_aux = ES(vec_aux, pi, wi, pij, w, 0.3, 0.1, 4500);
        int coste_nuevo = calcularFitness(pi, wi, pij, w, vec_aux);

        //Si es mejor que el nuevo
        if(coste_nuevo > coste_optimo){
            VS = vec_aux;
            coste_optimo  = coste_nuevo;
        }
        cont++;
    }

    cout << "Fitness: " << coste_optimo << endl;
    for(int i = 0; i < VS.size(); i++){
        cout << VS[i] << " " ;
    } 
    cout << endl;
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
    

    vector<double> VS1;
    generarSolIni(VS1, w, tam_mochila, wi);
    tantes = clock();
    cout << "ES" <<endl;
    VS1 = ES(VS1, pi, wi, pij, w);
    tdespues = clock();
    cout << "Fitness: " << calcularFitness(pi, wi, pij, w, VS1) << endl;
    for(int i = 0; i < VS1.size(); i++){
        cout << VS1[i] << " " ;
    } 
    cout << endl;
    cout <<"Tiempo: " << (double)(tdespues - tantes)/1000000 << endl;

    vector<double> VS3;
    generarSolIni(VS3, w, tam_mochila, wi);
    cout << "ILS_ES" <<endl;
    tantes = clock();
    ILS_ES(VS3, pi, wi, pij, w);
    tdespues = clock();
    cout <<"Tiempo: " << (double)(tdespues - tantes)/1000000 << endl;


    vector<double> VS2;
    generarSolIni(VS2, w, tam_mochila, wi);
    cout << "ILS" <<endl;
    tantes = clock();
    ILS(VS2, pi, wi, pij, w);
    tdespues = clock();
    cout <<"Tiempo: " << (double)(tdespues - tantes)/1000000 << endl;
    
    generarSolIni(VS, w, tam_mochila, wi);
    cout << "BMB" <<endl;
    tantes = clock();
    BMB(VS, pi, wi, pij, w);
    tdespues = clock();
    cout <<"Tiempo: " << (double)(tdespues - tantes)/1000000 << endl;
    
    
    //vector<pair<int, int>> vecinos;
    //generarvecinosES(VS1, vecinos, w, wi);

    return 0;

}
//***********************************************************************
//***********************************************************************
