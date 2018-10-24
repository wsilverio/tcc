/////////////////////////////////////////////////////////////////
// bibliotecas
/////////////////////////////////////////////////////////////////
// nativas
#include <SmingCore/SPI.h>
// usuário
#include <sensors.h>
#include <matrix.h>
#include <tabmask8.h>
#include <hardware.h>
/////////////////////////////////////////////////////////////////
// definições
/////////////////////////////////////////////////////////////////
// Configurações do SPI
#define SPI_FREQ_HZ 4000000
#define SPI_BYTE_ORDER MSBFIRST
#define SPI_MODE SPI_MODE2

// Recarga do timer de debounce: 10 x 10ms = 100ms.
#define DEBOUNCE_TIMER_RELOAD_VALUE ((uint16_t)(10))

// Cada registrador é responsável por 1 coluna. São 17 colunas (matriz) + 1 seletor de cor.
#define NUM_SHIFT_REGITERS (17+1)

// Quantidade de sensores individuais (power button, leftover).
#define NUM_SINGLE_SENSORS (1+1)

// Os bits de "sensorMask" são o espelho dos sensores.
// 128 sensores / 8 bits = 16 bytes
#define MASK_SIZE (16)

// Valor das entradas inválidas.
#define INVALID_INPUT ((uint8_t)0xFF)
/////////////////////////////////////////////////////////////////
// macros
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// dados personalizados, enumerações
/////////////////////////////////////////////////////////////////

typedef struct tagSENSOR
{
    uint16_t timer;
    bool level;
    bool previousLevel;
} SENSOR;

/////////////////////////////////////////////////////////////////
// variáveis globais
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// variáveis privadas do módulo
/////////////////////////////////////////////////////////////////

/**
 * Máscara dos sensores. Cada bit corresponde a um sensor da mesa.
 */
uint8_t sensorMask[MASK_SIZE];

/**
 * Estruturas de controle da máquina de estados do debounce.
 */
SENSOR sensors[NUM_SENSORS];

/**
 * Entradas mapeadas na máscara dos sensores.
 * Cada byte corresponde a uma entrada (entradas flutuantes, inclusive).
 */
const uint8_t mapped_inputs_in_sensor_mask[8*NUM_SHIFT_REGITERS+NUM_SINGLE_SENSORS] =
{
            // Power button
            0, // input 0

            // Leftover sensor
            1, // input 1

            // Seletor de cor
            2, // input 2
            3, // input 3
            4, // input 4
            5, // input 5
            6, // input 6
            7, // input 7
            8, // input 8
            9, // input 9

            // Coluna A
            10, // input 10
            11, // input 11
            12, // input 12
            13, // input 13
            14, // input 14
            INVALID_INPUT, // input 15
            INVALID_INPUT, // input 16
            INVALID_INPUT, // input 17

            // Coluna B
            15, // input 18
            16, // input 19
            17, // input 20
            18, // input 21
            19, // input 22
            20, // input 23
            INVALID_INPUT, // input 24
            INVALID_INPUT, // input 25

            // Coluna C
            21, // input 26
            22, // input 27
            23, // input 28
            24, // input 29
            25, // input 30
            26, // input 31
            27, // input 32
            INVALID_INPUT, // input 33

            // Coluna D
            28, // input 34
            29, // input 35
            30, // input 36
            31, // input 37
            32, // input 38
            33, // input 39
            34, // input 40
            INVALID_INPUT, // input 41

            // Coluna E
            35, // input 42
            36, // input 43
            37, // input 44
            38, // input 45
            39, // input 46
            40, // input 47
            41, // input 48
            42, // input 49

            // Coluna F
            43, // input 50
            44, // input 51
            45, // input 52
            46, // input 53
            47, // input 54
            48, // input 55
            49, // input 56
            INVALID_INPUT, // input 57

            // Coluna G
            50, // input 58
            51, // input 59
            52, // input 60
            53, // input 61
            54, // input 62
            55, // input 63
            56, // input 64
            57, // input 65

            // Coluna H
            58, // input 66
            59, // input 67
            60, // input 68
            61, // input 69
            62, // input 70
            63, // input 71
            64, // input 72
            INVALID_INPUT, // input 73

            // Coluna I
            65, // input 74
            66, // input 75
            67, // input 76
            68, // input 77
            69, // input 78
            70, // input 79
            71, // input 80
            72, // input 81

            // Coluna J
            73, // input 82
            74, // input 83
            75, // input 84
            76, // input 85
            77, // input 86
            78, // input 87
            79, // input 88
            INVALID_INPUT, // input 89

            // Coluna K
            80, // input 90
            81, // input 91
            82, // input 92
            83, // input 93
            84, // input 94
            85, // input 95
            86, // input 96
            87, // input 97

            // Coluna L
            88, // input 98
            89, // input 99
            90, // input 100
            91, // input 101
            92, // input 102
            93, // input 103
            94, // input 104
            INVALID_INPUT, // input 105

            // Coluna M
            95, // input 106
            96, // input 107
            97, // input 108
            98, // input 109
            99, // input 110
            100, // input 111
            101, // input 112
            102, // input 113

            // Coluna N
            103, // input 114
            104, // input 115
            105, // input 116
            106, // input 117
            107, // input 118
            108, // input 119
            109, // input 120
            INVALID_INPUT, // input 121

            // Coluna O
            110, // input 122
            111, // input 123
            112, // input 124
            113, // input 125
            114, // input 126
            115, // input 127
            116, // input 128
            INVALID_INPUT, // input 129

            // Coluna P
            117, // input 130
            118, // input 131
            119, // input 132
            120, // input 133
            121, // input 134
            122, // input 135
            INVALID_INPUT, // input 136
            INVALID_INPUT, // input 137

            // Coluna Q
            123, // input 138
            124, // input 139
            125, // input 140
            126, // input 141
            127, // input 142
            INVALID_INPUT, // input 143
            INVALID_INPUT, // input 144
            INVALID_INPUT // input 145
};

