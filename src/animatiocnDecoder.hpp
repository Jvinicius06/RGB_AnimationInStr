#include <Arduino.h>

//**************************************************
//                  List Animations
//**************************************************

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

listAnimation_t* startAnimationList = NULL;
listAnimation_t* actAnimation = NULL;
animation_t status;
animation_t act;

listAnimation_t* getLastAniation(bool penult) {
    listAnimation_t* aux = startAnimationList;
    if (aux == NULL) return NULL;
    while (aux->next != NULL) {
        aux = aux->next;
    }
    return aux;
}

void addAniation(animation_t index) {
    listAnimation_t* newAnimation = new listAnimation_t();
    memcpy(&newAnimation->value, &index, sizeof(animation_t));
    if (startAnimationList == NULL) {
        startAnimationList = newAnimation;
        return;
    }
    listAnimation_t* aux = startAnimationList;
    while (aux->next != NULL) {
        aux = aux->next;
    }
    aux->next = newAnimation;
}

void clearAniation() {
    listAnimation_t* aux = startAnimationList;
    if (aux == NULL) return;
    while (startAnimationList != NULL) {
        aux = startAnimationList;
        startAnimationList = startAnimationList->next;
        delete aux;
    }
}

//**************************************************
//               Variabes Globlas
//**************************************************

enum animationRunTypes {
    STOP,
    ANIMATION,
    STATIC,
};

uint8_t animationRun = STOP;
uint8_t staticR = 0;
uint8_t staticG = 0;
uint8_t staticB = 0;

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

uint16_t getValueOfAuxBuffer(bool isActivectRandon = false) {
    uint16_t aux = 0;
    aux = atol(auxBuffer);
    if (isActivectRandon && aux > 0xff) {
        aux = random(255);
    }
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
    // Serial.println(index);
    if (isNumber(index)) {
        // adiciona todos os numeros em um buffer auxiliar para ser processado
        // mais para frente
        auxBuffer[countAuxBuffer++] = index;
    } else {
        // coleta os valores numeros do auxilia buffer
        if (state > GETVALUE) {
            if (state == SETRED) {
                ani->R = getValueOfAuxBuffer(true);
                printSetValue("R", ani->R);
            } else if (state == SETBLUE) {
                ani->B = getValueOfAuxBuffer(true);
                printSetValue("B", ani->B);
            } else if (state == SETGREEN) {
                ani->G = getValueOfAuxBuffer(true);
                printSetValue("G", ani->G);
            } else if (state == SETTIMEFADE) {
                ani->fade = getValueOfAuxBuffer();
                printSetValue("Fade", ani->fade);
            } else if (state == SETDELAY) {
                ani->time = getValueOfAuxBuffer();
                printSetValue("Delay", ani->time);
            }
            state = GETWHOVALUE;  // reseta o state pois o proximo char nao é
                                  // numerico
        }
        countAuxBuffer = 0;
        memset(auxBuffer, 0, MAXAUXBUFFERLEN);
        // processamento dos caracteres
        if (index == ',') {
            return NEXTANIMATION;
        } else if (index == 'R') {
            state = SETRED;
        } else if (index == 'G') {
            state = SETGREEN;
        } else if (index == 'B') {
            state = SETBLUE;
        } else if (index == 'F') {
            state = SETTIMEFADE;
        } else if (index == 'W') {
            state = SETDELAY;
        } else if (index == 'S') {
        } else if (index == 'L') {
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
    animationRun = STOP;
    clearAniation();
    uint16_t i = 0;
    animation_t ani;
    memset(&ani, 0, sizeof(animation_t));
    ani.R = ani.G = ani.B = 0xff;

    if (*(animation + len) != ',') {
        memset(const_cast<char*>(animation + len), ',', 1);
    }
    state = GETWHOVALUE;
    while (i <= len) {
        uint8_t status = processingChar(*(animation + i), &ani);
        if (status == NORMAL) {
            // esse status indica que a operacao ainda esta acontecendo
        } else if (status == NEXTANIMATION) {
            addAniation(ani);
            memset(&ani, 0, sizeof(animation_t));
            ani.R = ani.G = ani.B = 0xff;
            Serial.println("NEXTANIMATION");
            // esse satatus indica que o evevnto de animacao esta completo
        } else {
            // idicador de erro no texto
            // return;
        }
        i++;
    }
    animationRun = ANIMATION;
}

//*********************************
//        LOOP ANIMATION
//*********************************

typedef void handleLeds_t(uint8_t, uint8_t, uint8_t);

handleLeds_t* hanldesLeds = NULL;

void setHandleLed(handleLeds_t* index) { hanldesLeds = index; }

void loopAnimation() {
    if (hanldesLeds == NULL) return;
    if (animationRun != ANIMATION) {
        if (animationRun == STATIC) {
            hanldesLeds(staticR, staticG, staticB);
        }
        return;
    }
    if (startAnimationList == NULL) return;
    if (actAnimation == NULL) {
        actAnimation = startAnimationList;
        act = actAnimation->value;
    }
    // seta RGB caso nao setado
    if (act.R == 0xff) act.R = status.R;
    if (act.G == 0xff) act.G = status.G;
    if (act.B == 0xff) act.B = status.B;

    // coleta os tempos
    if ((act.fade > 0 || act.time > 0) && act.at == 0) {
        act.at = millis();  // seta o tempo de inicio da fade somende se ouver
                            // um fade ou um temer
    }
    uint32_t timeOUT = act.at + act.fade + act.time;

    // processamento em tempo real
    uint32_t actTime = millis();
    // Serial.println(act.fade);
    if (actTime < timeOUT) {
        uint8_t R = act.R;
        uint8_t G = act.G;
        uint8_t B = act.B;

        if (act.fade > 0) {  // somente da fade se ouver
            uint32_t actTimeFade = max((act.at + act.fade), actTime);
            R = map(actTime, act.at, actTimeFade, status.R, act.R);
            G = map(actTime, act.at, actTimeFade, status.G, act.G);
            B = map(actTime, act.at, actTimeFade, status.B, act.B);
        }
        // Serial.println(actTimeFade);
        if (hanldesLeds != NULL) {
            (*hanldesLeds)(R, G, B);
        }
    } else {  // proximo estagio da animacao
        memcpy(
            &status, &act,
            sizeof(animation_t));  // copia o ultimo status para setar o proximo
        actAnimation = actAnimation->next;
        if (actAnimation != NULL) {
            act = actAnimation->value;
        }
    }

    // actAnimation->value
}

/**
 * @brief Set the Blinker Color function
 * esta funcao acender uma cor por determido tempo em millis segundos e dps
 * volta para animacao. No momento dessa funcao o codigo fica travado pos a
 * mesma nao é asyncrona
 *
 * @param R 0 - 255
 * @param G 0 - 255
 * @param B 0 - 255
 * @param timer 0 - 65536 - tempo para essa cor fica acesa
 */
void setBlinkerColor(uint8_t R, uint8_t G, uint8_t B, uint16_t timer) {
    if (hanldesLeds == NULL) return;
    hanldesLeds(R, G, B);
    delay(timer);
}

/**
 * @brief Stop Animation
 *
 */
void stopAnimation() { animationRun = STOP; }

/**
 * @brief Start Animation
 *
 */
void startAnimation() { animationRun = ANIMATION; }

/**
 * @brief Set the Static Color Function
 * essa funcao coloca uma cor fixa no led e para a animacao
 *
 * @param R
 * @param G
 * @param B
 */
void setStaticColor(uint8_t R, uint8_t G, uint8_t B) {
    animationRun = STATIC;
    staticR = R;
    staticG = G;
    staticB = B;
}