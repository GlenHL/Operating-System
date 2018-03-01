#include <iostream>
#include <fstream>
#include <windows.h>
#include <vector>
#include <time.h>

using namespace std;

const int maxCustomerNo = 20;					//�������й˿���������
const int ServerNo = 2;							//��Ա����
int Interval = 500;								//Ϊд���򷽱㣬�Թ˿͵���ʱ��ͳһ����һ��ϵ��

vector<int> LeaveList;							//�˿��뿪���б������й˿��뿪ʱ��������ֹ
vector<int> WaitingList;						//�˿͵ȴ����б����ڹ�Ա���νк�
int come_time[maxCustomerNo];					//�˿͵�����ʱ��
int wait_time[maxCustomerNo];					//�˿���Ҫ�������ʱ��
int serve_time[maxCustomerNo];					//�˿Ϳ�ʼ�������ʱ��
int leave_time[maxCustomerNo];					//�˿��뿪��ʱ��
int server_id[maxCustomerNo];					//�˿ͱ�����Ĺ�Ա���

HANDLE Server_mutex, Customer_mutex;			//��Ա���������˿ͻ�����
HANDLE Server_Semaphore, Customer_Semaphore;	//��Ա�ź������˿��ź���
HANDLE disp;									//���ڿ�����Ļ��ӡ�Ļ�����

ofstream out("output.txt");

//Server�̵߳Ĳ���
struct ServerParameter
{
	int ID;
};

//Customer�̵߳Ĳ���
struct CustomerParameter{
	int ID;
	int come_time;
	int wait_time;
};

//��Ա�߳�
DWORD WINAPI Server(LPVOID lpPara)
{
	ServerParameter *SP = (ServerParameter *)lpPara;
	while (true)
	{
		WaitForSingleObject(Customer_Semaphore, INFINITE); 
		WaitForSingleObject(Server_mutex, INFINITE);
		int ID = WaitingList[0];							//��ȡ��һ���ȴ��Ĺ˿ͱ��
		serve_time[ID] = int(clock());						//��ǰʱ�伴Ϊ��ʼ����ʱ��	
		server_id[ID] = SP->ID;								//��¼����Ĺ�Ա���
		WaitingList.erase(WaitingList.begin());				//�ӵȴ��б���ɾ���ù˿�

		
		WaitForSingleObject(disp, INFINITE);
		out <<"Customer "<< ID << " is being servered by Server " << SP->ID<<'.'<< endl;
		cout <<"Customer "<< ID << " is being servered by Server " << SP->ID<<'.'<< endl;
		ReleaseMutex(disp);
		ReleaseMutex(Server_mutex); 
		
		Sleep(wait_time[ID]);								//ģ��˿����ڱ�����
		
		WaitForSingleObject(Server_mutex, INFINITE);
		WaitForSingleObject(disp, INFINITE);
		out <<"Customer "<< ID << " is serverd." << endl;
		cout <<"Customer "<< ID << " is serverd." << endl;
		ReleaseMutex(disp);
		
		leave_time[ID] = int(clock());						//��ǰʱ�伴Ϊ�˿��뿪ʱ��
		LeaveList.erase(LeaveList.begin());					//���뿪�б���ɾ���ù˿�
		ReleaseMutex(Server_mutex); 
		ReleaseSemaphore(Server_Semaphore, 1, NULL); 
	}

	return 0;
}

