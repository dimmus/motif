#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <check.h>
#include "config.h"
#include "Log.h"
#include "suites.h"

// Test function to demonstrate logging
void test_function(void) {
   XM_LOG_DBG("This is a debug message from test_function");
   XM_LOG_INFO("This is an info message from test_function");
   XM_LOG_WARN("This is a warning message from test_function");
   XM_LOG_ERR("This is an error message from test_function");
}

// Thread function for testing multi-threaded logging
void *test_thread(void *arg) {
   int thread_id = *(int*)arg;
   
   for (int i = 0; i < 5; i++) {
      XM_LOG_INFO("Thread %d: Message %d", thread_id, i);
      struct timespec ts = {0, 1000000}; // 1ms delay
      nanosleep(&ts, NULL);
   }
   
   (void)thread_id; // Suppress unused variable warning
   return NULL;
}

START_TEST(test_log_init)
{
   ck_assert_msg(XmLogInit(), "Failed to initialize logging system");
}
END_TEST

START_TEST(test_basic_logging)
{
   ck_assert_msg(XmLogInit(), "Failed to initialize logging system");
   
   // Test basic logging functionality
   XM_LOG_INFO("Application started successfully");
   XM_LOG_WARN("This is a warning message to test the system");
   XM_LOG_ERR("This is an error message for testing");
   XM_LOG_CRIT("This is a critical message");
   
   test_function();
   ck_assert_msg(XM_TRUE, "Basic logging test completed");
   
   XmLogShutdown();
}
END_TEST

START_TEST(test_domain_registration)
{
   ck_assert_msg(XmLogInit(), "Failed to initialize logging system");
   
   int test_domain = XmLogDomainRegister("test_module", LOG_COLOR_CYAN);
   
   ck_assert_msg(test_domain >= 0, "Failed to register test domain");
   
   // Test domain-specific logging
   XM_LOG_DOM_INFO(test_domain, "Domain-specific info message");
   XM_LOG_DOM_WARN(test_domain, "Domain-specific warning message");
   XM_LOG_DOM_ERR(test_domain, "Domain-specific error message");
   
   // Cleanup
   XmLogDomainUnregister(test_domain);
   XmLogShutdown();
}
END_TEST

START_TEST(test_domain_registration_invalid)
{
   int test_domain = XmLogDomainRegister(NULL, LOG_COLOR_CYAN);
   
   ck_assert_msg(test_domain < 0, "Should fail to register domain with NULL name");
}
END_TEST

START_TEST(test_thread_management)
{
   XmLogThreadsEnable();
   
   ck_assert_msg(XmLogThreadsMainCheck(), "Main thread check should pass");
}
END_TEST

START_TEST(test_multi_threaded_logging)
{
   pthread_t threads[2];
   int thread_ids[2] = {1, 2};
   
   for (int i = 0; i < 2; i++) {
      ck_assert_msg(pthread_create(&threads[i], NULL, test_thread, &thread_ids[i]) == 0,
                    "Failed to create thread %d", i + 1);
   }
   
   // Wait for threads to complete
   for (int i = 0; i < 2; i++) {
      pthread_join(threads[i], NULL);
   }
   
   ck_assert_msg(XM_TRUE, "Multi-threaded logging test completed");
}
END_TEST

START_TEST(test_level_management)
{
   int current_level = XmLogLevelGet();
   ck_assert_msg(current_level >= 0, "Should get valid current level");
   
   // Set a new level
   XmLogLevelSet(XM_LOG_LEVEL_WARN);
   int new_level = XmLogLevelGet();
   ck_assert_msg(new_level == XM_LOG_LEVEL_WARN, "Level should be set to WARN");
   
   // Test logging at different levels
   XM_LOG_DBG("This debug message should not appear (level too low)");
   XM_LOG_INFO("This info message should not appear (level too low)");
   XM_LOG_WARN("This warning message should appear");
   XM_LOG_ERR("This error message should appear");
   
   // Restore original level
   XmLogLevelSet(current_level);
   ck_assert_msg(XmLogLevelGet() == current_level, "Level should be restored");
}
END_TEST

