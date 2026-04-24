#include <stdint.h>

asm(".set regnum_x0  ,  0");
asm(".set regnum_x1  ,  1");
asm(".set regnum_x2  ,  2");
asm(".set regnum_x3  ,  3");
asm(".set regnum_x4  ,  4");
asm(".set regnum_x5  ,  5");
asm(".set regnum_x6  ,  6");
asm(".set regnum_x7  ,  7");
asm(".set regnum_x8  ,  8");
asm(".set regnum_x9  ,  9");
asm(".set regnum_x10 , 10");
asm(".set regnum_x11 , 11");
asm(".set regnum_x12 , 12");
asm(".set regnum_x13 , 13");
asm(".set regnum_x14 , 14");
asm(".set regnum_x15 , 15");
asm(".set regnum_x16 , 16");
asm(".set regnum_x17 , 17");
asm(".set regnum_x18 , 18");
asm(".set regnum_x19 , 19");
asm(".set regnum_x20 , 20");
asm(".set regnum_x21 , 21");
asm(".set regnum_x22 , 22");
asm(".set regnum_x23 , 23");
asm(".set regnum_x24 , 24");
asm(".set regnum_x25 , 25");
asm(".set regnum_x26 , 26");
asm(".set regnum_x27 , 27");
asm(".set regnum_x28 , 28");
asm(".set regnum_x29 , 29");
asm(".set regnum_x30 , 30");
asm(".set regnum_x31 , 31");

asm(".set regnum_zero,  0");
asm(".set regnum_ra  ,  1");
asm(".set regnum_sp  ,  2");
asm(".set regnum_gp  ,  3");
asm(".set regnum_tp  ,  4");
asm(".set regnum_t0  ,  5");
asm(".set regnum_t1  ,  6");
asm(".set regnum_t2  ,  7");
asm(".set regnum_s0  ,  8");
asm(".set regnum_s1  ,  9");
asm(".set regnum_a0  , 10");
asm(".set regnum_a1  , 11");
asm(".set regnum_a2  , 12");
asm(".set regnum_a3  , 13");
asm(".set regnum_a4  , 14");
asm(".set regnum_a5  , 15");
asm(".set regnum_a6  , 16");
asm(".set regnum_a7  , 17");
asm(".set regnum_s2  , 18");
asm(".set regnum_s3  , 19");
asm(".set regnum_s4  , 20");
asm(".set regnum_s5  , 21");
asm(".set regnum_s6  , 22");
asm(".set regnum_s7  , 23");
asm(".set regnum_s8  , 24");
asm(".set regnum_s9  , 25");
asm(".set regnum_s10 , 26");
asm(".set regnum_s11 , 27");
asm(".set regnum_t3  , 28");
asm(".set regnum_t4  , 29");
asm(".set regnum_t5  , 30");
asm(".set regnum_t6  , 31");
asm(".set CUSTOM0  , 0x0B");
asm(".set CUSTOM1  , 0x2B");

#define opcode_R(opcode, func3, func7, rs1, rs2)   \
({                                             \
    register unsigned long __v;                \
    asm volatile(                              \
     ".word ((" #opcode ") | (regnum_%0 << 7) | (regnum_%1 << 15) | (regnum_%2 << 20) | ((" #func3 ") << 12) | ((" #func7 ") << 25));"   \
     : [rd] "=r" (__v)                          \
     : "r" (rs1), "r" (rs2)        \
    );                                         \
    __v;                                       \
})

#define bfloat16mul(rs1, rs2) opcode_R(CUSTOM1, 0x00, 0x00, rs1, rs2)
#define bfloat16div(rs1, rs2) opcode_R(CUSTOM1, 0x01, 0x00, rs1, rs2)
#define bfloat16add(rs1, rs2) opcode_R(CUSTOM1, 0x02, 0x00, rs1, rs2)
#define bfloat16sub(rs1, rs2) opcode_R(CUSTOM1, 0x03, 0x00, rs1, rs2)

void putc(char c)
{
	*(volatile char*)0x10000000 = c;
}

void puts(const char *s)
{
	while (*s) putc(*s++);
}

void *memcpy(void *dest, const void *src, int n)
{
	while (n) {
		n--;
		((char*)dest)[n] = ((char*)src)[n];
	}
	return dest;
}

void isr(){


}

