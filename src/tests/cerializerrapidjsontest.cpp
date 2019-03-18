#include "../test.h"


#include <Cerializer/RapidJsonObj.h>
#include "RapidJsonObjects.h"

#include <iostream>
#include <fstream>

class CerializerRapidJsonParseResult : public ParseResultBase {
public:
    ~CerializerRapidJsonParseResult() {
        if (bigObj) {
            delete bigObj;
        }
        if (biggerObj) {
            delete biggerObj;
        }
    }
    BigObj* bigObj{};
    BiggerObj* biggerObj{};
};

class CerializerRapidJsonStringResult : public StringResultBase {
public:
    CerializerRapidJsonStringResult() = default;
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};

class CerializerRapidJsonTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "Cerializer RapidJson (C++)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length, const char* jsonFileName) const {
        (void)length;
        CerializerRapidJsonParseResult* pr = new CerializerRapidJsonParseResult();
        
        rapidjson::Document d;
        d.Parse(json);

        if (d.HasParseError()) {
            delete pr;
            return nullptr;
        }

       if (strcmp(jsonFileName, "big.json") == 0) {
            pr->bigObj = new BigObj((BigObj::fromJson(d)));
        }
        else if (strcmp(jsonFileName, "bigger.json") == 0) {
            pr->biggerObj = new BiggerObj((BiggerObj::fromJson(d)));
        }
        
        return pr;
    }
#endif
/*
#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult, const char* jsonFileName) const {
        const CerializerRapidJsonParseResult* pr = static_cast<const CerializerRapidJsonParseResult*>(parseResult);
        CerializerRapidJsonStringResult* sr = new CerializerRapidJsonStringResult();

        if (strcmp(jsonFileName, "big.json") == 0) {
            sr->s = pr->bigObj->toJsonStr();
        }
        else if (strcmp(jsonFileName, "bigger.json") == 0) {
            sr->s = pr->biggerObj->toJsonStr();
        }

        return sr;
    }
#endif*/
};

REGISTER_TEST(CerializerRapidJsonTest);
