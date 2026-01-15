from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from webdriver_manager.chrome import ChromeDriverManager
from selenium.webdriver.common.action_chains import ActionChains
import time
import random
import datetime

# Inicia o cronômetro
start_time = time.time()
VIDEO_DURATION = 70  # 1:10 minutos em segundos

def print_execution_time():
    """Imprime o tempo decorrido desde o início da execução"""
    elapsed_time = time.time() - start_time
    print(f"\n[TEMPO DE EXECUÇÃO: {str(datetime.timedelta(seconds=elapsed_time))}]")

# Configurações do navegador
options = Options()
options.add_argument("--disable-blink-features=AutomationControlled")
options.add_argument("start-maximized")
options.add_argument(
    "user-agent=Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
    "AppleWebKit/537.36 (KHTML, like Gecko) "
    "Chrome/124.0.0.0 Safari/537.36"
)

# Inicialização do driver
print_execution_time()
print("Inicializando o navegador...")
driver = webdriver.Chrome(service=Service(ChromeDriverManager().install()), options=options)
actions = ActionChains(driver)
time.sleep(random.uniform(1, 3))

# Oculta o WebDriver
driver.execute_cdp_cmd("Page.addScriptToEvaluateOnNewDocument", {
    "source": """
        Object.defineProperty(navigator, 'webdriver', {
            get: () => undefined
        })
    """
})

try:
    # 1. Acessa o Google
    print_execution_time()
    print("Acessando o Google...")
    driver.get("https://google.com")
    time.sleep(random.uniform(2, 4))

    # 2. Pesquisa por "Alura"
    search_box = WebDriverWait(driver, 20).until(
        EC.presence_of_element_located((By.NAME, "q"))
    )
    time.sleep(random.uniform(1, 2))

    query = "Alura"
    for letra in query:
        search_box.send_keys(letra)
        time.sleep(random.uniform(0.2, 0.5))
    time.sleep(random.uniform(1, 2))
    search_box.send_keys(Keys.RETURN)
    time.sleep(random.uniform(3, 5))
    print_execution_time()
    print("Pesquisa no Google concluída")

    # 3. Clica no link da Alura nos resultados
    alura_link = WebDriverWait(driver, 20).until(
        EC.element_to_be_clickable(
            (By.XPATH, "//a[contains(@href, 'alura.com.br')]")
        )
    )
    time.sleep(random.uniform(1, 3))
    alura_link.click()

    WebDriverWait(driver, 25).until(
        EC.title_contains("Alura")
    )
    print_execution_time()
    print("ok - Acessou o site da Alura com sucesso")
    time.sleep(random.uniform(4, 7))

    # 4. Busca por "html e css" na Alura
    search_form = WebDriverWait(driver, 25).until(
        EC.presence_of_element_located((By.CLASS_NAME, "header__nav--busca-form"))
    )
    time.sleep(random.uniform(1, 3))
    
    search_input = search_form.find_element(By.TAG_NAME, "input")
    time.sleep(random.uniform(1, 2))
    
    query_alura = "html e css"
    for letra in query_alura:
        search_input.send_keys(letra)
        time.sleep(random.uniform(0.2, 0.6))
    time.sleep(random.uniform(1.5, 3))
    search_input.send_keys(Keys.RETURN)
    time.sleep(random.uniform(4, 7))
    print_execution_time()
    print("ok - Pesquisa por 'html e css' realizada na Alura")
    
    # 5. Clica no curso específico
    curso_link = WebDriverWait(driver, 25).until(
        EC.element_to_be_clickable(
            (By.XPATH, "//a[contains(@class, 'busca-resultado-link') and contains(@href, '/formacao-html-css')]")
        )
    )
    time.sleep(random.uniform(2, 4))
    
    driver.execute_script("arguments[0].scrollIntoView({behavior: 'smooth', block: 'center'});", curso_link)
    time.sleep(random.uniform(2, 5))
    
    curso_link.click()
    print_execution_time()
    print("ok - Clicou no link do curso de HTML e CSS")
    time.sleep(random.uniform(2, 4))
    
    # 6. Aguarda a página do curso carregar
    WebDriverWait(driver, 30).until(
        EC.title_contains("HTML e CSS")
    )
    print_execution_time()
    print("ok - Página do curso carregada com sucesso")
    time.sleep(random.uniform(5, 10))

    # 7. Encontrar e clicar no vídeo de introdução (COM CLIQUE DUPLO)
    try:
        print_execution_time()
        print("Localizando a seção do vídeo...")
        
        video_section = WebDriverWait(driver, 20).until(
            EC.presence_of_element_located(
                (By.XPATH, "//section[contains(@class, 'formacao-container-color-video')]")
            )
        )
        driver.execute_script("arguments[0].scrollIntoView({behavior: 'smooth', block: 'center'});", video_section)
        time.sleep(random.uniform(2, 4))
        
        print("ok - Seção do vídeo encontrada")
        print_execution_time()

        # Localiza o botão do vídeo
        video_button = WebDriverWait(driver, 15).until(
            EC.element_to_be_clickable(
                (By.XPATH, "//div[@data-videoid='UkpB-IYuwH4']//button | //div[contains(@class, 'videoPlayer-cover')]//button")
            )
        )

        # Clique duplo com ActionChains
        print("Executando clique duplo no vídeo...")
        actions.move_to_element(video_button).pause(1).click().pause(1).click().perform()
        time.sleep(3)  # Tempo para o vídeo iniciar
        
        print("ok - Clique duplo executado no player de vídeo")
        print_execution_time()

        # Verificação visual do player
        try:
            WebDriverWait(driver, 10).until(
                EC.visibility_of_element_located((By.XPATH, "//iframe[contains(@src, 'youtube.com')]"))
            )
            print("ok - Vídeo iniciado com sucesso (iframe visível)")
        except:
            print("AVISO: Não foi possível confirmar visualmente o vídeo, mas os cliques foram executados")

    except Exception as e:
        print(f"Erro ao tentar iniciar o vídeo: {str(e)}")
        driver.save_screenshot("video_error.png")
        print("Screenshot salvo como 'video_error.png' para análise")
        print_execution_time()

    # 8. Reproduzir o vídeo por 1:10 minutos
    print(f"\nReproduzindo vídeo por {VIDEO_DURATION} segundos...")
    for i in range(VIDEO_DURATION, 0, -1):
        mins, secs = divmod(i, 60)
        timer = f"{mins:02d}:{secs:02d}"
        print(f"\rTempo restante do vídeo: {timer}", end="", flush=True)
        time.sleep(1)
    
    print("\nok - Vídeo reproduzido completamente")
    print_execution_time()

except Exception as main_error:
    print(f"\nOcorreu um erro durante a execucao: {str(main_error)}")
    driver.save_screenshot("main_error.png")
    print("Screenshot salvo como 'main_error.png'")
    print_execution_time()

finally:
    print("\nFinalizando o navegador...")
    driver.quit()
    print_execution_time()
    print("Script concluído!")