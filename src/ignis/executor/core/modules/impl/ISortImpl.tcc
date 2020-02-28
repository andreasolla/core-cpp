
#include "ISortImpl.h"
#include <algorithm>
#include <cmath>

#define ISortImplClass ignis::executor::core::modules::impl::ISortImpl


template<typename Tp>
void ISortImplClass::sort(bool ascending) {
    sort < Tp > (ascending, -1);
}

template<typename Tp>
void ISortImplClass::sort(bool ascending, int64_t partitions) {
    std::less<Tp> comparator;
    sort_impl<Tp>([&comparator, ascending](const Tp &lhs, const Tp &rhs) {
        return comparator(lhs, rhs) == ascending;
    }, partitions);
}

template<typename Function>
void ISortImplClass::sortBy(bool ascending) {
    sortBy < Function > (ascending, -1);
}

template<typename Function>
void ISortImplClass::sortBy(bool ascending, int64_t partitions) {
    auto &context = executor_data->getContext();
    Function function;
    function.before(context);
    sort_impl<typename Function::_T1_type>(
            [&context, &function, ascending](const typename Function::_T1_type &lhs,
                                             const typename Function::_T2_type &rhs) {
                return function.call(
                        const_cast<typename Function::_T1_type &>(lhs),
                        const_cast<typename Function::_T2_type &>(rhs), context
                ) == ascending;
            }, partitions);
    function.after(context);
}

template<typename Tp, typename Cmp>
void ISortImplClass::sort_impl(Cmp comparator, int64_t partitions) {
    IGNIS_TRY()
        auto input = executor_data->getPartitions<Tp>();
        auto executors = executor_data->mpi().executors();
        /*Copy the data if they are reused*/
        if (input->cache()) {
            /*Work directly on the array to improve performance*/
            if (executor_data->getPartitionTools().isMemory(*input)) {
                input = input->clone();
            } else {
                /*Only group will be affected*/
                input = input->shadowCopy();
            }
        }

        /*Sort each partition*/
        IGNIS_LOG(info) << "Sort: sorting " << input->partitions() << " partitions locally";
        parallelLocalSort(*input, comparator);

        int64_t localPartitions = input->partitions();
        int64_t totalPartitions;
        executor_data->mpi().native().Allreduce(&localPartitions, &totalPartitions, 1, MPI::LONG_LONG, MPI::SUM);
        if (totalPartitions < 2) {
            executor_data->setPartitions(input);
            return;
        }

        /*Generates pivots to separate the elements in order*/
        int64_t samples = executor_data->getProperties().sortSamples();
        if (partitions > 0) {
            samples *= partitions / input->partitions() + 1;
        }
        IGNIS_LOG(info) << "Sort: selecting " << samples << " pivots";
        auto pivots = selectPivots(*input, samples);

        IGNIS_LOG(info) << "Sort: collecting pivots";
        executor_data->mpi().gather(*pivots, 0);

        if (executor_data->mpi().isRoot(0)) {
            auto group = executor_data->getPartitionTools().newPartitionGroup<Tp>(0);
            group->add(pivots);
            parallelLocalSort(*group, comparator);
            if (partitions > 0) {
                samples = partitions  - 1;
            } else {
                samples = totalPartitions  - 1;
            }

            IGNIS_LOG(info) << "Sort: selecting " << samples << " partition pivots";
            pivots = selectPivots(*group, samples);
        }

        IGNIS_LOG(info) << "Sort: broadcasting pivots ranges";
        executor_data->mpi().bcast(*pivots, 0);

        decltype(input) ranges = generateRanges(*input, *pivots);
        decltype(input) output = executor_data->getPartitionTools().newPartitionGroup<Tp>();
        auto executor_ranges = (int64_t) std::ceil(ranges->partitions() / (double) executors);
        int target = -1;
        IGNIS_LOG(info) << "Sort: exchanging ranges";
        for (int p = 0; p < ranges->partitions(); p++) {
            if (p % executor_ranges == 0) { target++; }
            executor_data->mpi().gather(*(*ranges)[p], target);
            if (executor_data->mpi().isRoot(target)) {
                output->add((*ranges)[p]);
            } else {
                (*ranges)[p].reset();
            }
        }

        /*Sort final partitions*/
        IGNIS_LOG(info) << "Sort: sorting again " << output->partitions() << " partitions locally";
        parallelLocalSort(*output, comparator);
        executor_data->setPartitions(output);
    IGNIS_CATCH()
}

