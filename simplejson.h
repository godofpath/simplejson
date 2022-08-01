#ifndef SIMPLEJSON_H
#define SIMPLEJSON_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <list>
#include <stdint.h>


enum JsonType {
    JTInt,
    JTDouble,
    JTString,
    JTBoolean,
    JTArr,
    JTObj,
    JTNull
};

struct JsonValue {
protected:
    union {
        bool m_b;
        int64_t m_i;
        double m_d;
        char * m_str = nullptr;
    } m_value;
    JsonType m_type;
public:
    JsonValue() {}
    JsonValue(const JsonValue&right) = delete;
    JsonValue& operator=(const JsonValue&right) = delete;

    virtual ~JsonValue() {
        if (JTObj == m_type) {
            for (auto v: m_dict) {
                delete v.second;
            }
        } else if (JTArr == m_type) {
            for (auto v: m_arr) {
                delete v;
            }
        } else if (JTString == m_type) {
            if (m_value.m_str) {
                delete []m_value.m_str;
            }
        }
    }
    void setType(JsonType jt) {
        m_type = jt;
    }
    JsonType type() const {return m_type;}
    void setInt(int64_t i) {
        m_type = JTInt;
        m_value.m_i = i;
    }
    int64_t toInt() const {
        return m_value.m_i;
    }
    void setBoolean(bool b) {
        m_type = JTBoolean;
        m_value.m_b = b;
    }
    bool toBoolean() const {
        return m_value.m_b;
    }
    void setDouble(double d) {
        m_type = JTDouble;
        m_value.m_d = d;
    }
    double toDouble() const {
        return m_value.m_d;
    }
    void setString(const char* str, int n) {
        m_type = JTString;
        m_value.m_str = new char[n+1];
        m_value.m_str[n] = 0;
        memcpy(m_value.m_str, str, n);
    }
    const char* toString() const {
        return m_value.m_str;
    }
    void setNull() {
        m_type = JTNull;
    }
    void addObj(const std::string& key, JsonValue* value) {
        m_dict[key] = value;
    }
    void addObj(JsonValue* value) {
        m_arr.push_back(value);
    }
    JsonValue* operator[](const std::string& key) const {
        if (JTObj == m_type) {
            decltype(m_dict.begin()) itr;
            if ( (itr = m_dict.find(key)) != m_dict.end() ) {
                return itr->second;
            } else {
                return nullptr;
            }
        }
        return nullptr;
    }
    JsonValue* operator[](uint32_t index) const {
        if (JTArr == m_type) {
            if ( m_arr.size() > index ) {
                return m_arr[index];
            } else {
                return nullptr;
            }
        }
        return nullptr;
    }
    int32_t count() const {
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

    std::string dumps(int level = 0) const {
        // 缩进先不管
        std::string s;
        char buf[32];
        if (m_type == JTBoolean) {
            return m_value.m_b? "true": "false";
        } else if (m_type == JTInt) {
            sprintf(buf, "%lld", m_value.m_i);
            return buf;
        } else if (m_type == JTDouble) {
            sprintf(buf, "%f", m_value.m_d);
            return buf;
        } else if (m_type == JTString) {
            return "\"" + std::string(m_value.m_str) + "\"";
        } else if (m_type == JTObj) {
            auto indentation = std::string(level, '\t');
            s += indentation;
            int count = 1;
            s += "{\n";
            for (auto itr = m_dict.begin(); itr != m_dict.end(); itr++) {
                s += (indentation + "\t");
                s += itr->first;
                s += ": ";
                s += itr->second->dumps();
                if (count < m_dict.size()) {
                    s += ",";
                }
                count++;
            }
            s = s.substr(0, s.size()-1);
            s += "\n}";
        } else if (m_type == JTArr) {
            auto indentation = std::string(level, '\t');
            s += indentation;
            int count = 1;
            s += "[\n";
            for (auto itr = m_arr.begin(); itr != m_arr.end(); itr++) {
                s += (*itr)->dumps();
                if (count < m_arr.size()) {
                    s += ",";
                }
                count++;
            }
            s = s.substr(0, s.size()-1);
            s += "\n]";
        }
        return s;
    }
private:
    std::map<std::string, JsonValue*> m_dict;
    std::vector<JsonValue*> m_arr;
};

using JsonObj   = JsonValue;
using JsonArray = JsonValue;




class JsonParser {
    enum ParseState {
        JsonStateUnknown,
//        JsonStateBeginAttr,
//        JsonStateEndAttr,
//        JsonStateBeginNumber,
//        JsonStateEndNumber,
//        JsonStateBeginStr,
//        JsonStateEndStr,
//        JsonStateBeginBoolean,
//        JsonStateEndBoolean,

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
            obj->setString(p, n);
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
            jv->setDouble(std::atof(m_content));
            *p = tmp;
            m_content = p;
            return jv;
        } else {
            tmp = *p;
            *p = 0;
            JsonValue* jv = new JsonValue;
            jv->setInt(std::atol(m_content));
            *p = tmp;
            m_content = p;
            return jv;
        }
    }

    JsonValue* parseBool() {
        JsonValue* obj = nullptr;
        if (memcmp(m_content, "true", 4)) {
            obj = new JsonValue;
            obj->setBoolean(true);
            m_content += 4;
            m_posInLine += 4;
        } else if (memcmp(m_content, "false", 5)) {
            obj = new JsonValue;
            obj->setBoolean(false);
            m_content += 5;
            m_posInLine += 5;
        }
        return obj;
    }

    JsonValue* parseNull() {
        JsonValue* obj = nullptr;
        if (memcmp(m_content, "null", 4)) {
            obj = new JsonValue;
            obj->setNull();
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
                        arr->addObj(obj);
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
                        obj->setString(p, len);
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
                    char* p;
                    int len;
                    std::tie(p, len) = parseString();
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
                        s->setString(p, len);
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
    JsonValue* load(const char* content, std::string& msg) {
        msg = "";
        m_content = const_cast<char*>(content);
        // std::vector<State> states(100);
        // std::vector<JsonValue*> parentObj;
        JsonValue* root = nullptr;

        while (*m_content != 0) {
            if (*m_content == '{') {
                m_content++;
                m_posInLine++;
                root = parseObj();
                break;
            } else if (*m_content == '[') {
                m_posInLine++;
                m_content++;
                root = parseArr();
                break;
            } else if (*m_content == '\n') {
                m_lines++;
                m_posInLine = 0;
            } else if (*m_content == '\t' || *m_content == ' ' || *m_content == '\r') {
                m_posInLine++;
            } else {
                return nullptr;
            }
            m_content++;
        }
        if (!root) {
            genMsg();
            msg = m_msg;
            return nullptr;
        }
        while (*m_content != 0) {
            if ((*m_content == '\n' || *m_content || '\t' || *m_content == ' ' || *m_content == '\r')) {
                m_content++;
            } else {
                genMsg();
                msg = m_msg;
                delete root;
                root = nullptr;
                return nullptr;
            }
        }
        msg = m_msg;
        init();
        return root;
    }
private:
    char *m_content = nullptr;
    int m_lines = 1;
    int m_posInLine = 1;
    char m_msg[32];
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

