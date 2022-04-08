//------------Марина Алина ПИН-34 1 вариант------------------------
//Реализуйте функцию star, которая создаёт N+1 процессов (1 «центральный» и N «крайних»)
//и посылает сообщение центральному процессу, который посылает сообщение всем остальным процессам 
//и дожидается от них ответа, после чего это повторяется (всего M раз). После того, как все события получены,
//все процессы заканчивают работу.

#include <stdio.h>
#include <mpi.h>
#include "windows.h"
#include <iostream>
#include <iomanip>
#include <string>
#define M 4

using namespace std;

int main(int* argc, char** argv)
{
	int proc_quantity, rank;
	MPI_Init(argc, &argv);
	MPI_Status status;

	// Получаем количество процессов и их текущий ранг
	MPI_Comm_size(MPI_COMM_WORLD, &proc_quantity);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	srand(time(NULL) + rank);

	//как много процессов используется
	if (rank == 0)
		cout << "Num of all processes: " << proc_quantity << endl;

	//Proccesses cycle
	for (int loop = 0; loop < M; loop++)
	{
		int random_value = rand() % 100;
		if (rank == 0)
		{
			printf("%d: sent out %d (loop %d)\n", rank, random_value, loop);
			for (int i = 0; i < proc_quantity - 1; i++)
				// Послать сообщение всем крайним процессам
				MPI_Send(&random_value, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);


			for (int i = 0; i < proc_quantity - 1; i++)
				// Принять сообщения от всех крайних процессов
				MPI_Recv(&random_value, 1, MPI_INT, MPI_ANY_SOURCE, 0 /*MPI_ANY_TAG*/, MPI_COMM_WORLD, &status);

		}
		else
		{
			// Принять сообщение от главного процесса
			MPI_Recv(&random_value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
			printf("%d: received %d (loop %d)\n", rank, random_value, loop);
			int confirmation_value = random_value + rank;
			//Послать сообщение главному процессу
			MPI_Send(&confirmation_value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			//printf("%d: sent out %d (loop %d)\n", rank, confirmation_value, loop);

		}
	}

	MPI_Finalize();
	return 0;

}