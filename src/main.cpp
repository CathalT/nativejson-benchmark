#include <algorithm>
#include <cfloat>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#if defined(_MSC_VER) || defined(__CYGWIN__)
#include <process.h>
#else
#include <spawn.h>
#include <sys/wait.h>
#endif
#include "test.h"
#include "timer.h"
#include "resultfilename.h"
#include "rapidjson/internal/ieee754.h"

static const unsigned cTrialCount = 10;
static const char* gProgramName;

struct TestJson {
    TestJson() : filename(), json(), length(), stat(), statUTF16() {}

    char* fullpath;
    char* filename;
    char* json;
    size_t length;
    Stat stat;           // Reference statistics
    Stat statUTF16;      // Reference statistics
};

typedef std::vector<TestJson> TestJsonList;

static void PrintStat(const Stat& stat) {
    printf("objectCount:  %10u\n", (unsigned)stat.objectCount);
    printf("arrayCount:   %10u\n", (unsigned)stat.arrayCount);
    printf("numberCount:  %10u\n", (unsigned)stat.numberCount);
    printf("stringCount:  %10u\n", (unsigned)stat.stringCount);
    printf("trueCount:    %10u\n", (unsigned)stat.trueCount);
    printf("falseCount:   %10u\n", (unsigned)stat.falseCount);
    printf("nullCount:    %10u\n", (unsigned)stat.nullCount);
    printf("memberCount:  %10u\n", (unsigned)stat.memberCount);
    printf("elementCount: %10u\n", (unsigned)stat.elementCount);
    printf("stringLength: %10u\n", (unsigned)stat.stringLength);
}

#if USE_MEMORYSTAT
static void PrintMemoryStat() {
    const MemoryStat& stat = Memory::Instance().GetStat();
    printf(
        "Memory stats:\n"
        " mallocCount = %u\n"
        "reallocCount = %u\n"
        "   freeCount = %u\n"
        " currentSize = %u\n"
        "    peakSize = %u\n",
        (unsigned)stat.mallocCount,
        (unsigned)stat.reallocCount,
        (unsigned)stat.freeCount,
        (unsigned)stat.currentSize,
        (unsigned)stat.peakSize);
}
#endif

static char* ReadJSON(FILE *fp, size_t* length) {
    fseek(fp, 0, SEEK_END);
    *length = (size_t)ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* json = (char*)malloc(*length + 1);
    fread(json, 1, *length, fp);
    json[*length] = '\0';
    fclose(fp);
    return json;
}

static char* ReadJSON(const char* filename, size_t* length) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        char buffer[FILENAME_MAX];
        sprintf(buffer, "../%s", filename);
        fp = fopen(buffer, "rb");
        if (!fp)
            return 0;
    }
    return ReadJSON(fp, length);
}

static void makeValidFilename(char *filename) {
    while (*filename) {
        switch (*filename) {
            case '/':
                *filename = '_';
                break;
        }
        filename++;
    }
}

static void EscapeString(FILE* fp, const char *s, size_t length) {
    fputc('"', fp);
    for (size_t j = 0; j < length; j++)
        if (s[j] != 0)
            fputc(s[j], fp);
        else
            fputs("\\0", fp);
    fputc('"', fp);
}

