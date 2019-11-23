% Matrices generator
%% Parametry
% Zakres, z jakiego bêd¹ losowane liczby naturalne wype³niaj¹ce macierz
% Pp = pocz¹tek, Pk = koniec zakresu
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
% Zerowanie wartoœci na g³ównej przek¹tnej
M = M - diag(diag(M));
% Macierz przedstwia graf bez pêtli w³asnych, zapisywanie do pliku
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