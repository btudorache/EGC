# Elemente de Grafica pe Calculator

Temele de la Elemente de Grafica pe Calculator 2021-2022. Au ca scop implementarea unor jocuri folosind framework-ul [pus la dispozitie de catre echipa cursului](https://github.com/UPB-Graphics/gfx-framework), care foloseste standardul OpenGL cu implementare in **C++**.

# Teme

1. Implementarea unui survival shooter 2D.

2. Implementarea unui labirint din care trebuie sa evadezi in 3D.

3. Implementarea unui ring de dans.


Mai multe detalii de implementare se gasesc in folderele asociate temelor.

# Detalii de rulare

Pentru a putea juca jocurile trebuie urmaritii pasii:

1. Instalarea [framework-ului folosit](https://github.com/UPB-Graphics/gfx-framework) dupa indicatiile specificate.

2. Adaugarea fiecarei teme in parte in folderul ```/src/lab_m1```.

3. Adaugarea directivelor de include ale fiecarui header al temelor in fisierul ```/src/lab_m1/lab_list.h``` (```#include "lab_m1/tema1/tema1.h"``` pentru tema 1 etc...)

4. Inlocuirea cu "tema" dorita in fisierul ```/src/main.cpp``` la linia 47 (```World* world = new m1::Tema1();``` pentru tema 1 etc...)

5. Compilarea si rularea proiectului.