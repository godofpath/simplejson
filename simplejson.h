#ifndef SIMPLEJSON_H
#define SIMPLEJSON_H

#include <cstring>
#include <map>
#include <vector>
#include <memory>
#include <list>
#include <utility>
#include <stdint.h>

enum JsonType {
    JTInt,
    JTDouble,
    JTString,
    JTBoolean,
    JTArr,
    JTObj,
    JTNull,
    JTUnknown
};
class JsonParser;
struct JsonValue {
protected:
    union {
        bool m_b;
        long m_i;
        double m_d;
        char * m_str = nullptr;
    } m_value;
    JsonType m_type = JTUnknown;
private:
    std::map<std::string, JsonValue*> m_dict;
    std::vector<JsonValue*> m_arr;
    JsonValue* clone() const {
        JsonValue * res = new JsonValue;
        res->m_value = m_value;
        res->m_type = m_type;
        if (m_type == JTString && m_value.m_str) {
            res->m_value.m_str = new char[strlen(m_value.m_str)+1];
            strcpy(res->m_value.m_str, m_value.m_str);
        } else if (JTArr == m_type) {
            for (auto& v: m_arr) {
                JsonValue* nv = v->clone();
                res->m_arr.push_back(nv);
            }
        } else if (JTObj == m_type) {
            for (auto& v: m_dict) {
                JsonValue* nv = v.second->clone();
                res->m_dict[v.first] = nv;
            }
        }
        return res;
    }
    void clearData() {
        if (m_type == JTString && m_value.m_str) {
            delete[] m_value.m_str;
            m_value.m_str = nullptr;
        } else if (JTArr == m_type) {
            for (auto& v: m_arr) {
                delete v;
            }
            m_arr.clear();
        } else if (JTObj == m_type) {
            for (auto& v: m_dict) {
                delete v.second;
            }
            m_dict.clear();
        }
    }
public:
    friend class JsonParser;
    JsonValue(JsonType t =JTObj) {m_type = t;}
    JsonValue(long i) {
        m_type = JTInt;
        m_value.m_i = i;
    }
    JsonValue(int i) {
        m_type = JTInt;
        m_value.m_i = i;
    }
    JsonValue(const char* str) {
        m_type = JTString;
        m_value.m_str = new char[strlen(str)+1];
        std::strcpy(m_value.m_str, str);
    }
    JsonValue(bool b) {
        m_type = JTBoolean;
        m_value.m_b = b;
    }
    JsonValue(double d) {
        m_type = JTDouble;
        m_value.m_d = d;
    }
    JsonValue(const JsonValue& right) {
        m_value = right.m_value;
        m_type = right.m_type;
        if (m_type == JTString && right.m_value.m_str) {
            m_value.m_str = new char[strlen(right.m_value.m_str)+1];
            std::strcpy(m_value.m_str, right.m_value.m_str);
        } else if (JTArr == m_type) {
            for (auto& v: right.m_arr) {
                m_arr.push_back(v->clone());
            }
        } else if (JTObj == m_type) {
            for (auto& v: right.m_dict) {
                m_dict[v.first] = v.second->clone();
            }
        }
    }
    JsonValue& operator=(const JsonValue&right) {
        if (this == &right) {
            return *this;
        }
        clearData();
        m_value = right.m_value;
        m_type = right.m_type;
        if (m_type == JTString) {
            if (right.m_value.m_str) {
                m_value.m_str = new char[strlen(right.m_value.m_str)+1];
                std::strcpy(m_value.m_str, right.m_value.m_str);
            }
        } else if (JTArr == m_type) {
            for (JsonValue* v: right.m_arr) {
                m_arr.push_back(v->clone());
            }
        } else if (JTObj == m_type) {
            for (auto& v: right.m_dict) {
                m_dict[v.first] = v.second->clone();
            }
        }
        return *this;
    }
    JsonValue& operator=(JsonValue&&right) {
        if (this==&right) {
            return *this;
        }
        clearData();
        m_type = right.m_type;
        m_value = right.m_value;
        m_dict = right.m_dict;
        m_arr = right.m_arr;

        right.m_value.m_str = nullptr;
        right.m_dict.clear();
        right.m_arr.clear();
        return *this;
    }
    JsonValue(JsonValue&&right) {
        m_type = right.m_type;
        m_value = right.m_value;
        m_dict = right.m_dict;
        m_arr = right.m_arr;

        right.m_value.m_str = nullptr;
        right.m_dict.clear();
        right.m_arr.clear();
    }

