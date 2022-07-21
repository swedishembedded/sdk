/**
 * This file should be included into every unit test before the unit under test!
 **/

#ifndef CMOCK_UNIT_H_
#define CMOCK_UNIT_H_

/** Remove the device tree driver init macro */
#undef DT_INST_FOREACH_STATUS_OKAY
#define DT_INST_FOREACH_STATUS_OKAY(macro)

#endif
