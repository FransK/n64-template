#ifndef CORE_ANIMATION_ADAPTERS_T3D_H
#define CORE_ANIMATION_ADAPTERS_T3D_H

#include <t3d/t3danim.h>
#include <memory>
#include <string>

namespace Adapters
{
    class ModelAdapter
    {
    public:
        explicit ModelAdapter(std::string path)
            : mModel(t3d_model_load(path.c_str())) {}

        ModelAdapter(const ModelAdapter &) = delete;
        ModelAdapter &operator=(const ModelAdapter &) = delete;
        ModelAdapter(ModelAdapter &&) = delete;
        ModelAdapter &operator=(ModelAdapter &&) = delete;

        T3DModel *getModel() const noexcept { return mModel.get(); }
        explicit operator bool() const noexcept { return mModel != nullptr; }

    private:
        struct Deleter
        {
            void operator()(T3DModel *p) const noexcept
            {
                if (p)
                    t3d_model_free(p);
            }
        };

        std::unique_ptr<T3DModel, Deleter> mModel;
    };
}

#endif // CORE_ANIMATION_ADAPTERS_T3D_H