START_TEST(test_domain_level_management)
{
   ck_assert_msg(XmLogInit(), "Failed to initialize logging system");
   
   int test_domain = XmLogDomainRegister("level_test", LOG_COLOR_CYAN);
   ck_assert_msg(test_domain >= 0, "Failed to register test domain");
   
   // Set domain level
   XmLogDomainRegisteredLevelSet(test_domain, XM_LOG_LEVEL_ERR);
   int domain_level = XmLogDomainRegisteredLevelGet(test_domain);
   ck_assert_msg(domain_level == XM_LOG_LEVEL_ERR, "Domain level should be set to ERR");
   
   // Test logging at domain level
   XM_LOG_DOM_INFO(test_domain, "This info message should not appear");
   XM_LOG_DOM_WARN(test_domain, "This warning message should not appear");
   XM_LOG_DOM_ERR(test_domain, "This error message should appear");
   
   // Reset domain level
   XmLogDomainRegisteredLevelSet(test_domain, XM_LOG_LEVEL_INFO);
   ck_assert_msg(XmLogDomainRegisteredLevelGet(test_domain) == XM_LOG_LEVEL_INFO,
                 "Domain level should be reset to INFO");
   
   // Cleanup
   XmLogDomainUnregister(test_domain);
   XmLogShutdown();
}
END_TEST

START_TEST(test_domain_level_management_invalid)
{
   // Test with invalid domain
   XmLogDomainRegisteredLevelSet(-1, XM_LOG_LEVEL_ERR);
   ck_assert_msg(XmLogDomainRegisteredLevelGet(-1) == XM_LOG_LEVEL_UNKNOWN,
                 "Invalid domain should return UNKNOWN level");
}
END_TEST

START_TEST(test_print_callback_switching)
{
   // Switch to stdout callback
   XmLogPrintCbSet(XmLogPrintCbStdOut, NULL);
   XM_LOG_INFO("This message should go to stdout");
   
   // Switch back to stderr callback
   XmLogPrintCbSet(XmLogPrintCbStdErr, NULL);
   XM_LOG_INFO("This message should go to stderr");
   
   ck_assert_msg(XM_TRUE, "Print callback switching test completed");
}
END_TEST

START_TEST(test_color_functions)
{
   // Test color functions
   const char *red_color = XmLogLevelColorGet(XM_LOG_LEVEL_ERR);
   const char *green_color = XmLogLevelColorGet(XM_LOG_LEVEL_INFO);
   const char *yellow_color = XmLogLevelColorGet(XM_LOG_LEVEL_WARN);
   const char *blue_color = XmLogLevelColorGet(XM_LOG_LEVEL_DBG);
   
   ck_assert_msg(red_color != NULL, "Should get red color for ERROR level");
   ck_assert_msg(green_color != NULL, "Should get green color for INFO level");
   ck_assert_msg(yellow_color != NULL, "Should get yellow color for WARN level");
   ck_assert_msg(blue_color != NULL, "Should get blue color for DEBUG level");
   
   // Test invalid level
   const char *invalid_color = XmLogLevelColorGet(99);
   ck_assert_msg(invalid_color != NULL && strlen(invalid_color) == 0,
                 "Invalid level should return empty string");
}
END_TEST

START_TEST(test_domain_by_name)
{
   int test_domain = XmLogDomainRegister("name_test", LOG_COLOR_CYAN);
   ck_assert_msg(test_domain >= 0, "Failed to register test domain");
   
   // Test setting level by name
   XmLogDomainLevelSet("name_test", XM_LOG_LEVEL_WARN);
   int level = XmLogDomainLevelGet("name_test");
   ck_assert_msg(level == XM_LOG_LEVEL_WARN, "Should get WARN level for name_test domain");
   
   // Test with invalid name
   int invalid_level = XmLogDomainLevelGet("nonexistent");
   ck_assert_msg(invalid_level == XM_LOG_LEVEL_UNKNOWN,
                 "Invalid domain name should return UNKNOWN level");
   
   // Cleanup
   XmLogDomainUnregister(test_domain);
}
END_TEST

