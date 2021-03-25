#pragma once
#include <cstddef>
#include <type_traits>

using namespace std;

#if defined(VIVADO_BACKEND)
#include "backend/vivado_impl.ipp"
#endif

#if defined(INTEL_BACKEND)
#include "backend/intel_impl.ipp"
#endif

#if defined(CATAPULT_BACKEND)
#include "backend/catapult_impl.ipp"
#endif

#if defined(FASTSIM_BACKEND)
#include "backend/gmp_impl.ipp"
#endif

#if defined(EXTINT_BACKEND)
#include "backend/extint_impl.ipp"
#endif