#define F_CPU 20000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

void delay_ms(uint16_t w){
	while (w-->0) _delay_ms(1);
}




// 1バイト送信 (送信までブロッキング)
void send_byte(uint8_t a)
{
    while (bit_is_clear(UCSR0A, UDRE0)); // 送信可能になるまでまつ
    UDR0 = a;
}

void send_da(uint16_t v){
	PORTC &= ~0x02;
	send_byte(0b00110000 | ((v >> 8) & 0x0f ));
	send_byte(v);

    while (bit_is_clear(UCSR0A, TXC0)); // wait to complete.
    _delay_us(5);
	PORTC |= 0x02;
}



void sign(uint8_t s) {
	PORTC |= 0x0C;
	if (s) {
		PORTC &= ~0x04;
	} else {
		PORTC &= ~0x08;
	}
	delay_ms(50);
	PORTC |= 0x0C;
}


int main(void)
{
	DDRC = 0xFF;
	PORTC = 0x00;

	UBRR0 = 0;
	DDRB |= (1); // XCK0
	UCSR0C = (1<<UMSEL01)|(1<<UMSEL00)|(0<<UCPHA0)|(0<<UCPOL0); // MSPI mode
	UCSR0B = (1<<TXEN0); // send only
	UBRR0 = 1; // baud

	send_da(0 * (4096 / 50)); // set 0mA
	delay_ms(1000);

	uint8_t count = 0;
	for (;;) {
		uint8_t i;
		for (i = 0; i < 20 ; i++) {
			delay_ms(10);
			send_da(i * (4096 / 50)); // 0.1mA * (4096 / 50) be careful!
		}
		//send_da(15 * (4096 / 50)); // 0.1mA * (4096 / 50) be careful!

		delay_ms(500);
		PORTC |= 0x01;

		for (i = 20; i >0 ; i--) {
			delay_ms(10);
			send_da(i * (4096 / 50)); // 0.1mA * (4096 / 50) be careful!
		}
		send_da(0 * (4096 / 50)); // set 0mA before switch.
		delay_ms(10);

		sign(count&1);
		delay_ms(100);

		PORTC &= ~0x01;

		count ++;
	}
	return 0;
}

