#pragma once
#include <vstl/vector.h>
#include <fstream>
#include <vstl/vstring.h>
#include <vstl/Memory.h>
#include <vstl/string_view.h>
#include <span>
namespace vstd {

struct VENGINE_DLL_COMMON CharSplitIterator {
    char const *curPtr;
    char const *endPtr;
    char sign;
    string_view result;
    string_view operator*() const;
    void operator++();
    bool operator==(IteEndTag) const;
};
struct VENGINE_DLL_COMMON StrVSplitIterator {
    char const *curPtr;
    char const *endPtr;
    string_view sign;
    string_view result;
    string_view operator*() const;
    void operator++();
    bool operator==(IteEndTag) const;
};
template<typename SignT, typename IteratorType>
struct StrvIEnumerator {
    char const *curPtr;
    char const *endPtr;
    SignT sign;
    IteratorType begin() const {
        IteratorType c{curPtr, endPtr, sign};
        ++c;
        return c;
    }
    IteEndTag end() const {
        return {};
    }
};
class VENGINE_DLL_COMMON StringUtil {
private:
    StringUtil() = delete;
    KILL_COPY_CONSTRUCT(StringUtil)
public:
    static StrvIEnumerator<char, CharSplitIterator> Split(string_view str, char sign) {
        return StrvIEnumerator<char, CharSplitIterator>{str.begin(), str.end(), sign};
    }
    static StrvIEnumerator<string_view, StrVSplitIterator> Split(string_view str, string_view sign) {
        return StrvIEnumerator<string_view, StrVSplitIterator>{str.begin(), str.end(), sign};
    }

    static variant<int64, double> StringToNumber(string_view numStr);
    static void ToLower(string &str);
    static void ToUpper(string &str);

    static string ToLower(string_view str);
    static string ToUpper(string_view str);
    static void EncodeToBase64(std::span<uint8_t const> binary, string &result);
    static void EncodeToBase64(std::span<uint8_t const> binary, char *result);
    static void DecodeFromBase64(string_view str, vector<uint8_t> &result);
    static void DecodeFromBase64(string_view str, uint8_t *size);
    static void TransformWCharToChar(
        wchar_t const *src,
        char *dst,
        size_t sz);
};
}// namespace vstd