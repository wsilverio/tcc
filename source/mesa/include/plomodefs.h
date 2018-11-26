#ifndef PLOMODEFS_H
#define PLOMODEFS_H

/////////////////////////////////////////////////////////////////
// bibliotecas
/////////////////////////////////////////////////////////////////
// nativas
//#include <stdlib.h>
#include <cstdint>
// usuário
#include <plomotypes.h>
/////////////////////////////////////////////////////////////////
// definições
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// dados personalizados, enumerações
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// variáveis públicas
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// macros
/////////////////////////////////////////////////////////////////

/**
*  \def ABS(x)
*   @brief      Calcula o valor absoluto do argumento \a x.
*   @param      x valor de entrada.
*   @return     valor absoluto de \a x.
*/
#define ABS(x) ( ((x)>0) ? (x) : -(x) )

/**
*  \def MAX(x, y)
*   @brief      Determina o maior valor entre os argumentos \a x e \a y.
*   @param      x primeiro valor.
*   @param      y segundo valor.
*   @return     maior valor entre \a x e \a y.
*/
#define MAX(x, y) ( (x)>(y) ? (x) : (y) )

/**
*  \def MIN(x, y)
*   @brief      Determina o menor valor entre os argumentos \a x e \a y.
*   @param      x primeiro valor.
*   @param      y segundo valor.
*   @return     menor valor entre \a x e \a y.
*/
#define MIN(x, y) ( (x)>(y) ? (y) : (x) )

/**
*  \def GET_ARRAY_LENGTH(arr)
*   @brief      Determina a quantidade de elementos de um array.
*   @param      arr array a ser consultado.
*   @return     quantidade de elementos no array.
*/
#define GET_ARRAY_LENGTH(arr) (sizeof(arr)/sizeof(((arr)[0])))

#ifndef NDEBUG
#if defined(USE_FULL_ASSERT)
/**
*   @brief      Callback da macro \a assert_param, em caso de erro.
*   @note       Deve ser implementada pelo usuário para tratamento do erro.
*/
void assert_failed();
/**
*  \def assert_param(expr)
*   @brief      Testa a expressão é verdadeira. Se for falsa, chama a função de callback.
*   @param      Expressão a ser testada.
*/
#define assert_param(expr) ((expr) ? (void)0 : assert_failed());
#endif // USE_FULL_ASSERT
#endif // NDEBUG

/////////////////////////////////////////////////////////////////
// protótipo das funções
/////////////////////////////////////////////////////////////////

#endif // PLOMODEFS_H
