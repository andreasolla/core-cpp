
#ifndef IGNIS_IGENERALSELECTOR_H
#define IGNIS_IGENERALSELECTOR_H

#include "ignis/executor/core/exception/ICompatibilityException.h"
#include "ignis/executor/core/RTTInfo.h"
#include "ignis/executor/core/modules/impl/IPipeImpl.h"
#include "ignis/executor/core/modules/impl/ISortImpl.h"
#include "ignis/executor/core/modules/impl/IIOImpl.h"
#include "ignis/executor/api/IReadIterator.h"

namespace ignis {
    namespace executor {
        namespace core {
            namespace selector {

                class IGeneralSelector {
                public:
                    virtual void loadClass(api::IContext& context) = 0;

                    virtual void map(modules::impl::IPipeImpl &impl) = 0;

                    virtual void filter(modules::impl::IPipeImpl &impl) = 0;

                    virtual void flatmap(modules::impl::IPipeImpl &impl) = 0;

                    virtual void keyBy(modules::impl::IPipeImpl &impl) = 0;

                    virtual void mapPartitions(modules::impl::IPipeImpl &impl, bool preservesPartitioning) = 0;

                    virtual void mapPartitionsWithIndex(modules::impl::IPipeImpl &impl, bool preservesPartitioning) = 0;

                    virtual void mapExecutor(modules::impl::IPipeImpl &impl) = 0;

                    virtual void sortBy(modules::impl::ISortImpl &impl, bool ascending) = 0;

                    virtual void sortBy(modules::impl::ISortImpl &impl, bool ascending, int64_t numPartitions) = 0;
                };


                template<typename Tp>
                class IGeneralSelectorImpl : public IGeneralSelector {
                public:
                    virtual void loadClass(api::IContext& context) { loadClass_check<Tp>(context, nullptr); };

                    virtual void map(modules::impl::IPipeImpl &impl) { map_check<Tp>(impl, nullptr); }

                    virtual void filter(modules::impl::IPipeImpl &impl) { filter_check<Tp>(impl, nullptr); }

                    virtual void flatmap(modules::impl::IPipeImpl &impl) { flatmap_check<Tp>(impl, nullptr, nullptr); }

                    virtual void keyBy(modules::impl::IPipeImpl &impl) { keyBy_check<Tp>(impl, nullptr); }

                    virtual void mapPartitions(modules::impl::IPipeImpl &impl, bool preservesPartitioning) {
                        mapPartitions_check<Tp>(impl, nullptr, nullptr, nullptr, preservesPartitioning);
                    }

                    virtual void mapPartitionsWithIndex(modules::impl::IPipeImpl &impl, bool preservesPartitioning) {
                        mapPartitionsWithIndex_check<Tp>(impl, nullptr, nullptr, nullptr, preservesPartitioning);
                    }

                    virtual void mapExecutor(modules::impl::IPipeImpl &impl) {
                        mapExecutor_check<Tp>(impl, nullptr);
                    }

                    virtual void sortBy(modules::impl::ISortImpl &impl, bool ascending) {
                        sortBy_check<Tp>(impl, nullptr, ascending);
                    }

                    virtual void sortBy(modules::impl::ISortImpl &impl, bool ascending, int64_t numPartitions) {
                        sortBy_check<Tp>(impl, nullptr, ascending, numPartitions);
                    }

                private:

                    template<typename Function>
                    void loadClass_check(api::IContext& context, decltype(&Function::before)*val) {
                        Function().before(context);
                    }

                    template<typename Function>
                    void loadClass_check(...) {
                        throw exception::ICompatibilyException("loadClass", RTTInfo::from<Function>());
                    }

                    template<typename Function>
                    void map_check(modules::impl::IPipeImpl &impl, typename Function::_IFunction_type *val) {
                        impl.map<Function>();
                    }

                    template<typename Function>
                    void map_check(...) { throw exception::ICompatibilyException("map", RTTInfo::from<Function>()); }

                    template<typename Function>
                    void filter_check(modules::impl::IPipeImpl &impl, typename Function::_IFunction_type *val) {
                        filter_check<Function>(impl, val, (typename Function::_R_type *) nullptr);
                    }

                    template<typename Function>
                    void
                    filter_check(modules::impl::IPipeImpl &impl, typename Function::_IFunction_type *val, bool *val2) {
                        impl.filter<Function>();
                    }

                    template<typename Function>
                    void filter_check(...) {
                        throw exception::ICompatibilyException("filter", RTTInfo::from<Function>());
                    }

