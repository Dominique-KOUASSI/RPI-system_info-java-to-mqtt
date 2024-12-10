#ifndef CABLE_HPP
    #define CABLE_HPP

#include <Arduino.h>

class Cable
{
    private:
        int inputPins[8];
        int outputPins[8];
        uint8_t dataS;
        uint8_t dataR;

        bool verifyCodes(uint8_t send, uint8_t recieve);

    public:
        Cable();
        Cable(int inputPins[], int outputPins[]);

        // codeur
        void envoi(uint8_t data);
        // decodeur
        void reception();

        /// @brief Fonction d'impr...hpp
        void printInputPins();
        void printOutputPins();

        void checkCable();
        void checkCableType();


};

#endif // CABLE_HPP