// https://github.com/redis/hiredis/issues/55

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "hiredis/hiredis.h"
#include "hiredis/async.h"
#include "hiredis/adapters/libevent.h"

struct event_base *g_base;

unsigned int g_rdb = 0;
int g_keyshift;

static void sig_handler(int signum)
{
	switch(signum) {
		case SIGPIPE:
			fprintf(stderr, "redis went away!\n");
		case SIGHUP:
		case SIGINT:
		case SIGTERM:
		case SIGQUIT:
			event_base_loopexit(g_base, NULL);
			break;
	}
}

#ifdef PRINTALL

static void redisEventPrint(redisAsyncContext *rac, void *reply, void *user_data)
{
	size_t i;
	redisReply *r;

	if(reply == NULL) { return; }

	r = reply;
	if(r->type == REDIS_REPLY_ARRAY) {
		for (i = 0; i < r->elements; i++) {
			printf("%lu) %s\n", i, r->element[i]->str);
		}
	}
}

#else

static void keySet(char *key)
{
	printf("set %s\n", key);
}

static void keyDel(char *key)
{
	printf("del %s\n", key);
}

static void keyExpired(char *key)
{
	printf("exp %s\n", key);
}

static void redisEventHandle(redisAsyncContext *rac, void *reply, void *user_data)
{
	char *cmd, *key;
	redisReply *r;

	if(reply == NULL) { return; }

	r = reply;
	if(r->type == REDIS_REPLY_ARRAY) {
		if(r->elements == 4) {
			cmd = r->element[3]->str;
			key = r->element[2]->str;
			if(strcmp(cmd, "set") == 0)     {     keySet(key+g_keyshift); }
			if(strcmp(cmd, "del") == 0)     {     keyDel(key+g_keyshift); }
			if(strcmp(cmd, "expired") == 0) { keyExpired(key+g_keyshift); }
		}
	}
}

#endif

static void bail(char *s)
{
	fprintf(stderr, "%s: <HOST> <PORT> [DB (0-15)]\n", s);
	exit(1);
}

int main(int argc, char *argv[])
{
	char subscription[64];
	redisAsyncContext *rac;

	if(argc == 4) {
		g_rdb = atoi(argv[3]);
	} else if(argc != 3) { bail(argv[0]); }
	if(g_rdb > 15) { bail(argv[0]); }

	g_keyshift = 15;
	if(g_rdb > 9) { g_keyshift++; }

	rac = redisAsyncConnect(argv[1], atoi(argv[2]));
	if(rac->err) {
		// This doesnt seem to work ...
		fprintf(stderr, "error: %s\n", rac->errstr);
		return 1;
	}

	signal(SIGINT,	sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGQUIT,	sig_handler);
	signal(SIGHUP,	sig_handler);
	//signal(SIGPIPE, SIG_IGN);
	signal(SIGPIPE,	sig_handler);

	g_base = event_base_new();
	redisLibeventAttach(rac, g_base);
	redisAsyncCommand(rac, NULL, NULL, "config set notify-keyspace-events KEA");
	snprintf(subscription, sizeof(subscription), "PSUBSCRIBE __keyspace@%u__:*", g_rdb);
#ifdef PRINTALL
	redisAsyncCommand(rac, redisEventPrint, NULL, subscription);
#else
	redisAsyncCommand(rac, redisEventHandle, NULL, subscription);
#endif
	event_base_dispatch(g_base);

	return 0;
}
