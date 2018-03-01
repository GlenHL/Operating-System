#include <iostream>
#include <fstream>
#include <windows.h>
#include <vector>
#include <stdlib.h> 


using namespace std;

//����������
void qsort(int *arr, int left, int right, int &len1, int &len2, int &position)
{
	if (left >= right) return;
	int leftTemp = left;
	int rightTemp = right;
	int key = arr[left];
	while (left < right)
	{
		while (left < right && arr[right] >= key ) right--;
		arr[left] = arr[right];
		while (left < right && arr[left] <= key) left++;
		arr[right] = arr[left];
	}
	arr[left] = key;
	len1 = left - leftTemp;
	len2 = rightTemp - left;
	position = left;
	if (rightTemp - leftTemp + 1 <= 1000)
	{
		qsort(arr, leftTemp, left - 1, len1, len2, position);
		qsort(arr, left + 1, rightTemp, len1, len2, position);
	}

}

//�����̲߳���
struct ArrInfo
{
	int len;
	int start;
	int end;
};

//���򻥳���
HANDLE Qsort_mutex;

DWORD WINAPI Qsort(LPVOID lpPara)
{
	ArrInfo *arrinfo = (ArrInfo *)lpPara;
	HANDLE hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, 
									  FALSE,
									  L"data");
	VOID *hMap = MapViewOfFile(hFileMap,
								FILE_MAP_ALL_ACCESS,
								0,
								0,
								0);
	//��ø����̹߳���������׵�ַ
	int *arr1 = (int *)hMap;

	WaitForSingleObject(Qsort_mutex, INFINITE);
	int left = arrinfo->start;
	int right = arrinfo->end;
	int len1, len2, position;
	qsort(arr1, left, right, len1, len2, position);
	ReleaseMutex(Qsort_mutex);

	ArrInfo arrinfo1, arrinfo2;
	arrinfo1.start = arrinfo->start;
	arrinfo1.end = position - 1;
	arrinfo1.len = len1;
	arrinfo2.start = position + 1;
	arrinfo2.end = arrinfo->end;
	arrinfo2.len = len2;
	//������һ���߳�
	if (arrinfo1.len > 1000)
	{
		HANDLE hQsort1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Qsort, &arrinfo1, 0, NULL);
		WaitForSingleObject(hQsort1, INFINITE);
		CloseHandle(hQsort1);
	}
	else
		//�������С��1000�����ڼ����ָ�������̣�������ɸö����ݿ�������
		qsort(arr1, arrinfo1.start, arrinfo1.end, len1, len2, position);
	//�����ڶ����߳�
	if (arrinfo2.len > 1000)
	{
		HANDLE hQsort2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Qsort, &arrinfo2, 0, NULL);
		WaitForSingleObject(hQsort2, INFINITE);
		CloseHandle(hQsort2);
	}
	else
		//�������С��1000�����ڼ����ָ�������̣�������ɸö����ݿ�������
		qsort(arr1, arrinfo2.start, arrinfo2.end, len1, len2, position);


	UnmapViewOfFile(hMap);
	CloseHandle(hFileMap);	
	return 0;
}

int main()
{
	//�������1000000����
	//ofstream input("input.txt");
	//for (int i = 1; i < 1000000; i++)
	//	input << rand() << ' ';
	//input << rand();

	//���ļ��ж�ȡ����
	ifstream file("input.txt");
	int num = 1000000;
    vector<int> a;
	int x;
    for (int i=0; i<num; i++)
	{
		file >> x;
		a.push_back(x);
	}

	//�������򻥳���
	Qsort_mutex = CreateMutex(NULL, FALSE, NULL);

	//�����ļ�ӳ�����
	HANDLE hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE,
										NULL,
										PAGE_READWRITE,
										0,
										0x1000000,
										L"data");
	//���ļ���ͼӳ�䵽�����̵ĵ�ַ�ռ�
	VOID *hMap = MapViewOfFile(hFileMap,
								FILE_MAP_ALL_ACCESS,
								0,
								0,
								0);

	//�������׵�ַ����ӳ���ַ�ռ���׵�ַ��ʹ����ָ��ͬһ���ڴ�
    memcpy(hMap, &a[0], num * sizeof(int));

	//�����̲߳���
    ArrInfo arrinfo;
    arrinfo.start = 0;
    arrinfo.end = num - 1;
    arrinfo.len = num;
	//�����������߳�
	HANDLE hQsort = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Qsort, &arrinfo, 0, NULL);
	WaitForSingleObject(hQsort, INFINITE);
	CloseHandle(hQsort);
	//�����д���ļ�
	int *b = (int *)hMap;
	ofstream output("output.txt");
	for (int i = 0; i < 1000000; i++)
		output << b[i] << ' ';
	//��ӡǰ100�����ݣ��Ա����
	//for (int i=0; i<100; i++)
	//{
	//	cout<< b[i]<<endl;
	//}	
	UnmapViewOfFile(hMap);
	CloseHandle(hFileMap);	
}