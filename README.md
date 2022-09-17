# simplejson
a simple c++ json class to load/access/modify/dump json

# usage
## Example 1

This example show how to create an instance, access elements, and dump to file([3.json](#j3))
```C++
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
    fo.open("d:/3.json", std::ios::out);
    if (fo.is_open()) {
        std::string dumps = book.dumps();
        fo.write(dumps.c_str(), dumps.size());
        fo.close();
    }
}

```
## Load, access, dump
This example show how to load a .json file([1.json](#j1)), access elements, and dump to file([2.json](#j2))
``` C++
void testLoadDump() {
    JsonParser parser;
    const char fn[] = "d:/1.json";
    const char outfn[] = "d:/2.json";
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
```


<span id="j1">file 1.json</span>
```json
{
    "name": "Cxy",
    "age": 10,
    "Address": {
        "province": "Guangdong",
        "city": "Guangzhou"
    },
    "courses": ["Math", "英语", "Others", {"dir":1, "d": 2}, [1,2,3,4]]
}
```
<span id="j2">file 2.json</span>
```json
{
	"Address": {
		"city": "Guangzhou",
		"province": "Guangdong"
	},
	"age": 10,
	"courses": ["Math","英语","Others",{
			"d": 2,
			"dir": 1
		},[1,2,3,4]],
	"name": "Cxy"
}
```
<span id="j3">file 3.json</span>
```json
{
	"borrowers": [{
			"borrowed date": "2019-03-03",
			"name": "David"
		},{
			"borrowed date": "2019-03-03",
			"name": "David"
		}],
	"date": "2012-03-03",
	"name": ["Li","Zhang",{
			"First-Name": "David",
			"Last-name": "Cao"
		}],
	"null-example": null,
	"on-hand quantity": 5,
	"position": {
		"bookshelf": 3,
		"building-no": 1,
		"floor": 9.990000,
		"test": {
			"First-Name": "David",
			"Last-name": "Cao"
		}
	},
	"price": 63.500000,
	"title": "Hello"
}
```
