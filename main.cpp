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

using namespace std;

//-----------------------------------------------------------------------
const double INIT_TEMPERATURE = 99999999999999999999999999999999999999999.0;
const double COOLING_RATE = 0.9;
const double ABSOLUTE_TEMPERATURE = 0.00000001;
const string FAIL_PATH = "./netTest/net1.mat";
const int NUM_THREADS = 1;


//----------------------------------------------------------------------

int** graph;
int numberVertics;
vector<int> citiesOrder;
vector<int> finalOrder;
int *arr;// arr is the array that stores the City order
omp_lock_t my_lock;

//Functions
vector<string> ReadFileToArrayLines();
int NumberVerticsFromArrayLines(vector<string> lines);
void ReadMatrix(vector<string> lines);
int Distance(int city1,int city2);
void SwapArr(int i,int j);
int GetTourLength(vector<int> cities);
void InitNearestNeighbourTour();
double GetRandomNumber(double i,double j);
void SwapForVector(int i,int j, vector<int> cities);
double GetProbability(int difference,double temperature);
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
        while(getline(file, tp))
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
    while(getline(lineStream, element, ' '))
    {
        elements.push_back(element);
    }
    numberVertics = atoi(elements[1].c_str());
    cout << "Number of vertices: " << numberVertics << "\n";
    return numberVertics;
}

void ReadMatrix(vector<string> lines)
{
    if(lines[numberVertics+1].compare("*Matrix") == 0)
    {
        int numberLine = numberVertics + 2;
        int row;

        int i;
        for(i=0; i<numberVertics; i++)
        {
            cout << "\t" << i+1;
        }
        cout << "\n";

        for(row = 0; row<numberVertics; row++)
        {
            string edgeValue;
            int col = 0;

            stringstream lineStream(lines[numberLine + row]);
            cout << row + 1 << "\t";

            while(getline(lineStream, edgeValue, ' '))
            {
                if(edgeValue.compare("") != 0)
                {
                    int value = atoi(edgeValue.c_str());
                    graph[row][col] = value;
                    cout << graph[row][col] << "\t";
                    col ++;
                }
            }
            cout << "\n";
        }
    }
}


int Distance(int city1,int city2)
{
    return graph[city1][city2];
}

void SwapArr(int i,int j)
{
    int temp=arr[i];
    arr[i]=arr[j];
    arr[j]=temp;
}

int GetTourLength(vector<int> cities) //This function returns the tour length of the current order of cities
{
    vector<int>:: iterator it;

    it=cities.begin();
    int pcity1=*it;
    int ncity;
    // cout<<"\n the pcity is "<<pcity1<<"\n";
    int tourLength=Distance(0,pcity1);
    for(it=cities.begin()+1; it!=cities.end(); it++)
    {
        ncity=*it;
        tourLength+=Distance(pcity1,ncity);

        pcity1=ncity;
    }

    tourLength+=Distance(pcity1,0); //adding the distance back to the source path
    return tourLength;
}

void InitNearestNeighbourTour() //puts the nearestNeighbourTour in the vector cities
{
    int numCities=numberVertics,i,j;

    for(i=0; i<numCities; i++)
    {
        arr[i]=i;
        //cout<<"arr is "<<i<<"\n";
    }
    int best,bestIndex;
    for(i=1; i<numCities; i++)
    {
        best=INT_MAX;
        for(j=i; j<numCities; j++)
        {
            if(Distance(i-1,j)<best)
            {
                best=Distance(i,j);
                bestIndex=j;
            }
        }
        SwapArr(i,bestIndex);
    }
}

double GetRandomNumber(double i,double j) //This function generates a random number between
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator (seed);
    uniform_real_distribution<double> distribution (i,j);
    //cout<<"returning "<<(double(distribution(generator)))<<"\n";
    return double(distribution(generator));
}

void SwapForVector(int i,int j, vector<int> cities)
{
    vector<int>::iterator it=cities.begin();
    int temp=*(it+i);
    *(it+i)=*(it+j);
    *(it+j)=temp;
}


