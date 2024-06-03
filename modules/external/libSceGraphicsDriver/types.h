#pragma once
#include "codes.h"

enum class SceWorkloadStatus : uint32_t {
  StatusOk                 = 0, ///< The operation was performed successfully.
  StatusInvalidStream      = 1, ///< The stream parameter was invalid.
  StatusInvalidWorkload    = 2, ///< The workload parameter was invalid.
  StatusInvalidPointer     = 3, ///< The required pointer argument was not set.
  StatusTooManyStreams     = 4, ///< The maximum number of streams has already been allocated.
  StatusTooManyWorkloads   = 5, ///< The maximum number of active workloads for a stream has been reached.
  StatusStreamNotAllocated = 6, ///< The stream to be destroyed was not created.
  StatusInternalError      = 7, ///< The system was unable to perform the work.
};

typedef uint32_t SceWorkloadStream;