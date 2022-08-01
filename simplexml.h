#ifndef SIMPLEXML_H
#define SIMPLEXML_H

#include <string.h>
#include <stdint.h>
#include <map>
#include <list>

namespace SimpleXml {
using namespace std;
struct ptrCmp
{
    bool operator()( const char * s1, const char * s2 ) const
    {
        return strcmp( s1, s2 ) < 0;
    }
};

class XmlNode {
private:
    XmlNode(const XmlNode&) {}
    XmlNode& operator= (const XmlNode&) {return *this;}
public:
    explicit XmlNode(const char*name) {
        nextSibling = nullptr;
        m_name = new char[strlen(name)+1];
        strcpy(m_name, name);
        m_text = nullptr;
    }
    void setSibling(XmlNode* node) {
        nextSibling = node;
    }
    XmlNode* sibling() {
        return nextSibling;
    }
    ~XmlNode () {
        if (m_text) delete[] m_text;
        if (m_name) delete[] m_name;
        for (XmlNode* c: m_childs) {
            delete c;
        }
    }
    void setText(const char* text) {
        if (m_text) {
            delete [] m_text;
        }
        m_text = new char[strlen(text)+1];
        strcpy(m_text, text);
    }
    const char* text() {
        return m_text;
    }
    bool attr(const char* key, int32_t& value) const {
        return true;
    }
    bool attr(const char* key, uint32_t& value) const {
        return true;
    }
    bool attr(const char* key, int64_t& value) const {
        return true;
    }
    bool attr(const char* key, uint64_t& value) const {
        return true;
    }
    bool attr(const char* key, char* value) const {
        return true;
    }
    bool attr(const char* key, bool& value) const {
        return true;
    }
    bool attr(const char* key, float& value) const {
        return true;
    }
    bool attr(const char* key, double& value) const {
        return true;
    }

    bool setAttr(const char* key, int32_t value) const {
        return true;
    }
    bool setAttr(const char* key, uint32_t value) const {
        return true;
    }
    bool setAttr(const char* key, int64_t value) const {
        return true;
    }
    bool setAttr(const char* key, uint64_t value) const {
        return true;
    }
    bool setAttr(const char* key, char* value) const {
        return true;
    }
    bool setAttr(const char* key, bool value) const {
        return true;
    }
    bool setAttr(const char* key, float value) const {
        return true;
    }
    bool setAttr(const char* key, double value) const {
        return true;
    }
    void addChild(XmlNode* node) {
        if (node)   m_childs.push_back(node);
    }
    void removeChild(XmlNode* node) {
        if (node) m_childs.remove(node);
    }
private:
    char * m_name;
    char * m_text;
    map<char*, char*, ptrCmp> m_attibutes;
    list<XmlNode*> m_childs;
    XmlNode* nextSibling;
};

class XmlDocument {
public:
    explicit XmlDocument() {
        m_root = nullptr;
    }
    bool load(const char* filename) {
        return true;
    }
    bool loads(const char* content) {
        return true;
    }
    XmlNode* root() {
        return m_root;
    }
private:
    char * m_encoding;
    float version;
    bool standalone;
    XmlNode* m_root;
};
}

#endif // SIMPLEXML_H
