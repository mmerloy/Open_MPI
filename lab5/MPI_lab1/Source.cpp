//------------Марина Алина ПИН-34 2 вариант------------------------
//Задание . Реализуйте на основе технологии MPI многопоточную программу с использованием
//групп процессов и коммуникаторов в соответствии с вариантом задания. Проверьте корректность работы программы. 
//Вариант 2. Реализуйте тип «длинное целое».Напишите программу, которая осуществляет умножение A целых чисел заданной длины методом Тоома - Кука.


#include <stdio.h>
#include <mpi.h>
#include <iostream>
#include <windows.h>
#include <time.h>
#include <memory>
#include <vector>
#include <string>

using namespace std;

void calcToomCook(int16_t*, int16_t*, int16_t*, int);
void multiplyNormal(int16_t*, int16_t*, int, int16_t*);
void multiplyToomCook3(int16_t*, int16_t*, int, int16_t*);
void doCarry(int16_t*, int);
string display(int16_t*, int16_t*, int16_t*, int);

void calcToomCook(int16_t* A, int16_t* B, int16_t* Z, int size)
{
	int16_t* a = new int16_t[size];
	int16_t* b = new int16_t[size];
	int i;

	for (i = 0; i < size; i++)
	{
		a[i] = A[i];
		b[i] = B[i];
	}
	multiplyToomCook3(a, b, size, Z);
	doCarry(Z, size * 2);
	//delete a, b;
}

void multiplyNormal(int16_t* a, int16_t* b, int size, int16_t* z)
{
	int i, j;
	for (i = 0; i < size * 2; i++)
	{
		z[i] = 0;
	}

	for (j = 0; j < size; j++)
	{
		for (i = 0; i < size; i++)
		{
			z[j + i] += a[i] * b[j];
		}
	}
}