START_TEST(test_domain_by_name_invalid)
{
   // Test with NULL name
   XmLogDomainLevelSet(NULL, XM_LOG_LEVEL_WARN);
   int level = XmLogDomainLevelGet(NULL);
   ck_assert_msg(level == XM_LOG_LEVEL_UNKNOWN,
                 "NULL domain name should return UNKNOWN level");
}
END_TEST

START_TEST(test_comprehensive_logging)
{
   ck_assert_msg(XmLogInit(), "Failed to initialize logging system");
   
   // Final comprehensive test
   XM_LOG_INFO("=== Final test message ===");
   XM_LOG_WARN("Testing all features together");
   XM_LOG_ERR("Error handling test");
   
   // Test with different domains
   int test_domain = XmLogDomainRegister("comprehensive_test", LOG_COLOR_CYAN);
   if (test_domain >= 0) {
      XM_LOG_DOM_INFO(test_domain, "Final domain-specific message");
      XmLogDomainUnregister(test_domain);
   }
   
   ck_assert_msg(XM_TRUE, "Comprehensive logging test completed");
   XmLogShutdown();
}
END_TEST

// Test output configuration initialization
START_TEST(test_output_init)
{
   // Test that XmLogInit can be called multiple times safely
   ck_assert_msg(XmLogInit(), "First XmLogInit call failed");
   ck_assert_msg(XmLogInit(), "Second XmLogInit call failed");
   
   // Test that logging works after initialization
   XM_LOG_INFO("Test message after initialization");
   
   ck_assert_msg(XM_TRUE, "Output initialization test passed");
}
END_TEST

// Test setting output to stdout
START_TEST(test_output_set_stdout)
{
   ck_assert_msg(XmLogSetOutput("stdout", NULL), "Failed to set output to stdout");
   
   // Test that logging works with stdout
   XM_LOG_INFO("This message should go to stdout");
   
   ck_assert_msg(XM_TRUE, "stdout output test passed");
}
END_TEST

// Test setting output to stderr
START_TEST(test_output_set_stderr)
{
   ck_assert_msg(XmLogSetOutput("stderr", NULL), "Failed to set output to stderr");
   
   // Test that logging works with stderr
   XM_LOG_WARN("This message should go to stderr");
   
   ck_assert_msg(XM_TRUE, "stderr output test passed");
}
END_TEST

// Test setting output to file
START_TEST(test_output_set_file)
{
   ck_assert_msg(XmLogInit(), "Failed to initialize logging system");
   
   const char *test_file = "test_output.log";
   
   ck_assert_msg(XmLogSetOutput("file", test_file), "Failed to set output to file");
   
   // Test that logging works with file output
   XM_LOG_ERR("This message should be written to %s", test_file);
   
   // Verify file was created
   FILE *fp = fopen(test_file, "r");
   ck_assert_msg(fp != NULL, "Log file was not created");
   if (fp) {
      fclose(fp);
      // Clean up test file
      unlink(test_file);
   }
   
   ck_assert_msg(XM_TRUE, "file output test passed");
   XmLogShutdown();
}
END_TEST

// Test invalid output settings
START_TEST(test_output_invalid)
{
   // Test invalid output type
   ck_assert_msg(!XmLogSetOutput("invalid", NULL), "Invalid output should fail");
   ck_assert_msg(!XmLogSetOutput(NULL, NULL), "NULL output should fail");
   
   // Test that logging still works after invalid calls
   XM_LOG_INFO("Logging should still work after invalid output calls");
   
   ck_assert_msg(XM_TRUE, "Invalid output test passed");
}
END_TEST

