# include <stdio.h>
# include <stdlib.h>
# include <windows.h>
# include <iostream>
# include <fstream>
# include <string>
using namespace std;

HANDLE AllocatorSemaphore, TrackSemaphore;
LPVOID start;
ofstream out("output.txt");

void printSystemInfo()
{
	SYSTEM_INFO info;  //ϵͳ��Ϣ
    GetSystemInfo (&info);
	out<<"ϵͳ��Ϣ"<<endl;
	//ָ��ҳ��Ĵ�С��ҳ�汣����ί�еĿ��������Ǳ� VirtualAlloc ����ʹ�õ�ҳ��С��
    out<<"dwPageSize"<<"\t"<<info.dwPageSize<<endl;
	//ָ��Ӧ�ó���Ͷ�̬���ӿ�(DLL)���Է��ʵ�����ڴ��ַ��
	out<<"lpMinimumapplicationAddress"<<"\t"<<info.lpMinimumApplicationAddress<<endl;
	//ָ��Ӧ�ó���Ͷ�̬���ӿ�(DLL)���Է��ʵ�����ڴ��ַ��
    out<<"lpMaximumApplicationAddress"<<"\t"<<info.lpMaximumApplicationAddress<<endl;
	//ָ��һ�������������ϵͳ��װ���˵����봦���������롣������0λ�Ǵ�����0��31λ�Ǵ�����31��
    out<<"dwActiveProcessorMask"<<"\t"<<info.dwActiveProcessorMask<<endl;
	//ָ��ϵͳ�еĴ���������Ŀ��
    out<<"dwNumberOfProcessors"<<"\t"<<info.dwNumberOfProcessors<<endl;    
	//ָ��ϵͳ�����봦���������͡�
	out<<"dwProcessorType"<<"\t"<<info.dwProcessorType<<endl;
	//ָ���Ѿ�������������ڴ�ռ�����ȡ�
	out<<"dwAllocationGranularity"<<"\t"<<info.dwAllocationGranularity<<endl;
	//ָ��ϵͳ��ϵ�ṹ�����Ĵ���������
	out<<"wProcessorLevel"<<"\t"<<info.wProcessorLevel<<endl;
	//ָ��ϵͳ��ϵ�ṹ�����Ĵ������޶��汾�š��±���ʾ�˶���ÿһ�ִ�������ϵ�����������޶��汾������ι��ɵġ�
	out<<"wProcessorRevision"<<"\t"<<info.wProcessorRevision<<endl;
	out<<"**************************************************"<<endl;
}

void printMemoryStatus()
{
	MEMORYSTATUS status; //�ڴ�״̬
	GlobalMemoryStatus (&status);
	out<<"�ڴ�״̬"<<endl;
	////�ṹ��ĳ���
	//out<<"dwLength"<<"\t"<<status.dwLength<<endl;
	////�����ڴ��ʹ�������0-100
	//out<<"dwMemoryLoad"<<"\t"<<status.dwMemoryLoad<<endl;
	////�����ڴ�Ĵ�С
	//out<<"dwTotalPhy"<<"\t"<<status.dwTotalPhys<<endl;
	//��ǰ���õ������ڴ��С
	out<<"dwAvailPhys"<<"\t"<<status.dwAvailPhys / 1024<<"KB"<<endl;
	////�ɵ���������洢���Ĵ�С�������ڴ�+ҳ�����ļ�
	//out<<"dwTotalPageFile"<<"\t"<<status.dwTotalPageFile<<endl;
	//��ǰ�ɵ���������洢���Ĵ�С
	out<<"dwAvailPageFile"<<"\t"<<status.dwAvailPageFile / 1024<<"KB"<<endl;
	////���������ַ�ռ����û�ģʽ�����Ĵ�С
	//out<<"dwTotalVirtual"<<"\t"<<status.dwTotalVirtual<<endl;
	//���������ַ�ռ����û�ģʽ�������õĴ�С
	out<<"dwAvailVirtual"<<"\t"<<status.dwAvailVirtual / 1024<<"KB"<<endl;
	//out<<"**************************************************"<<endl;
}

string getProtect(DWORD dwProtect)
{
	switch (dwProtect)
	{
	case PAGE_NOACCESS : return "PAGE_NOACCESS";
	case PAGE_READONLY : return "PAGE_READONLY";
	case PAGE_READWRITE: return "PAGE_READWRITE";
	case PAGE_EXECUTE : return "PAGE_EXECUTE";
	case PAGE_EXECUTE_READ: return "PAGE_EXECUTE_READ";
	case PAGE_EXECUTE_READWRITE : return "PAGE_EXECUTE_READWRITE";
	}
	return "does not have access";
}

string getState(DWORD dwState)
{
	switch (dwState)
	{
	case MEM_COMMIT: return "MEM_COMMIT";
	case MEM_FREE: return "MEM_FREE";
	case MEM_RESERVE: return "MEM_RESERVE";
	}
}