void multiplyToomCook3(int16_t* a, int16_t* b, int size, int16_t* z)
{
	int16_t* a0 = &a[0];							// Multiplicand / right side array point16_ter
	int16_t* a1 = &a[size / 3];						// Multiplicand / central array point16_ter
	int16_t* a2 = &a[size * 2 / 3];					// Multiplicand / left side array point16_ter
	int16_t* b0 = &b[0];							// Multiplier / right side array point16_ter
	int16_t* b1 = &b[size / 3];						// Multiplier / central array point16_ter
	int16_t* b2 = &b[size * 2 / 3];					// Multiplier / left side array point16_ter
	int16_t* c0 = &z[(size / 3) * 0];				// c0     
	int16_t* c2 = &z[(size / 3) * 2];				// c2     
	int16_t* c4 = &z[(size / 3) * 4];				// c4    

	int16_t* c1 = new int16_t[(size / 3) * 2];		// c1    
	int16_t* c3 = new int16_t[(size / 3) * 2];		// c3     
	int16_t* a_m2 = new int16_t[size / 3];			// a( -2) 
	int16_t* a_m1 = new int16_t[size / 3];			// a( -1) 
	int16_t* a_0 = new int16_t[size / 3];			// a(  0) 
	int16_t* a_1 = new int16_t[size / 3];			// a(  1) 
	int16_t* a_inf = new int16_t[size / 3];			// a(inf) 
	int16_t* b_m2 = new int16_t[size / 3];			// b( -2) 
	int16_t* b_m1 = new int16_t[size / 3];			// b( -1) 
	int16_t* b_0 = new int16_t[size / 3];			// b(  0) 
	int16_t* b_1 = new int16_t[size / 3];			// b(  1)
	int16_t* b_inf = new int16_t[size / 3];			// b(inf) 
	int16_t* c_m2 = new int16_t[(size / 3) * 2];	// c( -2)
	int16_t* c_m1 = new int16_t[(size / 3) * 2];	// c( -1)
	int16_t* c_0 = new int16_t[(size / 3) * 2];		// c(  0) 
	int16_t* c_1 = new int16_t[(size / 3) * 2];		// c(  1) 
	int16_t* c_inf = new int16_t[(size / 3) * 2];	// c(inf)

	int i;
	if (size <= 9)
	{
		multiplyNormal(a, b, size, z);
		return;
	}
	// ==== a(-2) = 4 * a2 - 2 * a1 + a0, b(-2) = 4 * b2 - 2 * b1 + b0
	for (i = 0; i < size / 3; i++)
	{
		a_m2[i] = (a2[i] << 2) - (a1[i] << 1) + a0[i];
		b_m2[i] = (b2[i] << 2) - (b1[i] << 1) + b0[i];
	}
	// ==== a(-1) = a2 - a1 + a0, b(-1) = b2 - b1 + b0
	for (i = 0; i < size / 3; i++)
	{
		a_m1[i] = a2[i] - a1[i] + a0[i];
		b_m1[i] = b2[i] - b1[i] + b0[i];
	}
	// ==== a(0) = a0, b(0) = b0
	for (i = 0; i < size / 3; i++) {
		a_0[i] = a0[i];
		b_0[i] = b0[i];
	}
	// ==== a(1) = a2 + a1 + a0, b(1) = b2 + b1 + b0
	for (i = 0; i < size / 3; i++) {
		a_1[i] = a2[i] + a1[i] + a0[i];
		b_1[i] = b2[i] + b1[i] + b0[i];
	}
	// ==== a(inf) = a2, b(inf) = b2
	for (i = 0; i < size / 3; i++) {
		a_inf[i] = a2[i];
		b_inf[i] = b2[i];
	}
	// ==== c(-2) = a(-2) * b(-2)
	multiplyToomCook3(a_m2, b_m2, size / 3, c_m2);
	// ==== c(-1) = a(-1) * b(-1)
	multiplyToomCook3(a_m1, b_m1, size / 3, c_m1);
	// ==== c(0) = a(0) * b(0)
	multiplyToomCook3(a_0, b_0, size / 3, c_0);
	// ==== c(1) = a(1) * b(1)
	multiplyToomCook3(a_1, b_1, size / 3, c_1);
	// ==== c(inf) = a(inf) * b(inf)
	multiplyToomCook3(a_inf, b_inf, size / 3, c_inf);
	// ==== c4 = 6 * c(inf) / 6
	for (i = 0; i < (size / 3) * 2; i++)
		c4[i] = c_inf[i];
	// ==== c3 = -c(-2) + 3 * c(-1) - 3 * c(0) + c(1) + 12 * c(inf) / 6
	for (i = 0; i < (size / 3) * 2; i++)
	{
		c3[i] = -c_m2[i];
		c3[i] += (c_m1[i] << 1) + c_m1[i];
		c3[i] -= (c_0[i] << 1) + c_0[i];
		c3[i] += c_1[i];
		c3[i] += (c_inf[i] << 3) + (c_inf[i] << 2);
		c3[i] /= 6;
	}
	// ==== c2 = 3 * c(-1) - 6 * c(0) + 3 * c(1) - 6 * c(inf) / 6
	for (i = 0; i < (size / 3) * 2; i++) {
		c2[i] = (c_m1[i] << 1) + c_m1[i];
		c2[i] -= (c_0[i] << 2) + (c_0[i] << 1);
		c2[i] += (c_1[i] << 1) + c_1[i];
		c2[i] -= (c_inf[i] << 2) + (c_inf[i] << 1);
		c2[i] /= 6;
	}
	// ==== c1 = c(-2) - 6 * c(-1) + 3 * c(0) + 2 * c(1) - 12 * c(inf) / 6
	for (i = 0; i < (size / 3) * 2; i++) {
		c1[i] = c_m2[i];
		c1[i] -= (c_m1[i] << 2) + (c_m1[i] << 1);
		c1[i] += (c_0[i] << 1) + c_0[i];
		c1[i] += (c_1[i] << 1);
		c1[i] -= (c_inf[i] << 3) + (c_inf[i] << 2);
		c1[i] /= 6;
	}
	// ==== c0 = 6 * c(0) / 6
	for (i = 0; i < (size / 3) * 2; i++)
		c0[i] = c_0[i];
	// ==== z = c4 * x^4 + c3 * x^3 + c2 * x^2 + c1 * x + c0
	for (i = 0; i < (size / 3) * 2; i++) z[i + size / 3] += c1[i];
	for (i = 0; i < (size / 3) * 2; i++) z[i + (size / 3) * 3] += c3[i];

	delete c1, c3, a_m2, a_m1, a_0, a_1, a_inf, b_m2, b_m1;
	delete b_0, b_1, b_inf, c_m2, c_m1, c_0, c_1, c_inf;
}

void doCarry(int16_t* a, int size) {
	int16_t cr;
	int i;
	cr = 0;
	for (i = 0; i < size; i++) {
		a[i] += cr;
		if (a[i] < 0) {
			cr = -(-(a[i] + 1) / 10 + 1);
		}
		else {
			cr = a[i] / 10;
		}
		a[i] -= cr * 10;
	}

	if (cr != 0) printf("[ OVERFLOW!! ] %d\n", cr);
}

