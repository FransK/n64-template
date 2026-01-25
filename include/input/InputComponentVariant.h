#ifndef INPUT_COMPONENT_VARIANT_H
#define INPUT_COMPONENT_VARIANT_H

#include <variant>
#include "inputComponent.h"
#include "AIInputStrategy.h"
#include "PlayerInputStrategy.h"

using InputComponentVariant = std::variant<
    InputComponent<PlayerInputStrategy>,
    InputComponent<AIInputStrategy>>;

#endif // INPUT_COMPONENT_VARIANT_H