/////////////////////////////////////////////////////////////////
// macros
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// protótipo das funções privadas
/////////////////////////////////////////////////////////////////
void update_sensor_mask();
void config_bit_in_sensor_mask(uint8_t inputIndex, uint8_t value);
bool get_level_from_sensor_mask(uint8_t sensorId);
void rising_edge_callback(uint8_t sensorId);
void falling_edge_callback(uint8_t sensorId);

/////////////////////////////////////////////////////////////////
// implementação do módulo
/////////////////////////////////////////////////////////////////

/**
 * @brief Inicialização dos sensores reflexivos (barramento SPI e entradas extras).
 * @param None.
 * @return None.
 */
void init_sensors()
{
    // Configuração dos pinos.
    pinMode(GPIO_SPI_MISO, INPUT);
    pinMode(GPIO_SPI_MOSI, OUTPUT);
    pinMode(GPIO_SPI_SCK, OUTPUT);
    pinMode(GPIO_SPI_SS, OUTPUT);
    pinMode(GPIO_POWER_SENSOR, INPUT);
    pinMode(GPIO_LEFTOVER_SENSOR, INPUT);

    // SS em nível alto.
    digitalWrite(GPIO_SPI_SS, HIGH);

    // Inicializa o periférico.
    SPI.begin();

    // Limpa o buffer.
    memset(sensorMask, 0x00, sizeof(sensorMask));

    ////////////////////////////
    // Inicialização da máquina.

    // Atualiza o buffer.
    update_sensor_mask();

    // Configura os parâmetros.
    for (uint8_t id = 0; id < NUM_SENSORS; ++id)
    {
        // Zera o timer.
        sensors[id].timer = DEBOUNCE_TIMER_RELOAD_VALUE;
        // Captura inicializa os estados.
        sensors[id].level = get_level_from_sensor_mask(id);
        sensors[id].previousLevel = !sensors[id].level;
    }
}

/**
 * @brief Atribui o bit referente ao índice do sensor na mascara de sensores.
 * @param inputIndex índice da entrada.
 * @param value valor de teste.
 */
void config_bit_in_sensor_mask(uint8_t inputIndex, uint8_t value)
{
    // Verifica se o índice é válido
    if (inputIndex < sizeof(mapped_inputs_in_sensor_mask))
    {
        // Índice válido.
        /////////////////

        // Remapeia o índice.
        inputIndex = mapped_inputs_in_sensor_mask[inputIndex];

        // Verifica se é uma entrada válida.
        if (inputIndex != INVALID_INPUT)
        {
            // O bit não deve ser ignorado.
            ///////////////////////////////

            // Mapeia o bit no buffer.
            auto maskIndex = (uint8_t)(inputIndex / 8);
            uint8_t bitPos = TABMASK_8[inputIndex % 8];

            assert(maskIndex < sizeof(sensorMask));

            // Testa se o valor é verdadeiro.
            if (value)
            {
                // Valor verdadeiro. Sinaliza o bit.
                ////////////////////////////////////
                sensorMask[maskIndex] |= bitPos;
            }
            else
            {
                // Valor falso. Limpa o bit.
                ////////////////////////////
                sensorMask[maskIndex] &= (uint8_t)(~bitPos);
            }
        }

    }
}

