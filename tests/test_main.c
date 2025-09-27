#define TESTING
#include "../src/main.c"
#include <check.h>
#include <string.h>

void setup(void) {
  bg_count = 0;
  memset(bg_processes, 0, sizeof(bg_processes));
}

START_TEST(test_parse_command_basic) {
  char input[] = "ls -l";
  char *args[MAX_ARGS];
  int background = 0;
  parse_command(input, args, &background);

  ck_assert_str_eq(args[0], "ls");
  ck_assert_str_eq(args[1], "-l");
  ck_assert_ptr_eq(args[2], NULL);
  ck_assert_int_eq(background, 0);
}
END_TEST

START_TEST(test_parse_command_background) {
  char input[] = "sleep 1 &";
  char *args[MAX_ARGS];
  int background = 0;
  parse_command(input, args, &background);

  ck_assert_str_eq(args[0], "sleep");
  ck_assert_str_eq(args[1], "1");
  ck_assert_ptr_eq(args[2], NULL);
  ck_assert_int_eq(background, 1);
}
END_TEST

START_TEST(test_is_internal_command_exit) {
  char *args[] = {"exit", NULL};
  ck_assert_int_eq(is_internal_command(args), 1);
}
END_TEST

START_TEST(test_is_internal_command_external) {
  char *args[] = {"ls", NULL};
  ck_assert_int_eq(is_internal_command(args), 0);
}
END_TEST

Suite *minishell_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Minishell");
  tc_core = tcase_create("Core");

  tcase_add_checked_fixture(tc_core, setup, NULL);
  tcase_add_test(tc_core, test_parse_command_basic);
  tcase_add_test(tc_core, test_parse_command_background);
  tcase_add_test(tc_core, test_is_internal_command_exit);
  tcase_add_test(tc_core, test_is_internal_command_external);

  suite_add_tcase(s, tc_core);
  return s;
}

int main(void) {
  int number_failed;
  Suite *s;
  SRunner *sr;

  s = minishell_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_VERBOSE);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? 0 : 1;
}
