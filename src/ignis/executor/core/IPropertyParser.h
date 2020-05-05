
#ifndef IGNIS_IPROPERTYPARSER_H
#define IGNIS_IPROPERTYPARSER_H

#include <unordered_map>
#include "exception/IInvalidArgument.h"

namespace ignis {
    namespace executor {
        namespace core {
            class IPropertyParser {
            public:
                IPropertyParser(std::unordered_map<std::string, std::string> &properties);

                virtual ~IPropertyParser();

                //All properties

                int cores() { return getNumber("ignis.executor.cores"); }

                int64_t partitionMinimal() { return getSize("ignis.partition.minimal"); }

                int64_t sortSamples() { return getMinNumber("ignis.modules.sort.samples", 1); }

                int64_t ioOverwrite() { return getBoolean("ignis.modules.io.overwrite"); }

                int8_t ioCompression() { return getRangeNumber("ignis.modules.io.compression", 0, 9); }

                int8_t msgCompression() { return getRangeNumber("ignis.transport.compression", 0, 9); }

                int8_t partitionCompression() { return getRangeNumber("ignis.partition.compression", 0, 9); }

                bool nativeSerialization() { return getString("ignis.partition.serialization") == "native"; }

                int64_t transportMinimal() {return getSize("ignis.transport.minimal");}

                int64_t transportElemSize() {return getSize("ignis.transport.element.size");}

                std::string partitionType() { return getString("ignis.partition.type"); }

                std::string jobName() { return getString("ignis.job.name"); }

                std::string JobDirectory() { return getString("ignis.job.directory"); }

                //Auxiliary functions

                std::string &getString(const std::string &key);

                int64_t getNumber(const std::string &key);

                int64_t getMinNumber(const std::string &key, const int64_t &min);

                int64_t getMaxNumber(const std::string &key, const int64_t &max);

                int64_t getRangeNumber(const std::string &key, const int64_t &min, const int64_t &max);

                size_t getSize(const std::string &key);

                bool getBoolean(const std::string &key);

            private:
                void parserError(const std::string &key, const std::string &value, size_t pos);

                std::unordered_map<std::string, std::string> &properties;

            };
        }
    }
}

#endif
