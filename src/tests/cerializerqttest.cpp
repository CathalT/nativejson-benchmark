#include "../test.h"

//#if HAS_QT
#include "QtJsonObjects.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>


class CerializerQtParseResult : public ParseResultBase {
public:
    ~CerializerQtParseResult() {
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

class CerializerQtStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.constData(); }

    QByteArray s;
};

class CerializerQtTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "Cerializer Qt (C++)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length, const char* jsonFileName) const {
        CerializerQtParseResult* pr = new CerializerQtParseResult;
        QJsonParseError error;
        QJsonDocument d = QJsonDocument::fromJson(QByteArray(json, length), &error);

        if (strcmp(jsonFileName, "big.json") == 0) {
            pr->bigObj = new BigObj(BigObj::fromJson(d.object()));
        }
        else if (strcmp(jsonFileName, "bigger.json") == 0) {
            pr->biggerObj = new BiggerObj(BiggerObj::fromJson(d.object()));
        }

        if (error.error != QJsonParseError::NoError) {
            delete pr;
            return 0;
        }
        else
    	   return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult, const char* jsonFileName) const {
        (void)jsonFileName;
        const CerializerQtParseResult* pr = static_cast<const CerializerQtParseResult*>(parseResult);
        CerializerQtStringResult* sr = new CerializerQtStringResult;

        if (strcmp(jsonFileName, "big.json") == 0) {
            QJsonDocument doc(pr->bigObj->toJson());
            sr->s = doc.toJson(QJsonDocument::Compact);
        }
        else if (strcmp(jsonFileName, "bigger.json") == 0) {
            QJsonDocument doc(pr->biggerObj->toJson());
            sr->s = doc.toJson(QJsonDocument::Compact);
        }

        return sr;
    }
#endif
};

REGISTER_TEST(CerializerQtTest);

//#endif
