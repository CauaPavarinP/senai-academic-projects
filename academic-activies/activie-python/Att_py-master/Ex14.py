numeros = []
for i in range(10):
    try:
        numero = int(input('Digite um número inteiro: '))#guarda o valor digitado
        numeros.append(numero)
    except ValueError:
        print('Entrada inválida!!!')
soma = sum(numeros) #soma os
media = soma / len(numeros)
#mostra a soma dos valor e a média
print('Soma:', soma)
print('Média:', media) 