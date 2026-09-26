# EmuControl rework

You are an AmigaOS expert, MUI 3.8, and PiStorm/Emu68.

Original repository is here :
https://github.com/michalsc/EmuControl/tree/main

## Onglet "Status"

Augmente la largeur fixe des labels de 25 pixels.
Renomme le cadre "Performance" en "CPU".
Renomme le cadre "JIT Cache" en "Cache".
Renomme le cadre "RasPi Core" en "Core".
Enlève les ":" dans les labels.

## Onglet "Control"

Renomme l'onglet en "JIT".
Renomme le cadre "Debug Range" en "Range".

## Onglet "Debug"

Renomme le cadre "Debug Range" en "Range".
Renomme le cadre "Debug Options" en "Actions".

## Onglet "About"

Ajoute un saut de ligne après la ligne après [...] control panel.
Puis dans la ligne de version de Emu68, reformatte en "Running on Emu68 version 1.1.0".

## Onglet "Information"

Renomme l'onglet en "System".

Créé un cadre "Information", alignment vertical, avec 2 colonnes, split les labels et les valeurs textes en 2 colonnes.

On agrémente ce cadre avec les informations déjà obtenues, en s'inspirant du projet "C:\Developers\Emu68Info_1.0\"

Voici ce qu'on veut obtenir, compacté comme ceci :

Variant       : pistorm32lite
Model         : Raspberry Pi 4 Model B Rev 1.5
Revision      : b03115 (id: 17)
Chipset       : BCM2711 (VC6)
Manufacturer  : Sony UK
Processor     : arm,cortex-a72 @ 1.8 GHz
Memory size   : RPI 1 GB, ARM 992 MB, GPU 32 MB
Frame size    : 1920 x 1080 @ 81.0 MHz
Firmware      : Jul 23, 2026 12:47:27
Uptime        : 1h 9m 43s (4183 secs)
Serial-number : 10000000c937f849
MAC address   : e4:5f:01:ff:eb:ec
Turbo mode    : Enabled
Over voltage  : Disabled
