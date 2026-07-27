/**************************************************/
/*
 *   Copyright (c) 2017 System LSI, Samsung Electronics, Inc.
 *   All right reserved.

 *   This software is the confidential and proprietary information
 *   of Samsung Electronics, Inc. (Confidential Information). You
 *   shall not disclose such Confidential Information and shall use
 *   it only in accordance with the terms of the license agreement
 *   you entered into with Samsung Electronics.
 */
/**************************************************/

#ifndef _test_desc_H_
#define _test_desc_H_

#include "in/sfr_ns_02_cmd.h"
#include "in/sfr_s_07_cmd.h"
#include "in/in_02_bin.h"
#include "in/sfr_s_10_cmd.h"
#include "in/in_08_bin.h"
#include "in/sfr_s_09_cmd.h"
#include "in/sfr_ns_10_cmd.h"
#include "in/sfr_s_04_cmd.h"
#include "in/in_03_bin.h"
#include "in/sfr_s_06_cmd.h"
#include "in/sfr_s_08_cmd.h"
#include "in/in_06_bin.h"
#include "in/sfr_ns_06_cmd.h"
#include "in/sfr_s_05_cmd.h"
#include "in/sfr_ns_04_cmd.h"
#include "in/in_07_bin.h"
#include "in/in_05_bin.h"
#include "in/in_01_bin.h"
#include "in/sfr_ns_03_cmd.h"
#include "in/in_10_bin.h"
#include "in/sfr_ns_01_cmd.h"
#include "in/sfr_ns_09_cmd.h"
#include "in/sfr_s_01_cmd.h"
#include "in/sfr_ns_07_cmd.h"
#include "in/in_09_bin.h"
#include "in/sfr_s_03_cmd.h"
#include "in/sfr_s_02_cmd.h"
#include "in/in_04_bin.h"
#include "in/sfr_ns_05_cmd.h"
#include "in/sfr_ns_08_cmd.h"

#include "gd/gl_03_bin.h"
#include "gd/gl_04_bin.h"
#include "gd/gl_10_bin.h"
#include "gd/gl_06_bin.h"
#include "gd/gl_08_bin.h"
#include "gd/gl_05_bin.h"
#include "gd/gl_01_bin.h"
#include "gd/gl_09_bin.h"
#include "gd/gl_02_bin.h"
#include "gd/gl_07_bin.h"

struct tsmux_reg;

typedef struct tsmux_testcase {
	int type;
	const struct tsmux_reg *nonsecure_sfr_dump;
	int nonsecure_sfr_count;
	const struct tsmux_reg *secure_sfr_dump;
	int secure_sfr_count;
	const unsigned char *input;
	int input_size;
	const unsigned char *golden;
	int golden_size;
} tsmux_testcase;

tsmux_testcase tsmux_testcases[] = {
{
	.type = TSMUX_M2M,
	.nonsecure_sfr_dump = sfr_ns_01,
	.nonsecure_sfr_count = ARRAY_SIZE(sfr_ns_01),
	.secure_sfr_dump = sfr_s_01,
	.secure_sfr_count = ARRAY_SIZE(sfr_s_01),
	.input = in_01,
	.input_size = sizeof(in_01),
	.golden = gl_01,
	.golden_size = sizeof(gl_01),
},
{
	.type = TSMUX_M2M,
	.nonsecure_sfr_dump = sfr_ns_02,
	.nonsecure_sfr_count = ARRAY_SIZE(sfr_ns_02),
	.secure_sfr_dump = sfr_s_02,
	.secure_sfr_count = ARRAY_SIZE(sfr_s_02),
	.input = in_02,
	.input_size = sizeof(in_02),
	.golden = gl_02,
	.golden_size = sizeof(gl_02),
},
{
	.type = TSMUX_M2M,
	.nonsecure_sfr_dump = sfr_ns_03,
	.nonsecure_sfr_count = ARRAY_SIZE(sfr_ns_03),
	.secure_sfr_dump = sfr_s_03,
	.secure_sfr_count = ARRAY_SIZE(sfr_s_03),
	.input = in_03,
	.input_size = sizeof(in_03),
	.golden = gl_03,
	.golden_size = sizeof(gl_03),
},
{
	.type = TSMUX_M2M,
	.nonsecure_sfr_dump = sfr_ns_04,
	.nonsecure_sfr_count = ARRAY_SIZE(sfr_ns_04),
	.secure_sfr_dump = sfr_s_04,
	.secure_sfr_count = ARRAY_SIZE(sfr_s_04),
	.input = in_04,
	.input_size = sizeof(in_04),
	.golden = gl_04,
	.golden_size = sizeof(gl_04),
},
{
	.type = TSMUX_M2M,
	.nonsecure_sfr_dump = sfr_ns_05,
	.nonsecure_sfr_count = ARRAY_SIZE(sfr_ns_05),
	.secure_sfr_dump = sfr_s_05,
	.secure_sfr_count = ARRAY_SIZE(sfr_s_05),
	.input = in_05,
	.input_size = sizeof(in_05),
	.golden = gl_05,
	.golden_size = sizeof(gl_05),
},
{
	.type = TSMUX_M2M,
	.nonsecure_sfr_dump = sfr_ns_06,
	.nonsecure_sfr_count = ARRAY_SIZE(sfr_ns_06),
	.secure_sfr_dump = sfr_s_06,
	.secure_sfr_count = ARRAY_SIZE(sfr_s_06),
	.input = in_06,
	.input_size = sizeof(in_06),
	.golden = gl_06,
	.golden_size = sizeof(gl_06),
},
{
	.type = TSMUX_M2M,
	.nonsecure_sfr_dump = sfr_ns_07,
	.nonsecure_sfr_count = ARRAY_SIZE(sfr_ns_07),
	.secure_sfr_dump = sfr_s_07,
	.secure_sfr_count = ARRAY_SIZE(sfr_s_07),
	.input = in_07,
	.input_size = sizeof(in_07),
	.golden = gl_07,
	.golden_size = sizeof(gl_07),
},
{
	.type = TSMUX_M2M,
	.nonsecure_sfr_dump = sfr_ns_08,
	.nonsecure_sfr_count = ARRAY_SIZE(sfr_ns_08),
	.secure_sfr_dump = sfr_s_08,
	.secure_sfr_count = ARRAY_SIZE(sfr_s_08),
	.input = in_08,
	.input_size = sizeof(in_08),
	.golden = gl_08,
	.golden_size = sizeof(gl_08),
},
{
	.type = TSMUX_M2M,
	.nonsecure_sfr_dump = sfr_ns_09,
	.nonsecure_sfr_count = ARRAY_SIZE(sfr_ns_09),
	.secure_sfr_dump = sfr_s_09,
	.secure_sfr_count = ARRAY_SIZE(sfr_s_09),
	.input = in_09,
	.input_size = sizeof(in_09),
	.golden = gl_09,
	.golden_size = sizeof(gl_09),
},
{
	.type = TSMUX_M2M,
	.nonsecure_sfr_dump = sfr_ns_10,
	.nonsecure_sfr_count = ARRAY_SIZE(sfr_ns_10),
	.secure_sfr_dump = sfr_s_10,
	.secure_sfr_count = ARRAY_SIZE(sfr_s_10),
	.input = in_10,
	.input_size = sizeof(in_10),
	.golden = gl_10,
	.golden_size = sizeof(gl_10),
},
};

#define NUM_TEST_CASE ARRAY_SIZE(tsmux_testcases)

#endif  /* _test_desc_H_ */