template<typename Tp, typename Cmp>
void ISortImplClass::parallelLocalSort(storage::IPartitionGroup <Tp> &group, Cmp comparator) {
    bool inMemory = executor_data->getPartitionTools().isMemory(group);
    IGNIS_OMP_EXCEPTION_INIT()
    #pragma omp parallel
    {
        IGNIS_OMP_TRY()
            /*Sort each partition locally*/
            #pragma omp for schedule(dynamic)
            for (int64_t p = 0; p < group.partitions(); p++) {
                if (inMemory) {
                    sortMemoryPartition<Tp>(reinterpret_cast<storage::IMemoryPartition <Tp> &>(*group[p]),
                                            comparator);
                } else {
                    #pragma omp critical
                    {
                        group[p] = sortPartition<Tp>(*group[p], comparator);
                    }
                }
            }
        IGNIS_OMP_CATCH()
    }
    IGNIS_OMP_EXCEPTION_END()
}

template<typename Tp>
std::shared_ptr<ignis::executor::core::storage::IMemoryPartition<Tp>>
ISortImplClass::selectPivots(storage::IPartitionGroup <Tp> &group, int64_t samples) {
    if (executor_data->getPartitionTools().isMemory(group)) {
        return selectMemoryPivots(group, samples);
    }
    auto pivots = executor_data->getPartitionTools().newMemoryPartition<Tp>();
    auto writer = pivots->writeIterator();
    IGNIS_OMP_EXCEPTION_INIT()
    #pragma omp parallel
    {
        IGNIS_OMP_TRY()
            #pragma omp for schedule(dynamic)
            for (int64_t p = 0; p < group.partitions(); p++) {
                auto skip = (group[p]->size() - samples) / (samples + 1);
                auto reader = group[p]->readIterator();
                for (int n = 0; n < samples; n++) {
                    for (int i = 0; i < skip; i++) {
                        reader->next();
                    }
                    if (!reader->hasNext()) {
                        break;
                    }
                    #pragma omp critical
                    {
                        writer->write(reader->next());
                    }
                }
            }
        IGNIS_OMP_CATCH()
    }
    IGNIS_OMP_EXCEPTION_END()
    return pivots;
}

template<typename Tp>
std::shared_ptr<ignis::executor::core::storage::IPartitionGroup<Tp>>
ISortImplClass::generateRanges(storage::IPartitionGroup <Tp> &group, storage::IMemoryPartition <Tp> &pivots) {
    if (executor_data->getPartitionTools().isMemory(group)) {
        return generateMemoryRanges(group, pivots);
    }
    auto ranges = executor_data->getPartitionTools().newPartitionGroup<Tp>(pivots.size() + 1);

    IGNIS_OMP_EXCEPTION_INIT()
    #pragma omp parallel
    {
        auto thread_ranges = executor_data->getPartitionTools().newPartitionGroup<Tp>(ranges->partitions());
        std::vector<std::shared_ptr<api::IWriteIterator < Tp>> > writers;
        for (int64_t p = 0; p < thread_ranges->partitions(); p++) {
            writers.push_back((*thread_ranges)[p]->writeIterator());
        }

        IGNIS_OMP_TRY()
            #pragma omp for schedule(dynamic)
            for (int64_t p = 0; p < group.partitions(); p++) {
                auto reader = group[p]->readIterator();
                while (reader->hasNext()) {
                    auto &elem = reader->next();
                    writers[searchRange(elem, pivots)]->write(elem);
                }
                group[p]->clear();
            }
            #pragma omp critical
            for (int64_t p = 0; p < thread_ranges->partitions(); p++) {
                (*thread_ranges)[p]->moveTo(*((*ranges)[p]));
            }
        IGNIS_OMP_CATCH()
    }
    IGNIS_OMP_EXCEPTION_END()
    group.clear();
    return ranges;
}


template<typename Tp, typename Cmp>
void ISortImplClass::sortMemoryPartition(storage::IMemoryPartition <Tp> &part, Cmp comparator) {
    std::stable_sort(part.begin(), part.end(), comparator);
}

template<typename Tp, typename Cmp>
std::shared_ptr<ignis::executor::core::storage::IPartition<Tp>>
ISortImplClass::sortPartition(storage::IPartition <Tp> &part, Cmp comparator) {
    auto read = part.readIterator();
    std::vector<std::shared_ptr<storage::IPartition < Tp>> > stack;

    int64_t i = 0;
    while (true) {
        while (i < part.size() && (stack.size() < 2 || stack.back()->size() != stack[stack.size() - 2]->size())) {
            stack.push_back(executor_data->getPartitionTools().newPartition<Tp>());
            stack.back()->writeIterator()->write(read->next());
            i++;
        }
        if (stack[0]->size() == part.size()) {
            break;
        }

        auto sorted = mergePartitions(*stack.back(), *stack[stack.size() - 2], comparator);

        stack.pop_back();
        stack.pop_back();
        stack.push_back(sorted);
    }
    return stack.back();
}


