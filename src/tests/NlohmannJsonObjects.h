#include <Cerializer/NlohmannJsonObj.h>

#include <tuple>

struct StringsStruct : public Cerial::NlohmannJsonObj<StringsStruct>
{
    StringsStruct() = default;
    StringsStruct(bool generatedata) {
        (void)generatedata;
        static int ssInt = 0;
        _205705993 = "_205705993" + std::to_string(ssInt);
        _205705994 = "_205705994" + std::to_string(ssInt);
        _205705995 = "_205705995" + std::to_string(ssInt);
        _205705996 = "_205705996" + std::to_string(ssInt);
        _205705998 = "_205705998" + std::to_string(ssInt);
        _205705999 = "_205705999" + std::to_string(ssInt);
        _205706000 = "_205706000" + std::to_string(ssInt);
        _205706001 = "_205706001" + std::to_string(ssInt);
        _205706002 = "_205706002" + std::to_string(ssInt);
        _205706003 = "_205706003" + std::to_string(ssInt);
        _205706004 = "_205706004" + std::to_string(ssInt);
        _205706005 = "_205706005" + std::to_string(ssInt);
        _205706006 = "_205706006" + std::to_string(ssInt);
        _205706007 = "_205706007" + std::to_string(ssInt);
        _205706008 = "_205706008" + std::to_string(ssInt);
        _205706009 = "_205706009" + std::to_string(ssInt);
        _342752287 = "_342752287" + std::to_string(ssInt);
        ssInt++;
    }

    std::string _205705993;
    std::string _205705994;
    std::string _205705995;
    std::string _205705996;
    std::string _205705998;
    std::string _205705999;
    std::string _205706000;
    std::string _205706001;
    std::string _205706002;
    std::string _205706003;
    std::string _205706004;
    std::string _205706005;
    std::string _205706006;
    std::string _205706007;
    std::string _205706008;
    std::string _205706009;
    std::string _342752287;

    S_PROPERTIES_BEGIN
        property(&StringsStruct::_205705993,"205705993"),
        property(&StringsStruct::_205705994, "205705994"),
        property(&StringsStruct::_205705995, "205705995"),
        property(&StringsStruct::_205705996, "205705996"),
        property(&StringsStruct::_205705998, "205705998"),
        property(&StringsStruct::_205705999, "205705999"),
        property(&StringsStruct::_205706000, "205706000"),
        property(&StringsStruct::_205706001, "205706001"),
        property(&StringsStruct::_205706002, "205706002"),
        property(&StringsStruct::_205706003, "205706003"),
        property(&StringsStruct::_205706004, "205706004"),
        property(&StringsStruct::_205706005, "205706005"),
        property(&StringsStruct::_205706006, "205706006"),
        property(&StringsStruct::_205706007, "205706007"),
        property(&StringsStruct::_205706008, "205706008"),
        property(&StringsStruct::_205706009, "205706009"),
        property(&StringsStruct::_342752287, "342752287")
    S_PROPERTIES_END
};

struct SingleStringObj : public Cerial::NlohmannJsonObj<SingleStringObj>
{
    SingleStringObj() = default;
    SingleStringObj(bool generatedata) {
        (void)generatedata;
        static int ssO = 0;
        _337100890 = "_337100890" + std::to_string(ssO);
        ssO++;
    }
    std::string _337100890;

    S_PROPERTIES_BEGIN
        property(&SingleStringObj::_337100890, "337100890")
    S_PROPERTIES_END
};

struct TypesStruct : public Cerial::NlohmannJsonObj<TypesStruct>
{
    TypesStruct() = default;
    TypesStruct(bool generatedata) {
        (void)generatedata;
        static int tsi = 1;
        static bool tsb = false;

        b = tsb;
        s = static_cast<short>(tsi);
        us = static_cast<unsigned short>(tsi);
        i = tsi;
        ui = tsi;
        l = tsi;
        ul = tsi;
        ll = tsi;
        //ull = tsi;
        f = 0.5;
        d = 0.0005;
        str = "ssOjb" + std::to_string(tsi);
        ws = L"wws" + std::to_wstring(tsi);
        
        for (auto x = 0; x < 10; ++x) {
            vI.push_back(tsi + x);
        }

        tsi++;
        tsb = !tsb;
    }

