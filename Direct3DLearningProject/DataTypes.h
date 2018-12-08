#pragma once

#if UNICODE
#define MESSAGE_FAILED(HR, STRING) CheckFailW(HR, STRING)
#elif
#define MESSAGE_FAILED(HR, STRING) CheckFailA(HR, STRING)
#endif