static bool ReadFiles(const char* path, TestJsonList& testJsons) {
    char fullpath[FILENAME_MAX];
    sprintf(fullpath, path, "data.txt");
    FILE* fp = fopen(fullpath, "r");
    if (!fp)
        return false;

    // Currently use RapidJSON to generate reference statistics
    TestList& tests = TestManager::Instance().GetTests();
    const TestBase* referenceTest = 0;
    for (TestList::iterator itr = tests.begin(); itr != tests.end(); ++itr) {
        if (strcmp((*itr)->GetName(), "RapidJSON (C++)") == 0) {
            referenceTest = *itr;
            break;
        }
    }

    if (!referenceTest) {
        printf("Error: Cannot find RapidJSON as refernce test. Not reading any files.\n");
        fclose(fp);
        return false;
    }

    while (!feof(fp)) {
        char filename[FILENAME_MAX];
        if (fscanf(fp, "%s", filename) == 1) {
            sprintf(fullpath, path, filename);
            FILE *fp2 = fopen(fullpath, "rb");
            if (!fp2) {
                printf("Error: Cannot read '%s'\n", filename);
                continue;
            }

            TestJson t = TestJson();
            t.fullpath = StrDup(fullpath);
            t.filename = StrDup(filename);
            t.json = ReadJSON(fp2, &t.length);

            // Generate reference statistics
#if TEST_SAXSTATISTICS
            if (!referenceTest->SaxStatistics(t.json, t.length, &t.stat))
                printf("Failed to generate reference statistics\n");
#endif

#if TEST_SAXSTATISTICSUTF16
            if (!referenceTest->SaxStatisticsUTF16(t.json, t.length, &t.statUTF16))
                printf("Failed to generate reference UTF16 statistics\n");
#endif
            printf("Read '%s' (%u bytes)\n", t.filename, (unsigned)t.length);
            PrintStat(t.stat);
            printf("\n");

            testJsons.push_back(t);
        }
    }

    fclose(fp);
    printf("\n");
    return true;
}

static void FreeFiles(TestJsonList& testJsons) {
    for (TestJsonList::iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        free(itr->fullpath);
        free(itr->filename);
        free(itr->json);
        itr->filename = 0;
        itr->json = 0;
    }
}

// Normally use this at the end of MEMORYSTAT_SCOPE()
#if USE_MEMORYSTAT
void CheckMemoryLeak() {
    const MemoryStat& stat = Memory::Instance().GetStat();
    if (stat.currentSize != 0) {
        printf("\nWarning: potential memory leak (%d allocations for %d bytes)\n",
            (int)stat.mallocCount + (int)stat.reallocCount - (int)stat.freeCount,
            (int)stat.currentSize);

        PrintMemoryStat();
        printf("\n");
    }
}
#define MEMORYSTAT_CHECKMEMORYLEAK() CheckMemoryLeak()
#else
#define MEMORYSTAT_CHECKMEMORYLEAK()
#endif

