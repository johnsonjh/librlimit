/* Copyright (c) 2019, Michael Santos <michael.santos@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/resource.h>
#include <sys/time.h>

#include <err.h>

#define LRL_RESOURCE(x)                                                        \
  { x, #x }

typedef struct {
  int resource;
  char *name;
} lrl_resource_t;

const lrl_resource_t limits[] = {
#ifdef RLIMIT_AS
    LRL_RESOURCE(RLIMIT_AS),
#endif
#ifdef RLIMIT_CORE
    LRL_RESOURCE(RLIMIT_CORE),
#endif
#ifdef RLIMIT_CPU
    LRL_RESOURCE(RLIMIT_CPU),
#endif
#ifdef RLIMIT_DATA
    LRL_RESOURCE(RLIMIT_DATA),
#endif
#ifdef RLIMIT_FSIZE
    LRL_RESOURCE(RLIMIT_FSIZE),
#endif
#ifdef RLIMIT_LOCKS
    LRL_RESOURCE(RLIMIT_LOCKS),
#endif
#ifdef RLIMIT_MEMLOCK
    LRL_RESOURCE(RLIMIT_MEMLOCK),
#endif
#ifdef RLIMIT_MSGQUEUE
    LRL_RESOURCE(RLIMIT_MSGQUEUE),
#endif
#ifdef RLIMIT_NICE
    LRL_RESOURCE(RLIMIT_NICE),
#endif
#ifdef RLIMIT_NOFILE
    LRL_RESOURCE(RLIMIT_NOFILE),
#endif
#ifdef RLIMIT_NPROC
    LRL_RESOURCE(RLIMIT_NPROC),
#endif
#ifdef RLIMIT_RSS
    LRL_RESOURCE(RLIMIT_RSS),
#endif
#ifdef RLIMIT_RTPRIO
    LRL_RESOURCE(RLIMIT_RTPRIO),
#endif
#ifdef RLIMIT_RTTIME
    LRL_RESOURCE(RLIMIT_RTTIME),
#endif
#ifdef RLIMIT_SIGPENDING
    LRL_RESOURCE(RLIMIT_SIGPENDING),
#endif
#ifdef RLIMIT_STACK
    LRL_RESOURCE(RLIMIT_STACK),
#endif

    {0, NULL}};

void _init(void);

int lrl_setrlimit(int resource, char *rlim);
void lrl_error(char *err, char *msg);

void _init(void) {
  char *name;
  char *env_error;
  const lrl_resource_t *p;

  env_error = getenv("LIBRLIMIT_ERROR");

  for (p = limits; p->name != NULL; p++) {
    name = getenv(p->name);
    if (lrl_setrlimit(p->resource, name) < 0)
      lrl_error(env_error, p->name);
  }
}

int lrl_setrlimit(int resource, char *rlim) {
  struct rlimit rl = {0};
  rlim_t cur = 0;
  char *endptr = NULL;

  if (rlim == NULL)
    return 0;

  errno = 0;
  cur = (rlim_t)strtoul(rlim, &endptr, 10);

  if (errno != 0)
    return -1;

  if (endptr != NULL) {
    errno = EINVAL;
    return -1;
  }

  rl.rlim_cur = cur;
  rl.rlim_max = cur;

  return setrlimit(resource, &rl);
}

void lrl_error(char *err, char *msg) {
  if (!err)
    _exit(111);
  warn("librlimit: %s", msg);
}
