#entrada dos dados
deslocamento = int(input('Digite o deslocamento: ')) 
texto = input('Digite o texto a ser criptografado: ')
texto_criptografado = ''

for letra in texto: #percorre cada caractere do texto fornecido.
    if letra.isupper(): #verifica se todas as letras são maisculas, se for maiscula retorna verdadeiro
        etra_criptografada = chr((ord(letra.lower()) + deslocamento - 97) % 26 + 65)  
    elif letra.islower(): #verifica se todas as letras são minusculas, se for minuscula retorna verdadeiro
        etra_criptografada = chr((ord(letra) + deslocamento - 97) % 26 + 97) 
    else: letra_criptografada = letra + letra_criptografada + letra_criptografada
    print('Texto criptografado:', texto_criptografado)