    virtual ~JsonValue() {
        clearData();
    }
private:
    void setType(JsonType jt) {
        m_type = jt;
    }
    void set(long i) {
        m_type = JTInt;
        m_value.m_i = i;
    }
    void set(bool b) {
        m_type = JTBoolean;
        m_value.m_b = b;
    }

    void set(double d) {
        m_type = JTDouble;
        m_value.m_d = d;
    }

    void set(const char* str, int n) {
        if (!str) {
            return;
        }
        m_type = JTString;
        m_value.m_str = new char[n+1];
        m_value.m_str[n] = 0;
        memcpy(m_value.m_str, str, n);
    }
    bool addObj(const std::string& key, JsonValue* value) {
        if (m_type != JTObj) return false;
        m_dict[key] = value;
        return true;
    }
    bool addObj(JsonValue* value) {
        if (m_type != JTArr) return false;
        m_arr.push_back(value);
        return true;
    }
public:
    JsonType type() const noexcept {return m_type;}
    bool toBoolean() const noexcept {
        return m_value.m_b;
    }
    long toInt() const noexcept {
        return m_value.m_i;
    }
    const char* toString() const noexcept {
        return m_value.m_str;
    }

    double toDouble() const noexcept {
        return m_value.m_d;
    }
    JsonValue& toJsonArray() {
        if (m_type != JTArr) {
            throw("obj is not array");
        }
        return *this;
    }
    JsonValue& toJsonObject(){
        if (m_type != JTObj) {
            throw("obj is not object");
        }
        return *this;
    }
    void set() {
        m_type = JTNull;
    }

    bool append(const char* value) {
        if (m_type != JTArr) return false;
        m_arr.push_back(new JsonValue(value));
        return true;
    }
    bool append(long value) {
        if (m_type != JTArr) return false;
        m_arr.push_back(new JsonValue(value));
        return true;
    }
    bool append(bool value) {
        if (m_type != JTArr) return false;
        m_arr.push_back(new JsonValue(value));
        return true;
    }
    bool append(double value) {
        if (m_type != JTArr) return false;
        m_arr.push_back(new JsonValue(value));
        return true;
    }
    bool append(const JsonValue& value) {
        if (m_type != JTArr) return false;
        m_arr.push_back(value.clone());
        return true;
    }
    bool append(JsonValue&& value) {
        if (m_type != JTArr) return false;
        m_arr.push_back(new JsonValue(std::forward<JsonValue&&>(value)));
        return true;
    }
    bool contain(const std::string& key) {
        if (JTObj == m_type) {
            decltype(m_dict.begin()) itr;
            if ( (itr = m_dict.find(key)) != m_dict.end() ) {
                return true;
            }
        }
        return false;
    }
    JsonValue& operator[](const std::string& key) {
        if (JTObj == m_type) {
            decltype(m_dict.begin()) itr;
            if ( (itr = m_dict.find(key)) != m_dict.end() ) {
                return *itr->second;
            } else {
                JsonValue* obj = new JsonValue;
                m_dict[key] = obj;
                return *obj;
            }
        }
        throw("key operation must with a JsonObj type.");
    }
    JsonValue& operator[](uint32_t index) {
        if (JTArr == m_type) {
            if ( m_arr.size() > index ) {
                return *m_arr[index];
            }
            throw("index out of bound.");
        }
        throw("index operation must with a JsonArr type.");
    }
    size_t size() const {
        if (JTArr == m_type) {
            return m_arr.size();
        }
        return -1;
    }
    bool isArrary() const {
        return m_type == JTArr;
    }
    bool isObj() const {
        return m_type == JTObj;
    }
    bool isString() const {
        return m_type == JTString;
    }
    bool isNull() const {
        return m_type == JTNull;
    }
    bool isInt() const {
        return m_type == JTInt;
    }
    bool isBool() const {
        return m_type == JTBoolean;
    }

