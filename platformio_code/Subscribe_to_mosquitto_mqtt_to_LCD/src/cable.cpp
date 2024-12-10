#include "cable.hpp"

#define VERBOSE 0

Cable::Cable(){};
Cable::Cable(int inputPins[], int outputPins[]){
    
    this->dataS = 0x00;
    this->dataR = 0x00;

    for(int i = 0;i < 8; i++){
        this->inputPins[i]  = inputPins[i];
        this->outputPins[i] = outputPins[i];
    }

    // Configuration des pins
    for(int i = 0;i < 8; i++){
        //pinMode(this->inputPins[i], INPUT);
        pinMode(this->inputPins[i], INPUT_PULLDOWN);        // Add pull down function.
        pinMode(this->outputPins[i], OUTPUT);
    }
};

void Cable::envoi(uint8_t data){
    this->dataS = data;

    for (int i = 0; i < 8; i++)
    {
        if (this->dataS & (1 << i)){
            if(VERBOSE){
                Serial.println((String)"Le bit "+i+" est a 1 Broch output "+this->outputPins[i]);
            }
            Serial.print(".");  // Show cable test is ongoing 

            digitalWrite(this->outputPins[i], HIGH);
        }else{
            if(VERBOSE){
                Serial.println((String)"Le bit "+i+" est a 0 Broch output "+this->outputPins[i]);
            }
            Serial.print(".");  // Show cable test is ongoing 

            digitalWrite(this->outputPins[i], LOW);
        }
    }

    if(VERBOSE){
        Serial.print("Data envoye : ");
        Serial.println(this->dataS, HEX);
    }

};

void Cable::reception(){
    for (int i = 0; i < 8; i++){
        if(digitalRead(this->inputPins[i]) == HIGH){
            if(VERBOSE){
                Serial.println((String)"Le bit "+i+" est HIGH Broch input "+this->inputPins[i]);
            }
            Serial.print(".");  // Show cable test is ongoing 

            this->dataR |= 1 << i;
        }
        else{
            if(VERBOSE){
                Serial.println((String)"Le bit "+i+" est LOW Broch input "+this->inputPins[i]);
            }
            Serial.print(".");  // Show cable test is ongoing 

            this->dataR &= ~(1 << i);
        }
    }

    if(VERBOSE){
    Serial.print("Data recu : ");
    Serial.println(dataR, HEX);
    }

};

/// @brief Fonction d'impr cpp
void Cable::printInputPins(){
    for(int i = 0;i < 8; i++){
        Serial.print(i);
        Serial.print(" : ");
        Serial.print(this->inputPins[i]);
        Serial.print('\n');
    }
};

void Cable::printOutputPins(){
    for(int i = 0;i < 8; i++){
        Serial.print(i);
        Serial.print(" : ");
        Serial.print(this->outputPins[i]);
        Serial.print('\n');
    }
};

bool Cable::verifyCodes(uint8_t send, uint8_t recieve){
    this->dataS = send;

    this->envoi(this->dataS);
    this->reception();

    if(recieve == this->dataR)
        return true;
  
    return false;
}

void Cable::checkCable(){
    if(this->verifyCodes(0xFF, 0xFF)){
        Serial.println("Le cable est fonctionnel");
    }
    else{
        Serial.println("Le cable n'est pas fonctionnel");
    }
};

/**
 * 
*/
/*
void Cable::checkCableType(){
    bool croise = true;
    uint8_t CodeSend[] = {0x01, 0x02, 0x08, 0x10, 0x04, 0x20, 0x40, 0x80};
    uint8_t CodeReci[] = {0x04, 0x20, 0x08, 0x10, 0x01, 0x02, 0x40, 0x80};

    for(int i = 0; i < sizeof(CodeReci)/sizeof(int) ; i++){
        Serial.println(i);
        if(!this->verifyCodes(CodeSend[i], CodeReci[i])){
            croise = false;
            continue;
        }
    }

    if(croise){
        Serial.println("Le cable est Croise");
    }
    else{
        Serial.println("Le cable est Droit");
    }
};
*/

void Cable::checkCableType(){
    bool crossCable = true;
    bool straightCable = true;
    bool noCable = true;

    /*
    uint8_t CodeSend[] = {0x01, 0x02, 0x08, 0x10, 0x04, 0x20, 0x40, 0x80};
    uint8_t CodeReci[] = {0x04, 0x20, 0x08, 0x10, 0x01, 0x02, 0x40, 0x80};
    */

    uint8_t CodeSend[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
    uint8_t CodeReciNoCable[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t CodeReciStraight[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
    uint8_t CodeReciCross[] = {0x04, 0x20, 0x01, 0x08, 0x10, 0x02, 0x40, 0x80};

    // Inform User
    Serial.println("");
    Serial.print("Test du cable en cours ");

    // Test for no Cable connected
    for(int i = 0; i < sizeof(CodeSend)/sizeof(uint8_t) ; i++){

        if(VERBOSE){Serial.println((String)"Iteration: "+i);}

        if(!this->verifyCodes(CodeSend[i], CodeReciNoCable[i])){
            noCable = false;
            //continue;
            break;
        }
    }

    // Test for a Straight Cable
    for(int i = 0; i < sizeof(CodeSend)/sizeof(uint8_t) ; i++){

        if(VERBOSE){Serial.println((String)"Iteration: "+i);}

        if(!this->verifyCodes(CodeSend[i], CodeReciStraight[i])){
            straightCable = false;
            //continue;
            break;
        }
    }

    // Test for a Cross Cable
    for(int i = 0; i < sizeof(CodeSend)/sizeof(uint8_t) ; i++){

        if(VERBOSE){Serial.println((String)"Iteration: "+i);}

        if(!this->verifyCodes(CodeSend[i], CodeReciCross[i])){
            crossCable = false;
            //continue;
            break;
        }
    }

    Serial.println("");
    if(straightCable){
        Serial.println("Le cable est Droit");
    } else if(crossCable){
        Serial.println("Le cable est Croise");
    } else if(noCable){
        Serial.println("Aucun cable n'est Connecte");
    } else {
        Serial.println("Le cable est Defectueux");
    }
};