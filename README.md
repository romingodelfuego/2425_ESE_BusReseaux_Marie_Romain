# TP Bus et Réseaux

Bienvenue sur le ReadMe de Romain Pace et Marie Caronello, élèves en spécialité ESE à l'ENSEA. Ce document présente en détail les étapes de notre TP, avec pour objectif d'approfondir notre compréhension des bus et des réseaux.

## Présentation

L’objectif de ce TP est de collecter les données du capteur BMP280 via le microcontrôleur STM32, puis de gérer ces données à l’aide d’une interface API. La figure ci-dessous présente l’architecture générale du système :

<p align="center">
  <img src="Images/architecture_systeme.png" alt="Architecture générale du système" width="600" height="auto">
</p>

## TP1 - Bus I2C

La première étape est de mettre en place la communication entre le microcontrôleur et les capteurs température, pression (BMP280) via  le bus I²C. Le capteur BMP280 comporte deux composants en I2C, ils vont donc partager le même bus et le STM32 va jouer le rôle de Master.


### Capteur BMP280

Avant de configurer le capteur BMP280 nous allons identifier certains éléments.

- Adresses I2C possibles pour le composant : Les adresses I2C du capteur BMP280 sont définies par les 6 premiers bits fixes : 111011x. Le dernier bit dépend de la connexion du pin SDO :
  - Si SDO est connecté à GND, l'adresse sera 1110110 (0x76).
  - Si SDO est connecté à VDDIO, l'adresse sera 1110111 (0x77).
<p align="center">
  <img src="Images/addresses_I2C.png" alt="Adresses I2C" width="600" height="auto">
</p>  

- Le registre et la valeur permettant d'identifier ce composant : Le registre utilisé pour identifier le capteur BMP280 est le registre 0xD0, et la valeur associée est 0x58. Cette valeur confirme qu'il s'agit bien du capteur BMP280, comme spécifié en page 26 du datasheet.
<p align="center">
  <img src="Images/registre_ID.png" alt="Registre d'identification" width="600" height="auto">
</p>

- Le registre et la valeur permettant de placer le composant en mode normal : Le registre de contrôle est 0xF4.La valeur à mettre dépend du mode que l'on souhaite activer. Pour placer le capteur en mode normal les derniers bits sont 11. Par exemple si on prend le cas d'un oversampling égal à 1 pour la température et la pression alors la valeur à configurer est 001 001 11 ce qui donne la valeur hexadécimale 0x27.
<p align="center">
  <img src="Images/controle_1.png" alt="Registre de controle" width="600" height="auto">
</p>
<p align="center">
  <img src="Images/controle_2.png" alt="Registre de controle" width="600" height="auto">
</p>

- Les registres contenant l'étalonnage du composant : Les registres contenant les données d'étalonnage sont situés entre les adresses 0x88 et 0xA1.
<p align="center">
  <img src="Images/etalonnage.png" alt="Registres d'étalonnage" width="600" height="auto">
</p>

- Les registres contenant la température (ainsi que le format) : Les registres utilisés pour lire la température sont 0xFA, 0xFB, et 0xFC.
<p align="center">
  <img src="Images/registres_temp.png" alt="Registres de température" width="600" height="auto">
</p>

- Les registres contenant la pression (ainsi que le format): Les registres utilisés pour lire la pression sont 0xF7, 0xF8, et 0xF9.
<p align="center">
  <img src="Images/registres_press.png" alt="Registres de pression" width="600" height="auto">
</p>

  - Les fonctions permettant le calcul de la température et de la pression compensées en format entier 32 bits : Ces fonctions sont à la page 45 et 46 de la datasheet et donnent :
  - La fonction de calcul pour la pression est : double bmp280_compensate_P_double(BMP280_S32_t adc_P)
  - La fonction de calcul pour la température est : double bmp280_compensate_T_double(BMP280_S32_t adc_T) 

Ce TP a permis de comprendre les principaux registres et leurs rôles dans la configuration et la lecture des données du capteur BMP280 via le bus I²C. 

###  Setup du STM32

Dans cette partie, nous avons determiné les broches pour configurer le Bus CAN, l'USART2, l'USART3 et la communication I2C.

