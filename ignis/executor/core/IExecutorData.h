
#ifndef IGNIS_EXECUTORDATA_H
#define IGNIS_EXECUTORDATA_H

#include <unordered_map>
#include "../../IHeaders.h"
#include "../api/IContext.h"
#include "IPropertiesParser.h"
#include "storage/IObject.h"
#include "IPostBox.h"

namespace ignis {
    namespace executor {
        namespace core {
            class IExecutorData {
            public:

                IExecutorData();

                std::shared_ptr<storage::IObject> loadObject(std::shared_ptr<storage::IObject> object);

                std::shared_ptr<storage::IObject> loadObject();

                void deleteLoadObject();

                api::IContext &getContext();

                core::IPropertiesParser &getParser();

                IPostBox &getPostBox();

                int64_t getThreads();

                virtual ~IExecutorData();

                std::unordered_map<std::string, std::shared_ptr<void>> libraries;

            private:
                std::shared_ptr<storage::IObject> loaded_object;
                IPostBox post_box;
                api::IContext context;
                core::IPropertiesParser properties_parser;
            };
        }
    }
}

#endif
