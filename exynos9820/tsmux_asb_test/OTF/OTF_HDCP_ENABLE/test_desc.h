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

#include "in/in_02_bin.h"
#include "in/sfr_ns_01_cmd.h"
#include "in/sfr_s_02_cmd.h"
#include "in/sfr_s_01_cmd.h"
#include "in/in_01_bin.h"
#include "in/sfr_ns_02_cmd.h"

#include "gd/gl_01_bin.h"
#include "gd/gl_02_bin.h"

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
	.type = TSMUX_OTF,
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
	.type = TSMUX_OTF,
	.nonsecure_sfr_dump = sfr_ns_02,
	.nonsecure_sfr_count = ARRAY_SIZE(sfr_ns_02),
	.secure_sfr_dump = sfr_s_02,
	.secure_sfr_count = ARRAY_SIZE(sfr_s_02),
	.input = in_02,
	.input_size = sizeof(in_02),
	.golden = gl_02,
	.golden_size = sizeof(gl_02),
},
};

#define NUM_TEST_CASE ARRAY_SIZE(tsmux_testcases)

#endif  /* _test_desc_H_ */
