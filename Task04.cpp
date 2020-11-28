/*
* Ф.И.Г: Яхя Янал, БПИ197
* Вариант: 28
* текст задания:
*
* И снова пляшущие человечки. Узнав о планах преступников
* озвученных в задаче 33, Шерлок Холмс предложил лондонской полиции
* специальную машину для дешифровки сообщений злоумышленников.
* Реализовать многопоточное приложение, дешифрующее кодированный
* текст. В качестве ключа используется известная кодовая таблица,
* устанавливающая однозначное соответствие между каждой буквой и какимнибудь числом. Процессом узнавания кода в решении задачи пренебречь.
* Каждый поток дешифрует свои кусочки текста. При решении использовать
* парадигму портфеля задач.
*
*/

/*----------------------- code table----------------------------
	space   ->  10
	a       ->  11
	b       ->  12
	c       ->  13
	d       ->  14
	e       ->  15
	f       ->  16
	g       ->  17
	h       ->  18
	i       ->  19
	j       ->  20
	k       ->  21
	l       ->  22
	m       ->  23
	n       ->  24
	o       ->  25
	p       ->  26
	q       ->  27
	r       ->  28
	s       ->  29
	t       ->  30
	u       ->  31
	v       ->  32
	w       ->  33
	x       ->  34
	y       ->  35
	z       ->  36
	A       ->  37
	B       ->  38
	C       ->  39
	D       ->  40
	E       ->  41
	F       ->  42
	G       ->  43
	H       ->  44
	I       ->  45
	J       ->  46
	K       ->  47
	L       ->  48
	M       ->  49
	N       ->  50
	O       ->  51
	P       ->  52
	Q       ->  53
	R       ->  54
	S       ->  55
	T       ->  56
	U       ->  57
	V       ->  58
	W       ->  59
	X       ->  60
	Y       ->  61
	Z       ->  62
	\n      ->  63
*///---------------------------------------------------

#include <thread>
#include <fstream>
#include <mutex>
#include <iostream>
#include <queue>
#include <string>
#include "Timer.h"
#include <omp.h>

using namespace std;

string* results;
queue<string> tasks;
const static string alphbet = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\n"; //the alphbet, which this alg can decode
mutex mtx;

// our star ))), it's responsible for decoding the message code
void encrypte_message(const string& subString, int index) {
	if (subString.length() % 2 != 0) {
		unique_lock<mutex> u_lock(mtx); //mtx.lock()
		cout << "Sorry, Can't read this code, it contains odd code's length number,";
		cout << " and our alg specify for each char two numbers" << endl;
		exit(0);
	}
	string temp = "";
	string resultOfEncrypting = "";
	for (int i = 0; i < subString.length(); i++)
	{
		temp += subString[i];
		if (temp.length() >= 2) {
			resultOfEncrypting += alphbet[stoi(temp) - 10];
			temp = "";

		}
	}
	results[index] = resultOfEncrypting;
}

//method to create tasks for each thread
void make_tasks(int i, const string& encryptedMessage) {
	int remainder = encryptedMessage.length() % i;
	int subSlength = encryptedMessage.length() / i;
	int RemainingItems = 0;
	if (remainder == 0 && subSlength % 2 != 0) {
		subSlength -= 1;
		RemainingItems = i;
	}
	else if (remainder != 0 && subSlength % 2 == 0) {
		RemainingItems = remainder;
	}
	else if (remainder != 0 && subSlength % 2 != 0) {
		subSlength -= 1;
		RemainingItems = i;
		RemainingItems += remainder;

	}
	for (int j = 0; j < i; j++)
	{
		if (j == i - 1) {//to add the remaining elements in the last task
			tasks.push(encryptedMessage.substr(j * subSlength, subSlength + RemainingItems));
			break;
		}
		tasks.push(encryptedMessage.substr(j * subSlength, subSlength));
	}

}

void decoding(int numOfTh, const string& encryptedMessage) {
	results = new string[numOfTh]; // becuase of we know the num of threads so we initialize the dynamic array results 
	make_tasks(numOfTh, encryptedMessage); // split tasks in order to give task to each thread;
	int index = 0;

#pragma omp parallel while
	{
		while (index!=numOfTh)
		{
			string curTask = tasks.front();//git new task
			tasks.pop();
			encrypte_message(curTask, index); //solve new task
			index++;
		} 
	}
	//alternative solution
	//#pragma omp parallel for
	//{
	//	for(int i = 0; i < numOfTh ; i++)
	//	{
	//		string curTask = tasks.front();//git new task
	//		tasks.pop();
	//		encrypte_message(curTask, i); //solve new task
	//	} 
	//}
}

//method to read the encrypted message from test file
string read_encrypted_message(string path) {
	string totalTestString = "";
	ifstream myTest;
	myTest.open(path);
	if (!myTest) {
		cout << "Unable to open file";
		exit(1);
	}
	string line = "";
	while (getline(myTest, line))
	{
		totalTestString += line;
	}
	myTest.close();
	return totalTestString;
}

//the main method
int main(int argc, char** args)
{
	if (argc != 2) {
		cout << "Sorry, can't implement, You should pass to command arg only file's path where is your message code!" << endl;
		exit(0);
	}
	string encryptedMessage = read_encrypted_message(args[1]);
	if (encryptedMessage.length() % 2 != 0) {
		cout << "Sorry, your code message has odd length, the code should has even length" << endl;
		exit(0);
	}
	Timer timer; // my own class: Timer
	int numOfThread = pow(encryptedMessage.length() / 2, (1 / 2.0)); // you can chenge the num of threads into the program
	cout << "================================= NUMBER OF WORKING PROCESSES ====================================\n\n";
	cout << numOfThread << endl;
	cout << "\n--------------------------------------------------------------------------------------------------\n\n";
	cout << "===================================== ENCRYPTED MESSAGE ==========================================\n\n";
	cout << encryptedMessage << endl;
	cout << "\n--------------------------------------------------------------------------------------------------\n\n";
	timer.start();
	decoding(numOfThread, encryptedMessage); // start decoding 
	timer.stop();
	cout << "======================================= DECODED MESSAGE ==========================================\n\n";
	for (int j = 0; j < numOfThread; j++)
	{
		cout << results[j];
	}
	cout << "\n\n--------------------------------------------------------------------------------------------------\n\n";
	cout << "=========================================== RUNTIME ==============================================\n\n";
	cout << timer.result<chrono::nanoseconds>() << " milliseconds" << endl;
	cout << "\n--------------------------------------------------------------------------------------------------\n";

	delete[] results;
	
	return 0;
}
