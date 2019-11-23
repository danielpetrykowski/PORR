% Matrices generator
%% Parametry
% Zakres, z jakiego b�d� losowane liczby naturalne wype�niaj�ce macierz
% Pp = pocz�tek, Pk = koniec zakresu
Pp = 1;
Pk = 10;
% Wymiar macierzy generowanej
w = 10;
%% Generacja
newFile = fopen( 'net2.txt', 'wt' );
fprintf(newFile, "*Vertices " + w);
for i = 1 : w
   fprintf(newFile, '\n %d "v%d"', i, i); 
end
M = randi([Pp,Pk],w);
% Zerowanie warto�ci na g��wnej przek�tnej
M = M - diag(diag(M));
% Macierz przedstwia graf bez p�tli w�asnych, zapisywanie do pliku
fprintf(newFile, "\n *Matrix");
fclose(newFile);

newFile = fopen('net2.txt', 'a+');
fprintf(newFile, '\n');
for i = 1 : w
   for j = 1 : w
       fprintf(newFile, '\t %d', M(i,j));
   end
   fprintf(newFile, '\n');
end
fclose(newFile);