static void Verify(const TestBase& test, const TestJsonList& testJsons) {
    (void)testJsons;

    printf("Verifying %s ... ", test.GetName());
    fflush(stdout);

    bool failed = false;

#if TEST_PARSE && TEST_STATISTICS
    for (TestJsonList::const_iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        MEMORYSTAT_SCOPE();

        test.SetUp();
        ParseResultBase* dom1 = test.Parse(itr->json, itr->length, itr->filename);
        if (!dom1) {
            printf("\nFailed to parse '%s'\n", itr->filename);
            failed = true;
            test.TearDown();
            continue;
        }

        Stat stat1;
        if (!test.Statistics(dom1, &stat1)) {
            printf("Not support Statistics\n");
            delete dom1;
            test.TearDown();
            continue;
        }

        StringResultBase* json1 = test.Stringify(dom1, itr->filename);
        delete dom1;

        if (!json1) {
            // Some libraries may not support stringify, but still check statistics
            if (memcmp(&stat1, &itr->stat, sizeof(Stat)) != 0 &&
                memcmp(&stat1, &itr->statUTF16, sizeof(Stat)) != 0)
            {
                printf("\nStatistics of '%s' is different from reference.\n\n", itr->filename);
                printf("Reference\n---------\n");
                PrintStat(itr->stat);
                printf("\nStat 1\n--------\n");
                PrintStat(stat1);
                printf("\n");
                failed = true;
            }
            test.TearDown();
            continue;
        }

        ParseResultBase* dom2 = test.Parse(json1->c_str(), strlen(json1->c_str()), itr->filename);
        if (!dom2) {
            printf("\nFailed to parse '%s' 2nd time\n", itr->filename);
            failed = true;

            // Write out json1 for diagnosis
            char filename[FILENAME_MAX];
            sprintf(filename, "%s_%s", test.GetName(), itr->filename);
            makeValidFilename(filename);
            FILE* fp = fopen(filename, "wb");
            fwrite(json1->c_str(), strlen(json1->c_str()), 1, fp);
            fclose(fp);

            delete json1;
            test.TearDown();
            continue;
        }

        Stat stat2;
        test.Statistics(dom2, &stat2);

        StringResultBase* json2 = test.Stringify(dom2, itr->filename);
        delete dom2;

        Stat* statProblem = 0;
        int statProblemWhich = 0;
        if (memcmp(&stat1, &itr->stat,      sizeof(Stat)) != 0 &&
            memcmp(&stat1, &itr->statUTF16, sizeof(Stat)) != 0)
        {
            statProblem = &stat1;
            statProblemWhich = 1;
        }
        else if (memcmp(&stat1, &itr->stat,      sizeof(Stat)) != 0 &&
                 memcmp(&stat1, &itr->statUTF16, sizeof(Stat)) != 0)
        {
            statProblem = &stat2;
            statProblemWhich = 2;
        }

        if (statProblem != 0) {
            printf("\nStatistics of '%s' is different from reference.\n\n", itr->filename);
            printf("Reference\n---------\n");
            PrintStat(itr->stat);
            printf("\nStat #%d\n--------\n", statProblemWhich);
            PrintStat(*statProblem);
            printf("\n");

            // Write out json1 for diagnosis
            char filename[FILENAME_MAX];
            sprintf(filename, "%s_%s", test.GetName(), itr->filename);
            makeValidFilename(filename);
            FILE* fp = fopen(filename, "wb");
            fwrite(json1->c_str(), strlen(json1->c_str()), 1, fp);
            fclose(fp);

            failed = true;
        }

        delete json1;
        delete json2;
        test.TearDown();

        MEMORYSTAT_CHECKMEMORYLEAK();
    }
#endif

#if TEST_SAXSTATISTICS
    // Verify SaxStatistics()
    for (TestJsonList::const_iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        MEMORYSTAT_SCOPE();
        Stat stat1;
        if (test.SaxStatistics(itr->json, itr->length, &stat1)) {
            if (memcmp(&stat1, &itr->stat, sizeof(Stat)) != 0 &&
                memcmp(&stat1, &itr->statUTF16, sizeof(Stat)) != 0)
            {
                printf("\nSaxStatistics of '%s' is different from reference.\n\n", itr->filename);
                printf("Reference\n---------\n");
                PrintStat(itr->stat);
                printf("\nStat #%d\n--------\n", 1);
                PrintStat(stat1);
                printf("\n");
            }
        }
    }
#endif

    printf(failed ? "Failed\n" : "OK\n");
}

static void VerifyAll(const TestJsonList& testJsons) {
    TestList& tests = TestManager::Instance().GetTests();
    for (TestList::iterator itr = tests.begin(); itr != tests.end(); ++itr)
        Verify(**itr, testJsons);

    printf("\n");
}

#if USE_MEMORYSTAT
#define BENCH_MEMORYSTAT_INIT()             MemoryStat memoryStat = MemoryStat()
#define BENCH_MEMORYSTAT_ITERATION(trial)   if (trial == 0) memoryStat = Memory::Instance().GetStat()
#ifdef _MSC_VER
#define BENCH_MEMORYSTAT_OUTPUT(fp)         fprintf(fp, ",%Iu,%Iu,%Iu", memoryStat.currentSize, memoryStat.peakSize, memoryStat.mallocCount + memoryStat.reallocCount)
#else
#define BENCH_MEMORYSTAT_OUTPUT(fp)         fprintf(fp, ",%zu,%zu,%zu", memoryStat.currentSize, memoryStat.peakSize, memoryStat.mallocCount + memoryStat.reallocCount)
#endif
#else
#define BENCH_MEMORYSTAT_INIT()
#define BENCH_MEMORYSTAT_ITERATION(trial)
#define BENCH_MEMORYSTAT_OUTPUT(fp)
#endif