    bool b;
    short s;
    unsigned short us;
    int i;
    unsigned int ui;
    long l;
    unsigned long ul;
    long long ll;
    float f;
    double d;
    std::string str;
    std::wstring ws;
    std::vector<int> vI;

    S_PROPERTIES_BEGIN
        S_PROPERTY(TypesStruct, b),
        S_PROPERTY(TypesStruct, s),
        S_PROPERTY(TypesStruct, us),
        S_PROPERTY(TypesStruct, i),
        S_PROPERTY(TypesStruct, ui),
        S_PROPERTY(TypesStruct, l),
        S_PROPERTY(TypesStruct, ul),
        S_PROPERTY(TypesStruct, ll),
        S_PROPERTY(TypesStruct, d),
        S_PROPERTY(TypesStruct, f),
        S_PROPERTY(TypesStruct, str),
        S_PROPERTY(TypesStruct, ws),
        S_PROPERTY(TypesStruct, vI)
    S_PROPERTIES_END
};

struct Event : public Cerial::NlohmannJsonObj<Event>
{
    Event() = default;
    Event(bool generatedata){
        (void)generatedata;
        static int ei = 1;
        description = "descript" + std::to_string(ei);
        id = ei;
        logo = "logo" + std::to_string(ei);
        name = "name" + std::to_string(ei);

        for (int i = 0; i < 5; ++i) {
            subTopicIds.push_back(ei + i);
            topicIds.push_back(1 + ei + i);
        }
        subjectCode = "subCode" + std::to_string(ei);
        subtitle = "subtitle" + std::to_string(ei);

        ei++;
    }
    std::string description;
    unsigned int id;
    std::string logo;
    std::string name;
    std::vector<unsigned int> subTopicIds;
    std::string subjectCode;
    std::string subtitle;
    std::vector<unsigned int> topicIds;

    S_PROPERTIES_BEGIN
        S_PROPERTY(Event, description),
        S_PROPERTY(Event, id),
        S_PROPERTY(Event, logo),
        S_PROPERTY(Event, name),
        S_PROPERTY(Event, subTopicIds),
        S_PROPERTY(Event, subjectCode),
        S_PROPERTY(Event, subtitle),
        S_PROPERTY(Event, topicIds)
    S_PROPERTIES_END
};

struct EventsCollection : public Cerial::NlohmannJsonObj<EventsCollection>
{
    Event _138586341;
    Event _138586345; 
    Event _138586349;
    Event _138586353;
    Event _138586357;

    S_PROPERTIES_BEGIN
        property(&EventsCollection::_138586341, "138586341"),
        property(&EventsCollection::_138586345, "138586345"),
        property(&EventsCollection::_138586349, "138586349"),
        property(&EventsCollection::_138586353, "138586353"),
        property(&EventsCollection::_138586357, "138586357")
    S_PROPERTIES_END
};

struct ObjectCollection : public Cerial::NlohmannJsonObj<ObjectCollection>
{
    ObjectCollection() = default;
    StringsStruct stringsStruct;
    SingleStringObj singleStringObj;
    TypesStruct typesStruct;
    EventsCollection eventsCollection;
    
    StringsStruct stringsStruct1;
    SingleStringObj singleStringObj1;
    TypesStruct typesStruct1;
    EventsCollection eventsCollection1;

    StringsStruct stringsStruct2;
    SingleStringObj singleStringObj2;
    TypesStruct typesStruct2;
    EventsCollection eventsCollection2;

    StringsStruct stringsStruct3;
    SingleStringObj singleStringObj3;
    TypesStruct typesStruct3;
    EventsCollection eventsCollection3;

    StringsStruct stringsStruct4;
    SingleStringObj singleStringObj4;
    TypesStruct typesStruct4;
    EventsCollection eventsCollection4;

    StringsStruct stringsStruct5;
    SingleStringObj singleStringObj5;
    TypesStruct typesStruct5;
    EventsCollection eventsCollection5;

