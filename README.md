    Manea Lidia-Elena
 
  Tema testeaza interactiunea dintre un utilizator si server, prin diferite comenzi si interactiunea cu un REST API:

1. register: un utilizator se inregistreaza pe server, cu un username si parola. Sunt testate urmatoarele situatii, in functie
de scenarii: daca inregistrarea a fost facuta cu succes, se merge mai departe; daca username ul a fost luat,  se afiseaza
un mesaj de eroare, in concordanta.

2. login: un utilizator se logheaza pe server, cu username ul si parola cu care acesta a fost inregistrat. In functie de
situatie, apar urmatoarele mesaje: daca username ul si parola sunt corecte, atunci utilizatorul s a logat cu succes;
daca username ul si/sau parola sunt gresite, atunci apare un mesaj de eroare corespunzator. In cazul loginului cu succes, 
se salveaza un cookie.

3. enter_library: aceasta comanda se executa doar daca utilizatorul este logat. Daca nu este logat, se afiseaza un mesaj 
de semnalare al acestui fapt. Daca este logat, se intra in librarie.

4. get_books: daca utilizatorul este logat, se afiseaza toate cartile in format json: se afiseaza id ul si titlul cartii respective.
Daca nu exista carti, se afiseaza lista goala. Daca utilizatorul nu este logat, atunci se afiseaza un mesaj de semnalare.

5. get_book: daca utilizatorul este logat, acesta introduce un id pentru o carte pe care acesta doreste sa o vada (se afiseaza
toate detaliile in format json). Daca utilizatorul nu este logat, se afiseaza un mesaj de semnalare.

6. add_book: se adauga o carte in biblioteca utilizatorului daca acesta este logat. Se introduc detaliile despre carte si 
apoi , daca acestea sunt valide, cartea se introduce in biblioteca. Daca utilizatorul nu este logat, se afiseaza un mesaj.

7. delete_book: se sterge o carte cu indexul dat. Daca indexul nu se gaseste, adica cartea nu exista, se afiseaza mesaj corespunzator.
Altfel, se sterge cartea din biblioteca. 

8. logout: utilizatorul da log out daca este logat. Se elibereaza cookie ul si token ul. Daca utilizatorul nu este logat, nu are rost
sa dea log out.

9. exit: da exit

La fiecare pas se verifica urmatoarele erori acolo unde este cazul: daca cookie ul este gol, daca tokenul jwt este gol, daca datele
introduse sunt invalide (la register,  la login, la add_book daca nu se respecta ok formatul datelor cartii care sa fie adaugata). Totodata,
inainte de trimiterea unei cereri catre server, se deschide conexiunea catre el, iar dupa trimitere si procesare, aceasta se inchide. Am
verificat fiecare situatie legata de login in cazul unei erori intamplatoare sau unui comportament nenatural.
La fiecare pas, dupa citirea datelor din input, se formeaza mesajul si se trimite cerere catre server. Am folosit nlohmann pentru jsonuri,
fiind usor de folosit si putand manipula datele usor. 