#if TEST_PARSE
static void BenchParse(const TestBase& test, const TestJsonList& testJsons, FILE *fp) {
    MEMORYSTAT_SCOPE();
    for (TestJsonList::const_iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        printf("%15s %-20s ... ", "Parse", itr->filename);
        fflush(stdout);

        double minDuration = DBL_MAX;

        BENCH_MEMORYSTAT_INIT();
        bool supported = true;
        for (unsigned trial = 0; trial < cTrialCount; trial++) {
            test.SetUp();
            Timer timer;
            ParseResultBase* dom;
            {
                MEMORYSTAT_SCOPE();

                timer.Start();
                dom = test.Parse(itr->json, itr->length, itr->filename);
                timer.Stop();

                BENCH_MEMORYSTAT_ITERATION(trial);
            }

            delete dom;

            if (!dom) {
                supported = false;
                test.TearDown();
                break;
            }

            double duration = timer.GetElapsedMilliseconds();
            minDuration = std::min(minDuration, duration);
            test.TearDown();
        }

        if (!supported)
            printf("Not support\n");
        else {
            double throughput = itr->length / (1024.0 * 1024.0) / (minDuration * 0.001);
            printf("%6.3f ms  %3.3f MB/s\n", minDuration, throughput);

            fprintf(fp, "1. Parse,%s,%s,%f", test.GetName(), itr->filename, minDuration);
            BENCH_MEMORYSTAT_OUTPUT(fp);
            fprintf(fp, ",0");  // Code size
            fputc('\n', fp);
        }
    }
    MEMORYSTAT_CHECKMEMORYLEAK();
}
#else
static void BenchParse(const TestBase&, const TestJsonList&, FILE *) {
}
#endif

#if TEST_PARSE && TEST_STRINGIFY
static void BenchStringify(const TestBase& test, const TestJsonList& testJsons, FILE *fp) {
    MEMORYSTAT_SCOPE();
    for (TestJsonList::const_iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        printf("%15s %-20s ... ", "Stringify", itr->filename);
        fflush(stdout);

        test.SetUp();
        double minDuration = DBL_MAX;
        ParseResultBase* dom = test.Parse(itr->json, itr->length, itr->filename);

        BENCH_MEMORYSTAT_INIT();
        bool supported = true;
        for (unsigned trial = 0; trial < cTrialCount; trial++) {
            Timer timer;
            StringResultBase* json;
            {
                MEMORYSTAT_SCOPE();

                timer.Start();
                json = test.Stringify(dom, itr->filename);
                timer.Stop();

                BENCH_MEMORYSTAT_ITERATION(trial);
            }

            delete json;

            if (!json) {
                supported = false;
                break;
            }

            double duration = timer.GetElapsedMilliseconds();
            minDuration = std::min(minDuration, duration);
        }

        delete dom;
        test.TearDown();

        if (!supported)
            printf("Not support\n");
        else {
            double throughput = itr->length / (1024.0 * 1024.0) / (minDuration * 0.001);
            printf("%6.3f ms  %3.3f MB/s\n", minDuration, throughput);

            fprintf(fp, "2. Stringify,%s,%s,%f", test.GetName(), itr->filename, minDuration);
            BENCH_MEMORYSTAT_OUTPUT(fp);
            fprintf(fp, ",0");  // Code size
            fputc('\n', fp);
        }
    }
    MEMORYSTAT_CHECKMEMORYLEAK();
}
#else
static void BenchStringify(const TestBase&, const TestJsonList&, FILE *) {
}
#endif

