def fatorial(n, resultado=1):
   '''
   Função que lê um número inteiro n >= 0 e imprime n! 
   '''
   if n == 0 or n == 1: # caso base 
      return resultado 
   else: # passo recursivo
      return fatorial(n - 1, n * resultado)
  
# Função principal
def main(): 
   n = int(input('Digite um número inteiro: '))
   resultado = fatorial(n)
   print(20*'#')
   print('Fatorial:', resultado)
   print(20*'#') 
   main()