/**
 * @brief Verifica o valor de um determinado sensor na máscara de sensores.
 * @param sensorId índice do sensor.
 * @return valor do sensor (true, se em nível alto; false, se em nível baixo).
 */
bool get_level_from_sensor_mask(uint8_t sensorId)
{
    // Mapeia o índice.
    auto maskIndex = (uint8_t)(sensorId / 8);
    uint8_t bitPos = TABMASK_8[sensorId % 8];

    assert(maskIndex < sizeof(sensorMask));

    return (bool)(sensorMask[maskIndex] & bitPos);
}

/**
 * @brief Atualiza o buffer \a sensorMask.
 * @param None.
 * @return None.
 */
void update_sensor_mask()
{
    // Índice das entradas.
    uint8_t inputIndex = 0;

    /////////////////////////////////////////////////////////////////////////////
    // Leitura dos sensores conectados diretamente aos pinos do microcontrolador.

    // Atribui o valor do primeiro sensor (power button).
    config_bit_in_sensor_mask(inputIndex++, digitalRead(GPIO_POWER_SENSOR));

    // Atribui o valor do segundo sensor (seletor de cor).
    config_bit_in_sensor_mask(inputIndex++, digitalRead(GPIO_LEFTOVER_SENSOR));

    //////////////////////////////////////////////////////////////
    // Faz a leitura sequencial dos registradores de deslocamento.

    // Configura o barramento.
    SPI.beginTransaction(SPISettings(SPI_FREQ_HZ, SPI_BYTE_ORDER, SPI_MODE));

    // Faz os registradores capturarem o estado das portas (borda de descida).
    digitalWrite(GPIO_SPI_SS, LOW);

    // Percorre todos os registradores de deslocamento.
    for (uint8_t bufferIndex = 0; bufferIndex < NUM_SHIFT_REGITERS; ++bufferIndex)
    {
        // Lê o registrador atual.
        uint8_t collumn = SPI.read8();

        // Máscara dentro do registrador atual.
        uint8_t fastMask = 0b00000001;

        // Armazena seus bits na máscara dos sensores.
        for (uint8_t i = 0; i < 8; ++i)
        {
            // Atribui o valor do sensor ao buffer.
            config_bit_in_sensor_mask(inputIndex++, collumn & fastMask);

            // Próximo bit.
            fastMask <<= 1;
        }

    }

    // Libera os registradores.
    digitalWrite(GPIO_SPI_SS, HIGH);

    // Encerra a recepção.
    SPI.endTransaction();
}

/**
 * @brief função de callback para uma borda de subida identificada.
 * @param sensorId índice do sensor que gerou o evento.
 */
void rising_edge_callback(uint8_t sensorId)
{
    if (sensorId < NUM_SENSORS)
    {

    }
}

/**
 * @brief função de callback para uma borda de descida identificada.
 * @param sensorId índice do sensor que gerou o evento.
 */
void falling_edge_callback(uint8_t sensorId)
{
    if (sensorId < NUM_SENSORS)
    {

    }
}

/**
 * @brief rotina da tarefa periódica do módulo dos sensores.
 */
void sensor_task()
{
    // Atualiza o buffer.
    update_sensor_mask();

    // Percorre o buffer inteiro.
    for (uint8_t id = 0; id < NUM_SENSORS; ++id)
    {
        SENSOR &thisSensor = sensors[id];

        // Atualiza o nível atual.
        thisSensor.level = get_level_from_sensor_mask(id);

        // Verifica se houve mudança do nível desde a última execução da tarefa.
        if (thisSensor.level != thisSensor.previousLevel)
        {
            // Houve mudança do nível do sensor.
            ////////////////////////////////////

            // Recarrega o timer.
            thisSensor.timer = DEBOUNCE_TIMER_RELOAD_VALUE;

            // Atualiza o estado anterior.
            thisSensor.previousLevel = thisSensor.level;
        }
        else
        {
            // Não houve mudança do nível do sensor.
            ////////////////////////////////////////

            // Verifica se o timer está rodando.
            if (thisSensor.timer)
            {
                // O timer está rodando.
                ////////////////////////

                // Decrementa o timer e verifica se houve timeout.
                if (--thisSensor.timer == 0)
                {
                    // Houve timeout.
                    /////////////////

                    // Executa a função de callback conforme o nível.
                    if (thisSensor.level)
                    {
                        // Borda de subida.
                        ///////////////////

                        rising_edge_callback(id);
                    }
                    else
                    {
                        // Borda de descida.
                        ////////////////////

                        falling_edge_callback(id);
                    }
                }
            }
        }

    }
}