#if TEST_PARSE && TEST_PRETTIFY
static void BenchPrettify(const TestBase& test, const TestJsonList& testJsons, FILE *fp) {
    MEMORYSTAT_SCOPE();
    for (TestJsonList::const_iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        printf("%15s %-20s ... ", "Prettify", itr->filename);
        fflush(stdout);

        test.SetUp();
        double minDuration = DBL_MAX;
        ParseResultBase* dom = test.Parse(itr->json, itr->length, itr->filename);

        BENCH_MEMORYSTAT_INIT();
        bool supported = true;
        for (unsigned trial = 0; trial < cTrialCount; trial++) {
            Timer timer;
            StringResultBase* json;
            {
                MEMORYSTAT_SCOPE();

                timer.Start();
                json = test.Prettify(dom);
                timer.Stop();

                BENCH_MEMORYSTAT_ITERATION(trial);
            }

            delete json;

            if (!json) {
                supported = false;
                break;
            }

            double duration = timer.GetElapsedMilliseconds();
            minDuration = std::min(minDuration, duration);
        }

        delete dom;
        test.TearDown();

        if (!supported)
            printf("Not support\n");
        else {
            double throughput = itr->length / (1024.0 * 1024.0) / (minDuration * 0.001);
            printf("%6.3f ms  %3.3f MB/s\n", minDuration, throughput);

            fprintf(fp, "3. Prettify,%s,%s,%f", test.GetName(), itr->filename, minDuration);
            BENCH_MEMORYSTAT_OUTPUT(fp);
            fprintf(fp, ",0");  // Code size
            fputc('\n', fp);
        }
    }
    MEMORYSTAT_CHECKMEMORYLEAK();
}
#else
static void BenchPrettify(const TestBase&, const TestJsonList&, FILE *) {
}
#endif

#if TEST_PARSE && TEST_STATISTICS
static void BenchStatistics(const TestBase& test, const TestJsonList& testJsons, FILE *fp) {
    MEMORYSTAT_SCOPE();
    for (TestJsonList::const_iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        printf("%15s %-20s ... ", "Statistics", itr->filename);
        fflush(stdout);

        test.SetUp();
        double minDuration = DBL_MAX;
        ParseResultBase* dom = test.Parse(itr->json, itr->length, itr->filename);

        BENCH_MEMORYSTAT_INIT();
        bool supported = true;
        for (unsigned trial = 0; trial < cTrialCount; trial++) {
            Timer timer;
            {
                MEMORYSTAT_SCOPE();

                timer.Start();
                Stat stat;
                supported = test.Statistics(dom, &stat);
                timer.Stop();

                BENCH_MEMORYSTAT_ITERATION(trial);
            }

            if (!supported)
                break;

            double duration = timer.GetElapsedMilliseconds();
            minDuration = std::min(minDuration, duration);
        }

        delete dom;
        test.TearDown();

        if (!supported)
            printf("Not support\n");
        else {
            double throughput = itr->length / (1024.0 * 1024.0) / (minDuration * 0.001);
            printf("%6.3f ms  %3.3f MB/s\n", minDuration, throughput);

            fprintf(fp, "4. Statistics,%s,%s,%f", test.GetName(), itr->filename, minDuration);
            BENCH_MEMORYSTAT_OUTPUT(fp);
            fprintf(fp, ",0");  // Code size
            fputc('\n', fp);
        }
    }
    MEMORYSTAT_CHECKMEMORYLEAK();
}
#else
static void BenchStatistics(const TestBase&, const TestJsonList&, FILE *) {
}
#endif

