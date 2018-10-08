
#include "IObjectTest.h"
#include <vector>
#include <thrift/transport/TBufferTransports.h>
#include "../../../../ignis/data/IZlibTransport.h"
#include "../../../../ignis/data/IObjectProtocol.h"

using namespace ignis::executor::core::storage;
using namespace apache::thrift::transport;

void IObjectTest::setUp() {
    auto manager = std::make_shared<api::IManager<std::string>>();
    auto manager_any = (std::shared_ptr<api::IManager<IObject::Any>> &) manager;
    object = getObject(manager_any, 100, 10 * 1024);
}

void IObjectTest::itWriteItReadTest() {
    std::srand(0);
    std::vector<std::string> examples;
    for (int i = 0; i < 100; i++) {
        examples.push_back(std::to_string(std::rand() % 100));
    }
//////////////////////////////////////////////////////////////////////////
    auto write = object->writeIterator();
    for (auto &elem: examples) {
        write->write((IObject::Any &) elem);
    }
    CPPUNIT_ASSERT_EQUAL(examples.size(), object->getSize());
//////////////////////////////////////////////////////////////////////////
    auto read = object->readIterator();
    for (auto &elem: examples) {
        CPPUNIT_ASSERT(read->hashNext());
        CPPUNIT_ASSERT_EQUAL(elem, (std::string &) read->next());
    }
    CPPUNIT_ASSERT(!read->hashNext());
}

void IObjectTest::itWriteTransReadTest() {
    std::srand(0);
    std::vector<std::string> examples;
    for (int i = 0; i < 100; i++) {
        examples.push_back(std::to_string(std::rand() % 100));
    }
//////////////////////////////////////////////////////////////////////////
    auto write = object->writeIterator();
    for (auto &elem: examples) {
        write->write((IObject::Any &) elem);
    }
    CPPUNIT_ASSERT_EQUAL(examples.size(), object->getSize());
//////////////////////////////////////////////////////////////////////////
    auto w_buffer = std::make_shared<transport::TMemoryBuffer>();
    object->write(w_buffer, 6);

    auto w_transport = std::make_shared<data::IZlibTransport>(w_buffer);
    auto w_protocol = std::make_shared<data::IObjectProtocol>(w_transport);
    data::handle::IReader<std::vector<std::string>> reader;
    std::shared_ptr<std::vector<std::string>> result = w_protocol->readObject(reader);

    CPPUNIT_ASSERT_EQUAL(examples.size(), result->size());
    for (int i = 0; i < examples.size(); i++) {
        CPPUNIT_ASSERT_EQUAL(examples[i], (*result)[i]);
    }
}

void IObjectTest::transWriteItReadTest() {
    std::srand(0);
    std::vector<std::string> examples;
    for (int i = 0; i < 100; i++) {
        examples.push_back(std::to_string(std::rand() % 100));
    }
//////////////////////////////////////////////////////////////////////////
    auto r_buffer = std::make_shared<transport::TMemoryBuffer>();
    auto r_transport = std::make_shared<data::IZlibTransport>(r_buffer, 6);
    auto r_protocol = std::make_shared<data::IObjectProtocol>(r_transport);
    data::handle::IWriter<std::vector<std::string>> writer;
    r_protocol->writeObject(examples, writer);
    r_transport->flush();
    object->read(r_buffer);
//////////////////////////////////////////////////////////////////////////
    auto read = object->readIterator();
    for (auto &elem: examples) {
        CPPUNIT_ASSERT(read->hashNext());
        CPPUNIT_ASSERT_EQUAL(elem, (std::string &) read->next());
    }
    CPPUNIT_ASSERT(!read->hashNext());
}

void IObjectTest::transWriteTransReadTest() {
    std::srand(0);
    std::vector<std::string> examples;
    for (int i = 0; i < 100; i++) {
        examples.push_back(std::to_string(std::rand() % 100));
    }
//////////////////////////////////////////////////////////////////////////
    auto r_buffer = std::make_shared<transport::TMemoryBuffer>();
    auto r_transport = std::make_shared<data::IZlibTransport>(r_buffer, 6);
    auto r_protocol = std::make_shared<data::IObjectProtocol>(r_transport);
    data::handle::IWriter<std::vector<std::string>> writer;
    r_protocol->writeObject(examples, writer);
    r_transport->flush();
    object->read(r_buffer);
//////////////////////////////////////////////////////////////////////////
    auto w_buffer = std::make_shared<transport::TMemoryBuffer>();
    object->write(w_buffer, 6);

    auto w_transport = std::make_shared<data::IZlibTransport>(w_buffer);
    auto w_protocol = std::make_shared<data::IObjectProtocol>(w_transport);
    data::handle::IReader<std::vector<std::string>> reader;
    std::shared_ptr<std::vector<std::string>> result = w_protocol->readObject(reader);

    CPPUNIT_ASSERT_EQUAL(examples.size(), result->size());
    for (int i = 0; i < examples.size(); i++) {
        CPPUNIT_ASSERT_EQUAL(examples[i], (*result)[i]);
    }
}

void IObjectTest::clearTest() {

}

void IObjectTest::appendTest() {

}

void IObjectTest::copyTest() {

}

void IObjectTest::moveTest() {

}

void IObjectTest::tearDown() {
    object->clear();
    object.reset();
}