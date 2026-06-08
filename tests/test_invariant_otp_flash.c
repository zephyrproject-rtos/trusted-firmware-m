#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Include the production code and its dependencies */
#include "tfm_plat_otp.h"
#include "flash_otp_nv_counters_backend.h"

START_TEST(test_otp_read_no_overread)
{
    /* Invariant: Reading an OTP element with a length larger than the element's
     * actual size MUST NOT succeed or MUST only return the element's true size
     * worth of data — never leaking adjacent secrets. */

    struct {
        enum tfm_otp_element_id_t id;
        size_t legitimate_size;
        size_t oversized_len;
    } cases[] = {
        /* Exact exploit: request far more than HUK size (32 bytes) */
        { PLAT_OTP_ID_HUK,      32,  512 },
        /* Boundary: one byte over IAK size */
        { PLAT_OTP_ID_IAK,      32,  33  },
        /* Valid input: exact size should succeed normally */
        { PLAT_OTP_ID_IAK_LEN,  4,   4   },
    };
    int num_cases = sizeof(cases) / sizeof(cases[0]);

    for (int i = 0; i < num_cases; i++) {
        uint8_t buf[1024];
        memset(buf, 0xAA, sizeof(buf));

        enum tfm_plat_err_t err = tfm_plat_otp_read(
            cases[i].id, cases[i].oversized_len, buf);

        if (cases[i].oversized_len > cases[i].legitimate_size) {
            /* Either the call must fail, or bytes beyond the legitimate
             * element size must remain untouched (no adjacent data leaked). */
            if (err == TFM_PLAT_ERR_SUCCESS) {
                /* Verify no data was written past the element's true size */
                for (size_t j = cases[i].legitimate_size; j < cases[i].oversized_len; j++) {
                    ck_assert_msg(buf[j] == 0xAA || buf[j] == 0x00,
                        "OTP over-read: adjacent memory leaked at offset %zu for element %d",
                        j, cases[i].id);
                }
            }
            /* If err != SUCCESS, the function correctly rejected the oversized read */
        } else {
            /* Valid size: should succeed */
            ck_assert_int_eq(err, TFM_PLAT_ERR_SUCCESS);
        }
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_otp_read_no_overread);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}