                    template<typename Function>
                    void flatmap_check(modules::impl::IPipeImpl &impl,
                                       typename Function::_IFunction_type::_R_type::iterator *val,
                                       typename Function::_IFunction_type::_R_type::value_type *val2) {
                        impl.flatmap<Function>();
                    }

                    template<typename Function>
                    void flatmap_check(...) {
                        throw exception::ICompatibilyException("flatmap", RTTInfo::from<Function>());
                    }

                    template<typename Function>
                    void keyBy_check(modules::impl::IPipeImpl &impl, typename Function::_IFunction_type *val) {
                        impl.keyBy<Function>();
                    }

                    template<typename Function>
                    void keyBy_check(...) {
                        throw exception::ICompatibilyException("keyBy", RTTInfo::from<Function>());
                    }

                    template<typename Function>
                    void mapPartitions_check(modules::impl::IPipeImpl &impl,
                                             typename Function::_IFunction_type::_R_type::iterator *val,
                                             typename Function::_IFunction_type::_R_type::value_type *val2,
                                             typename Function::_IFunction_type::_T_type::_IReadIterator_type *val3,
                                             bool preservesPartitioning) {
                        impl.mapPartitions<Function>(preservesPartitioning);
                    }

                    template<typename Function>
                    void mapPartitions_check(...) {
                        throw exception::ICompatibilyException("mapPartitions", RTTInfo::from<Function>());
                    }

                    template<typename Function>
                    void mapPartitionsWithIndex_check(modules::impl::IPipeImpl &impl,
                                                      typename Function::_IFunction2_type::_R_type::iterator *val,
                                                      typename Function::_IFunction2_type::_R_type::value_type *val2,
                                                      typename Function::_IFunction2_type::_T2_type::_IReadIterator_type *val3,
                                                      bool preservesPartitioning) {
                        mapPartitionsWithIndex_check<Function>(impl, (typename Function::_T1_type *) nullptr,
                                                               preservesPartitioning);
                    }

                    template<typename Function>
                    void mapPartitionsWithIndex_check(modules::impl::IPipeImpl &impl,
                                                      int64_t *val,
                                                      bool preservesPartitioning) {
                        impl.mapPartitionsWithIndex<Function>(preservesPartitioning);
                    }

                    template<typename Function>
                    void mapPartitionsWithIndex_check(...) {
                        throw exception::ICompatibilyException("mapPartitionsWithIndex", RTTInfo::from<Function>());
                    }

                    template<typename Function>
                    void
                    mapExecutor_check(modules::impl::IPipeImpl &impl, typename Function::_IVoidFunction_type *val) {
                        mapExecutor_check(impl, val, (typename Function::_IVoidFunction_type::_T_type *) nullptr);
                    }

                    template<typename Function, typename Tpv>
                    void mapExecutor_check(modules::impl::IPipeImpl &impl, typename Function::_IVoidFunction_type *val,
                                           api::IVector<api::IVector < Tpv> *

                    > *val2) {
                        impl.mapExecutor<Function>();
                    }


                    template<typename Function>
                    void mapExecutor_check(...) {
                        throw exception::ICompatibilyException("mapExecutor", RTTInfo::from<Function>());
                    }

                    template<typename Function>
                    void sortBy_check(modules::impl::ISortImpl &impl, typename Function::_IFunction2_type *val,
                                      bool ascending) {
                        sortBy_check<Function>(impl, (typename Function::_T2_type *) nullptr,
                                               (typename Function::_R_type *) nullptr, ascending);
                    }

                    template<typename Function>
                    void
                    sortBy_check(modules::impl::ISortImpl &impl, typename Function::_T1_type **val, bool *val2,
                                 bool ascending) {
                        impl.sortBy<Function>(ascending);
                    }

                    template<typename Function>
                    void sortBy_check(modules::impl::ISortImpl &impl, typename Function::_IFunction2_type *val,
                                      bool ascending, int64_t numPartitions) {
                        sortBy_check<Function>(impl, (typename Function::_T2_type *) nullptr,
                                               (typename Function::_R_type *) nullptr, ascending,
                                               numPartitions);
                    }

                    template<typename Function>
                    void
                    sortBy_check(modules::impl::ISortImpl &impl, typename Function::_T1_type *val, bool *val2,
                                 bool ascending, int64_t numPartitions) {
                        impl.sortBy<Function>(ascending, numPartitions);
                    }

                    template<typename Function>
                    void sortBy_check(...) {
                        throw exception::ICompatibilyException("sortBy", RTTInfo::from<Function>());
                    }

                };
            }
        }
    }
}

#endif

