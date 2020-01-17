#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <windows.h>
#include <climits>
#include <omp.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <CL/cl.h>

#define MAX_SOURCE_SIZE (0x100000)


using namespace std;

//-----------------------------------------------------------------------
const double INIT_TEMPERATURE = 99999999999999999999999999999999999999999.0;
const double COOLING_RATE = 0.9;
const double ABSOLUTE_TEMPERATURE = 0.00000001;
const string FAIL_PATH = "D:/DanielPetrykowski/Documents/studia/PORR/PORR/netTest/net2.txt";
const int NUM_THREADS = 1;


//----------------------------------------------------------------------

int** graph; 
cl_int* graphOpenCL;
cl_int numberVertics;
vector<int> citiesOrder;
cl_int* citiesOrderOpenCL;// arr is the array that stores the City order

//Functions
vector<string> ReadFileToArrayLines();
int NumberVerticsFromArrayLines(vector<string> lines);
void ReadMatrix(vector<string> lines);
int Distance(int city1, int city2);
void SwapArr(int i, int j);
int GetTourLength(vector<int> cities);
void InitNearestNeighbourTour();
double GetRandomNumber(double i, double j);
void SwapForVector(int i, int j, vector<int> cities);
double GetProbability(int difference, double temperature);
void PrintPath(vector<int> path);

vector<string> ReadFileToArrayLines()
{
	// Read file
	vector<string> lines;
	fstream file;
	file.open(FAIL_PATH, ios::in);
	if (file.is_open())
	{
		string tp;
		while (getline(file, tp))
		{
			lines.push_back(tp);
		}
		file.close();
	}
	return lines;
}

int NumberVerticsFromArrayLines(vector<string> lines)
{
	vector<string> elements;
	string element;

	stringstream lineStream(lines[0]);
	while (getline(lineStream, element, ' '))
	{
		elements.push_back(element);
	}
	numberVertics = atoi(elements[1].c_str());
	cout << "Number of vertices: " << numberVertics << "\n";
	return numberVertics;
}

void ReadMatrix(vector<string> lines)
{
	if (lines[numberVertics + 1].compare("*Matrix") == 0)
	{
		int numberLine = numberVertics + 2;
		int row;

		int i;
		for (i = 0; i < numberVertics; i++)
		{
			cout << "\t" << i + 1;
		}
		cout << "\n";

		for (row = 0; row < numberVertics; row++)
		{
			string edgeValue;
			int col = 0;

			stringstream lineStream(lines[numberLine + row]);
			cout << row + 1 << "\t";

			while (getline(lineStream, edgeValue, ' '))
			{
				if (edgeValue.compare("") != 0)
				{
					int value = atoi(edgeValue.c_str());
					graph[row][col] = value;
					cout << graph[row][col] << "\t";
					col++;
				}
			}
			cout << "\n";
		}
	}
}


int Distance(int city1, int city2)
{
	return graph[city1][city2];
}

void SwapArr(int i, int j)
{
	int temp = citiesOrderOpenCL[i];
	citiesOrderOpenCL[i] = citiesOrderOpenCL[j];
	citiesOrderOpenCL[j] = temp;
}

int GetTourLength(vector<int> cities) //This function returns the tour length of the current order of cities
{
	vector<int>::iterator it;

	it = cities.begin();
	int pcity1 = *it;
	int ncity;
	// cout<<"\n the pcity is "<<pcity1<<"\n";
	int tourLength = Distance(0, pcity1);
	for (it = cities.begin() + 1; it != cities.end(); it++)
	{
		ncity = *it;
		tourLength += Distance(pcity1, ncity);

		pcity1 = ncity;
	}

	tourLength += Distance(pcity1, 0); //adding the distance back to the source path
	return tourLength;
}

void InitNearestNeighbourTour() //puts the nearestNeighbourTour in the vector cities
{
	int numCities = numberVertics, i, j;

	for (i = 0; i < numCities; i++)
	{
		citiesOrderOpenCL[i] = i;
		//cout<<"arr is "<<i<<"\n";
	}
	int best, bestIndex;
	for (i = 1; i < numCities; i++)
	{
		best = INT_MAX;
		for (j = i; j < numCities; j++)
		{
			if (Distance(i - 1, j) < best)
			{
				best = Distance(i, j);
				bestIndex = j;
			}
		}
		SwapArr(i, bestIndex);
	}
}

