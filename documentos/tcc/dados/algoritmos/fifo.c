/////////////////////////////////////////////////////////////////
// bibliotecas
/////////////////////////////////////////////////////////////////
// nativas

// usuário
#include "fifo.h"
/////////////////////////////////////////////////////////////////
// definições
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// dados personalizados, enumerações
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// variáveis globais
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// variáveis privadas do módulo
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// macros
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// protótipo das privadas funções
/////////////////////////////////////////////////////////////////

/**
 * @brief Calcula a quantidade de dados presentes na fifo.
 * @param fifo Fifo a ser analisada.
 * @return Quantidade de dados presentes na fifo.
 */
uint8_t fifo_data_length(Fifo *fifo)
{
    if (fifo->writeIndex >= fifo->readIndex)
    {
        return (fifo->writeIndex - fifo->readIndex);
    }
    // else
    return (uint8_t)((fifo->size) - fifo->readIndex + fifo->writeIndex);
}

/**
 * @brief Insere um determinado dado na fifo, caso haja espaço.
 * @param fifo Fifo a ser operada.
 * @param data Dado a ser inserido.
 * @return "true", se sucesso. "false" caso erro.
 */
bool fifo_push(Fifo *fifo, uint8_t data)
{
    if (fifo_data_length(fifo) == (fifo->size - 1))
    {
        return false;
    }

    fifo->buffer[fifo->writeIndex] = data;

    if (++fifo->writeIndex >= fifo->size)
    {
        fifo->writeIndex = fifo->size - fifo->writeIndex;
    }

    return true;
}

/**
 * @brief Remove o dado mais antigo da fifo.
 * @param fifo Fifo a ser operada
 * @param data Variável a armazenar o dado retirado.
* @return "true", se sucesso. "false" caso erro.
 */
bool fifo_pull(Fifo *fifo, uint8_t *data)
{
    if (fifo_data_length(fifo) == 0)
    {
        return false;
    }

    *data = fifo->buffer[fifo->readIndex];

    if (++fifo->readIndex >= fifo->size)
    {
        fifo->readIndex = fifo->size - fifo->readIndex;
    }

    return true;
}
