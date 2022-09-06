#define __STDC_WANT_LIB_EXT1__ 1

#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <map>
#include <chrono>

using namespace std;

void copy(string first, char* second)
{
	int i;
	for (i = 0; first[i] != '\0'; i++)
		second[i] = first[i];
	second[i] = first[i];
}

void tokenizing(string mail, queue<string> & Queue)
{
	char* token;
	char* line = new char[mail.size() + 1];
	copy(mail, line);
	char delimiter[] = { " ,.'÷╥╧{[]}~|?!-=/+%^@&*$#_\"<>\\:;1234567890()" };
	token = strtok_s(line, delimiter, &line);
	while (token)
	{
		Queue.push(token);
		token = strtok_s(line, delimiter, &line);
	}
}

bool checkStopWords(string word)
{
	bool checkStopWord = false;
	string stopWord;
	ifstream file("stopWords.txt");
	while (!file.eof())
	{
		file >> stopWord;
		if (word.size() == stopWord.size())
		{
			if (word == stopWord)
			{
				file.close();
				return true;
			}
		}
	}
	file.close();
	return false;
}

void makeDictionary(const string filename)
{
	ifstream input(filename);
	map<string, unsigned int> word_data;
	string word;
	while (input >> word)
	{
		if (word_data.find(word) != word_data.end())
			word_data[word]++;
		else
			word_data[word] = 1;
	}
	input.close();
	ofstream output(filename);
	for (auto iter = word_data.begin(); iter != word_data.end(); ++iter)
	{
		output << iter->second << "\t" << iter->first << "\n";
	}
}

void readDictionary(const string filename, map<string, unsigned int> & spamWord)
{
	ifstream input(filename);
	string word; unsigned int num;
	while (input >> num >> word)
	{
		spamWord.insert(pair<string, int>(word, num));
	}
	input.close();
}

void preProcessing()
{
	ifstream fin("Dataset.csv");
	queue<string> Queue;
	string  line;
	getline(fin, line, '\n');
	int i = 0;
	ofstream fout;
	while (!fin.eof())
	{
		cout << i << "\n";
		getline(fin, line, '\n');
		transform(line.begin(), line.end(), line.begin(), ::tolower);
		tokenizing(line, Queue);
		string spamOrHam = Queue.front();
		Queue.pop();
		if (spamOrHam == "spam")
			fout.open("spam.txt", ios::out | ios::app);
		else
			fout.open("notSpam.txt", ios::out | ios::app);
		int size = Queue.size();
		for (int i = 0; i < size; i++)
		{
			if (checkStopWords(Queue.front()) == false)
				Queue.push(Queue.front());
			Queue.pop();
		}
		while (!Queue.empty())
		{
			fout << Queue.front() << "\n";
			Queue.pop();
		}
		i++;
		fout.close();
	}
	makeDictionary("spam.txt");
	makeDictionary("notSpam.txt");
}

int main()
{
	auto start = chrono::high_resolution_clock::now();
	ifstream file1("notSpam.txt"), file2("spam.txt");
	if (!file1 && !file2)
	{
		file1.close(); file2.close();
		preProcessing();
	}
	else
	{
		file1.close(); file2.close();
	}

	string mail = "people nowthe weather or climate in any particular environment can change and affect what people eat and how much of it they are able to eat .";
	transform(mail.begin(), mail.end(), mail.begin(), ::tolower);

	queue<string> Queue;
	tokenizing(mail, Queue);
	int size = Queue.size();
	for (int i = 0; i < size; i++)
	{
		if (checkStopWords(Queue.front()) == false)
			Queue.push(Queue.front());
		Queue.pop();
	}

	map<string, unsigned int> spam;
	map <string, unsigned int> notSpam;
	readDictionary("spam.txt", spam);
	readDictionary("notSpam.txt", notSpam);
	map <string, unsigned int> totalword(spam.begin(), spam.end());

	for (auto iter = notSpam.begin(); iter != notSpam.end(); ++iter)
	{
		if (totalword.find(iter->first) != totalword.end())
			totalword[iter->first] += iter->second;
		else
			totalword[iter->first] = iter->second;
	}

	int n, d;
	double prob, productSpam = 0, productNotSpam = 0, probabilityOfSpam, probabilityOfNotSpam;
	probabilityOfNotSpam = notSpam.size() / double(totalword.size());
	probabilityOfSpam = spam.size() / double(totalword.size());
	cout << "probabilityOfSpam = " << probabilityOfSpam << "\tprobabilityOfNotSpam = " << probabilityOfNotSpam << "\n";
	while (!Queue.empty())
	{
		cout << "Spam = ";
		auto i = spam.find(Queue.front());
		if (i != spam.end())
			n = i->second + 1, d = spam.size() + 2;
		else
			n = 0 + 1, d = spam.size() + 2;
		prob = n / double(d);
		if (productSpam == 0)
			productSpam = prob;
		else
			productSpam = productSpam * prob;
		cout << n << "/" << d << " = " << prob << "\t";

		cout << "Not Spam = ";
		auto j = notSpam.find(Queue.front());
		if (j != notSpam.end())
			n = j->second + 1, d = notSpam.size() + 2;
		else
			n = 0 + 1, d = notSpam.size() + 2;
		prob = n / double(d);
		if (productNotSpam == 0)
			productNotSpam = prob;
		else
			productNotSpam = productNotSpam * prob;
		cout << n << "/" << d << " = " << prob << "\t";

		cout << Queue.front() << "\n";
		Queue.pop();
	}

	productSpam = productSpam * probabilityOfSpam;
	productNotSpam = productNotSpam * probabilityOfNotSpam;

	cout << "Spam = " << productSpam << "\t Not Spam = " << productNotSpam << "\n";
	if (productSpam < productNotSpam)
		cout << "not spam has higher probability\n";
	else if (productSpam > productNotSpam)
		cout << "spam has higher probability\n";

	auto finish = chrono::high_resolution_clock::now();
	chrono::duration<double> elapsed = finish - start;
	cout << "Elapsed time: " << elapsed.count() << " s\n";
	return 0;
}