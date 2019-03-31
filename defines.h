#ifndef DEFINES_H
#define DEFINES_H

#define		EDGE_TRIGGERED          0x1
#define		LEVEL_SENSITIVE         0x0
#define		CPU0         			0x01	// bit-mask; bit 0 represents cpu0
#define		ENABLE 				    0x1

#define		KEY0 					0b0001
#define		KEY1 					0b0010
#define		KEY2					0b0100
#define		KEY3					0b1000

#define		RIGHT					1
#define		LEFT					2

#define		USER_MODE				0b10000
#define		FIQ_MODE				0b10001
#define		IRQ_MODE				0b10010
#define		SVC_MODE				0b10011
#define		ABORT_MODE				0b10111
#define		UNDEF_MODE				0b11011
#define		SYS_MODE				0b11111

#define		INT_ENABLE				0b01000000
#define		INT_DISABLE			    0b11000000

#define     EXTENDED_KEYS           0xE0
#define     BREAK                   0xF0
#define     RIGHT_ARROW             0x74
#define     LEFT_ARROW              0x6B
#define     UP_ARROW                0x75
#define     DOWN_ARROW              0x72
#define     SPACE_KEY               0x29
#define     ENTER_KEY               0x5A
#define     ESC                     0x76



#endif //MAIN_C_DEFINES_H