double GetProbability(int difference,double temperature) //This function finds the probability of how bad the new solution is
{
    return exp(-1*difference/temperature);
}

void PrintPath(vector<int> path)
{
    int length = path.size();
    for(int i = 0; i<length; i++)
    {
        cout << path[i] + 1;
        if(i!=length-1)
        {
            cout << " -> ";
        }
    }
    cout << "\n";
}

int main()
{
    LARGE_INTEGER freq, start, end;

    vector<string> lines = ReadFileToArrayLines();
    numberVertics = NumberVerticsFromArrayLines(lines);

    graph = new int*[numberVertics];
    for(int i = 0; i < numberVertics; ++i)
        graph[i] = new int[numberVertics];
    arr = new int[numberVertics];

    ReadMatrix(lines);

    InitNearestNeighbourTour(); //start with a decent point
    int numCities=numberVertics;
    citiesOrder.clear();
    finalOrder.clear();
    for(int i=0; i<numCities; i++)
    {
        citiesOrder.push_back(arr[i]);
    }
    int bestTourLength=GetTourLength(citiesOrder);


    int mini=INT_MAX;
    if(mini > bestTourLength )
        mini=bestTourLength;


    {
        double temperature;
        vector<int>::iterator it,it2;
        int position1=0,position2=0;
        int newTourLength,difference;

        vector<int> copyCitiesOrder = citiesOrder;

        {
            printf("Program is executed on %d threads.\n",
                   omp_get_num_threads());
            QueryPerformanceFrequency(&freq);
            QueryPerformanceCounter(&start);
        }

        for(int rs=0; rs<numberVertics*(numberVertics-1); rs++)
        {
            temperature = INIT_TEMPERATURE; //Initial Temperature
            int bestTourLengthLoop = bestTourLength;
            vector<int> bestOrderLoop;
            bestOrderLoop.clear();

            while(temperature > ABSOLUTE_TEMPERATURE)
            {
                position1=int(GetRandomNumber(0,numberVertics));
                position2=int(GetRandomNumber(0,numberVertics));
                while(position1==position2 or( (position1>numberVertics-1) or (position2>numberVertics-1)))
                {
                    position1=int(GetRandomNumber(0,numberVertics));
                    position2=int(GetRandomNumber(0,numberVertics));
                }

                SwapForVector(position1,position2,copyCitiesOrder);
                it2=copyCitiesOrder.begin();
                if(position2 > position1)
                    random_shuffle(it2+position1,it2+position2);
                newTourLength=GetTourLength(copyCitiesOrder);


                if(mini > newTourLength )
                {
                    if(mini > newTourLength )
                        mini=newTourLength;
                }

                difference=newTourLength-bestTourLengthLoop;
                double prob = GetProbability(difference,temperature);
                double rand = GetRandomNumber(0,1);

                if(difference <0 or (difference >0 and prob > rand))
                {
                    bestOrderLoop.clear();

                    for(it=copyCitiesOrder.begin(); it!=copyCitiesOrder.end(); it++)
                    {
                        bestOrderLoop.push_back(*it);
                    }
                    bestTourLengthLoop=difference+bestTourLengthLoop;
                }

                temperature=temperature*COOLING_RATE;
            }

            if(bestTourLength>bestTourLengthLoop)
            {
                if(bestTourLength>bestTourLengthLoop)
                {
                    bestTourLength = bestTourLengthLoop;
                    finalOrder.clear();
                    for(it=bestOrderLoop.begin(); it!=bestOrderLoop.end(); it++)
                    {
                        finalOrder.push_back(*it);
                    }
                }
            }

            random_shuffle(copyCitiesOrder.begin(),copyCitiesOrder.end());
        }
    }

    QueryPerformanceCounter(&end);
    double durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;

    cout<<"Processing time " << durationInSeconds <<"\n";
    cout<<"The best solution is "<<bestTourLength<<"\n";
    cout << "The best path found:\n";
    PrintPath(finalOrder);
    cout<<"The minimum solution found is  "<<mini<<"\n";
    return 0;
}
