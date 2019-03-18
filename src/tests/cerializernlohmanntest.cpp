
#include "../test.h"
#include "nlohmann/src/json.hpp"
#include "NlohmannJsonObjects.h"

using namespace nlohmann;

class CerializerNlohmannParseResult : public ParseResultBase {
public:
    ~CerializerNlohmannParseResult() {
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

class CerializerNlohmannStringResult : public StringResultBase {
public:
    CerializerNlohmannStringResult() = default;
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};

class CerializerNlohmannTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "Cerializer Nlohmann (C++11)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* j, size_t length, const char* jsonFileName) const {
        (void)length;
        CerializerNlohmannParseResult* pr = new CerializerNlohmannParseResult;
        try {
            json root = json::parse(j);
            if (strcmp(jsonFileName, "big.json") == 0) {
                pr->bigObj = new BigObj((BigObj::fromJson(root)));
            }
            else if (strcmp(jsonFileName, "bigger.json") == 0) {
                pr->biggerObj = new BiggerObj((BiggerObj::fromJson(root)));
            }
        }
        catch (...) {
            delete pr;
            return 0;
        }

    	return pr;
    }
#endif
    /*
#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult, const char* jsonFileName) const {
        (void)jsonFileName;
        const CerializerNlohmannParseResult* pr = static_cast<const CerializerNlohmannParseResult*>(parseResult);
        CerializerNlohmannStringResult* sr = new CerializerNlohmannStringResult;
        //sr->s = pr->root.dump();
        return sr;
    }
#endif*/
};

REGISTER_TEST(CerializerNlohmannTest);
