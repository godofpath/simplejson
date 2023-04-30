#include <iostream>
#include <fstream>
#include "simplejson.h"

using namespace std;

void testModifyAccess() {
    JsonObj book;
    book["title"] = "Hello";
    book["date"] = "2012-03-03";
    book["on-hand quantity"] = 5;
    book["price"] = 63.5;
    book["null-example"] = JsonValue(JTNull);
    JsonArray borrowers(JTArr);
    JsonObj b1;
    b1["name"] = "David";
    b1["borrowed date"] = "2019-03-03";
    borrowers.append(std::move(b1));
    JsonObj b2;
    b2["name"] = "David";
    b2["borrowed date"] = "2019-03-03";
    borrowers.append(std::move(b2));
    book["borrowers"] = borrowers;
    JsonObj position;
    position["building-no"] = 1;
    position["floor"] = 2;
    position["floor"] = 9.99;
    position["bookshelf"] = 3;

    JsonArray authors(JTArr);
    authors.append("Li");
    authors.append("Zhang");
    JsonObj longName;
    longName["First-Name"] = "David";
    longName["Last-name"] = "Cao";
    authors.append(longName);
    book["name"] = authors;
    position["test"] = longName;

    book["position"] = position;
    cout << position.dumps();
    cout.flush();
    std::ofstream fo;
    fo.open("./3.json", std::ios::out);
    if (fo.is_open()) {
        std::string dumps = book.dumps();
        fo.write(dumps.c_str(), dumps.size());
        fo.close();
    }
}

void testLoadDump() {
    JsonParser parser;
    const char fn[] = "./1.json";
    const char outfn[] = "./2.json";
    std::ifstream fin;
    fin.open(fn, std::ios::in);

    if (fin.is_open()) {
        std::string msg;
        fin.seekg(0,std::ios::end);
        int len = fin.tellg();
        fin.seekg(0,std::ios::beg);

        std::unique_ptr<char[]> buff(new char[len+1]());

        fin.read(buff.get() , len);
        fin.close();
        JsonObj obj = parser.load(buff.get(), msg);
        if (!msg.empty()) {
            cout << "parse error:" << msg.c_str();
        } else {
            // how to access elements in Json
            cout << "name is " << obj["name"].toString() << " age is " << obj["age"].toInt() << " courses:";
            JsonArray& courses = obj["courses"].toJsonArray();
            for (int i = 0; i < courses.size(); i++) {
                if (courses[i].isString()) {
                    cout << courses[i].toString() << endl;
                }
            }
            // Write to another file
            std::ofstream fo;
            fo.open(outfn, std::ios::out);
            if (fo.is_open()) {
                std::string dumps = obj.dumps();
                fo.write(dumps.c_str(), dumps.size());
                fo.close();
            }
        }        
        cout.flush();
    }
}

int main() {
    testModifyAccess();
    testLoadDump();
    return 0;
}