    std::string dumps(int level = 1) const {
        // 缩进先不管
        std::string s;
        char buf[32];
        if (m_type == JTBoolean) {
            return m_value.m_b? "true": "false";
        } else if (m_type == JTInt) {
            sprintf(buf, "%ld", m_value.m_i);
            return buf;
        } else if (m_type == JTDouble) {
            sprintf(buf, "%f", m_value.m_d);
            return buf;
        } else if (m_type == JTString) {
            sprintf(buf, "\"%s\"", m_value.m_str);
            return buf;
        } else if (m_type == JTNull) {
            return "null";
        } else if (m_type == JTObj) {
            auto indentation = std::string(level, '\t');
            int count = 1;
            s += "{\n";
            for (auto itr = m_dict.begin(); itr != m_dict.end(); itr++) {
                s += (indentation);
                s += "\"" + itr->first + "\": ";
                s += itr->second->dumps(level+1);
                if (count < m_dict.size()) {
                    s += ",\n";
                }
                count++;
            }

            s += "\n";
            s += indentation.substr(0, indentation.size()-1);
            s += "}";
        } else if (m_type == JTArr) {
            auto indentation = std::string(level, '\t');
            int count = 1;
            s += "[";
            for (auto itr = m_arr.begin(); itr != m_arr.end(); itr++) {
                s += (*itr)->dumps(level+1);
                if (count < m_arr.size()) {
                    s += ",";
                }
                count++;
            }
            s += "]";
        }
        return s;
    }
};

using JsonObj   = JsonValue;
using JsonArray = JsonValue;




class JsonParser {
    enum ParseState {
        JsonStateUnknown,

        JsonStateArrBegin,
        JsonStateArrNeedCommaOrEnd,
        JsonStateArrNeedElement,
        JsonStateArrEnd,

        JsonStateObjBegin,
        JsonStateObjNeedAttr,
        JsonStateObjNeedCommaOrEnd,
        JsonStateObjNeedColon,
        JsonStateObjNeedValue,
        JsonStateObjEnd,
    };
    std::tuple<char*, int> parseString() {
        char * p = m_content;
        while (*m_content != '\0' && *m_content != '"') {
            if (*m_content == '\\') {
                m_content++;
                if (!(*m_content=='\\' || *m_content=='*' || *m_content=='t' || *m_content=='r' ||
                    *m_content=='n' || *m_content=='b' || *m_content=='"')) {
                    return std::make_tuple(nullptr, 0);
                }
            }
            m_content++;
        }
        if (*m_content != '"') {
            return std::make_tuple(nullptr, 0);
        } else {
            m_content++;
            return std::make_tuple(p, m_content-p-1);
        }

    }
    JsonValue* parseStringAsObj() {
        char * p; int n;
        std::tie(p, n) = parseString();
        if (p) {
            return nullptr;
        } else {
            JsonValue* obj = new JsonValue;
            obj->setType(JTString);
            obj->set(p, n);
            return obj;
        }

    }

    JsonValue* parseNumber() {
        int dots = 0, es = 0;
        char * p = m_content;
        while(*p != 0) {
            if (*p == '.') {
                dots++;
            } else if (*p == 'e') {
                es++;
            } else if (*p == ' ' || *p == '\r'
                       || *p == '\n'|| *p == ','|| *p == ']'|| *p == '}') {
               break;
            } else if (*p < '0' || *p > '9') {
                return nullptr;
            }
            m_posInLine++;
            p++;
        }
        if (dots > 1 || es > 1) {
            return nullptr;
        }
        char tmp;
        if (dots > 0) {
            tmp = *p;
            *p = 0;
            JsonValue* jv = new JsonValue;
            jv->set(std::atof(m_content));
            *p = tmp;
            m_content = p;
            return jv;
        } else {
            tmp = *p;
            *p = 0;
            JsonValue* jv = new JsonValue;
            jv->set(std::atol(m_content));
            *p = tmp;
            m_content = p;
            return jv;
        }
    }

    JsonValue* parseBool() {
        JsonValue* obj = nullptr;
        if (memcmp(m_content, "true", 4)) {
            obj = new JsonValue;
            obj->set(true);
            m_content += 4;
            m_posInLine += 4;
        } else if (memcmp(m_content, "false", 5)) {
            obj = new JsonValue;
            obj->set(false);
            m_content += 5;
            m_posInLine += 5;
        }
        return obj;
    }

    JsonValue* parseNull() {
        JsonValue* obj = nullptr;
        if (memcmp(m_content, "null", 4)) {
            obj = new JsonValue;
            obj->set();
            m_content += 4;
            m_posInLine += 4;
        }
        return obj;
    }

