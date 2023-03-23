#include <xapian.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <queue>
#include <fstream>
#include <codecvt>
#include <unordered_set>
#include <regex>
#include <chrono>

using namespace std::chrono;

using namespace std;
const string STOP_WORDS = "stopwords.txt";

void load_stopWords(unordered_set<string> &stopWords) {
    ifstream stpWord(STOP_WORDS);
    if (!stpWord.is_open()) {
        cerr << "Can't open stopwords.txt";
        ::exit(0);
    }
    string word;
    while (getline(stpWord, word)) {
        stopWords.insert(word);
        word.clear();
    }
}

void load_metaData(const string &dataset_name, vector<string> &data) {
    ifstream metaData(dataset_name);
    string line;
    while (getline(metaData, line)) {
        data.push_back(line);
        line.clear();
    }
    if (data.size() % 2 != 0) {
        cerr << "metadata problem";
        ::exit(0);
    }
}

// Return the size of letter
int utf8_bytes(const char *s) {
    unsigned char c = s[0];
    if (c <= 0x7F) {
        return 1;
    }
    if ((c >> 5) == 0x6) {
        return 2;
    }
    if ((c >> 4) == 0xE) {
        return 3;
    }
    if ((c >> 3) == 0x1E) {
        return 4;
    }
    return 0;
}

void word_tokenize(const string& s, vector<string> &tokens) {
    regex oneByteLetter("[a-zA-Z0-9]+");

    wregex twoBytesLetter(L"[\u00C0-\u00D6\u00D8-\u00F6\u00F8-\u024F]");

    stringstream ss(s);
    string token;
    while (getline(ss, token, ' ')) {
        // if token is empty, skip
        if (token.empty()) continue;
        // 对字符进行替换
        queue<int> len_of_each_char;
        for (const char *p = token.c_str(); *p != '\0'; p += utf8_bytes(p)) {
            len_of_each_char.push(utf8_bytes(p));
        }
        int start = 0;
        while (!len_of_each_char.empty()) {
            int len = len_of_each_char.front();
            len_of_each_char.pop();
            string temp = token.substr(start, len);
            if(len == 1){
                if (!regex_search(temp, oneByteLetter)) {
                    token.erase(start, len);
                    token.insert(start, " ");
                    start += 1;
                } else {
                    start += len;
                }
            }
            else if(len == 2){
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                std::wstring wide = converter.from_bytes(temp);
                if (!regex_search(wide, twoBytesLetter)) {
                    token.erase(start, len);
                    token.insert(start, " ");
                    start += 1;
                } else {
                    start += len;
                }
            }
            else{
                token.erase(start, len);
                token.insert(start, " ");
                start += 1;
            }
        }
        // 去掉被替换的字符
        stringstream subToken(token);
        string temp;
        while (subToken >> temp) {
            if (!temp.empty())
                tokens.push_back(temp);
        }
    }
}

void remove_stop_words(vector<string> &terms, const vector<string> &tokens, const unordered_set<string> &stopWords) {
    for (const auto &token: tokens) {
        if (stopWords.find(token) == stopWords.end()) {
            terms.push_back(token);
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 3) {
        cerr << "Usage: <dataset_name> <index_name>" << endl;
        exit(0);
    }
    auto startTime = system_clock::now();
    string dataset_name = argv[1];
    string index_name = argv[2];
    Xapian::WritableDatabase db(index_name, Xapian::DB_CREATE_OR_OPEN);
    Xapian::Document doc;
    unordered_set<string> stopWords;
    vector<string> data;
    load_metaData(dataset_name, data);
    load_stopWords(stopWords);
    vector<string> tokens;
    for (int i = 0; i < data.size(); i += 2) {
        doc.add_value(0, data[i]);
        doc.add_value(1, data[i + 1]);
        string d = data[i] + ' ' + data[i + 1];
        word_tokenize(d, tokens);
        vector<string> terms;
        remove_stop_words(terms, tokens, stopWords);
        for (const auto &term: terms) { doc.add_term(term); }
        db.add_document(doc);
        if ((i != 0 && i % 1000 == 0) || i == data.size() - 2) { db.commit(); }
        doc.clear_terms();
        doc.clear_values();
        tokens.clear();
    }
    auto endTime = system_clock::now();
    auto duration = duration_cast<microseconds>(endTime - startTime);
    std::cout << double(duration.count()) * microseconds::period::num / microseconds::period::den << endl;
}

