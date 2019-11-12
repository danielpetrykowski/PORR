# PORR

1. Opis Michała z Messengera (pozwoliłem sobie usunąć dygresje)
 - stworzyć generator sieci, czyli po prostu graf, miasta jako wierzchołki i krawędzie jako drogi, różne koszty poruszania się po krawędziach. Wpierw proponuje zrobić małą sieć (~10 wierzchołków) i zobaczyć, jak nam to będzie leciało na laptopach. 
 - może nam wysłać jakiś algorytm kontrolowanego wyżarzania, ale "jest tego w internecie 150", więc sami mamy sobie wybrać, jaki chcemy, "najlepiej jakiś najprostszy". 
 - po zaimplementowaniu tego schematu wyżarzania ustalamy parametry: rozkład ma być jednostajny, matematycznie opisany przez exp(-delta f/ kt). Generacja nowego punktu (czyli przejście do nowego wierzchołka w grafie po krawędzi) ma się odbywać na początku z dużym prawdopodobieństwem, żeby poznać dziedzinę, potem to p-wo proponuje zmniejszać o stały epsilon. Jeśli t będzie u nas za mały, to wpadniemy w minimum lokalne, i to odpowiada za szybkiemu wyżarzaniu i przechodzeniu cząsteczek w postać szkła, tego chcemy uniknąć. Dobre t powoduje, że mamy odpowiednio dużo czasu na poszukiwanie optymalnego rozwiązania i znajdziemy minimum globalne, to jest odpowiednik struktury krystalicznej o optymalnej, minimalnej energii... 
 - po zaimplementowaniu wersji sekwencyjnej mamy przeprowadzić testy i zobaczyć, jaki ten algorytm ma związek z łańcuchami Markowa. Pomocne mogą być teksty Metropolisa, który opisywał całe zjawisko symulowanego wyżarzania w latach sześćdziesiątych XX wieku i Kirk-Patricka
 - potem II i III część opierają się na dodaniu dyrektyw zrównoleglających najpierw w Open MP, a potem w MPI i Open CL, na karcie GPGPU. II etap projektu to Open MP i tutaj profesorka chce, żebyśmy sprawdzili nasz algorytm dla większej sieci (zresztą wersję sekwencyjną też, zapomniałem o tym napisać, dodaję teraz). Sprawdzić, jak działa to przy wykorzystaniu wszystkich rdzeni, które ma najlepszy z naszych komputerów, optymalnie byłoby, żeby liczba wątków była 2x większa od liczby rdzeni. Potem mamy zrobić test załamania zrównoleglania, czyli wprowadzić np. 3x wątków niż mamy rdzeni. Zwiększać t i pokazywać na wykresach, jak wygląda szybkość pracy algorytmu. Wszystko do tego momentu mamy oddać na I termin (czyli zdaje się do połowy grudnia).
 - potem mamy jeszcze zrównoleglić to w postaci gwiazdy w MPI w III etapie przyjmując inne dane początkowe, a na sam koniec w Open CL. 
 
 
Co do spraw technicznych: profesorka rekomenduje pisanie w C, bo to sprawdzona metoda i ponoć bezpieczna, ale powiedziała, że jak się bardzo uprzemy, to Python i ew. Java też mogą być.

2. Generownanie sieci za pomocą Pajek [Pajkek do ściągnięcia - http://mrvar.fdv.uni-lj.si/pajek/]
Stworzenie sieci:
Network -> Create New Network -> Complete Network -> Undirected

Nadanie losowych wartości krawędzią:
Partition -> Create Random Partition -> 1-Mode
	Wybierz ile w sieci ma być róznych wartości odległości
Partition -> Copy to Vector
Operations -> Network + Vector -> Transform -> "Vector(s) -> Line values" -> Multiply

Save:
File -> Save -> [Zapisz jako typ: Pajek Valued Matrix]


3. Linki 
Algorytm który został zadaptowany w orginale znajdziecie:
http://deerishi.github.io/tsp-using-simulated-annealing-c-/
Inne przydatne strony z algorytmami i kodem dotyczącymi symulownego wyrzażania:
https://github.com/arjun-krishna/TSP
https://codereview.stackexchange.com/questions/70310/simple-simulated-annealing-template-in-c11
Jakaś praca magisterska na temat cyklów Hamiltona w grafach (przystępnie też opisuje algorytm symulowanego wyrzażania):
http://users.uj.edu.pl/~ufkapano/download/Piotr_Szestalo_2015.pdf