string display(int16_t* a, int16_t* b, int16_t* z, int size)
{
	string result = "";
	int i;
	int aLen = size, bLen = size, zLen = size * 2;
	while (a[aLen - 1] == 0) if (a[aLen - 1] == 0) aLen--;
	while (b[bLen - 1] == 0) if (b[bLen - 1] == 0) bLen--;
	while (z[zLen - 1] == 0) if (z[zLen - 1] == 0) zLen--;
	// a 
	result += "a =\t";
	for (i = aLen - 1; i >= 0; i--) {
		result += to_string(a[i]);
		if ((aLen - i) % 10 == 0) result += " ";
		if ((aLen - i) % 50 == 0) result += "\n\t";
	}
	result += "\n";
	// b 
	result += "b =\t";
	for (i = bLen - 1; i >= 0; i--) {
		result += to_string(b[i]);
		if ((bLen - i) % 10 == 0) result += " ";
		if ((bLen - i) % 50 == 0) result += "\n\t";
	}
	result += "\n";
	// z 
	result += "z =\t";
	for (i = zLen - 1; i >= 0; i--) {
		result += to_string(z[i]);
		if ((zLen - i) % 10 == 0) result += " ";
		if ((zLen - i) % 50 == 0) result += "\n\t";
	}
	result += "\n\n";

	return result;
}

void removeRanksNumbers(vector<int>& v, int numtasks)
{
	for (int i = v.size() - 1; i > 0; i--)
	{
		//if (i % 2 != 0)//				//
		{
			v.erase(v.begin() + i);
		}
	}
}

string getNumber(int16_t* number, int size)
{
	string result = "";
	int i;
	int aLen = size;
	while (number[aLen - 1] == 0) if (number[aLen - 1] == 0) aLen--;
	for (i = aLen - 1; i >= 0; i--) {
		result += to_string(number[i]);
		if ((aLen - i) % 10 == 0) result += " ";
		if ((aLen - i) % 50 == 0) result += "\n\t";
	}
	result += "\n";
	return result;
}

void copyNumbers(int16_t* first, int16_t* second, int size)
{
	for (int i = 0; i < size; i++)
	{
		first[i] = second[i];
	}
}

int main(int* argc, char** argv)
{
	int size = 9;
	int i;

	int numtasks, rank;

	MPI_Status status;
	MPI_Init(argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	int currentsize = 3 * size * pow(3, numtasks);
	MPI_Datatype MPI_LONG_NUMBER;
	MPI_Type_contiguous(currentsize, MPI_SHORT, &MPI_LONG_NUMBER);
	MPI_Type_commit(&MPI_LONG_NUMBER);

	MPI_Group MY_GROUP;
	MPI_Comm MY_COMM;
	MPI_Comm_group(MPI_COMM_WORLD, &MY_GROUP);
	MPI_Comm_create(MPI_COMM_WORLD, MY_GROUP, &MY_COMM);

	srand(time(NULL) + rank);

	int16_t* A = new int16_t[currentsize];
	int16_t* B = new int16_t[currentsize];
	int16_t* Z = new int16_t[currentsize];
	for (i = 0; i < size; i++)
	{
		A[i] = rand() % 10;
		B[i] = rand() % 10;
		Z[i] = 0;
	}
	for (i = size; i < currentsize; i++)
	{
		A[i] = 0;
		B[i] = 0;
		Z[i] = 0;
	}

	calcToomCook(A, B, Z, currentsize);

	cout << to_string(rank) + "\n" + display(A, B, Z, currentsize) + "\n\n";

	vector<int> procNumbers;
	for (int i = 0; i < numtasks; i++)
	{
		procNumbers.push_back(i);
	}
	Sleep(100);

	int iteration = 1;

	while (procNumbers.size() != 1)
	{
		MPI_Group_incl(MY_GROUP, procNumbers.size(), procNumbers.data(), &MY_GROUP);

		MPI_Comm_create(MY_COMM, MY_GROUP, &MY_COMM);

		for (int i = 0; i < procNumbers.size(); i++)
		{
			if (procNumbers[i] == rank)
			{
				if (i % 2 == 0)
				{
					copyNumbers(A, Z, currentsize);
				}
				else
				{
					MPI_Send(Z, 1, MPI_LONG_NUMBER, procNumbers[i - 1] / iteration, 0, MY_COMM);
				}
				break;
			}
		}
		Sleep(100);
		for (int i = 0; i < procNumbers.size(); i++)
		{
			if (procNumbers[i] == rank)
			{
				if (i % 2 == 0)
				{
					MPI_Recv(B, 1, MPI_LONG_NUMBER, procNumbers[i + 1] / iteration, 0, MY_COMM, &status);
					calcToomCook(A, B, Z, currentsize);
				}
			}
		}
		removeRanksNumbers(procNumbers, numtasks);

		iteration *= 2;
	}

	if (rank == 0)
	{
		Sleep(100);
		//cout << "----------------------------\n\nresult: " 
		//	+ getNumber(Z, currentsize)
		//	+ "\n----------------------------\n";
	}


	MPI_Group_free(&MY_GROUP);
	MPI_Comm_free(&MY_COMM);

	delete A, B, Z;
	MPI_Finalize();

	return 0;
}