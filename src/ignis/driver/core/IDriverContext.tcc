
#include "IDriverContext.h"

#include "ignis/executor/core/selector/ITypeSelector.h"
#include "ignis/driver/api/IDriverException.h"
#include "ignis/rpc/driver/IDriverException_types.h"

#define IDriverContextClass ignis::driver::core::IDriverContext

template<typename Tp>
ignis::rpc::ISource IDriverContextClass::registerType() {
    IGNIS_RPC_TRY()
        auto type = std::make_shared<ignis::executor::core::selector::ITypeSelectorImpl<Tp>>();
        executor_data->registerType(type);
        rpc::ISource src;
        src.obj.__set_name(type->info().getStandardName());
        return src;
    IGNIS_RPC_CATCH()
}

template<typename C>
int64_t IDriverContextClass::parallelize(const C &collection, int64_t partitions) {
    try {
        auto group = executor_data->getPartitionTools().newPartitionGroup<typename C::value_type>();
        int64_t elems = collection.size();
        int64_t partition_elems = elems / partitions;
        int64_t remainder = elems % partitions;
        auto it = collection.begin();

        for (int64_t p = 0; p < partitions; p++) {
            auto partition = std::make_shared<ignis::executor::core::storage::IMemoryPartition<typename C::value_type>>(
                    partition_elems + 1);
            auto writer = partition->writeIterator();
            auto men_writer = executor_data->getPartitionTools().toMemory(*writer);

            for (int64_t i = 0; i < partition_elems; i++) {
                men_writer.write(*it);
                it++;
            }

            if(p<remainder){
                men_writer.write(*it);
                it++;
            }

            group->add(partition);
        }
        std::lock_guard<std::mutex> lock(mutex);
        executor_data->setPartitions<typename C::value_type>(group);
        return this->saveContext();
    } catch (executor::core::exception::IException &ex) {
        throw api::IDriverException(ex.what(), ex.toString());
    } catch (std::exception &ex) {
        throw api::IDriverException(ex.what());
    }
}

template<typename Tp>
std::vector<Tp> IDriverContextClass::collect(int64_t id) {
    try {
        std::shared_ptr<ignis::executor::core::storage::IPartitionGroup<Tp>> group;
        {
            std::lock_guard<std::mutex> lock(mutex);
            this->loadContext(id);
            group = executor_data->getPartitions<Tp>();
        }
        int64_t elems = 0;
        for(auto& tp: *group){
            elems+= tp->size();
        }
        std::vector<Tp> result;
        result.reserve(elems);
        for(auto& tp: *group){
            elems+= tp->size();
        }

        for(auto& tp: *group){
            auto reader = tp->readIterator();
            while(reader->hasNext()){
                result.push_back(std::move(reader->next()));
            }
        }
        return std::move(result);
    } catch (executor::core::exception::IException &ex) {
        throw api::IDriverException(ex.what(), ex.toString());
    } catch (std::exception &ex) {
        throw api::IDriverException(ex.what());
    }
}

template<typename Tp>
Tp IDriverContextClass::collect1(int64_t id) {
    return collect<Tp>(id)[0];
}