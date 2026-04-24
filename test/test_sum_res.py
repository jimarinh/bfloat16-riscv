import tensorflow as tf
import numpy as np

#Muestra el valor aproximado y la representación en hexadecimal del número bfloat16
def print_bf16(str, value):
    bits = tf.bitcast(value, tf.uint16).numpy()
    print(str, value.numpy(), " bfloat16: ", format(bits, '04x'))


# Usa tensorflow para crear dos números en formato bfloat16
value1 = tf.constant(45.83, dtype=tf.bfloat16)
value2 = tf.constant(625.41, dtype=tf.bfloat16)

print("Valores de entrada")
print_bf16("Value1 (45.83):", value1)
print_bf16("Value2 (625.41):", value2)

#Prueba de suma

print("Prueba de suma")
y = value1 + value2
print_bf16("x1+x2:", y)

y = (-value1) + value2
print_bf16("(-x1)+x2:", y)

y = value1 + (-value2)
print_bf16("x1+(-x2):", y)

y = (-value1) + (-value2)
print_bf16("(-x1)+(-x2):", y)


#Prueba de resta

print("Prueba de resta")
y = value1 - value2
print_bf16("x1-x2:", y)

y = (-value1) - value2
print_bf16("(-x1)-x2:", y)

y = value1 - (-value2)
print_bf16("x1-(-x2):", y)

y = (-value1) - (-value2)
print_bf16("(-x1)-(-x2):", y)