// Test build-time configuration
START_TEST(test_build_time_config)
{
   // Test that build-time configuration is available
#ifdef XM_DEFAULT_LOG_LEVEL
   ck_assert_msg(XM_TRUE, "XM_DEFAULT_LOG_LEVEL is defined");
#else
   ck_assert_msg(XM_FALSE, "XM_DEFAULT_LOG_LEVEL should be defined");
#endif

#ifdef XM_DEFAULT_LOG_OUTPUT
   ck_assert_msg(XM_TRUE, "XM_DEFAULT_LOG_OUTPUT is defined");
   
   // Test that the output value is valid
   const char *output = XM_DEFAULT_LOG_OUTPUT;
   ck_assert_msg(strcmp(output, "stderr") == 0 || 
                 strcmp(output, "stdout") == 0 || 
                 strcmp(output, "file") == 0, 
                 "XM_DEFAULT_LOG_OUTPUT has invalid value: %s", output);
#else
   ck_assert_msg(XM_FALSE, "XM_DEFAULT_LOG_OUTPUT should be defined");
#endif
   
   ck_assert_msg(XM_TRUE, "Build-time configuration test passed");
}
END_TEST

// Setup function for Log tests
void log_setup(void)
{
   XmLogInit();
}

// Teardown function for Log tests
void log_teardown(void)
{
   XmLogShutdown();
}

void log_suite(SRunner *runner)
{
   TCase *t;
   Suite *s = suite_create("Log");

   t = tcase_create("Initialization");
   tcase_add_test(t, test_log_init);
   tcase_set_timeout(t, 1);
   suite_add_tcase(s, t);

   t = tcase_create("Basic logging");
   tcase_add_test(t, test_basic_logging);
   tcase_set_timeout(t, 1);
   suite_add_tcase(s, t);

   t = tcase_create("Domain registration");
   tcase_add_test(t, test_domain_registration);
   tcase_add_test(t, test_domain_registration_invalid);
   tcase_set_timeout(t, 1);
   suite_add_tcase(s, t);

   t = tcase_create("Thread management");
   tcase_add_test(t, test_thread_management);
   tcase_add_test(t, test_multi_threaded_logging);
   tcase_set_timeout(t, 2);
   suite_add_tcase(s, t);

   t = tcase_create("Level management");
   tcase_add_test(t, test_level_management);
   tcase_add_test(t, test_domain_level_management);
   tcase_add_test(t, test_domain_level_management_invalid);
   tcase_set_timeout(t, 1);
   suite_add_tcase(s, t);

   t = tcase_create("Print callbacks");
   tcase_add_test(t, test_print_callback_switching);
   tcase_set_timeout(t, 1);
   suite_add_tcase(s, t);

   t = tcase_create("Color functions");
   tcase_add_test(t, test_color_functions);
   tcase_set_timeout(t, 1);
   suite_add_tcase(s, t);

   t = tcase_create("Domain by name");
   tcase_add_test(t, test_domain_by_name);
   tcase_add_test(t, test_domain_by_name_invalid);
   tcase_set_timeout(t, 1);
   suite_add_tcase(s, t);

   t = tcase_create("Comprehensive");
   tcase_add_test(t, test_comprehensive_logging);
   tcase_set_timeout(t, 1);
   suite_add_tcase(s, t);

   // Test suite for output configuration functionality
   t = tcase_create("Output Configuration");
   tcase_add_test(t, test_output_init);
   tcase_add_test(t, test_output_set_stdout);
   tcase_add_test(t, test_output_set_stderr);
   tcase_add_test(t, test_output_set_file);
   tcase_add_test(t, test_output_invalid);
   tcase_add_test(t, test_build_time_config);
   tcase_set_timeout(t, 1);
   suite_add_tcase(s, t);

   srunner_add_suite(runner, s);
}
