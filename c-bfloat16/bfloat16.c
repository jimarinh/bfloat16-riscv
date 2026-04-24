// Routines for bfloat16 arithmetics
// Alexander López Parrado (2024)
#include <stdint.h>
#include <stdio.h>
#include <math.h>

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

// Half precision floating point division
uint16_t bfloat16div(uint16_t a, uint16_t b)
{
    uint16_t s;
    uint16_t result;
    uint16_t adder_out, sub_out, q_exp;
    uint16_t q_mant;
    uint32_t div_out;

    // Sign computation
    s = (a ^ b) & 0x8000;

    // Subtraction of exponents
    sub_out = (((a >> 7) & 0xFF) - ((b >> 7) & 0xFF)) & 0x1FF;

    // Add bias
    adder_out = sub_out + 127;

    // Fractional divider (4 extra bits)
    div_out = ((uint32_t)(0x80 | (a & 0x7F)) << 11) / (uint32_t)(0x80 | (b & 0x7F));

    // Exponent normalization
    q_exp = (div_out & 0x800) ? adder_out : adder_out - 1;

    // Significand normalization
    q_mant = (div_out & 0x800) ? (div_out >> 1) & 0x7FF : (div_out) & 0x7FF;

    // Rounding
    bfloat16round(&q_mant, &q_exp);
    q_mant &= 0x7f;

    // Result
    result = s | (q_exp << 7) | q_mant;

    // If numerator is zero and denominator is different from zero result is zero
    if (((a & 0x7FFF) == 0) && ((b & 0x7FFF) != 0))
        result = s;

    // If denominator is zero and numerator is different from zero result is infinity
    if (((a & 0x7FFF) != 0) && ((b & 0x7FFF) == 0))
        result = s | (255 << 7);

    // If exponent underflow result is zero
    if (q_exp == 0)
        result = s;

    // If exponent overflow result is infinity
    if (q_exp == 255)
        result = s | (255 << 7);

    // If exponent overflow result is infinity
    if ((q_exp & 0x100) && (a & 0x4000) && ((b & 0x4000) == 0))
        result = s | (255 << 7);

    // If exponent underflow result is zero
    if ((q_exp & 0x100) && ((a & 0x4000) == 0) && (b & 0x4000))
        result = s;

    return result;
}

// Half precision floating point multiplication
uint16_t bfloat16mul(uint16_t a, uint16_t b)
{
    uint16_t s;
    uint16_t result;
    uint16_t adder_out, sub_out, p_exp;
    uint16_t p_mant;
    uint32_t mult_out;

    s = (a ^ b) & 0x8000;
    adder_out = (((a >> 7) & 0xFF) + ((b >> 7) & 0xFF)) & 0x1FF;
    sub_out = adder_out - 127;
    mult_out = (uint32_t)(0x80 | (a & 0x7F)) * (uint32_t)(0x80 | (b & 0x7F));

    p_exp = (mult_out & 0x8000) ? sub_out + 1 : sub_out;
    p_mant = (mult_out & 0x8000) ? (mult_out >> 5) & 0x7FF : (mult_out >> 4) & 0x7FF;

    // Rounding
    bfloat16round(&p_mant, &p_exp);
    p_mant &= 0x7f;

    result = s | (p_exp << 7) | p_mant;

    if (((a & 0x7FFF) == 0) || ((b & 0x7FFF) == 0))
        result = s;
    if (p_exp == 0)
        result = s;
    if (p_exp == 255)
        result = s | (255 << 7);
    if ((p_exp & 0x100) && (a & 0x4000) && (b & 0x4000))
        result = s | (255 << 7);

    if ((p_exp & 0x100) && ((a & 0x4000) == 0) && ((b & 0x4000) == 0))
        result = s;

    return result;
}

// Pre-processing of mantisa to include guard, round and sticky bits for add/sub
uint16_t bfloat16addsubpp(uint16_t mant, uint16_t exp_diff)
{
    switch (exp_diff)
    {
    case 0:
        return (mant << 3);
        break;

    case 1:
        return (mant << 2);
        break;

    case 2:
        return (mant << 1);
        break;

    case 3:
        return (mant);
        break;

    case 4:
        return (mant >> 1);
        break;
    case 5:
        return (mant >> 2);
        break;
    case 6:
        return (mant >> 3);
        break;
    case 7:
        return (mant >> 4);
        break;
    case 8:
        return (mant >> 5);
        break;

    default:
        // No significand bits left
        return (0);
        break;
    }
}

