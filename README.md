# simplejson
a simple c++ json class to load or dump json 

# usage
json example:
```
{
    "msg": "hello",
    "ivalue": 10,
    "doublevalue": 100.9,
    "arr": ["dddd", 30, {}],
    "arr1": ["ddd", 999.09, {"l1k": "l1v", "l2k":[1,2,3,4,5,6]}],
    "arr2": ["ddd", 999.09, {"l1k": "l1v", "l2k":[1,2,3,4,5,6]}],
    "key" : {
        "k": "v"
    }
}
```
c++ example:
```
JsonParser p;
JsonValue* j;
JsonParser p;

j = p.load(jsonstr, msg);
JsonValue& jr = *j;
jr["msg"]->toString();
jr["ivalue"]->toInt();
jr["doublevalue"]->toDouble();

JsonArray& arr = *jr["arr"];
arr[0].toString();
jr.dumps().c_str();
delete j;

```
