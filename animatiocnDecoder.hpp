#include <Arduino.h>

typedef struct animation {
    uint8_t R;      // cor Vermelhar
    uint8_t G;      // cor Verde
    uint8_t B;      // cor Azul
    uint16_t fade;  // tempo do fade
    uint16_t time;
    uint32_t at;
} animation_t;

typedef struct list {
    animation_t value;
    list* next;
} listAnimation_t;

listAnimation_t startAnimation;
listAnimation_t actAnimation;
animation_t status;
animation_t act;

animation_t* addAniation() {}
listAnimation_t* getLastAniation(bool penult) {
    listAnimation_t* aux = &startAnimation;
    while (aux->next != NULL) {
        aux = aux->next;
    }
    return aux;
}

void clearAniation() {}

//**************************************************
//                 Log Animation
//**************************************************

void printSetValue(const char* who, uint16_t value) {
    Serial.print("Value set to ");
    Serial.print(who);
    Serial.print(" -> ");
    Serial.println(value);
}

//**************************************************
//                 Set Animation
//**************************************************

uint8_t state;

enum states {
    INITIAL = 0,
    GETWHOVALUE,
    GETVALUE,
    SETRED,
    SETBLUE,
    SETGREEN,
    SETTIMEFADE,
    SETDELAY,
};

#define MAXAUXBUFFERLEN 10
uint8_t countAuxBuffer = 0;
char auxBuffer[MAXAUXBUFFERLEN];

enum statusProcessing { NORMAL, NEXTANIMATION, ERRROR };

uint16_t getValueOfAuxBuffer() {
    uint16_t aux = 0;
    aux = atol(auxBuffer);
    return aux;
}

bool isNumber(char index) { return isDigit(index); }
/**
 * @brief funcao interna para processadomento de uma animacao
 *
 * @param index caracter a ser processado
 * @param ani a animacao atual para cerregar os parametros
 * @return uint8_t
 */
uint8_t processingChar(char index, animation_t* ani) {
    if (isNumber(index)) {
        // adiciona todos os numeros em um buffer auxiliar para ser processado
        // mais para frente
        auxBuffer[countAuxBuffer++] = index;
    } else {
        // coleta os valores numeros do auxilia buffer
        if (state > GETVALUE) {
            if (state == SETRED) {
                ani->R = getValueOfAuxBuffer();
                printSetValue("R", ani->R);
            } else if (state == SETBLUE) {
                ani->B = getValueOfAuxBuffer();
                printSetValue("B", ani->R);
            } else if (state == SETGREEN) {
                ani->G = getValueOfAuxBuffer();
                printSetValue("G", ani->R);
            } else if (state == SETTIMEFADE) {
                ani->fade = getValueOfAuxBuffer();
                printSetValue("Fade", ani->R);
            } else if (state == SETDELAY) {
                ani->time = getValueOfAuxBuffer();
                printSetValue("Delay", ani->R);
            }
        }
        countAuxBuffer = 0;
        memset(auxBuffer, 0, MAXAUXBUFFERLEN);
        // processamento dos caracteres
        if (',') {
            return NEXTANIMATION;
        } else if ('R') {
            state = SETRED;
        } else if ('G') {
            state = SETGREEN;
        } else if ('B') {
            state = SETBLUE;
        } else if ('F') {
            state = SETTIMEFADE;
        } else if ('W') {
            state = SETDELAY;
        } else if ('S') {
        } else if ('L') {
        } else {
            return ERRROR;
        }
    }
    return NORMAL;
}

/**
 * @brief Set the Animation object
 *
 * @param animation texto de animacao
 * @param len tamanho em caracteres da animcacao
 */
void setAnimation(const char* animation, uint16_t len) {
    uint16_t i = 0;
    animation_t ani;
    memset(&ani, 0, sizeof(animation_t));
    memset(const_cast<char*>(animation + len), ',', 1);
    ani.R = ani.G = ani.B = 0xff;
    state = GETWHOVALUE;
    while (i <= len) {
        uint8_t status = processingChar(*(animation + i), &ani);
        if (status == NORMAL) {
            // esse status indica que a operacao ainda esta acontecendo
        } else if (status == NEXTANIMATION) {
            // esse satatus indica que o evevnto de animacao esta completo
        } else {
            // idicador de erro no texto
            // return false;
        }
        i++;
    }
}

//*********************************
//        LOOP ANIMATION
//*********************************

void loopAnimation() {}