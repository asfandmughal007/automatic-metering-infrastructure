// Tiny test helper.
//   CHECK      - must pass.
//   KNOWN_BUG  - documents a defect in the original FYP code. It reports XFAIL while the bug is present
//                and turns into a hard failure (XPASS) once the code is fixed, so the test gets updated.
#pragma once
#include <cmath>
#include <cstdio>
static int g_fail = 0, g_pass = 0, g_xfail = 0;

#define CHECK(cond, msg)                                                  \
  do {                                                                    \
    if (cond) { ++g_pass; std::printf("  PASS   %s\n", msg); }            \
    else { ++g_fail; std::printf("  FAIL   %s   [line %d]\n", msg, __LINE__); } \
  } while (0)

#define CHECK_NEAR(val, expect, tol, msg)                                                      \
  do {                                                                                         \
    double _v = (val), _e = (expect);                                                          \
    if (std::fabs(_v - _e) <= (tol)) {                                                         \
      ++g_pass; std::printf("  PASS   %s (got %.3f, want %.3f +/-%.3f)\n", msg, _v, _e, (double)(tol)); \
    } else {                                                                                   \
      ++g_fail; std::printf("  FAIL   %s (got %.3f, want %.3f +/-%.3f)\n", msg, _v, _e, (double)(tol)); \
    }                                                                                          \
  } while (0)

// `cond` is the CORRECT behaviour, which the original code currently does not deliver.
#define KNOWN_BUG(cond, msg)                                                                   \
  do {                                                                                         \
    if (!(cond)) { ++g_xfail; std::printf("  XFAIL  %s  (known bug, see docs/KNOWN_ISSUES.md)\n", msg); } \
    else { ++g_fail; std::printf("  XPASS  %s  (bug appears fixed - update test and docs)\n", msg); } \
  } while (0)

#define SUMMARY()                                                                              \
  do {                                                                                         \
    std::printf("\n%d passed, %d known-bug (xfail), %d failed\n", g_pass, g_xfail, g_fail);   \
    return g_fail ? 1 : 0;                                                                     \
  } while (0)