#if TEST_SAXROUNDTRIP
static void BenchSaxRoundtrip(const TestBase& test, const TestJsonList& testJsons, FILE *fp) {
    MEMORYSTAT_SCOPE();
    for (TestJsonList::const_iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        printf("%15s %-20s ... ", "SaxRoundtrip", itr->filename);
        fflush(stdout);

        double minDuration = DBL_MAX;

        BENCH_MEMORYSTAT_INIT();
        bool supported = true;
        for (unsigned trial = 0; trial < cTrialCount; trial++) {
            test.SetUp();
            Timer timer;
            StringResultBase* json;
            {
                MEMORYSTAT_SCOPE();

                timer.Start();
                json = test.SaxRoundtrip(itr->json, itr->length);
                timer.Stop();

                BENCH_MEMORYSTAT_ITERATION(trial);
            }

            delete json;

            if (!json) {
                supported = false;
                test.TearDown();
                break;
            }

            double duration = timer.GetElapsedMilliseconds();
            minDuration = std::min(minDuration, duration);
            test.TearDown();
        }

        if (!supported)
            printf("Not support\n");
        else {
            double throughput = itr->length / (1024.0 * 1024.0) / (minDuration * 0.001);
            printf("%6.3f ms  %3.3f MB/s\n", minDuration, throughput);

            fprintf(fp, "5. Sax Round-trip,%s,%s,%f", test.GetName(), itr->filename, minDuration);
            BENCH_MEMORYSTAT_OUTPUT(fp);
            fprintf(fp, ",0");  // Code size
            fputc('\n', fp);
        }
    }
    MEMORYSTAT_CHECKMEMORYLEAK();
}
#else
static void BenchSaxRoundtrip(const TestBase&, const TestJsonList&, FILE*) {
}
#endif

#if TEST_SAXSTATISTICS
static void BenchSaxStatistics(const TestBase& test, const TestJsonList& testJsons, FILE *fp) {
    MEMORYSTAT_SCOPE();
    for (TestJsonList::const_iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        printf("%15s %-20s ... ", "Sax Statistics", itr->filename);
        fflush(stdout);

        double minDuration = DBL_MAX;

        BENCH_MEMORYSTAT_INIT();
        bool supported = true;
        for (unsigned trial = 0; trial < cTrialCount; trial++) {
            test.SetUp();
            Timer timer;
            {
                MEMORYSTAT_SCOPE();

                timer.Start();
                Stat stat;
                supported = test.SaxStatistics(itr->json, itr->length, &stat);
                timer.Stop();

                BENCH_MEMORYSTAT_ITERATION(trial);
            }

            if (!supported) {
                test.TearDown();
                break;
            }

            double duration = timer.GetElapsedMilliseconds();
            minDuration = std::min(minDuration, duration);
            test.TearDown();
        }

        if (!supported)
            printf("Not support\n");
        else {
            double throughput = itr->length / (1024.0 * 1024.0) / (minDuration * 0.001);
            printf("%6.3f ms  %3.3f MB/s\n", minDuration, throughput);

            fprintf(fp, "6. SaxStatistics,%s,%s,%f", test.GetName(), itr->filename, minDuration);
            BENCH_MEMORYSTAT_OUTPUT(fp);
            fprintf(fp, ",0");  // Code size
            fputc('\n', fp);
        }
    }
    MEMORYSTAT_CHECKMEMORYLEAK();
}
#else
static void BenchSaxStatistics(const TestBase&, const TestJsonList&, FILE*) {
}
#endif

static void BenchCodeSize(const TestBase& test, const TestJsonList& testJsons, FILE *fp) {
    (void)testJsons;

#if _MSC_VER
    const char cSeparator = '\\';
#else
    const char cSeparator = '/';
#endif

    // Compute path of bin
    char path[FILENAME_MAX];
    strcpy(path, gProgramName);
    char* lastSep = strrchr(path, cSeparator);
    *lastSep = '\0';

    // Compute filename suffix (e.g. "_release_x32_vs2010.exe"
    const char* filename_suffix = strchr(lastSep + 1, '_');

    // Compute test base name (e.g. "rapidjsontest")
    char testFilename[FILENAME_MAX];
    strcpy(testFilename, test.GetFilename());
    *strrchr(testFilename, '.') = '\0';
    const char* testBaseName = strrchr(testFilename, cSeparator) + 1;

    // Assemble a full path
    char fullpath[FILENAME_MAX];
    sprintf(fullpath, "%s%cjsonstat%cjsonstat_%s%s", path, cSeparator, cSeparator, testBaseName, filename_suffix);

    char * const argv[] = { fullpath, testJsons.front().fullpath, NULL };
#ifdef _MSC_VER
    int ret = _spawnv(_P_WAIT, fullpath, argv);
#elif defined(__CYGWIN__)
    int ret = spawnv(_P_WAIT, fullpath, argv);
#else
    pid_t pid;
    int ret = posix_spawn(&pid, fullpath, NULL, NULL, argv, NULL);
    if (ret == 0) {
        int status;
        waitpid(pid, &status, 0);
    }
#endif

    if (ret != 0) {
        printf("Execute '%s' failed (ret=%d)\n", fullpath, ret);
        return;
    }

    // Get file size
    FILE *fp2 = fopen(fullpath, "rb");
    if (fp2) {
        fseek(fp2, 0, SEEK_END);
        unsigned fileSize = (unsigned)ftell(fp2);
        printf("jsonstat file size = %u\n", fileSize);
        fprintf(fp, "7. Code size,%s,jsonstat,0", test.GetName());
#if USE_MEMORYSTAT
        fprintf(fp, ",0,0,0");
#endif
        fprintf(fp, ",%u\n", fileSize);
        fclose(fp2);
    }
    else
        printf("File '%s' not found\n", fullpath);
}