template<typename Tp, typename Cmp>
std::shared_ptr<ignis::executor::core::storage::IPartition<Tp>> ISortImplClass::mergePartitions(
        storage::IPartition <Tp> &p1, storage::IPartition <Tp> &p2, Cmp comparator) {
    auto sorted = executor_data->getPartitionTools().newPartition<Tp>();
    auto sw = sorted->writeIterator();
    auto r1 = p1.readIterator();
    auto r2 = p2.readIterator();
    auto head1 = r1->nextShared();
    auto head2 = r2->nextShared();

    while (true) {
        if (comparator(*head1, *head2)) {
            sw->write(*head1);
            if (r1->hasNext()) {
                head1 = r1->nextShared();
            } else {
                sw->write(*head2);
                storage::copy(*r2, *sw);
                break;
            }
        } else {
            sw->write(*head2);
            if (r2->hasNext()) {
                head2 = r2->nextShared();
            } else {
                sw->write(*head1);
                storage::copy(*r1, *sw);
                break;
            }
        }
    }
    return sorted;
}

template<typename Tp>
std::shared_ptr<ignis::executor::core::storage::IMemoryPartition<Tp>>
ISortImplClass::selectMemoryPivots(storage::IPartitionGroup <Tp> &group, int64_t samples) {
    auto pivots = executor_data->getPartitionTools().newMemoryPartition<Tp>();
    auto writer = pivots->writeIterator();
    auto &men_writer = executor_data->getPartitionTools().toMemory(*writer);

    for (int64_t p = 0; p < group.partitions(); p++) {
        auto skip = (group[p]->size() - samples) / (samples + 1);
        auto &part = executor_data->getPartitionTools().toMemory(*group[p]);
        auto pos = skip;
        for (int n = 0; n < samples; n++) {
            men_writer.write(part[pos++]);
            pos += skip;
        }
    }
    return pivots;
}


template<typename Tp>
std::shared_ptr<ignis::executor::core::storage::IPartitionGroup<Tp>>
ISortImplClass::generateMemoryRanges(storage::IPartitionGroup <Tp> &group, storage::IMemoryPartition <Tp> &pivots) {
    auto ranges = executor_data->getPartitionTools().newPartitionGroup<Tp>(pivots.size() + 1);

    IGNIS_OMP_EXCEPTION_INIT()
    #pragma omp parallel
    {
        auto thread_ranges = executor_data->getPartitionTools().newPartitionGroup<Tp>(ranges->partitions());
        std::vector<std::shared_ptr<storage::IMemoryWriteIterator < Tp>> > writers(ranges->partitions());
        for (int64_t p = 0; p < thread_ranges->partitions(); p++) {
            auto it = (*thread_ranges)[p]->writeIterator();
            writers[p] = executor_data->getPartitionTools().toMemory(it);
        }

        IGNIS_OMP_TRY()
            #pragma omp for schedule(dynamic)
            for (int64_t p = 0; p < group.partitions(); p++) {
                auto &part = executor_data->getPartitionTools().toMemory(*group[p]);
                for (int64_t i = 0; i < group[p]->size(); i++) {
                    writers[searchRange(part[i], pivots)]->write(std::move(part[i]));
                }
                group[p]->clear();
            }
            #pragma omp critical
            for (int64_t p = 0; p < thread_ranges->partitions(); p++) {
                (*thread_ranges)[p]->moveTo(*((*ranges)[p]));
            }
        IGNIS_OMP_CATCH()
    }
    IGNIS_OMP_EXCEPTION_END()
    group.clear();
    return ranges;
}

template<typename Tp>
int64_t ISortImplClass::searchRange(Tp &elem, storage::IMemoryPartition <Tp> &pivots) {
    int64_t start = 0;
    int64_t end = pivots.size() - 1;
    int64_t mid;
    while (start < end) {
        mid = (start + end) / 2;
        if (elem < pivots[mid]) {
            end = mid - 1;
        } else {
            start = mid + 1;
        }
    }
    if (elem < pivots[start]) {
        return start;
    } else {
        return start + 1;
    }
}


#undef ISortImplClass