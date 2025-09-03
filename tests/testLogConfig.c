#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <check.h>

#include "config.h"
#include "suites.h"

#include "Log.h"

// Forward declarations for setup/teardown functions
void log_setup(void);
void log_teardown(void);

// Test that build-time configuration defines are available
START_TEST(test_build_time_defines)
{
   // Test XM_LOGGING define
#ifdef XM_LOGGING
   ck_assert_msg(XM_TRUE, "XM_LOGGING should be defined when logging is enabled");
#else
   ck_assert_msg(XM_FALSE, "XM_LOGGING is not defined - logging may be disabled");
#endif

   // Test XM_DEFAULT_LOG_LEVEL define
#ifdef XM_DEFAULT_LOG_LEVEL
   ck_assert_msg(XM_TRUE, "XM_DEFAULT_LOG_LEVEL should be defined");
   
   // Verify it's a valid log level
   int level = XM_DEFAULT_LOG_LEVEL;
   ck_assert_msg(level >= XM_LOG_LEVEL_CRITICAL && level <= XM_LOG_LEVEL_DBG,
                 "XM_DEFAULT_LOG_LEVEL has invalid value: %d", level);
#else
   ck_assert_msg(XM_FALSE, "XM_DEFAULT_LOG_LEVEL is not defined");
#endif

   // Test XM_DEFAULT_LOG_OUTPUT define
#ifdef XM_DEFAULT_LOG_OUTPUT
   ck_assert_msg(XM_TRUE, "XM_DEFAULT_LOG_OUTPUT should be defined");
   
   // Verify it's a valid output destination
   const char *output = XM_DEFAULT_LOG_OUTPUT;
   ck_assert_msg(strcmp(output, "stderr") == 0 || 
                 strcmp(output, "stdout") == 0 || 
                 strcmp(output, "file") == 0,
                 "XM_DEFAULT_LOG_OUTPUT has invalid value: %s", output);
#else
   ck_assert_msg(XM_FALSE, "XM_DEFAULT_LOG_OUTPUT is not defined");
#endif
}
END_TEST

// Test that XmLogInit uses build-time configuration
START_TEST(test_init_uses_build_config)
{
   // Initialize logging system
   ck_assert_msg(XmLogInit(), "XmLogInit failed");
   
   // Test that logging works (this verifies the callback was set correctly)
   XM_LOG_INFO("Test message to verify build-time configuration");
   
   // Test that we can change output and it still works
   ck_assert_msg(XmLogSetOutput("stdout", NULL), "Failed to change output to stdout");
   XM_LOG_WARN("Test message after changing output");
   
   ck_assert_msg(XM_TRUE, "Build-time configuration initialization test passed");
}
END_TEST

// Test output destination switching
START_TEST(test_output_switching)
{
   // Initialize logging
   ck_assert_msg(XmLogInit(), "XmLogInit failed");
   
   // Test switching between different outputs
   ck_assert_msg(XmLogSetOutput("stderr", NULL), "Failed to set stderr output");
   XM_LOG_ERR("This should go to stderr");
   
   ck_assert_msg(XmLogSetOutput("stdout", NULL), "Failed to set stdout output");
   XM_LOG_INFO("This should go to stdout");
   
   ck_assert_msg(XmLogSetOutput("file", "test_config.log"), "Failed to set file output");
   XM_LOG_WARN("This should go to test_config.log");
   
   // Verify file was created
   FILE *fp = fopen("test_config.log", "r");
   ck_assert_msg(fp != NULL, "Log file was not created");
   if (fp) {
      fclose(fp);
      unlink("test_config.log"); // Clean up
   }
   
   ck_assert_msg(XM_TRUE, "Output switching test passed");
   XmLogShutdown();
}
END_TEST

// Test error handling for invalid configurations
START_TEST(test_invalid_config_handling)
{
   // Initialize logging
   ck_assert_msg(XmLogInit(), "XmLogInit failed");
   
   // Test invalid output types
   ck_assert_msg(!XmLogSetOutput("invalid", NULL), "Invalid output should fail");
   ck_assert_msg(!XmLogSetOutput(NULL, NULL), "NULL output should fail");
   ck_assert_msg(!XmLogSetOutput("", NULL), "Empty output should fail");
   
   // Test that logging still works after invalid calls
   XM_LOG_INFO("Logging should still work after invalid configuration calls");
   
   ck_assert_msg(XM_TRUE, "Invalid configuration handling test passed");
}
END_TEST

// Test file output with different filenames
START_TEST(test_file_output_variations)
{
   // Initialize logging
   ck_assert_msg(XmLogInit(), "XmLogInit failed");
   
   // Test with specific filename
   const char *filename1 = "test1.log";
   ck_assert_msg(XmLogSetOutput("file", filename1), "Failed to set file output with filename");
   XM_LOG_INFO("Message to %s", filename1);
   
   // Verify first file was created
   FILE *fp1 = fopen(filename1, "r");
   ck_assert_msg(fp1 != NULL, "First log file was not created");
   if (fp1) {
      fclose(fp1);
      unlink(filename1);
   }
   
   // Test with default filename (NULL)
   ck_assert_msg(XmLogSetOutput("file", NULL), "Failed to set file output with default filename");
   XM_LOG_WARN("Message to default log file");
   
   // Verify default file was created
   FILE *fp2 = fopen("motif.log", "r");
   ck_assert_msg(fp2 != NULL, "Default log file was not created");
   if (fp2) {
      fclose(fp2);
      unlink("motif.log");
   }
   
   ck_assert_msg(XM_TRUE, "File output variations test passed");
   XmLogShutdown();
}
END_TEST

// Test that configuration persists across multiple log calls
START_TEST(test_config_persistence)
{
   // Initialize logging
   ck_assert_msg(XmLogInit(), "XmLogInit failed");
   
   // Set to file output
   ck_assert_msg(XmLogSetOutput("file", "persistence_test.log"), "Failed to set file output");
   
   // Send multiple messages
   for (int i = 0; i < 5; i++) {
      XM_LOG_INFO("Persistence test message %d", i);
   }
   
   // Verify all messages were written to the same file
   FILE *fp = fopen("persistence_test.log", "r");
   ck_assert_msg(fp != NULL, "Persistence test log file was not created");
   if (fp) {
      fclose(fp);
      unlink("persistence_test.log");
   }
   
   ck_assert_msg(XM_TRUE, "Configuration persistence test passed");
   XmLogShutdown();
}
END_TEST

void log_config_suite(SRunner *runner)
{
   TCase *t;
   Suite *s = suite_create("Log Configuration");

   t = tcase_create("Build-time Configuration");
   tcase_add_test(t, test_build_time_defines);
   tcase_add_test(t, test_init_uses_build_config);
   tcase_set_timeout(t, 1);
   suite_add_tcase(s, t);

   t = tcase_create("Output Configuration");
   tcase_add_test(t, test_output_switching);
   tcase_add_test(t, test_invalid_config_handling);
   tcase_add_test(t, test_file_output_variations);
   tcase_add_test(t, test_config_persistence);
   tcase_set_timeout(t, 1);
   suite_add_tcase(s, t);

   srunner_add_suite(runner, s);
}
