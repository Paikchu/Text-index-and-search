//
// Created by Yuchen Zhang on 3/15/23.
//


#include <xapian.h>
#include <iostream>
#include <regex>
#include <chrono>

using namespace std;
using namespace std::chrono;

const string YELLOW = "\033[33m";
const string YELLOW_END = "\033[0m";

int main(int argc, char **argv) {
    if (argc < 4) {
        cerr << "Usage: <index_name> <top_k> <keyword1> <keyword2> ..." << endl;
        exit(0);
    }
    auto startTime = system_clock::now();
    string index_name = argv[1];
    int top_k = stoi(argv[2]);
    vector<string> keywords;
    bool isANDQuery = false;
    for (int i = 3; i < argc; ++i) {
        string keyword = argv[i];
        if (keyword[0] == '+') {
            isANDQuery = true;
            keywords.emplace_back(keyword.substr(1, keyword.size() - 1));
        } else {
            keywords.emplace_back(keyword);
        }
    }

    Xapian::WritableDatabase db(index_name);

    if (isANDQuery) {
        Xapian::Query query(
                Xapian::Query::OP_AND,
                keywords.begin(),
                keywords.end()
        );
        Xapian::Enquire enquire(db);
        enquire.set_query(query);

        Xapian::MSet matches = enquire.get_mset(0, top_k);
        // cout << "mset size is " << matches.size() << endl;

        for (Xapian::MSetIterator match = matches.begin(); match != matches.end(); match++) {
            Xapian::Document doc = match.get_document();
            string name = doc.get_value(0);
            string description = doc.get_value(1);

            for (const auto &keyword: keywords) {
                regex pattern(keyword);
                string new_keyword = YELLOW;
                new_keyword += keyword;
                new_keyword += YELLOW_END;
                description = regex_replace(description, pattern, new_keyword);

            }
            // cout << name << endl;
            // cout << description << endl;
        }
    } else {
        Xapian::Query query(
                Xapian::Query::OP_OR,
                keywords.begin(),
                keywords.end()
        );
            Xapian::Enquire enquire(db);
            enquire.set_query(query);

            Xapian::MSet matches = enquire.get_mset(0, top_k);
            // cout << "mset size is " << matches.size() << endl;

            for (Xapian::MSetIterator match = matches.begin(); match != matches.end(); match++) {
                Xapian::Document doc = match.get_document();
                string name = doc.get_value(0);
                string description = doc.get_value(1);

                for (const auto &keyword: keywords) {
                    regex pattern(keyword);
                    string new_keyword = YELLOW;
                    new_keyword += keyword;
                    new_keyword += YELLOW_END;
                    description = regex_replace(description, pattern, new_keyword);

                }
            }
    }
    auto endTime = system_clock::now();
    auto duration = duration_cast<microseconds>(endTime - startTime);
    std::cout << double(duration.count()) * microseconds::period::num / microseconds::period::den << endl;
}