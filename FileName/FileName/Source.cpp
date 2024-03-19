#include <iostream>
#include <windows.h>
#include <string>
#include <set>
#include <limits>
#include <fstream>
#include <urlmon.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#pragma comment(lib, "urlmon.lib")
using namespace std;

void clearInputBuffer() 
{
	while (cin.get() != '\n'); // Продолжаем считывать символы, пока не достигнем конца строки
}

void getUserCurrencyAndDate(string& date, string& currency)
{
	int year, month, day;
	set<string> validCurrencies = { "USD", "EUR"};

	cout << "Введите год: ";
	while (!(cin >> year) || year < 2000 || year > 2100) 
	{
		cout << "Ошибка. Введите корректный год (от 2000 до 2100): ";
		cin.clear(); 
		clearInputBuffer();
	}

	cout << "Введите месяц: ";
	while (!(cin >> month) || month < 1 || month > 12) 
	{
		cout << "Ошибка. Введите корректный месяц (от 1 до 12): ";
		cin.clear();
		clearInputBuffer();
	}

	cout << "Введите день: ";
	while (!(cin >> day) || day < 1 || day > 31) 
	{ 
		cout << "Ошибка. Введите корректный день (от 1 до 31): ";
		cin.clear();
		clearInputBuffer(); 
	} 

	// Преобразование даты в строку формата YYYYMMDD
	char buffer[11];
	snprintf(buffer, sizeof(buffer), "%04d%02d%02d", year, month, day);
	date = string(buffer);

	cout << "Введите код валюты (USD, EUR): ";
	while (!(cin >> currency) || validCurrencies.find(currency) == validCurrencies.end()) 
	{
		cout << "Ошибка. Введите одну из допустимых валют (USD, EUR): ";
		cin.clear();
		clearInputBuffer(); 
	}
}

bool currencyData(const string& date, const string& currency, const string& destFile) 
{
	string srcURL = "https://bank.gov.ua/NBUStatService/v1/statdirectory/exchange?json";
	
	if (!currency.empty())
	{
		srcURL = "https://bank.gov.ua/NBUStatService/v1/statdirectory/exchange?valcode=" + currency + "&date=" + date + "&json";
	}

	return S_OK == URLDownloadToFileA(NULL, srcURL.c_str(), destFile.c_str(), 0, NULL);
}


void parseAndShowData(const string& jsonLine) {
	size_t startPos, endPos;
	string date, currency, rate;

	// Поиск и извлечение даты
	startPos = jsonLine.find("\"exchangedate\":\"") + 16;
	endPos = jsonLine.find("\"", startPos);
	date = jsonLine.substr(startPos, endPos - startPos);

	// Поиск и извлечение кода валюты
	startPos = jsonLine.find("\"cc\":\"") + 6;
	endPos = jsonLine.find("\"", startPos);
	currency = jsonLine.substr(startPos, endPos - startPos);

	// Поиск и извлечение курса
	startPos = jsonLine.find("\"rate\":") + 7;
	endPos = jsonLine.find(",", startPos);
	rate = jsonLine.substr(startPos, endPos - startPos);

	// Вывод информации
	cout << "Дата: " << date << "г, Валюта - " << currency << ", 1 " << currency << " = " << rate << " грн." << endl;
}

void showData(const string& destFile) 
{
	ifstream file(destFile);
	string line, jsonContent;

	while (getline(file, line)) 
	{
		jsonContent += line;
	}

	parseAndShowData(jsonContent);
}


int main()
{

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	setlocale(0, "ru");
	
	string date, currency;

	getUserCurrencyAndDate(date, currency); 
	
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	string destFile = "file.txt"; 
	if (currencyData(date, currency, destFile)) 
	{ 
		cout << "Saved to " << destFile << "\n";
		showData(destFile); 
	}
	else 
	{
		cout << "Fail!\n";
	}
	
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	/*
	1. С клавиатуры вводим следующие данные:
	  дата
	  Валюта
	  Вывести актуальную информацию!
	  формат вывода - Дата: 19.03.2024г, Валюта - USD, 1$ = 38.9744 грн.
	*/

}