    StringsStruct stringsStruct6;
    SingleStringObj singleStringObj6;
    TypesStruct typesStruct6;
    EventsCollection eventsCollection6;

    StringsStruct stringsStruct7;
    SingleStringObj singleStringObj7;
    TypesStruct typesStruct7;
    EventsCollection eventsCollection7;

    StringsStruct stringsStruct8;
    SingleStringObj singleStringObj8;
    TypesStruct typesStruct8;
    EventsCollection eventsCollection8;

    StringsStruct stringsStruct9;
    SingleStringObj singleStringObj9;
    TypesStruct typesStruct9;
    EventsCollection eventsCollection9;

    StringsStruct stringsStruct10;
    SingleStringObj singleStringObj10;
    TypesStruct typesStruct10;
    EventsCollection eventsCollection10;

    S_PROPERTIES_BEGIN
        S_PROPERTY(ObjectCollection, stringsStruct),
        S_PROPERTY(ObjectCollection, eventsCollection),
        S_PROPERTY(ObjectCollection, singleStringObj),
        S_PROPERTY(ObjectCollection, typesStruct),

        S_PROPERTY(ObjectCollection, stringsStruct1),
        S_PROPERTY(ObjectCollection, eventsCollection1),
        S_PROPERTY(ObjectCollection, singleStringObj1),
        S_PROPERTY(ObjectCollection, typesStruct1),

        S_PROPERTY(ObjectCollection, stringsStruct2),
        S_PROPERTY(ObjectCollection, eventsCollection2),
        S_PROPERTY(ObjectCollection, singleStringObj2),
        S_PROPERTY(ObjectCollection, typesStruct2),

        S_PROPERTY(ObjectCollection, stringsStruct3),
        S_PROPERTY(ObjectCollection, eventsCollection3),
        S_PROPERTY(ObjectCollection, singleStringObj3),
        S_PROPERTY(ObjectCollection, typesStruct3),

        S_PROPERTY(ObjectCollection, stringsStruct4),
        S_PROPERTY(ObjectCollection, eventsCollection4),
        S_PROPERTY(ObjectCollection, singleStringObj4),
        S_PROPERTY(ObjectCollection, typesStruct4),

        S_PROPERTY(ObjectCollection, stringsStruct5),
        S_PROPERTY(ObjectCollection, eventsCollection5),
        S_PROPERTY(ObjectCollection, singleStringObj5),
        S_PROPERTY(ObjectCollection, typesStruct5),

        S_PROPERTY(ObjectCollection, stringsStruct6),
        S_PROPERTY(ObjectCollection, eventsCollection6),
        S_PROPERTY(ObjectCollection, singleStringObj6),
        S_PROPERTY(ObjectCollection, typesStruct6),

        S_PROPERTY(ObjectCollection, stringsStruct7),
        S_PROPERTY(ObjectCollection, eventsCollection7),
        S_PROPERTY(ObjectCollection, singleStringObj7),
        S_PROPERTY(ObjectCollection, typesStruct7),

        S_PROPERTY(ObjectCollection, stringsStruct8),
        S_PROPERTY(ObjectCollection, eventsCollection8),
        S_PROPERTY(ObjectCollection, singleStringObj8),
        S_PROPERTY(ObjectCollection, typesStruct8),

        S_PROPERTY(ObjectCollection, stringsStruct9),
        S_PROPERTY(ObjectCollection, eventsCollection9),
        S_PROPERTY(ObjectCollection, singleStringObj9),
        S_PROPERTY(ObjectCollection, typesStruct9),

        S_PROPERTY(ObjectCollection, stringsStruct10),
        S_PROPERTY(ObjectCollection, eventsCollection10),
        S_PROPERTY(ObjectCollection, singleStringObj10),
        S_PROPERTY(ObjectCollection, typesStruct10)
    S_PROPERTIES_END
};