// Half precision floating point add/sub
uint16_t bfloat16addsub(uint16_t a, uint16_t b, uint8_t as)
{
    uint16_t s_a, s_b, as_s;
    uint16_t result;
    uint16_t exp_a, exp_b, exp_diff, as_exp;
    uint16_t as_mant, mant_a, mant_b, mant_sum;
    uint16_t mant_a_shifted, mant_b_shifted;
    uint16_t pattern;
    uint32_t addsub_out;
    uint8_t leading_zeros;

    // Exponents, significands and signs unpacking
    exp_a = (a >> 7) & 0xFF;
    exp_b = (b >> 7) & 0xFF;
    mant_a = 0x80 | (a & 0x7F);
    mant_b = 0x80 | (b & 0x7F);
    s_a = (a >> 15) & 0x1;
    s_b = (b >> 15) & 0x1;

    // Select larger exponent and significand normalization (include GRS bits)
    if (exp_a > exp_b)
    {
        exp_diff = exp_a - exp_b;

        mant_b_shifted = bfloat16addsubpp(mant_b, exp_diff);
        mant_a_shifted = mant_a << 3;
        as_exp = exp_a;
    }
    else
    {
        exp_diff = exp_b - exp_a;
        mant_a_shifted = bfloat16addsubpp(mant_a, exp_diff);
        mant_b_shifted = mant_b << 3;
        as_exp = exp_b;
    }

    // If signs are different and operation is either add or sub
    if (as ^ s_a ^ s_b)
    {
        // Subtract mantissas (different signs and add or sub operation -> subtraction)
        mant_sum = (mant_a_shifted > mant_b_shifted) ? mant_a_shifted - mant_b_shifted : mant_b_shifted - mant_a_shifted;
        // Select sign of result
        as_s = (mant_a_shifted > mant_b_shifted) ? s_a : (s_b ^ as);

        // Rounding
        bfloat16round(&mant_sum, &as_exp);

        // If result is zero
        if (mant_sum == 0)
        {
            as_exp = 0;
            as_mant = 0;
        }
        else
        {
            // Renormalizes through significand left-shifting and exponent decrement
            as_mant = mant_sum;
            while (1)
            {
                if (as_mant & 0x80)
                    break;
                else
                {
                    as_mant <<= 1;
                    as_exp -= 1;
                }
            }
        }
    }
    else // signs are equal and operation is add
    {
        // Add mantissas
        // equal signs and add operation then add
        mant_sum = mant_a_shifted + mant_b_shifted;
        as_s = s_a;
        // If result is zero
        if (mant_sum == 0)
        {
            as_exp = 0;
            as_mant = 0;
        }
        else
        {
            // If significand overflow -> renormalize
            if (mant_sum & 0x800)
            {
                as_mant = mant_sum >> 1;
                as_exp = as_exp + 1;
            }
            else
            {
                as_mant = mant_sum;
                as_exp = as_exp;
            }

            // Rounding
            bfloat16round(&as_mant, &as_exp);
        }
    }

    result = (as_s << 15) | (as_exp << 7) | (as_mant & 0x7f);

    if ((as_exp & 0x100))
        result = (as_s << 15) | (255 << 7);

    return result;
}

// Convert bfloat16 to float
float bftof(uint16_t x)
{

    uint32_t aux = (uint32_t)x << 16;

    return *((float *)&aux);
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
    //uint16_t k = ftobf(0.5);
    //return bfloat16mul(k, bfloat16addsub(bfloat16div(x, bfloat16addsub(ftobf(1.0), x & 0x7fff, 0)), ftobf(1.0), 0));
    float xf=bftof(x);
     float yf=1.0/(1+exp(-xf));
     return ftobf(yf);
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
    uint16_t y0 = bfloat16_sigmoid(bfloat16addsub(bfloat16addsub(bfloat16mul(inputs[0], w00), bfloat16mul(inputs[1], w01), 0), b0, 0));
    uint16_t y1 = bfloat16_sigmoid(bfloat16addsub(bfloat16addsub(bfloat16mul(inputs[0], w10), bfloat16mul(inputs[1], w11), 0), b1, 0));

    // Output layer neuron
    uint16_t y = bfloat16_sigmoid(bfloat16addsub(bfloat16addsub(bfloat16mul(y0, w20), bfloat16mul(y1, w21), 0), b2, 0));

    return y;
}

int main()
{

    // Test of half precision floating point arithmetic

    uint16_t x1, x2;
    x1 = ftobf(0.78923);
    x2 = ftobf(-15.7833);
    printf("%x,%x\n", x1, x2);

    printf("%f+%f=%f\n", bftof(x1), bftof(x2), bftof(bfloat16addsub(x1, x2, 0)));
    printf("%f-%f=%f\n", bftof(x1), bftof(x2), bftof(bfloat16addsub(x1, x2, 1)));
    printf("%f*%f=%f\n", bftof(x1), bftof(x2), bftof(bfloat16mul(x1, x2)));
    printf("%f/%f=%f\n", bftof(x1), bftof(x2), bftof(bfloat16div(x1, x2)));

    x2 = ftobf(3.4);
    x1 = ftobf(400.7);

    printf("%f+%f=%f\n", bftof(x1), bftof(x2), bftof(bfloat16addsub(x1, x2, 0)));
    printf("%f-%f=%f\n", bftof(x1), bftof(x2), bftof(bfloat16addsub(x1, x2, 1)));
    printf("%f*%f=%f\n", bftof(x1), bftof(x2), bftof(bfloat16mul(x1, x2)));
    printf("%f/%f=%f\n", bftof(x1), bftof(x2), bftof(bfloat16div(x1, x2)));

    // Test of sigmoid function

    x1 = ftobf(100);
    printf("sigmoid(%f)=%f\n", bftof(x1), bftof(bfloat16_sigmoid(x1)));

    // Test of ANN for XOR gate

    uint16_t inputs[2];

    inputs[0] = ftobf(0.0);
    inputs[1] = ftobf(0.0);
    printf("%f xor %f = %f\n", bftof(inputs[0]), bftof(inputs[1]), bftof(test_xor_ann(inputs)));

    inputs[0] = ftobf(0.0);
    inputs[1] = ftobf(1.0);
    printf("%f xor %f = %f\n", bftof(inputs[0]), bftof(inputs[1]), bftof(test_xor_ann(inputs)));

    inputs[0] = ftobf(1.0);
    inputs[1] = ftobf(0.0);
    printf("%f xor %f = %f\n", bftof(inputs[0]), bftof(inputs[1]), bftof(test_xor_ann(inputs)));

    inputs[0] = ftobf(1.0);
    inputs[1] = ftobf(1.0);
    printf("%f xor %f = %f\n", bftof(inputs[0]), bftof(inputs[1]), bftof(test_xor_ann(inputs)));
}