static void BenchPerformance(const TestBase& test, const TestJsonList& testJsons, FILE *fp) {
    printf("Benchmarking Performance of %s\n", test.GetName());

    BenchParse(test, testJsons, fp);
    BenchStringify(test, testJsons, fp);
    BenchPrettify(test, testJsons, fp);
    BenchStatistics(test, testJsons, fp);
    BenchSaxRoundtrip(test, testJsons, fp);
    BenchSaxStatistics(test, testJsons, fp);
    BenchCodeSize(test, testJsons, fp);
    
    printf("\n");
}

static void BenchAllPerformance(const TestJsonList& testJsons) {
    // Try to write to /result path, where template.php exists
    FILE *fp;
    if ((fp = fopen("../../result/performance.php", "r")) != NULL) {
        fclose(fp);
        fp = fopen("../../result/performance_" RESULT_FILENAME, "w");
    }
    else if ((fp = fopen("../result/performance.php", "r")) != NULL) {
        fclose(fp);
        fp = fopen("../result/performance_" RESULT_FILENAME, "w");
    }
    else
        fp = fopen("performance_" RESULT_FILENAME, "w");

    fputs("Type,Library,Filename,Time (ms)", fp);
#if USE_MEMORYSTAT
    fputs(",Memory (byte),MemoryPeak (byte),AllocCount", fp);
#endif
    fputs(",FileSize (byte)\n", fp);

    TestList& tests = TestManager::Instance().GetTests();
    for (TestList::iterator itr = tests.begin(); itr != tests.end(); ++itr)
        BenchPerformance(**itr, testJsons, fp);

    fclose(fp);
}

int main(int argc, char* argv[]) {
#if _MSC_VER
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //void *testWhetherMemoryLeakDetectionWorks = malloc(1);
#endif

    bool doVerify = true;
    bool doPerformance = true;
    bool doConformance = true;

    if (argc == 2) {
        if (strcmp(argv[1], "--verify-only") == 0) {
            doVerify = true;
            doPerformance = doConformance = false;
        }
        else if (strcmp(argv[1], "--performance-only") == 0) {
            doPerformance = true;
            doVerify = doConformance = false;
        }
        else if (strcmp(argv[1], "--conformance-only") == 0) {
            doConformance = true;
            doVerify = doPerformance = false;
        }
        else {
            fprintf(stderr, "Invalid option\n");
            exit(1);
        }
    }

    gProgramName = argv[0];

    MEMORYSTAT_SCOPE();

    {
        // Read files
        TestJsonList testJsons;
        if (!ReadFiles("../data/%s", testJsons))
            ReadFiles("../../data/%s", testJsons);

        // sort tests
        TestList& tests = TestManager::Instance().GetTests();
        std::sort(tests.begin(), tests.end());

        if (doVerify)
            VerifyAll(testJsons);

        if (doPerformance)
            BenchAllPerformance(testJsons);

        printf("\n");

        FreeFiles(testJsons);
    }

    MEMORYSTAT_CHECKMEMORYLEAK();
}