struct BigObj : public Cerial::NlohmannJsonObj<BigObj> {
    BigObj() = default;
    ObjectCollection _1;
    ObjectCollection _2;
    ObjectCollection _3;
    ObjectCollection _4;
    ObjectCollection _5;
    ObjectCollection _6;
    ObjectCollection _7;
    ObjectCollection _8;
    ObjectCollection _9;
    ObjectCollection _10;
    ObjectCollection _11;
    ObjectCollection _12;
    ObjectCollection _13;
    ObjectCollection _14;
    ObjectCollection _15;
    ObjectCollection _16;
    ObjectCollection _17;
    ObjectCollection _18;
    ObjectCollection _19;
    ObjectCollection _20;
    ObjectCollection _21;
    ObjectCollection _22;
    ObjectCollection _23;
    ObjectCollection _24;
    ObjectCollection _25;
    ObjectCollection _26;
    ObjectCollection _27;
    ObjectCollection _28;
    ObjectCollection _29;
    ObjectCollection _30;

    S_PROPERTIES_BEGIN
        S_PROPERTY(BigObj, _1),
        S_PROPERTY(BigObj, _2),
        S_PROPERTY(BigObj, _3),
        S_PROPERTY(BigObj, _4),
        S_PROPERTY(BigObj, _5),
        S_PROPERTY(BigObj, _6),
        S_PROPERTY(BigObj, _7),
        S_PROPERTY(BigObj, _8),
        S_PROPERTY(BigObj, _9),
        S_PROPERTY(BigObj, _10),
        S_PROPERTY(BigObj, _11),
        S_PROPERTY(BigObj, _12),
        S_PROPERTY(BigObj, _13),
        S_PROPERTY(BigObj, _14),
        S_PROPERTY(BigObj, _15),
        S_PROPERTY(BigObj, _16),
        S_PROPERTY(BigObj, _17),
        S_PROPERTY(BigObj, _18),
        S_PROPERTY(BigObj, _19),
        S_PROPERTY(BigObj, _20),
        S_PROPERTY(BigObj, _21),
        S_PROPERTY(BigObj, _22),
        S_PROPERTY(BigObj, _23),
        S_PROPERTY(BigObj, _24),
        S_PROPERTY(BigObj, _25),
        S_PROPERTY(BigObj, _26),
        S_PROPERTY(BigObj, _27),
        S_PROPERTY(BigObj, _28),
        S_PROPERTY(BigObj, _29),
        S_PROPERTY(BigObj, _30)
    S_PROPERTIES_END
};

struct BiggerObj : public Cerial::NlohmannJsonObj<BiggerObj> {

    BiggerObj() {
        _1 = std::make_shared<BigObj>();
        _2 = std::make_shared<BigObj>();
        _3 = std::make_shared<BigObj>();
        _4 = std::make_shared<BigObj>();
        _5 = std::make_shared<BigObj>();
        _6 = std::make_shared<BigObj>();
        _7 = std::make_shared<BigObj>();
        _8 = std::make_shared<BigObj>();
        _9 = std::make_shared<BigObj>();
        _10 = std::make_shared<BigObj>();
    }

    std::shared_ptr<BigObj> _1;
    std::shared_ptr<BigObj> _2;
    std::shared_ptr<BigObj> _3;
    std::shared_ptr<BigObj> _4;
    std::shared_ptr<BigObj> _5;
    std::shared_ptr<BigObj> _6;
    std::shared_ptr<BigObj> _7;
    std::shared_ptr<BigObj> _8;
    std::shared_ptr<BigObj> _9;
    std::shared_ptr<BigObj> _10;

    S_PROPERTIES_BEGIN
        S_PROPERTY(BiggerObj, _1),
        S_PROPERTY(BiggerObj, _2),
        S_PROPERTY(BiggerObj, _3),
        S_PROPERTY(BiggerObj, _4),
        S_PROPERTY(BiggerObj, _5),
        S_PROPERTY(BiggerObj, _6),
        S_PROPERTY(BiggerObj, _7),
        S_PROPERTY(BiggerObj, _8),
        S_PROPERTY(BiggerObj, _9),
        S_PROPERTY(BiggerObj, _10)
    S_PROPERTIES_END
};