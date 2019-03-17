#include "../test.h"


#include <Cerializer/RapidJsonObj.h>
#include "cerial_canada_struct.h"

#include <iostream>
#include <fstream>

struct T {
    bool b;
};

class CerializerRapidJsonParseResult : public ParseResultBase {
public:
    rapidjson::Document d;
    ObjectCollection c = ObjectCollection::fromJson(d);
};

class CerializerRapidJsonStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return "";/* s.constData();*/ }

    const std::string& s;
};

class CerializerRapidJsonTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "Qt (C++)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        BiggerObj b;
      

        std::ofstream outfile;
        outfile.open("generated.json");
        outfile << b.toJsonStr().c_str() << std::endl;

        outfile.close();

        /*QtParseResult* pr = new QtParseResult;
        QJsonParseError error;
        pr->d = QJsonDocument::fromJson(QByteArray(json, length), &error);
        if (error.error != QJsonParseError::NoError) {
            delete pr;
            return 0;
        }
        else
    	   return pr;*/
        return nullptr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
       /* const QtParseResult* pr = static_cast<const QtParseResult*>(parseResult);
        QtStringResult* sr = new QtStringResult;
        sr->s = pr->d.toJson(QJsonDocument::Compact);
        return sr;*/
        return nullptr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        /*const QtParseResult* pr = static_cast<const QtParseResult*>(parseResult);
        QtStringResult* sr = new QtStringResult;
        sr->s = pr->d.toJson(QJsonDocument::Indented);
        return sr;*/
        return nullptr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        /*const QtParseResult* pr = static_cast<const QtParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        if (pr->d.isObject())
            GenStat(*stat, pr->d.object());
        else if (pr->d.isArray())
            GenStat(*stat, pr->d.array());*/
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        /*QJsonDocument v = QJsonDocument::fromJson(QByteArray(json));
        *d = v.array()[0].toDouble();*/
        return true;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        /*QJsonDocument v = QJsonDocument::fromJson(QByteArray(json));
        s = v.array()[0].toString().toStdString();*/
        return true;
    }
#endif
};

REGISTER_TEST(CerializerRapidJsonTest);
