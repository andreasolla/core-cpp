#include <cstdlib>
#include <iostream>
#include "data/serialization/IReader.h"
#include "data/serialization/IWriter.h"
#include "executor/api/IgnisExecutorException.h"
#include "driver/api/IgnisException.h"
#include "executor/core/IDinamicObject.h"

using namespace std;
using namespace ignis::executor::api;
using namespace ignis::driver::api;

class Test{
public:
    Test(){cout << "c" << endl;}
    ~Test(){cout << "d" << endl;}
};

int main(int argc, char *argv[]) {

    ignis::data::serialization::IReader r;
    ignis::data::serialization::IWriter w;

    return EXIT_SUCCESS;
}

