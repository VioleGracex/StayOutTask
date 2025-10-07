#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fcntl.h>
#include <io.h>
#include "entry_utils.h"

struct TestResult {
    int number;
    std::wstring description;
    bool passed;
    std::wstring expected;
    std::wstring actual;
};

std::vector<TestResult> test_sorting() {
    std::vector<TestResult> results;
    std::vector<Entry> entries = {
        {L"Иван", L"Иванов", L"3515454"},
        {L"Олег", L"Сидоров", L"23515423"},
        {L"Иван", L"Петров", L"3515455"},
        {L"Иван", L"Сидоров", L"3515422"},
        {L"John", L"Smith", L"5551234"},
        {L"Jane", L"Smith", L"5556789"}
    };

    // Sort by firstName (Cyrillic before Latin)
    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
        return russian_english_compare(a.firstName, b.firstName);
        });
    results.push_back({ 1, L"Sort by firstName: 1st should be Cyrillic ('Иван')",
        entries.front().firstName == L"Иван",
        L"Иван", entries.front().firstName });

    results.push_back({ 2, L"Sort by firstName: last should be Latin or 'Олег'",
        entries.back().firstName == L"John" || entries.back().firstName == L"Jane" || entries.back().firstName == L"Олег",
        L"John/Jane/Олег", entries.back().firstName });

    // Sort by lastName (Cyrillic before Latin)
    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
        return russian_english_compare(a.lastName, b.lastName);
        });
    results.push_back({ 3, L"Sort by lastName: 1st should be Cyrillic ('Иванов')",
        entries.front().lastName == L"Иванов",
        L"Иванов", entries.front().lastName });

    results.push_back({ 4, L"Sort by lastName: last should be Latin or 'Сидоров'",
        entries.back().lastName == L"Smith" || entries.back().lastName == L"Сидоров",
        L"Smith/Сидоров", entries.back().lastName });

    // Sort by phone
    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
        return russian_english_compare(a.phone, b.phone);
        });
    results.push_back({ 5, L"Sort by phone: 1st is 23515423/3515422/3515454",
        entries.front().phone == L"23515423" || entries.front().phone == L"3515422" || entries.front().phone == L"3515454",
        L"23515423/3515422/3515454", entries.front().phone });

    results.push_back({ 6, L"Sort by phone: last is 5556789",
        entries.back().phone == L"5556789",
        L"5556789", entries.back().phone });

    return results;
}

// Returns true if all tests pass
bool run_tests() {
    int oldMode = _setmode(_fileno(stdout), _O_U16TEXT); (void)oldMode;
    std::vector<TestResult> sortResults = test_sorting();

    bool sortOk = true;
    for (const auto& r : sortResults) {
        if (!r.passed) {
            std::wcout << L"[FAIL] sort test #" << r.number << L": " << r.description << L"\n";
            std::wcout << L"    Expected: " << r.expected << L"\n";
            std::wcout << L"    Actual:   " << r.actual << L"\n";
            sortOk = false;
        }
    }
    if (sortOk) std::wcout << L"Все сортировка тесты пройдены\n";
    else std::wcout << L"Сортировка тесты НЕ пройдены!\n";

    if (sortOk) {
        std::wcout << L"Все unit тесты пройдены!\n";
        return true;
    }
    else {
        std::wcout << L"ВНИМАНИЕ! Есть ошибки в тестах!\n";
        return false;
    }
}