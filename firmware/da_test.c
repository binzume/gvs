// D/A test
// 0V -> 2V(2mA) -> 0V -> -2V(-2mA) -> 0V -> ...
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#ifndef XCK_DDR
// atmega88 XCK Port settings.
#define XCK_DDR     DDRD
#define XCK_BIT     4
#endif

// MCP4922 port settings.
#define DA_CS_PORT  PORTB
#define DA_CS_DDR   DDRB
#define DA_CS_BIT   0

// relay control pins.
#define SIGN_CTRL_PORT PORTB
#define SIGN_CTRL_DDR DDRB
#define SIGN_CTRL1_BIT 1
#define SIGN_CTRL2_BIT 2

#define MAX_DA 20 // Current(mA) = MAX_DA * AVR_Vcc(5V) / 5 / 10

void delay_ms(uint16_t w){
    while (w-->0) _delay_ms(1);
}


void send_byte(uint8_t d) {
    // wait to become writable.
    while (bit_is_clear(UCSR0A, UDRE0));
    UDR0 = d;
}

void set_da(uint16_t v) {
    DA_CS_PORT &= ~_BV(DA_CS_BIT);
    send_byte(0b00110000 | ((v >> 8) & 0x0f ));
    send_byte(v);
    // wait to complete.
    while (bit_is_clear(UCSR0A, TXC0));
    _delay_us(5);
    DA_CS_PORT |= _BV(DA_CS_BIT);
}

void set_sign(uint8_t s) {
    SIGN_CTRL_PORT &= ~(_BV(SIGN_CTRL1_BIT) | _BV(SIGN_CTRL1_BIT));
    if (s) {
        SIGN_CTRL_PORT |= _BV(SIGN_CTRL1_BIT);
    } else {
        SIGN_CTRL_PORT |= _BV(SIGN_CTRL2_BIT);
    }
    delay_ms(50);
    SIGN_CTRL_PORT &= ~(_BV(SIGN_CTRL1_BIT) | _BV(SIGN_CTRL1_BIT));
}

// Initialize D/A values.
void init_da() {
    DA_CS_DDR |= _BV(DA_CS_BIT); // MCP4922 CS#
    XCK_DDR |= _BV(XCK_BIT); // XCK
    UBRR0 = 0;
    UCSR0C = (1<<UMSEL01)|(1<<UMSEL00)|(0<<UCPHA0)|(0<<UCPOL0); // MSPI mode
    UCSR0B = (1<<TXEN0); // Send only.
    UBRR0 = 1; // Bbaud rate

    SIGN_CTRL_DDR |= _BV(SIGN_CTRL1_BIT) | _BV(SIGN_CTRL1_BIT);

    delay_ms(1);
    set_da(0);
}

int main(void) {
    DDRC = 0xFF;
    PORTC = 0x00;

    init_da();
    delay_ms(1000);

    uint8_t count, i;
    for (count = 0;;count ++) {
        for (i = 0; i <= MAX_DA; i++) {
            delay_ms(10);
            set_da(i * (4096 / 50)); // 0.1mA * (4096 / 50) be careful!
        }

        delay_ms(500);
        PORTC ^= 0x01; // LED

        for (i = MAX_DA; i >0 ; i--) {
            delay_ms(10);
            set_da(i * (4096 / 50));
        }

        set_da(0 * (4096 / 50)); // set 0mA before switch.
        delay_ms(1);
        set_sign(count&1);

        delay_ms(500);
        PORTC ^= 0x01; // LED
    }
    return 0;
}
