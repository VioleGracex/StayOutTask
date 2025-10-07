#include "entry_utils.h"

std::wstring trim(const std::wstring& s) {
    size_t start = s.find_first_not_of(L" \t\r\n");
    size_t end = s.find_last_not_of(L" \t\r\n");
    if (start == std::wstring::npos || end == std::wstring::npos) return L"";
    return s.substr(start, end - start + 1);
}

bool parseLine(const std::wstring& line, Entry& entry) {
    std::wstring l = trim(line);
    size_t colon = l.find(L':');
    if (colon != std::wstring::npos) {
        std::wstring left = trim(l.substr(0, colon));
        std::wstring right = trim(l.substr(colon + 1));
        size_t space = left.find(L' ');
        if (space != std::wstring::npos) {
            entry.lastName = left.substr(0, space);
            entry.firstName = left.substr(space + 1);
            entry.phone = right;
            return true;
        }
        space = right.find(L' ');
        if (space != std::wstring::npos) {
            entry.phone = left;
            entry.lastName = right.substr(0, space);
            entry.firstName = right.substr(space + 1);
            return true;
        }
    }
    size_t dash = l.find(L'-');
    if (dash != std::wstring::npos) {
        std::wstring left = trim(l.substr(0, dash));
        std::wstring right = trim(l.substr(dash + 1));
        size_t space = left.find(L' ');
        if (space != std::wstring::npos) {
            entry.firstName = left.substr(0, space);
            entry.lastName = left.substr(space + 1);
            entry.phone = right;
            return true;
        }
    }
    size_t comma1 = l.find(L',');
    if (comma1 != std::wstring::npos) {
        size_t comma2 = l.find(L',', comma1 + 1);
        if (comma2 != std::wstring::npos) {
            entry.firstName = trim(l.substr(0, comma1));
            entry.lastName = trim(l.substr(comma1 + 1, comma2 - comma1 - 1));
            entry.phone = trim(l.substr(comma2 + 1));
            return true;
        }
    }
    return false;
}

// Prioritize Russian (Cyrillic) before English (Latin)
bool russian_english_compare(const std::wstring& a, const std::wstring& b) {
    auto is_cyrillic = [](wchar_t ch) {
        return (ch >= L'А' && ch <= L'я') || ch == L'Ё' || ch == L'ё';
        };
    size_t i = 0, na = a.size(), nb = b.size();
    while (i < na && i < nb) {
        wchar_t ca = a[i], cb = b[i];
        bool a_cyr = is_cyrillic(ca), b_cyr = is_cyrillic(cb);
        if (a_cyr != b_cyr)
            return a_cyr; // Cyrillic first
        if (ca != cb)
            return ca < cb;
        ++i;
    }
    return na < nb;
}