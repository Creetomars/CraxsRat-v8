// SPDX-FileCopyrightText: 2022 GustavoLCR
// SPDX-License-Identifier: LGPL-3.0-only

#include "common_windows.h"

#define EXCEPTION_ACCESS_VIOLATION         0xC0000005
#define EXCEPTION_DATATYPE_MISALIGNMENT    0x80000002
#define EXCEPTION_BREAKPOINT               0x80000003
#define EXCEPTION_SINGLE_STEP              0x80000004
#define EXCEPTION_ARRAY_BOUNDS_EXCEEDED    0xC000008C
#define EXCEPTION_FLT_DENORMAL_OPERAND     0xC000008D
#define EXCEPTION_FLT_DIVIDE_BY_ZERO       0xC000008E
#define EXCEPTION_FLT_INEXACT_RESULT       0xC000008F
#define EXCEPTION_FLT_INVALID_OPERATION    0xC0000090
#define EXCEPTION_FLT_OVERFLOW             0xC0000091
#define EXCEPTION_FLT_STACK_CHECK          0xC0000092
#define EXCEPTION_FLT_UNDERFLOW            0xC0000093
#define EXCEPTION_INT_DIVIDE_BY_ZERO       0xC0000094
#define EXCEPTION_INT_OVERFLOW             0xC0000095
#define EXCEPTION_PRIV_INSTRUCTION         0xC0000096
#define EXCEPTION_IN_PAGE_ERROR            0xC0000006
#define EXCEPTION_ILLEGAL_INSTRUCTION      0xC000001D
#define EXCEPTION_NONCONTINUABLE_EXCEPTION 0xC0000025
#define EXCEPTION_STACK_OVERFLOW           0xC00000FD
#define EXCEPTION_INVALID_DISPOSITION      0xC0000026
#define EXCEPTION_GUARD_PAGE               0x80000001
#define EXCEPTION_INVALID_HANDLE           0xC0000008
#define EXCEPTION_POSSIBLE_DEADLOCK        0xC0000194
#define CONTROL_C_EXIT                     0x40010005
#define DBG_CONTROL_BREAK                  0x40010008
#define STATUS_UNWIND_CONSOLIDATE          0x80000029

RzDebugReasonType windows_exception_to_reason(ut32 exception_code) {
	switch (exception_code) {
	case EXCEPTION_ACCESS_VIOLATION:
	case EXCEPTION_GUARD_PAGE:
		return RZ_DEBUG_REASON_SEGFAULT;
	case EXCEPTION_BREAKPOINT:
		return RZ_DEBUG_REASON_BREAKPOINT;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
	case EXCEPTION_FLT_INEXACT_RESULT:
	case EXCEPTION_FLT_INVALID_OPERATION:
	case EXCEPTION_FLT_OVERFLOW:
	case EXCEPTION_FLT_STACK_CHECK:
	case EXCEPTION_FLT_UNDERFLOW:
		return RZ_DEBUG_REASON_FPU;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		return RZ_DEBUG_REASON_ILLEGAL;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		return RZ_DEBUG_REASON_DIVBYZERO;
	case EXCEPTION_SINGLE_STEP:
		return RZ_DEBUG_REASON_STEP;
	default:
		return RZ_DEBUG_REASON_TRAP;
	}
}

static const char *get_exception_name(ut32 ExceptionCode) {
#define EXCEPTION_STR(x) \
	case x: return #x
	switch (ExceptionCode) {
		EXCEPTION_STR(EXCEPTION_ACCESS_VIOLATION);
		EXCEPTION_STR(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
		EXCEPTION_STR(EXCEPTION_BREAKPOINT);
		EXCEPTION_STR(EXCEPTION_DATATYPE_MISALIGNMENT);
		EXCEPTION_STR(EXCEPTION_FLT_DENORMAL_OPERAND);
		EXCEPTION_STR(EXCEPTION_FLT_DIVIDE_BY_ZERO);
		EXCEPTION_STR(EXCEPTION_FLT_INEXACT_RESULT);
		EXCEPTION_STR(EXCEPTION_FLT_INVALID_OPERATION);
		EXCEPTION_STR(EXCEPTION_FLT_OVERFLOW);
		EXCEPTION_STR(EXCEPTION_FLT_STACK_CHECK);
		EXCEPTION_STR(EXCEPTION_FLT_UNDERFLOW);
		EXCEPTION_STR(EXCEPTION_GUARD_PAGE);
		EXCEPTION_STR(EXCEPTION_ILLEGAL_INSTRUCTION);
		EXCEPTION_STR(EXCEPTION_IN_PAGE_ERROR);
		EXCEPTION_STR(EXCEPTION_INT_DIVIDE_BY_ZERO);
		EXCEPTION_STR(EXCEPTION_INT_OVERFLOW);
		EXCEPTION_STR(EXCEPTION_INVALID_DISPOSITION);
		EXCEPTION_STR(EXCEPTION_INVALID_HANDLE);
		EXCEPTION_STR(EXCEPTION_NONCONTINUABLE_EXCEPTION);
		EXCEPTION_STR(EXCEPTION_PRIV_INSTRUCTION);
		EXCEPTION_STR(EXCEPTION_SINGLE_STEP);
		EXCEPTION_STR(EXCEPTION_STACK_OVERFLOW);
		EXCEPTION_STR(STATUS_UNWIND_CONSOLIDATE);
		EXCEPTION_STR(EXCEPTION_POSSIBLE_DEADLOCK);
		EXCEPTION_STR(DBG_CONTROL_BREAK);
		EXCEPTION_STR(CONTROL_C_EXIT);
	case 0x6BA: return "FILE_DIALOG_EXCEPTION";
	case 0x406D1388: return "MS_VC_EXCEPTION";
	default:
		return "Unknown";
	}
#undef EXCEPTION_STR
}

bool windows_is_exception_fatal(ut32 exception_code) {
	switch (exception_code) {
	case EXCEPTION_ACCESS_VIOLATION:
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
	case EXCEPTION_ILLEGAL_INSTRUCTION:
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
	case EXCEPTION_STACK_OVERFLOW:
	case EXCEPTION_GUARD_PAGE:
	case EXCEPTION_PRIV_INSTRUCTION:
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
	case EXCEPTION_FLT_STACK_CHECK:
	case EXCEPTION_IN_PAGE_ERROR:
		return true;
	default:
		return false;
	}
}

void windows_print_exception_event(ut32 pid, ut32 tid, ut32 exception_code, bool second_chance) {
	bool is_fatal = windows_is_exception_fatal(exception_code);
	RZ_LOG_INFO("(%" PFMT32u ") %s Exception %04X (%s) in thread %" PFMT32u "\n",
		pid,
		is_fatal ? "Fatal" : "Non-fatal",
		exception_code, get_exception_name(exception_code),
		tid);
	if (second_chance) {
		RZ_LOG_WARN("A second-chance exception has ocurred!\n");
	}
	if (is_fatal) {
		RZ_LOG_INFO("Use 'dce' continue into exception handler\n");
	}
}
