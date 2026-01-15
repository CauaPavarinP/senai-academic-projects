#guarda o valor mais alto
def maior3(a, b, c):
    if a >= b and a >= c: 
        return a 
    elif b >= c:
        return b
    else: return c

#guarda os numeros digitados
n1=int(input('Digite um número:'))
n2=int(input('Digite um número:'))
n3=int(input('Digite um número:'))

#mostra o valor mais alto
resultado = maior3(n1,n2,n3)
print(resultado)
    