// Rounding function requires 3 lower bits in mantissa be GRS (guard, round and sticky) bits
void bfloat16round(uint16_t *mant, uint16_t *exp)
{
    uint8_t grs;

    // Get guard, round and sticky bits
    grs = (*mant) & 0x7;

    
    switch (grs)
    {
    case 0:
        // No rounding
        *mant = (*mant) >> 3;
        break;

    case 1:
        // No rounding    
        *mant = (*mant) >> 3;
        break;

    case 2:
        // No rounding    
        *mant = (*mant) >> 3;
        break;

    case 3:
        // No rounding    
        *mant = (*mant) >> 3;
        break;

    case 4:
        // Tie, check LSB in mantissa to decide
        if ((*mant) & 0x8)
            *mant = ((*mant) >> 3) + 1;
        else
            *mant = (*mant) >> 3;

        break;
    default:
        // Round up
        *mant = ((*mant) >> 3) + 1;
        break;
    }

    // If rounding led to significand overflow
    if ((*mant) & 0x100)
    {
        *mant = ((*mant) >> 1);
        *exp = *exp + 1;
    }
    else
    {
        *mant = (*mant);
    }
}

// Convert float to bfloat16
uint16_t ftobf(float x)
{

    uint32_t aux = (*((uint32_t *)&x));

    // Right shift by keeping 10 fractional bits
    // to allow rounding
    uint16_t mant = (0x400|(aux >> 13)) & 0x7ff;
    uint16_t exp = (aux >> 23) & 0xff;
    uint16_t s = (aux >> 31);

    // Nearest to even rounding
    bfloat16round(&mant, &exp);

    return (s << 15) | (exp << 7) | ((mant)&0x7f);
}

// Approximation of sigmoid function
uint16_t bfloat16_sigmoid(uint16_t x)
{
    uint16_t k = ftobf(0.5);
    return bfloat16mul(k, bfloat16add(bfloat16div(x, bfloat16add(ftobf(1.0), x & 0x7fff)), ftobf(1.0)));
    //float xf=bftof(x);
     //float yf=1.0/(1+exp(-xf));
     //return ftobf(yf);
}

// function implementing an ann that approximates XOR gate
uint16_t test_xor_ann(uint16_t *inputs)
{

    // Weights and biases for pre-trained ann
    uint16_t w00 = ftobf(9.904663);
    uint16_t w01 = ftobf(9.904645);
    uint16_t w10 = ftobf(4.3833785);
    uint16_t w11 = ftobf(4.383373);
    uint16_t w20 = ftobf(12.21035);
    uint16_t w21 = ftobf(-13.690663);
    uint16_t b0 = ftobf(-6.486979);
    uint16_t b1 = ftobf(-6.5934243);
    uint16_t b2 = ftobf(-5.166204);

    // Hidden layer neurons
    uint16_t y0 = bfloat16_sigmoid(bfloat16add(bfloat16add(bfloat16mul(inputs[0], w00), bfloat16mul(inputs[1], w01)), b0));
    uint16_t y1 = bfloat16_sigmoid(bfloat16add(bfloat16add(bfloat16mul(inputs[0], w10), bfloat16mul(inputs[1], w11)), b1));

    // Output layer neuron
    uint16_t y = bfloat16_sigmoid(bfloat16add(bfloat16add(bfloat16mul(y0, w20), bfloat16mul(y1, w21)), b2));

    return y;
}

void main()
{
	char leds=1;
	int counter=0;
	uint32_t x1,x2,y_mul,y_div,y_sum,y_res;
	uint32_t y00,y01,y10,y11;

	x1=ftobf(25.369);
	x2=ftobf(-14.789);
	
	*(volatile char*)0x20000000=leds;

	y_mul=bfloat16mul(x1,x2);
	y_div=bfloat16div(x1,x2);
	y_sum=bfloat16add(x1,x2);
	y_res=bfloat16sub(x1,x2);

	uint16_t inputs[2];

    inputs[0] = ftobf(0.0);
    inputs[1] = ftobf(0.0);
	y00=test_xor_ann(inputs);

	inputs[0] = ftobf(1.0);
    inputs[1] = ftobf(0.0);
	y01=test_xor_ann(inputs);

	inputs[0] = ftobf(0.0);
    inputs[1] = ftobf(1.0);
	y10=test_xor_ann(inputs);

	inputs[0] = ftobf(1.0);
    inputs[1] = ftobf(1.0);
	y11=test_xor_ann(inputs);	





	

	while (1)
	{
		*(volatile char*)0x20000000=leds;
		leds=leds+1;
		if(leds==16)
		 leds=0;
		*(volatile int*)0x10000000=y00;
		*(volatile int*)0x10000004=y01;
		*(volatile int*)0x10000008=y10;
		*(volatile int*)0x1000000c=y11;
		for(int i=0;i<1000000;i++);
	}
	
}