    JsonValue* parseArr() {
        ParseState state = JsonStateArrBegin;
        std::unique_ptr<JsonValue> arr(new JsonValue);
        arr->setType(JTArr);
        while(*m_content) {
            if (*m_content == '\n') {
                m_lines++;
                m_posInLine = 0;
                m_content++;
                continue;
            } else if (*m_content == '\t' || *m_content == ' ' || *m_content == '\r') {
                m_posInLine++;
                m_content++;
                continue;
            }
            if (state == JsonStateArrNeedElement || state == JsonStateArrBegin) {
                if (*m_content == 'n') {
                    auto obj = parseNull();
                    if (obj) {
                        arr->append(obj);
                    } else {
                        return nullptr;
                    }
                    state = JsonStateArrNeedCommaOrEnd;
                    continue;
                } else if (*m_content == 't' || *m_content == 'f') {
                    auto obj = parseBool();
                    if (obj) {
                        arr->addObj(obj);
                    } else {
                        return nullptr;
                    }
                    state = JsonStateArrNeedCommaOrEnd;
                    continue;
                } else if (*m_content == '"') {
                    m_content++;
                    m_posInLine++;
                    char * p;
                    int len;
                    std::tie(p, len) = parseString();
                    if (p) {
                        JsonValue* obj = new JsonValue;
                        obj->set(p, len);
                        arr->addObj(obj);
                    } else {
                        return nullptr;
                    }
                    state = JsonStateArrNeedCommaOrEnd;
                    continue;
                } else if (*m_content == '-' || (*m_content <= '9' && *m_content >= '0')) {
                    auto obj = parseNumber();
                    if (obj) {
                        arr->addObj(obj);
                    } else {
                        return nullptr;
                    }
                    state = JsonStateArrNeedCommaOrEnd;
                    continue;
                } else if (*m_content == '[') {
                    m_content++;
                    m_posInLine++;
                    auto obj = parseArr();
                    if (obj) {
                        arr->addObj(obj);
                    } else {
                        return nullptr;
                    }
                    state = JsonStateArrNeedCommaOrEnd;
                    continue;
                } else if (*m_content == '{') {
                    m_content++;
                    m_posInLine++;
                    auto obj = parseObj();
                    if (obj) {
                        arr->addObj(obj);
                    } else {
                        return nullptr;
                    }
                    state = JsonStateArrNeedCommaOrEnd;
                    continue;
                } else if (*m_content == ']') {
                    if (state == JsonStateArrBegin) {
                        state = JsonStateArrEnd;
                        m_content++;
                        m_posInLine++;
                        break;
                    } else {
                        return nullptr;
                    }
                } else {
                    return nullptr;
                }
            } else if (state == JsonStateArrNeedCommaOrEnd) {
                if (*m_content == ',') {
                    state = JsonStateArrNeedElement;
                    m_content++;
                } else if (*m_content == ']') {
                    state = JsonStateArrEnd;
                    m_content++;
                    break;
                } else {
                    return nullptr;
                }
            }
        }
        if (state == JsonStateArrEnd) {
            return arr.release();
        } else {
            return nullptr;
        }
    }

