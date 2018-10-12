
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestResult.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestCase.h>

#include "executor/core/storage/IMemoryObjectTest.h"
#include "executor/core/storage/IRawMemoryObjectTest.h"

#include "executor/core/modules/IMapperModuleTest.h"
#include "../ignis/executor/core/modules/IMapperModule.h"


CPPUNIT_TEST_SUITE_REGISTRATION(ignis::executor::core::storage::IMemoryObjectTest);
CPPUNIT_TEST_SUITE_REGISTRATION(ignis::executor::core::storage::IRawMemoryObjectTest);

CPPUNIT_TEST_SUITE_REGISTRATION(ignis::executor::core::modules::IMapperModuleTest);

int main(int argc, char *argv[]) {
    CPPUNIT_NS::TestResult results;
    CPPUNIT_NS::TestResultCollector result_collector;
    CPPUNIT_NS::BriefTestProgressListener progress;
    CPPUNIT_NS::TestRunner runner;
    CPPUNIT_NS::CompilerOutputter compileroutputter(&result_collector, std::cerr);

    results.addListener(&result_collector);
    results.addListener(&progress);
    runner.addTest(CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
    runner.run(results);
    compileroutputter.write();

    return result_collector.wasSuccessful() ? 0 : 1;
}













