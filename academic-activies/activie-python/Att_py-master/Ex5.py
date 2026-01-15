#calcula o maximo divisor comum 
def mdc(a, b):
    if b == 0: 
        return a
    else: 
        return mdc(b, a % b) 
    
#guarda os valores digitados e mostra o maximo divisor comum
num1 = int(input('Digite um número:'))
num2 = int(input('Digite outro número:'))
resultado = mdc(num1, num2)
print('MDC:', resultado)
 