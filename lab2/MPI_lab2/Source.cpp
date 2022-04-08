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

    int proc_quantity;
    int rank;
    int mes = 0;
    MPI_Init(nullptr, nullptr);

    bool finished = false;
    MPI_Status status;
    srand(time(NULL));

    // �������� ���������� ��������� � �� ������� ����
    MPI_Comm_size(MPI_COMM_WORLD, &proc_quantity);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //��� ����� ��������� ������������
    if (rank == 0)
    {
        cout << "Num of all processes: " << proc_quantity << endl;
    }

    //Proccesses cycle
    while (!finished)
    {

        for (int round = M; round > 0; round--)
        {
            //if (finished) break;//!
            if (rank == 0)
            {
                for (int side_proc = 1; side_proc < proc_quantity; side_proc++)
                {
                    if (finished) break;
                    // ������� ��������� ���� ������� ���������
                    MPI_Send(&mes, 1, MPI_INT, side_proc, 0, MPI_COMM_WORLD);

                }

                for (int side_proc = 1; side_proc < proc_quantity; side_proc++)
                {
                    // ������� ��������� �� ���� ������� ���������
                    MPI_Recv(&mes, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                    //����� ��������� -1
                    if (mes == -1)
                    {
                        cout << "Proc " << rank << " Get message : " << mes << " From: " << status.MPI_SOURCE << " Proc* finished" << endl;
                        finished = true;     //����������� ����
                        //��������� �����, ����� ����� �������
                        for (int side_proc = 1; side_proc < proc_quantity; side_proc++)
                            MPI_Send(&mes, 1, MPI_INT, side_proc, 0, MPI_COMM_WORLD);

                    }
                    else
                    {
                        finished = false;
                        cout << "Proc " << rank << " Get message: " << mes << " From: " << status.MPI_SOURCE << endl;

                    }

                }

            }
            //Other proccesses part
            else
            {
                //�������� ���������
                mes = rand() % 10 - 1;
                //������� ��������� �������� ��������
                MPI_Send(&mes, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);


                // ������� ��������� �� �������� ��������
                MPI_Recv(&mes, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
                cout << "//Proc " << rank << " Get message: " << mes << " From: " << status.MPI_SOURCE << endl;


                //���� mes == -1 ��������� ��������
                if (mes == -1)
                {
                    finished = true;
                    cout << "Proc " << rank << " finished" << endl;

                }

                Sleep(5);
            }

        }

    }

    MPI_Finalize();
    return 0;

}