    JsonValue* parseObj() {
        ParseState state = JsonStateObjBegin;
        std::unique_ptr<JsonValue> obj(new JsonValue);
        obj->setType(JTObj);
        char key[128];
        while(*m_content) {
            if (*m_content == '\n') {
                m_lines++;
                m_posInLine = 0;
                m_content++;
                continue;
            } else if (*m_content == '\t' || *m_content == ' ' || *m_content == '\r') {
                m_posInLine++;
                m_content++;
                continue;
            }
            if (state == JsonStateObjNeedAttr || state == JsonStateObjBegin) {
                if (*m_content == '"') {
                    m_content++;
                    m_posInLine++;
                    // char* p;
                    // int len;
                    // std::tie(p, len) = parseString();
                    auto [p, len] = parseString();
                    if (p) {
                        memcpy(key, p, len);
                        key[len] = 0;
                        state = JsonStateObjNeedColon;
                        continue;
                    }
                } else if (*m_content == '}') {
                    if (state == JsonStateObjBegin) {
                        m_content++;
                        m_posInLine++;
                        state = JsonStateObjEnd;
                        break;
                    }
                } else {
                    return nullptr;
                }
            } else if (state == JsonStateObjNeedColon) {
                if (*m_content == ':') {
                    state = JsonStateObjNeedValue;
                    m_content++;
                    m_posInLine++;
                } else {
                    return nullptr;
                }
            } else if (state == JsonStateObjNeedValue) {
                if (*m_content == 'n') {
                    auto o = parseNull();
                    if (o) {
                        obj->addObj(key, o);
                    } else {
                        return nullptr;
                    }
                    state = JsonStateObjNeedCommaOrEnd;
                    continue;
                } else if (*m_content == 't' || *m_content == 'f') {
                    auto b = parseBool();
                    if (b) {
                        obj->addObj(key, b);
                    } else {
                        return nullptr;
                    }
                    state = JsonStateObjNeedCommaOrEnd;
                    continue;
                } else if (*m_content == '"') {
                    m_content++;
                    m_posInLine++;
                    char * p;
                    int len;
                    std::tie(p, len) = parseString();
                    if (p) {
                        JsonValue* s = new JsonValue;
                        s->set(p, len);
                        obj->addObj(key, s);
                    } else {
                        return nullptr;
                    }
                    state = JsonStateObjNeedCommaOrEnd;
                    continue;
                } else if (*m_content == '-' || (*m_content <= '9' && *m_content >= '0')) {
                    auto num = parseNumber();
                    if (num) {
                        obj->addObj(key, num);
                    } else {
                        return nullptr;
                    }
                    state = JsonStateObjNeedCommaOrEnd;
                    continue;
                } else if (*m_content == '[') {
                    m_content++;
                    m_posInLine++;
                    auto arr = parseArr();
                    if (arr) {
                        obj->addObj(key, arr);
                    } else {
                        return nullptr;
                    }
                    state = JsonStateObjNeedCommaOrEnd;
                    continue;
                } else if (*m_content == '{') {
                    m_content++;
                    m_posInLine++;
                    auto o = parseObj();
                    if (o) {
                        obj->addObj(key, o);
                    } else {
                        return nullptr;
                    }
                    state = JsonStateObjNeedCommaOrEnd;
                    continue;
                }

            } else if (state == JsonStateObjNeedCommaOrEnd) {
                m_posInLine++;
                if (*m_content == ',') {
                    state = JsonStateObjNeedAttr;
                    m_content++;
                } else if (*m_content == '}') {
                    state = JsonStateObjEnd;
                    m_content++;
                    break;
                } else {
                    return nullptr;
                }
            }
        }
        if (state == JsonStateObjEnd) {
            return obj.release();
        } else {
            return nullptr;
        }
    }

    const char* errorMsg() {
        return m_msg;
    }
public:
    JsonValue load(const char* content, std::string& msg) {
        msg = "";
        m_content = const_cast<char*>(content);
        // std::vector<State> states(100);
        // std::vector<JsonValue*> parentObj;
        JsonValue root;
        JsonValue * proot = nullptr;
        while (*m_content != 0) {
            if (*m_content == '{') {
                m_content++;
                m_posInLine++;
                proot = parseObj();
                break;
            } else if (*m_content == '[') {
                m_posInLine++;
                m_content++;
                proot = parseArr();
                break;
            } else if (*m_content == '\n') {
                m_lines++;
                m_posInLine = 0;
            } else if (*m_content == '\t' || *m_content == ' ' || *m_content == '\r') {
                m_posInLine++;
            } else {
                return root;
            }
            m_content++;
        }
        if (!proot) {
            genMsg();
            msg = m_msg;
            return JsonValue();
        }
        while (*m_content != 0) {
            if ((*m_content == '\n' || *m_content || '\t' || *m_content == ' ' || *m_content == '\r')) {
                m_content++;
            } else {
                genMsg();
                msg = m_msg;
                delete proot;
                proot = nullptr;
                return JsonValue();
            }
        }
        msg = m_msg;
        init();
        root = JsonValue(std::move(*proot));
        delete proot;
        return root;
    }
private:
    char *m_content = nullptr;
    int m_lines = 1;
    int m_posInLine = 1;
    char m_msg[64]={0};
    void init() {
        // m_content = nullptr;
        m_lines = 1;
        m_posInLine = 1;
    }
    void genMsg() {
        sprintf(m_msg, "bad format lines:%d, pos:%d", m_lines, m_posInLine);
    }
};   // Parser end

#endif // SIMPLEJSON_H

