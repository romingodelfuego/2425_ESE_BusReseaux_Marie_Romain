# TP Bus et Réseaux

Bienvenue sur le ReadMe de Romain Pace et Marie Caronello, élèves en spécialité ESE à l'ENSEA. Ce document présente en détail les étapes de notre TP, avec pour objectif d'approfondir notre compréhension des bus et des réseaux.

## TP1 - Bus I2C

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






