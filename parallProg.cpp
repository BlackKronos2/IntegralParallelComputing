#pragma region INCLUDES

#include <iostream>
#include "pthread.h"
#include "stdio.h"
#include <windows.h>
#pragma comment(lib, "pthreadVCE2.lib")
#include <string>

using namespace std;

#pragma endregion

#pragma region CONSTS

#define THREADS_COUNT 5
#define THREAD_SUCCESS_CODE 0

#pragma endregion


double FuncY(double x)
{
	return pow(x, 2) / 2;
}

struct IntegInfo {
public:
	double start, end;
	int n;

	double result;
};

double CentralRectangle(double a, double b, int n)
{
	double result = 0;
	double h = (b - a) / n;

	double sum = (FuncY(a) + FuncY(b)) / 2;

	for (int i = 0; i < n; i++)
	{
		double x = a + i * h;
		sum += FuncY(x);
	}

	result = h * sum;
	return result;
}

void* Worker_thread(void* arg)
{
	IntegInfo* info = (IntegInfo*)arg;

	double result = CentralRectangle(info->start, info->end, info->n);
    printf("Результат потока = %f\n", result);
	
	info->result = result;

    pthread_exit(0);
    return (void*)&result;
}

void ShowError(string error)
{
	printf("Ошибка: %s", error);
	exit(-1);
}

void* ThereadCreate(double start, double end, int stepCount)
{
	pthread_t my_thread[THREADS_COUNT];
	IntegInfo threads_data[THREADS_COUNT];

	double interval = (abs(start) + abs(end)) / THREADS_COUNT;

	for (int i = 0; i < THREADS_COUNT; i++)
	{
		threads_data[i].start = start + (i) * interval;
		threads_data[i].end = start + (i + 1) * interval;
		threads_data[i].n = stepCount;

		threads_data[i].result = 0;
	}

	int thread_status;

	for (int thread_num = 0; thread_num < THREADS_COUNT; thread_num++)
	{
		thread_status = pthread_create(&my_thread[thread_num], NULL, &Worker_thread, (void*)&threads_data[thread_num]);
		if (thread_status != THREAD_SUCCESS_CODE)
			ShowError("Ошибка создания потока");

		thread_status = pthread_join(my_thread[thread_num], NULL);
		if (thread_status != THREAD_SUCCESS_CODE)
			ShowError("Ошибка старта потоков");
	}

	return (void*)&threads_data;
}

int main()
{
	setlocale(LC_ALL, "Russian");

	double start = -1;
	double end = 1;

	int stepCount = 100;

	printf("Границы: %f - %f\n", start, end);
	printf("Количество шагов: %d\n", stepCount);

    pthread_t my_thread[THREADS_COUNT];

	printf("Выполнение...\n");

	unsigned int start_time = clock(); // начальное время
	IntegInfo* threads_data = (IntegInfo*)ThereadCreate(start, end, stepCount);

	double result = 0;
	
	for (int i = 0; i < THREADS_COUNT; i++)
	{
		IntegInfo info = threads_data[i];
		result += info.result;
	}

	unsigned int end_time = clock(); // конечное время
	
	printf("Программа успешно завершена, результат: %f\n", result);
	printf("Время выполнения: %d мс", end_time - start_time);
    pthread_exit(NULL);
}
