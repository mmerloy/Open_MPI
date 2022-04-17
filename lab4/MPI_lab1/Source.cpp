//------------Марина Алина ПИН-34 2 вариант------------------------
//Реализуйте на основе технологии MPI многопоточную программу в соответствии с вариантом задания. 
//Найдите возможные способы проверки корректности работы программы. Оцените их возможные достоинства и недостатки.
//Проверьте корректность работы программы при помощи одного из них.
//Вариант 2. Реализуйте тип «полином».Напишите программу, которая осуществляет умножение A полиномов заданной степени.


#include <iostream>
#include <vector>
#include <random>
#include <time.h>
#include <string>  
#include <mpi.h>
#include <windows.h> //
#include <cstdlib> //

#define MAXSIZE 12

using namespace std;

string pm(int possibly_negative_value)
{
	string res;
	if (possibly_negative_value < 0)
		res = " - " + to_string(-possibly_negative_value);
	else
		res = " + " + to_string(possibly_negative_value);
	return res;
}

string pmx(int possibly_negative_value, int power)
{
	if (power == 0)
		return pm(possibly_negative_value);
	string res = pm(possibly_negative_value) + "x";
	if (power != 1)
		res += "^" + to_string(power);
	return res;
}

vector<int> multiplyPoly(vector<int> A, vector<int> B)
{
	vector<int> res_poly(A.size() + B.size() - 1, 0);

	for (int i = 0; i < A.size(); i++)
	{
		for (int j = 0; j < B.size(); j++)
		{
			res_poly[i + j] += A[i] * B[j];
		}
	}
	return res_poly;
}

void printPoly(vector<int> poly)
{
	const int a = poly.size() - 1;
	for (int i = poly.size() - 1; i > -1; i--)
	{
		if (poly[i] == 0)
		{
			continue;
		}
		if (i == a) // does not work
			// if 1x = x
			cout << poly[i] << "^" << i;
		else
			cout << pmx(poly[i], i);
	}
	cout << endl;
}

vector<int> createPoly(int poly_size)
{
	vector<int> new_poly(poly_size, 0);
	for (int i = 0; i < poly_size; i++)
	{
		new_poly[i] = rand() % 10 - 5;
	}
	return new_poly;
}

void send(vector<vector<int>>* polynomials, MPI_Datatype MPI_Poly, int which_pr);

void fillPoly(vector<vector<int>>* polynomials, int how_many_needed)
{
	int poly_size = MAXSIZE;
	vector<int> res_poly(MAXSIZE, 0);
	for (int i = 0; i < how_many_needed; i++)
		polynomials[0].push_back(createPoly(rand() % 6 + 1));
	for (int i = 0; i < polynomials[0].size(); i++)
	{
		polynomials[0][i].resize(MAXSIZE);
	}
}

int main(int* argc, char* argv[])
{

	srand(time(NULL));
	vector<vector<int>> polynomials;

	int* tmp_poly1 = new int[MAXSIZE];
	int* tmp_poly2 = new int[MAXSIZE];
	int size, rank;

	MPI_Init(argc, &argv);

	MPI_Status status;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Datatype MPI_Poly;
	MPI_Type_contiguous(MAXSIZE, MPI_INT, &MPI_Poly);
	MPI_Type_commit(&MPI_Poly);

	if (rank == 0)
	{
		int full_size = (size - 1) * 2; // refactoring needed
		fillPoly(&polynomials, full_size);
		for (int i = 0; i < full_size; i++)
			printPoly(polynomials[i]);
		for (int i = 1; i < size; i++)
		{
			send(&polynomials, MPI_Poly, i);
			Sleep(100);
		}
		if (polynomials.size() < 2)
		{
			//break;
		}
		//Sleep(100);
		cout << "Barrier 0";
		//MPI_Barrier(MPI_COMM_WORLD);
	}
	else
	{
		vector<int> res_poly(MAXSIZE, 0);
		cout << "rank " << rank << endl;
		MPI_Recv(tmp_poly1, 1, MPI_Poly, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		cout << "Recieved poly 1 on " << rank << endl;
		MPI_Recv(tmp_poly2, 1, MPI_Poly, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		cout << "Recieved poly 2 on " << rank << endl;
		res_poly = multiplyPoly(vector<int>(tmp_poly1, tmp_poly1 + MAXSIZE),
			vector<int>(tmp_poly2, tmp_poly2 + MAXSIZE)); // how did it work again?
		//MPI_Send(&res_poly.front(), 1, MPI_Poly, 0, 1, MPI_COMM_WORLD);
		cout << "result on " << rank << ":";
		printPoly(res_poly);
		cout << "Barrier " << rank;
		//MPI_Barrier(MPI_COMM_WORLD);

		//Sleep(100);
	}

	MPI_Finalize();
	return 0;
}

void send(vector<vector<int>>* polynomials, MPI_Datatype MPI_Poly, int which_pr)
{
	cout << "first: ";
	printPoly(polynomials[0].back());

	MPI_Send(&polynomials[0].back().front(), 1, MPI_Poly, which_pr, 1, MPI_COMM_WORLD);
	polynomials[0].pop_back();
	cout << "second: ";
	printPoly(polynomials[0].back());
	MPI_Send(&polynomials[0].back().front(), 1, MPI_Poly, which_pr, 1, MPI_COMM_WORLD);
	polynomials[0].pop_back();
}
