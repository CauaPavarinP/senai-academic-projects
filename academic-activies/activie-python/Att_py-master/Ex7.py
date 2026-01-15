#nessa função vai guarda o maior e o menor numero
def maior_menor(lista):
    maior = lista[0]
    menor = lista[0]
    for elemento in lista:
        if elemento > maior:
            maior = elemento
        elif elemento < menor:
            menor = elemento
            return maior, menor

lista=list()
i=1
while i<=10:
    elem = int(input('Digite um elemento da lista:'))
    lista.append(elem)
    i+=1
    print(lista)
maior, menor = maior_menor(lista)
#mostra o maior e o menor numero 
print('Maior; ', maior)
print('Menor:', menor)