void printMemoryInfo(LPVOID Address)
{
	SYSTEM_INFO info;  //ϵͳ��Ϣ
    GetSystemInfo (&info);
	MEMORY_BASIC_INFORMATION mem; //�ڴ������Ϣ
	VirtualQuery (Address, &mem, sizeof(MEMORY_BASIC_INFORMATION) );
	out<<"�ڴ������Ϣ"<<endl;
	// //��ѯ�ڴ����ռ�ĵ�һ��ҳ�����ַ
	//out<<"BaseAddress"<<"\t"<<mem.BaseAddress<<endl;
	////�ڴ����ռ�ĵ�һ���������ַ��С�ڵ���BaseAddress��Ҳ����˵BaseAddressһ��������AllocationBase����ķ�Χ��
	//out<<"AllocationBase"<<"\t"<<mem.AllocationBase<<endl;
	//���򱻳��α���ʱ����ı�������
	out<<"AllocationProtect"<<"\t"<<getProtect(mem.AllocationProtect)<<endl;
	//��BaseAddress��ʼ��������ͬ���Ե�ҳ��Ĵ�С
	out<<"RegionSize"<<"\t"<<mem.RegionSize / 1024 <<"KB"<<endl;
	//ҳ���״̬�������ֿ���ֵ��MEM_COMMIT��MEM_FREE��MEM_RESERVE
	out<<"State"<<"\t"<<getState(mem.State)<<endl;
	//ҳ������ԣ�����ܵ�ȡֵ��AllocationProtect��ͬ
	out<<"Protect"<<"\t"<<getProtect(mem.Protect)<<endl;
	////���ڴ������ͣ������ֿ���ֵ��MEM_IMAGE��MEM_MAPPED��MEM_PRIVATE
	//out<<"Type"<<"\t"<<mem.Type<<endl;
	out<<"**************************************************"<<endl;
}

DWORD WINAPI Tracker(LPVOID param)
{
	for (int i = 0;i < 30;i++)
	{
		WaitForSingleObject(TrackSemaphore, INFINITE);	//����trac�ź���
		printMemoryStatus();	//��ӡ�ڴ�״̬��Ϣ
		printMemoryInfo(start);	//��ӡ�ڴ������Ϣ
		ReleaseSemaphore(AllocatorSemaphore, 1, NULL);	//�ͷ�allo�ź���
	}
	return 0;
}

DWORD WINAPI Allocator(LPVOID param)
{
	SYSTEM_INFO info;
	GetSystemInfo (&info);
	DWORD Protect[5] = {PAGE_READONLY, PAGE_READWRITE, PAGE_EXECUTE, PAGE_EXECUTE_READ, PAGE_EXECUTE_READWRITE};
	string protect[5] = {"PAGE_READONLY", "PAGE_READWRITE", "PAGE_EXECUTE", "PAGE_EXECUTE_READ", "PAGE_EXECUTE_READWRITE"};
	long size;
	out<<"ģ���ڴ���俪ʼ��"<<endl;
	printMemoryStatus();
	out<<"**************************************************"<<endl;
	for (int j = 0; j<30; j++)
	{
		//����allo�ź���
		WaitForSingleObject(AllocatorSemaphore, INFINITE); 
		int i = j % 6;
		int Pages = j / 6 + 1;
		if (i == 0)
		{
			out<<"�����ڴ�"<<endl;
			start = VirtualAlloc(NULL, Pages * info.dwPageSize, MEM_RESERVE, PAGE_NOACCESS);
			size = Pages * info.dwPageSize;
			out<<"��ʼ��ַ��"<<start<<"\t"<<"�ڴ��С��"<<size / 1024<<"KB"<<endl;
		}
		if (i == 1)
		{
			out<<"�ύ�ڴ�"<<endl;
			start = VirtualAlloc(start, size, MEM_COMMIT, Protect[int(j / 6)]);
			out<<"��ʼ��ַ��"<<start<<"\t"<<"�ڴ��С��"<<size / 1024<<"KB"<<endl;
			out<<"��������"<<protect[j / 6]<<endl;
		}
		if (i == 2)
		{
			out<<"�����ڴ�"<<endl;
			out<<"��ʼ��ַ��"<<start<<"\t"<<"�ڴ��С��"<<size / 1024<<"KB"<<endl;
			if (!VirtualLock(start, size))
				out<<GetLastError()<<endl;
		}
		if (i == 3)
		{
			out<<"�����ڴ�"<<endl;
			out<<"��ʼ��ַ��"<<start<<"\t"<<"�ڴ��С��"<<size / 1024<<"KB"<<endl;
			if (!VirtualUnlock(start, size))
				out<<GetLastError()<<endl;	 
		}
		if (i == 4)
		{
			out<<"�����ڴ�"<<endl;
			out<<"��ʼ��ַ��"<<start<<"\t"<<"�ڴ��С��"<<size / 1024<<"KB"<<endl;
			if (!VirtualFree(start, size, MEM_DECOMMIT))
				out<<GetLastError()<<endl;	 
		}
		if (i == 5)
		{
			out<<"�ͷ��ڴ�"<<endl;
			out<<"��ʼ��ַ��"<<start<<"\t"<<"�ڴ��С��"<<size / 1024<<"KB"<<endl;
			if (!VirtualFree(start, 0, MEM_RELEASE))
				out<<GetLastError()<<endl;	
		}
		//�ͷ�trac�ź���
		ReleaseSemaphore(TrackSemaphore, 1, NULL);
	}
	
	Sleep(1000);
	return 0;

}

int main()
{
	out<<"����ʼ��"<<endl;
	//printSystemInfo();
	//printMemoryInfo();
	//printMemoryStatus();

	AllocatorSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
	TrackSemaphore = CreateSemaphore(NULL, 0, 1, NULL);

	DWORD TrackID, AllocatorID;
	HANDLE TrackerHandle = CreateThread(NULL, 0, Tracker, NULL, 0, &TrackID);
	HANDLE AllocatorHandle = CreateThread(NULL, 0, Allocator, NULL, 0, &AllocatorID);

	if (TrackerHandle != NULL)
	{
		WaitForSingleObject(TrackerHandle, INFINITE);
		CloseHandle(TrackerHandle);
	}		
	if (AllocatorHandle != NULL)
	{
		WaitForSingleObject(AllocatorHandle, INFINITE);
		CloseHandle(AllocatorHandle);
	}
	out<<"���������"<<endl;
	printMemoryStatus();
}