- Pour la connexion à l'aide du bus CAN entre le moteur et la STM32 : Rx -> PB8; Tx -> PB9
- Pour la connexion USART2 entre le port USB et la STM32 : Rx ->PA3 ; Tx -> PA2
- Pour la connexion USART1 entre la Raspberry Pi et la STM32 : Rx -> PA9 ; Tx -> PA10
- Pour la conneion I2C entre le capteur BMP280 et la STM32 : SDA -> PB7 ; SCL -> PB6

On commence par s'intéresser dans un premier temps à la redirection du printf. Pour cela dans le fichier stm32f4xx_hal_msp.c, on ajoute du code permettant de gérer le printf, à l’aide de putchar. Lors du test, il ne faut pas oublier le \r\n pour que le message s’affiche directement.
<p align="center">
  <img src="Images/printf.png" alt="Registres de pression" width="600" height="auto">
</p>

### Communication I²C

#### Communication avec le BMP280

On utilise les fonctions Transmit et Receive de la bibliothèque HAL pour établir la communication I²C. 

- HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
- HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)

Où on a :

- I2C_HandleTypeDef hi2c: structure stockant les informations du contrôleur I²C
- uint16_t DevAddress: adresse I³C du périphérique Slave avec lequel on souhaite interagir.
- uint8_t *pData: buffer de données
- uint16_t Size: taille du buffer de données
- uint32_t Timeout: peut prendre la valeur HAL_MAX_DELAY

Cependant, il est important de noter que ces fonctions exigent une adresse de 8 bits, tandis que l'adresse I²C est, elle, codée sur 7 bits. Par conséquent, il est nécessaire de décaler l'adresse I²C d'un bit vers la gauche avant de l'utiliser avec les fonctions HAL.

##### Identification du BMP280
Dans un premier temps, il faut identifier notre capteur pour cela nous devons lire la valeur du registre ID (0xD0). On doit donc envoyer l'adresse du registre ID, 0xD0, et recevoir 1 octet correspondant au contenu du registre, 0x58, on fait cela avec les fonctions HAL_StatusTypeDef HAL_I2C_Master_Receive et HAL_StatusTypeDef HAL_I2C_Master_Transmit. On va construire une fonction qui va renvoyer la valeur du registre 0xD0.

<p align="center">
  <img src="Images/id_capteur.png" alt="Obtenir l'ID du capteur" width="600" height="auto">
</p>

Après éxécution de ce code nous obtenons 88 soit en hexadécimal 0x58, c'est ce qu on souhaitait obtenir.

##### Configuration du BMP280

On va donc à présent configurer le BMP280 en mode normal avec une Pressure oversampling = x16 et une Temperature oversampling = x2. On va utiliser la documentation et découvrir que le regitsre de contrôle 0xF4 vu précédemment permet de configurer tous ces éléments. En effet :

- Pressure oversampling : x16 -> 101
- Temperature oversampling : x2 -> 010
- mode normal : 11

Nous avons donc écrit la valeur 0b01010111 qui vaut 0x57 en hexadécimal dans le registre 0xF4.

<p align="center">
  <img src="Images/config.png" alt="Configure BMP280" width="600" height="auto">
</p>

##### Récupération de l'étalonnage, de la température et de la pression

Maintenant on veut récupérer les valeurs de la température et de la pression et envoyez sur le port série le valeurs 32 bit non compensées de la pression de la température. Pour cela il faut envoyer l'adresse du registre et ensuite on reçoit l'étalonnage dans un tableau adapté. L'adresse du registre est donnée par la documentation :

<p align="center">
  <img src="Images/memory_map.png" alt="carte des registres d'étalonnage" width="600" height="auto">
</p>

Le registre d'étalonnage est ici notre point d'intérêt, et son adresse correspond à celle du premier élément du tableau, soit 0x88. Nous devons donc transmettre cette adresse, puis recevoir les données d'étalonnage dans un tableau de 26 éléments. Ensuite, nous avons organisé les données d'étalonnage dans le format approprié pour effectuer le calcul de compensation.

<p align="center">
  <img src="Images/etalonnage.png" alt="Registres d'étalonnage" width="600" height="auto">
</p>

On va donc à l'aide des indications d'étalonnage creer deux focntion get_pressure et get_temperature qui vont permettre de réccupérer la température grâce aux différents registres. 
<p align="center">
  <img src="Images/get_pressure.png" alt="Fonction pour obtenir la température" width="600" height="auto">
