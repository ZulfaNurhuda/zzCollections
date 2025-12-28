#ifndef COMMON_H
#define COMMON_H

/**
 * @file common.h
 * @brief Backward compatibility header
 *
 * This header provides backward compatibility by including both
 * types.h (callback typedefs) and utils.h (default implementations).
 *
 * New code should include types.h and utils.h directly for better
 * modularity. This header is maintained for existing code compatibility.
 */

#include "types.h"
#include "utils.h"

#endif // COMMON_H