//�˿��߳�
DWORD WINAPI Customer(LPVOID lpPara)
{
	CustomerParameter *CP = (CustomerParameter *)lpPara;
	//�˿͵���
	WaitForSingleObject(disp, INFINITE);
	out << "Customer " << CP->ID << " is coming." << endl;
	cout << "Customer " << CP->ID << " is coming." << endl;
	ReleaseMutex(disp);
	//�ڵȴ��б���뿪�б�ĩβ��Ӹù˿ͱ��
	WaitingList.push_back(CP->ID);
	LeaveList.push_back(CP->ID);
	//��¼�˿͵���ʱ��
	int come_clock = int(clock());

	WaitForSingleObject(Server_Semaphore, INFINITE); 
	WaitForSingleObject(Customer_mutex, INFINITE);
	//��¼�˿͵ȴ�ʱ��
	int wait_clock = floor((int(clock()) - come_clock) / Interval);
	WaitForSingleObject(disp, INFINITE);
	out << "After " << wait_clock << " seconds, " << "Customer " << CP->ID << " is called." << endl;
	cout << "After " << wait_clock << " seconds, " << "Customer " << CP->ID << " is called." << endl;
	ReleaseMutex(disp);
	ReleaseMutex(Customer_mutex);  
	ReleaseSemaphore(Customer_Semaphore, 1, NULL); 
	return 0;
}


int main()
{
	int init_time = int(clock());
	//��Ա�߳���˿��̵߳��߳�ID
	DWORD ServerID[ServerNo];
	DWORD CustomerID[maxCustomerNo];
	//�߳̾��
	HANDLE ServerHandle[ServerNo];
	HANDLE CustomerHandle[maxCustomerNo];

	disp = CreateMutex(NULL, FALSE, NULL);

	Server_mutex = CreateMutex(NULL, FALSE, NULL);
	Customer_mutex = CreateMutex(NULL, FALSE, NULL);
	Server_Semaphore = CreateSemaphore (NULL, ServerNo, ServerNo, NULL);
    Customer_Semaphore = CreateSemaphore (NULL, 0, ServerNo, NULL);
	//������Ա�߳�
	ServerParameter SP[ServerNo];
	for (int i = 0; i < ServerNo; i++)
	{
		SP[i].ID = i;
		ServerHandle[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Server, &SP[i], 0, &ServerID[i]);
        if (ServerHandle[i]==NULL) return -1;
	}

	//����˿͵�����Ϣ���������˿��߳�
	int n;
	ifstream file("Customer.txt");
	double last_time = 0;
	CustomerParameter CP[maxCustomerNo];
	while (!file.eof())
	{
		file >> n;
		file >> come_time[n] >> wait_time[n];
		come_time[n] = come_time[n] * Interval;
		wait_time[n] = wait_time[n] * Interval;
		CP[n].ID = n;
		CP[n].come_time = come_time[n];
		CP[n].wait_time = wait_time[n];
		int t = (come_time[n] - last_time);
		//����˿�ͬʱ������t����Ϊ��ֵ
		if (t < 0) t = 0;
		//ģ����һ���˿͵�����ʱ��
		Sleep(t);
		CustomerHandle[n] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Customer, &CP[n], 0, &CustomerID[n]);
		last_time = int(clock()) - init_time;
	}
	//�ȴ�ֱ���˿��߳�ִ�����
	for (int i = 1; i <= n; i++)
		if (CustomerHandle[i] != NULL)
		{
			WaitForSingleObject(CustomerHandle[i], INFINITE);
			CloseHandle(CustomerHandle[1]);
		}

	//�ȴ�ֱ����Ա�߳�ִ�����
	while (LeaveList.size() != 0)
	{
		if (LeaveList.size() == 0)
			for (int i = 0; i < ServerNo; i++)
				CloseHandle(ServerHandle);
	}
	//������
	for (int i = 1; i <= n; i++)
	{
		come_time[i] = floor(come_time[i] / Interval);
		serve_time[i] = floor(serve_time[i] / Interval);
		wait_time[i] = floor(wait_time[i] / Interval);
		leave_time[i] = floor(leave_time[i] / Interval);
		out<< i <<" & "<< come_time[i]<<" & "<<serve_time[i]<<" & "<<wait_time[i]<<" & "<<leave_time[i]<<" & "<<server_id[i] << endl;
		cout<< i <<' '<< come_time[i]<<' '<<serve_time[i]<<' '<<wait_time[i]<<' '<<leave_time[i]<<' '<<server_id[i] << endl;
	}

}