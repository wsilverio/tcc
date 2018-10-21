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

// Cada registrador é responsável por 1 coluna. São 17 colunas (matriz) + 1 seletor de cor.
#define NUM_SHIFT_REGITERS (17+1)

// Quantidade de sensores individuais (power button, leftover).
#define NUM_SINGLE_SENSORS (1+1)

// Os bits de "sensorMask" são o espelho dos sensores.
// 128 sensores / 8 bits = 16 bytes
#define MASK_SIZE (16)
/////////////////////////////////////////////////////////////////
// macros
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
// Máscara dos sensores.
uint8_t sensorMask[MASK_SIZE];

// Mapa das entradas na máscara dos sensores.
// Como há entradas "flutuantes" (colunas com menos de 8 leds), o mapeamento é necessário
// para que a máscara contenha somente as entradas válidas.
const uint8_t map_input_in_sensor_mask[8*NUM_SHIFT_REGITERS+NUM_SINGLE_SENSORS] =
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
            0xFF, // input 15
            0xFF, // input 16
            0xFF, // input 17

            // Coluna B
            15, // input 18
            16, // input 19
            17, // input 20
            18, // input 21
            19, // input 22
            20, // input 23
            0xFF, // input 24
            0xFF, // input 25

            // Coluna C
            21, // input 26
            22, // input 27
            23, // input 28
            24, // input 29
            25, // input 30
            26, // input 31
            27, // input 32
            0xFF, // input 33

            // Coluna D
            28, // input 34
            29, // input 35
            30, // input 36
            31, // input 37
            32, // input 38
            33, // input 39
            34, // input 40
            0xFF, // input 41

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
            0xFF, // input 57

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
            0xFF, // input 73

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
            0xFF, // input 89

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
            0xFF, // input 105

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
            0xFF, // input 121

            // Coluna O
            110, // input 122
            111, // input 123
            112, // input 124
            113, // input 125
            114, // input 126
            115, // input 127
            116, // input 128
            0xFF, // input 129

            // Coluna P
            117, // input 130
            118, // input 131
            119, // input 132
            120, // input 133
            121, // input 134
            122, // input 135
            0xFF, // input 136
            0xFF, // input 137

            // Coluna Q
            123, // input 138
            124, // input 139
            125, // input 140
            126, // input 141
            127, // input 142
            0xFF, // input 143
            0xFF, // input 144
            0xFF, // input 145
        };
/////////////////////////////////////////////////////////////////
// macros
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// protótipo das funções privadas
/////////////////////////////////////////////////////////////////
void update_sensor_mask();
void config_bit_in_sensor_mask();

/////////////////////////////////////////////////////////////////
// implementação do módulo
/////////////////////////////////////////////////////////////////

/**
 * @brief Inicialização dos sensores reflexivos (barramento SPI).
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

    // SS em nível alto.
    digitalWrite(GPIO_SPI_SS, HIGH);

    // Inicializa o periférico.
    SPI.begin();
}

/**
 * @brief Seta o bit referente ao índice do sensor na mascara de sensores, se o valor de teste for verdadeiro.
 * @param mappedSensorIndex índice do sensor.
 * @param value valor de teste.
 */
void config_bit_in_sensor_mask(uint8_t mappedSensorIndex, bool value)
{
    // Verifíca se o índice é válido.
    if (mappedSensorIndex < NUM_SENSORS)
    {
        // Índice válido.
        /////////////////

        // Remapeia o sensor.
        uint8_t sensorMaskIndex = (uint8_t)(mappedSensorIndex / 8);
        uint8_t bitPos = TABMASK_8[mappedSensorIndex % 8];

        // Testa se o valor é verdadeiro.
        if (value)
        {
            // Valor verdadeiro. Sinaliza o bit.
            ////////////////////////////////////
            sensorMask[sensorMaskIndex] |= bitPos;
        }
        else
        {
            // Valor falso. Limpa o bit.
            ////////////////////////////
            sensorMask[sensorMaskIndex] &= (uint8_t)(~bitPos);
        }
    }
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

    // Índices de mapeamento.
    uint8_t sensorMaskIndex;
    uint8_t bitPos;

    // Limpa o buffer.
    memset(sensorMask, 0x00, sizeof(sensorMask));

    /////////////////////////////////////////////////////////////////////////////
    // Leitura dos sensores conectados diretamente aos pinos do microcontrolador.

    // Lê o primeiro sensor (power button).
    map_index_and_bit_position(inputIndex++, &sensorMaskIndex, &bitPos);

    sensorMask[0] = (uint8_t)(digitalRead(GPIO_POWER_SENSOR)?(TABMASK_8[0]):(0));
    inputIndex++;

    // Lê o segundo sensor (seletor de cor).
    sensorMask[0] |= (uint8_t)(digitalRead(GPIO_LEFTOVER_SENSOR)?(TABMASK_8[0]):(0));
    inputIndex++;

    //////////////////////////////////////////////////////////////
    // Faz a leitura sequencial dos registradores de deslocamento.

    // Configura o barramento.
    SPI.beginTransaction(SPISettings(SPI_FREQ_HZ, SPI_BYTE_ORDER, SPI_MODE));

    // Faz os registradores capturarem o estado das portas.
    digitalWrite(GPIO_SPI_SS, LOW);

    uint8_t collumns[1+NUM_SHIFT_REGITERS];
    for (uint8_t bufferIndex = 0; bufferIndex < sizeof(collumns); ++bufferIndex)
    {
        collumns[bufferIndex] = SPI.read8();
    }

    // Encerra a recepção.
    SPI.endTransaction();
}
