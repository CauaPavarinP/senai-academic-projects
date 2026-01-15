#converte o valor digitado em quilometros para metros
def converter_quilometros_para_metros(quilometros):
    metros = quilometros * 1000 #aqui faz o calculo para transformar em metros
    return metros #retorna o valor em metros
try:
    quilometros = float(input('Digite a distância em quilômetros: ')) #guarda o valor digitado em Km
    metros = converter_quilometros_para_metros(quilometros) #joga o valor pra função calcular
    print('metros:', metros) #mostra o valor em metros
except ValueError: 
    print('Entrada inválida!') 