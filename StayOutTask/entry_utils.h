#pragma once
#include <string>

struct Entry {
    std::wstring firstName, lastName, phone;
};

std::wstring trim(const std::wstring& s);
bool parseLine(const std::wstring& line, Entry& entry);

// Prioritize Russian (Cyrillic) before English (Latin)
bool russian_english_compare(const std::wstring& a, const std::wstring& b);