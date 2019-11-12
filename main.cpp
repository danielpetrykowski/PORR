#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <fstream>
#include <math.h>
#include <random>
#include <sstream>
#include <algorithm>
#include <chrono>


using namespace std;

int** graph;
int numberVertics;
vector<int> cities;
vector<int> finalOrder;
int *arr;// arr is the array that stores the City order

//wczytanie grafu

    vector<string> readFileToArrayLines(){
        // Read file
        vector<string> lines;
        fstream file;
        file.open("net1.mat", ios::in);
        if (file.is_open()){
            string tp;
            while(getline(file, tp)){
                lines.push_back(tp);
            }
            file.close();
        }
        return lines;
    }

    int numberVerticsFromArrayLines(vector<string> lines){
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

    void ReadMatrix(vector<string> lines){
        if(lines[numberVertics+1].compare("*Matrix") == 0)
        {
            int numberLine = numberVertics + 2;
            int row;

            int i;
            for(i=0;i<numberVertics;i++)
            {
                cout << "\t" << i+1;
            }
            cout << "\n";

            for(row = 0; row<numberVertics;row++){
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


int distance(int city1,int city2)
{
    return graph[city1][city2];
}

void swapArr(int i,int j)
{
    int temp=arr[i];
    arr[i]=arr[j];
    arr[j]=temp;
}

int getTourLength() //This function returns the tour length of the current order of cities
{
//    cout<<"the cuurentcities are \n";
    vector<int>:: iterator it;
//    for(it=cities.begin();it!=cities.end();it++)
//    {
//        cout<<*it<<" ";
//    }
//    cout<<"\n";


    it=cities.begin();
    int pcity1=*it,ncity;
    cout<<"\n the pcity is "<<pcity1<<"\n";
    int tourLength=distance(0,pcity1);
    for(it=cities.begin()+1;it!=cities.end();it++)
    {
        ncity=*it;
        tourLength+=distance(pcity1,ncity);

        pcity1=ncity;
    }

    tourLength+=distance(pcity1,0); //adding the distance back to the source path
    return tourLength;
}

int getNearestNeighbourTour() //puts the nearestNeighbourTour in the vector cities
{
    int numCities=numberVertics,i,j;

    for(i=0;i<numCities;i++)
    {
        arr[i]=i;
        //cout<<"arr is "<<i<<"\n";
    }
    int best,bestIndex;
    for(i=1;i<numCities;i++)
    {
        best=INT_MAX;
        for(j=i;j<numCities;j++)
        {
            if(distance(i-1,j)<best)
            {
                best=distance(i,j);
                bestIndex=j;
            }
        }
        swapArr(i,bestIndex);
    }
    cities.clear();
    for(i=1;i<numCities;i++)
    {
        cities.push_back(arr[i]);
        finalOrder.push_back(arr[i]);
    }

    int nearestNeighbourTourLength=getTourLength();
    return nearestNeighbourTourLength;
}

double getRandomNumber(double i,double j) //This function generates a random number between
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator (seed);
    uniform_real_distribution<double> distribution (i,j);
    //cout<<"returning "<<(double(distribution(generator)))<<"\n";
    return double(distribution(generator));
}

void swap2(int i,int j)
{
    vector<int>::iterator it=cities.begin();
    int temp=*(it+i);
    *(it+i)=*(it+j);
    *(it+j)=temp;

}

double getProbability(int difference,double temperature) //This function finds the probability of how bad the new solution is
{
    return exp(-1*difference/temperature);
}

int main()
{
    vector<int>::iterator it,it2;
    vector<string> lines = readFileToArrayLines();
    numberVertics = numberVerticsFromArrayLines(lines);

    graph = new int*[numberVertics];
    for(int i = 0; i < numberVertics; ++i)
        graph[i] = new int[numberVertics];
    arr = new int[numberVertics];

    ReadMatrix(lines);

        //Generate the initial city tour and get the  nearestNeighbourTourLength
    int bestTourLength=getNearestNeighbourTour(); //start with a decent point

    int mini=INT_MAX;
    if(mini > bestTourLength ) mini=bestTourLength;
    double temperature,coolingRate=0.9,absoluteTemperature=0.00001,probability;
    int position1=0,position2=0;
    int newTourLength,difference;
    std::fstream fs;
    fs.open ("tspResults.txt", std::fstream::in | std::fstream::out );

    for(int rs=0;rs<100;rs++)
    {
        temperature=99999999999999999999999999999999999999999.0; //Initial Temperature
        //cout<<"doing rs "<<rs<<"\n";
        fs<<"[";
        while(temperature > absoluteTemperature)
        {
            //cout<<"hi";


            position1=int(getRandomNumber(0,numberVertics-1));
            position2=int(getRandomNumber(0,numberVertics-1));
            while(position1==position2 or( (position1>numberVertics-2) or (position2>numberVertics-2)))
            {
                position1=int(getRandomNumber(0,numberVertics-2));
                position2=int(getRandomNumber(0,numberVertics-2));

            }
            //cout<<"position1 is "<<position1<<" position2 is "<<position2<<"\n";
            swap2(position1,position2);
            it2=cities.begin();
            if(position2 > position1)
                random_shuffle(it2+position1,it2+position2);
            newTourLength=getTourLength();
            if(mini > newTourLength ) mini=newTourLength;
            fs<<newTourLength<<",";
            cout<<"current tour length is "<<newTourLength<<" n bestTourLength is "<<bestTourLength<<"\n\n";
            difference=newTourLength-bestTourLength;

            if(difference <0 or (difference >0 and  getProbability(difference,temperature) > getRandomNumber(0,1)))
            {
                finalOrder.clear();

                for(it=cities.begin();it!=cities.end();it++)
                {
                    finalOrder.push_back(*it);
                }
                bestTourLength=difference+bestTourLength;
            }
            temperature=temperature*coolingRate;

        }
        fs<<"]\n";
        random_shuffle(cities.begin(),cities.end());
        //cout<<"the best solution is "<<bestTourLength<<"\n";
      }
      fs.close();
    cout<<"the best solution is "<<bestTourLength<<"\n";
    cout<<" the minimum solution found is  "<<mini<<"\n";
    return 0;
}
