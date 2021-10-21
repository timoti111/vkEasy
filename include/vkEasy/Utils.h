#pragma once

#include <vkEasy/global.h>

bool featuresAtLeastOneTrue(const vk::PhysicalDeviceFeatures& features);
vk::PhysicalDeviceFeatures featuresOr(const vk::PhysicalDeviceFeatures& lhs, const vk::PhysicalDeviceFeatures& rhs);
vk::PhysicalDeviceFeatures featuresAnd(const vk::PhysicalDeviceFeatures& lhs, const vk::PhysicalDeviceFeatures& rhs);
bool printUnsupported(const vk::PhysicalDeviceFeatures& device, const vk::PhysicalDeviceFeatures& needed);