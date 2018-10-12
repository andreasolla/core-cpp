
#ifndef IGNIS_IFUNCTION_H
#define IGNIS_IFUNCTION_H

#include "IFunctionBase.h"

namespace ignis {
    namespace executor {
        namespace api {
            namespace function {

                template<typename T, typename R>
                class IFunction : public IFunctionBase<T,R>{
                public:

                    virtual void before(IContext &context) {}

                    virtual R call(T &t, IContext &context) {}

                    virtual void after(IContext &context) {}

                };
            }
        }
    }
}

#endif
