// Copyright 2023 iLogtail Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "unittest/Unittest.h"
#include <stdio.h>
#include <fstream>
#include "reader/DelimiterLogFileReader.h"
#include "common/RuntimeUtil.h"
#include "common/FileSystemUtil.h"
#include "util.h"

DECLARE_FLAG_INT32(force_release_deleted_file_fd_timeout);

namespace logtail {

class DelimiterParseLogLineUnittest : public ::testing::Test {
public:
    void TestCanBeParsed();
    void TestCanNotBeParsedUnDiscard();
    void TestCanNotBeParsedDiscard();

    static void SetUpTestCase() { BOOL_FLAG(ilogtail_discard_old_data) = false; }
};

UNIT_TEST_CASE(DelimiterParseLogLineUnittest, TestCanBeParsed);
UNIT_TEST_CASE(DelimiterParseLogLineUnittest, TestCanNotBeParsedUnDiscard);
UNIT_TEST_CASE(DelimiterParseLogLineUnittest, TestCanNotBeParsedDiscard);

void DelimiterParseLogLineUnittest::TestCanBeParsed() {
    DelimiterLogFileReader logFileReader("project",
                                         "logstore",
                                         "dir",
                                         "file",
                                         INT32_FLAG(default_tail_limit_kb),
                                         "",
                                         "",
                                         "",
                                         ENCODING_UTF8,
                                         ",",
                                         '\'',
                                         false,
                                         false,
                                         false,
                                         false,
                                         false);
    std::vector<std::string> columnKeys = {"time", "method", "url", "request_time"};
    logFileReader.SetColumnKeys(columnKeys, "time");
    sls_logs::LogGroup logGroup;
    ParseLogError error;
    LogtailTime lastLogLineTime = {0, 0};
    std::string lastLogTimeStr = "";
    uint32_t logGroupSize = 0;
    std::string testLog = "2013-10-31 21:03:49,POST,PutData?Category\n=YunOsAccountOpLog\n,0.024";
    bool successful
        = logFileReader.ParseLogLine(testLog, logGroup, error, lastLogLineTime, lastLogTimeStr, logGroupSize);
    APSARA_TEST_TRUE_FATAL(successful);
    APSARA_TEST_EQUAL_FATAL(logGroupSize, 109);
}

void DelimiterParseLogLineUnittest::TestCanNotBeParsedUnDiscard() {
    DelimiterLogFileReader logFileReader("project",
                                         "logstore",
                                         "dir",
                                         "file",
                                         INT32_FLAG(default_tail_limit_kb),
                                         "",
                                         "",
                                         "",
                                         ENCODING_UTF8,
                                         ",",
                                         '\'',
                                         false,
                                         false,
                                         false,
                                         false,
                                         false);
    std::vector<std::string> columnKeys = {"time", "method", "url", "request_time"};
    logFileReader.SetColumnKeys(columnKeys, "time");
    sls_logs::LogGroup logGroup;
    ParseLogError error;
    LogtailTime lastLogLineTime = {0, 0};
    std::string lastLogTimeStr = "";
    uint32_t logGroupSize = 0;
    logFileReader.mDiscardUnmatch = false;
    std::string testLog = "2013-10-31 21:03:49|POST|PutData?Category\n=YunOsAccountOpLog\n|0.024";
    bool successful
        = logFileReader.ParseLogLine(testLog, logGroup, error, lastLogLineTime, lastLogTimeStr, logGroupSize);
    APSARA_TEST_FALSE_FATAL(successful);
    APSARA_TEST_EQUAL_FATAL(logGroupSize, 83);
}

void DelimiterParseLogLineUnittest::TestCanNotBeParsedDiscard() {
    DelimiterLogFileReader logFileReader("project",
                                         "logstore",
                                         "dir",
                                         "file",
                                         INT32_FLAG(default_tail_limit_kb),
                                         "",
                                         "",
                                         "",
                                         ENCODING_UTF8,
                                         ",",
                                         '\'',
                                         false,
                                         false,
                                         false,
                                         false,
                                         false);
    std::vector<std::string> columnKeys = {"time", "method", "url", "request_time"};
    logFileReader.SetColumnKeys(columnKeys, "time");
    sls_logs::LogGroup logGroup;
    ParseLogError error;
    LogtailTime lastLogLineTime = {0, 0};
    std::string lastLogTimeStr = "";
    uint32_t logGroupSize = 0;
    logFileReader.mDiscardUnmatch = true;
    std::string testLog = "2013-10-31 21:03:49|POST|PutData?Category\n=YunOsAccountOpLog\n|0.024";
    bool successful
        = logFileReader.ParseLogLine(testLog, logGroup, error, lastLogLineTime, lastLogTimeStr, logGroupSize);
    APSARA_TEST_FALSE_FATAL(successful);
    APSARA_TEST_EQUAL_FATAL(logGroupSize, 0);
}


} // namespace logtail

int main(int argc, char** argv) {
    logtail::Logger::Instance().InitGlobalLoggers();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}