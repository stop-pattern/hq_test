#include <Arduino.h>
#include <PCA95x5.h>
#include <array>

PCA9555 ioex;
using PCA95x5::Level::Level;
using PCA95x5::Port::Port;

void bids();
void writeSpeed(uint8_t speed);
const int del = 300;

void setup() {
    pinMode(0, INPUT);
    Serial.begin(115200);
    delay(1000);

    Wire.begin(22, 21);  // sda,scl v1.00はデフォルトと逆
    ioex.attach(Wire);
    ioex.polarity(PCA95x5::Polarity::ORIGINAL_ALL);
    ioex.direction(PCA95x5::Direction::OUT_ALL);
    ioex.write(PCA95x5::Level::H_ALL);
    delay(1000);
}

void loop() {
    for (size_t i = 0; i < 140 + 1; i++) {
        writeSpeed(i);
        Serial.println(i);
        delay(del);
    }
    for (size_t i = 0; i < 1000; i++) {
        if (!digitalRead(0)) bids();
        delay(1);
    }
    for (int i = 140; i > 0; i--) {
        writeSpeed(i);
        Serial.println(i);
        delay(del);
    }
    for (size_t i = 0; i < 1000; i++) {
        if (!digitalRead(0)) bids();
        delay(1);
    }
}

/* --- for bve trainsim through bids serial --- */

void bids() {
    while (!digitalRead(0)) {
        Serial.println("TRIE1");
        auto res = Serial.readStringUntil('\n');
        Serial.println("> " + res);
        auto spd = res.substring(6).toInt();
        Serial.println("set speed: " + String(spd));
        writeSpeed(spd);
        delay(100);
    }
}

/* --- for speedometor --- */

void ioexWrite(uint8_t num, Port digit3, Port digit2, Port digit1,
               Port digit0) {
    std::array<Port, 4> ports = {digit0, digit1, digit2, digit3};
    for (size_t i = 0; i < ports.size(); i++) {
        auto param = (num & (0b0001 << i)) ? Level::L : Level::H;
        ioex.write(ports.at(i), param);
    }
}

void ioexWrite(uint8_t num, Port digit0) {
    auto param = (num & 0b0001) ? Level::L : Level::H;
    ioex.write(digit0, param);
}

void writeSpeed(uint8_t speed) {
    if (speed > 140) return;
    std::array<uint8_t, 3> digit;
    for (auto &&i : digit) {
        i = speed % 10;
        speed /= 10;
    }
    ioexWrite(digit.at(0), Port::P13, Port::P12, Port::P11, Port::P10);
    ioexWrite(digit.at(1), Port::P17, Port::P16, Port::P15, Port::P14);
    ioexWrite(digit.at(2), Port::P00);
}