</p>

##### Calcul des températures et des pression compensées

Pour cette dernière partie , une fois qu on a récupéré la température et la pression nous devons la compenser pour pouvoir l'intérpréter correctement et pour que ces valeurs soient au bon format. Pour cela nous avons juste a copier les codes présents dans la documentation page 45 et 46 :
<p align="center">
  <img src="Images/compensation.png" alt="Température et pression compensées" width="600" height="auto">
</p>

### ADXL345 et MPU9250

Après avoir configurer les capteurs sur le BMP280, on veut configurer l'ADXL345 et le MPU9250 sur le même principe.

#### ADXL345

Tout d'abord interessons nous à l'ADXL345, nous allons créer deux fonctions : ADXL345_check() et ADXL345_init(), de la même manière que pour le BMP280. On ajoute aussi une fonction pour lire les données (x, y, z) : ADXL345_read_axes(int16_t* x, int16_t* y, int16_t* z).

<p align="center">
  <img src="Images/ADXL345.png" alt="Liste des fonctions de l'ADXL345" width="600" height="auto">
</p>

#### MPU9250

De la même manière pour le MPU9250 on initialise deux fonctions MPU9250_init() et MPU9250_check() pour initialiser le capteur et le configurer. Puis comme le MPU9250 permet d'obtenir des informations avec un accéléromètre, un gyroscope et un magnétomètre on va établir 3 fonctions pour lire ces différentes données : MPU9250_read_accel(SensorData *accel) MPU9250_read_gyro(SensorData *gyro) MPU9250_read_mag(SensorData *mag). Enfin on va créer une fonction global qui permettra l'envoie de ces données MPU9250_send_data(MPU9250_Data *data). On établit alors une strucutre pour stocker les données en fonction de leur catégorie du capteur SensorData et une strusture pour garder l'ensemble des données MPU9250_Data.

<p align="center">
  <img src="Images/MPU9250.png" alt="Listes des fonctions du MPU9250" width="600" height="auto">
</p>

## TP2 - Interfaçage STM32 - Raspberry

## TP3 - Interface REST

## TP4 - Bus CAN

L'objectif ici est de mettre en place un périphérique sur un bus CAN entre le moteur et la carte STM32L476. On devra donc ici réussir à piloter un module moteur pas-à-pas alimenté en 12V via le bus CAN, en réglant la vitesse de communication à 500 kbit/s, en prenant en compte des spécificités techniques liées au ratio de timing CAN.

On definit donc le CAN tel que Rx -> PB8 et Tx -> PB9. Deplus on veut que la vitesse du CAN soit de 500kbit/s ainsi on doit mettre le prescaler à 6, le Time Quanta in bit Segment 1 à 12 et le Time Quanta in bit Segment 2 à 2. 

Deplus pour perfectionner la configuration on s'intéresse à l'angle tel que donné dans la documentation, on choisit de se mettre en mode manuel. 

<p align="center">
  <img src="Images/angle.png" alt="Configuration de l'angle en mode manuel" width="600" height="auto">
</p>

Premièrement on souhaite faire bouger le moteur de 90 degrès dans un sens puis dans l'autre sens pour cela on va créer une fonction permettant de faire bouger le moteur en fonction de l'angle et du sens. Pour cela on va utiliser le pheader qui va permettre de caractériser les informations, dans un premier temps on a choisit de mettre le moteur en mode normal donc StdId = 0x61, enfin on va transmettre les informations sous une liste de 2 éléments donc DLC = 0x02.

<p align="center">
  <img src="Images/code_moteur.png" alt="Code du fonctionnmeent du moteur" width="600" height="auto">
</p>

Ensuite on veut faire en sorte que le mouvement du moteur soit proportionnel à la valeur du capteur. Pour cela on va retenir la température au lancement du code puis on va faire bouger le moteur en fonction de la variation de température, le coefficient utilisé pour modéliser cette variation de température est 2. Si la variation est supérieur à 180 degrés alors le moteur tourne dans le sens horaire, à l'inverse si la variation est inférieur à 180 degrés alors le moteur tourne dans le sens imaginaire. 

<p align="center">
  <img src="Images/moteur_temp.png" alt="Mouvement du moteur en fonction de la température" width="600" height="auto">
</p>
