

#include <xapian.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <random>
#include <regex>
#include <chrono>

// #include "searcher.h"
using namespace std;

using namespace std::chrono;
const string YELLOW = "\033[33m";
const string YELLOW_END = "\033[0m";


// void get_indexed_words(const string& dbname) {
//     Xapian::Database db(dbname);
//     ofstream output("words.txt");
//     if (!output.is_open()) {
//         cerr << "Unable to open output file: words.txt" << endl;
//         exit(1);
//     }

//     Xapian::TermIterator term_iter = db.allterms_begin();
//     for (; term_iter != db.allterms_end(); ++term_iter) {
//         output << *term_iter << endl;
//     }
//     output.close();
// }



int random_number(int n) {
    std::random_device rd; // Random number generator device
    std::mt19937 gen(rd()); // Initialize Mersenne Twister random number generator
    std::uniform_int_distribution<> dist(0, n - 1); // Define the distribution range

    return dist(gen); // Generate a random number in the specified range
}


void get_indexed_words(const std::string& dbname, std::vector<std::string>& words) {
    Xapian::Database db(dbname);

    Xapian::TermIterator term_iter = db.allterms_begin();
    for (; term_iter != db.allterms_end(); ++term_iter) {
        words.push_back(*term_iter);
        // cout<<*term_iter<<endl;
    }
}

void searcher(string index_name, int top_k, vector<string> keywords, int flag) {
    bool isANDQuery;
    if(flag==0) {
        isANDQuery = false;
    } else {
        isANDQuery = true;
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
        cout << "mset size is " << matches.size() << endl;

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
            cout << name << endl;
            cout << description << endl;
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
            cout << "mset size is " << matches.size() << endl;

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
                cout << name << endl;
                cout << description << endl;
            }
    }
}


int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: <index_name>" << endl;
        exit(0);
    }
    std::vector<std::string> words;
    string dbname = argv[1];
    get_indexed_words(dbname, words);
    int n = words.size();

    int cardinality = 40;
    // cout << words.size()<<endl;

    double time = 0.0;
    for(int j = 0; j < 30; j++) {
        vector<string> strs;
        for(int i = 0; i < cardinality; i++) {
            int random_num = random_number(n);
            // cout << words[random_num] << endl;
            strs.emplace_back(words[random_num]);
            
            // cout << random_num << endl;
        }
        auto startTime = system_clock::now();
        searcher(dbname, 40, strs, 0);
        auto endTime = system_clock::now();
        auto duration = duration_cast<microseconds>(endTime - startTime);

        time += double(duration.count()) * microseconds::period::num / microseconds::period::den;

    }
    cout<<"Average time: " << time/30 << "s" << endl;
    
    return 0;

}
