#include "../test.h"

//#if HAS_CPPREST

#include <cpprest/json.h>
#include "CpprestJsonObjects.h"

#include <strstream>
#include <sstream>

using namespace web::json;
using namespace utility::conversions;

class CerializerCpprestsdkParseResult : public ParseResultBase {
public:
    ~CerializerCpprestsdkParseResult() {
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

class CerializerCpprestsdkStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};

class CerializerCpprestsdkTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "Cerializer C++ REST SDK (C++14)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif
	
#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length, const char* jsonFileName) const {
        (void)length;
        CerializerCpprestsdkParseResult* pr = new CerializerCpprestsdkParseResult;
		std::istrstream is (json);
        try {
            if (strcmp(jsonFileName, "big.json") == 0) {
                pr->bigObj = new BigObj((BigObj::fromJson(value::parse(is))));
            }
            else if (strcmp(jsonFileName, "bigger.json") == 0) {
                pr->biggerObj = new BiggerObj((BiggerObj::fromJson(value::parse(is))));
            }
        }
        catch (web::json::json_exception& e) {
            printf("Parse error '%s'\n", e.what());
            delete pr;
            pr = 0;
        }
        catch (...) {
            delete pr;
            pr = 0;
        }
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult, const char* jsonFileName) const {
        (void)jsonFileName;
        const CerializerCpprestsdkParseResult* pr = static_cast<const CerializerCpprestsdkParseResult*>(parseResult);
        CerializerCpprestsdkStringResult* sr = new CerializerCpprestsdkStringResult;

        std::ostringstream os;
        if (strcmp(jsonFileName, "big.json") == 0) {
            pr->bigObj->toJson().serialize(os);
        }
        else if (strcmp(jsonFileName, "bigger.json") == 0) {
            pr->biggerObj->toJson().serialize(os);
        }

        sr->s = os.str();
        return sr;
    }
#endif
};

REGISTER_TEST(CerializerCpprestsdkTest);

//#endif