AM_CFLAGS = -Wall -Werror -I../src
bin_PROGRAMS = zdtm_test_daemon zdtm_prepare_message_test
#zdtm_prepare_message_test_LDFLAGS = -L../src/ -lzdtmsync
zdtm_prepare_message_test_SOURCES = zdtm_prepare_message_test.c
#zdtm_test_daemon_LDFLAGS = -L../src/ -lzdtmsync
zdtm_test_daemon_SOURCES = zdtm_test_daemon.c
LDADD = ../src/libzdtmsync.la
