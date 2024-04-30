#include <Arduino.h>

#define ROUNDS 44
#define KEY_WORDS 4

// Function to perform the SIMON round
void simon_round(uint32_t &x, uint32_t &y, uint32_t k) {
    x = (x << 1) | (x >> (32 - 1));
    x ^= y;
    y ^= k;
    y = (y >> 1) | (y << (32 - 1));
}

// Function to expand the key
void simon_expand(uint32_t const K[KEY_WORDS], uint32_t S[ROUNDS]) {
    uint32_t i, b = K[0];
    uint32_t a[KEY_WORDS - 1];

    for (i = 0; i < (KEY_WORDS - 1); i++) {
        a[i] = K[i + 1];
    }

    S[0] = b;
    for (i = 0; i < ROUNDS - 1; i++) {
        simon_round(a[i % (KEY_WORDS - 1)], b, i);
        S[i + 1] = b;
    }
}

// Function to encrypt plaintext using the key schedule
void simon_encrypt(uint32_t plaintext[2], uint32_t ciphertext[2], uint32_t const key_schedule[ROUNDS]) {
    uint32_t i;
    ciphertext[0] = plaintext[0];
    ciphertext[1] = plaintext[1];
    for (i = 0; i < ROUNDS; i++) {
        simon_round(ciphertext[1], ciphertext[0], key_schedule[i]);
    }
}

void setup() {
    Serial.begin(9600);
    Serial.println("Enter any text and press enter:");
}

void loop() {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim(); // Trim any whitespace

        if (input.length() < 8) {
            input += String("        ").substring(0, 8 - input.length()); // Pad with spaces if too short
        }

        uint32_t key[KEY_WORDS] = {0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10};
        uint32_t key_schedule[ROUNDS];
        simon_expand(key, key_schedule);

        uint32_t plaintext[2] = {0, 0};
        memcpy(&plaintext[0], input.c_str(), 4);
        memcpy(&plaintext[1], input.c_str() + 4, 4);

        uint32_t ciphertext[2];
        simon_encrypt(plaintext, ciphertext, key_schedule);

        Serial.print("Original Text: ");
        Serial.println(input);
        Serial.print("Encrypted: ");
        Serial.print(ciphertext[0], HEX);
        Serial.print(" ");
        Serial.println(ciphertext[1], HEX);
        Serial.println();

        Serial.println("[Type a new text in the Serial Monitor]");
    }
}