void InitCitiesOrderOpenCL() {
	for (int i = 0; i < numberVertics; i++) {
		for (int j = 0;j < numberVertics;j++) {
			graphOpenCL[i * numberVertics + j] = graph[j][i];
		}
	}
}

void InitCitiesOrder() {
	citiesOrder.clear();

	// Inicjalizacja vectora z kolejnoœci¹ odwiedzanych miast
	for (int i = 0; i < numberVertics; i++)
	{
		citiesOrder.push_back(citiesOrderOpenCL[i]);
	}
}

double GetRandomNumber(double i, double j) //This function generates a random number between
{
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	default_random_engine generator(seed);
	uniform_real_distribution<double> distribution(i, j);
	//cout<<"returning "<<(double(distribution(generator)))<<"\n";
	return double(distribution(generator));
}

void SwapForVector(int i, int j, vector<int> cities)
{
	vector<int>::iterator it = cities.begin();
	int temp = *(it + i);
	*(it + i) = *(it + j);
	*(it + j) = temp;
}


double GetProbability(int difference, double temperature) //This function finds the probability of how bad the new solution is
{
	return exp(-1 * difference / temperature);
}

void PrintPath(vector<int> path)
{
	int length = path.size();
	for (int i = 0; i < length; i++)
	{
		cout << path[i] + 1;
		if (i != length - 1)
		{
			cout << " -> ";
		}
	}
	cout << "\n";
}

