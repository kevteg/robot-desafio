/**
 * @file promedio.h
 * @brief Clase de ayuda para mantener un promedio de las N lecturas de tipo V
 * @author Kevin Hernández, Ángel Gil
 */
#ifndef AUXILIAR_PROMEDIO_H
#define AUXILIAR_PROMEDIO_H
template <typename Var, int N> class promedioDinamico{
	private:
		Var muestra[N];
		Var sumatoria;
		Var posPrimer;
	public:
		/*
		 * @brief Constructor de la clase
		 * @param default_value valor por defecto para inicializar el promedio
		 */
		promedioDinamico(Var default_value){
			sumatoria = 0;
			posPrimer = 0;
			for (int i = 0; i < N; i++) {
				muestra[i] = default_value;
				sumatoria += muestra[i];
			}
		}
		/*
		 * @brief Agregar una nueva muestra.
		 * @param nueva_muestra, nueva muestra a agregar al promedio
		 */
		inline Var add(Var nueva_muestra){
			sumatoria = sumatoria - muestra[posPrimer] + nueva_muestra;
			muestra[posPrimer++] = nueva_muestra;
			if (posPrimer >= N)
				posPrimer = 0;
			return sumatoria / N;
		}

};
#endif