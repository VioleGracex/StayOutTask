#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <locale>
#include <codecvt>
#include <fcntl.h>
#include <io.h>
#include "entry_utils.h"
#include <windows.h>

// Сравнение только по русскому алфавиту
bool lexicographical_sort(const std::wstring& a, const std::wstring& b) {
    size_t i = 0, na = a.size(), nb = b.size();
    while (i < na && i < nb) {
        wchar_t ca = a[i], cb = b[i];
        if (ca != cb)
            return ca < cb;
        ++i;
    }
    return na < nb;
}

int main() {

    std::locale::global(std::locale(""));
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    _setmode(_fileno(stdout), _O_U8TEXT);
    _setmode(_fileno(stdin), _O_U8TEXT);

    std::wcout << L"Введите строки для сортировки (пустая строка - завершить):\n";
    std::vector<Entry> entries;
    std::vector<std::wstring> failedLines;
    std::wstring line;
    while (true) {
        std::getline(std::wcin, line);
        if (trim(line).empty()) break;
        Entry e;
        if (parseLine(line, e)) {
            entries.push_back(e);
        }
        else {
            failedLines.push_back(line);
        }
    }

    int mode = 0;
    std::wcout << L"Выберите режим сортировки:\n"
        << L"1 - по имени (Имя Фамилия: Телефон)\n"
        << L"2 - по фамилии (Фамилия Имя: Телефон)\n"
        << L"3 - по телефону (Телефон: Фамилия Имя)\n"
        << L"Ваш выбор: ";
    std::wcin >> mode;
    std::wcin.ignore(1000, L'\n');

    // Сортировка только по русским буквам
    auto cmp = [&](const Entry& a, const Entry& b) {
        if (mode == 1) return lexicographical_sort(a.firstName, b.firstName);
        if (mode == 2) return lexicographical_sort(a.lastName, b.lastName);
        if (mode == 3) return lexicographical_sort(a.phone, b.phone);
        return false;
        };
    std::sort(entries.begin(), entries.end(), cmp);

    std::wcout << L"\nРезультаты сортировки:\n";
    for (const auto& e : entries)
        if (mode == 1) std::wcout << e.firstName << L" " << e.lastName << L": " << e.phone << L"\n";
        else if (mode == 2) std::wcout << e.lastName << L" " << e.firstName << L": " << e.phone << L"\n";
        else if (mode == 3) std::wcout << e.phone << L": " << e.lastName << L" " << e.firstName << L"\n";
        else std::wcout << L"Некорректный выбор!\n";

    if (!failedLines.empty()) {
        std::wcout << L"\nСледующие строки не соответствуют ни одному формату и не были отсортированы:\n";
        for (const auto& f : failedLines)
            std::wcout << f << L"\n";
    }
    return 0;
}