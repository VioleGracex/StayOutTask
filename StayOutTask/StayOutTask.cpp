#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <locale>
#include <codecvt>
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include "entry_utils.h"

// Проверка существования файла по широкому пути (Windows & Unicode)
bool file_exists(const std::wstring& filename) {
    struct _stat64i32 st;
    return _wstat(filename.c_str(), &st) == 0;
}

extern bool run_tests();

int wmain(int argc, wchar_t* argv[]) {
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stderr), _O_U16TEXT);
    std::locale::global(std::locale(""));

    bool RUN_UNIT_TESTS = true;
    bool healthy = true;
    if (RUN_UNIT_TESTS) {
        healthy = run_tests();
        if (!healthy) {
            std::wcout << L"Тесты не пройдены. Вы уверены, что хотите запустить программу? (y/n): ";
            std::wstring answer;
            std::getline(std::wcin, answer);
            if (answer != L"y" && answer != L"Y" && answer != L"д" && answer != L"Д") {
                std::wcout << L"Работа программы завершена.\n";
                return 1;
            }
        }
        else {
            std::wcout << L"Программа здорова. Продолжаем...\n";
        }
    }

    bool again = true;
    while (again) {
        // Получение пути к входному файлу
        std::wstring inputFile;
        bool inputFileFound = false;
        while (!inputFileFound) {
            if (argc > 1 && wcslen(argv[1]) > 0) {
                inputFile = argv[1];
                std::wcout << L"Используется файл: " << inputFile << std::endl;
            }
            else {
                std::wcout << L"Введите путь к файлу или перетащите файл сюда и нажмите Enter: ";
                std::getline(std::wcin, inputFile);
                inputFile = trim(inputFile);
                if (inputFile.empty()) {
                    inputFile = L"input.txt";
                    std::wcout << L"Файл не указан, используется input.txt" << std::endl;
                }
            }

            // Проверка существования входного файла
            if (!file_exists(inputFile)) {
                std::wcerr << L"Ошибка: файл " << inputFile << L" не найден!\n";
                std::wcout << L"Попробуйте еще раз, или введите 'n' чтобы выйти.\n";
                std::wcout << L"Введите новый путь к файлу или 'n' для выхода: ";
                std::wstring retryInput;
                std::getline(std::wcin, retryInput);
                retryInput = trim(retryInput);
                if (retryInput == L"n" || retryInput == L"N") {
                    again = false;
                    break; 
                }
                if (!retryInput.empty()) {
                    inputFile = retryInput;
                }
                continue; 
            }
            inputFileFound = true;
        }
        if (!again) break;

        // Получение пути к выходному файлу
        std::wstring outputFile;
        while (true) {
            std::wcout << L"Введите имя файла для вывода результатов (например: output.txt): ";
            std::getline(std::wcin, outputFile);
            outputFile = trim(outputFile);
            if (outputFile.empty()) {
                outputFile = L"output.txt";
                std::wcout << L"Имя файла не указано, используется имя по умолчанию: output.txt\n";
            }
            if (file_exists(outputFile)) {
                std::wcout << L"Файл \"" << outputFile << L"\" уже существует. Перезаписать? (y/n): ";
                std::wstring ow;
                std::getline(std::wcin, ow);
                if (ow == L"y" || ow == L"Y" || ow == L"д" || ow == L"Д") break;
                else continue;
            }
            break;
        }

        // Открытие входного файла (UTF-8)
        std::wifstream in;
        in.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
        in.open(inputFile);
        if (!in.is_open()) {
            std::wcerr << L"Ошибка: не удалось открыть файл " << inputFile << std::endl;
            continue; 
        }

        std::vector<Entry> entries;
        std::vector<std::wstring> failedLines;
        std::wstring line;

        std::wcout << L"Введите тип сортировки:\n";
        std::wcout << L"1 - по имени (Имя Фамилия: Телефон)\n";
        std::wcout << L"2 - по фамилии (Фамилия Имя: Телефон)\n";
        std::wcout << L"3 - по телефону (Телефон: Фамилия Имя)\n";
        std::wcout << L"4 - CSV (Имя,Фамилия,Телефон)\n";
        std::wcout << L"Ваш выбор: ";
        int sortType = 2;
        std::wcin >> sortType;
        std::wcin.ignore(1000, L'\n');

        while (std::getline(in, line)) {
            if (trim(line).empty()) continue;
            Entry e;
            if (parseLine(line, e)) {
                entries.push_back(e);
            }
            else {
                failedLines.push_back(line);
            }
        }
        in.close();

        // Открытие выходного файла (UTF-8)
        std::wofstream out;
        out.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
        out.open(outputFile);
        if (!out.is_open()) {
            std::wcerr << L"Ошибка: не удалось открыть файл для записи " << outputFile << std::endl;
            continue; 
        }

        if (sortType == 1) {
            std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
                return russian_english_compare(a.firstName, b.firstName);
                });
            for (const auto& e : entries)
                out << e.firstName << L" " << e.lastName << L": " << e.phone << L"\n";
        }
        else if (sortType == 2) {
            std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
                return russian_english_compare(a.lastName, b.lastName);
                });
            for (const auto& e : entries)
                out << e.lastName << L" " << e.firstName << L": " << e.phone << L"\n";
        }
        else if (sortType == 3) {
            std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
                return russian_english_compare(a.phone, b.phone);
                });
            for (const auto& e : entries)
                out << e.phone << L": " << e.lastName << L" " << e.firstName << L"\n";
        }
        else if (sortType == 4) {
            std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
                if (russian_english_compare(a.firstName, b.firstName)) return true;
                if (russian_english_compare(b.firstName, a.firstName)) return false;
                if (russian_english_compare(a.lastName, b.lastName)) return true;
                if (russian_english_compare(b.lastName, a.lastName)) return false;
                return russian_english_compare(a.phone, b.phone);
                });
            for (const auto& e : entries)
                out << e.firstName << L"," << e.lastName << L"," << e.phone << L"\n";
        }
        else {
            out << L"Некорректный выбор!\n";
        }

        if (!failedLines.empty()) {
            out << L"\nСледующие строки не соответствуют ни одному формату и не были отсортированы:\n";
            for (const auto& f : failedLines)
                out << f << L"\n";
        }
        out.close();

        std::wcout << L"Готово! Результаты записаны в " << outputFile << L"\n";
        std::wcout << L"Хотите выполнить еще одну задачу? (y/n): ";
        std::wstring ans;
        std::getline(std::wcin, ans);
        again = (ans == L"y" || ans == L"Y" || ans == L"д" || ans == L"Д");
    }

    std::wcout << L"Работа программы завершена.\n";
    return 0;
}