int main()
{
	// Zmienne czasu
	LARGE_INTEGER freq, start, end;


	vector<string> lines = ReadFileToArrayLines();				// Czytanie pliku z grafem 
	numberVertics = NumberVerticsFromArrayLines(lines);			// Liczba wierzcho³ków grafu

	// Inicjalizacja tablicy przechowuj¹cej wagi przejazdu pomiêdzy miastami
	graph = new int* [numberVertics];
	for (int i = 0; i < numberVertics; ++i)
		graph[i] = new int[numberVertics];

	// Inicjalizacja tablic przechowuj¹cej kolejnoœæ odwiedzanych miast
	citiesOrderOpenCL = new int[numberVertics];		

	//Inicjalizacja tablicy przekazywanje do bufora kernela
	graphOpenCL = new cl_int[numberVertics * numberVertics];

	// Czytanie grafu i zapisywanie do tablicy graph
	ReadMatrix(lines);

	InitNearestNeighbourTour(); //pocz¹tkowe ustawienie kolejnoœci odwiedzanych miast i zapis ich do tablicy citiesOrderOpenCL
	InitCitiesOrderOpenCL();
	InitCitiesOrder();

	// Pocz¹tkowa wartoœæ d³ugoœci œcie¿ki
	int bestTourLengthLoop = GetTourLength(citiesOrder);


	int globalMini = INT_MAX;
	if (globalMini > bestTourLengthLoop)
		globalMini = bestTourLengthLoop;


	double temperature = INIT_TEMPERATURE; //Initial Temperature


	int numberLoop = numberVertics* (numberVertics - 1);



	//Konfiguracja OpenCL
	cl_int status; // CL_SUCCESS je¿eli ok, kod b³êdu wpp.
	cl_uint num_platforms = 0;
	// SprawdŸ liczbê platform OpenCL
	status = clGetPlatformIDs(0, NULL, &num_platforms);
	if (num_platforms == 0 || status != CL_SUCCESS) { return EXIT_FAILURE; }
	// Pobierz identyfikatory platform
	std::vector<cl_platform_id> platforms(num_platforms);
	// Drugi raz clGetPlatformIDs, inne parametry
	status = clGetPlatformIDs(num_platforms, &platforms.front(), NULL);


	// Szukamy platformy z odpowiednim typem urz¹dzenia
	const cl_device_type kDeviceType = CL_DEVICE_TYPE_GPU;
	cl_device_id device = NULL; // Wystarczy nam pierwsze z brzegu urz¹dzenie
	cl_platform_id platform = NULL;
	bool found = false;
	for (cl_uint i = 0; i < num_platforms && !found; ++i) {
		cl_uint count = 0;
		status = clGetDeviceIDs(platforms[i], kDeviceType, 1, &device, &count);
		if (count == 1) { // OK, znaleŸliœmy
			platform = platforms[i];
			found = true;
		}
	}

	char* profile = NULL;
	size_t size;
	clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, NULL, profile, &size); // get size of profile char array
	profile = (char*)malloc(size);
	clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, size, profile, NULL); // get profile char array
	std::cout << profile << std::endl;


	// Tworzymy kontekst obliczeniowy
	const cl_context_properties prop[] = { // Parametry okreœlaj¹ce kontekst
	CL_CONTEXT_PLATFORM, (cl_context_properties)platform, 0 }; // 0 -> znacznik koñca listy parametrów

	cl_context context = clCreateContextFromType(prop, kDeviceType, NULL, NULL, &status);
	if (status != CL_SUCCESS) {
		std::cout << "Problem z tworzeniem kontekstu obliczeniowego. Kod b³êdu: "
			<< status << std::endl;
	}
	// Tworzymy kolejkê poleceñ OpenCL
	cl_command_queue cmd_queue = clCreateCommandQueue(context, device, 0, &status);

	FILE* fp;
	char* source_str;
	size_t source_size;

	fp = fopen("D:\\DanielPetrykowski\\Documents\\studia\\PORR\\PORR\\PORRVisualStudio\\vector_add_kernel.cl", "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	// Tworzymy obiekt programu OpenCL
	cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source_str,
		(const size_t*)&source_size, NULL);
	status = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
	if (status != CL_SUCCESS) {
		char log[1024] = {};
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
			1024, log, NULL);
		std::cout << "build log:\n" << log << std::endl;
		return EXIT_FAILURE;
	}

	
	cl_int* tourLengthResult = new cl_int[numberLoop];
	cl_int* citiesOrderOutResult = new cl_int[numberLoop * numberVertics];
	cl_uint2 random;


	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);

	// Start algorytmu symulowanego wyrza¿ania
	while (temperature > ABSOLUTE_TEMPERATURE)
	{
		int difference;

/*		std::cout << " in city: ";
		for (int i = 0;i < numberVertics; i++) {
			std::cout << citiesOrderOpenCL[i] << " ";
		}
		std::cout << std::endl <<std::endl;*/


		cl_kernel kernel = clCreateKernel(program, "loop", NULL);
		// Alokacja buforów na dane wejœciowe i wyniki obliczeñ kernela
				// Bufor na macierz przechowuj¹ca dane odleg³oœci pomiêdzy miastami
		cl_mem graphBuffor = clCreateBuffer(context, CL_MEM_READ_ONLY |
			CL_MEM_COPY_HOST_PTR,
			sizeof(cl_int) * numberVertics * numberVertics, graphOpenCL, NULL);
		cl_mem citiesOrderInBuffor = clCreateBuffer(context, CL_MEM_READ_ONLY |
			CL_MEM_COPY_HOST_PTR,
			sizeof(cl_int) * numberVertics, citiesOrderOpenCL, NULL);
				// Buffor na dane wyjœciowe - nowa d³ugoœæ œcie¿ki
		cl_mem tourLengthBuffor = clCreateBuffer(context, CL_MEM_READ_WRITE,
			sizeof(cl_int) * numberLoop, NULL, NULL);
				// Bufor na dane wyjœciowe - nowa kolejnoœæ odwiedzanych miast
		cl_mem citiesOrderOutBuffor = clCreateBuffer(context, CL_MEM_READ_WRITE,
			sizeof(cl_int) * numberLoop * numberVertics, NULL, NULL);

		
		random.x = GetRandomNumber(0, 5000);
		random.y = GetRandomNumber(0, 5000);

		// Przeka¿ parametry dla kernela
		clSetKernelArg(kernel, 0, sizeof(cl_int), (void*)&numberVertics);
		clSetKernelArg(kernel, 1, sizeof(cl_uint2), (void*)&random);
		clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&citiesOrderInBuffor);
		clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&graphBuffor);
		clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*)&tourLengthBuffor);
		clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&citiesOrderOutBuffor);

		const int dimensions = 1;
		// Definiujemy przestrzeñ indeksowania
		size_t global_work_size[dimensions] = { numberLoop };
		cl_event event;
		status = clEnqueueNDRangeKernel(cmd_queue, kernel, dimensions, NULL,
			global_work_size, NULL, 0, NULL, &event);
		clWaitForEvents(1, &event);
		// Kopiujemy wynik z pamiêci urz¹dzenia do pamiêci hosta
		status = clEnqueueReadBuffer(cmd_queue, tourLengthBuffor, CL_TRUE,
			0, numberLoop * sizeof(cl_int), tourLengthResult, 0, NULL, NULL
		);
		status = clEnqueueReadBuffer(cmd_queue, citiesOrderOutBuffor, CL_TRUE,
			0, numberLoop * numberVertics * sizeof(cl_int), citiesOrderOutResult, 0, NULL, NULL
		);

		// Minimalna d³ugoœæ œcie¿ki 



		//for (int rs = 0; rs < numberLoop; rs++)
		//{
		//	int position1 = 0, position2 = 0;

		//	position1 = int(GetRandomNumber(0, numberVertics));
		//	position2 = int(GetRandomNumber(0, numberVertics));
		//	while (position1 == position2 or ((position1 > numberVertics - 1) or (position2 > numberVertics - 1)))
		//	{
		//		position1 = int(GetRandomNumber(0, numberVertics));
		//		position2 = int(GetRandomNumber(0, numberVertics));
		//	}

		//	SwapForVector(position1, position2, copyCitiesOrder[rs]);
		//	vector<int>::iterator it2 = copyCitiesOrder[rs].begin();
		//	if (position2 > position1)
		//		random_shuffle(it2 + position1, it2 + position2);
		//	newTourLength[rs] = GetTourLength(copyCitiesOrder[rs]);

		//}

		int miniTourLengthLoopId = 0;
		for (int i = 1; i < numberLoop; i++)
		{
			if (tourLengthResult[miniTourLengthLoopId] > tourLengthResult[i])
				miniTourLengthLoopId = i;
		}


		if (globalMini > tourLengthResult[miniTourLengthLoopId])
		{
			globalMini = tourLengthResult[miniTourLengthLoopId];
		}

		difference = tourLengthResult[miniTourLengthLoopId] - bestTourLengthLoop;
		double prob = GetProbability(difference, temperature);
		double rand = GetRandomNumber(0, 1);

		//for (int i = 0;i < numberLoop;i++) {
		//	for (int j = 0;j < numberVertics;j++) {
		//		std::cout << citiesOrderOutResult[i * numberVertics + j];
		//	}
		//	std::cout << "  ->  " << tourLengthResult[i];
		//	std::cout << std::endl;
		//}

		if (difference < 0 or (difference > 0 and prob > rand))
		{
			int id = 0;
		//	std::cout << "out city: ";
			for (int i = miniTourLengthLoopId * numberVertics; i < (miniTourLengthLoopId + 1) * numberVertics; i++) {
				citiesOrderOpenCL[id] = citiesOrderOutResult[i];
			//	std::cout << citiesOrderOutResult[i] << " ";
				id++;
			}
		//	std::cout << std::endl;

			InitCitiesOrder();
			bestTourLengthLoop = difference + bestTourLengthLoop;
		}


		temperature = temperature * COOLING_RATE;


		clFinish(cmd_queue); // Czekamy na zakoñczenie zadañ
// Zwalniamy pamiêæ
		clReleaseKernel(kernel);
		clReleaseMemObject(tourLengthBuffor);
		clReleaseMemObject(citiesOrderOutBuffor);
		clReleaseMemObject(citiesOrderInBuffor);
		clReleaseMemObject(graphBuffor);

	}

	clReleaseProgram(program);
	clReleaseCommandQueue(cmd_queue);
	clReleaseContext(context);

	QueryPerformanceCounter(&end);
	double durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;

	cout << "Processing time " << durationInSeconds << "\n";
	cout << "The best solution is " << bestTourLengthLoop << "\n";
	cout << "The best path found:\n";
	PrintPath(citiesOrder);
	cout << "The minimum solution found is  " << globalMini << "\n";
	return 0;
}
