
def insertion_sort(lista):
    for i in range(1, len(lista)):
        chave = lista[i]
        j = i - 1 
        while j >= 0 and chave < lista[j]:
            elem = int(input('Digite um elemento da lista:'))
            lista.append(elem)
            i+=1
            print(lista) 
            insertion_sort(lista) 
            print(lista)

