/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2016-2019 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

/**
 * Assorted utilities that don't belong anywhere else.
 */

#ifndef BFS_UTIL_H
#define BFS_UTIL_H

#include <dirent.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <regex.h>
#include <stdbool.h>
#include <sys/types.h>
#include <time.h>

// Some portability concerns

#ifdef __has_feature
#	define BFS_HAS_FEATURE(feature, fallback) __has_feature(feature)
#else
#	define BFS_HAS_FEATURE(feature, fallback) fallback
#endif

#ifdef __has_include
#	define BFS_HAS_INCLUDE(header, fallback) __has_include(header)
#else
#	define BFS_HAS_INCLUDE(header, fallback) fallback
#endif

#ifndef BFS_HAS_MNTENT
#	define BFS_HAS_MNTENT BFS_HAS_INCLUDE(<mntent.h>, __GLIBC__)
#endif

#ifndef BFS_HAS_SYS_ACL
#	define BFS_HAS_SYS_ACL BFS_HAS_INCLUDE(<sys/acl.h>, true)
#endif

#ifndef BFS_HAS_SYS_CAPABILITY
#	define BFS_HAS_SYS_CAPABILITY BFS_HAS_INCLUDE(<sys/capability.h>, __linux__)
#endif

#ifndef BFS_HAS_SYS_MKDEV
#	define BFS_HAS_SYS_MKDEV BFS_HAS_INCLUDE(<sys/mkdev.h>, false)
#endif

#ifndef BFS_HAS_SYS_PARAM
#	define BFS_HAS_SYS_PARAM BFS_HAS_INCLUDE(<sys/param.h>, true)
#endif

#ifndef BFS_HAS_SYS_SYSMACROS
#	define BFS_HAS_SYS_SYSMACROS BFS_HAS_INCLUDE(<sys/sysmacros.h>, __GLIBC__)
#endif

#ifndef BFS_HAS_SYS_XATTR
#	define BFS_HAS_SYS_XATTR BFS_HAS_INCLUDE(<sys/xattr.h>, __linux__)
#endif

#if !defined(FNM_CASEFOLD) && defined(FNM_IGNORECASE)
#	define FNM_CASEFOLD FNM_IGNORECASE
#endif

#ifndef O_DIRECTORY
#	define O_DIRECTORY 0
#endif

/**
 * Adds compiler warnings for bad printf()-style function calls, if supported.
 */
#if __GNUC__
#	define BFS_FORMATTER(fmt, args) __attribute__((format(printf, fmt, args)))
#else
#	define BFS_FORMATTER(fmt, args)
#endif

/**
 * readdir() wrapper that makes error handling cleaner.
 */
int xreaddir(DIR *dir, struct dirent **de);

/**
 * readlinkat() wrapper that dynamically allocates the result.
 *
 * @param fd
 *         The base directory descriptor.
 * @param path
 *         The path to the link, relative to fd.
 * @param size
 *         An estimate for the size of the link name (pass 0 if unknown).
 * @return The target of the link, allocated with malloc(), or NULL on failure.
 */
char *xreadlinkat(int fd, const char *path, size_t size);

/**
 * Check if a file descriptor is open.
 */
bool isopen(int fd);

/**
 * Open a file and redirect it to a particular descriptor.
 *
 * @param fd
 *         The file descriptor to redirect.
 * @param path
 *         The path to open.
 * @param flags
 *         The flags passed to open().
 * @param mode
 *         The mode passed to open() (optional).
 * @return fd on success, -1 on failure.
 */
int redirect(int fd, const char *path, int flags, ...);

/**
 * Like dup(), but set the FD_CLOEXEC flag.
 *
 * @param fd
 *         The file descriptor to duplicate.
 * @return A duplicated file descriptor, or -1 on failure.
 */
int dup_cloexec(int fd);

/**
 * Like pipe(), but set the FD_CLOEXEC flag.
 *
 * @param pipefd
 *         The array to hold the two file descriptors.
 * @return 0 on success, -1 on failure.
 */
int pipe_cloexec(int pipefd[2]);

/**
 * Dynamically allocate a regex error message.
 *
 * @param err
 *         The error code to stringify.
 * @param regex
 *         The (partially) compiled regex.
 * @return A human-readable description of the error, allocated with malloc().
 */
char *xregerror(int err, const regex_t *regex);

/**
 * localtime_r() wrapper that calls tzset() first.
 *
 * @param timep
 *         The time_t to convert.
 * @param result
 *         Buffer to hold the result.
 * @return 0 on success, -1 on failure.
 */
int xlocaltime(const time_t *timep, struct tm *result);

/**
 * Format a mode like ls -l (e.g. -rw-r--r--).
 *
 * @param mode
 *         The mode to format.
 * @param str
 *         The string to hold the formatted mode.
 */
void format_mode(mode_t mode, char str[11]);

/**
 * basename() variant that doesn't modify the input.
 *
 * @param path
 *         The path in question.
 * @return A pointer into path at the base name offset.
 */
const char *xbasename(const char *path);

/**
 * Wrapper for faccessat() that handles some portability issues.
 */
int xfaccessat(int fd, const char *path, int amode);

/**
 * Return whether an error code is due to a path not existing.
 */
bool is_nonexistence_error(int error);

/**
 * Process a yes/no prompt.
 *
 * @return 1 for yes, 0 for no, and -1 for unknown.
 */
int ynprompt(void);

/**
 * Portable version of makedev().
 */
dev_t bfs_makedev(int ma, int mi);

/**
 * Portable version of major().
 */
int bfs_major(dev_t dev);

/**
 * Portable version of minor().
 */
int bfs_minor(dev_t dev);

#endif // BFS_UTIL_H
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2018-2019 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

/**
 * A facade over the stat() API that unifies some details that diverge between
 * implementations, like the names of the timespec fields and the presence of
 * file "birth" times.  On new enough Linux kernels, the facade is backed by
 * statx() instead, and so it exposes a similar interface with a mask for which
 * fields were successfully returned.
 */

#ifndef BFS_STAT_H
#define BFS_STAT_H

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#if BFS_HAS_SYS_PARAM
#	include <sys/param.h>
#endif

/**
 * bfs_stat field bitmask.
 */
enum bfs_stat_field {
	BFS_STAT_DEV    = 1 << 0,
	BFS_STAT_INO    = 1 << 1,
	BFS_STAT_TYPE   = 1 << 2,
	BFS_STAT_MODE   = 1 << 3,
	BFS_STAT_NLINK  = 1 << 4,
	BFS_STAT_GID    = 1 << 5,
	BFS_STAT_UID    = 1 << 6,
	BFS_STAT_SIZE   = 1 << 7,
	BFS_STAT_BLOCKS = 1 << 8,
	BFS_STAT_RDEV   = 1 << 9,
	BFS_STAT_ATIME  = 1 << 10,
	BFS_STAT_BTIME  = 1 << 11,
	BFS_STAT_CTIME  = 1 << 12,
	BFS_STAT_MTIME  = 1 << 13,
};

/**
 * Get the human-readable name of a bfs_stat field.
 */
const char *bfs_stat_field_name(enum bfs_stat_field field);

/**
 * bfs_stat() flags.
 */
enum bfs_stat_flag {
	/** Follow symlinks (the default). */
	BFS_STAT_FOLLOW = 0,
	/** Never follow symlinks. */
	BFS_STAT_NOFOLLOW = 1 << 0,
	/** Try to follow symlinks, but fall back to the link itself if broken. */
	BFS_STAT_TRYFOLLOW = 1 << 1,
};

#ifdef DEV_BSIZE
#	define BFS_STAT_BLKSIZE DEV_BSIZE
#elif defined(S_BLKSIZE)
#	define BFS_STAT_BLKSIZE S_BLKSIZE
#else
#	define BFS_STAT_BLKSIZE 512
#endif

/**
 * Facade over struct stat.
 */
struct bfs_stat {
	/** Bitmask indicating filled fields. */
	enum bfs_stat_field mask;

	/** Device ID containing the file. */
	dev_t dev;
	/** Inode number. */
	ino_t ino;
	/** File type and access mode. */
	mode_t mode;
	/** Number of hard links. */
	nlink_t nlink;
	/** Owner group ID. */
	gid_t gid;
	/** Owner user ID. */
	uid_t uid;
	/** File size in bytes. */
	off_t size;
	/** Number of disk blocks allocated (of size BFS_STAT_BLKSIZE). */
	blkcnt_t blocks;
	/** The device ID represented by this file. */
	dev_t rdev;

	/** Access time. */
	struct timespec atime;
	/** Birth/creation time. */
	struct timespec btime;
	/** Status change time. */
	struct timespec ctime;
	/** Modification time. */
	struct timespec mtime;
};

/**
 * Facade over fstatat().
 *
 * @param at_fd
 *         The base file descriptor for the lookup.
 * @param at_path
 *         The path to stat, relative to at_fd.  Pass NULL to fstat() at_fd
 *         itself.
 * @param flags
 *         Flags that affect the lookup.
 * @param[out] buf
 *         A place to store the stat buffer, if successful.
 * @return
 *         0 on success, -1 on error.
 */
int bfs_stat(int at_fd, const char *at_path, enum bfs_stat_flag flags, struct bfs_stat *buf);

/**
 * Get a particular time field from a bfs_stat() buffer.
 */
const struct timespec *bfs_stat_time(const struct bfs_stat *buf, enum bfs_stat_field field);

/**
 * A unique ID for a file.
 */
typedef unsigned char bfs_file_id[sizeof(dev_t) + sizeof(ino_t)];

/**
 * Compute a unique ID for a file.
 */
void bfs_stat_id(const struct bfs_stat *buf, bfs_file_id *id);

#endif // BFS_STAT_H
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2015-2018 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

/**
 * Utilities for colored output on ANSI terminals.
 */

#ifndef BFS_COLOR_H
#define BFS_COLOR_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

/**
 * A lookup table for colors.
 */
struct colors;

/**
 * Parse a color table.
 *
 * @param ls_colors
 *         A color table in the LS_COLORS environment variable format.
 * @return The parsed color table.
 */
struct colors *parse_colors(const char *ls_colors);

/**
 * Free a color table.
 *
 * @param colors
 *         The color table to free.
 */
void free_colors(struct colors *colors);

/**
 * A file/stream with associated colors.
 */
typedef struct CFILE {
	/** The underlying file/stream. */
	FILE *file;
	/** The color table to use, if any. */
	const struct colors *colors;
	/** Whether to close the underlying stream. */
	bool close;
} CFILE;

/**
 * Open a file for colored output.
 *
 * @param path
 *         The path to the file to open.
 * @param colors
 *         The color table to use if file is a TTY.
 * @return A colored file stream.
 */
CFILE *cfopen(const char *path, const struct colors *colors);

/**
 * Make a colored copy of an open file.
 *
 * @param file
 *         The underlying file.
 * @param colors
 *         The color table to use if file is a TTY.
 * @return A colored wrapper around file.
 */
CFILE *cfdup(FILE *file, const struct colors *colors);

/**
 * Close a colored file.
 *
 * @param cfile
 *         The colored file to close.
 * @return 0 on success, -1 on failure.
 */
int cfclose(CFILE *cfile);

/**
 * Colored, formatted output.
 *
 * @param cfile
 *         The colored stream to print to.
 * @param format
 *         A printf()-style format string, supporting these format specifiers:
 *
 *         %c: A single character
 *         %d: An integer
 *         %g: A double
 *         %s: A string
 *         %zu: A size_t
 *         %m: strerror(errno)
 *         %pP: A colored file path, from a const struct BFTW * argument
 *         %pL: A colored link target, from a const struct BFTW * argument
 *         %%: A literal '%'
 *         ${cc}: Change the color to 'cc'
 *         $$: A literal '$'
 * @return 0 on success, -1 on failure.
 */
BFS_FORMATTER(2, 3)
int cfprintf(CFILE *cfile, const char *format, ...);

/**
 * cfprintf() variant that takes a va_list.
 */
int cvfprintf(CFILE *cfile, const char *format, va_list args);

#endif // BFS_COLOR_H
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2019 Tavian Barnes <tavianator@tavianator.com>             *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

#ifndef BFS_TRIE_H
#define BFS_TRIE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * A trie that holds a set of fixed- or variable-length strings.
 */
struct trie {
	uintptr_t root;
};

/**
 * A leaf of a trie.
 */
struct trie_leaf {
	/**
	 * An arbitrary value associated with this leaf.
	 */
	void *value;

	/**
	 * The length of the key in bytes.
	 */
	size_t length;

	/**
	 * The key itself, stored inline.
	 */
	char key[];
};

/**
 * Initialize an empty trie.
 */
void trie_init(struct trie *trie);

/**
 * Get the first (lexicographically earliest) leaf in the trie.
 *
 * @param trie
 *         The trie to search.
 * @return
 *         The first leaf, or NULL if the trie is empty.
 */
struct trie_leaf *trie_first_leaf(const struct trie *trie);

/**
 * Find the leaf for a string key.
 *
 * @param trie
 *         The trie to search.
 * @param key
 *         The key to look up.
 * @return
 *         The found leaf, or NULL if the key is not present.
 */
struct trie_leaf *trie_find_str(const struct trie *trie, const char *key);

/**
 * Find the leaf for a fixed-size key.
 *
 * @param trie
 *         The trie to search.
 * @param key
 *         The key to look up.
 * @param length
 *         The length of the key in bytes.
 * @return
 *         The found leaf, or NULL if the key is not present.
 */
struct trie_leaf *trie_find_mem(const struct trie *trie, const void *key, size_t length);

/**
 * Find the shortest leaf that starts with a given key.
 *
 * @param trie
 *         The trie to search.
 * @param key
 *         The key to look up.
 * @return
 *         A leaf that starts with the given key, or NULL.
 */
struct trie_leaf *trie_find_postfix(const struct trie *trie, const char *key);

/**
 * Find the leaf that is the longest prefix of the given key.
 *
 * @param trie
 *         The trie to search.
 * @param key
 *         The key to look up.
 * @return
 *         The longest prefix match for the given key, or NULL.
 */
struct trie_leaf *trie_find_prefix(const struct trie *trie, const char *key);

/**
 * Insert a string key into the trie.
 *
 * @param trie
 *         The trie to modify.
 * @param key
 *         The key to insert.
 * @return
 *         The inserted leaf, or NULL on failure.
 */
struct trie_leaf *trie_insert_str(struct trie *trie, const char *key);

/**
 * Insert a fixed-size key into the trie.
 *
 * @param trie
 *         The trie to modify.
 * @param key
 *         The key to insert.
 * @param length
 *         The length of the key in bytes.
 * @return
 *         The inserted leaf, or NULL on failure.
 */
struct trie_leaf *trie_insert_mem(struct trie *trie, const void *key, size_t length);

/**
 * Remove a leaf from a trie.
 *
 * @param trie
 *         The trie to modify.
 * @param leaf
 *         The leaf to remove.
 */
void trie_remove(struct trie *trie, struct trie_leaf *leaf);

/**
 * Destroy a trie and its contents.
 */
void trie_destroy(struct trie *trie);

#endif // BFS_TRIE_H
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2015-2018 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

/**
 * The expression tree representation.
 */

#ifndef BFS_EXPR_H
#define BFS_EXPR_H

#include <regex.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include <time.h>

/**
 * A command line expression.
 */
struct expr;

/**
 * Ephemeral state for evaluating an expression.
 */
struct eval_state;

/**
 * Expression evaluation function.
 *
 * @param expr
 *         The current expression.
 * @param state
 *         The current evaluation state.
 * @return
 *         The result of the test.
 */
typedef bool eval_fn(const struct expr *expr, struct eval_state *state);

/**
 * Possible types of numeric comparison.
 */
enum cmp_flag {
	/** Exactly n. */
	CMP_EXACT,
	/** Less than n. */
	CMP_LESS,
	/** Greater than n. */
	CMP_GREATER,
};

/**
 * Possible types of mode comparison.
 */
enum mode_cmp {
	/** Mode is an exact match (MODE). */
	MODE_EXACT,
	/** Mode has all these bits (-MODE). */
	MODE_ALL,
	/** Mode has any of these bits (/MODE). */
	MODE_ANY,
};

/**
 * Possible time units.
 */
enum time_unit {
	/** Minutes. */
	MINUTES,
	/** Days. */
	DAYS,
};

/**
 * Possible file size units.
 */
enum size_unit {
	/** 512-byte blocks. */
	SIZE_BLOCKS,
	/** Single bytes. */
	SIZE_BYTES,
	/** Two-byte words. */
	SIZE_WORDS,
	/** Kibibytes. */
	SIZE_KB,
	/** Mebibytes. */
	SIZE_MB,
	/** Gibibytes. */
	SIZE_GB,
	/** Tebibytes. */
	SIZE_TB,
	/** Pebibytes. */
	SIZE_PB,
};

struct expr {
	/** The function that evaluates this expression. */
	eval_fn *eval;

	/** The left hand side of the expression. */
	struct expr *lhs;
	/** The right hand side of the expression. */
	struct expr *rhs;

	/** Whether this expression has no side effects. */
	bool pure;
	/** Whether this expression always evaluates to true. */
	bool always_true;
	/** Whether this expression always evaluates to false. */
	bool always_false;

	/** Estimated cost. */
	double cost;
	/** Estimated probability of success. */
	double probability;
	/** Number of times this predicate was executed. */
	size_t evaluations;
	/** Number of times this predicate succeeded. */
	size_t successes;
	/** Total time spent running this predicate. */
	struct timespec elapsed;

	/** The number of command line arguments for this expression. */
	size_t argc;
	/** The command line arguments comprising this expression. */
	char **argv;

	/** The optional comparison flag. */
	enum cmp_flag cmp_flag;

	/** The mode comparison flag. */
	enum mode_cmp mode_cmp;
	/** Mode to use for files. */
	mode_t file_mode;
	/** Mode to use for directories (different due to X). */
	mode_t dir_mode;

	/** The optional stat field to look at. */
	enum bfs_stat_field stat_field;
	/** The optional reference time. */
	struct timespec reftime;
	/** The optional time unit. */
	enum time_unit time_unit;

	/** The optional size unit. */
	enum size_unit size_unit;

	/** Optional device number for a target file. */
	dev_t dev;
	/** Optional inode number for a target file. */
	ino_t ino;

	/** File to output to. */
	CFILE *cfile;

	/** Optional compiled regex. */
	regex_t *regex;

	/** Optional exec command. */
	struct bfs_exec *execbuf;

	/** Optional printf command. */
	struct bfs_printf *printf;

	/** Optional integer data for this expression. */
	long long idata;

	/** Optional string data for this expression. */
	const char *sdata;

	/** The number of files this expression keeps open between evaluations. */
	int persistent_fds;
	/** The number of files this expression opens during evaluation. */
	int ephemeral_fds;
};

/** Singleton true expression instance. */
extern struct expr expr_true;
/** Singleton false expression instance. */
extern struct expr expr_false;

/**
 * Create a new expression.
 */
struct expr *new_expr(eval_fn *eval, size_t argc, char **argv);

/**
 * @return Whether expr is known to always quit.
 */
bool expr_never_returns(const struct expr *expr);

/**
 * @return The result of the comparison for this expression.
 */
bool expr_cmp(const struct expr *expr, long long n);

/**
 * Dump a parsed expression.
 */
void dump_expr(CFILE *cfile, const struct expr *expr, bool verbose);

/**
 * Free an expression tree.
 */
void free_expr(struct expr *expr);

#endif // BFS_EXPR_H
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2015-2019 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

/**
 * Constants about the bfs program itself.
 */

#ifndef BFS_H
#define BFS_H

#ifndef BFS_VERSION
#	define BFS_VERSION "1.5"
#endif

#ifndef BFS_HOMEPAGE
#	define BFS_HOMEPAGE "https://github.com/tavianator/bfs"
#endif

#endif // BFS_H
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2015-2019 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

/**
 * A file-walking API based on nftw().
 */

#ifndef BFS_BFTW_H
#define BFS_BFTW_H

#include <stddef.h>
#include <sys/types.h>

/**
 * Possible file types.
 */
enum bftw_typeflag {
	/** Unknown type. */
	BFTW_UNKNOWN = 0,
	/** Block device. */
	BFTW_BLK     = 1 << 0,
	/** Character device. */
	BFTW_CHR     = 1 << 1,
	/** Directory. */
	BFTW_DIR     = 1 << 2,
	/** Solaris door. */
	BFTW_DOOR    = 1 << 3,
	/** Pipe. */
	BFTW_FIFO    = 1 << 4,
	/** Symbolic link. */
	BFTW_LNK     = 1 << 5,
	/** Solaris event port. */
	BFTW_PORT    = 1 << 6,
	/** Regular file. */
	BFTW_REG     = 1 << 7,
	/** Socket. */
	BFTW_SOCK    = 1 << 8,
	/** BSD whiteout. */
	BFTW_WHT     = 1 << 9,
	/** An error occurred for this file. */
	BFTW_ERROR   = 1 << 10,
};

/**
 * Convert a bfs_stat() mode to a bftw() typeflag.
 */
enum bftw_typeflag bftw_mode_typeflag(mode_t mode);

/**
 * Possible visit occurrences.
 */
enum bftw_visit {
	/** Pre-order visit. */
	BFTW_PRE,
	/** Post-order visit. */
	BFTW_POST,
};

/**
 * Cached bfs_stat() info for a file.
 */
struct bftw_stat {
	/** A pointer to the bfs_stat() buffer, if available. */
	const struct bfs_stat *buf;
	/** Storage for the bfs_stat() buffer, if needed. */
	struct bfs_stat storage;
	/** The cached error code, if any. */
	int error;
};

/**
 * Data about the current file for the bftw() callback.
 */
struct BFTW {
	/** The path to the file. */
	const char *path;
	/** The string offset of the filename. */
	size_t nameoff;

	/** The root path passed to bftw(). */
	const char *root;
	/** The depth of this file in the traversal. */
	size_t depth;
	/** Which visit this is. */
	enum bftw_visit visit;

	/** The file type. */
	enum bftw_typeflag typeflag;
	/** The errno that occurred, if typeflag == BFTW_ERROR. */
	int error;

	/** A parent file descriptor for the *at() family of calls. */
	int at_fd;
	/** The path relative to at_fd for the *at() family of calls. */
	const char *at_path;

	/** Flags for bfs_stat(). */
	enum bfs_stat_flag stat_flags;
	/** Cached bfs_stat() info for BFS_STAT_NOFOLLOW. */
	struct bftw_stat lstat_cache;
	/** Cached bfs_stat() info for BFS_STAT_FOLLOW. */
	struct bftw_stat stat_cache;
};

/**
 * Get bfs_stat() info for a file encountered during bftw(), caching the result
 * whenever possible.
 *
 * @param ftwbuf
 *         bftw() data for the file to stat.
 * @param flags
 *         flags for bfs_stat().  Pass ftwbuf->stat_flags for the default flags.
 * @return
 *         A pointer to a bfs_stat() buffer, or NULL if the call failed.
 */
const struct bfs_stat *bftw_stat(const struct BFTW *ftwbuf, enum bfs_stat_flag flags);

/**
 * Get the type of a file encountered during bftw(), with flags controlling
 * whether to follow links.  This function will avoid calling bfs_stat() if
 * possible.
 *
 * @param ftwbuf
 *         bftw() data for the file to check.
 * @param flags
 *         flags for bfs_stat().  Pass ftwbuf->stat_flags for the default flags.
 * @return
 *         The type of the file, or BFTW_ERROR if an error occurred.
 */
enum bftw_typeflag bftw_typeflag(const struct BFTW *ftwbuf, enum bfs_stat_flag flags);

/**
 * Walk actions returned by the bftw() callback.
 */
enum bftw_action {
	/** Keep walking. */
	BFTW_CONTINUE,
	/** Skip this path's children. */
	BFTW_PRUNE,
	/** Stop walking. */
	BFTW_STOP,
};

/**
 * Callback function type for bftw().
 *
 * @param ftwbuf
 *         Data about the current file.
 * @param ptr
 *         The pointer passed to bftw().
 * @return
 *         An action value.
 */
typedef enum bftw_action bftw_callback(const struct BFTW *ftwbuf, void *ptr);

/**
 * Flags that control bftw() behavior.
 */
enum bftw_flags {
	/** stat() each encountered file. */
	BFTW_STAT          = 1 << 0,
	/** Attempt to recover from encountered errors. */
	BFTW_RECOVER       = 1 << 1,
	/** Visit directories in post-order as well as pre-order. */
	BFTW_DEPTH         = 1 << 2,
	/** If the initial path is a symbolic link, follow it. */
	BFTW_COMFOLLOW     = 1 << 3,
	/** Follow all symbolic links. */
	BFTW_LOGICAL       = 1 << 4,
	/** Detect directory cycles. */
	BFTW_DETECT_CYCLES = 1 << 5,
	/** Stay on the same filesystem. */
	BFTW_XDEV          = 1 << 6,
};

/**
 * Tree search strategies for bftw().
 */
enum bftw_strategy {
	/** Breadth-first search. */
	BFTW_BFS,
	/** Depth-first search. */
	BFTW_DFS,
	/** Iterative deepening search. */
	BFTW_IDS,
};

/**
 * Structure for holding the arguments passed to bftw().
 */
struct bftw_args {
	/** The path(s) to start from. */
	const char **paths;
	/** The number of starting paths. */
	size_t npaths;
	/** The callback to invoke. */
	bftw_callback *callback;
	/** A pointer which is passed to the callback. */
	void *ptr;
	/** The maximum number of file descriptors to keep open. */
	int nopenfd;
	/** Flags that control bftw() behaviour. */
	enum bftw_flags flags;
	/** The search strategy to use. */
	enum bftw_strategy strategy;
	/** The parsed mount table, if available. */
	const struct bfs_mtab *mtab;
};

/**
 * Breadth First Tree Walk (or Better File Tree Walk).
 *
 * Like ftw(3) and nftw(3), this function walks a directory tree recursively,
 * and invokes a callback for each path it encounters.
 *
 * @param args
 *         The arguments that control the walk.
 * @return
 *         0 on success, or -1 on failure.
 */
int bftw(const struct bftw_args *args);

#endif // BFS_BFTW_H
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2015-2018 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

/**
 * Representation of the parsed command line.
 */

#ifndef BFS_CMDLINE_H
#define BFS_CMDLINE_H


/**
 * Various debugging flags.
 */
enum debug_flags {
	/** Print cost estimates. */
	DEBUG_COST   = 1 << 0,
	/** Print executed command details. */
	DEBUG_EXEC   = 1 << 1,
	/** Print optimization details. */
	DEBUG_OPT    = 1 << 2,
	/** Print rate information. */
	DEBUG_RATES  = 1 << 3,
	/** Trace the filesystem traversal. */
	DEBUG_SEARCH = 1 << 4,
	/** Trace all stat() calls. */
	DEBUG_STAT   = 1 << 5,
	/** Print the parse tree. */
	DEBUG_TREE   = 1 << 6,
	/** All debug flags. */
	DEBUG_ALL    = (1 << 7) - 1,
};

/**
 * The parsed command line.
 */
struct cmdline {
	/** The unparsed command line arguments. */
	char **argv;

	/** The root paths. */
	const char **paths;
	/** The number of root paths. */
	size_t npaths;

	/** Color data. */
	struct colors *colors;
	/** Colored stdout. */
	CFILE *cout;
	/** Colored stderr. */
	CFILE *cerr;

	/** Table of mounted file systems. */
	struct bfs_mtab *mtab;
	/** The error that occurred parsing the mount table, if any. */
	int mtab_error;

	/** -mindepth option. */
	int mindepth;
	/** -maxdepth option. */
	int maxdepth;

	/** bftw() flags. */
	enum bftw_flags flags;
	/** bftw() search strategy. */
	enum bftw_strategy strategy;

	/** Optimization level. */
	int optlevel;
	/** Debugging flags. */
	enum debug_flags debug;
	/** Whether to only handle paths with xargs-safe characters. */
	bool xargs_safe;
	/** Whether to ignore deletions that race with bfs. */
	bool ignore_races;
	/** Whether to only return unique files. */
	bool unique;

	/** The command line expression. */
	struct expr *expr;

	/** All the open files owned by the command line. */
	struct trie open_files;
	/** The number of open files owned by the command line. */
	int nopen_files;
};

/**
 * Parse the command line.
 */
struct cmdline *parse_cmdline(int argc, char *argv[]);

/**
 * Dump the parsed command line.
 */
void dump_cmdline(const struct cmdline *cmdline, bool verbose);

/**
 * Optimize the parsed command line.
 *
 * @return 0 if successful, -1 on error.
 */
int optimize_cmdline(struct cmdline *cmdline);

/**
 * Evaluate the command line.
 */
int eval_cmdline(const struct cmdline *cmdline);

/**
 * Free the parsed command line.
 *
 * @return 0 if successful, -1 on error.
 */
int free_cmdline(struct cmdline *cmdline);

#endif // BFS_CMDLINE_H
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2019 Tavian Barnes <tavianator@tavianator.com>             *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

/**
 * Formatters for diagnostic messages.
 */

#ifndef BFS_DIAG_H
#define BFS_DIAG_H

#include <stdarg.h>

/**
 * Shorthand for printing error messages.
 */
BFS_FORMATTER(2, 3)
void bfs_error(const struct cmdline *cmdline, const char *format, ...);

/**
 * Shorthand for printing warning messages.
 */
BFS_FORMATTER(2, 3)
void bfs_warning(const struct cmdline *cmdline, const char *format, ...);

/**
 * bfs_error() variant that takes a va_list.
 */
void bfs_verror(const struct cmdline *cmdline, const char *format, va_list args);

/**
 * bfs_warning() variant that takes a va_list.
 */
void bfs_vwarning(const struct cmdline *cmdline, const char *format, va_list args);

/**
 * Print the error message prefix.
 */
void bfs_error_prefix(const struct cmdline *cmdline);

/**
 * Print the warning message prefix.
 */
void bfs_warning_prefix(const struct cmdline *cmdline);

#endif // BFS_DIAG_H
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2016-2019 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

/**
 * A dynamic string library.
 */

#ifndef BFS_DSTRING_H
#define BFS_DSTRING_H

#include <stddef.h>

/**
 * Allocate a dynamic string.
 *
 * @param capacity
 *         The initial capacity of the string.
 */
char *dstralloc(size_t capacity);

/**
 * Create a dynamic copy of a string.
 *
 * @param str
 *         The NUL-terminated string to copy.
 */
char *dstrdup(const char *str);

/**
 * Get a dynamic string's length.
 *
 * @param dstr
 *         The string to measure.
 * @return The length of dstr.
 */
size_t dstrlen(const char *dstr);

/**
 * Reserve some capacity in a dynamic string.
 *
 * @param dstr
 *         The dynamic string to preallocate.
 * @param capacity
 *         The new capacity for the string.
 * @return 0 on success, -1 on failure.
 */
int dstreserve(char **dstr, size_t capacity);

/**
 * Resize a dynamic string.
 *
 * @param dstr
 *         The dynamic string to resize.
 * @param length
 *         The new length for the dynamic string.
 * @return 0 on success, -1 on failure.
 */
int dstresize(char **dstr, size_t length);

/**
 * Append to a dynamic string.
 *
 * @param dest
 *         The destination dynamic string.
 * @param src
 *         The string to append.
 * @return 0 on success, -1 on failure.
 */
int dstrcat(char **dest, const char *src);

/**
 * Append to a dynamic string.
 *
 * @param dest
 *         The destination dynamic string.
 * @param src
 *         The string to append.
 * @param n
 *         The maximum number of characters to take from src.
 * @return 0 on success, -1 on failure.
 */
int dstrncat(char **dest, const char *src, size_t n);

/**
 * Append a single character to a dynamic string.
 *
 * @param str
 *         The string to append to.
 * @param c
 *         The character to append.
 * @return 0 on success, -1 on failure.
 */
int dstrapp(char **str, char c);

/**
 * Create a dynamic string from a format string.
 *
 * @param format
 *         The format string to fill in.
 * @param ...
 *         Any arguments for the format string.
 * @return
 *         The created string, or NULL on failure.
 */
BFS_FORMATTER(1, 2)
char *dstrprintf(const char *format, ...);

/**
 * Free a dynamic string.
 *
 * @param dstr
 *         The string to free.
 */
void dstrfree(char *dstr);

#endif // BFS_DSTRING_H
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2015-2018 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

/**
 * The evaluation functions that implement literal expressions like -name,
 * -print, etc.
 */

#ifndef BFS_EVAL_H
#define BFS_EVAL_H


// Predicate evaluation functions
bool eval_true(const struct expr *expr, struct eval_state *state);
bool eval_false(const struct expr *expr, struct eval_state *state);

bool eval_access(const struct expr *expr, struct eval_state *state);
bool eval_acl(const struct expr *expr, struct eval_state *state);
bool eval_capable(const struct expr *expr, struct eval_state *state);
bool eval_perm(const struct expr *expr, struct eval_state *state);
bool eval_xattr(const struct expr *expr, struct eval_state *state);

bool eval_newer(const struct expr *expr, struct eval_state *state);
bool eval_time(const struct expr *expr, struct eval_state *state);
bool eval_used(const struct expr *expr, struct eval_state *state);

bool eval_gid(const struct expr *expr, struct eval_state *state);
bool eval_uid(const struct expr *expr, struct eval_state *state);
bool eval_nogroup(const struct expr *expr, struct eval_state *state);
bool eval_nouser(const struct expr *expr, struct eval_state *state);

bool eval_depth(const struct expr *expr, struct eval_state *state);
bool eval_empty(const struct expr *expr, struct eval_state *state);
bool eval_fstype(const struct expr *expr, struct eval_state *state);
bool eval_hidden(const struct expr *expr, struct eval_state *state);
bool eval_inum(const struct expr *expr, struct eval_state *state);
bool eval_links(const struct expr *expr, struct eval_state *state);
bool eval_samefile(const struct expr *expr, struct eval_state *state);
bool eval_size(const struct expr *expr, struct eval_state *state);
bool eval_sparse(const struct expr *expr, struct eval_state *state);
bool eval_type(const struct expr *expr, struct eval_state *state);
bool eval_xtype(const struct expr *expr, struct eval_state *state);

bool eval_lname(const struct expr *expr, struct eval_state *state);
bool eval_name(const struct expr *expr, struct eval_state *state);
bool eval_path(const struct expr *expr, struct eval_state *state);
bool eval_regex(const struct expr *expr, struct eval_state *state);

bool eval_delete(const struct expr *expr, struct eval_state *state);
bool eval_exec(const struct expr *expr, struct eval_state *state);
bool eval_exit(const struct expr *expr, struct eval_state *state);
bool eval_nohidden(const struct expr *expr, struct eval_state *state);
bool eval_fls(const struct expr *expr, struct eval_state *state);
bool eval_fprint(const struct expr *expr, struct eval_state *state);
bool eval_fprint0(const struct expr *expr, struct eval_state *state);
bool eval_fprintf(const struct expr *expr, struct eval_state *state);
bool eval_fprintx(const struct expr *expr, struct eval_state *state);
bool eval_prune(const struct expr *expr, struct eval_state *state);
bool eval_quit(const struct expr *expr, struct eval_state *state);

// Operator evaluation functions
bool eval_not(const struct expr *expr, struct eval_state *state);
bool eval_and(const struct expr *expr, struct eval_state *state);
bool eval_or(const struct expr *expr, struct eval_state *state);
bool eval_comma(const struct expr *expr, struct eval_state *state);

#endif // BFS_EVAL_H
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2017 Tavian Barnes <tavianator@tavianator.com>             *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

/**
 * Implementation of -exec/-execdir/-ok/-okdir.
 */

#ifndef BFS_EXEC_H
#define BFS_EXEC_H


struct cmdline;

/**
 * Flags for the -exec actions.
 */
enum bfs_exec_flags {
	/** Prompt the user before executing (-ok, -okdir). */
	BFS_EXEC_CONFIRM = 1 << 0,
	/** Run the command in the file's parent directory (-execdir, -okdir). */
	BFS_EXEC_CHDIR   = 1 << 1,
	/** Pass multiple files at once to the command (-exec ... {} +). */
	BFS_EXEC_MULTI   = 1 << 2,
	/** Print debugging information (-D exec). */
	BFS_EXEC_DEBUG   = 1 << 3,
};

/**
 * Buffer for a command line to be executed.
 */
struct bfs_exec {
	/** Flags for this exec buffer. */
	enum bfs_exec_flags flags;

	/** Command line template. */
	char **tmpl_argv;
	/** Command line template size. */
	size_t tmpl_argc;

	/** The built command line. */
	char **argv;
	/** Number of command line arguments. */
	size_t argc;
	/** Capacity of argv. */
	size_t argv_cap;

	/** Current size of all arguments. */
	size_t arg_size;
	/** Maximum arg_size before E2BIG. */
	size_t arg_max;

	/** A file descriptor for the working directory, for BFS_EXEC_CHDIR. */
	int wd_fd;
	/** The path to the working directory, for BFS_EXEC_CHDIR. */
	char *wd_path;
	/** Length of the working directory path. */
	size_t wd_len;

	/** The ultimate return value for bfs_exec_finish(). */
	int ret;
};

/**
 * Parse an exec action.
 *
 * @param argv
 *         The (bfs) command line argument to parse.
 * @param flags
 *         Any flags for this exec action.
 * @param cmdline
 *         The command line.
 * @return The parsed exec action, or NULL on failure.
 */
struct bfs_exec *parse_bfs_exec(char **argv, enum bfs_exec_flags flags, const struct cmdline *cmdline);

/**
 * Execute the command for a file.
 *
 * @param execbuf
 *         The parsed exec action.
 * @param ftwbuf
 *         The bftw() data for the current file.
 * @return 0 if the command succeeded, -1 if it failed.  If the command could
 *         be executed, -1 is returned, and errno will be non-zero.  For
 *         BFS_EXEC_MULTI, errors will not be reported until bfs_exec_finish().
 */
int bfs_exec(struct bfs_exec *execbuf, const struct BFTW *ftwbuf);

/**
 * Finish executing any commands.
 *
 * @param execbuf
 *         The parsed exec action.
 * @return 0 on success, -1 if any errors were encountered.
 */
int bfs_exec_finish(struct bfs_exec *execbuf);

/**
 * Free a parsed exec action.
 */
void free_bfs_exec(struct bfs_exec *execbuf);

#endif // BFS_EXEC_H
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2019 Tavian Barnes <tavianator@tavianator.com>             *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

/**
 * A facade over (file)system features that are (un)implemented differently
 * between platforms.
 */

#ifndef BFS_FSADE_H
#define BFS_FSADE_H

#include <stdbool.h>

#define BFS_CAN_CHECK_ACL BFS_HAS_SYS_ACL

#if !defined(BFS_CAN_CHECK_CAPABILITIES) && BFS_HAS_SYS_CAPABILITY && !__FreeBSD__
#	include <sys/capability.h>
#	ifdef CAP_CHOWN
#		define BFS_CAN_CHECK_CAPABILITIES true
#	endif
#endif

#define BFS_CAN_CHECK_XATTRS BFS_HAS_SYS_XATTR

/**
 * Check if a file has a non-trvial Access Control List.
 *
 * @param ftwbuf
 *         The file to check.
 * @return
 *         1 if it does, 0 if it doesn't, or -1 if an error occurred.
 */
int bfs_check_acl(const struct BFTW *ftwbuf);

/**
 * Check if a file has a non-trvial capability set.
 *
 * @param ftwbuf
 *         The file to check.
 * @return
 *         1 if it does, 0 if it doesn't, or -1 if an error occurred.
 */
int bfs_check_capabilities(const struct BFTW *ftwbuf);

/**
 * Check if a file has any extended attributes set.
 *
 * @param ftwbuf
 *         The file to check.
 * @return
 *         1 if it does, 0 if it doesn't, or -1 if an error occurred.
 */
int bfs_check_xattrs(const struct BFTW *ftwbuf);

#endif // BFS_FSADE_H
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2017-2019 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

/**
 * A facade over platform-specific APIs for enumerating mounted filesystems.
 */

#ifndef BFS_MTAB_H
#define BFS_MTAB_H

#include <stdbool.h>

/**
 * A file system mount table.
 */
struct bfs_mtab;

/**
 * Parse the mount table.
 *
 * @return
 *         The parsed mount table, or NULL on error.
 */
struct bfs_mtab *parse_bfs_mtab(void);

/**
 * Determine the file system type that a file is on.
 *
 * @param mtab
 *         The current mount table.
 * @param statbuf
 *         The bfs_stat() buffer for the file in question.
 * @return
 *         The type of file system containing this file, "unknown" if not known,
 *         or NULL on error.
 */
const char *bfs_fstype(const struct bfs_mtab *mtab, const struct bfs_stat *statbuf);

/**
 * Check if a file could be a mount point.
 *
 * @param mtab
 *         The current mount table.
 * @param path
 *         The path to check.
 * @return
 *         Whether the named file could be a mount point.
 */
bool bfs_maybe_mount(const struct bfs_mtab *mtab, const char *path);

/**
 * Free a mount table.
 */
void free_bfs_mtab(struct bfs_mtab *mtab);

#endif // BFS_MTAB_H
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2017-2019 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

/**
 * Implementation of -printf/-fprintf.
 */

#ifndef BFS_PRINTF_H
#define BFS_PRINTF_H

#include <stdbool.h>
#include <stdio.h>

/**
 * A printf command, the result of parsing a single format string.
 */
struct bfs_printf;

/**
 * Parse a -printf format string.
 *
 * @param format
 *         The format string to parse.
 * @param cmdline
 *         The command line.
 * @return The parsed printf command, or NULL on failure.
 */
struct bfs_printf *parse_bfs_printf(const char *format, struct cmdline *cmdline);

/**
 * Evaluate a parsed format string.
 *
 * @param file
 *         The FILE to print to.
 * @param command
 *         The parsed printf format.
 * @param ftwbuf
 *         The bftw() data for the current file.  If needs_stat is true, statbuf
 *         must be non-NULL.
 * @return 0 on success, -1 on failure.
 */
int bfs_printf(FILE *file, const struct bfs_printf *command, const struct BFTW *ftwbuf);

/**
 * Free a parsed format string.
 */
void free_bfs_printf(struct bfs_printf *command);

#endif // BFS_PRINTF_H
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2018-2019 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

/**
 * A process-spawning library inspired by posix_spawn().
 */

#ifndef BFS_SPAWN_H
#define BFS_SPAWN_H

#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>

/**
 * bfs_spawn() flags.
 */
enum bfs_spawn_flags {
	/** Use the PATH variable to resolve the executable (like execvp()). */
	BFS_SPAWN_USEPATH = 1 << 0,
};

/**
 * bfs_spawn() attributes, controlling the context of the new process.
 */
struct bfs_spawn {
	enum bfs_spawn_flags flags;
	struct bfs_spawn_action *actions;
	struct bfs_spawn_action **tail;
};

/**
 * Create a new bfs_spawn() context.
 *
 * @return 0 on success, -1 on failure.
 */
int bfs_spawn_init(struct bfs_spawn *ctx);

/**
 * Destroy a bfs_spawn() context.
 *
 * @return 0 on success, -1 on failure.
 */
int bfs_spawn_destroy(struct bfs_spawn *ctx);

/**
 * Set the flags for a bfs_spawn() context.
 *
 * @return 0 on success, -1 on failure.
 */
int bfs_spawn_setflags(struct bfs_spawn *ctx, enum bfs_spawn_flags flags);

/**
 * Add a close() action to a bfs_spawn() context.
 *
 * @return 0 on success, -1 on failure.
 */
int bfs_spawn_addclose(struct bfs_spawn *ctx, int fd);

/**
 * Add a dup2() action to a bfs_spawn() context.
 *
 * @return 0 on success, -1 on failure.
 */
int bfs_spawn_adddup2(struct bfs_spawn *ctx, int oldfd, int newfd);

/**
 * Add an fchdir() action to a bfs_spawn() context.
 *
 * @return 0 on success, -1 on failure.
 */
int bfs_spawn_addfchdir(struct bfs_spawn *ctx, int fd);

/**
 * Spawn a new process.
 *
 * @param exe
 *         The executable to run.
 * @param ctx
 *         The context for the new process.
 * @param argv
 *         The arguments for the new process.
 * @param envp
 *         The environment variables for the new process.
 * @return
 *         The PID of the new process, or -1 on error.
 */
pid_t bfs_spawn(const char *exe, const struct bfs_spawn *ctx, char **argv, char **envp);

#endif // BFS_SPAWN_H
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2016 Tavian Barnes <tavianator@tavianator.com>             *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

#ifndef BFS_TYPO_H
#define BFS_TYPO_H

/**
 * Find the "typo" distance between two strings.
 *
 * @param actual
 *         The actual string typed by the user.
 * @param expected
 *         The expected valid string.
 * @return The distance between the two strings.
 */
int typo_distance(const char *actual, const char *expected);

#endif // BFS_TYPO_H
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2015-2019 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

/**
 * The bftw() implementation consists of the following components:
 *
 * - struct bftw_file: A file that has been encountered during the traversal.
 *   They have reference-counted links to their parents in the directory tree.
 *
 * - struct bftw_cache: Holds bftw_file's with open file descriptors, used for
 *   openat() to minimize the amount of path re-traversals that need to happen.
 *   Currently implemented as a priority queue based on depth and reference
 *   count.
 *
 * - struct bftw_queue: The queue of bftw_file's left to explore.  Implemented
 *   as a simple circular buffer.
 *
 * - struct bftw_reader: A reader object that simplifies reading directories and
 *   reporting errors.
 *
 * - struct bftw_state: Represents the current state of the traversal, allowing
 *   various helper functions to take fewer parameters.
 */

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * A file.
 */
struct bftw_file {
	/** The parent directory, if any. */
	struct bftw_file *parent;
	/** This file's depth in the walk. */
	size_t depth;
	/** The root path this file was found from. */
	const char *root;

	/** The next file in the queue, if any. */
	struct bftw_file *next;

	/** Reference count. */
	size_t refcount;
	/** Index in the bftw_cache priority queue. */
	size_t heap_index;

	/** An open descriptor to this file, or -1. */
	int fd;

	/** This file's type, if known. */
	enum bftw_typeflag typeflag;
	/** The device number, for cycle detection. */
	dev_t dev;
	/** The inode number, for cycle detection. */
	ino_t ino;

	/** The offset of this file in the full path. */
	size_t nameoff;
	/** The length of the file's name. */
	size_t namelen;
	/** The file's name. */
	char name[];
};

/**
 * A cache of open directories.
 */
struct bftw_cache {
	/** A min-heap of open directories. */
	struct bftw_file **heap;
	/** Current heap size. */
	size_t size;
	/** Maximum heap size. */
	size_t capacity;
};

/** Initialize a cache. */
static int bftw_cache_init(struct bftw_cache *cache, size_t capacity) {
	cache->heap = malloc(capacity*sizeof(*cache->heap));
	if (!cache->heap) {
		return -1;
	}

	cache->size = 0;
	cache->capacity = capacity;
	return 0;
}

/** Destroy a cache. */
static void bftw_cache_destroy(struct bftw_cache *cache) {
	assert(cache->size == 0);
	free(cache->heap);
}

/** Check if two heap entries are in heap order. */
static bool bftw_heap_check(const struct bftw_file *parent, const struct bftw_file *child) {
	if (parent->depth > child->depth) {
		return true;
	} else if (parent->depth < child->depth) {
		return false;
	} else {
		return parent->refcount <= child->refcount;
	}
}

/** Move a bftw_file to a particular place in the heap. */
static void bftw_heap_move(struct bftw_cache *cache, struct bftw_file *file, size_t i) {
	cache->heap[i] = file;
	file->heap_index = i;
}

/** Bubble an entry up the heap. */
static void bftw_heap_bubble_up(struct bftw_cache *cache, struct bftw_file *file) {
	size_t i = file->heap_index;

	while (i > 0) {
		size_t pi = (i - 1)/2;
		struct bftw_file *parent = cache->heap[pi];
		if (bftw_heap_check(parent, file)) {
			break;
		}

		bftw_heap_move(cache, parent, i);
		i = pi;
	}

	bftw_heap_move(cache, file, i);
}

/** Bubble an entry down the heap. */
static void bftw_heap_bubble_down(struct bftw_cache *cache, struct bftw_file *file) {
	size_t i = file->heap_index;

	while (true) {
		size_t ci = 2*i + 1;
		if (ci >= cache->size) {
			break;
		}

		struct bftw_file *child = cache->heap[ci];

		size_t ri = ci + 1;
		if (ri < cache->size) {
			struct bftw_file *right = cache->heap[ri];
			if (!bftw_heap_check(child, right)) {
				ci = ri;
				child = right;
			}
		}

		if (bftw_heap_check(file, child)) {
			break;
		}

		bftw_heap_move(cache, child, i);
		i = ci;
	}

	bftw_heap_move(cache, file, i);
}

/** Bubble an entry up or down the heap. */
static void bftw_heap_bubble(struct bftw_cache *cache, struct bftw_file *file) {
	size_t i = file->heap_index;

	if (i > 0) {
		size_t pi = (i - 1)/2;
		struct bftw_file *parent = cache->heap[pi];
		if (!bftw_heap_check(parent, file)) {
			bftw_heap_bubble_up(cache, file);
			return;
		}
	}

	bftw_heap_bubble_down(cache, file);
}

/** Increment a bftw_file's reference count. */
static size_t bftw_file_incref(struct bftw_cache *cache, struct bftw_file *file) {
	size_t ret = ++file->refcount;
	if (file->fd >= 0) {
		bftw_heap_bubble_down(cache, file);
	}
	return ret;
}

/** Decrement a bftw_file's reference count. */
static size_t bftw_file_decref(struct bftw_cache *cache, struct bftw_file *file) {
	size_t ret = --file->refcount;
	if (file->fd >= 0) {
		bftw_heap_bubble_up(cache, file);
	}
	return ret;
}

/** Add a bftw_file to the cache. */
static void bftw_cache_add(struct bftw_cache *cache, struct bftw_file *file) {
	assert(cache->size < cache->capacity);
	assert(file->fd >= 0);

	size_t size = cache->size++;
	file->heap_index = size;
	bftw_heap_bubble_up(cache, file);
}

/** Remove a bftw_file from the cache. */
static void bftw_cache_remove(struct bftw_cache *cache, struct bftw_file *file) {
	assert(cache->size > 0);
	assert(file->fd >= 0);

	size_t size = --cache->size;
	size_t i = file->heap_index;
	if (i != size) {
		struct bftw_file *end = cache->heap[size];
		end->heap_index = i;
		bftw_heap_bubble(cache, end);
	}
}

/** Close a bftw_file. */
static void bftw_file_close(struct bftw_cache *cache, struct bftw_file *file) {
	assert(file->fd >= 0);

	bftw_cache_remove(cache, file);

	close(file->fd);
	file->fd = -1;
}

/** Pop a directory from the cache. */
static void bftw_cache_pop(struct bftw_cache *cache) {
	assert(cache->size > 0);
	bftw_file_close(cache, cache->heap[0]);
}

/**
 * Shrink the cache, to recover from EMFILE.
 *
 * @param cache
 *         The cache in question.
 * @param saved
 *         A bftw_file that must be preserved.
 * @return
 *         0 if successfully shrunk, otherwise -1.
 */
static int bftw_cache_shrink(struct bftw_cache *cache, const struct bftw_file *saved) {
	int ret = -1;
	struct bftw_file *file = NULL;

	if (cache->size >= 1) {
		file = cache->heap[0];
		if (file == saved && cache->size >= 2) {
			file = cache->heap[1];
		}
	}

	if (file && file != saved) {
		bftw_file_close(cache, file);
		ret = 0;
	}

	cache->capacity = cache->size;
	return ret;
}

/** Compute the name offset of a child path. */
static size_t bftw_child_nameoff(const struct bftw_file *parent) {
	size_t ret = parent->nameoff + parent->namelen;
	if (parent->name[parent->namelen - 1] != '/') {
		++ret;
	}
	return ret;
}

/** Create a new bftw_file. */
static struct bftw_file *bftw_file_new(struct bftw_cache *cache, struct bftw_file *parent, const char *name) {
	size_t namelen = strlen(name);
	size_t size = sizeof(struct bftw_file) + namelen + 1;

	struct bftw_file *file = malloc(size);
	if (!file) {
		return NULL;
	}

	file->parent = parent;

	if (parent) {
		file->depth = parent->depth + 1;
		file->root = parent->root;
		file->nameoff = bftw_child_nameoff(parent);
		bftw_file_incref(cache, parent);
	} else {
		file->depth = 0;
		file->root = name;
		file->nameoff = 0;
	}

	file->next = NULL;

	file->refcount = 1;
	file->fd = -1;

	file->typeflag = BFTW_UNKNOWN;
	file->dev = -1;
	file->ino = -1;

	file->namelen = namelen;
	memcpy(file->name, name, namelen + 1);

	return file;
}

/**
 * Get the appropriate (fd, path) pair for the *at() family of functions.
 *
 * @param file
 *         The file being accessed.
 * @param[out] at_fd
 *         Will hold the appropriate file descriptor to use.
 * @param[in,out] at_path
 *         Will hold the appropriate path to use.
 * @return The closest open ancestor file.
 */
static struct bftw_file *bftw_file_base(struct bftw_file *file, int *at_fd, const char **at_path) {
	struct bftw_file *base = file;

	do {
		base = base->parent;
	} while (base && base->fd < 0);

	if (base) {
		*at_fd = base->fd;
		*at_path += bftw_child_nameoff(base);
	}

	return base;
}

/**
 * Open a bftw_file relative to another one.
 *
 * @param cache
 *         The cache to hold the file.
 * @param file
 *         The file to open.
 * @param base
 *         The base directory for the relative path (may be NULL).
 * @param at_fd
 *         The base file descriptor, AT_FDCWD if base == NULL.
 * @param at_path
 *         The relative path to the file.
 * @return
 *         The opened file descriptor, or negative on error.
 */
static int bftw_file_openat(struct bftw_cache *cache, struct bftw_file *file, const struct bftw_file *base, int at_fd, const char *at_path) {
	assert(file->fd < 0);

	int flags = O_RDONLY | O_CLOEXEC | O_DIRECTORY;
	int fd = openat(at_fd, at_path, flags);

	if (fd < 0 && errno == EMFILE) {
		if (bftw_cache_shrink(cache, base) == 0) {
			fd = openat(base->fd, at_path, flags);
		}
	}

	if (fd >= 0) {
		if (cache->size == cache->capacity) {
			bftw_cache_pop(cache);
		}

		file->fd = fd;
		bftw_cache_add(cache, file);
	}

	return fd;
}

/**
 * Open a bftw_file.
 *
 * @param cache
 *         The cache to hold the file.
 * @param file
 *         The file to open.
 * @param path
 *         The full path to the file.
 * @return
 *         The opened file descriptor, or negative on error.
 */
static int bftw_file_open(struct bftw_cache *cache, struct bftw_file *file, const char *path) {
	int at_fd = AT_FDCWD;
	const char *at_path = path;
	struct bftw_file *base = bftw_file_base(file, &at_fd, &at_path);

	int fd = bftw_file_openat(cache, file, base, at_fd, at_path);
	if (fd >= 0 || errno != ENAMETOOLONG) {
		return fd;
	}

	// Handle ENAMETOOLONG by manually traversing the path component-by-component

	// -1 to include the root, which has depth == 0
	size_t offset = base ? base->depth : -1;
	size_t levels = file->depth - offset;
	if (levels < 2) {
		return fd;
	}

	struct bftw_file **parents = malloc(levels * sizeof(*parents));
	if (!parents) {
		return fd;
	}

	struct bftw_file *parent = file;
	for (size_t i = levels; i-- > 0;) {
		parents[i] = parent;
		parent = parent->parent;
	}

	for (size_t i = 0; i < levels; ++i) {
		fd = bftw_file_openat(cache, parents[i], base, at_fd, parents[i]->name);
		if (fd < 0) {
			break;
		}

		base = parents[i];
		at_fd = fd;
	}

	free(parents);
	return fd;
}

/**
 * Open a DIR* for a bftw_file.
 *
 * @param cache
 *         The cache to hold the file.
 * @param file
 *         The directory to open.
 * @param path
 *         The full path to the directory.
 * @return
 *         The opened DIR *, or NULL on error.
 */
static DIR *bftw_file_opendir(struct bftw_cache *cache, struct bftw_file *file, const char *path) {
	int fd = bftw_file_open(cache, file, path);
	if (fd < 0) {
		return NULL;
	}

	// Now we dup() the fd and pass it to fdopendir().  This way we can
	// close the DIR* as soon as we're done with it, reducing the memory
	// footprint significantly, while keeping the fd around for future
	// openat() calls.

	int dfd = dup_cloexec(fd);

	if (dfd < 0 && errno == EMFILE) {
		if (bftw_cache_shrink(cache, file) == 0) {
			dfd = dup_cloexec(fd);
		}
	}

	if (dfd < 0) {
		return NULL;
	}

	DIR *ret = fdopendir(dfd);
	if (!ret) {
		int error = errno;
		close(dfd);
		errno = error;
	}

	return ret;
}

/** Free a bftw_file. */
static void bftw_file_free(struct bftw_cache *cache, struct bftw_file *file) {
	assert(file->refcount == 0);

	if (file->fd >= 0) {
		bftw_file_close(cache, file);
	}

	free(file);
}

/**
 * A queue of bftw_file's to examine.
 */
struct bftw_queue {
	/** The head of the queue. */
	struct bftw_file *head;
	/** The tail of the queue. */
	struct bftw_file *tail;
};

/** Initialize a bftw_queue. */
static void bftw_queue_init(struct bftw_queue *queue) {
	queue->head = NULL;
	queue->tail = NULL;
}

/** Add a file to the tail of the bftw_queue. */
static void bftw_queue_push(struct bftw_queue *queue, struct bftw_file *file) {
	assert(file->next == NULL);

	if (!queue->head) {
		queue->head = file;
	}
	if (queue->tail) {
		queue->tail->next = file;
	}
	queue->tail = file;
}

/** Prepend a queue to the head of another one. */
static void bftw_queue_prepend(struct bftw_queue *head, struct bftw_queue *tail) {
	if (head->tail) {
		head->tail->next = tail->head;
	}
	if (head->head) {
		tail->head = head->head;
	}
	if (!tail->tail) {
		tail->tail = head->tail;
	}
	head->head = NULL;
	head->tail = NULL;
}

/** Pop the next file from the head of the queue. */
static struct bftw_file *bftw_queue_pop(struct bftw_queue *queue) {
	struct bftw_file *file = queue->head;
	queue->head = file->next;
	if (queue->tail == file) {
		queue->tail = NULL;
	}
	file->next = NULL;
	return file;
}

/**
 * A directory reader.
 */
struct bftw_reader {
	/** The open handle to the directory. */
	DIR *dir;
	/** The current directory entry. */
	struct dirent *de;
	/** Any error code that has occurred. */
	int error;
};

/** Initialize a reader. */
static void bftw_reader_init(struct bftw_reader *reader) {
	reader->dir = NULL;
	reader->de = NULL;
	reader->error = 0;
}

/** Open a directory for reading. */
static int bftw_reader_open(struct bftw_reader *reader, struct bftw_cache *cache, struct bftw_file *file, const char *path) {
	assert(!reader->dir);
	assert(!reader->de);

	reader->error = 0;

	reader->dir = bftw_file_opendir(cache, file, path);
	if (!reader->dir) {
		reader->error = errno;
		return -1;
	}

	return 0;
}

/** Read a directory entry. */
static int bftw_reader_read(struct bftw_reader *reader) {
	if (!reader->dir) {
		return -1;
	}

	if (xreaddir(reader->dir, &reader->de) != 0) {
		reader->error = errno;
		return -1;
	} else if (reader->de) {
		return 1;
	} else {
		return 0;
	}
}

/** Close a directory. */
static int bftw_reader_close(struct bftw_reader *reader) {
	int ret = 0;
	if (reader->dir && closedir(reader->dir) != 0) {
		reader->error = errno;
		ret = -1;
	}

	reader->de = NULL;
	reader->dir = NULL;
	return ret;
}

/**
 * Holds the current state of the bftw() traversal.
 */
struct bftw_state {
	/** bftw() callback. */
	bftw_callback *callback;
	/** bftw() callback data. */
	void *ptr;
	/** bftw() flags. */
	enum bftw_flags flags;
	/** Search strategy. */
	enum bftw_strategy strategy;
	/** The mount table. */
	const struct bfs_mtab *mtab;

	/** The appropriate errno value, if any. */
	int error;

	/** The cache of open directories. */
	struct bftw_cache cache;
	/** The queue of directories left to explore. */
	struct bftw_queue queue;
	/** An intermediate queue used for depth-first searches. */
	struct bftw_queue prequeue;

	/** The current path. */
	char *path;
	/** The current file. */
	struct bftw_file *file;
	/** The previous file. */
	struct bftw_file *previous;
	/** The reader for the current directory. */
	struct bftw_reader reader;

	/** Extra data about the current file. */
	struct BFTW ftwbuf;
};

/**
 * Initialize the bftw() state.
 */
static int bftw_state_init(struct bftw_state *state, const struct bftw_args *args) {
	state->callback = args->callback;
	state->ptr = args->ptr;
	state->flags = args->flags;
	state->strategy = args->strategy;
	state->mtab = args->mtab;

	state->error = 0;

	if (args->nopenfd < 2) {
		errno = EMFILE;
		goto err;
	}

	// Reserve 1 fd for the open DIR *
	if (bftw_cache_init(&state->cache, args->nopenfd - 1) != 0) {
		goto err;
	}

	bftw_queue_init(&state->queue);
	bftw_queue_init(&state->prequeue);

	state->path = dstralloc(0);
	if (!state->path) {
		goto err_cache;
	}

	state->file = NULL;
	state->previous = NULL;
	bftw_reader_init(&state->reader);

	return 0;

err_cache:
	bftw_cache_destroy(&state->cache);
err:
	return -1;
}

enum bftw_typeflag bftw_mode_typeflag(mode_t mode) {
	switch (mode & S_IFMT) {
#ifdef S_IFBLK
	case S_IFBLK:
		return BFTW_BLK;
#endif
#ifdef S_IFCHR
	case S_IFCHR:
		return BFTW_CHR;
#endif
#ifdef S_IFDIR
	case S_IFDIR:
		return BFTW_DIR;
#endif
#ifdef S_IFDOOR
	case S_IFDOOR:
		return BFTW_DOOR;
#endif
#ifdef S_IFIFO
	case S_IFIFO:
		return BFTW_FIFO;
#endif
#ifdef S_IFLNK
	case S_IFLNK:
		return BFTW_LNK;
#endif
#ifdef S_IFPORT
	case S_IFPORT:
		return BFTW_PORT;
#endif
#ifdef S_IFREG
	case S_IFREG:
		return BFTW_REG;
#endif
#ifdef S_IFSOCK
	case S_IFSOCK:
		return BFTW_SOCK;
#endif
#ifdef S_IFWHT
	case S_IFWHT:
		return BFTW_WHT;
#endif

	default:
		return BFTW_UNKNOWN;
	}
}

static enum bftw_typeflag bftw_dirent_typeflag(const struct dirent *de) {
#if defined(_DIRENT_HAVE_D_TYPE) || defined(DT_UNKNOWN)
	switch (de->d_type) {
#ifdef DT_BLK
	case DT_BLK:
		return BFTW_BLK;
#endif
#ifdef DT_CHR
	case DT_CHR:
		return BFTW_CHR;
#endif
#ifdef DT_DIR
	case DT_DIR:
		return BFTW_DIR;
#endif
#ifdef DT_DOOR
	case DT_DOOR:
		return BFTW_DOOR;
#endif
#ifdef DT_FIFO
	case DT_FIFO:
		return BFTW_FIFO;
#endif
#ifdef DT_LNK
	case DT_LNK:
		return BFTW_LNK;
#endif
#ifdef DT_PORT
	case DT_PORT:
		return BFTW_PORT;
#endif
#ifdef DT_REG
	case DT_REG:
		return BFTW_REG;
#endif
#ifdef DT_SOCK
	case DT_SOCK:
		return BFTW_SOCK;
#endif
#ifdef DT_WHT
	case DT_WHT:
		return BFTW_WHT;
#endif
	}
#endif

	return BFTW_UNKNOWN;
}

/** Cached bfs_stat(). */
static const struct bfs_stat *bftw_stat_impl(struct BFTW *ftwbuf, struct bftw_stat *cache, enum bfs_stat_flag flags) {
	if (!cache->buf) {
		if (cache->error) {
			errno = cache->error;
		} else if (bfs_stat(ftwbuf->at_fd, ftwbuf->at_path, flags, &cache->storage) == 0) {
			cache->buf = &cache->storage;
		} else {
			cache->error = errno;
		}
	}

	return cache->buf;
}

const struct bfs_stat *bftw_stat(const struct BFTW *ftwbuf, enum bfs_stat_flag flags) {
	struct BFTW *mutbuf = (struct BFTW *)ftwbuf;
	const struct bfs_stat *ret;

	if (flags & BFS_STAT_NOFOLLOW) {
		ret = bftw_stat_impl(mutbuf, &mutbuf->lstat_cache, BFS_STAT_NOFOLLOW);
		if (ret && !S_ISLNK(ret->mode) && !mutbuf->stat_cache.buf) {
			// Non-link, so share stat info
			mutbuf->stat_cache.buf = ret;
		}
	} else {
		ret = bftw_stat_impl(mutbuf, &mutbuf->stat_cache, BFS_STAT_FOLLOW);
		if (!ret && (flags & BFS_STAT_TRYFOLLOW) && is_nonexistence_error(errno)) {
			ret = bftw_stat_impl(mutbuf, &mutbuf->lstat_cache, BFS_STAT_NOFOLLOW);
		}
	}

	return ret;
}

enum bftw_typeflag bftw_typeflag(const struct BFTW *ftwbuf, enum bfs_stat_flag flags) {
	if (ftwbuf->stat_flags & BFS_STAT_NOFOLLOW) {
		if ((flags & BFS_STAT_NOFOLLOW) || ftwbuf->typeflag != BFTW_LNK) {
			return ftwbuf->typeflag;
		}
	} else if ((flags & (BFS_STAT_NOFOLLOW | BFS_STAT_TRYFOLLOW)) == BFS_STAT_TRYFOLLOW || ftwbuf->typeflag == BFTW_LNK) {
		return ftwbuf->typeflag;
	}

	const struct bfs_stat *statbuf = bftw_stat(ftwbuf, flags);
	if (statbuf) {
		return bftw_mode_typeflag(statbuf->mode);
	} else {
		return BFTW_ERROR;
	}
}

/**
 * Update the path for the current file.
 */
static int bftw_update_path(struct bftw_state *state, const char *name) {
	const struct bftw_file *file = state->file;
	size_t length = file ? file->nameoff + file->namelen : 0;

	assert(dstrlen(state->path) >= length);
	dstresize(&state->path, length);

	if (name) {
		if (length > 0 && state->path[length - 1] != '/') {
			if (dstrapp(&state->path, '/') != 0) {
				return -1;
			}
		}
		if (dstrcat(&state->path, name) != 0) {
			return -1;
		}
	}

	return 0;
}

/** Check if a stat() call is needed for this visit. */
static bool bftw_need_stat(const struct bftw_state *state) {
	if (state->flags & BFTW_STAT) {
		return true;
	}

	const struct BFTW *ftwbuf = &state->ftwbuf;
	if (ftwbuf->typeflag == BFTW_UNKNOWN) {
		return true;
	}

	if (ftwbuf->typeflag == BFTW_LNK && !(ftwbuf->stat_flags & BFS_STAT_NOFOLLOW)) {
		return true;
	}

	if (ftwbuf->typeflag == BFTW_DIR) {
		if (state->flags & (BFTW_DETECT_CYCLES | BFTW_XDEV)) {
			return true;
		}
#if __linux__
	} else if (state->mtab) {
		// Linux fills in d_type from the underlying inode, even when
		// the directory entry is a bind mount point.  In that case, we
		// need to stat() to get the correct type.  We don't need to
		// check for directories because they can only be mounted over
		// by other directories.
		if (bfs_maybe_mount(state->mtab, ftwbuf->path)) {
			return true;
		}
#endif
	}

	return false;
}

/** Initialize bftw_stat cache. */
static void bftw_stat_init(struct bftw_stat *cache) {
	cache->buf = NULL;
	cache->error = 0;
}

/**
 * Open a file if necessary.
 *
 * @param file
 *         The file to open.
 * @param path
 *         The path to that file or one of its descendants.
 * @return
 *         The opened file descriptor, or -1 on error.
 */
static int bftw_ensure_open(struct bftw_cache *cache, struct bftw_file *file, const char *path) {
	int ret = file->fd;

	if (ret < 0) {
		char *copy = strndup(path, file->nameoff + file->namelen);
		if (!copy) {
			return -1;
		}

		ret = bftw_file_open(cache, file, copy);
		free(copy);
	}

	return ret;
}

/**
 * Initialize the buffers with data about the current path.
 */
static void bftw_init_ftwbuf(struct bftw_state *state, enum bftw_visit visit) {
	struct bftw_file *file = state->file;
	const struct bftw_reader *reader = &state->reader;
	const struct dirent *de = reader->de;

	struct BFTW *ftwbuf = &state->ftwbuf;
	ftwbuf->path = state->path;
	ftwbuf->root = file ? file->root : ftwbuf->path;
	ftwbuf->depth = 0;
	ftwbuf->visit = visit;
	ftwbuf->typeflag = BFTW_UNKNOWN;
	ftwbuf->error = reader->error;
	ftwbuf->at_fd = AT_FDCWD;
	ftwbuf->at_path = ftwbuf->path;
	ftwbuf->stat_flags = BFS_STAT_NOFOLLOW;
	bftw_stat_init(&ftwbuf->lstat_cache);
	bftw_stat_init(&ftwbuf->stat_cache);

	struct bftw_file *parent = NULL;
	if (de) {
		parent = file;
		ftwbuf->depth = file->depth + 1;
		ftwbuf->typeflag = bftw_dirent_typeflag(de);
		ftwbuf->nameoff = bftw_child_nameoff(file);
	} else if (file) {
		parent = file->parent;
		ftwbuf->depth = file->depth;
		ftwbuf->typeflag = file->typeflag;
		ftwbuf->nameoff = file->nameoff;
	}

	if (parent) {
		// Try to ensure the immediate parent is open, to avoid ENAMETOOLONG
		if (bftw_ensure_open(&state->cache, parent, state->path) >= 0) {
			ftwbuf->at_fd = parent->fd;
			ftwbuf->at_path += ftwbuf->nameoff;
		} else {
			ftwbuf->error = errno;
		}
	}

	if (ftwbuf->depth == 0) {
		// Compute the name offset for root paths like "foo/bar"
		ftwbuf->nameoff = xbasename(ftwbuf->path) - ftwbuf->path;
	}

	if (ftwbuf->error != 0) {
		ftwbuf->typeflag = BFTW_ERROR;
		return;
	}

	int follow_flags = BFTW_LOGICAL;
	if (ftwbuf->depth == 0) {
		follow_flags |= BFTW_COMFOLLOW;
	}
	bool follow = state->flags & follow_flags;
	if (follow) {
		ftwbuf->stat_flags = BFS_STAT_TRYFOLLOW;
	}

	const struct bfs_stat *statbuf = NULL;
	if (bftw_need_stat(state)) {
		statbuf = bftw_stat(ftwbuf, ftwbuf->stat_flags);
		if (statbuf) {
			ftwbuf->typeflag = bftw_mode_typeflag(statbuf->mode);
		} else {
			ftwbuf->typeflag = BFTW_ERROR;
			ftwbuf->error = errno;
			return;
		}
	}

	if (ftwbuf->typeflag == BFTW_DIR && (state->flags & BFTW_DETECT_CYCLES)) {
		for (const struct bftw_file *parent = file; parent; parent = parent->parent) {
			if (parent->depth == ftwbuf->depth) {
				continue;
			}
			if (parent->dev == statbuf->dev && parent->ino == statbuf->ino) {
				ftwbuf->typeflag = BFTW_ERROR;
				ftwbuf->error = ELOOP;
				return;
			}
		}
	}
}

/** Fill file identity information from an ftwbuf. */
static void bftw_fill_id(struct bftw_file *file, const struct BFTW *ftwbuf) {
	const struct bfs_stat *statbuf = ftwbuf->stat_cache.buf;
	if (!statbuf || (ftwbuf->stat_flags & BFS_STAT_NOFOLLOW)) {
		statbuf = ftwbuf->lstat_cache.buf;
	}
	if (statbuf) {
		file->dev = statbuf->dev;
		file->ino = statbuf->ino;
	}
}

/**
 * Visit a path, invoking the callback.
 */
static enum bftw_action bftw_visit(struct bftw_state *state, const char *name, enum bftw_visit visit) {
	if (bftw_update_path(state, name) != 0) {
		state->error = errno;
		return BFTW_STOP;
	}

	const struct BFTW *ftwbuf = &state->ftwbuf;
	bftw_init_ftwbuf(state, visit);

	// Never give the callback BFTW_ERROR unless BFTW_RECOVER is specified
	if (ftwbuf->typeflag == BFTW_ERROR && !(state->flags & BFTW_RECOVER)) {
		state->error = ftwbuf->error;
		return BFTW_STOP;
	}

	enum bftw_action ret = state->callback(ftwbuf, state->ptr);
	switch (ret) {
	case BFTW_CONTINUE:
		break;
	case BFTW_PRUNE:
	case BFTW_STOP:
		goto done;
	default:
		state->error = EINVAL;
		return BFTW_STOP;
	}

	if (visit != BFTW_PRE || ftwbuf->typeflag != BFTW_DIR) {
		ret = BFTW_PRUNE;
		goto done;
	}

	if (state->flags & BFTW_XDEV) {
		const struct bftw_file *parent = state->file;
		if (parent && !name) {
			parent = parent->parent;
		}

		if (parent) {
			const struct bfs_stat *statbuf = bftw_stat(ftwbuf, ftwbuf->stat_flags);
			if (statbuf && statbuf->dev != parent->dev) {
				ret = BFTW_PRUNE;
				goto done;
			}
		}
	}

done:
	if (state->file && !name) {
		bftw_fill_id(state->file, ftwbuf);
	}

	return ret;
}

/**
 * Push a new file onto the queue.
 */
static int bftw_push(struct bftw_state *state, const char *name, bool fill_id) {
	struct bftw_file *parent = state->file;
	struct bftw_file *file = bftw_file_new(&state->cache, parent, name);
	if (!file) {
		state->error = errno;
		return -1;
	}

	struct dirent *de = state->reader.de;
	if (de) {
		file->typeflag = bftw_dirent_typeflag(de);
	}

	if (fill_id) {
		bftw_fill_id(file, &state->ftwbuf);
	}

	if (state->strategy == BFTW_DFS) {
		bftw_queue_push(&state->prequeue, file);
	} else {
		bftw_queue_push(&state->queue, file);
	}

	return 0;
}

/**
 * Build the path to the current file.
 */
static int bftw_build_path(struct bftw_state *state) {
	const struct bftw_file *file = state->file;

	size_t pathlen = file->nameoff + file->namelen;
	if (dstresize(&state->path, pathlen) != 0) {
		state->error = errno;
		return -1;
	}

	// Try to find a common ancestor with the existing path
	const struct bftw_file *ancestor = state->previous;
	while (ancestor && ancestor->depth > file->depth) {
		ancestor = ancestor->parent;
	}

	// Build the path backwards
	while (file && file != ancestor) {
		if (file->nameoff > 0) {
			state->path[file->nameoff - 1] = '/';
		}
		memcpy(state->path + file->nameoff, file->name, file->namelen);

		if (ancestor && ancestor->depth == file->depth) {
			ancestor = ancestor->parent;
		}
		file = file->parent;
	}

	state->previous = state->file;
	return 0;
}

/**
 * Pop the next file from the queue.
 */
static int bftw_pop(struct bftw_state *state) {
	if (state->strategy == BFTW_DFS) {
		bftw_queue_prepend(&state->prequeue, &state->queue);
	}

	if (!state->queue.head) {
		return 0;
	}

	state->file = bftw_queue_pop(&state->queue);

	if (bftw_build_path(state) != 0) {
		return -1;
	}

	return 1;
}

/**
 * Open a reader for the current directory.
 */
static struct bftw_reader *bftw_open(struct bftw_state *state) {
	struct bftw_reader *reader = &state->reader;
	bftw_reader_open(reader, &state->cache, state->file, state->path);
	return reader;
}

/**
 * Close and release the reader.
 */
static enum bftw_action bftw_release_reader(struct bftw_state *state, bool do_visit) {
	enum bftw_action ret = BFTW_CONTINUE;

	struct bftw_reader *reader = &state->reader;
	bftw_reader_close(reader);

	if (reader->error != 0) {
		if (do_visit) {
			if (bftw_visit(state, NULL, BFTW_PRE) == BFTW_STOP) {
				ret = BFTW_STOP;
			}
		} else {
			state->error = reader->error;
		}
		reader->error = 0;
	}

	return ret;
}

/**
 * Finalize and free a file we're done with.
 */
static enum bftw_action bftw_release_file(struct bftw_state *state, bool visit_file, bool visit_parents) {
	enum bftw_action ret = BFTW_CONTINUE;

	if (!(state->flags & BFTW_DEPTH)) {
		visit_file = false;
		visit_parents = false;
	}
	bool do_visit = visit_file;

	while (state->file) {
		if (bftw_file_decref(&state->cache, state->file) > 0) {
			state->file = NULL;
			break;
		}

		if (do_visit) {
			if (bftw_visit(state, NULL, BFTW_POST) == BFTW_STOP) {
				ret = BFTW_STOP;
				visit_parents = false;
			}
		}
		do_visit = visit_parents;

		struct bftw_file *parent = state->file->parent;
		if (state->previous == state->file) {
			state->previous = parent;
		}
		bftw_file_free(&state->cache, state->file);
		state->file = parent;
	}

	return ret;
}

/**
 * Drain all the entries from a bftw_queue.
 */
static void bftw_drain_queue(struct bftw_state *state, struct bftw_queue *queue) {
	while (queue->head) {
		state->file = bftw_queue_pop(queue);
		bftw_release_file(state, false, false);
	}
}

/**
 * Dispose of the bftw() state.
 *
 * @return
 *         The bftw() return value.
 */
static int bftw_state_destroy(struct bftw_state *state) {
	dstrfree(state->path);

	bftw_release_reader(state, false);

	bftw_release_file(state, false, false);
	bftw_drain_queue(state, &state->prequeue);
	bftw_drain_queue(state, &state->queue);

	bftw_cache_destroy(&state->cache);

	errno = state->error;
	return state->error ? -1 : 0;
}

/**
 * Breadth-first bftw() implementation.
 */
static int bftw_bfs(const struct bftw_args *args) {
	struct bftw_state state;
	if (bftw_state_init(&state, args) != 0) {
		return -1;
	}

	for (size_t i = 0; i < args->npaths; ++i) {
		const char *path = args->paths[i];

		switch (bftw_visit(&state, path, BFTW_PRE)) {
		case BFTW_CONTINUE:
			break;
		case BFTW_PRUNE:
			continue;
		case BFTW_STOP:
			goto done;
		}

		if (bftw_push(&state, path, true) != 0) {
			goto done;
		}
	}

	while (bftw_pop(&state) > 0) {
		struct bftw_reader *reader = bftw_open(&state);

		while (bftw_reader_read(reader) > 0) {
			const char *name = reader->de->d_name;

			switch (bftw_visit(&state, name, BFTW_PRE)) {
			case BFTW_CONTINUE:
				break;
			case BFTW_PRUNE:
				continue;
			case BFTW_STOP:
				goto done;
			}

			if (bftw_push(&state, name, true) != 0) {
				goto done;
			}
		}

		if (bftw_release_reader(&state, true) == BFTW_STOP) {
			goto done;
		}
		if (bftw_release_file(&state, true, true) == BFTW_STOP) {
			goto done;
		}
	}

done:
	return bftw_state_destroy(&state);
}

/**
 * Depth-first bftw() implementation.
 */
static int bftw_dfs(const struct bftw_args *args) {
	struct bftw_state state;
	if (bftw_state_init(&state, args) != 0) {
		return -1;
	}

	for (size_t i = 0; i < args->npaths; ++i) {
		if (bftw_push(&state, args->paths[i], false) != 0) {
			goto done;
		}
	}

	while (bftw_pop(&state) > 0) {
		bool visit_post = true;

		switch (bftw_visit(&state, NULL, BFTW_PRE)) {
		case BFTW_CONTINUE:
			break;
		case BFTW_PRUNE:
			visit_post = false;
			goto next;
		case BFTW_STOP:
			goto done;
		}

		struct bftw_reader *reader = bftw_open(&state);

		while (bftw_reader_read(reader) > 0) {
			if (bftw_push(&state, reader->de->d_name, false) != 0) {
				goto done;
			}
		}

		if (bftw_release_reader(&state, true) == BFTW_STOP) {
			goto done;
		}

	next:
		if (bftw_release_file(&state, visit_post, true) == BFTW_STOP) {
			goto done;
		}
	}

done:
	return bftw_state_destroy(&state);
}

/**
 * Iterative deepening search state.
 */
struct bftw_ids_state {
	/** The wrapped callback. */
	bftw_callback *delegate;
	/** The wrapped callback arguments. */
	void *ptr;
	/** Which visit this search corresponds to. */
	enum bftw_visit visit;
	/** The current target depth. */
	size_t depth;
	/** The set of pruned paths. */
	struct trie *pruned;
	/** An error code to report. */
	int error;
	/** Whether the bottom has been found. */
	bool bottom;
	/** Whether to quit the search. */
	bool quit;
};

/** Iterative deepening callback function. */
static enum bftw_action bftw_ids_callback(const struct BFTW *ftwbuf, void *ptr) {
	struct bftw_ids_state *state = ptr;

	struct BFTW *mutbuf = (struct BFTW *)ftwbuf;
	mutbuf->visit = state->visit;

	if (ftwbuf->typeflag == BFTW_ERROR) {
		if (state->depth - ftwbuf->depth <= 1) {
			return state->delegate(ftwbuf, state->ptr);
		} else {
			return BFTW_PRUNE;
		}
	}

	if (ftwbuf->depth < state->depth) {
		if (trie_find_str(state->pruned, ftwbuf->path)) {
			return BFTW_PRUNE;
		} else {
			return BFTW_CONTINUE;
		}
	} else if (state->visit == BFTW_POST) {
		if (trie_find_str(state->pruned, ftwbuf->path)) {
			return BFTW_PRUNE;
		}
	}

	state->bottom = false;

	enum bftw_action ret = state->delegate(ftwbuf, state->ptr);

	switch (ret) {
	case BFTW_CONTINUE:
		ret = BFTW_PRUNE;
		break;
	case BFTW_PRUNE:
		if (ftwbuf->typeflag == BFTW_DIR) {
			if (!trie_insert_str(state->pruned, ftwbuf->path)) {
				state->error = errno;
				state->quit = true;
				ret = BFTW_STOP;
			}
		}
		break;
	case BFTW_STOP:
		state->quit = true;
		break;
	}

	return ret;
}

/**
 * Iterative deepening bftw() wrapper.
 */
static int bftw_ids(const struct bftw_args *args) {
	struct trie pruned;
	trie_init(&pruned);

	struct bftw_ids_state state = {
		.delegate = args->callback,
		.ptr = args->ptr,
		.visit = BFTW_PRE,
		.depth = 0,
		.pruned = &pruned,
		.bottom = false,
	};

	struct bftw_args ids_args = *args;
	ids_args.callback = bftw_ids_callback;
	ids_args.ptr = &state;
	ids_args.flags &= ~BFTW_DEPTH;
	ids_args.strategy = BFTW_DFS;

	int ret = 0;

	while (ret == 0 && !state.quit && !state.bottom) {
		state.bottom = true;

		// bftw_bfs() is more efficient than bftw_dfs() since it visits
		// directory entries as it reads them.  With args->strategy ==
		// BFTW_DFS, it gives a hybrid ordering that visits immediate
		// children first, then deeper descendants depth-first.  This
		// doesn't matter for iterative deepening since we only visit
		// one level at a time.
		ret = bftw_bfs(&ids_args);

		++state.depth;
	}

	if (args->flags & BFTW_DEPTH) {
		state.visit = BFTW_POST;

		while (ret == 0 && !state.quit && state.depth > 0) {
			--state.depth;
			ret = bftw_bfs(&ids_args);
		}
	}

	if (state.error) {
		ret = -1;
	} else {
		state.error = errno;
	}
	trie_destroy(&pruned);
	errno = state.error;
	return ret;
}

int bftw(const struct bftw_args *args) {
	switch (args->strategy) {
	case BFTW_BFS:
		return bftw_bfs(args);
	case BFTW_DFS:
		return bftw_dfs(args);
	case BFTW_IDS:
		return bftw_ids(args);
	}

	errno = EINVAL;
	return -1;
}
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2015-2019 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * The parsed form of LS_COLORS.
 */
struct colors {
	char *reset;
	char *leftcode;
	char *rightcode;
	char *endcode;
	char *clear_to_eol;

	char *bold;
	char *gray;
	char *red;
	char *green;
	char *yellow;
	char *blue;
	char *magenta;
	char *cyan;
	char *white;

	char *warning;
	char *error;

	char *normal;

	char *file;
	char *multi_hard;
	char *executable;
	char *capable;
	char *setgid;
	char *setuid;

	char *directory;
	char *sticky;
	char *other_writable;
	char *sticky_other_writable;

	char *link;
	char *orphan;
	char *missing;
	bool link_as_target;

	char *blockdev;
	char *chardev;
	char *door;
	char *pipe;
	char *socket;

	/** A mapping from color names (fi, di, ln, etc.) to struct fields. */
	struct trie names;

	/** A mapping from file extensions to colors. */
	struct trie ext_colors;
};

/** Initialize a color in the table. */
static int init_color(struct colors *colors, const char *name, const char *value, char **field) {
	if (value) {
		*field = dstrdup(value);
		if (!*field) {
			return -1;
		}
	} else {
		*field = NULL;
	}

	struct trie_leaf *leaf = trie_insert_str(&colors->names, name);
	if (leaf) {
		leaf->value = field;
		return 0;
	} else {
		return -1;
	}
}

/** Get a color from the table. */
static char **get_color(const struct colors *colors, const char *name) {
	const struct trie_leaf *leaf = trie_find_str(&colors->names, name);
	if (leaf) {
		return (char **)leaf->value;
	} else {
		return NULL;
	}
}

/** Set the value of a color. */
static void set_color(struct colors *colors, const char *name, char *value) {
	char **color = get_color(colors, name);
	if (color) {
		dstrfree(*color);
		*color = value;
	}
}

/**
 * Transform a file extension for fast lookups, by reversing and lowercasing it.
 */
static void extxfrm(char *ext) {
	size_t len = strlen(ext);
	for (size_t i = 0; i < len - i; ++i) {
		char a = ext[i];
		char b = ext[len - i - 1];

		// What's internationalization?  Doesn't matter, this is what
		// GNU ls does.  Luckily, since there's no standard C way to
		// casefold.  Not using tolower() here since it respects the
		// current locale, which GNU ls doesn't do.
		if (a >= 'A' && a <= 'Z') {
			a += 'a' - 'A';
		}
		if (b >= 'A' && b <= 'Z') {
			b += 'a' - 'A';
		}

		ext[i] = b;
		ext[len - i - 1] = a;
	}
}

/**
 * Set the color for an extension.
 */
static int set_ext_color(struct colors *colors, char *key, const char *value) {
	extxfrm(key);

	// A later *.x should override any earlier *.x, *.y.x, etc.
	struct trie_leaf *match;
	while ((match = trie_find_postfix(&colors->ext_colors, key))) {
		dstrfree(match->value);
		trie_remove(&colors->ext_colors, match);
	}

	struct trie_leaf *leaf = trie_insert_str(&colors->ext_colors, key);
	if (leaf) {
		leaf->value = (char *)value;
		return 0;
	} else {
		return -1;
	}
}

/**
 * Find a color by an extension.
 */
static const char *get_ext_color(const struct colors *colors, const char *filename) {
	char *xfrm = strdup(filename);
	if (!xfrm) {
		return NULL;
	}
	extxfrm(xfrm);

	const struct trie_leaf *leaf = trie_find_prefix(&colors->ext_colors, xfrm);
	free(xfrm);
	if (leaf) {
		return leaf->value;
	} else {
		return NULL;
	}
}

/**
 * Parse a chunk of LS_COLORS that may have escape sequences.  The supported
 * escapes are:
 *
 * \a, \b, \f, \n, \r, \t, \v:
 *     As in C
 * \e:
 *     ESC (\033)
 * \?:
 *     DEL (\177)
 * \_:
 *     ' ' (space)
 * \NNN:
 *     Octal
 * \xNN:
 *     Hex
 * ^C:
 *     Control character.
 *
 * See man dir_colors.
 *
 * @param value
 *         The value to parse.
 * @param end
 *         The character that marks the end of the chunk.
 * @param[out] next
 *         Will be set to the next chunk.
 * @return
 *         The parsed chunk as a dstring.
 */
static char *unescape(const char *value, char end, const char **next) {
	if (!value) {
		goto fail;
	}

	char *str = dstralloc(0);
	if (!str) {
		goto fail_str;
	}

	const char *i;
	for (i = value; *i && *i != end; ++i) {
		unsigned char c = 0;

		switch (*i) {
		case '\\':
			switch (*++i) {
			case 'a':
				c = '\a';
				break;
			case 'b':
				c = '\b';
				break;
			case 'e':
				c = '\033';
				break;
			case 'f':
				c = '\f';
				break;
			case 'n':
				c = '\n';
				break;
			case 'r':
				c = '\r';
				break;
			case 't':
				c = '\t';
				break;
			case 'v':
				c = '\v';
				break;
			case '?':
				c = '\177';
				break;
			case '_':
				c = ' ';
				break;

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				while (i[1] >= '0' && i[1] <= '7') {
					c <<= 3;
					c |= *i++ - '0';
				}
				c <<= 3;
				c |= *i - '0';
				break;

			case 'X':
			case 'x':
				while (true) {
					if (i[1] >= '0' && i[1] <= '9') {
						c <<= 4;
						c |= i[1] - '0';
					} else if (i[1] >= 'A' && i[1] <= 'F') {
						c <<= 4;
						c |= i[1] - 'A' + 0xA;
					} else if (i[1] >= 'a' && i[1] <= 'f') {
						c <<= 4;
						c |= i[1] - 'a' + 0xA;
					} else {
						break;
					}
					++i;
				}
				break;

			case '\0':
				goto fail_str;

			default:
				c = *i;
				break;
			}
			break;

		case '^':
			switch (*++i) {
			case '?':
				c = '\177';
				break;
			case '\0':
				goto fail_str;
			default:
				// CTRL masks bits 6 and 7
				c = *i & 0x1F;
				break;
			}
			break;

		default:
			c = *i;
			break;
		}

		if (dstrapp(&str, c) != 0) {
			goto fail_str;
		}
	}

	if (*i) {
		*next = i + 1;
	} else {
		*next = NULL;
	}

	return str;

fail_str:
	dstrfree(str);
fail:
	*next = NULL;
	return NULL;
}

struct colors *parse_colors(const char *ls_colors) {
	struct colors *colors = malloc(sizeof(struct colors));
	if (!colors) {
		return NULL;
	}

	trie_init(&colors->names);
	trie_init(&colors->ext_colors);

	int ret = 0;

	// From man console_codes

	ret |= init_color(colors, "rs", "0",      &colors->reset);
	ret |= init_color(colors, "lc", "\033[",  &colors->leftcode);
	ret |= init_color(colors, "rc", "m",      &colors->rightcode);
	ret |= init_color(colors, "ec", NULL,     &colors->endcode);
	ret |= init_color(colors, "cl", "\033[K", &colors->clear_to_eol);

	ret |= init_color(colors, "bld", "01",    &colors->bold);
	ret |= init_color(colors, "gry", "01;30", &colors->gray);
	ret |= init_color(colors, "red", "01;31", &colors->red);
	ret |= init_color(colors, "grn", "01;32", &colors->green);
	ret |= init_color(colors, "ylw", "01;33", &colors->yellow);
	ret |= init_color(colors, "blu", "01;34", &colors->blue);
	ret |= init_color(colors, "mag", "01;35", &colors->magenta);
	ret |= init_color(colors, "cyn", "01;36", &colors->cyan);
	ret |= init_color(colors, "wht", "01;37", &colors->white);

	ret |= init_color(colors, "wr", "01;33", &colors->warning);
	ret |= init_color(colors, "er", "01;31", &colors->error);

	// Defaults from man dir_colors

	ret |= init_color(colors, "no", NULL, &colors->normal);

	ret |= init_color(colors, "fi", NULL,    &colors->file);
	ret |= init_color(colors, "mh", NULL,    &colors->multi_hard);
	ret |= init_color(colors, "ex", "01;32", &colors->executable);
	ret |= init_color(colors, "ca", "30;41", &colors->capable);
	ret |= init_color(colors, "sg", "30;43", &colors->setgid);
	ret |= init_color(colors, "su", "37;41", &colors->setuid);

	ret |= init_color(colors, "di", "01;34", &colors->directory);
	ret |= init_color(colors, "st", "37;44", &colors->sticky);
	ret |= init_color(colors, "ow", "34;42", &colors->other_writable);
	ret |= init_color(colors, "tw", "30;42", &colors->sticky_other_writable);

	ret |= init_color(colors, "ln", "01;36", &colors->link);
	ret |= init_color(colors, "or", NULL,    &colors->orphan);
	ret |= init_color(colors, "mi", NULL,    &colors->missing);
	colors->link_as_target = false;

	ret |= init_color(colors, "bd", "01;33", &colors->blockdev);
	ret |= init_color(colors, "cd", "01;33", &colors->chardev);
	ret |= init_color(colors, "do", "01;35", &colors->door);
	ret |= init_color(colors, "pi", "33",    &colors->pipe);
	ret |= init_color(colors, "so", "01;35", &colors->socket);

	if (ret) {
		free_colors(colors);
		return NULL;
	}

	for (const char *chunk = ls_colors, *next; chunk; chunk = next) {
		if (chunk[0] == '*') {
			char *key = unescape(chunk + 1, '=', &next);
			if (!key) {
				continue;
			}

			char *value = unescape(next, ':', &next);
			if (value) {
				if (set_ext_color(colors, key, value) != 0) {
					dstrfree(value);
				}
			}

			dstrfree(key);
		} else {
			const char *equals = strchr(chunk, '=');
			if (!equals) {
				break;
			}

			char *value = unescape(equals + 1, ':', &next);
			if (!value) {
				continue;
			}

			char *key = strndup(chunk, equals - chunk);
			if (!key) {
				dstrfree(value);
				continue;
			}

			// All-zero values should be treated like NULL, to fall
			// back on any other relevant coloring for that file
			if (strspn(value, "0") == strlen(value)
			    && strcmp(key, "rs") != 0
			    && strcmp(key, "lc") != 0
			    && strcmp(key, "rc") != 0
			    && strcmp(key, "ec") != 0) {
				dstrfree(value);
				value = NULL;
			}

			set_color(colors, key, value);
			free(key);
		}
	}

	if (colors->link && strcmp(colors->link, "target") == 0) {
		colors->link_as_target = true;
		dstrfree(colors->link);
		colors->link = NULL;
	}

	return colors;
}

void free_colors(struct colors *colors) {
	if (colors) {
		struct trie_leaf *leaf;
		while ((leaf = trie_first_leaf(&colors->ext_colors))) {
			dstrfree(leaf->value);
			trie_remove(&colors->ext_colors, leaf);
		}
		trie_destroy(&colors->ext_colors);

		while ((leaf = trie_first_leaf(&colors->names))) {
			char **field = leaf->value;
			dstrfree(*field);
			trie_remove(&colors->names, leaf);
		}
		trie_destroy(&colors->names);

		free(colors);
	}
}

CFILE *cfopen(const char *path, const struct colors *colors) {
	CFILE *cfile = malloc(sizeof(*cfile));
	if (!cfile) {
		return NULL;
	}

	cfile->close = false;
	cfile->file = fopen(path, "wb");
	if (!cfile->file) {
		cfclose(cfile);
		return NULL;
	}
	cfile->close = true;

	if (isatty(fileno(cfile->file))) {
		cfile->colors = colors;
	} else {
		cfile->colors = NULL;
	}

	return cfile;
}

CFILE *cfdup(FILE *file, const struct colors *colors) {
	CFILE *cfile = malloc(sizeof(*cfile));
	if (!cfile) {
		return NULL;
	}

	cfile->close = false;
	cfile->file = file;

	if (isatty(fileno(file))) {
		cfile->colors = colors;
	} else {
		cfile->colors = NULL;
	}

	return cfile;
}

int cfclose(CFILE *cfile) {
	int ret = 0;
	if (cfile) {
		if (cfile->close) {
			ret = fclose(cfile->file);
		}
		free(cfile);
	}
	return ret;
}

/** Check if a symlink is broken. */
static bool is_link_broken(const struct BFTW *ftwbuf) {
	if (ftwbuf->stat_flags & BFS_STAT_NOFOLLOW) {
		return xfaccessat(ftwbuf->at_fd, ftwbuf->at_path, F_OK) != 0;
	} else {
		return true;
	}
}

/** Get the color for a file. */
static const char *file_color(const struct colors *colors, const char *filename, const struct BFTW *ftwbuf, enum bfs_stat_flag flags) {
	enum bftw_typeflag typeflag = bftw_typeflag(ftwbuf, flags);
	if (typeflag == BFTW_ERROR) {
		goto error;
	}

	const struct bfs_stat *statbuf = NULL;
	const char *color = NULL;

	switch (typeflag) {
	case BFTW_REG:
		if (colors->setuid || colors->setgid || colors->executable || colors->multi_hard) {
			statbuf = bftw_stat(ftwbuf, flags);
			if (!statbuf) {
				goto error;
			}
		}

		if (colors->setuid && (statbuf->mode & 04000)) {
			color = colors->setuid;
		} else if (colors->setgid && (statbuf->mode & 02000)) {
			color = colors->setgid;
		} else if (colors->capable && bfs_check_capabilities(ftwbuf) > 0) {
			color = colors->capable;
		} else if (colors->executable && (statbuf->mode & 00111)) {
			color = colors->executable;
		} else if (colors->multi_hard && statbuf->nlink > 1) {
			color = colors->multi_hard;
		}

		if (!color) {
			color = get_ext_color(colors, filename);
		}

		if (!color) {
			color = colors->file;
		}

		break;

	case BFTW_DIR:
		if (colors->sticky_other_writable || colors->other_writable || colors->sticky) {
			statbuf = bftw_stat(ftwbuf, flags);
			if (!statbuf) {
				goto error;
			}
		}

		if (colors->sticky_other_writable && (statbuf->mode & 01002) == 01002) {
			color = colors->sticky_other_writable;
		} else if (colors->other_writable && (statbuf->mode & 00002)) {
			color = colors->other_writable;
		} else if (colors->sticky && (statbuf->mode & 01000)) {
			color = colors->sticky;
		} else {
			color = colors->directory;
		}

		break;

	case BFTW_LNK:
		if (colors->orphan && is_link_broken(ftwbuf)) {
			color = colors->orphan;
		} else {
			color = colors->link;
		}
		break;

	case BFTW_BLK:
		color = colors->blockdev;
		break;
	case BFTW_CHR:
		color = colors->chardev;
		break;
	case BFTW_FIFO:
		color = colors->pipe;
		break;
	case BFTW_SOCK:
		color = colors->socket;
		break;
	case BFTW_DOOR:
		color = colors->door;
		break;

	default:
		break;
	}

	if (!color) {
		color = colors->normal;
	}

	return color;

error:
	if (colors->missing) {
		return colors->missing;
	} else {
		return colors->orphan;
	}
}

/** Print a fixed-length string. */
static int print_strn(const char *str, size_t len, FILE *file) {
	if (fwrite(str, 1, len, file) == len) {
		return 0;
	} else {
		return -1;
	}
}

/** Print a dstring. */
static int print_dstr(const char *str, FILE *file) {
	return print_strn(str, dstrlen(str), file);
}

/** Print an ANSI escape sequence. */
static int print_esc(const struct colors *colors, const char *esc, FILE *file) {
	if (print_dstr(colors->leftcode, file) != 0) {
		return -1;
	}
	if (print_dstr(esc, file) != 0) {
		return -1;
	}
	if (print_dstr(colors->rightcode, file) != 0) {
		return -1;
	}

	return 0;
}

/** Reset after an ANSI escape sequence. */
static int print_reset(const struct colors *colors, FILE *file) {
	if (colors->endcode) {
		return print_dstr(colors->endcode, file);
	} else {
		return print_esc(colors, colors->reset, file);
	}
}

/** Print a string with an optional color. */
static int print_colored(const struct colors *colors, const char *esc, const char *str, size_t len, FILE *file) {
	if (esc) {
		if (print_esc(colors, esc, file) != 0) {
			return -1;
		}
	}
	if (print_strn(str, len, file) != 0) {
		return -1;
	}
	if (esc) {
		if (print_reset(colors, file) != 0) {
			return -1;
		}
	}

	return 0;
}

/** Print a path with colors. */
static int print_path_colored(CFILE *cfile, const char *path, const struct BFTW *ftwbuf, enum bfs_stat_flag flags) {
	const struct colors *colors = cfile->colors;
	FILE *file = cfile->file;

	size_t nameoff;
	if (path == ftwbuf->path) {
		nameoff = ftwbuf->nameoff;
	} else {
		nameoff = xbasename(path) - path;
	}

	if (nameoff > 0) {
		if (print_colored(colors, colors->directory, path, nameoff, file) != 0) {
			return -1;
		}
	}

	const char *filename = path + nameoff;
	const char *color = file_color(colors, filename, ftwbuf, flags);
	return print_colored(colors, color, filename, strlen(filename), file);
}

/** Print the path to a file with the appropriate colors. */
static int print_path(CFILE *cfile, const struct BFTW *ftwbuf) {
	const struct colors *colors = cfile->colors;
	if (!colors) {
		return fputs(ftwbuf->path, cfile->file) == EOF ? -1 : 0;
	}

	enum bfs_stat_flag flags = ftwbuf->stat_flags;
	if (colors && colors->link_as_target && ftwbuf->typeflag == BFTW_LNK) {
		flags = BFS_STAT_TRYFOLLOW;
	}

	return print_path_colored(cfile, ftwbuf->path, ftwbuf, flags);
}

/** Print a link target with the appropriate colors. */
static int print_link_target(CFILE *cfile, const struct BFTW *ftwbuf) {
	int ret = -1;

	size_t len = 0;
	const struct bfs_stat *statbuf = bftw_stat(ftwbuf, BFS_STAT_NOFOLLOW);
	if (statbuf) {
		len = statbuf->size;
	}

	char *target = xreadlinkat(ftwbuf->at_fd, ftwbuf->at_path, len);
	if (!target) {
		goto done;
	}

	if (!cfile->colors) {
		ret = fputs(target, cfile->file) == EOF ? -1 : 0;
		goto done;
	}

	ret = print_path_colored(cfile, target, ftwbuf, BFS_STAT_FOLLOW);

done:
	free(target);
	return ret;
}

int cfprintf(CFILE *cfile, const char *format, ...) {
	va_list args;
	va_start(args, format);
	int ret = cvfprintf(cfile, format, args);
	va_end(args);
	return ret;
}

int cvfprintf(CFILE *cfile, const char *format, va_list args) {
	const struct colors *colors = cfile->colors;
	FILE *file = cfile->file;
	int error = errno;

	for (const char *i = format; *i; ++i) {
		size_t verbatim = strcspn(i, "%$");
		if (fwrite(i, 1, verbatim, file) != verbatim) {
			return -1;
		}

		i += verbatim;
		switch (*i) {
		case '%':
			switch (*++i) {
			case '%':
				if (fputc('%', file) == EOF) {
					return -1;
				}
				break;

			case 'c':
				if (fputc(va_arg(args, int), file) == EOF) {
					return -1;
				}
				break;

			case 'd':
				if (fprintf(file, "%d", va_arg(args, int)) < 0) {
					return -1;
				}
				break;

			case 'g':
				if (fprintf(file, "%g", va_arg(args, double)) < 0) {
					return -1;
				}
				break;

			case 's':
				if (fputs(va_arg(args, const char *), file) == EOF) {
					return -1;
				}
				break;

			case 'z':
				++i;
				if (*i != 'u') {
					goto invalid;
				}
				if (fprintf(file, "%zu", va_arg(args, size_t)) < 0) {
					return -1;
				}
				break;

			case 'm':
				if (fputs(strerror(error), file) == EOF) {
					return -1;
				}
				break;

			case 'p':
				switch (*++i) {
				case 'P':
					if (print_path(cfile, va_arg(args, const struct BFTW *)) != 0) {
						return -1;
					}
					break;

				case 'L':
					if (print_link_target(cfile, va_arg(args, const struct BFTW *)) != 0) {
						return -1;
					}
					break;

				default:
					goto invalid;
				}

				break;

			default:
				goto invalid;
			}
			break;

		case '$':
			switch (*++i) {
			case '$':
				if (fputc('$', file) == EOF) {
					return -1;
				}
				break;

			case '{': {
				++i;
				const char *end = strchr(i, '}');
				if (!end) {
					goto invalid;
				}
				if (!colors) {
					i = end;
					break;
				}

				size_t len = end - i;
				char name[len + 1];
				memcpy(name, i, len);
				name[len] = '\0';

				char **esc = get_color(colors, name);
				if (!esc) {
					goto invalid;
				}
				if (*esc) {
					if (print_esc(colors, *esc, file) != 0) {
						return -1;
					}
				}

				i = end;
				break;
			}

			default:
				goto invalid;
			}
			break;

		default:
			return 0;
		}

	}

	return 0;

invalid:
	assert(false);
	errno = EINVAL;
	return -1;
}
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2019 Tavian Barnes <tavianator@tavianator.com>             *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

void bfs_error(const struct cmdline *cmdline, const char *format, ...)  {
	va_list args;
	va_start(args, format);
	bfs_verror(cmdline, format, args);
	va_end(args);
}

void bfs_warning(const struct cmdline *cmdline, const char *format, ...)  {
	va_list args;
	va_start(args, format);
	bfs_vwarning(cmdline, format, args);
	va_end(args);
}

void bfs_verror(const struct cmdline *cmdline, const char *format, va_list args) {
	int error = errno;

	bfs_error_prefix(cmdline);

	errno = error;
	cvfprintf(cmdline->cerr, format, args);
}

void bfs_vwarning(const struct cmdline *cmdline, const char *format, va_list args) {
	int error = errno;

	bfs_warning_prefix(cmdline);

	errno = error;
	cvfprintf(cmdline->cerr, format, args);
}

void bfs_error_prefix(const struct cmdline *cmdline) {
	cfprintf(cmdline->cerr, "${bld}%s:${rs} ${er}error:${rs} ", xbasename(cmdline->argv[0]));
}

void bfs_warning_prefix(const struct cmdline *cmdline) {
	cfprintf(cmdline->cerr, "${bld}%s:${rs} ${wr}warning:${rs} ", xbasename(cmdline->argv[0]));
}
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2016-2019 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * The memory representation of a dynamic string.  Users get a pointer to data.
 */
struct dstring {
	size_t capacity;
	size_t length;
	char data[];
};

/** Get the string header from the string data pointer. */
static struct dstring *dstrheader(const char *dstr) {
	return (struct dstring *)(dstr - offsetof(struct dstring, data));
}

/** Get the correct size for a dstring with the given capacity. */
static size_t dstrsize(size_t capacity) {
	return sizeof(struct dstring) + capacity + 1;
}

/** Allocate a dstring with the given contents. */
static char *dstralloc_impl(size_t capacity, size_t length, const char *data) {
	struct dstring *header = malloc(dstrsize(capacity));
	if (!header) {
		return NULL;
	}

	header->capacity = capacity;
	header->length = length;
	return memcpy(header->data, data, length + 1);
}

char *dstralloc(size_t capacity) {
	return dstralloc_impl(capacity, 0, "");
}

char *dstrdup(const char *str) {
	size_t len = strlen(str);
	return dstralloc_impl(len, len, str);
}

size_t dstrlen(const char *dstr) {
	return dstrheader(dstr)->length;
}

int dstreserve(char **dstr, size_t capacity) {
	struct dstring *header = dstrheader(*dstr);

	if (capacity > header->capacity) {
		capacity *= 2;

		header = realloc(header, dstrsize(capacity));
		if (!header) {
			return -1;
		}
		header->capacity = capacity;

		*dstr = header->data;
	}

	return 0;
}

int dstresize(char **dstr, size_t length) {
	if (dstreserve(dstr, length) != 0) {
		return -1;
	}

	struct dstring *header = dstrheader(*dstr);
	header->length = length;
	header->data[length] = '\0';

	return 0;
}

/** Common implementation of dstr{cat,ncat,app}. */
static int dstrcat_impl(char **dest, const char *src, size_t srclen) {
	size_t oldlen = dstrlen(*dest);
	size_t newlen = oldlen + srclen;

	if (dstresize(dest, newlen) != 0) {
		return -1;
	}

	memcpy(*dest + oldlen, src, srclen);
	return 0;
}

int dstrcat(char **dest, const char *src) {
	return dstrcat_impl(dest, src, strlen(src));
}

int dstrncat(char **dest, const char *src, size_t n) {
	return dstrcat_impl(dest, src, strnlen(src, n));
}

int dstrapp(char **str, char c) {
	return dstrcat_impl(str, &c, 1);
}

char *dstrprintf(const char *format, ...) {
	va_list args;

	va_start(args, format);
	int len = vsnprintf(NULL, 0, format, args);
	va_end(args);

	assert(len > 0);

	char *str = dstralloc(len);
	if (!str) {
		return NULL;
	}

	va_start(args, format);
	len = vsnprintf(str, len + 1, format, args);
	va_end(args);

	struct dstring *header = dstrheader(str);
	assert(len == header->capacity);
	header->length = len;

	return str;
}

void dstrfree(char *dstr) {
	if (dstr) {
		free(dstrheader(dstr));
	}
}
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2015-2019 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

/**
 * Implementation of all the literal expressions.
 */

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <grp.h>
#include <pwd.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

struct eval_state {
	/** Data about the current file. */
	const struct BFTW *ftwbuf;
	/** The parsed command line. */
	const struct cmdline *cmdline;
	/** The bftw() callback return value. */
	enum bftw_action action;
	/** The eval_cmdline() return value. */
	int *ret;
	/** Whether to quit immediately. */
	bool quit;
};

/**
 * Print an error message.
 */
BFS_FORMATTER(2, 3)
static void eval_error(struct eval_state *state, const char *format, ...) {
	int error = errno;
	const struct cmdline *cmdline = state->cmdline;
	CFILE *cerr = cmdline->cerr;

	bfs_error(cmdline, "%pP: ", state->ftwbuf);

	va_list args;
	va_start(args, format);
	errno = error;
	cvfprintf(cerr, format, args);
	va_end(args);
}

/**
 * Check if an error should be ignored.
 */
static bool eval_should_ignore(const struct eval_state *state, int error) {
	return state->cmdline->ignore_races
		&& is_nonexistence_error(error)
		&& state->ftwbuf->depth > 0;
}

/**
 * Report an error that occurs during evaluation.
 */
static void eval_report_error(struct eval_state *state) {
	if (!eval_should_ignore(state, errno)) {
		eval_error(state, "%m.\n");
		*state->ret = EXIT_FAILURE;
	}
}

/**
 * Perform a bfs_stat() call if necessary.
 */
static const struct bfs_stat *eval_stat(struct eval_state *state) {
	const struct BFTW *ftwbuf = state->ftwbuf;
	const struct bfs_stat *ret = bftw_stat(ftwbuf, ftwbuf->stat_flags);
	if (!ret) {
		eval_report_error(state);
	}
	return ret;
}

/**
 * Get the difference (in seconds) between two struct timespecs.
 */
static time_t timespec_diff(const struct timespec *lhs, const struct timespec *rhs) {
	time_t ret = lhs->tv_sec - rhs->tv_sec;
	if (lhs->tv_nsec < rhs->tv_nsec) {
		--ret;
	}
	return ret;
}

bool expr_cmp(const struct expr *expr, long long n) {
	switch (expr->cmp_flag) {
	case CMP_EXACT:
		return n == expr->idata;
	case CMP_LESS:
		return n < expr->idata;
	case CMP_GREATER:
		return n > expr->idata;
	}

	return false;
}

/**
 * -true test.
 */
bool eval_true(const struct expr *expr, struct eval_state *state) {
	return true;
}

/**
 * -false test.
 */
bool eval_false(const struct expr *expr, struct eval_state *state) {
	return false;
}

/**
 * -executable, -readable, -writable tests.
 */
bool eval_access(const struct expr *expr, struct eval_state *state) {
	const struct BFTW *ftwbuf = state->ftwbuf;
	return xfaccessat(ftwbuf->at_fd, ftwbuf->at_path, expr->idata) == 0;
}

/**
 * -acl test.
 */
bool eval_acl(const struct expr *expr, struct eval_state *state) {
	int ret = bfs_check_acl(state->ftwbuf);
	if (ret >= 0) {
		return ret;
	} else {
		eval_report_error(state);
		return false;
	}
}

/**
 * -capable test.
 */
bool eval_capable(const struct expr *expr, struct eval_state *state) {
	int ret = bfs_check_capabilities(state->ftwbuf);
	if (ret >= 0) {
		return ret;
	} else {
		eval_report_error(state);
		return false;
	}
}

/**
 * Get the given timespec field out of a stat buffer.
 */
static const struct timespec *eval_stat_time(const struct bfs_stat *statbuf, enum bfs_stat_field field, struct eval_state *state) {
	const struct timespec *ret = bfs_stat_time(statbuf, field);
	if (!ret) {
		eval_error(state, "Couldn't get file %s: %m.\n", bfs_stat_field_name(field));
		*state->ret = EXIT_FAILURE;
	}
	return ret;
}

/**
 * -[aBcm]?newer tests.
 */
bool eval_newer(const struct expr *expr, struct eval_state *state) {
	const struct bfs_stat *statbuf = eval_stat(state);
	if (!statbuf) {
		return false;
	}

	const struct timespec *time = eval_stat_time(statbuf, expr->stat_field, state);
	if (!time) {
		return false;
	}

	return time->tv_sec > expr->reftime.tv_sec
		|| (time->tv_sec == expr->reftime.tv_sec && time->tv_nsec > expr->reftime.tv_nsec);
}

/**
 * -[aBcm]{min,time} tests.
 */
bool eval_time(const struct expr *expr, struct eval_state *state) {
	const struct bfs_stat *statbuf = eval_stat(state);
	if (!statbuf) {
		return false;
	}

	const struct timespec *time = eval_stat_time(statbuf, expr->stat_field, state);
	if (!time) {
		return false;
	}

	time_t diff = timespec_diff(&expr->reftime, time);
	switch (expr->time_unit) {
	case MINUTES:
		diff /= 60;
		break;
	case DAYS:
		diff /= 60*60*24;
		break;
	}

	return expr_cmp(expr, diff);
}

/**
 * -used test.
 */
bool eval_used(const struct expr *expr, struct eval_state *state) {
	const struct bfs_stat *statbuf = eval_stat(state);
	if (!statbuf) {
		return false;
	}

	const struct timespec *atime = eval_stat_time(statbuf, BFS_STAT_ATIME, state);
	const struct timespec *ctime = eval_stat_time(statbuf, BFS_STAT_CTIME, state);
	if (!atime || !ctime) {
		return false;
	}

	time_t diff = timespec_diff(atime, ctime);
	diff /= 60*60*24;
	return expr_cmp(expr, diff);
}

/**
 * -gid test.
 */
bool eval_gid(const struct expr *expr, struct eval_state *state) {
	const struct bfs_stat *statbuf = eval_stat(state);
	if (!statbuf) {
		return false;
	}

	return expr_cmp(expr, statbuf->gid);
}

/**
 * -uid test.
 */
bool eval_uid(const struct expr *expr, struct eval_state *state) {
	const struct bfs_stat *statbuf = eval_stat(state);
	if (!statbuf) {
		return false;
	}

	return expr_cmp(expr, statbuf->uid);
}

/**
 * -nogroup test.
 */
bool eval_nogroup(const struct expr *expr, struct eval_state *state) {
	const struct bfs_stat *statbuf = eval_stat(state);
	if (!statbuf) {
		return false;
	}

	errno = 0;
	if (getgrgid(statbuf->gid) == NULL) {
		if (errno == 0) {
			return true;
		} else {
			eval_report_error(state);
		}
	}

	return false;
}

/**
 * -nouser test.
 */
bool eval_nouser(const struct expr *expr, struct eval_state *state) {
	const struct bfs_stat *statbuf = eval_stat(state);
	if (!statbuf) {
		return false;
	}

	errno = 0;
	if (getpwuid(statbuf->uid) == NULL) {
		if (errno == 0) {
			return true;
		} else {
			eval_report_error(state);
		}
	}

	return false;
}

/**
 * -delete action.
 */
bool eval_delete(const struct expr *expr, struct eval_state *state) {
	const struct BFTW *ftwbuf = state->ftwbuf;

	// Don't try to delete the current directory
	if (strcmp(ftwbuf->path, ".") == 0) {
		return true;
	}

	int flag = 0;

	// We need to know the actual type of the path, not what it points to
	enum bftw_typeflag type = bftw_typeflag(ftwbuf, BFS_STAT_NOFOLLOW);
	if (type == BFTW_DIR) {
		flag |= AT_REMOVEDIR;
	} else if (type == BFTW_ERROR) {
		eval_report_error(state);
		return false;
	}

	if (unlinkat(ftwbuf->at_fd, ftwbuf->at_path, flag) != 0) {
		eval_report_error(state);
		return false;
	}

	return true;
}

/** Finish any pending -exec ... + operations. */
static int eval_exec_finish(const struct expr *expr, const struct cmdline *cmdline) {
	int ret = 0;
	if (expr->execbuf && bfs_exec_finish(expr->execbuf) != 0) {
		if (errno != 0) {
			bfs_error(cmdline, "%s %s: %m.\n", expr->argv[0], expr->argv[1]);
		}
		ret = -1;
	}
	if (expr->lhs && eval_exec_finish(expr->lhs, cmdline) != 0) {
		ret = -1;
	}
	if (expr->rhs && eval_exec_finish(expr->rhs, cmdline) != 0) {
		ret = -1;
	}
	return ret;
}

/**
 * -exec[dir]/-ok[dir] actions.
 */
bool eval_exec(const struct expr *expr, struct eval_state *state) {
	bool ret = bfs_exec(expr->execbuf, state->ftwbuf) == 0;
	if (errno != 0) {
		eval_error(state, "%s %s: %m.\n", expr->argv[0], expr->argv[1]);
		*state->ret = EXIT_FAILURE;
	}
	return ret;
}

/**
 * -exit action.
 */
bool eval_exit(const struct expr *expr, struct eval_state *state) {
	state->action = BFTW_STOP;
	*state->ret = expr->idata;
	state->quit = true;
	return true;
}

/**
 * -depth N test.
 */
bool eval_depth(const struct expr *expr, struct eval_state *state) {
	return expr_cmp(expr, state->ftwbuf->depth);
}

/**
 * -empty test.
 */
bool eval_empty(const struct expr *expr, struct eval_state *state) {
	bool ret = false;
	const struct BFTW *ftwbuf = state->ftwbuf;

	if (ftwbuf->typeflag == BFTW_DIR) {
		int dfd = openat(ftwbuf->at_fd, ftwbuf->at_path, O_RDONLY | O_CLOEXEC | O_DIRECTORY);
		if (dfd < 0) {
			eval_report_error(state);
			goto done;
		}

		DIR *dir = fdopendir(dfd);
		if (!dir) {
			eval_report_error(state);
			close(dfd);
			goto done;
		}

		struct dirent *de;
		if (xreaddir(dir, &de) == 0) {
			ret = !de;
		} else {
			eval_report_error(state);
		}

		closedir(dir);
	} else {
		const struct bfs_stat *statbuf = eval_stat(state);
		if (statbuf) {
			ret = statbuf->size == 0;
		}
	}

done:
	return ret;
}

/**
 * -fstype test.
 */
bool eval_fstype(const struct expr *expr, struct eval_state *state) {
	const struct bfs_stat *statbuf = eval_stat(state);
	if (!statbuf) {
		return false;
	}

	const char *type = bfs_fstype(state->cmdline->mtab, statbuf);
	return strcmp(type, expr->sdata) == 0;
}

/**
 * -hidden test.
 */
bool eval_hidden(const struct expr *expr, struct eval_state *state) {
	const struct BFTW *ftwbuf = state->ftwbuf;
	return ftwbuf->nameoff > 0 && ftwbuf->path[ftwbuf->nameoff] == '.';
}

/**
 * -nohidden action.
 */
bool eval_nohidden(const struct expr *expr, struct eval_state *state) {
	if (eval_hidden(expr, state)) {
		eval_prune(expr, state);
		return false;
	} else {
		return true;
	}
}

/**
 * -inum test.
 */
bool eval_inum(const struct expr *expr, struct eval_state *state) {
	const struct bfs_stat *statbuf = eval_stat(state);
	if (!statbuf) {
		return false;
	}

	return expr_cmp(expr, statbuf->ino);
}

/**
 * -links test.
 */
bool eval_links(const struct expr *expr, struct eval_state *state) {
	const struct bfs_stat *statbuf = eval_stat(state);
	if (!statbuf) {
		return false;
	}

	return expr_cmp(expr, statbuf->nlink);
}

/**
 * -i?lname test.
 */
bool eval_lname(const struct expr *expr, struct eval_state *state) {
	bool ret = false;
	char *name = NULL;

	const struct BFTW *ftwbuf = state->ftwbuf;
	if (ftwbuf->typeflag != BFTW_LNK) {
		goto done;
	}

	const struct bfs_stat *statbuf = eval_stat(state);
	if (!statbuf) {
		goto done;
	}

	name = xreadlinkat(ftwbuf->at_fd, ftwbuf->at_path, statbuf->size);
	if (!name) {
		eval_report_error(state);
		goto done;
	}

	ret = fnmatch(expr->sdata, name, expr->idata) == 0;

done:
	free(name);
	return ret;
}

/**
 * -i?name test.
 */
bool eval_name(const struct expr *expr, struct eval_state *state) {
	const struct BFTW *ftwbuf = state->ftwbuf;

	const char *name = ftwbuf->path + ftwbuf->nameoff;
	char *copy = NULL;
	if (ftwbuf->depth == 0) {
		// Any trailing slashes are not part of the name.  This can only
		// happen for the root path.
		const char *slash = strchr(name, '/');
		if (slash && slash > name) {
			copy = strndup(name, slash - name);
			if (!copy) {
				eval_report_error(state);
				return false;
			}
			name = copy;
		}
	}

	bool ret = fnmatch(expr->sdata, name, expr->idata) == 0;
	free(copy);
	return ret;
}

/**
 * -i?path test.
 */
bool eval_path(const struct expr *expr, struct eval_state *state) {
	const struct BFTW *ftwbuf = state->ftwbuf;
	return fnmatch(expr->sdata, ftwbuf->path, expr->idata) == 0;
}

/**
 * -perm test.
 */
bool eval_perm(const struct expr *expr, struct eval_state *state) {
	const struct bfs_stat *statbuf = eval_stat(state);
	if (!statbuf) {
		return false;
	}

	mode_t mode = statbuf->mode;
	mode_t target;
	if (state->ftwbuf->typeflag == BFTW_DIR) {
		target = expr->dir_mode;
	} else {
		target = expr->file_mode;
	}

	switch (expr->mode_cmp) {
	case MODE_EXACT:
		return (mode & 07777) == target;

	case MODE_ALL:
		return (mode & target) == target;

	case MODE_ANY:
		return !(mode & target) == !target;
	}

	return false;
}

/**
 * -f?ls action.
 */
bool eval_fls(const struct expr *expr, struct eval_state *state) {
	CFILE *cfile = expr->cfile;
	FILE *file = cfile->file;
	const struct BFTW *ftwbuf = state->ftwbuf;
	const struct bfs_stat *statbuf = eval_stat(state);
	if (!statbuf) {
		goto done;
	}

	uintmax_t ino = statbuf->ino;
	uintmax_t blocks = ((uintmax_t)statbuf->blocks*BFS_STAT_BLKSIZE + 1023)/1024;
	char mode[11];
	format_mode(statbuf->mode, mode);
	uintmax_t nlink = statbuf->nlink;
	if (fprintf(file, "%9ju %6ju %s %3ju ", ino, blocks, mode, nlink) < 0) {
		goto error;
	}

	uintmax_t uid = statbuf->uid;
	struct passwd *pwd = getpwuid(uid);
	if (pwd) {
		if (fprintf(file, " %-8s", pwd->pw_name) < 0) {
			goto error;
		}
	} else {
		if (fprintf(file, " %-8ju", uid) < 0) {
			goto error;
		}
	}

	uintmax_t gid = statbuf->gid;
	struct group *grp = getgrgid(gid);
	if (grp) {
		if (fprintf(file, " %-8s", grp->gr_name) < 0) {
			goto error;
		}
	} else {
		if (fprintf(file, " %-8ju", gid) < 0) {
			goto error;
		}
	}

	if (ftwbuf->typeflag & (BFTW_BLK | BFTW_CHR)) {
		int ma = bfs_major(statbuf->rdev);
		int mi = bfs_minor(statbuf->rdev);
		if (fprintf(file, " %3d, %3d", ma, mi) < 0) {
			goto error;
		}
	} else {
		uintmax_t size = statbuf->size;
		if (fprintf(file, " %8ju", size) < 0) {
			goto error;
		}
	}

	time_t time = statbuf->mtime.tv_sec;
	time_t now = expr->reftime.tv_sec;
	time_t six_months_ago = now - 6*30*24*60*60;
	time_t tomorrow = now + 24*60*60;
	struct tm tm;
	if (xlocaltime(&time, &tm) != 0) {
		goto error;
	}
	char time_str[256];
	const char *time_format = "%b %e %H:%M";
	if (time <= six_months_ago || time >= tomorrow) {
		time_format = "%b %e  %Y";
	}
	if (!strftime(time_str, sizeof(time_str), time_format, &tm)) {
		errno = EOVERFLOW;
		goto error;
	}
	if (fprintf(file, " %s", time_str) < 0) {
		goto error;
	}

	if (cfprintf(cfile, " %pP", ftwbuf) < 0) {
		goto error;
	}

	if (ftwbuf->typeflag == BFTW_LNK) {
		if (cfprintf(cfile, " -> %pL", ftwbuf) < 0) {
			goto error;
		}
	}

	if (fputc('\n', file) == EOF) {
		goto error;
	}

done:
	return true;

error:
	eval_report_error(state);
	return true;
}

/**
 * -f?print action.
 */
bool eval_fprint(const struct expr *expr, struct eval_state *state) {
	if (cfprintf(expr->cfile, "%pP\n", state->ftwbuf) < 0) {
		eval_report_error(state);
	}
	return true;
}

/**
 * -f?print0 action.
 */
bool eval_fprint0(const struct expr *expr, struct eval_state *state) {
	const char *path = state->ftwbuf->path;
	size_t length = strlen(path) + 1;
	if (fwrite(path, 1, length, expr->cfile->file) != length) {
		eval_report_error(state);
	}
	return true;
}

/**
 * -f?printf action.
 */
bool eval_fprintf(const struct expr *expr, struct eval_state *state) {
	if (bfs_printf(expr->cfile->file, expr->printf, state->ftwbuf) != 0) {
		eval_report_error(state);
	}

	return true;
}

/**
 * -printx action.
 */
bool eval_fprintx(const struct expr *expr, struct eval_state *state) {
	FILE *file = expr->cfile->file;
	const char *path = state->ftwbuf->path;

	while (true) {
		size_t span = strcspn(path, " \t\n\\$'\"`");
		if (fwrite(path, 1, span, file) != span) {
			goto error;
		}
		path += span;

		char c = path[0];
		if (!c) {
			break;
		}

		char escaped[] = {'\\', c};
		if (fwrite(escaped, 1, sizeof(escaped), file) != sizeof(escaped)) {
			goto error;
		}
		++path;
	}


	if (fputc('\n', file) == EOF) {
		goto error;
	}

	return true;

error:
	eval_report_error(state);
	return true;
}

/**
 * -prune action.
 */
bool eval_prune(const struct expr *expr, struct eval_state *state) {
	state->action = BFTW_PRUNE;
	return true;
}

/**
 * -quit action.
 */
bool eval_quit(const struct expr *expr, struct eval_state *state) {
	state->action = BFTW_STOP;
	state->quit = true;
	return true;
}

/**
 * -i?regex test.
 */
bool eval_regex(const struct expr *expr, struct eval_state *state) {
	const char *path = state->ftwbuf->path;
	size_t len = strlen(path);
	regmatch_t match = {
		.rm_so = 0,
		.rm_eo = len,
	};

	int flags = 0;
#ifdef REG_STARTEND
	flags |= REG_STARTEND;
#endif
	int err = regexec(expr->regex, path, 1, &match, flags);
	if (err == 0) {
		return match.rm_so == 0 && match.rm_eo == len;
	} else if (err != REG_NOMATCH) {
		char *str = xregerror(err, expr->regex);
		if (str) {
			eval_error(state, "%s.\n", str);
			free(str);
		} else {
			perror("xregerror()");
		}

		*state->ret = EXIT_FAILURE;
	}

	return false;
}

/**
 * -samefile test.
 */
bool eval_samefile(const struct expr *expr, struct eval_state *state) {
	const struct bfs_stat *statbuf = eval_stat(state);
	if (!statbuf) {
		return false;
	}

	return statbuf->dev == expr->dev && statbuf->ino == expr->ino;
}

/**
 * -size test.
 */
bool eval_size(const struct expr *expr, struct eval_state *state) {
	const struct bfs_stat *statbuf = eval_stat(state);
	if (!statbuf) {
		return false;
	}

	static const off_t scales[] = {
		[SIZE_BLOCKS] = 512,
		[SIZE_BYTES] = 1,
		[SIZE_WORDS] = 2,
		[SIZE_KB] = 1024,
		[SIZE_MB] = 1024LL*1024,
		[SIZE_GB] = 1024LL*1024*1024,
		[SIZE_TB] = 1024LL*1024*1024*1024,
		[SIZE_PB] = 1024LL*1024*1024*1024*1024,
	};

	off_t scale = scales[expr->size_unit];
	off_t size = (statbuf->size + scale - 1)/scale; // Round up
	return expr_cmp(expr, size);
}

/**
 * -sparse test.
 */
bool eval_sparse(const struct expr *expr, struct eval_state *state) {
	const struct bfs_stat *statbuf = eval_stat(state);
	if (!statbuf) {
		return false;
	}

	blkcnt_t expected = (statbuf->size + BFS_STAT_BLKSIZE - 1)/BFS_STAT_BLKSIZE;
	return statbuf->blocks < expected;
}

/**
 * -type test.
 */
bool eval_type(const struct expr *expr, struct eval_state *state) {
	return state->ftwbuf->typeflag & expr->idata;
}

/**
 * -xattr test.
 */
bool eval_xattr(const struct expr *expr, struct eval_state *state) {
	int ret = bfs_check_xattrs(state->ftwbuf);
	if (ret >= 0) {
		return ret;
	} else {
		eval_report_error(state);
		return false;
	}
}

/**
 * -xtype test.
 */
bool eval_xtype(const struct expr *expr, struct eval_state *state) {
	const struct BFTW *ftwbuf = state->ftwbuf;
	enum bfs_stat_flag flags = ftwbuf->stat_flags ^ (BFS_STAT_NOFOLLOW | BFS_STAT_TRYFOLLOW);
	enum bftw_typeflag type = bftw_typeflag(ftwbuf, flags);
	if (type == BFTW_ERROR) {
		eval_report_error(state);
		return false;
	} else {
		return type & expr->idata;
	}
}

#if _POSIX_MONOTONIC_CLOCK > 0
#	define BFS_CLOCK CLOCK_MONOTONIC
#elif _POSIX_TIMERS > 0
#	define BFS_CLOCK CLOCK_REALTIME
#endif

/**
 * Call clock_gettime(), if available.
 */
static int eval_gettime(struct timespec *ts) {
#ifdef BFS_CLOCK
	int ret = clock_gettime(BFS_CLOCK, ts);
	if (ret != 0) {
		perror("clock_gettime()");
	}
	return ret;
#else
	return -1;
#endif
}

/**
 * Record the time that elapsed evaluating an expression.
 */
static void add_elapsed(struct expr *expr, const struct timespec *start, const struct timespec *end) {
	expr->elapsed.tv_sec += end->tv_sec - start->tv_sec;
	expr->elapsed.tv_nsec += end->tv_nsec - start->tv_nsec;
	if (expr->elapsed.tv_nsec < 0) {
		expr->elapsed.tv_nsec += 1000000000L;
		--expr->elapsed.tv_sec;
	} else if (expr->elapsed.tv_nsec >= 1000000000L) {
		expr->elapsed.tv_nsec -= 1000000000L;
		++expr->elapsed.tv_sec;
	}
}

/**
 * Evaluate an expression.
 */
static bool eval_expr(struct expr *expr, struct eval_state *state) {
	struct timespec start, end;
	bool time = state->cmdline->debug & DEBUG_RATES;
	if (time) {
		if (eval_gettime(&start) != 0) {
			time = false;
		}
	}

	assert(!state->quit);

	bool ret = expr->eval(expr, state);

	if (time) {
		if (eval_gettime(&end) == 0) {
			add_elapsed(expr, &start, &end);
		}
	}

	++expr->evaluations;
	if (ret) {
		++expr->successes;
	}

	if (expr_never_returns(expr)) {
		assert(state->quit);
	} else if (!state->quit) {
		assert(!expr->always_true || ret);
		assert(!expr->always_false || !ret);
	}

	return ret;
}

/**
 * Evaluate a negation.
 */
bool eval_not(const struct expr *expr, struct eval_state *state) {
	return !eval_expr(expr->rhs, state);
}

/**
 * Evaluate a conjunction.
 */
bool eval_and(const struct expr *expr, struct eval_state *state) {
	if (!eval_expr(expr->lhs, state)) {
		return false;
	}

	if (state->quit) {
		return false;
	}

	return eval_expr(expr->rhs, state);
}

/**
 * Evaluate a disjunction.
 */
bool eval_or(const struct expr *expr, struct eval_state *state) {
	if (eval_expr(expr->lhs, state)) {
		return true;
	}

	if (state->quit) {
		return false;
	}

	return eval_expr(expr->rhs, state);
}

/**
 * Evaluate the comma operator.
 */
bool eval_comma(const struct expr *expr, struct eval_state *state) {
	eval_expr(expr->lhs, state);

	if (state->quit) {
		return false;
	}

	return eval_expr(expr->rhs, state);
}

/** Check if we've seen a file before. */
static bool eval_file_unique(struct eval_state *state, struct trie *seen) {
	const struct bfs_stat *statbuf = eval_stat(state);
	if (!statbuf) {
		return false;
	}

	bfs_file_id id;
	bfs_stat_id(statbuf, &id);

	struct trie_leaf *leaf = trie_insert_mem(seen, id, sizeof(id));
	if (!leaf) {
		eval_report_error(state);
		return false;
	}

	if (leaf->value) {
		state->action = BFTW_PRUNE;
		return false;
	} else {
		leaf->value = leaf;
		return true;
	}
}

#define DEBUG_FLAG(flags, flag)				\
	do {						\
		if ((flags & flag) || flags == flag) {	\
			fputs(#flag, stderr);		\
			flags ^= flag;			\
			if (flags) {			\
				fputs(" | ", stderr);	\
			}				\
		}					\
	} while (0)

/**
 * Log a stat() call.
 */
static void debug_stat(const struct BFTW *ftwbuf, const struct bftw_stat *cache, enum bfs_stat_flag flags) {
	fprintf(stderr, "bfs_stat(");
	if (ftwbuf->at_fd == AT_FDCWD) {
		fprintf(stderr, "AT_FDCWD");
	} else {
		size_t baselen = strlen(ftwbuf->path) - strlen(ftwbuf->at_path);
		fprintf(stderr, "\"");
		fwrite(ftwbuf->path, 1, baselen, stderr);
		fprintf(stderr, "\"");
	}

	fprintf(stderr, ", \"%s\", ", ftwbuf->at_path);

	DEBUG_FLAG(flags, BFS_STAT_FOLLOW);
	DEBUG_FLAG(flags, BFS_STAT_NOFOLLOW);
	DEBUG_FLAG(flags, BFS_STAT_TRYFOLLOW);

	fprintf(stderr, ") == %d", cache->buf ? 0 : -1);

	if (cache->error) {
		fprintf(stderr, " [%d]", cache->error);
	}

	fprintf(stderr, "\n");
}

/**
 * Log any stat() calls that happened.
 */
static void debug_stats(const struct BFTW *ftwbuf) {
	const struct bfs_stat *statbuf = ftwbuf->stat_cache.buf;
	if (statbuf || ftwbuf->stat_cache.error) {
		debug_stat(ftwbuf, &ftwbuf->stat_cache, BFS_STAT_FOLLOW);
	}

	const struct bfs_stat *lstatbuf = ftwbuf->lstat_cache.buf;
	if ((lstatbuf && lstatbuf != statbuf) || ftwbuf->lstat_cache.error) {
		debug_stat(ftwbuf, &ftwbuf->lstat_cache, BFS_STAT_NOFOLLOW);
	}
}

/**
 * Dump the bftw_typeflag for -D search.
 */
static const char *dump_bftw_typeflag(enum bftw_typeflag type) {
#define DUMP_BFTW_TYPEFLAG_CASE(flag)		\
	case flag:				\
		return #flag

	switch (type) {
		DUMP_BFTW_TYPEFLAG_CASE(BFTW_BLK);
		DUMP_BFTW_TYPEFLAG_CASE(BFTW_CHR);
		DUMP_BFTW_TYPEFLAG_CASE(BFTW_DIR);
		DUMP_BFTW_TYPEFLAG_CASE(BFTW_DOOR);
		DUMP_BFTW_TYPEFLAG_CASE(BFTW_FIFO);
		DUMP_BFTW_TYPEFLAG_CASE(BFTW_LNK);
		DUMP_BFTW_TYPEFLAG_CASE(BFTW_PORT);
		DUMP_BFTW_TYPEFLAG_CASE(BFTW_REG);
		DUMP_BFTW_TYPEFLAG_CASE(BFTW_SOCK);
		DUMP_BFTW_TYPEFLAG_CASE(BFTW_WHT);

		DUMP_BFTW_TYPEFLAG_CASE(BFTW_ERROR);

	default:
		DUMP_BFTW_TYPEFLAG_CASE(BFTW_UNKNOWN);
	}
}

#define DUMP_BFTW_MAP(value) [value] = #value

/**
 * Dump the bftw_visit for -D search.
 */
static const char *dump_bftw_visit(enum bftw_visit visit) {
	static const char *visits[] = {
		DUMP_BFTW_MAP(BFTW_PRE),
		DUMP_BFTW_MAP(BFTW_POST),
	};
	return visits[visit];
}

/**
 * Dump the bftw_action for -D search.
 */
static const char *dump_bftw_action(enum bftw_action action) {
	static const char *actions[] = {
		DUMP_BFTW_MAP(BFTW_CONTINUE),
		DUMP_BFTW_MAP(BFTW_PRUNE),
		DUMP_BFTW_MAP(BFTW_STOP),
	};
	return actions[action];
}

/**
 * Type passed as the argument to the bftw() callback.
 */
struct callback_args {
	/** The parsed command line. */
	const struct cmdline *cmdline;
	/** The set of seen files. */
	struct trie *seen;
	/** Eventual return value from eval_cmdline(). */
	int ret;
};

/**
 * bftw() callback.
 */
static enum bftw_action cmdline_callback(const struct BFTW *ftwbuf, void *ptr) {
	struct callback_args *args = ptr;

	const struct cmdline *cmdline = args->cmdline;

	struct eval_state state;
	state.ftwbuf = ftwbuf;
	state.cmdline = cmdline;
	state.action = BFTW_CONTINUE;
	state.ret = &args->ret;
	state.quit = false;

	if (ftwbuf->typeflag == BFTW_ERROR) {
		if (!eval_should_ignore(&state, ftwbuf->error)) {
			args->ret = EXIT_FAILURE;
			eval_error(&state, "%s.\n", strerror(ftwbuf->error));
		}
		state.action = BFTW_PRUNE;
		goto done;
	}

	if (cmdline->unique && ftwbuf->visit == BFTW_PRE) {
		if (!eval_file_unique(&state, args->seen)) {
			goto done;
		}
	}

	if (cmdline->xargs_safe && strpbrk(ftwbuf->path, " \t\n\'\"\\")) {
		args->ret = EXIT_FAILURE;
		eval_error(&state, "Path is not safe for xargs.\n");
		state.action = BFTW_PRUNE;
		goto done;
	}

	if (cmdline->maxdepth < 0 || ftwbuf->depth >= cmdline->maxdepth) {
		state.action = BFTW_PRUNE;
	}

	// In -depth mode, only handle directories on the BFTW_POST visit
	enum bftw_visit expected_visit = BFTW_PRE;
	if ((cmdline->flags & BFTW_DEPTH)
	    && (cmdline->strategy == BFTW_IDS || ftwbuf->typeflag == BFTW_DIR)
	    && ftwbuf->depth < cmdline->maxdepth) {
		expected_visit = BFTW_POST;
	}

	if (ftwbuf->visit == expected_visit
	    && ftwbuf->depth >= cmdline->mindepth
	    && ftwbuf->depth <= cmdline->maxdepth) {
		eval_expr(cmdline->expr, &state);
	}

done:
	if (cmdline->debug & DEBUG_STAT) {
		debug_stats(ftwbuf);
	}

	if (cmdline->debug & DEBUG_SEARCH) {
		fprintf(stderr, "cmdline_callback({\n");
		fprintf(stderr, "\t.path = \"%s\",\n", ftwbuf->path);
		fprintf(stderr, "\t.root = \"%s\",\n", ftwbuf->root);
		fprintf(stderr, "\t.depth = %zu,\n", ftwbuf->depth);
		fprintf(stderr, "\t.visit = %s,\n", dump_bftw_visit(ftwbuf->visit));
		fprintf(stderr, "\t.typeflag = %s,\n", dump_bftw_typeflag(ftwbuf->typeflag));
		fprintf(stderr, "\t.error = %d,\n", ftwbuf->error);
		fprintf(stderr, "}) == %s\n", dump_bftw_action(state.action));
	}

	return state.action;
}

/**
 * Infer the number of open file descriptors we're allowed to have.
 */
static int infer_fdlimit(const struct cmdline *cmdline) {
	int ret = 4096;

	struct rlimit rl;
	if (getrlimit(RLIMIT_NOFILE, &rl) == 0) {
		if (rl.rlim_cur != RLIM_INFINITY) {
			ret = rl.rlim_cur;
		}
	}

	// 3 for std{in,out,err}
	int nopen = 3 + cmdline->nopen_files;

	// Check /proc/self/fd for the current number of open fds, if possible
	// (we may have inherited more than just the standard ones)
	DIR *dir = opendir("/proc/self/fd");
	if (!dir) {
		dir = opendir("/dev/fd");
	}
	if (dir) {
		// Account for 'dir' itself
		nopen = -1;

		struct dirent *de;
		while (xreaddir(dir, &de) == 0 && de) {
			++nopen;
		}

		closedir(dir);
	}

	ret -= nopen;
	ret -= cmdline->expr->persistent_fds;
	ret -= cmdline->expr->ephemeral_fds;

	// bftw() needs at least 2 available fds
	if (ret < 2) {
		ret = 2;
	}

	return ret;
}

/**
 * Dump the bftw() flags for -D search.
 */
static void dump_bftw_flags(enum bftw_flags flags) {
	DEBUG_FLAG(flags, 0);
	DEBUG_FLAG(flags, BFTW_STAT);
	DEBUG_FLAG(flags, BFTW_RECOVER);
	DEBUG_FLAG(flags, BFTW_DEPTH);
	DEBUG_FLAG(flags, BFTW_COMFOLLOW);
	DEBUG_FLAG(flags, BFTW_LOGICAL);
	DEBUG_FLAG(flags, BFTW_DETECT_CYCLES);
	DEBUG_FLAG(flags, BFTW_XDEV);

	assert(!flags);
}

/**
 * Dump the bftw_strategy for -D search.
 */
static const char *dump_bftw_strategy(enum bftw_strategy strategy) {
	static const char *strategies[] = {
		DUMP_BFTW_MAP(BFTW_BFS),
		DUMP_BFTW_MAP(BFTW_DFS),
		DUMP_BFTW_MAP(BFTW_IDS),
	};
	return strategies[strategy];
}

/**
 * Evaluate the command line.
 */
int eval_cmdline(const struct cmdline *cmdline) {
	if (!cmdline->expr) {
		return EXIT_SUCCESS;
	}

	struct callback_args args = {
		.cmdline = cmdline,
		.ret = EXIT_SUCCESS,
	};

	struct trie seen;
	if (cmdline->unique) {
		trie_init(&seen);
		args.seen = &seen;
	}

	struct bftw_args bftw_args = {
		.paths = cmdline->paths,
		.npaths = cmdline->npaths,
		.callback = cmdline_callback,
		.ptr = &args,
		.nopenfd = infer_fdlimit(cmdline),
		.flags = cmdline->flags,
		.strategy = cmdline->strategy,
		.mtab = cmdline->mtab,
	};

	if (cmdline->debug & DEBUG_SEARCH) {
		fprintf(stderr, "bftw({\n");
		fprintf(stderr, "\t.paths = {\n");
		for (size_t i = 0; i < bftw_args.npaths; ++i) {
			fprintf(stderr, "\t\t\"%s\",\n", bftw_args.paths[i]);
		}
		fprintf(stderr, "\t},\n");
		fprintf(stderr, "\t.npaths = %zu,\n", bftw_args.npaths);
		fprintf(stderr, "\t.callback = cmdline_callback,\n");
		fprintf(stderr, "\t.ptr = &args,\n");
		fprintf(stderr, "\t.nopenfd = %d,\n", bftw_args.nopenfd);
		fprintf(stderr, "\t.flags = ");
		dump_bftw_flags(bftw_args.flags);
		fprintf(stderr, ",\n\t.strategy = %s,\n", dump_bftw_strategy(bftw_args.strategy));
		fprintf(stderr, "\t.mtab = ");
		if (bftw_args.mtab) {
			fprintf(stderr, "cmdline->mtab");
		} else {
			fprintf(stderr, "NULL");
		}
		fprintf(stderr, ",\n})\n");
	}

	if (bftw(&bftw_args) != 0) {
		args.ret = EXIT_FAILURE;
		perror("bftw()");
	}

	if (eval_exec_finish(cmdline->expr, cmdline) != 0) {
		args.ret = EXIT_FAILURE;
	}

	if (cmdline->debug & DEBUG_RATES) {
		dump_cmdline(cmdline, true);
	}

	if (cmdline->unique) {
		trie_destroy(&seen);
	}

	return args.ret;
}
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2017-2018 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/** Print some debugging info. */
static void bfs_exec_debug(const struct bfs_exec *execbuf, const char *format, ...) {
	if (!(execbuf->flags & BFS_EXEC_DEBUG)) {
		return;
	}

	if (execbuf->flags & BFS_EXEC_CONFIRM) {
		fputs("-ok", stderr);
	} else {
		fputs("-exec", stderr);
	}
	if (execbuf->flags & BFS_EXEC_CHDIR) {
		fputs("dir", stderr);
	}
	fputs(": ", stderr);

	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
}

extern char **environ;

/** Determine the size of a single argument, for comparison to arg_max. */
static size_t bfs_exec_arg_size(const char *arg) {
	return sizeof(arg) + strlen(arg) + 1;
}

/** Even if we can pass a bigger argument list, cap it here. */
#define BFS_EXEC_ARG_MAX (16*1024*1024)

/** Determine the maximum argv size. */
static size_t bfs_exec_arg_max(const struct bfs_exec *execbuf) {
	long arg_max = sysconf(_SC_ARG_MAX);
	bfs_exec_debug(execbuf, "ARG_MAX: %ld according to sysconf()\n", arg_max);
	if (arg_max < 0) {
		arg_max = BFS_EXEC_ARG_MAX;
		bfs_exec_debug(execbuf, "ARG_MAX: %ld assumed\n", arg_max);
	}

	// We have to share space with the environment variables
	for (char **envp = environ; *envp; ++envp) {
		arg_max -= bfs_exec_arg_size(*envp);
	}
	// Account for the terminating NULL entry
	arg_max -= sizeof(char *);
	bfs_exec_debug(execbuf, "ARG_MAX: %ld remaining after environment variables\n", arg_max);

	// Account for the fixed arguments
	for (size_t i = 0; i < execbuf->tmpl_argc - 1; ++i) {
		arg_max -= bfs_exec_arg_size(execbuf->tmpl_argv[i]);
	}
	// Account for the terminating NULL entry
	arg_max -= sizeof(char *);
	bfs_exec_debug(execbuf, "ARG_MAX: %ld remaining after fixed arguments\n", arg_max);

	// Assume arguments are counted with the granularity of a single page,
	// so allow a one page cushion to account for rounding up
	long page_size = sysconf(_SC_PAGESIZE);
	if (page_size < 4096) {
		page_size = 4096;
	}
	arg_max -= page_size;
	bfs_exec_debug(execbuf, "ARG_MAX: %ld remaining after page cushion\n", arg_max);

	// POSIX recommends an additional 2048 bytes of headroom
	arg_max -= 2048;
	bfs_exec_debug(execbuf, "ARG_MAX: %ld remaining after headroom\n", arg_max);

	if (arg_max < 0) {
		arg_max = 0;
	} else if (arg_max > BFS_EXEC_ARG_MAX) {
		arg_max = BFS_EXEC_ARG_MAX;
	}

	bfs_exec_debug(execbuf, "ARG_MAX: %ld final value\n", arg_max);
	return arg_max;
}

struct bfs_exec *parse_bfs_exec(char **argv, enum bfs_exec_flags flags, const struct cmdline *cmdline) {
	struct bfs_exec *execbuf = malloc(sizeof(*execbuf));
	if (!execbuf) {
		perror("malloc()");
		goto fail;
	}

	execbuf->flags = flags;
	execbuf->argv = NULL;
	execbuf->argc = 0;
	execbuf->argv_cap = 0;
	execbuf->arg_size = 0;
	execbuf->arg_max = 0;
	execbuf->wd_fd = -1;
	execbuf->wd_path = NULL;
	execbuf->wd_len = 0;
	execbuf->ret = 0;

	if (cmdline->debug & DEBUG_EXEC) {
		execbuf->flags |= BFS_EXEC_DEBUG;
	}

	size_t i;
	for (i = 1; ; ++i) {
		const char *arg = argv[i];
		if (!arg) {
			if (execbuf->flags & BFS_EXEC_CONFIRM) {
				bfs_error(cmdline, "%s: Expected '... ;'.\n", argv[0]);
			} else {
				bfs_error(cmdline, "%s: Expected '... ;' or '... {} +'.\n", argv[0]);
			}
			goto fail;
		} else if (strcmp(arg, ";") == 0) {
			break;
		} else if (strcmp(arg, "+") == 0) {
			if (!(execbuf->flags & BFS_EXEC_CONFIRM) && strcmp(argv[i - 1], "{}") == 0) {
				execbuf->flags |= BFS_EXEC_MULTI;
				break;
			}
		}
	}

	execbuf->tmpl_argv = argv + 1;
	execbuf->tmpl_argc = i - 1;

	if (execbuf->tmpl_argc == 0) {
		bfs_error(cmdline, "%s: Missing command.\n", argv[0]);
		goto fail;
	}

	execbuf->argv_cap = execbuf->tmpl_argc + 1;
	execbuf->argv = malloc(execbuf->argv_cap*sizeof(*execbuf->argv));
	if (!execbuf->argv) {
		perror("malloc()");
		goto fail;
	}

	if (execbuf->flags & BFS_EXEC_MULTI) {
		for (i = 0; i < execbuf->tmpl_argc - 1; ++i) {
			char *arg = execbuf->tmpl_argv[i];
			if (strstr(arg, "{}")) {
				bfs_error(cmdline, "%s ... +: Only one '{}' is supported.\n", argv[0]);
				goto fail;
			}
			execbuf->argv[i] = arg;
		}
		execbuf->argc = execbuf->tmpl_argc - 1;

		execbuf->arg_max = bfs_exec_arg_max(execbuf);
	}

	return execbuf;

fail:
	free_bfs_exec(execbuf);
	return NULL;
}

/** Format the current path for use as a command line argument. */
static char *bfs_exec_format_path(const struct bfs_exec *execbuf, const struct BFTW *ftwbuf) {
	if (!(execbuf->flags & BFS_EXEC_CHDIR)) {
		return strdup(ftwbuf->path);
	}

	const char *name = ftwbuf->path + ftwbuf->nameoff;

	if (name[0] == '/') {
		// Must be a root path ("/", "//", etc.)
		return strdup(name);
	}

	// For compatibility with GNU find, use './name' instead of just 'name'
	char *path = malloc(2 + strlen(name) + 1);
	if (!path) {
		return NULL;
	}

	strcpy(path, "./");
	strcpy(path + 2, name);

	return path;
}

/** Format an argument, expanding "{}" to the current path. */
static char *bfs_exec_format_arg(char *arg, const char *path) {
	char *match = strstr(arg, "{}");
	if (!match) {
		return arg;
	}

	char *ret = dstralloc(0);
	if (!ret) {
		return NULL;
	}

	char *last = arg;
	do {
		if (dstrncat(&ret, last, match - last) != 0) {
			goto err;
		}
		if (dstrcat(&ret, path) != 0) {
			goto err;
		}

		last = match + 2;
		match = strstr(last, "{}");
	} while (match);

	if (dstrcat(&ret, last) != 0) {
		goto err;
	}

	return ret;

err:
	dstrfree(ret);
	return NULL;
}

/** Free a formatted argument. */
static void bfs_exec_free_arg(char *arg, const char *tmpl) {
	if (arg != tmpl) {
		dstrfree(arg);
	}
}

/** Open a file to use as the working directory. */
static int bfs_exec_openwd(struct bfs_exec *execbuf, const struct BFTW *ftwbuf) {
	assert(execbuf->wd_fd < 0);
	assert(!execbuf->wd_path);

	if (ftwbuf->at_fd != AT_FDCWD) {
		// Rely on at_fd being the immediate parent
		assert(ftwbuf->at_path == xbasename(ftwbuf->at_path));

		execbuf->wd_fd = ftwbuf->at_fd;
		if (!(execbuf->flags & BFS_EXEC_MULTI)) {
			return 0;
		}

		execbuf->wd_fd = dup_cloexec(execbuf->wd_fd);
		if (execbuf->wd_fd < 0) {
			return -1;
		}
	}

	execbuf->wd_len = ftwbuf->nameoff;
	if (execbuf->wd_len == 0) {
		if (ftwbuf->path[0] == '/') {
			++execbuf->wd_len;
		} else {
			// The path is something like "foo", so we're already in the right directory
			return 0;
		}
	}

	execbuf->wd_path = strndup(ftwbuf->path, execbuf->wd_len);
	if (!execbuf->wd_path) {
		return -1;
	}

	if (execbuf->wd_fd < 0) {
		execbuf->wd_fd = open(execbuf->wd_path, O_RDONLY | O_CLOEXEC | O_DIRECTORY);
	}

	if (execbuf->wd_fd < 0) {
		return -1;
	}

	return 0;
}

/** Close the working directory. */
static int bfs_exec_closewd(struct bfs_exec *execbuf, const struct BFTW *ftwbuf) {
	int ret = 0;

	if (execbuf->wd_fd >= 0) {
		if (!ftwbuf || execbuf->wd_fd != ftwbuf->at_fd) {
			ret = close(execbuf->wd_fd);
		}
		execbuf->wd_fd = -1;
	}

	if (execbuf->wd_path) {
		free(execbuf->wd_path);
		execbuf->wd_path = NULL;
		execbuf->wd_len = 0;
	}

	return ret;
}

/** Actually spawn the process. */
static int bfs_exec_spawn(const struct bfs_exec *execbuf) {
	if (execbuf->flags & BFS_EXEC_CONFIRM) {
		for (size_t i = 0; i < execbuf->argc; ++i) {
			fprintf(stderr, "%s ", execbuf->argv[i]);
		}
		fprintf(stderr, "? ");

		if (ynprompt() <= 0) {
			errno = 0;
			return -1;
		}
	}

	if (execbuf->flags & BFS_EXEC_MULTI) {
		bfs_exec_debug(execbuf, "Executing '%s' ... [%zu arguments] (size %zu)\n",
		               execbuf->argv[0], execbuf->argc - 1, execbuf->arg_size);
	} else {
		bfs_exec_debug(execbuf, "Executing '%s' ... [%zu arguments]\n", execbuf->argv[0], execbuf->argc - 1);
	}

	pid_t pid = -1;
	int error;

	struct bfs_spawn ctx;
	if (bfs_spawn_init(&ctx) != 0) {
		return -1;
	}

	if (bfs_spawn_setflags(&ctx, BFS_SPAWN_USEPATH) != 0) {
		goto fail;
	}

	if (execbuf->wd_fd >= 0) {
		if (bfs_spawn_addfchdir(&ctx, execbuf->wd_fd) != 0) {
			goto fail;
		}
	}

	pid = bfs_spawn(execbuf->argv[0], &ctx, execbuf->argv, environ);
fail:
	error = errno;
	bfs_spawn_destroy(&ctx);
	if (pid < 0) {
		errno = error;
		return -1;
	}

	int wstatus;
	if (waitpid(pid, &wstatus, 0) < 0) {
		return -1;
	}

	int ret = -1;

	if (WIFEXITED(wstatus)) {
		int status = WEXITSTATUS(wstatus);
		if (status == EXIT_SUCCESS) {
			ret = 0;
		} else {
			bfs_exec_debug(execbuf, "Command '%s' failed with status %d\n", execbuf->argv[0], status);
		}
	} else if (WIFSIGNALED(wstatus)) {
		int sig = WTERMSIG(wstatus);
		bfs_exec_debug(execbuf, "Command '%s' terminated by signal %d\n", execbuf->argv[0], sig);
	} else {
		bfs_exec_debug(execbuf, "Command '%s' terminated abnormally\n", execbuf->argv[0]);
	}

	errno = 0;
	return ret;
}

/** exec() a command for a single file. */
static int bfs_exec_single(struct bfs_exec *execbuf, const struct BFTW *ftwbuf) {
	int ret = -1, error = 0;

	char *path = bfs_exec_format_path(execbuf, ftwbuf);
	if (!path) {
		goto out;
	}

	size_t i;
	for (i = 0; i < execbuf->tmpl_argc; ++i) {
		execbuf->argv[i] = bfs_exec_format_arg(execbuf->tmpl_argv[i], path);
		if (!execbuf->argv[i]) {
			goto out_free;
		}
	}
	execbuf->argv[i] = NULL;
	execbuf->argc = i;

	if (execbuf->flags & BFS_EXEC_CHDIR) {
		if (bfs_exec_openwd(execbuf, ftwbuf) != 0) {
			goto out_free;
		}
	}

	ret = bfs_exec_spawn(execbuf);

out_free:
	error = errno;

	bfs_exec_closewd(execbuf, ftwbuf);

	for (size_t j = 0; j < i; ++j) {
		bfs_exec_free_arg(execbuf->argv[j], execbuf->tmpl_argv[j]);
	}

	free(path);

	errno = error;

out:
	return ret;
}

/** Check if any arguments remain in the buffer. */
static bool bfs_exec_args_remain(const struct bfs_exec *execbuf) {
	return execbuf->argc >= execbuf->tmpl_argc;
}

/** Execute the pending command from a BFS_EXEC_MULTI execbuf. */
static int bfs_exec_flush(struct bfs_exec *execbuf) {
	int ret = 0, error = 0;

	size_t orig_argc = execbuf->argc;
	while (bfs_exec_args_remain(execbuf)) {
		execbuf->argv[execbuf->argc] = NULL;
		ret = bfs_exec_spawn(execbuf);
		error = errno;
		if (ret == 0 || error != E2BIG) {
			break;
		}

		// Try to recover from E2BIG by trying fewer and fewer arguments
		// until they fit
		bfs_exec_debug(execbuf, "Got E2BIG, shrinking argument list...\n");
		execbuf->argv[execbuf->argc] = execbuf->argv[execbuf->argc - 1];
		execbuf->arg_size -= bfs_exec_arg_size(execbuf->argv[execbuf->argc]);
		--execbuf->argc;
	}
	size_t new_argc = execbuf->argc;
	size_t new_size = execbuf->arg_size;

	for (size_t i = execbuf->tmpl_argc - 1; i < new_argc; ++i) {
		free(execbuf->argv[i]);
	}
	execbuf->argc = execbuf->tmpl_argc - 1;
	execbuf->arg_size = 0;

	if (new_argc < orig_argc) {
		execbuf->arg_max = new_size;
		bfs_exec_debug(execbuf, "ARG_MAX: %zu\n", execbuf->arg_max);

		// If we recovered from E2BIG, there are unused arguments at the
		// end of the list
		for (size_t i = new_argc + 1; i <= orig_argc; ++i) {
			if (error == 0) {
				execbuf->argv[execbuf->argc] = execbuf->argv[i];
				execbuf->arg_size += bfs_exec_arg_size(execbuf->argv[execbuf->argc]);
				++execbuf->argc;
			} else {
				free(execbuf->argv[i]);
			}
		}
	}

	errno = error;
	return ret;
}

/** Check if we need to flush the execbuf because we're changing directories. */
static bool bfs_exec_changed_dirs(const struct bfs_exec *execbuf, const struct BFTW *ftwbuf) {
	if (execbuf->flags & BFS_EXEC_CHDIR) {
		if (ftwbuf->nameoff > execbuf->wd_len
		    || (execbuf->wd_path && strncmp(ftwbuf->path, execbuf->wd_path, execbuf->wd_len) != 0)) {
			bfs_exec_debug(execbuf, "Changed directories, executing buffered command\n");
			return true;
		}
	}

	return false;
}

/** Check if we need to flush the execbuf because we're too big. */
static bool bfs_exec_would_overflow(const struct bfs_exec *execbuf, const char *arg) {
	size_t next_size = execbuf->arg_size + bfs_exec_arg_size(arg);
	if (next_size > execbuf->arg_max) {
		bfs_exec_debug(execbuf, "Command size (%zu) would exceed maximum (%zu), executing buffered command\n",
		               next_size, execbuf->arg_max);
		return true;
	}

	return false;
}

/** Push a new argument to a BFS_EXEC_MULTI execbuf. */
static int bfs_exec_push(struct bfs_exec *execbuf, char *arg) {
	execbuf->argv[execbuf->argc] = arg;

	if (execbuf->argc + 1 >= execbuf->argv_cap) {
		size_t cap = 2*execbuf->argv_cap;
		char **argv = realloc(execbuf->argv, cap*sizeof(*argv));
		if (!argv) {
			return -1;
		}
		execbuf->argv = argv;
		execbuf->argv_cap = cap;
	}

	++execbuf->argc;
	execbuf->arg_size += bfs_exec_arg_size(arg);
	return 0;
}

/** Handle a new path for a BFS_EXEC_MULTI execbuf. */
static int bfs_exec_multi(struct bfs_exec *execbuf, const struct BFTW *ftwbuf) {
	int ret = 0;

	char *arg = bfs_exec_format_path(execbuf, ftwbuf);
	if (!arg) {
		ret = -1;
		goto out;
	}

	if (bfs_exec_changed_dirs(execbuf, ftwbuf)) {
		while (bfs_exec_args_remain(execbuf)) {
			ret |= bfs_exec_flush(execbuf);
		}
		bfs_exec_closewd(execbuf, ftwbuf);
	} else if (bfs_exec_would_overflow(execbuf, arg)) {
		ret |= bfs_exec_flush(execbuf);
	}

	if ((execbuf->flags & BFS_EXEC_CHDIR) && execbuf->wd_fd < 0) {
		if (bfs_exec_openwd(execbuf, ftwbuf) != 0) {
			ret = -1;
			goto out_arg;
		}
	}

	if (bfs_exec_push(execbuf, arg) != 0) {
		ret = -1;
		goto out_arg;
	}

	// arg will get cleaned up later by bfs_exec_flush()
	goto out;

out_arg:
	free(arg);
out:
	return ret;
}

int bfs_exec(struct bfs_exec *execbuf, const struct BFTW *ftwbuf) {
	if (execbuf->flags & BFS_EXEC_MULTI) {
		if (bfs_exec_multi(execbuf, ftwbuf) == 0) {
			errno = 0;
		} else {
			execbuf->ret = -1;
		}
		// -exec ... + never returns false
		return 0;
	} else {
		return bfs_exec_single(execbuf, ftwbuf);
	}
}

int bfs_exec_finish(struct bfs_exec *execbuf) {
	if (execbuf->flags & BFS_EXEC_MULTI) {
		bfs_exec_debug(execbuf, "Finishing execution, executing buffered command\n");
		while (bfs_exec_args_remain(execbuf)) {
			execbuf->ret |= bfs_exec_flush(execbuf);
		}
		if (execbuf->ret != 0) {
			bfs_exec_debug(execbuf, "One or more executions of '%s' failed\n", execbuf->argv[0]);
		}
	}
	return execbuf->ret;
}

void free_bfs_exec(struct bfs_exec *execbuf) {
	if (execbuf) {
		bfs_exec_closewd(execbuf, NULL);
		free(execbuf->argv);
		free(execbuf);
	}
}
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2019 Tavian Barnes <tavianator@tavianator.com>             *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

#include <assert.h>
#include <errno.h>
#include <unistd.h>

#if BFS_CAN_CHECK_ACL
#	include <sys/acl.h>
#endif

#if BFS_CAN_CHECK_CAPABILITIES
#	include <sys/capability.h>
#endif

#if BFS_CAN_CHECK_XATTRS
#	include <sys/xattr.h>
#endif

#if BFS_CAN_CHECK_ACL || BFS_CAN_CHECK_CAPABILITIES || BFS_CAN_CHECK_XATTRS

/**
 * Many of the APIs used here don't have *at() variants, but we can try to
 * emulate something similar if /proc/self/fd is available.
 */
static const char *fake_at(const struct BFTW *ftwbuf) {
	static bool proc_works = true;
	static bool proc_checked = false;

	char *path = NULL;
	if (!proc_works || ftwbuf->at_fd == AT_FDCWD) {
		goto fail;
	}

	path = dstrprintf("/proc/self/fd/%d/", ftwbuf->at_fd);
	if (!path) {
		goto fail;
	}

	if (!proc_checked) {
		proc_checked = true;
		if (xfaccessat(AT_FDCWD, path, F_OK) != 0) {
			proc_works = false;
			goto fail;
		}
	}

	if (dstrcat(&path, ftwbuf->at_path) != 0) {
		goto fail;
	}

	return path;

fail:
	dstrfree(path);
	return ftwbuf->path;
}

static void free_fake_at(const struct BFTW *ftwbuf, const char *path) {
	if (path != ftwbuf->path) {
		dstrfree((char *)path);
	}
}

/**
 * Check if an error was caused by the absence of support or data for a feature.
 */
static bool is_absence_error(int error) {
	// If the OS doesn't support the feature, it's obviously not enabled for
	// any files
	if (error == ENOTSUP) {
		return true;
	}

	// On Linux, ACLs and capabilities are implemented in terms of extended
	// attributes, which report ENODATA/ENOATTR when missing

#ifdef ENODATA
	if (error == ENODATA) {
		return true;
	}
#endif

#if defined(ENOATTR) && ENOATTR != ENODATA
	if (error == ENOATTR) {
		return true;
	}
#endif

#if __APPLE__
	// On macOS, ENOENT can also signal that a file has no ACLs
	if (error == ENOENT) {
		return true;
	}
#endif

	return false;
}

#endif // BFS_CAN_CHECK_ACL || BFS_CAN_CHECK_CAPABILITIES || BFS_CAN_CHECK_XATTRS

#if BFS_CAN_CHECK_ACL

/** Check if any ACLs of the given type are non-trivial. */
static int bfs_check_acl_type(const char *path, acl_type_t type) {
	acl_t acl = acl_get_file(path, type);
	if (!acl) {
		if (is_absence_error(errno)) {
			return 0;
		} else {
			return -1;
		}
	}

	int ret = 0;
	acl_entry_t entry;
	for (int status = acl_get_entry(acl, ACL_FIRST_ENTRY, &entry);
#if __APPLE__
	     // POSIX.1e specifies a return value of 1 for success, but macOS
	     // returns 0 instead
	     status == 0;
#else
	     status > 0;
#endif
	     status = acl_get_entry(acl, ACL_NEXT_ENTRY, &entry)) {
#if defined(ACL_USER_OBJ) && defined(ACL_GROUP_OBJ) && defined(ACL_OTHER)
		acl_tag_t tag;
		if (acl_get_tag_type(entry, &tag) != 0) {
			continue;
		}
		if (tag != ACL_USER_OBJ && tag != ACL_GROUP_OBJ && tag != ACL_OTHER) {
			ret = 1;
			break;
		}
#else
		ret = 1;
		break;
#endif
	}

	acl_free(acl);
	return ret;
}

int bfs_check_acl(const struct BFTW *ftwbuf) {
	if (ftwbuf->typeflag == BFTW_LNK) {
		return 0;
	}

	const char *path = fake_at(ftwbuf);

	int error = ENOTSUP;
	int ret = -1;

#if __APPLE__
	// macOS gives EINVAL for either of the two standard ACL types,
	// supporting only ACL_TYPE_EXTENDED
	if (ret <= 0) {
		ret = bfs_check_acl_type(path, ACL_TYPE_EXTENDED);
		if (ret < 0) {
			error = errno;
		}
	}
#else
	if (ret <= 0) {
		ret = bfs_check_acl_type(path, ACL_TYPE_ACCESS);
		if (ret < 0) {
			error = errno;
		}
	}

	if (ret <= 0 && ftwbuf->typeflag == BFTW_DIR) {
		ret = bfs_check_acl_type(path, ACL_TYPE_DEFAULT);
		if (ret < 0) {
			error = errno;
		}
	}
#endif

	free_fake_at(ftwbuf, path);
	errno = error;
	return ret;
}

#else // !BFS_CAN_CHECK_ACL

int bfs_check_acl(const struct BFTW *ftwbuf) {
	errno = ENOTSUP;
	return -1;
}

#endif

#if BFS_CAN_CHECK_CAPABILITIES

int bfs_check_capabilities(const struct BFTW *ftwbuf) {
	if (ftwbuf->typeflag == BFTW_LNK) {
		return 0;
	}

	int ret = -1, error;
	const char *path = fake_at(ftwbuf);

	cap_t caps = cap_get_file(path);
	if (!caps) {
		error = errno;
		if (is_absence_error(error)) {
			ret = 0;
		}
		goto out_path;
	}

	// TODO: Any better way to check for a non-empty capability set?
	char *text = cap_to_text(caps, NULL);
	if (!text) {
		error = errno;
		goto out_caps;
	}
	ret = text[0] ? 1 : 0;

	error = errno;
	cap_free(text);
out_caps:
	cap_free(caps);
out_path:
	free_fake_at(ftwbuf, path);
	errno = error;
	return ret;
}

#else // !BFS_CAN_CHECK_CAPABILITIES

int bfs_check_capabilities(const struct BFTW *ftwbuf) {
	errno = ENOTSUP;
	return -1;
}

#endif

#if BFS_CAN_CHECK_XATTRS

int bfs_check_xattrs(const struct BFTW *ftwbuf) {
	const char *path = fake_at(ftwbuf);
	ssize_t len;

#if __APPLE__
	int options = ftwbuf->typeflag == BFTW_LNK ? XATTR_NOFOLLOW : 0;
	len = listxattr(path, NULL, 0, options);
#else
	if (ftwbuf->typeflag == BFTW_LNK) {
		len = llistxattr(path, NULL, 0);
	} else {
		len = listxattr(path, NULL, 0);
	}
#endif

	int error = errno;

	free_fake_at(ftwbuf, path);

	if (len > 0) {
		return 1;
	} else if (len == 0 || is_absence_error(error)) {
		return 0;
	} else if (error == E2BIG) {
		return 1;
	} else {
		errno = error;
		return -1;
	}
}

#else // !BFS_CAN_CHECK_XATTRS

int bfs_check_xattrs(const struct BFTW *ftwbuf) {
	errno = ENOTSUP;
	return -1;
}

#endif
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2015-2019 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

/**
 * - main(): the entry point for bfs(1), a breadth-first version of find(1)
 *     - main.c        (this file)
 *
 * - parse_cmdline(): parses the command line into an expression tree
 *     - cmdline.h     (declares the parsed command line structure)
 *     - expr.h        (declares the expression tree nodes)
 *     - parse.c       (the parser itself)
 *     - opt.c         (the expression optimizer)
 *
 * - eval_cmdline(): runs the expression on every file it sees
 *     - eval.[ch]     (the main evaluation functions)
 *     - exec.[ch]     (implements -exec[dir]/-ok[dir])
 *     - printf.[ch]   (implements -[f]printf)
 *
 * - bftw(): used by eval_cmdline() to walk the directory tree(s)
 *     - bftw.[ch]     (an extended version of nftw(3))
 *
 * - Utilities:
 *     - bfs.h         (constants about bfs itself)
 *     - color.[ch]    (for pretty terminal colors)
 *     - diag.[ch]     (formats diagnostic messages)
 *     - dstring.[ch]  (a dynamic string library)
 *     - fsade.[ch]    (a facade over non-standard filesystem features)
 *     - mtab.[ch]     (parses the system's mount table)
 *     - spawn.[ch]    (spawns processes)
 *     - stat.[ch]     (wraps stat(), or statx() on Linux)
 *     - trie.[ch]     (a trie set/map implementation)
 *     - typo.[ch]     (fuzzy matching for typos)
 *     - util.[ch]     (everything else)
 */

#include <errno.h>
#include <fcntl.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Make sure the standard streams std{in,out,err} are open.  If they are not,
 * future open() calls may use those file descriptors, and std{in,out,err} will
 * use them unintentionally.
 */
static int open_std_streams(void) {
#ifdef O_PATH
	const int inflags = O_PATH, outflags = O_PATH;
#else
	// These are intentionally backwards so that bfs >&- still fails with EBADF
	const int inflags = O_WRONLY, outflags = O_RDONLY;
#endif

	if (!isopen(STDERR_FILENO) && redirect(STDERR_FILENO, "/dev/null", outflags) < 0) {
		return -1;
	}
	if (!isopen(STDOUT_FILENO) && redirect(STDOUT_FILENO, "/dev/null", outflags) < 0) {
		perror("redirect()");
		return -1;
	}
	if (!isopen(STDIN_FILENO) && redirect(STDIN_FILENO, "/dev/null", inflags) < 0) {
		perror("redirect()");
		return -1;
	}

	return 0;
}

/**
 * bfs entry point.
 */
int main(int argc, char *argv[]) {
	int ret = EXIT_FAILURE;

	// Make sure the standard streams are open
	if (open_std_streams() != 0) {
		goto done;
	}

	// Use the system locale instead of "C"
	setlocale(LC_ALL, "");

	struct cmdline *cmdline = parse_cmdline(argc, argv);
	if (cmdline) {
		ret = eval_cmdline(cmdline);
	}

	if (free_cmdline(cmdline) != 0 && ret == EXIT_SUCCESS) {
		ret = EXIT_FAILURE;
	}

done:
	return ret;
}
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2017-2019 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#if BFS_HAS_SYS_PARAM
#	include <sys/param.h>
#endif

#if BFS_HAS_MNTENT
#	define BFS_MNTENT 1
#elif BSD
#	define BFS_MNTINFO 1
#elif __SVR4
#	define BFS_MNTTAB 1
#endif

#if BFS_MNTENT
#	include <mntent.h>
#	include <paths.h>
#	include <stdio.h>
#elif BFS_MNTINFO
#	include <sys/mount.h>
#	include <sys/ucred.h>
#elif BFS_MNTTAB
#	include <stdio.h>
#	include <sys/mnttab.h>
#endif

struct bfs_mtab {
	/** A map from device ID to file system type. */
	struct trie types;
	/** The names of all the mount points. */
	struct trie names;
};

/**
 * Add an entry to the mount table.
 */
static int bfs_mtab_add(struct bfs_mtab *mtab, const char *path, dev_t dev, const char *type) {
	if (!trie_insert_str(&mtab->names, xbasename(path))) {
		return -1;
	}

	struct trie_leaf *leaf = trie_insert_mem(&mtab->types, &dev, sizeof(dev));
	if (!leaf) {
		return -1;
	}

	if (leaf->value) {
		return 0;
	}

	leaf->value = strdup(type);
	if (leaf->value) {
		return 0;
	} else {
		trie_remove(&mtab->types, leaf);
		return -1;
	}
}

struct bfs_mtab *parse_bfs_mtab() {
#if BFS_MNTENT

	FILE *file = setmntent(_PATH_MOUNTED, "r");
	if (!file) {
		// In case we're in a chroot or something with /proc but no /etc/mtab
		file = setmntent("/proc/mounts", "r");
	}
	if (!file) {
		goto fail;
	}

	struct bfs_mtab *mtab = malloc(sizeof(*mtab));
	if (!mtab) {
		goto fail_file;
	}
	trie_init(&mtab->types);
	trie_init(&mtab->names);

	struct mntent *mnt;
	while ((mnt = getmntent(file))) {
		struct bfs_stat sb;
		if (bfs_stat(AT_FDCWD, mnt->mnt_dir, BFS_STAT_NOFOLLOW, &sb) != 0) {
			continue;
		}

		if (bfs_mtab_add(mtab, mnt->mnt_dir, sb.dev, mnt->mnt_type) != 0) {
			goto fail_mtab;
		}
	}

	endmntent(file);
	return mtab;

fail_mtab:
	free_bfs_mtab(mtab);
fail_file:
	endmntent(file);
fail:
	return NULL;

#elif BFS_MNTINFO

	struct statfs *mntbuf;
	int size = getmntinfo(&mntbuf, MNT_WAIT);
	if (size < 0) {
		return NULL;
	}

	struct bfs_mtab *mtab = malloc(sizeof(*mtab));
	if (!mtab) {
		goto fail;
	}
	trie_init(&mtab->types);
	trie_init(&mtab->names);

	for (struct statfs *mnt = mntbuf; mnt < mntbuf + size; ++mnt) {
		struct bfs_stat sb;
		if (bfs_stat(AT_FDCWD, mnt->f_mntonname, BFS_STAT_NOFOLLOW, &sb) != 0) {
			continue;
		}

		if (bfs_mtab_add(mtab, mnt->f_mntonname, sb.dev, mnt->f_fstypename) != 0) {
			goto fail_mtab;
		}
	}

	return mtab;

fail_mtab:
	free_bfs_mtab(mtab);
fail:
	return NULL;

#elif BFS_MNTTAB

	FILE *file = fopen(MNTTAB, "r");
	if (!file) {
		goto fail;
	}

	struct bfs_mtab *mtab = malloc(sizeof(*mtab));
	if (!mtab) {
		goto fail_file;
	}
	trie_init(&mtab->types);
	trie_init(&mtab->names);

	struct mnttab mnt;
	while (getmntent(file, &mnt) == 0) {
		struct bfs_stat sb;
		if (bfs_stat(AT_FDCWD, mnt.mnt_mountp, BFS_STAT_NOFOLLOW, &sb) != 0) {
			continue;
		}

		if (bfs_mtab_add(mtab, mnt.mnt_mountp, sb.dev, mnt.mnt_fstype) != 0) {
			goto fail_mtab;
		}
	}

	fclose(file);
	return mtab;

fail_mtab:
	free_bfs_mtab(mtab);
fail_file:
	fclose(file);
fail:
	return NULL;

#else

	errno = ENOTSUP;
	return NULL;
#endif
}

const char *bfs_fstype(const struct bfs_mtab *mtab, const struct bfs_stat *statbuf) {
	const struct trie_leaf *leaf = trie_find_mem(&mtab->types, &statbuf->dev, sizeof(statbuf->dev));
	if (leaf) {
		return leaf->value;
	} else {
		return "unknown";
	}
}

bool bfs_maybe_mount(const struct bfs_mtab *mtab, const char *path) {
	const char *name = xbasename(path);
	return trie_find_str(&mtab->names, name);
}

void free_bfs_mtab(struct bfs_mtab *mtab) {
	if (mtab) {
		trie_destroy(&mtab->names);

		struct trie_leaf *leaf;
		while ((leaf = trie_first_leaf(&mtab->types))) {
			free(leaf->value);
			trie_remove(&mtab->types, leaf);
		}
		trie_destroy(&mtab->types);

		free(mtab);
	}
}
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2017-2019 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

/**
 * The expression optimizer.  Different optimization levels are supported:
 *
 * -O1: basic logical simplifications, like folding (-true -and -foo) to -foo.
 *
 * -O2: dead code elimination and data flow analysis.  struct opt_facts is used
 * to record data flow facts that are true at various points of evaluation.
 * Specifically, struct opt_facts records the facts that must be true before an
 * expression is evaluated (state->facts), and those that must be true after the
 * expression is evaluated, given that it returns true (state->facts_when_true)
 * or false (state->facts_when_true).  Additionally, state->facts_when_impure
 * records the possible data flow facts before any expressions with side effects
 * are evaluated.
 *
 * -O3: expression re-ordering to reduce expected cost.  In an expression like
 * (-foo -and -bar), if both -foo and -bar are pure (no side effects), they can
 * be re-ordered to (-bar -and -foo).  This is profitable if the expected cost
 * is lower for the re-ordered expression, for example if -foo is very slow or
 * -bar is likely to return false.
 *
 * -O4/-Ofast: aggressive optimizations that may affect correctness in corner
 * cases.  The main effect is to use facts_when_impure to determine if any side-
 * effects are reachable at all, and skipping the traversal if not.
 */

#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>

static char *fake_and_arg = "-a";
static char *fake_or_arg = "-o";
static char *fake_not_arg = "!";

/**
 * A contrained integer range.
 */
struct range {
	/** The (inclusive) minimum value. */
	long long min;
	/** The (inclusive) maximum value. */
	long long max;
};

/** Compute the minimum of two values. */
static long long min_value(long long a, long long b) {
	if (a < b) {
		return a;
	} else {
		return b;
	}
}

/** Compute the maximum of two values. */
static long long max_value(long long a, long long b) {
	if (a > b) {
		return a;
	} else {
		return b;
	}
}

/** Constrain the minimum of a range. */
static void constrain_min(struct range *range, long long value) {
	range->min = max_value(range->min, value);
}

/** Contrain the maximum of a range. */
static void constrain_max(struct range *range, long long value) {
	range->max = min_value(range->max, value);
}

/** Remove a single value from a range. */
static void range_remove(struct range *range, long long value) {
	if (range->min == value) {
		if (range->min == LLONG_MAX) {
			range->max = LLONG_MIN;
		} else {
			++range->min;
		}
	}

	if (range->max == value) {
		if (range->max == LLONG_MIN) {
			range->min = LLONG_MAX;
		} else {
			--range->max;
		}
	}
}

/** Compute the union of two ranges. */
static void range_union(struct range *result, const struct range *lhs, const struct range *rhs) {
	result->min = min_value(lhs->min, rhs->min);
	result->max = max_value(lhs->max, rhs->max);
}

/** Check if a range contains no values. */
static bool range_impossible(const struct range *range) {
	return range->min > range->max;
}

/** Set a range to contain no values. */
static void set_range_impossible(struct range *range) {
	range->min = LLONG_MAX;
	range->max = LLONG_MIN;
}

/**
 * Types of ranges we track.
 */
enum range_type {
	/** Search tree depth. */
	DEPTH_RANGE,
	/** Group ID. */
	GID_RANGE,
	/** Inode number.  */
	INUM_RANGE,
	/** Hard link count. */
	LINKS_RANGE,
	/** File size. */
	SIZE_RANGE,
	/** User ID. */
	UID_RANGE,
	/** The number of range_types. */
	MAX_RANGE,
};

/**
 * Data flow facts about an evaluation point.
 */
struct opt_facts {
	/** The value ranges we track. */
	struct range ranges[MAX_RANGE];

	/** Bitmask of possible file types. */
	enum bftw_typeflag types;
	/** Bitmask of possible link target types. */
	enum bftw_typeflag xtypes;
};

/** Initialize some data flow facts. */
static void facts_init(struct opt_facts *facts) {
	for (int i = 0; i < MAX_RANGE; ++i) {
		struct range *range = facts->ranges + i;
		range->min = 0; // All ranges we currently track are non-negative
		range->max = LLONG_MAX;
	}

	facts->types = ~0;
	facts->xtypes = ~0;
}

/** Compute the union of two fact sets. */
static void facts_union(struct opt_facts *result, const struct opt_facts *lhs, const struct opt_facts *rhs) {
	for (int i = 0; i < MAX_RANGE; ++i) {
		range_union(result->ranges + i, lhs->ranges + i, rhs->ranges + i);
	}

	result->types = lhs->types | rhs->types;
	result->xtypes = lhs->xtypes | rhs->xtypes;
}

/** Determine whether a fact set is impossible. */
static bool facts_impossible(const struct opt_facts *facts) {
	for (int i = 0; i < MAX_RANGE; ++i) {
		if (range_impossible(facts->ranges + i)) {
			return true;
		}
	}

	if (!facts->types || !facts->xtypes) {
		return true;
	}

	return false;
}

/** Set some facts to be impossible. */
static void set_facts_impossible(struct opt_facts *facts) {
	for (int i = 0; i < MAX_RANGE; ++i) {
		set_range_impossible(facts->ranges + i);
	}

	facts->types = 0;
	facts->xtypes = 0;
}

/**
 * Optimizer state.
 */
struct opt_state {
	/** The command line we're optimizing. */
	const struct cmdline *cmdline;

	/** Data flow facts before this expression is evaluated. */
	struct opt_facts facts;
	/** Data flow facts after this expression returns true. */
	struct opt_facts facts_when_true;
	/** Data flow facts after this expression returns false. */
	struct opt_facts facts_when_false;
	/** Data flow facts before any side-effecting expressions are evaluated. */
	struct opt_facts *facts_when_impure;
};

/** Log an optimization. */
static void debug_opt(const struct opt_state *state, const char *format, ...) {
	if (!(state->cmdline->debug & DEBUG_OPT)) {
		return;
	}

	CFILE *cerr = state->cmdline->cerr;

	va_list args;
	va_start(args, format);

	for (const char *i = format; *i != '\0'; ++i) {
		if (*i == '%') {
			switch (*++i) {
			case 'd':
				fprintf(cerr->file, "%d", va_arg(args, int));
				break;

			case 'e':
				dump_expr(cerr, va_arg(args, const struct expr *), false);
				break;

			case 'g':
				cfprintf(cerr, "${ylw}%g${rs}", va_arg(args, double));
				break;

			default:
				assert(false);
				break;
			}
		} else {
			fputc(*i, stderr);
		}
	}

	va_end(args);
}

/** Extract a child expression, freeing the outer expression. */
static struct expr *extract_child_expr(struct expr *expr, struct expr **child) {
	struct expr *ret = *child;
	*child = NULL;
	free_expr(expr);
	return ret;
}

/**
 * Negate an expression.
 */
static struct expr *negate_expr(struct expr *rhs, char **argv) {
	if (rhs->eval == eval_not) {
		return extract_child_expr(rhs, &rhs->rhs);
	}

	struct expr *expr = new_expr(eval_not, 1, argv);
	if (!expr) {
		free_expr(rhs);
		return NULL;
	}

	expr->rhs = rhs;
	return expr;
}

static struct expr *optimize_not_expr(const struct opt_state *state, struct expr *expr);
static struct expr *optimize_and_expr(const struct opt_state *state, struct expr *expr);
static struct expr *optimize_or_expr(const struct opt_state *state, struct expr *expr);

/**
 * Apply De Morgan's laws.
 */
static struct expr *de_morgan(const struct opt_state *state, struct expr *expr, char **argv) {
	debug_opt(state, "-O1: De Morgan's laws: %e ", expr);

	struct expr *parent = negate_expr(expr, argv);
	if (!parent) {
		return NULL;
	}

	bool has_parent = true;
	if (parent->eval != eval_not) {
		expr = parent;
		has_parent = false;
	}

	if (expr->eval == eval_and) {
		expr->eval = eval_or;
		expr->argv = &fake_or_arg;
	} else {
		assert(expr->eval == eval_or);
		expr->eval = eval_and;
		expr->argv = &fake_and_arg;
	}

	expr->lhs = negate_expr(expr->lhs, argv);
	expr->rhs = negate_expr(expr->rhs, argv);
	if (!expr->lhs || !expr->rhs) {
		free_expr(parent);
		return NULL;
	}

	debug_opt(state, "<==> %e\n", parent);

	if (expr->lhs->eval == eval_not) {
		expr->lhs = optimize_not_expr(state, expr->lhs);
	}
	if (expr->rhs->eval == eval_not) {
		expr->rhs = optimize_not_expr(state, expr->rhs);
	}
	if (!expr->lhs || !expr->rhs) {
		free_expr(parent);
		return NULL;
	}

	if (expr->eval == eval_and) {
		expr = optimize_and_expr(state, expr);
	} else {
		expr = optimize_or_expr(state, expr);
	}
	if (!expr) {
		if (has_parent) {
			parent->rhs = NULL;
			free_expr(parent);
		}
		return NULL;
	}

	if (has_parent) {
		parent = optimize_not_expr(state, parent);
	}
	return parent;
}

/** Optimize an expression recursively. */
static struct expr *optimize_expr_recursive(struct opt_state *state, struct expr *expr);

/**
 * Optimize a negation.
 */
static struct expr *optimize_not_expr(const struct opt_state *state, struct expr *expr) {
	assert(expr->eval == eval_not);

	struct expr *rhs = expr->rhs;

	int optlevel = state->cmdline->optlevel;
	if (optlevel >= 1) {
		if (rhs == &expr_true) {
			debug_opt(state, "-O1: constant propagation: %e <==> %e\n", expr, &expr_false);
			free_expr(expr);
			return &expr_false;
		} else if (rhs == &expr_false) {
			debug_opt(state, "-O1: constant propagation: %e <==> %e\n", expr, &expr_true);
			free_expr(expr);
			return &expr_true;
		} else if (rhs->eval == eval_not) {
			debug_opt(state, "-O1: double negation: %e <==> %e\n", expr, rhs->rhs);
			return extract_child_expr(expr, &rhs->rhs);
		} else if (expr_never_returns(rhs)) {
			debug_opt(state, "-O1: reachability: %e <==> %e\n", expr, rhs);
			return extract_child_expr(expr, &expr->rhs);
		} else if ((rhs->eval == eval_and || rhs->eval == eval_or)
			   && (rhs->lhs->eval == eval_not || rhs->rhs->eval == eval_not)) {
			return de_morgan(state, expr, expr->argv);
		}
	}

	expr->pure = rhs->pure;
	expr->always_true = rhs->always_false;
	expr->always_false = rhs->always_true;
	expr->cost = rhs->cost;
	expr->probability = 1.0 - rhs->probability;

	return expr;
}

/** Optimize a negation recursively. */
static struct expr *optimize_not_expr_recursive(struct opt_state *state, struct expr *expr) {
	struct opt_state rhs_state = *state;
	expr->rhs = optimize_expr_recursive(&rhs_state, expr->rhs);
	if (!expr->rhs) {
		goto fail;
	}

	state->facts_when_true = rhs_state.facts_when_false;
	state->facts_when_false = rhs_state.facts_when_true;

	return optimize_not_expr(state, expr);

fail:
	free_expr(expr);
	return NULL;
}

/** Optimize a conjunction. */
static struct expr *optimize_and_expr(const struct opt_state *state, struct expr *expr) {
	assert(expr->eval == eval_and);

	struct expr *lhs = expr->lhs;
	struct expr *rhs = expr->rhs;

	int optlevel = state->cmdline->optlevel;
	if (optlevel >= 1) {
		if (lhs == &expr_true) {
			debug_opt(state, "-O1: conjunction elimination: %e <==> %e\n", expr, rhs);
			return extract_child_expr(expr, &expr->rhs);
		} else if (rhs == &expr_true) {
			debug_opt(state, "-O1: conjunction elimination: %e <==> %e\n", expr, lhs);
			return extract_child_expr(expr, &expr->lhs);
		} else if (lhs->always_false) {
			debug_opt(state, "-O1: short-circuit: %e <==> %e\n", expr, lhs);
			return extract_child_expr(expr, &expr->lhs);
		} else if (lhs->always_true && rhs == &expr_false) {
			debug_opt(state, "-O1: strength reduction: %e <==> ", expr);
			struct expr *ret = extract_child_expr(expr, &expr->lhs);
			ret = negate_expr(ret, &fake_not_arg);
			if (ret) {
				debug_opt(state, "%e\n", ret);
			}
			return ret;
		} else if (optlevel >= 2 && lhs->pure && rhs == &expr_false) {
			debug_opt(state, "-O2: purity: %e <==> %e\n", expr, rhs);
			return extract_child_expr(expr, &expr->rhs);
		} else if (lhs->eval == eval_not && rhs->eval == eval_not) {
			return de_morgan(state, expr, expr->lhs->argv);
		}
	}

	expr->pure = lhs->pure && rhs->pure;
	expr->always_true = lhs->always_true && rhs->always_true;
	expr->always_false = lhs->always_false || rhs->always_false;
	expr->cost = lhs->cost + lhs->probability*rhs->cost;
	expr->probability = lhs->probability*rhs->probability;

	return expr;
}

/** Optimize a conjunction recursively. */
static struct expr *optimize_and_expr_recursive(struct opt_state *state, struct expr *expr) {
	struct opt_state lhs_state = *state;
	expr->lhs = optimize_expr_recursive(&lhs_state, expr->lhs);
	if (!expr->lhs) {
		goto fail;
	}

	struct opt_state rhs_state = *state;
	rhs_state.facts = lhs_state.facts_when_true;
	expr->rhs = optimize_expr_recursive(&rhs_state, expr->rhs);
	if (!expr->rhs) {
		goto fail;
	}

	state->facts_when_true = rhs_state.facts_when_true;
	facts_union(&state->facts_when_false, &lhs_state.facts_when_false, &rhs_state.facts_when_false);

	return optimize_and_expr(state, expr);

fail:
	free_expr(expr);
	return NULL;
}

/** Optimize a disjunction. */
static struct expr *optimize_or_expr(const struct opt_state *state, struct expr *expr) {
	assert(expr->eval == eval_or);

	struct expr *lhs = expr->lhs;
	struct expr *rhs = expr->rhs;

	int optlevel = state->cmdline->optlevel;
	if (optlevel >= 1) {
		if (lhs->always_true) {
			debug_opt(state, "-O1: short-circuit: %e <==> %e\n", expr, lhs);
			return extract_child_expr(expr, &expr->lhs);
		} else if (lhs == &expr_false) {
			debug_opt(state, "-O1: disjunctive syllogism: %e <==> %e\n", expr, rhs);
			return extract_child_expr(expr, &expr->rhs);
		} else if (rhs == &expr_false) {
			debug_opt(state, "-O1: disjunctive syllogism: %e <==> %e\n", expr, lhs);
			return extract_child_expr(expr, &expr->lhs);
		} else if (lhs->always_false && rhs == &expr_true) {
			debug_opt(state, "-O1: strength reduction: %e <==> ", expr);
			struct expr *ret = extract_child_expr(expr, &expr->lhs);
			ret = negate_expr(ret, &fake_not_arg);
			if (ret) {
				debug_opt(state, "%e\n", ret);
			}
			return ret;
		} else if (optlevel >= 2 && lhs->pure && rhs == &expr_true) {
			debug_opt(state, "-O2: purity: %e <==> %e\n", expr, rhs);
			return extract_child_expr(expr, &expr->rhs);
		} else if (lhs->eval == eval_not && rhs->eval == eval_not) {
			return de_morgan(state, expr, expr->lhs->argv);
		}
	}

	expr->pure = lhs->pure && rhs->pure;
	expr->always_true = lhs->always_true || rhs->always_true;
	expr->always_false = lhs->always_false && rhs->always_false;
	expr->cost = lhs->cost + (1 - lhs->probability)*rhs->cost;
	expr->probability = lhs->probability + rhs->probability - lhs->probability*rhs->probability;

	return expr;
}

/** Optimize a disjunction recursively. */
static struct expr *optimize_or_expr_recursive(struct opt_state *state, struct expr *expr) {
	struct opt_state lhs_state = *state;
	expr->lhs = optimize_expr_recursive(&lhs_state, expr->lhs);
	if (!expr->lhs) {
		goto fail;
	}

	struct opt_state rhs_state = *state;
	rhs_state.facts = lhs_state.facts_when_false;
	expr->rhs = optimize_expr_recursive(&rhs_state, expr->rhs);
	if (!expr->rhs) {
		goto fail;
	}

	facts_union(&state->facts_when_true, &lhs_state.facts_when_true, &rhs_state.facts_when_true);
	state->facts_when_false = rhs_state.facts_when_false;

	return optimize_or_expr(state, expr);

fail:
	free_expr(expr);
	return NULL;
}

/** Optimize an expression in an ignored-result context. */
static struct expr *ignore_result(const struct opt_state *state, struct expr *expr) {
	int optlevel = state->cmdline->optlevel;

	if (optlevel >= 1) {
		while (true) {
			if (expr->eval == eval_not) {
				debug_opt(state, "-O1: ignored result: %e --> %e\n", expr, expr->rhs);
				expr = extract_child_expr(expr, &expr->rhs);
			} else if (optlevel >= 2
			           && (expr->eval == eval_and || expr->eval == eval_or || expr->eval == eval_comma)
			           && expr->rhs->pure) {
				debug_opt(state, "-O2: ignored result: %e --> %e\n", expr, expr->lhs);
				expr = extract_child_expr(expr, &expr->lhs);
			} else {
				break;
			}
		}

		if (optlevel >= 2 && expr->pure && expr != &expr_false) {
			debug_opt(state, "-O2: ignored result: %e --> %e\n", expr, &expr_false);
			free_expr(expr);
			expr = &expr_false;
		}
	}

	return expr;
}

/** Optimize a comma expression. */
static struct expr *optimize_comma_expr(const struct opt_state *state, struct expr *expr) {
	assert(expr->eval == eval_comma);

	struct expr *lhs = expr->lhs;
	struct expr *rhs = expr->rhs;

	int optlevel = state->cmdline->optlevel;
	if (optlevel >= 1) {
		lhs = expr->lhs = ignore_result(state, lhs);

		if (expr_never_returns(lhs)) {
			debug_opt(state, "-O1: reachability: %e <==> %e\n", expr, lhs);
			return extract_child_expr(expr, &expr->lhs);
		} else if ((lhs->always_true && rhs == &expr_true)
			   || (lhs->always_false && rhs == &expr_false)) {
			debug_opt(state, "-O1: redundancy elimination: %e <==> %e\n", expr, lhs);
			return extract_child_expr(expr, &expr->lhs);
		} else if (optlevel >= 2 && lhs->pure) {
			debug_opt(state, "-O2: purity: %e <==> %e\n", expr, rhs);
			return extract_child_expr(expr, &expr->rhs);
		}
	}

	expr->pure = lhs->pure && rhs->pure;
	expr->always_true = expr_never_returns(lhs) || rhs->always_true;
	expr->always_false = expr_never_returns(lhs) || rhs->always_false;
	expr->cost = lhs->cost + rhs->cost;
	expr->probability = rhs->probability;

	return expr;
}

/** Optimize a comma expression recursively. */
static struct expr *optimize_comma_expr_recursive(struct opt_state *state, struct expr *expr) {
	struct opt_state lhs_state = *state;
	expr->lhs = optimize_expr_recursive(&lhs_state, expr->lhs);
	if (!expr->lhs) {
		goto fail;
	}

	struct opt_state rhs_state = *state;
	facts_union(&rhs_state.facts, &lhs_state.facts_when_true, &lhs_state.facts_when_false);
	expr->rhs = optimize_expr_recursive(&rhs_state, expr->rhs);
	if (!expr->rhs) {
		goto fail;
	}

	return optimize_comma_expr(state, expr);

fail:
	free_expr(expr);
	return NULL;
}

/** Infer data flow facts about an icmp-style ([+-]N) expression */
static void infer_icmp_facts(struct opt_state *state, const struct expr *expr, enum range_type type) {
	struct range *range_when_true = state->facts_when_true.ranges + type;
	struct range *range_when_false = state->facts_when_false.ranges + type;
	long long value = expr->idata;

	switch (expr->cmp_flag) {
	case CMP_EXACT:
		constrain_min(range_when_true, value);
		constrain_max(range_when_true, value);
		range_remove(range_when_false, value);
		break;

	case CMP_LESS:
		constrain_min(range_when_false, value);
		constrain_max(range_when_true, value);
		range_remove(range_when_true, value);
		break;

	case CMP_GREATER:
		constrain_max(range_when_false, value);
		constrain_min(range_when_true, value);
		range_remove(range_when_true, value);
		break;
	}
}

/** Infer data flow facts about a -samefile expression. */
static void infer_samefile_facts(struct opt_state *state, const struct expr *expr) {
	struct range *range_when_true = state->facts_when_true.ranges + INUM_RANGE;
	constrain_min(range_when_true, expr->ino);
	constrain_max(range_when_true, expr->ino);
}

/** Infer data flow facts about a -type expression. */
static void infer_type_facts(struct opt_state *state, const struct expr *expr) {
	state->facts_when_true.types &= expr->idata;
	state->facts_when_false.types &= ~expr->idata;
}

/** Infer data flow facts about an -xtype expression. */
static void infer_xtype_facts(struct opt_state *state, const struct expr *expr) {
	state->facts_when_true.xtypes &= expr->idata;
	state->facts_when_false.xtypes &= ~expr->idata;
}

static struct expr *optimize_expr_recursive(struct opt_state *state, struct expr *expr) {
	state->facts_when_true = state->facts;
	state->facts_when_false = state->facts;

	if (expr->eval == eval_depth) {
		infer_icmp_facts(state, expr, DEPTH_RANGE);
	} else if (expr->eval == eval_gid) {
		infer_icmp_facts(state, expr, GID_RANGE);
	} else if (expr->eval == eval_inum) {
		infer_icmp_facts(state, expr, INUM_RANGE);
	} else if (expr->eval == eval_links) {
		infer_icmp_facts(state, expr, LINKS_RANGE);
	} else if (expr->eval == eval_samefile) {
		infer_samefile_facts(state, expr);
	} else if (expr->eval == eval_size) {
		infer_icmp_facts(state, expr, SIZE_RANGE);
	} else if (expr->eval == eval_type) {
		infer_type_facts(state, expr);
	} else if (expr->eval == eval_uid) {
		infer_icmp_facts(state, expr, UID_RANGE);
	} else if (expr->eval == eval_xtype) {
		infer_xtype_facts(state, expr);
	} else if (expr->eval == eval_not) {
		expr = optimize_not_expr_recursive(state, expr);
	} else if (expr->eval == eval_and) {
		expr = optimize_and_expr_recursive(state, expr);
	} else if (expr->eval == eval_or) {
		expr = optimize_or_expr_recursive(state, expr);
	} else if (expr->eval == eval_comma) {
		expr = optimize_comma_expr_recursive(state, expr);
	} else if (!expr->pure) {
		facts_union(state->facts_when_impure, state->facts_when_impure, &state->facts);
	}

	if (!expr) {
		goto done;
	}

	struct expr *lhs = expr->lhs;
	struct expr *rhs = expr->rhs;
	if (rhs) {
		expr->persistent_fds = rhs->persistent_fds;
		expr->ephemeral_fds = rhs->ephemeral_fds;
	}
	if (lhs) {
		expr->persistent_fds += lhs->persistent_fds;
		if (lhs->ephemeral_fds > expr->ephemeral_fds) {
			expr->ephemeral_fds = lhs->ephemeral_fds;
		}
	}

	if (expr->always_true) {
		set_facts_impossible(&state->facts_when_false);
	}
	if (expr->always_false) {
		set_facts_impossible(&state->facts_when_true);
	}

	if (state->cmdline->optlevel < 2 || expr == &expr_true || expr == &expr_false) {
		goto done;
	}

	if (facts_impossible(&state->facts_when_true)) {
		if (expr->pure) {
			debug_opt(state, "-O2: data flow: %e --> %e\n", expr, &expr_false);
			free_expr(expr);
			expr = &expr_false;
		} else {
			expr->always_false = true;
			expr->probability = 0.0;
		}
	} else if (facts_impossible(&state->facts_when_false)) {
		if (expr->pure) {
			debug_opt(state, "-O2: data flow: %e --> %e\n", expr, &expr_true);
			free_expr(expr);
			expr = &expr_true;
		} else {
			expr->always_true = true;
			expr->probability = 1.0;
		}
	}

done:
	return expr;
}

/** Swap the children of a binary expression if it would reduce the cost. */
static bool reorder_expr(const struct opt_state *state, struct expr *expr, double swapped_cost) {
	if (swapped_cost < expr->cost) {
		debug_opt(state, "-O3: cost: %e", expr);
		struct expr *lhs = expr->lhs;
		expr->lhs = expr->rhs;
		expr->rhs = lhs;
		debug_opt(state, " <==> %e (~%g --> ~%g)\n", expr, expr->cost, swapped_cost);
		expr->cost = swapped_cost;
		return true;
	} else {
		return false;
	}
}

/**
 * Recursively reorder sub-expressions to reduce the overall cost.
 *
 * @param expr
 *         The expression to optimize.
 * @return
 *         Whether any subexpression was reordered.
 */
static bool reorder_expr_recursive(const struct opt_state *state, struct expr *expr) {
	bool ret = false;
	struct expr *lhs = expr->lhs;
	struct expr *rhs = expr->rhs;

	if (lhs) {
		ret |= reorder_expr_recursive(state, lhs);
	}
	if (rhs) {
		ret |= reorder_expr_recursive(state, rhs);
	}

	if (expr->eval == eval_and || expr->eval == eval_or) {
		if (lhs->pure && rhs->pure) {
			double rhs_prob = expr->eval == eval_and ? rhs->probability : 1.0 - rhs->probability;
			double swapped_cost = rhs->cost + rhs_prob*lhs->cost;
			ret |= reorder_expr(state, expr, swapped_cost);
		}
	}

	return ret;
}

int optimize_cmdline(struct cmdline *cmdline) {
	struct opt_facts facts_when_impure;
	set_facts_impossible(&facts_when_impure);

	struct opt_state state = {
		.cmdline = cmdline,
		.facts_when_impure = &facts_when_impure,
	};
	facts_init(&state.facts);

	struct range *depth = state.facts.ranges + DEPTH_RANGE;
	depth->min = cmdline->mindepth;
	depth->max = cmdline->maxdepth;

	int optlevel = cmdline->optlevel;

	cmdline->expr = optimize_expr_recursive(&state, cmdline->expr);
	if (!cmdline->expr) {
		return -1;
	}

	if (optlevel >= 3 && reorder_expr_recursive(&state, cmdline->expr)) {
		// Re-do optimizations to account for the new ordering
		set_facts_impossible(&facts_when_impure);
		cmdline->expr = optimize_expr_recursive(&state, cmdline->expr);
		if (!cmdline->expr) {
			return -1;
		}
	}

	cmdline->expr = ignore_result(&state, cmdline->expr);

	const struct range *depth_when_impure = facts_when_impure.ranges + DEPTH_RANGE;
	long long mindepth = depth_when_impure->min;
	long long maxdepth = depth_when_impure->max;

	if (optlevel >= 2 && mindepth > cmdline->mindepth) {
		if (mindepth > INT_MAX) {
			mindepth = INT_MAX;
		}
		cmdline->mindepth = mindepth;
		debug_opt(&state, "-O2: data flow: mindepth --> %d\n", cmdline->mindepth);
	}

	if (optlevel >= 4 && maxdepth < cmdline->maxdepth) {
		if (maxdepth < INT_MIN) {
			maxdepth = INT_MIN;
		}
		cmdline->maxdepth = maxdepth;
		debug_opt(&state, "-O4: data flow: maxdepth --> %d\n", cmdline->maxdepth);
	}

	return 0;
}
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2015-2019 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

/**
 * The command line parser.  Expressions are parsed by recursive descent, with a
 * grammar described in the comments of the parse_*() functions.  The parser
 * also accepts flags and paths at any point in the expression, by treating
 * flags like always-true options, and skipping over paths wherever they appear.
 */

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <regex.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

// Strings printed by -D tree for "fake" expressions
static char *fake_false_arg = "-false";
static char *fake_print_arg = "-print";
static char *fake_true_arg = "-true";

// Cost estimation constants
#define FAST_COST     40.0
#define STAT_COST   1000.0
#define PRINT_COST 20000.0

struct expr expr_true = {
	.eval = eval_true,
	.lhs = NULL,
	.rhs = NULL,
	.pure = true,
	.always_true = true,
	.cost = FAST_COST,
	.probability = 1.0,
	.argc = 1,
	.argv = &fake_true_arg,
};

struct expr expr_false = {
	.eval = eval_false,
	.lhs = NULL,
	.rhs = NULL,
	.pure = true,
	.always_false = true,
	.cost = FAST_COST,
	.probability = 0.0,
	.argc = 1,
	.argv = &fake_false_arg,
};

/**
 * Free an expression.
 */
void free_expr(struct expr *expr) {
	if (!expr || expr == &expr_true || expr == &expr_false) {
		return;
	}

	if (expr->regex) {
		regfree(expr->regex);
		free(expr->regex);
	}

	free_bfs_printf(expr->printf);
	free_bfs_exec(expr->execbuf);

	free_expr(expr->lhs);
	free_expr(expr->rhs);

	free(expr);
}

struct expr *new_expr(eval_fn *eval, size_t argc, char **argv) {
	struct expr *expr = malloc(sizeof(*expr));
	if (!expr) {
		perror("malloc()");
		return NULL;
	}

	expr->eval = eval;
	expr->lhs = NULL;
	expr->rhs = NULL;
	expr->pure = false;
	expr->always_true = false;
	expr->always_false = false;
	expr->cost = FAST_COST;
	expr->probability = 0.5;
	expr->evaluations = 0;
	expr->successes = 0;
	expr->elapsed.tv_sec = 0;
	expr->elapsed.tv_nsec = 0;
	expr->argc = argc;
	expr->argv = argv;
	expr->cfile = NULL;
	expr->regex = NULL;
	expr->execbuf = NULL;
	expr->printf = NULL;
	expr->persistent_fds = 0;
	expr->ephemeral_fds = 0;
	return expr;
}

/**
 * Create a new unary expression.
 */
static struct expr *new_unary_expr(eval_fn *eval, struct expr *rhs, char **argv) {
	struct expr *expr = new_expr(eval, 1, argv);
	if (!expr) {
		free_expr(rhs);
		return NULL;
	}

	expr->rhs = rhs;
	expr->persistent_fds = rhs->persistent_fds;
	expr->ephemeral_fds = rhs->ephemeral_fds;
	return expr;
}

/**
 * Create a new binary expression.
 */
static struct expr *new_binary_expr(eval_fn *eval, struct expr *lhs, struct expr *rhs, char **argv) {
	struct expr *expr = new_expr(eval, 1, argv);
	if (!expr) {
		free_expr(rhs);
		free_expr(lhs);
		return NULL;
	}

	expr->lhs = lhs;
	expr->rhs = rhs;
	expr->persistent_fds = lhs->persistent_fds + rhs->persistent_fds;
	if (lhs->ephemeral_fds > rhs->ephemeral_fds) {
		expr->ephemeral_fds = lhs->ephemeral_fds;
	} else {
		expr->ephemeral_fds = rhs->ephemeral_fds;
	}
	return expr;
}

/**
 * Check if an expression never returns.
 */
bool expr_never_returns(const struct expr *expr) {
	// Expressions that never return are vacuously both always true and always false
	return expr->always_true && expr->always_false;
}

/**
 * Set an expression to always return true.
 */
static void expr_set_always_true(struct expr *expr) {
	expr->always_true = true;
	expr->probability = 1.0;
}

/**
 * Set an expression to never return.
 */
static void expr_set_never_returns(struct expr *expr) {
	expr->always_true = expr->always_false = true;
}

/**
 * Dump the parsed expression tree, for debugging.
 */
void dump_expr(CFILE *cfile, const struct expr *expr, bool verbose) {
	fputs("(", cfile->file);

	if (expr->lhs || expr->rhs) {
		cfprintf(cfile, "${red}%s${rs}", expr->argv[0]);
	} else {
		cfprintf(cfile, "${blu}%s${rs}", expr->argv[0]);
	}

	for (size_t i = 1; i < expr->argc; ++i) {
		cfprintf(cfile, " ${bld}%s${rs}", expr->argv[i]);
	}

	if (verbose) {
		double rate = 0.0, time = 0.0;
		if (expr->evaluations) {
			rate = 100.0*expr->successes/expr->evaluations;
			time = (1.0e9*expr->elapsed.tv_sec + expr->elapsed.tv_nsec)/expr->evaluations;
		}
		cfprintf(cfile, " [${ylw}%zu${rs}/${ylw}%zu${rs}=${ylw}%g%%${rs}; ${ylw}%gns${rs}]", expr->successes, expr->evaluations, rate, time);
	}

	if (expr->lhs) {
		fputs(" ", cfile->file);
		dump_expr(cfile, expr->lhs, verbose);
	}

	if (expr->rhs) {
		fputs(" ", cfile->file);
		dump_expr(cfile, expr->rhs, verbose);
	}

	fputs(")", cfile->file);
}

/**
 * An open file for the command line.
 */
struct open_file {
	/** The file itself. */
	CFILE *cfile;
	/** The path to the file (for diagnostics). */
	const char *path;
};

/**
 * Free the parsed command line.
 */
int free_cmdline(struct cmdline *cmdline) {
	int ret = 0;

	if (cmdline) {
		CFILE *cout = cmdline->cout;
		CFILE *cerr = cmdline->cerr;

		free_expr(cmdline->expr);

		free_bfs_mtab(cmdline->mtab);

		struct trie_leaf *leaf;
		while ((leaf = trie_first_leaf(&cmdline->open_files))) {
			struct open_file *ofile = leaf->value;

			if (cfclose(ofile->cfile) != 0) {
				if (cerr) {
					bfs_error(cmdline, "'%s': %m.\n", ofile->path);
				}
				ret = -1;
			}

			free(ofile);
			trie_remove(&cmdline->open_files, leaf);
		}
		trie_destroy(&cmdline->open_files);

		if (cout && fflush(cout->file) != 0) {
			if (cerr) {
				bfs_error(cmdline, "standard output: %m.\n");
			}
			ret = -1;
		}

		cfclose(cout);
		cfclose(cerr);

		free_colors(cmdline->colors);
		free(cmdline->paths);
		free(cmdline->argv);
		free(cmdline);
	}

	return ret;
}

/**
 * Color use flags.
 */
enum use_color {
	COLOR_NEVER,
	COLOR_AUTO,
	COLOR_ALWAYS,
};

/**
 * Ephemeral state for parsing the command line.
 */
struct parser_state {
	/** The command line being constructed. */
	struct cmdline *cmdline;
	/** The command line arguments being parsed. */
	char **argv;
	/** The name of this program. */
	const char *command;

	/** The current regex flags to use. */
	int regex_flags;

	/** Whether stdout is a terminal. */
	bool stdout_tty;
	/** Whether this session is interactive (stdin and stderr are each a terminal). */
	bool interactive;
	/** Whether -color or -nocolor has been passed. */
	enum use_color use_color;
	/** Whether a -print action is implied. */
	bool implicit_print;
	/** Whether warnings are enabled (see -warn, -nowarn). */
	bool warn;
	/** Whether the expression has started. */
	bool expr_started;
	/** Whether any non-option arguments have been encountered. */
	bool non_option_seen;
	/** Whether an information option like -help or -version was passed. */
	bool just_info;

	/** The last non-path argument. */
	const char *last_arg;
	/** A "-depth"-type argument if any. */
	const char *depth_arg;
	/** A "-prune"-type argument if any. */
	const char *prune_arg;

	/** The current time. */
	struct timespec now;
};

/**
 * Possible token types.
 */
enum token_type {
	/** A flag. */
	T_FLAG,
	/** A root path. */
	T_PATH,
	/** An option. */
	T_OPTION,
	/** A test. */
	T_TEST,
	/** An action. */
	T_ACTION,
	/** An operator. */
	T_OPERATOR,
};

/**
 * Print an error message during parsing.
 */
BFS_FORMATTER(2, 3)
static void parse_error(const struct parser_state *state, const char *format, ...) {
	va_list args;
	va_start(args, format);
	bfs_verror(state->cmdline, format, args);
	va_end(args);
}

/**
 * Print a warning message during parsing.
 */
BFS_FORMATTER(2, 3)
static void parse_warning(const struct parser_state *state, const char *format, ...) {
	va_list args;
	va_start(args, format);
	bfs_vwarning(state->cmdline, format, args);
	va_end(args);
}

/**
 * Fill in a "-print"-type expression.
 */
static void init_print_expr(struct parser_state *state, struct expr *expr) {
	expr_set_always_true(expr);
	expr->cost = PRINT_COST;
	expr->cfile = state->cmdline->cout;
}

/**
 * Open a file for an expression.
 */
static int expr_open(struct parser_state *state, struct expr *expr, const char *path) {
	int ret = -1;

	struct cmdline *cmdline = state->cmdline;

	CFILE *cfile = cfopen(path, state->use_color ? cmdline->colors : NULL);
	if (!cfile) {
		parse_error(state, "'%s': %m.\n", path);
		goto out;
	}

	struct bfs_stat sb;
	if (bfs_stat(fileno(cfile->file), NULL, 0, &sb) != 0) {
		parse_error(state, "'%s': %m.\n", path);
		goto out_close;
	}

	bfs_file_id id;
	bfs_stat_id(&sb, &id);

	struct trie_leaf *leaf = trie_insert_mem(&cmdline->open_files, id, sizeof(id));
	if (leaf->value) {
		struct open_file *ofile = leaf->value;
		expr->cfile = ofile->cfile;
		ret = 0;
		goto out_close;
	}

	struct open_file *ofile = malloc(sizeof(*ofile));
	if (!ofile) {
		perror("malloc()");
		trie_remove(&cmdline->open_files, leaf);
		goto out_close;
	}

	ofile->cfile = cfile;
	ofile->path = path;
	leaf->value = ofile;
	++cmdline->nopen_files;

	expr->cfile = cfile;

	ret = 0;
	goto out;

out_close:
	cfclose(cfile);
out:
	return ret;
}

/**
 * Invoke bfs_stat() on an argument.
 */
static int stat_arg(const struct parser_state *state, struct expr *expr, struct bfs_stat *sb) {
	const struct cmdline *cmdline = state->cmdline;

	bool follow = cmdline->flags & (BFTW_COMFOLLOW | BFTW_LOGICAL);
	enum bfs_stat_flag flags = follow ? BFS_STAT_TRYFOLLOW : BFS_STAT_NOFOLLOW;

	int ret = bfs_stat(AT_FDCWD, expr->sdata, flags, sb);
	if (ret != 0) {
		parse_error(state, "'%s': %m.\n", expr->sdata);
	}
	return ret;
}

/**
 * Parse the expression specified on the command line.
 */
static struct expr *parse_expr(struct parser_state *state);

/**
 * Advance by a single token.
 */
static char **parser_advance(struct parser_state *state, enum token_type type, size_t argc) {
	if (type != T_FLAG && type != T_PATH) {
		state->expr_started = true;

		if (type != T_OPTION) {
			state->non_option_seen = true;
		}
	}

	if (type != T_PATH) {
		state->last_arg = *state->argv;
	}

	char **argv = state->argv;
	state->argv += argc;
	return argv;
}

/**
 * Parse a root path.
 */
static int parse_root(struct parser_state *state, const char *path) {
	struct cmdline *cmdline = state->cmdline;
	size_t i = cmdline->npaths;
	if ((i & (i + 1)) == 0) {
		const char **paths = realloc(cmdline->paths, 2*(i + 1)*sizeof(*paths));
		if (!paths) {
			return -1;
		}
		cmdline->paths = paths;
	}

	cmdline->paths[i] = path;
	cmdline->npaths = i + 1;
	return 0;
}

/**
 * While parsing an expression, skip any paths and add them to the cmdline.
 */
static int skip_paths(struct parser_state *state) {
	while (true) {
		const char *arg = state->argv[0];
		if (!arg) {
			return 0;
		}

		if (arg[0] == '-') {
			if (strcmp(arg, "--") == 0) {
				// find uses -- to separate flags from the rest
				// of the command line.  We allow mixing flags
				// and paths/predicates, so we just ignore --.
				parser_advance(state, T_FLAG, 1);
				continue;
			}
			if (strcmp(arg, "-") != 0) {
				// - by itself is a file name.  Anything else
				// starting with - is a flag/predicate.
				return 0;
			}
		}

		// By POSIX, these are always options
		if (strcmp(arg, "(") == 0 || strcmp(arg, "!") == 0) {
			return 0;
		}

		if (state->expr_started) {
			// By POSIX, these can be paths.  We only treat them as
			// such at the beginning of the command line.
			if (strcmp(arg, ")") == 0 || strcmp(arg, ",") == 0) {
				return 0;
			}
		}

		if (parse_root(state, arg) != 0) {
			return -1;
		}

		parser_advance(state, T_PATH, 1);
	}
}

/** Integer parsing flags. */
enum int_flags {
	IF_BASE_MASK   = 0x03F,
	IF_INT         = 0x040,
	IF_LONG        = 0x080,
	IF_LONG_LONG   = 0x0C0,
	IF_SIZE_MASK   = 0x0C0,
	IF_UNSIGNED    = 0x100,
	IF_PARTIAL_OK  = 0x200,
	IF_QUIET       = 0x400,
};

/**
 * Parse an integer.
 */
static const char *parse_int(const struct parser_state *state, const char *str, void *result, enum int_flags flags) {
	char *endptr;

	int base = flags & IF_BASE_MASK;
	if (base == 0) {
		base = 10;
	}

	errno = 0;
	long long value = strtoll(str, &endptr, base);
	if (errno != 0) {
		goto bad;
	}

	if (endptr == str) {
		goto bad;
	}

	if (!(flags & IF_PARTIAL_OK) && *endptr != '\0') {
		goto bad;
	}

	if ((flags & IF_UNSIGNED) && value < 0) {
		goto bad;
	}

	switch (flags & IF_SIZE_MASK) {
	case IF_INT:
		if (value < INT_MIN || value > INT_MAX) {
			goto bad;
		}
		*(int *)result = value;
		break;

	case IF_LONG:
		if (value < LONG_MIN || value > LONG_MAX) {
			goto bad;
		}
		*(long *)result = value;
		break;

	case IF_LONG_LONG:
		*(long long *)result = value;
		break;
	}

	return endptr;

bad:
	if (!(flags & IF_QUIET)) {
		parse_error(state, "'%s' is not a valid integer.\n", str);
	}
	return NULL;
}

/**
 * Parse an integer and a comparison flag.
 */
static const char *parse_icmp(const struct parser_state *state, const char *str, struct expr *expr, enum int_flags flags) {
	switch (str[0]) {
	case '-':
		expr->cmp_flag = CMP_LESS;
		++str;
		break;
	case '+':
		expr->cmp_flag = CMP_GREATER;
		++str;
		break;
	default:
		expr->cmp_flag = CMP_EXACT;
		break;
	}

	return parse_int(state, str, &expr->idata, flags | IF_LONG_LONG | IF_UNSIGNED);
}

/**
 * Check if a string could be an integer comparison.
 */
static bool looks_like_icmp(const char *str) {
	int i;

	// One +/- for the comparison flag, one for the sign
	for (i = 0; i < 2; ++i) {
		if (str[i] != '-' && str[i] != '+') {
			break;
		}
	}

	return str[i] >= '0' && str[i] <= '9';
}

/**
 * Parse a single flag.
 */
static struct expr *parse_flag(struct parser_state *state, size_t argc) {
	parser_advance(state, T_FLAG, argc);
	return &expr_true;
}

/**
 * Parse a flag that doesn't take a value.
 */
static struct expr *parse_nullary_flag(struct parser_state *state) {
	return parse_flag(state, 1);
}

/**
 * Parse a flag that takes a single value.
 */
static struct expr *parse_unary_flag(struct parser_state *state) {
	return parse_flag(state, 2);
}

/**
 * Parse a single option.
 */
static struct expr *parse_option(struct parser_state *state, size_t argc) {
	const char *arg = *parser_advance(state, T_OPTION, argc);

	if (state->warn && state->non_option_seen) {
		parse_warning(state,
		              "The '%s' option applies to the entire command line.  For clarity, place\n"
		              "it before any non-option arguments.\n\n",
		              arg);
	}


	return &expr_true;
}

/**
 * Parse an option that doesn't take a value.
 */
static struct expr *parse_nullary_option(struct parser_state *state) {
	return parse_option(state, 1);
}

/**
 * Parse an option that takes a value.
 */
static struct expr *parse_unary_option(struct parser_state *state) {
	return parse_option(state, 2);
}

/**
 * Parse a single positional option.
 */
static struct expr *parse_positional_option(struct parser_state *state, size_t argc) {
	parser_advance(state, T_OPTION, argc);
	return &expr_true;
}

/**
 * Parse a positional option that doesn't take a value.
 */
static struct expr *parse_nullary_positional_option(struct parser_state *state) {
	return parse_positional_option(state, 1);
}

/**
 * Parse a positional option that takes a single value.
 */
static struct expr *parse_unary_positional_option(struct parser_state *state, const char **value) {
	const char *arg = state->argv[0];
	*value = state->argv[1];
	if (!*value) {
		parse_error(state, "%s needs a value.\n", arg);
		return NULL;
	}

	return parse_positional_option(state, 2);
}

/**
 * Parse a single test.
 */
static struct expr *parse_test(struct parser_state *state, eval_fn *eval, size_t argc) {
	char **argv = parser_advance(state, T_TEST, argc);
	struct expr *expr = new_expr(eval, argc, argv);
	if (expr) {
		expr->pure = true;
	}
	return expr;
}

/**
 * Parse a test that doesn't take a value.
 */
static struct expr *parse_nullary_test(struct parser_state *state, eval_fn *eval) {
	return parse_test(state, eval, 1);
}

/**
 * Parse a test that takes a value.
 */
static struct expr *parse_unary_test(struct parser_state *state, eval_fn *eval) {
	const char *arg = state->argv[0];
	const char *value = state->argv[1];
	if (!value) {
		parse_error(state, "%s needs a value.\n", arg);
		return NULL;
	}

	struct expr *expr = parse_test(state, eval, 2);
	if (expr) {
		expr->sdata = value;
	}
	return expr;
}

/**
 * Parse a single action.
 */
static struct expr *parse_action(struct parser_state *state, eval_fn *eval, size_t argc) {
	if (eval != eval_nohidden && eval != eval_prune && eval != eval_quit) {
		state->implicit_print = false;
	}

	char **argv = parser_advance(state, T_ACTION, argc);
	return new_expr(eval, argc, argv);
}

/**
 * Parse an action that takes no arguments.
 */
static struct expr *parse_nullary_action(struct parser_state *state, eval_fn *eval) {
	return parse_action(state, eval, 1);
}

/**
 * Parse an action that takes one argument.
 */
static struct expr *parse_unary_action(struct parser_state *state, eval_fn *eval) {
	const char *arg = state->argv[0];
	const char *value = state->argv[1];
	if (!value) {
		parse_error(state, "%s needs a value.\n", arg);
		return NULL;
	}

	struct expr *expr = parse_action(state, eval, 2);
	if (expr) {
		expr->sdata = value;
	}
	return expr;
}

/**
 * Parse a test expression with integer data and a comparison flag.
 */
static struct expr *parse_test_icmp(struct parser_state *state, eval_fn *eval) {
	struct expr *expr = parse_unary_test(state, eval);
	if (!expr) {
		return NULL;
	}

	if (!parse_icmp(state, expr->sdata, expr, 0)) {
		free_expr(expr);
		return NULL;
	}

	return expr;
}

/**
 * Print usage information for -D.
 */
static void debug_help(CFILE *cfile) {
	cfprintf(cfile, "Supported debug flags:\n\n");

	cfprintf(cfile, "  ${bld}help${rs}:   This message.\n");
	cfprintf(cfile, "  ${bld}cost${rs}:   Show cost estimates.\n");
	cfprintf(cfile, "  ${bld}exec${rs}:   Print executed command details.\n");
	cfprintf(cfile, "  ${bld}opt${rs}:    Print optimization details.\n");
	cfprintf(cfile, "  ${bld}rates${rs}:  Print predicate success rates.\n");
	cfprintf(cfile, "  ${bld}search${rs}: Trace the filesystem traversal.\n");
	cfprintf(cfile, "  ${bld}stat${rs}:   Trace all stat() calls.\n");
	cfprintf(cfile, "  ${bld}tree${rs}:   Print the parse tree.\n");
	cfprintf(cfile, "  ${bld}all${rs}:    All debug flags at once.\n");
}

/** A named debug flag. */
struct debug_flag {
	enum debug_flags flag;
	const char *name;
};

/** The table of debug flags. */
struct debug_flag debug_flags[] = {
	{DEBUG_ALL,    "all"},
	{DEBUG_COST,   "cost"},
	{DEBUG_EXEC,   "exec"},
	{DEBUG_OPT,    "opt"},
	{DEBUG_RATES,  "rates"},
	{DEBUG_SEARCH, "search"},
	{DEBUG_STAT,   "stat"},
	{DEBUG_TREE,   "tree"},
	{0},
};

/** Check if a substring matches a debug flag. */
static bool parse_debug_flag(const char *flag, size_t len, const char *expected) {
	if (len == strlen(expected)) {
		return strncmp(flag, expected, len) == 0;
	} else {
		return false;
	}
}

/**
 * Parse -D FLAG.
 */
static struct expr *parse_debug(struct parser_state *state, int arg1, int arg2) {
	struct cmdline *cmdline = state->cmdline;

	const char *arg = state->argv[0];
	const char *flags = state->argv[1];
	if (!flags) {
		parse_error(state, "%s needs a flag.\n\n", arg);
		debug_help(cmdline->cerr);
		return NULL;
	}

	bool unrecognized = false;

	for (const char *flag = flags, *next; flag; flag = next) {
		size_t len = strcspn(flag, ",");
		if (flag[len]) {
			next = flag + len + 1;
		} else {
			next = NULL;
		}

		if (parse_debug_flag(flag, len, "help")) {
			debug_help(cmdline->cout);
			state->just_info = true;
			return NULL;
		}

		for (int i = 0; ; ++i) {
			const char *expected = debug_flags[i].name;
			if (!expected) {
				parse_warning(state, "Unrecognized debug flag '");
				fwrite(flag, 1, len, stderr);
				fputs("'\n\n", stderr);
				unrecognized = true;
				break;
			}

			if (parse_debug_flag(flag, len, expected)) {
				cmdline->debug |= debug_flags[i].flag;
				break;
			}
		}
	}

	if (unrecognized) {
		debug_help(cmdline->cerr);
		cfprintf(cmdline->cerr, "\n");
	}

	return parse_unary_flag(state);
}

/**
 * Parse -On.
 */
static struct expr *parse_optlevel(struct parser_state *state, int arg1, int arg2) {
	int *optlevel = &state->cmdline->optlevel;

	if (strcmp(state->argv[0], "-Ofast") == 0) {
		*optlevel = 4;
	} else if (!parse_int(state, state->argv[0] + 2, optlevel, IF_INT | IF_UNSIGNED)) {
		return NULL;
	}

	if (state->warn && *optlevel > 4) {
		parse_warning(state, "%s is the same as -O4.\n\n", state->argv[0]);
	}

	return parse_nullary_flag(state);
}

/**
 * Parse -[PHL], -(no)?follow.
 */
static struct expr *parse_follow(struct parser_state *state, int flags, int option) {
	struct cmdline *cmdline = state->cmdline;
	cmdline->flags &= ~(BFTW_COMFOLLOW | BFTW_LOGICAL);
	cmdline->flags |= flags;
	if (option) {
		return parse_nullary_positional_option(state);
	} else {
		return parse_nullary_flag(state);
	}
}

/**
 * Parse -X.
 */
static struct expr *parse_xargs_safe(struct parser_state *state, int arg1, int arg2) {
	state->cmdline->xargs_safe = true;
	return parse_nullary_flag(state);
}

/**
 * Parse -executable, -readable, -writable
 */
static struct expr *parse_access(struct parser_state *state, int flag, int arg2) {
	struct expr *expr = parse_nullary_test(state, eval_access);
	if (!expr) {
		return NULL;
	}

	expr->idata = flag;
	expr->cost = STAT_COST;

	switch (flag) {
	case R_OK:
		expr->probability = 0.99;
		break;
	case W_OK:
		expr->probability = 0.8;
		break;
	case X_OK:
		expr->probability = 0.2;
		break;
	}

	return expr;
}

/**
 * Parse -acl.
 */
static struct expr *parse_acl(struct parser_state *state, int flag, int arg2) {
#if BFS_CAN_CHECK_ACL
	struct expr *expr = parse_nullary_test(state, eval_acl);
	if (expr) {
		expr->cost = STAT_COST;
		expr->probability = 0.00002;
	}
	return expr;
#else
	parse_error(state, "%s is missing platform support.\n", state->argv[0]);
	return NULL;
#endif
}

/**
 * Parse -[aBcm]?newer.
 */
static struct expr *parse_newer(struct parser_state *state, int field, int arg2) {
	struct expr *expr = parse_unary_test(state, eval_newer);
	if (!expr) {
		return NULL;
	}

	struct bfs_stat sb;
	if (stat_arg(state, expr, &sb) != 0) {
		goto fail;
	}

	expr->cost = STAT_COST;
	expr->reftime = sb.mtime;
	expr->stat_field = field;
	return expr;

fail:
	free_expr(expr);
	return NULL;
}

/**
 * Parse -[aBcm]{min,time}.
 */
static struct expr *parse_time(struct parser_state *state, int field, int unit) {
	struct expr *expr = parse_test_icmp(state, eval_time);
	if (!expr) {
		return NULL;
	}

	expr->cost = STAT_COST;
	expr->reftime = state->now;
	expr->stat_field = field;
	expr->time_unit = unit;
	return expr;
}

/**
 * Parse -capable.
 */
static struct expr *parse_capable(struct parser_state *state, int flag, int arg2) {
#if BFS_CAN_CHECK_CAPABILITIES
	struct expr *expr = parse_nullary_test(state, eval_capable);
	if (expr) {
		expr->cost = STAT_COST;
		expr->probability = 0.000002;
	}
	return expr;
#else
	parse_error(state, "%s is missing platform support.\n", state->argv[0]);
	return NULL;
#endif
}

/**
 * Parse -(no)?color.
 */
static struct expr *parse_color(struct parser_state *state, int color, int arg2) {
	struct cmdline *cmdline = state->cmdline;
	struct colors *colors = cmdline->colors;
	if (color) {
		state->use_color = COLOR_ALWAYS;
		cmdline->cout->colors = colors;
		cmdline->cerr->colors = colors;
	} else {
		state->use_color = COLOR_NEVER;
		cmdline->cout->colors = NULL;
		cmdline->cerr->colors = NULL;
	}
	return parse_nullary_option(state);
}

/**
 * Parse -{false,true}.
 */
static struct expr *parse_const(struct parser_state *state, int value, int arg2) {
	parser_advance(state, T_TEST, 1);
	return value ? &expr_true : &expr_false;
}

/**
 * Parse -daystart.
 */
static struct expr *parse_daystart(struct parser_state *state, int arg1, int arg2) {
	struct tm tm;
	if (xlocaltime(&state->now.tv_sec, &tm) != 0) {
		perror("xlocaltime()");
		return NULL;
	}

	if (tm.tm_hour || tm.tm_min || tm.tm_sec || state->now.tv_nsec) {
		++tm.tm_mday;
	}
	tm.tm_hour = 0;
	tm.tm_min = 0;
	tm.tm_sec = 0;

	time_t time = mktime(&tm);
	if (time == -1) {
		perror("mktime()");
		return NULL;
	}

	state->now.tv_sec = time;
	state->now.tv_nsec = 0;

	return parse_nullary_positional_option(state);
}

/**
 * Parse -delete.
 */
static struct expr *parse_delete(struct parser_state *state, int arg1, int arg2) {
	state->cmdline->flags |= BFTW_DEPTH;
	state->depth_arg = state->argv[0];
	return parse_nullary_action(state, eval_delete);
}

/**
 * Parse -d.
 */
static struct expr *parse_depth(struct parser_state *state, int arg1, int arg2) {
	state->cmdline->flags |= BFTW_DEPTH;
	state->depth_arg = state->argv[0];
	return parse_nullary_flag(state);
}

/**
 * Parse -depth [N].
 */
static struct expr *parse_depth_n(struct parser_state *state, int arg1, int arg2) {
	const char *arg = state->argv[1];
	if (arg && looks_like_icmp(arg)) {
		return parse_test_icmp(state, eval_depth);
	} else {
		return parse_depth(state, arg1, arg2);
	}
}

/**
 * Parse -{min,max}depth N.
 */
static struct expr *parse_depth_limit(struct parser_state *state, int is_min, int arg2) {
	struct cmdline *cmdline = state->cmdline;
	const char *arg = state->argv[0];
	const char *value = state->argv[1];
	if (!value) {
		parse_error(state, "%s needs a value.\n", arg);
		return NULL;
	}

	int *depth = is_min ? &cmdline->mindepth : &cmdline->maxdepth;
	if (!parse_int(state, value, depth, IF_INT | IF_UNSIGNED)) {
		return NULL;
	}

	return parse_unary_option(state);
}

/**
 * Parse -empty.
 */
static struct expr *parse_empty(struct parser_state *state, int arg1, int arg2) {
	struct expr *expr = parse_nullary_test(state, eval_empty);
	if (!expr) {
		return NULL;
	}

	expr->cost = 2000.0;
	expr->probability = 0.01;

	if (state->cmdline->optlevel < 4) {
		// Since -empty attempts to open and read directories, it may
		// have side effects such as reporting permission errors, and
		// thus shouldn't be re-ordered without aggressive optimizations
		expr->pure = false;
	}

	expr->ephemeral_fds = 1;

	return expr;
}

/**
 * Parse -exec(dir)?/-ok(dir)?.
 */
static struct expr *parse_exec(struct parser_state *state, int flags, int arg2) {
	struct bfs_exec *execbuf = parse_bfs_exec(state->argv, flags, state->cmdline);
	if (!execbuf) {
		return NULL;
	}

	struct expr *expr = parse_action(state, eval_exec, execbuf->tmpl_argc + 2);
	if (!expr) {
		free_bfs_exec(execbuf);
		return NULL;
	}

	expr->execbuf = execbuf;

	if (execbuf->flags & BFS_EXEC_MULTI) {
		expr_set_always_true(expr);
	} else {
		expr->cost = 1000000.0;
	}

	expr->ephemeral_fds = 2;
	if (execbuf->flags & BFS_EXEC_CHDIR) {
		if (execbuf->flags & BFS_EXEC_MULTI) {
			expr->persistent_fds = 1;
		} else {
			++expr->ephemeral_fds;
		}
	}

	return expr;
}

/**
 * Parse -exit [STATUS].
 */
static struct expr *parse_exit(struct parser_state *state, int arg1, int arg2) {
	size_t argc = 1;
	const char *value = state->argv[1];

	int status = EXIT_SUCCESS;
	if (value && parse_int(state, value, &status, IF_INT | IF_UNSIGNED | IF_QUIET)) {
		argc = 2;
	}

	struct expr *expr = parse_action(state, eval_exit, argc);
	if (expr) {
		expr_set_never_returns(expr);
		expr->idata = status;
	}
	return expr;
}

/**
 * Parse -f PATH.
 */
static struct expr *parse_f(struct parser_state *state, int arg1, int arg2) {
	parser_advance(state, T_FLAG, 1);

	const char *path = state->argv[0];
	if (!path) {
		parse_error(state, "-f requires a path.\n");
		return NULL;
	}

	if (parse_root(state, path) != 0) {
		return NULL;
	}

	parser_advance(state, T_PATH, 1);
	return &expr_true;
}

/**
 * Parse -fls FILE.
 */
static struct expr *parse_fls(struct parser_state *state, int arg1, int arg2) {
	struct expr *expr = parse_unary_action(state, eval_fls);
	if (expr) {
		expr_set_always_true(expr);
		expr->cost = PRINT_COST;
		if (expr_open(state, expr, expr->sdata) != 0) {
			goto fail;
		}
		expr->reftime = state->now;
	}
	return expr;

fail:
	free_expr(expr);
	return NULL;
}

/**
 * Parse -fprint FILE.
 */
static struct expr *parse_fprint(struct parser_state *state, int arg1, int arg2) {
	struct expr *expr = parse_unary_action(state, eval_fprint);
	if (expr) {
		expr_set_always_true(expr);
		expr->cost = PRINT_COST;
		if (expr_open(state, expr, expr->sdata) != 0) {
			goto fail;
		}
	}
	return expr;

fail:
	free_expr(expr);
	return NULL;
}

/**
 * Parse -fprint0 FILE.
 */
static struct expr *parse_fprint0(struct parser_state *state, int arg1, int arg2) {
	struct expr *expr = parse_unary_action(state, eval_fprint0);
	if (expr) {
		expr_set_always_true(expr);
		expr->cost = PRINT_COST;
		if (expr_open(state, expr, expr->sdata) != 0) {
			goto fail;
		}
	}
	return expr;

fail:
	free_expr(expr);
	return NULL;
}

/**
 * Parse -fprintf FILE FORMAT.
 */
static struct expr *parse_fprintf(struct parser_state *state, int arg1, int arg2) {
	const char *arg = state->argv[0];

	const char *file = state->argv[1];
	if (!file) {
		parse_error(state, "%s needs a file.\n", arg);
		return NULL;
	}

	const char *format = state->argv[2];
	if (!format) {
		parse_error(state, "%s needs a format string.\n", arg);
		return NULL;
	}

	struct expr *expr = parse_action(state, eval_fprintf, 3);
	if (!expr) {
		return NULL;
	}

	expr_set_always_true(expr);

	expr->cost = PRINT_COST;

	if (expr_open(state, expr, file) != 0) {
		goto fail;
	}

	expr->printf = parse_bfs_printf(format, state->cmdline);
	if (!expr->printf) {
		goto fail;
	}

	return expr;

fail:
	free_expr(expr);
	return NULL;
}

/**
 * Parse -fstype TYPE.
 */
static struct expr *parse_fstype(struct parser_state *state, int arg1, int arg2) {
	struct cmdline *cmdline = state->cmdline;
	if (!cmdline->mtab) {
		parse_error(state, "Couldn't parse the mount table: %s.\n", strerror(cmdline->mtab_error));
		return NULL;
	}

	struct expr *expr = parse_unary_test(state, eval_fstype);
	if (expr) {
		expr->cost = STAT_COST;
	}
	return expr;
}

/**
 * Parse -gid/-group.
 */
static struct expr *parse_group(struct parser_state *state, int arg1, int arg2) {
	const char *arg = state->argv[0];

	struct expr *expr = parse_unary_test(state, eval_gid);
	if (!expr) {
		return NULL;
	}

	struct group *grp = getgrnam(expr->sdata);
	if (grp) {
		expr->idata = grp->gr_gid;
		expr->cmp_flag = CMP_EXACT;
	} else if (looks_like_icmp(expr->sdata)) {
		if (!parse_icmp(state, expr->sdata, expr, 0)) {
			goto fail;
		}
	} else {
		parse_error(state, "%s %s: No such group.\n", arg, expr->sdata);
		goto fail;
	}

	expr->cost = STAT_COST;

	return expr;

fail:
	free_expr(expr);
	return NULL;
}

/**
 * Parse -unique.
 */
static struct expr *parse_unique(struct parser_state *state, int arg1, int arg2) {
	state->cmdline->unique = true;
	return parse_nullary_option(state);
}

/**
 * Parse -used N.
 */
static struct expr *parse_used(struct parser_state *state, int arg1, int arg2) {
	struct expr *expr = parse_test_icmp(state, eval_used);
	if (expr) {
		expr->cost = STAT_COST;
	}
	return expr;
}

/**
 * Parse -uid/-user.
 */
static struct expr *parse_user(struct parser_state *state, int arg1, int arg2) {
	const char *arg = state->argv[0];

	struct expr *expr = parse_unary_test(state, eval_uid);
	if (!expr) {
		return NULL;
	}

	struct passwd *pwd = getpwnam(expr->sdata);
	if (pwd) {
		expr->idata = pwd->pw_uid;
		expr->cmp_flag = CMP_EXACT;
	} else if (looks_like_icmp(expr->sdata)) {
		if (!parse_icmp(state, expr->sdata, expr, 0)) {
			goto fail;
		}
	} else {
		parse_error(state, "%s %s: No such user.\n", arg, expr->sdata);
		goto fail;
	}

	expr->cost = STAT_COST;

	return expr;

fail:
	free_expr(expr);
	return NULL;
}

/**
 * Parse -hidden.
 */
static struct expr *parse_hidden(struct parser_state *state, int arg1, int arg2) {
	struct expr *expr = parse_nullary_test(state, eval_hidden);
	if (expr) {
		expr->probability = 0.01;
	}
	return expr;
}

/**
 * Parse -(no)?ignore_readdir_race.
 */
static struct expr *parse_ignore_races(struct parser_state *state, int ignore, int arg2) {
	state->cmdline->ignore_races = ignore;
	return parse_nullary_option(state);
}

/**
 * Parse -inum N.
 */
static struct expr *parse_inum(struct parser_state *state, int arg1, int arg2) {
	struct expr *expr = parse_test_icmp(state, eval_inum);
	if (expr) {
		expr->cost = STAT_COST;
		expr->probability = expr->cmp_flag == CMP_EXACT ? 0.01 : 0.50;
	}
	return expr;
}

/**
 * Parse -links N.
 */
static struct expr *parse_links(struct parser_state *state, int arg1, int arg2) {
	struct expr *expr = parse_test_icmp(state, eval_links);
	if (expr) {
		expr->cost = STAT_COST;
		expr->probability = expr_cmp(expr, 1) ? 0.99 : 0.01;
	}
	return expr;
}

/**
 * Parse -ls.
 */
static struct expr *parse_ls(struct parser_state *state, int arg1, int arg2) {
	struct expr *expr = parse_nullary_action(state, eval_fls);
	if (expr) {
		init_print_expr(state, expr);
		expr->reftime = state->now;
	}
	return expr;
}

/**
 * Parse -mount, -xdev.
 */
static struct expr *parse_mount(struct parser_state *state, int arg1, int arg2) {
	state->cmdline->flags |= BFTW_XDEV;
	return parse_nullary_option(state);
}

/**
 * Common code for fnmatch() tests.
 */
static struct expr *parse_fnmatch(const struct parser_state *state, struct expr *expr, bool casefold) {
	if (!expr) {
		return NULL;
	}

	if (casefold) {
#ifdef FNM_CASEFOLD
		expr->idata = FNM_CASEFOLD;
#else
		parse_error(state, "%s is missing platform support.\n", expr->argv[0]);
		free_expr(expr);
		return NULL;
#endif
	} else {
		expr->idata = 0;
	}

	expr->cost = 400.0;

	if (strchr(expr->sdata, '*')) {
		expr->probability = 0.5;
	} else {
		expr->probability = 0.1;
	}

	return expr;
}

/**
 * Parse -i?name.
 */
static struct expr *parse_name(struct parser_state *state, int casefold, int arg2) {
	struct expr *expr = parse_unary_test(state, eval_name);
	return parse_fnmatch(state, expr, casefold);
}

/**
 * Parse -i?path, -i?wholename.
 */
static struct expr *parse_path(struct parser_state *state, int casefold, int arg2) {
	struct expr *expr = parse_unary_test(state, eval_path);
	return parse_fnmatch(state, expr, casefold);
}

/**
 * Parse -i?lname.
 */
static struct expr *parse_lname(struct parser_state *state, int casefold, int arg2) {
	struct expr *expr = parse_unary_test(state, eval_lname);
	return parse_fnmatch(state, expr, casefold);
}

/** Get the bfs_stat_field for X/Y in -newerXY */
static enum bfs_stat_field parse_newerxy_field(char c) {
	switch (c) {
	case 'a':
		return BFS_STAT_ATIME;
	case 'B':
		return BFS_STAT_BTIME;
	case 'c':
		return BFS_STAT_CTIME;
	case 'm':
		return BFS_STAT_MTIME;
	default:
		return 0;
	}
}

/**
 * Parse -newerXY.
 */
static struct expr *parse_newerxy(struct parser_state *state, int arg1, int arg2) {
	const char *arg = state->argv[0];
	if (strlen(arg) != 8) {
		parse_error(state, "Expected -newerXY; found %s.\n", arg);
		return NULL;
	}

	struct expr *expr = parse_unary_test(state, eval_newer);
	if (!expr) {
		goto fail;
	}

	expr->stat_field = parse_newerxy_field(arg[6]);
	if (!expr->stat_field) {
		parse_error(state, "%s: For -newerXY, X should be 'a', 'c', 'm', or 'B'.\n", arg);
		goto fail;
	}

	if (arg[7] == 't') {
		parse_error(state, "%s: Explicit reference times ('t') are not supported.\n", arg);
		goto fail;
	} else {
		enum bfs_stat_field field = parse_newerxy_field(arg[7]);
		if (!field) {
			parse_error(state, "%s: For -newerXY, Y should be 'a', 'c', 'm', 'B', or 't'.\n", arg);
			goto fail;
		}

		struct bfs_stat sb;
		if (stat_arg(state, expr, &sb) != 0) {
			goto fail;
		}


		const struct timespec *reftime = bfs_stat_time(&sb, field);
		if (!reftime) {
			parse_error(state, "'%s': Couldn't get file %s.\n", expr->sdata, bfs_stat_field_name(field));
			goto fail;
		}

		expr->reftime = *reftime;
	}

	expr->cost = STAT_COST;

	return expr;

fail:
	free_expr(expr);
	return NULL;
}

/**
 * Parse -nogroup.
 */
static struct expr *parse_nogroup(struct parser_state *state, int arg1, int arg2) {
	struct expr *expr = parse_nullary_test(state, eval_nogroup);
	if (expr) {
		expr->cost = 9000.0;
		expr->probability = 0.01;
		expr->ephemeral_fds = 1;
	}
	return expr;
}

/**
 * Parse -nohidden.
 */
static struct expr *parse_nohidden(struct parser_state *state, int arg1, int arg2) {
	state->prune_arg = state->argv[0];
	return parse_nullary_action(state, eval_nohidden);
}

/**
 * Parse -noleaf.
 */
static struct expr *parse_noleaf(struct parser_state *state, int arg1, int arg2) {
	if (state->warn) {
		parse_warning(state, "bfs does not apply the optimization that %s inhibits.\n\n", state->argv[0]);
	}

	return parse_nullary_option(state);
}

/**
 * Parse -nouser.
 */
static struct expr *parse_nouser(struct parser_state *state, int arg1, int arg2) {
	struct expr *expr = parse_nullary_test(state, eval_nouser);
	if (expr) {
		expr->cost = 9000.0;
		expr->probability = 0.01;
		expr->ephemeral_fds = 1;
	}
	return expr;
}

/**
 * Parse a permission mode like chmod(1).
 */
static int parse_mode(const struct parser_state *state, const char *mode, struct expr *expr) {
	if (mode[0] >= '0' && mode[0] <= '9') {
		unsigned int parsed;
		if (!parse_int(state, mode, &parsed, 8 | IF_INT | IF_UNSIGNED | IF_QUIET)) {
			goto fail;
		}
		if (parsed > 07777) {
			goto fail;
		}

		expr->file_mode = parsed;
		expr->dir_mode = parsed;
		return 0;
	}

	expr->file_mode = 0;
	expr->dir_mode = 0;

	// Parse the same grammar as chmod(1), which looks like this:
	//
	// MODE : CLAUSE ["," CLAUSE]*
	//
	// CLAUSE : WHO* ACTION+
	//
	// WHO : "u" | "g" | "o" | "a"
	//
	// ACTION : OP PERM*
	//        | OP PERMCOPY
	//
	// OP : "+" | "-" | "="
	//
	// PERM : "r" | "w" | "x" | "X" | "s" | "t"
	//
	// PERMCOPY : "u" | "g" | "o"

	// State machine state
	enum {
		MODE_CLAUSE,
		MODE_WHO,
		MODE_ACTION,
		MODE_ACTION_APPLY,
		MODE_OP,
		MODE_PERM,
	} mstate = MODE_CLAUSE;

	enum {
		MODE_PLUS,
		MODE_MINUS,
		MODE_EQUALS,
	} op;

	mode_t who;
	mode_t file_change;
	mode_t dir_change;

	const char *i = mode;
	while (true) {
		switch (mstate) {
		case MODE_CLAUSE:
			who = 0;
			mstate = MODE_WHO;
			// Fallthrough

		case MODE_WHO:
			switch (*i) {
			case 'u':
				who |= 0700;
				break;
			case 'g':
				who |= 0070;
				break;
			case 'o':
				who |= 0007;
				break;
			case 'a':
				who |= 0777;
				break;
			default:
				mstate = MODE_ACTION;
				continue;
			}
			break;

		case MODE_ACTION_APPLY:
			switch (op) {
			case MODE_EQUALS:
				expr->file_mode &= ~who;
				expr->dir_mode &= ~who;
				// Fallthrough
			case MODE_PLUS:
				expr->file_mode |= file_change;
				expr->dir_mode |= dir_change;
				break;
			case MODE_MINUS:
				expr->file_mode &= ~file_change;
				expr->dir_mode &= ~dir_change;
				break;
			}
			// Fallthrough

		case MODE_ACTION:
			if (who == 0) {
				who = 0777;
			}

			switch (*i) {
			case '+':
				op = MODE_PLUS;
				mstate = MODE_OP;
				break;
			case '-':
				op = MODE_MINUS;
				mstate = MODE_OP;
				break;
			case '=':
				op = MODE_EQUALS;
				mstate = MODE_OP;
				break;

			case ',':
				if (mstate == MODE_ACTION_APPLY) {
					mstate = MODE_CLAUSE;
				} else {
					goto fail;
				}
				break;

			case '\0':
				if (mstate == MODE_ACTION_APPLY) {
					goto done;
				} else {
					goto fail;
				}

			default:
				goto fail;
			}
			break;

		case MODE_OP:
			switch (*i) {
			case 'u':
				file_change = (expr->file_mode >> 6) & 07;
				dir_change = (expr->dir_mode >> 6) & 07;
				break;
			case 'g':
				file_change = (expr->file_mode >> 3) & 07;
				dir_change = (expr->dir_mode >> 3) & 07;
				break;
			case 'o':
				file_change = expr->file_mode & 07;
				dir_change = expr->dir_mode & 07;
				break;

			default:
				file_change = 0;
				dir_change = 0;
				mstate = MODE_PERM;
				continue;
			}

			file_change |= (file_change << 6) | (file_change << 3);
			file_change &= who;
			dir_change |= (dir_change << 6) | (dir_change << 3);
			dir_change &= who;
			mstate = MODE_ACTION_APPLY;
			break;

		case MODE_PERM:
			switch (*i) {
			case 'r':
				file_change |= who & 0444;
				dir_change |= who & 0444;
				break;
			case 'w':
				file_change |= who & 0222;
				dir_change |= who & 0222;
				break;
			case 'x':
				file_change |= who & 0111;
				// Fallthrough
			case 'X':
				dir_change |= who & 0111;
				break;
			case 's':
				if (who & 0700) {
					file_change |= S_ISUID;
					dir_change |= S_ISUID;
				}
				if (who & 0070) {
					file_change |= S_ISGID;
					dir_change |= S_ISGID;
				}
				break;
			case 't':
				file_change |= S_ISVTX;
				dir_change |= S_ISVTX;
				break;
			default:
				mstate = MODE_ACTION_APPLY;
				continue;
			}
			break;
		}

		++i;
	}

done:
	return 0;

fail:
	parse_error(state, "'%s' is an invalid mode.\n", mode);
	return -1;
}

/**
 * Parse -perm MODE.
 */
static struct expr *parse_perm(struct parser_state *state, int field, int arg2) {
	struct expr *expr = parse_unary_test(state, eval_perm);
	if (!expr) {
		return NULL;
	}

	const char *mode = expr->sdata;
	switch (mode[0]) {
	case '-':
		expr->mode_cmp = MODE_ALL;
		++mode;
		break;
	case '/':
		expr->mode_cmp = MODE_ANY;
		++mode;
		break;
	case '+':
		if (mode[1] >= '0' && mode[1] <= '9') {
			expr->mode_cmp = MODE_ANY;
			++mode;
			break;
		}
		// Fallthrough
	default:
		expr->mode_cmp = MODE_EXACT;
		break;
	}

	if (parse_mode(state, mode, expr) != 0) {
		goto fail;
	}

	expr->cost = STAT_COST;

	return expr;

fail:
	free_expr(expr);
	return NULL;
}

/**
 * Parse -print.
 */
static struct expr *parse_print(struct parser_state *state, int arg1, int arg2) {
	struct expr *expr = parse_nullary_action(state, eval_fprint);
	if (expr) {
		init_print_expr(state, expr);
	}
	return expr;
}

/**
 * Parse -print0.
 */
static struct expr *parse_print0(struct parser_state *state, int arg1, int arg2) {
	struct expr *expr = parse_nullary_action(state, eval_fprint0);
	if (expr) {
		init_print_expr(state, expr);
	}
	return expr;
}

/**
 * Parse -printf FORMAT.
 */
static struct expr *parse_printf(struct parser_state *state, int arg1, int arg2) {
	struct expr *expr = parse_unary_action(state, eval_fprintf);
	if (!expr) {
		return NULL;
	}

	init_print_expr(state, expr);

	expr->printf = parse_bfs_printf(expr->sdata, state->cmdline);
	if (!expr->printf) {
		free_expr(expr);
		return NULL;
	}

	return expr;
}

/**
 * Parse -printx.
 */
static struct expr *parse_printx(struct parser_state *state, int arg1, int arg2) {
	struct expr *expr = parse_nullary_action(state, eval_fprintx);
	if (expr) {
		init_print_expr(state, expr);
	}
	return expr;
}

/**
 * Parse -prune.
 */
static struct expr *parse_prune(struct parser_state *state, int arg1, int arg2) {
	state->prune_arg = state->argv[0];

	struct expr *expr = parse_nullary_action(state, eval_prune);
	if (expr) {
		expr_set_always_true(expr);
	}
	return expr;
}

/**
 * Parse -quit.
 */
static struct expr *parse_quit(struct parser_state *state, int arg1, int arg2) {
	struct expr *expr = parse_nullary_action(state, eval_quit);
	if (expr) {
		expr_set_never_returns(expr);
	}
	return expr;
}

/**
 * Parse -i?regex.
 */
static struct expr *parse_regex(struct parser_state *state, int flags, int arg2) {
	struct expr *expr = parse_unary_test(state, eval_regex);
	if (!expr) {
		goto fail;
	}

	expr->regex = malloc(sizeof(regex_t));
	if (!expr->regex) {
		perror("malloc()");
		goto fail;
	}

	int err = regcomp(expr->regex, expr->sdata, state->regex_flags | flags);
	if (err != 0) {
		char *str = xregerror(err, expr->regex);
		if (str) {
			parse_error(state, "%s %s: %s.\n", expr->argv[0], expr->argv[1], str);
			free(str);
		} else {
			perror("xregerror()");
		}
		goto fail_regex;
	}

	return expr;

fail_regex:
	free(expr->regex);
	expr->regex = NULL;
fail:
	free_expr(expr);
	return NULL;
}

/**
 * Parse -E.
 */
static struct expr *parse_regex_extended(struct parser_state *state, int arg1, int arg2) {
	state->regex_flags = REG_EXTENDED;
	return parse_nullary_flag(state);
}

/**
 * Parse -regextype TYPE.
 */
static struct expr *parse_regextype(struct parser_state *state, int arg1, int arg2) {
	const char *type;
	struct expr *expr = parse_unary_positional_option(state, &type);
	if (!expr) {
		goto fail;
	}

	FILE *file = stderr;

	if (strcmp(type, "posix-basic") == 0) {
		state->regex_flags = 0;
	} else if (strcmp(type, "posix-extended") == 0) {
		state->regex_flags = REG_EXTENDED;
	} else if (strcmp(type, "help") == 0) {
		state->just_info = true;
		file = stdout;
		goto fail_list_types;
	} else {
		goto fail_bad_type;
	}

	return expr;

fail_bad_type:
	parse_error(state, "Unsupported -regextype '%s'.\n\n", type);
fail_list_types:
	fputs("Supported types are:\n\n", file);
	fputs("  posix-basic:    POSIX basic regular expressions (BRE)\n", file);
	fputs("  posix-extended: POSIX extended regular expressions (ERE)\n", file);
fail:
	free_expr(expr);
	return NULL;
}

/**
 * Parse -samefile FILE.
 */
static struct expr *parse_samefile(struct parser_state *state, int arg1, int arg2) {
	struct expr *expr = parse_unary_test(state, eval_samefile);
	if (!expr) {
		return NULL;
	}

	struct bfs_stat sb;
	if (stat_arg(state, expr, &sb) != 0) {
		free_expr(expr);
		return NULL;
	}

	expr->dev = sb.dev;
	expr->ino = sb.ino;

	expr->cost = STAT_COST;
	expr->probability = 0.01;

	return expr;
}

/**
 * Parse -S STRATEGY.
 */
static struct expr *parse_search_strategy(struct parser_state *state, int arg1, int arg2) {
	const char *flag = state->argv[0];
	const char *arg = state->argv[1];
	if (!arg) {
		parse_error(state, "%s needs an argument.\n", flag);
		return NULL;
	}

	struct cmdline *cmdline = state->cmdline;
	FILE *file = stderr;

	if (strcmp(arg, "bfs") == 0) {
		cmdline->strategy = BFTW_BFS;
	} else if (strcmp(arg, "dfs") == 0) {
		cmdline->strategy = BFTW_DFS;
	} else if (strcmp(arg, "ids") == 0) {
		cmdline->strategy = BFTW_IDS;
	} else if (strcmp(arg, "help") == 0) {
		state->just_info = true;
		file = stdout;
		goto fail_list_strategies;
	} else {
		goto fail_bad_strategy;
	}

	return parse_unary_flag(state);

fail_bad_strategy:
	parse_error(state, "Unrecognized search strategy '%s'.\n\n", arg);
fail_list_strategies:
	fputs("Supported search strategies:\n\n", file);
	fputs("  bfs: breadth-first search\n", file);
	fputs("  dfs: depth-first search\n", file);
	fputs("  ids: iterative deepening search\n", file);
	return NULL;
}

/**
 * Parse -size N[cwbkMGTP]?.
 */
static struct expr *parse_size(struct parser_state *state, int arg1, int arg2) {
	struct expr *expr = parse_unary_test(state, eval_size);
	if (!expr) {
		return NULL;
	}

	const char *unit = parse_icmp(state, expr->sdata, expr, IF_PARTIAL_OK);
	if (!unit) {
		goto fail;
	}

	if (strlen(unit) > 1) {
		goto bad_unit;
	}

	switch (*unit) {
	case '\0':
	case 'b':
		expr->size_unit = SIZE_BLOCKS;
		break;
	case 'c':
		expr->size_unit = SIZE_BYTES;
		break;
	case 'w':
		expr->size_unit = SIZE_WORDS;
		break;
	case 'k':
		expr->size_unit = SIZE_KB;
		break;
	case 'M':
		expr->size_unit = SIZE_MB;
		break;
	case 'G':
		expr->size_unit = SIZE_GB;
		break;
	case 'T':
		expr->size_unit = SIZE_TB;
		break;
	case 'P':
		expr->size_unit = SIZE_PB;
		break;

	default:
		goto bad_unit;
	}

	expr->cost = STAT_COST;
	expr->probability = expr->cmp_flag == CMP_EXACT ? 0.01 : 0.50;

	return expr;

bad_unit:
	parse_error(state, "%s %s: Expected a size unit (one of cwbkMGTP); found '%s'.\n",
	            expr->argv[0], expr->argv[1], unit);
fail:
	free_expr(expr);
	return NULL;
}

/**
 * Parse -sparse.
 */
static struct expr *parse_sparse(struct parser_state *state, int arg1, int arg2) {
	struct expr *expr = parse_nullary_test(state, eval_sparse);
	if (expr) {
		expr->cost = STAT_COST;
	}
	return expr;
}

/**
 * Parse -x?type [bcdpflsD].
 */
static struct expr *parse_type(struct parser_state *state, int x, int arg2) {
	eval_fn *eval = x ? eval_xtype : eval_type;
	struct expr *expr = parse_unary_test(state, eval);
	if (!expr) {
		return NULL;
	}

	enum bftw_typeflag types = 0;
	double probability = 0.0;

	const char *c = expr->sdata;
	while (true) {
		enum bftw_typeflag type;
		double type_prob;

		switch (*c) {
		case 'b':
			type = BFTW_BLK;
			type_prob = 0.00000721183;
			break;
		case 'c':
			type = BFTW_CHR;
			type_prob = 0.0000499855;
			break;
		case 'd':
			type = BFTW_DIR;
			type_prob = 0.114475;
			break;
		case 'D':
			type = BFTW_DOOR;
			type_prob = 0.000001;
			break;
		case 'p':
			type = BFTW_FIFO;
			type_prob = 0.00000248684;
			break;
		case 'f':
			type = BFTW_REG;
			type_prob = 0.859772;
			break;
		case 'l':
			type = BFTW_LNK;
			type_prob = 0.0256816;
			break;
		case 's':
			type = BFTW_SOCK;
			type_prob = 0.0000116881;
			break;
		case 'w':
			type = BFTW_WHT;
			type_prob = 0.000001;
			break;

		case '\0':
			parse_error(state, "%s %s: Expected a type flag.\n", expr->argv[0], expr->argv[1]);
			goto fail;

		default:
			parse_error(state, "%s %s: Unknown type flag '%c' (expected one of [bcdpflsD]).\n",
			            expr->argv[0], expr->argv[1], *c);
			goto fail;
		}

		if (!(types & type)) {
			types |= type;
			probability += type_prob;
		}

		++c;
		if (*c == '\0') {
			break;
		} else if (*c == ',') {
			++c;
			continue;
		} else {
			parse_error(state, "%s %s: Types must be comma-separated.\n", expr->argv[0], expr->argv[1]);
			goto fail;
		}
	}

	expr->idata = types;
	expr->probability = probability;

	if (x && state->cmdline->optlevel < 4) {
		// Since -xtype dereferences symbolic links, it may have side
		// effects such as reporting permission errors, and thus
		// shouldn't be re-ordered without aggressive optimizations
		expr->pure = false;
	}

	return expr;

fail:
	free_expr(expr);
	return NULL;
}

/**
 * Parse -(no)?warn.
 */
static struct expr *parse_warn(struct parser_state *state, int warn, int arg2) {
	state->warn = warn;
	return parse_nullary_positional_option(state);
}

/**
 * Parse -xattr.
 */
static struct expr *parse_xattr(struct parser_state *state, int arg1, int arg2) {
#if BFS_CAN_CHECK_XATTRS
	struct expr *expr = parse_nullary_test(state, eval_xattr);
	if (expr) {
		expr->cost = STAT_COST;
		expr->probability = 0.01;
	}
	return expr;
#else
	parse_error(state, "%s is missing platform support.\n", state->argv[0]);
	return NULL;
#endif
}

/**
 * Launch a pager for the help output.
 */
static CFILE *launch_pager(pid_t *pid, CFILE *cout) {
	char *pager = getenv("PAGER");
	if (!pager) {
		pager = "more";
	}

	int pipefd[2];
	if (pipe(pipefd) != 0) {
		goto fail;
	}

	FILE *file = fdopen(pipefd[1], "w");
	if (!file) {
		goto fail_pipe;
	}
	pipefd[1] = -1;

	CFILE *ret = cfdup(file, NULL);
	if (!ret) {
		goto fail_file;
	}
	file = NULL;
	ret->close = true;
	ret->colors = cout->colors;

	struct bfs_spawn ctx;
	if (bfs_spawn_init(&ctx) != 0) {
		goto fail_ret;
	}

	if (bfs_spawn_setflags(&ctx, BFS_SPAWN_USEPATH) != 0) {
		goto fail_ctx;
	}

	if (bfs_spawn_addclose(&ctx, fileno(ret->file)) != 0) {
		goto fail_ctx;
	}
	if (bfs_spawn_adddup2(&ctx, pipefd[0], STDIN_FILENO) != 0) {
		goto fail_ctx;
	}
	if (bfs_spawn_addclose(&ctx, pipefd[0]) != 0) {
		goto fail_ctx;
	}

	char *argv[] = {
		pager,
		NULL,
	};

	extern char **environ;
	char **envp = environ;

	if (!getenv("LESS")) {
		size_t envc;
		for (envc = 0; environ[envc]; ++envc);
		++envc;

		envp = malloc((envc + 1)*sizeof(*envp));
		if (!envp) {
			goto fail_ctx;
		}

		memcpy(envp, environ, (envc - 1)*sizeof(*envp));
		envp[envc - 1] = "LESS=FKRX";
		envp[envc] = NULL;
	}

	*pid = bfs_spawn(pager, &ctx, argv, envp);
	if (*pid < 0) {
		goto fail_envp;
	}

	close(pipefd[0]);
	if (envp != environ) {
		free(envp);
	}
	bfs_spawn_destroy(&ctx);
	return ret;

fail_envp:
	if (envp != environ) {
		free(envp);
	}
fail_ctx:
	bfs_spawn_destroy(&ctx);
fail_ret:
	cfclose(ret);
fail_file:
	if (file) {
		fclose(file);
	}
fail_pipe:
	if (pipefd[1] >= 0) {
		close(pipefd[1]);
	}
	if (pipefd[0] >= 0) {
		close(pipefd[0]);
	}
fail:
	return cout;
}

/**
 * "Parse" -help.
 */
static struct expr *parse_help(struct parser_state *state, int arg1, int arg2) {
	CFILE *cout = state->cmdline->cout;

	pid_t pager = -1;
	if (state->stdout_tty) {
		cout = launch_pager(&pager, cout);
	}

	cfprintf(cout, "Usage: ${ex}%s${rs} [${cyn}flags${rs}...] [${mag}paths${rs}...] [${blu}expression${rs}...]\n\n",
		 state->command);

	cfprintf(cout, "${ex}bfs${rs} is compatible with ${ex}find${rs}, with some extensions. "
		       "${cyn}Flags${rs} (${cyn}-H${rs}/${cyn}-L${rs}/${cyn}-P${rs} etc.), ${mag}paths${rs},\n"
		       "and ${blu}expressions${rs} may be freely mixed in any order.\n\n");

	cfprintf(cout, "${bld}Flags:${rs}\n\n");

	cfprintf(cout, "  ${cyn}-H${rs}\n");
	cfprintf(cout, "      Follow symbolic links on the command line, but not while searching\n");
	cfprintf(cout, "  ${cyn}-L${rs}\n");
	cfprintf(cout, "      Follow all symbolic links\n");
	cfprintf(cout, "  ${cyn}-P${rs}\n");
	cfprintf(cout, "      Never follow symbolic links (the default)\n");

	cfprintf(cout, "  ${cyn}-E${rs}\n");
	cfprintf(cout, "      Use extended regular expressions (same as ${blu}-regextype${rs} ${bld}posix-extended${rs})\n");
	cfprintf(cout, "  ${cyn}-X${rs}\n");
	cfprintf(cout, "      Filter out files with non-${ex}xargs${rs}-safe names\n");
	cfprintf(cout, "  ${cyn}-d${rs}\n");
	cfprintf(cout, "      Search in post-order (same as ${blu}-depth${rs})\n");
	cfprintf(cout, "  ${cyn}-x${rs}\n");
	cfprintf(cout, "      Don't descend into other mount points (same as ${blu}-xdev${rs})\n");

	cfprintf(cout, "  ${cyn}-f${rs} ${mag}PATH${rs}\n");
	cfprintf(cout, "      Treat ${mag}PATH${rs} as a path to search (useful if begins with a dash)\n");
	cfprintf(cout, "  ${cyn}-D${rs} ${bld}FLAG${rs}\n");
	cfprintf(cout, "      Turn on a debugging flag (see ${cyn}-D${rs} ${bld}help${rs})\n");
	cfprintf(cout, "  ${cyn}-O${rs}${bld}N${rs}\n");
	cfprintf(cout, "      Enable optimization level ${bld}N${rs} (default: 3)\n");
	cfprintf(cout, "  ${cyn}-S${rs} ${bld}bfs${rs}|${bld}dfs${rs}|${bld}ids${rs}\n");
	cfprintf(cout, "      Use ${bld}b${rs}readth-${bld}f${rs}irst/${bld}d${rs}epth-${bld}f${rs}irst/${bld}i${rs}terative ${bld}d${rs}eepening ${bld}s${rs}earch (default: ${cyn}-S${rs} ${bld}bfs${rs})\n\n");

	cfprintf(cout, "${bld}Operators:${rs}\n\n");

	cfprintf(cout, "  ${red}(${rs} ${blu}expression${rs} ${red})${rs}\n\n");

	cfprintf(cout, "  ${red}!${rs} ${blu}expression${rs}\n");
	cfprintf(cout, "  ${red}-not${rs} ${blu}expression${rs}\n\n");

	cfprintf(cout, "  ${blu}expression${rs} ${blu}expression${rs}\n");
	cfprintf(cout, "  ${blu}expression${rs} ${red}-a${rs} ${blu}expression${rs}\n");
	cfprintf(cout, "  ${blu}expression${rs} ${red}-and${rs} ${blu}expression${rs}\n\n");

	cfprintf(cout, "  ${blu}expression${rs} ${red}-o${rs} ${blu}expression${rs}\n");
	cfprintf(cout, "  ${blu}expression${rs} ${red}-or${rs} ${blu}expression${rs}\n\n");

	cfprintf(cout, "  ${blu}expression${rs} ${red},${rs} ${blu}expression${rs}\n\n");

	cfprintf(cout, "${bld}Options:${rs}\n\n");

	cfprintf(cout, "  ${blu}-color${rs}\n");
	cfprintf(cout, "  ${blu}-nocolor${rs}\n");
	cfprintf(cout, "      Turn colors on or off (default: ${blu}-color${rs} if outputting to a terminal,\n");
	cfprintf(cout, "      ${blu}-nocolor${rs} otherwise)\n");
	cfprintf(cout, "  ${blu}-daystart${rs}\n");
	cfprintf(cout, "      Measure times relative to the start of today\n");
	cfprintf(cout, "  ${blu}-depth${rs}\n");
	cfprintf(cout, "      Search in post-order (descendents first)\n");
	cfprintf(cout, "  ${blu}-follow${rs}\n");
	cfprintf(cout, "      Follow all symbolic links (same as ${cyn}-L${rs})\n");
	cfprintf(cout, "  ${blu}-ignore_readdir_race${rs}\n");
	cfprintf(cout, "  ${blu}-noignore_readdir_race${rs}\n");
	cfprintf(cout, "      Whether to report an error if ${ex}bfs${rs} detects that the file tree is modified\n");
	cfprintf(cout, "      during the search (default: ${blu}-noignore_readdir_race${rs})\n");
	cfprintf(cout, "  ${blu}-maxdepth${rs} ${bld}N${rs}\n");
	cfprintf(cout, "  ${blu}-mindepth${rs} ${bld}N${rs}\n");
	cfprintf(cout, "      Ignore files deeper/shallower than ${bld}N${rs}\n");
	cfprintf(cout, "  ${blu}-mount${rs}\n");
	cfprintf(cout, "  ${blu}-xdev${rs}\n");
	cfprintf(cout, "      Don't descend into other mount points\n");
	cfprintf(cout, "  ${blu}-noleaf${rs}\n");
	cfprintf(cout, "      Ignored; for compatibility with GNU find\n");
	cfprintf(cout, "  ${blu}-regextype${rs} ${bld}TYPE${rs}\n");
	cfprintf(cout, "      Use ${bld}TYPE${rs}-flavored regexes (default: ${bld}posix-basic${rs}; see ${blu}-regextype${rs}"
	                " ${bld}help${rs})\n");
	cfprintf(cout, "  ${blu}-unique${rs}\n");
	cfprintf(cout, "      Skip any files that have already been seen\n");
	cfprintf(cout, "  ${blu}-warn${rs}\n");
	cfprintf(cout, "  ${blu}-nowarn${rs}\n");
	cfprintf(cout, "      Turn on or off warnings about the command line\n\n");

	cfprintf(cout, "${bld}Tests:${rs}\n\n");

#if BFS_CAN_CHECK_ACL
	cfprintf(cout, "  ${blu}-acl${rs}\n");
	cfprintf(cout, "      Find files with a non-trivial Access Control List\n");
#endif
	cfprintf(cout, "  ${blu}-${rs}[${blu}aBcm${rs}]${blu}min${rs} ${bld}[-+]N${rs}\n");
	cfprintf(cout, "      Find files ${blu}a${rs}ccessed/${blu}B${rs}irthed/${blu}c${rs}hanged/${blu}m${rs}odified ${bld}N${rs} minutes ago\n");
	cfprintf(cout, "  ${blu}-${rs}[${blu}aBcm${rs}]${blu}newer${rs} ${bld}FILE${rs}\n");
	cfprintf(cout, "      Find files ${blu}a${rs}ccessed/${blu}B${rs}irthed/${blu}c${rs}hanged/${blu}m${rs}odified more recently than ${bld}FILE${rs} was\n"
	               "      modified\n");
	cfprintf(cout, "  ${blu}-${rs}[${blu}aBcm${rs}]${blu}time${rs} ${bld}[-+]N${rs}\n");
	cfprintf(cout, "      Find files ${blu}a${rs}ccessed/${blu}B${rs}irthed/${blu}c${rs}hanged/${blu}m${rs}odified ${bld}N${rs} days ago\n");
#if BFS_CAN_CHECK_CAPABILITIES
	cfprintf(cout, "  ${blu}-capable${rs}\n");
	cfprintf(cout, "      Find files with POSIX.1e capabilities set\n");
#endif
	cfprintf(cout, "  ${blu}-depth${rs} ${bld}[-+]N${rs}\n");
	cfprintf(cout, "      Find files with depth ${bld}N${rs}\n");
	cfprintf(cout, "  ${blu}-empty${rs}\n");
	cfprintf(cout, "      Find empty files/directories\n");
	cfprintf(cout, "  ${blu}-executable${rs}\n");
	cfprintf(cout, "  ${blu}-readable${rs}\n");
	cfprintf(cout, "  ${blu}-writable${rs}\n");
	cfprintf(cout, "      Find files the current user can execute/read/write\n");
	cfprintf(cout, "  ${blu}-false${rs}\n");
	cfprintf(cout, "  ${blu}-true${rs}\n");
	cfprintf(cout, "      Always false/true\n");
	cfprintf(cout, "  ${blu}-fstype${rs} ${bld}TYPE${rs}\n");
	cfprintf(cout, "      Find files on file systems with the given ${bld}TYPE${rs}\n");
	cfprintf(cout, "  ${blu}-gid${rs} ${bld}[-+]N${rs}\n");
	cfprintf(cout, "  ${blu}-uid${rs} ${bld}[-+]N${rs}\n");
	cfprintf(cout, "      Find files owned by group/user ID ${bld}N${rs}\n");
	cfprintf(cout, "  ${blu}-group${rs} ${bld}NAME${rs}\n");
	cfprintf(cout, "  ${blu}-user${rs}  ${bld}NAME${rs}\n");
	cfprintf(cout, "      Find files owned by the group/user ${bld}NAME${rs}\n");
	cfprintf(cout, "  ${blu}-hidden${rs}\n");
	cfprintf(cout, "  ${blu}-nohidden${rs}\n");
	cfprintf(cout, "      Find hidden files, or filter them out\n");
#ifdef FNM_CASEFOLD
	cfprintf(cout, "  ${blu}-ilname${rs} ${bld}GLOB${rs}\n");
	cfprintf(cout, "  ${blu}-iname${rs}  ${bld}GLOB${rs}\n");
	cfprintf(cout, "  ${blu}-ipath${rs}  ${bld}GLOB${rs}\n");
	cfprintf(cout, "  ${blu}-iregex${rs} ${bld}REGEX${rs}\n");
	cfprintf(cout, "  ${blu}-iwholename${rs} ${bld}GLOB${rs}\n");
	cfprintf(cout, "      Case-insensitive versions of ${blu}-lname${rs}/${blu}-name${rs}/${blu}-path${rs}"
	               "/${blu}-regex${rs}/${blu}-wholename${rs}\n");
#endif
	cfprintf(cout, "  ${blu}-inum${rs} ${bld}[-+]N${rs}\n");
	cfprintf(cout, "      Find files with inode number ${bld}N${rs}\n");
	cfprintf(cout, "  ${blu}-links${rs} ${bld}[-+]N${rs}\n");
	cfprintf(cout, "      Find files with ${bld}N${rs} hard links\n");
	cfprintf(cout, "  ${blu}-lname${rs} ${bld}GLOB${rs}\n");
	cfprintf(cout, "      Find symbolic links whose target matches the ${bld}GLOB${rs}\n");
	cfprintf(cout, "  ${blu}-name${rs} ${bld}GLOB${rs}\n");
	cfprintf(cout, "      Find files whose name matches the ${bld}GLOB${rs}\n");
	cfprintf(cout, "  ${blu}-newer${rs} ${bld}FILE${rs}\n");
	cfprintf(cout, "      Find files newer than ${bld}FILE${rs}\n");
	cfprintf(cout, "  ${blu}-newer${rs}${bld}XY${rs} ${bld}REFERENCE${rs}\n");
	cfprintf(cout, "      Find files whose ${bld}X${rs} time is newer than the ${bld}Y${rs} time of"
	               " ${bld}REFERENCE${rs}.  ${bld}X${rs} and ${bld}Y${rs}\n");
	cfprintf(cout, "      can be any of [${bld}aBcm${rs}].\n");
	cfprintf(cout, "  ${blu}-nogroup${rs}\n");
	cfprintf(cout, "  ${blu}-nouser${rs}\n");
	cfprintf(cout, "      Find files owned by nonexistent groups/users\n");
	cfprintf(cout, "  ${blu}-path${rs} ${bld}GLOB${rs}\n");
	cfprintf(cout, "  ${blu}-wholename${rs} ${bld}GLOB${rs}\n");
	cfprintf(cout, "      Find files whose entire path matches the ${bld}GLOB${rs}\n");
	cfprintf(cout, "  ${blu}-perm${rs} ${bld}[-]MODE${rs}\n");
	cfprintf(cout, "      Find files with a matching mode\n");
	cfprintf(cout, "  ${blu}-regex${rs} ${bld}REGEX${rs}\n");
	cfprintf(cout, "      Find files whose entire path matches the regular expression ${bld}REGEX${rs}\n");
	cfprintf(cout, "  ${blu}-samefile${rs} ${bld}FILE${rs}\n");
	cfprintf(cout, "      Find hard links to ${bld}FILE${rs}\n");
	cfprintf(cout, "  ${blu}-size${rs} ${bld}[-+]N[cwbkMGTP]${rs}\n");
	cfprintf(cout, "      Find files with the given size, in 1-byte ${bld}c${rs}haracters, 2-byte ${bld}w${rs}ords,\n");
	cfprintf(cout, "      512-byte ${bld}b${rs}locks (default), or ${bld}k${rs}iB/${bld}M${rs}iB/${bld}G${rs}iB/${bld}T${rs}iB/${bld}P${rs}iB\n");
	cfprintf(cout, "  ${blu}-sparse${rs}\n");
	cfprintf(cout, "      Find files that occupy fewer disk blocks than expected\n");
	cfprintf(cout, "  ${blu}-type${rs} ${bld}[bcdlpfswD]${rs}\n");
	cfprintf(cout, "      Find files of the given type\n");
	cfprintf(cout, "  ${blu}-used${rs} ${bld}[-+]N${rs}\n");
	cfprintf(cout, "      Find files last accessed ${bld}N${rs} days after they were changed\n");
#if BFS_CAN_CHECK_XATTRS
	cfprintf(cout, "  ${blu}-xattr${rs}\n");
	cfprintf(cout, "      Find files with extended attributes\n");
#endif
	cfprintf(cout, "  ${blu}-xtype${rs} ${bld}[bcdlpfswD]${rs}\n");
	cfprintf(cout, "      Find files of the given type, following links when ${blu}-type${rs} would not, and\n");
	cfprintf(cout, "      vice versa\n\n");

	cfprintf(cout, "${bld}Actions:${rs}\n\n");

	cfprintf(cout, "  ${blu}-delete${rs}\n");
	cfprintf(cout, "  ${blu}-rm${rs}\n");
	cfprintf(cout, "      Delete any found files (implies ${blu}-depth${rs})\n");
	cfprintf(cout, "  ${blu}-exec${rs} ${bld}command ... {} ;${rs}\n");
	cfprintf(cout, "      Execute a command\n");
	cfprintf(cout, "  ${blu}-exec${rs} ${bld}command ... {} +${rs}\n");
	cfprintf(cout, "      Execute a command with multiple files at once\n");
	cfprintf(cout, "  ${blu}-ok${rs} ${bld}command ... {} ;${rs}\n");
	cfprintf(cout, "      Prompt the user whether to execute a command\n");
	cfprintf(cout, "  ${blu}-execdir${rs} ${bld}command ... {} ;${rs}\n");
	cfprintf(cout, "  ${blu}-execdir${rs} ${bld}command ... {} +${rs}\n");
	cfprintf(cout, "  ${blu}-okdir${rs} ${bld}command ... {} ;${rs}\n");
	cfprintf(cout, "      Like ${blu}-exec${rs}/${blu}-ok${rs}, but run the command in the same directory as the found\n");
	cfprintf(cout, "      file(s)\n");
	cfprintf(cout, "  ${blu}-exit${rs} [${bld}STATUS${rs}]\n");
	cfprintf(cout, "      Exit immediately with the given status (%d if unspecified)\n", EXIT_SUCCESS);
	cfprintf(cout, "  ${blu}-fls${rs} ${bld}FILE${rs}\n");
	cfprintf(cout, "  ${blu}-fprint${rs} ${bld}FILE${rs}\n");
	cfprintf(cout, "  ${blu}-fprint0${rs} ${bld}FILE${rs}\n");
	cfprintf(cout, "  ${blu}-fprintf${rs} ${bld}FORMAT${rs} ${bld}FILE${rs}\n");
	cfprintf(cout, "      Like ${blu}-ls${rs}/${blu}-print${rs}/${blu}-print0${rs}/${blu}-printf${rs}, but write to ${bld}FILE${rs} instead of standard\n"
	               "      output\n");
	cfprintf(cout, "  ${blu}-ls${rs}\n");
	cfprintf(cout, "      List files like ${ex}ls${rs} ${bld}-dils${rs}\n");
	cfprintf(cout, "  ${blu}-nohidden${rs}\n");
	cfprintf(cout, "      Filter out hidden files and directories\n");
	cfprintf(cout, "  ${blu}-print${rs}\n");
	cfprintf(cout, "      Print the path to the found file\n");
	cfprintf(cout, "  ${blu}-print0${rs}\n");
	cfprintf(cout, "      Like ${blu}-print${rs}, but use the null character ('\\0') as a separator rather than\n");
	cfprintf(cout, "      newlines\n");
	cfprintf(cout, "  ${blu}-printf${rs} ${bld}FORMAT${rs}\n");
	cfprintf(cout, "      Print according to a format string (see ${ex}man${rs} ${bld}find${rs}).  The additional format\n");
	cfprintf(cout, "      directives %%w and %%W${bld}k${rs} for printing file birth times are supported.\n");
	cfprintf(cout, "  ${blu}-printx${rs}\n");
	cfprintf(cout, "      Like ${blu}-print${rs}, but escape whitespace and quotation characters, to make the\n");
	cfprintf(cout, "      output safe for ${ex}xargs${rs}.  Consider using ${blu}-print0${rs} and ${ex}xargs${rs} ${bld}-0${rs} instead.\n");
	cfprintf(cout, "  ${blu}-prune${rs}\n");
	cfprintf(cout, "      Don't descend into this directory\n");
	cfprintf(cout, "  ${blu}-quit${rs}\n");
	cfprintf(cout, "      Quit immediately\n");
	cfprintf(cout, "  ${blu}-version${rs}\n");
	cfprintf(cout, "      Print version information\n");
	cfprintf(cout, "  ${blu}-help${rs}\n");
	cfprintf(cout, "      Print this help message\n\n");

	cfprintf(cout, "%s\n", BFS_HOMEPAGE);

	if (pager > 0) {
		cfclose(cout);
		waitpid(pager, NULL, 0);
	}

	state->just_info = true;
	return NULL;
}

/**
 * "Parse" -version.
 */
static struct expr *parse_version(struct parser_state *state, int arg1, int arg2) {
	cfprintf(state->cmdline->cout, "${ex}bfs${rs} ${bld}%s${rs}\n\n", BFS_VERSION);

	printf("%s\n", BFS_HOMEPAGE);

	state->just_info = true;
	return NULL;
}

typedef struct expr *parse_fn(struct parser_state *state, int arg1, int arg2);

/**
 * An entry in the parse table for literals.
 */
struct table_entry {
	char *arg;
	parse_fn *parse;
	int arg1;
	int arg2;
	bool prefix;
};

/**
 * The parse table for literals.
 */
static const struct table_entry parse_table[] = {
	{"--"},
	{"--help", parse_help},
	{"--version", parse_version},
	{"-Bmin", parse_time, BFS_STAT_BTIME, MINUTES},
	{"-Bnewer", parse_newer, BFS_STAT_BTIME},
	{"-Btime", parse_time, BFS_STAT_BTIME, DAYS},
	{"-D", parse_debug},
	{"-E", parse_regex_extended},
	{"-H", parse_follow, BFTW_COMFOLLOW, false},
	{"-L", parse_follow, BFTW_LOGICAL, false},
	{"-O", parse_optlevel, 0, 0, true},
	{"-P", parse_follow, 0, false},
	{"-S", parse_search_strategy},
	{"-X", parse_xargs_safe},
	{"-a"},
	{"-acl", parse_acl},
	{"-amin", parse_time, BFS_STAT_ATIME, MINUTES},
	{"-and"},
	{"-anewer", parse_newer, BFS_STAT_ATIME},
	{"-atime", parse_time, BFS_STAT_ATIME, DAYS},
	{"-capable", parse_capable},
	{"-cmin", parse_time, BFS_STAT_CTIME, MINUTES},
	{"-cnewer", parse_newer, BFS_STAT_CTIME},
	{"-color", parse_color, true},
	{"-ctime", parse_time, BFS_STAT_CTIME, DAYS},
	{"-d", parse_depth},
	{"-daystart", parse_daystart},
	{"-delete", parse_delete},
	{"-depth", parse_depth_n},
	{"-empty", parse_empty},
	{"-exec", parse_exec, 0},
	{"-execdir", parse_exec, BFS_EXEC_CHDIR},
	{"-executable", parse_access, X_OK},
	{"-exit", parse_exit},
	{"-f", parse_f},
	{"-false", parse_const, false},
	{"-fls", parse_fls},
	{"-follow", parse_follow, BFTW_LOGICAL, true},
	{"-fprint", parse_fprint},
	{"-fprint0", parse_fprint0},
	{"-fprintf", parse_fprintf},
	{"-fstype", parse_fstype},
	{"-gid", parse_group},
	{"-group", parse_group},
	{"-help", parse_help},
	{"-hidden", parse_hidden},
	{"-ignore_readdir_race", parse_ignore_races, true},
	{"-ilname", parse_lname, true},
	{"-iname", parse_name, true},
	{"-inum", parse_inum},
	{"-ipath", parse_path, true},
	{"-iregex", parse_regex, REG_ICASE},
	{"-iwholename", parse_path, true},
	{"-links", parse_links},
	{"-lname", parse_lname, false},
	{"-ls", parse_ls},
	{"-maxdepth", parse_depth_limit, false},
	{"-mindepth", parse_depth_limit, true},
	{"-mmin", parse_time, BFS_STAT_MTIME, MINUTES},
	{"-mnewer", parse_newer, BFS_STAT_MTIME},
	{"-mount", parse_mount},
	{"-mtime", parse_time, BFS_STAT_MTIME, DAYS},
	{"-name", parse_name, false},
	{"-newer", parse_newer, BFS_STAT_MTIME},
	{"-newer", parse_newerxy, 0, 0, true},
	{"-nocolor", parse_color, false},
	{"-nogroup", parse_nogroup},
	{"-nohidden", parse_nohidden},
	{"-noignore_readdir_race", parse_ignore_races, false},
	{"-noleaf", parse_noleaf},
	{"-not"},
	{"-nouser", parse_nouser},
	{"-nowarn", parse_warn, false},
	{"-o"},
	{"-ok", parse_exec, BFS_EXEC_CONFIRM},
	{"-okdir", parse_exec, BFS_EXEC_CONFIRM | BFS_EXEC_CHDIR},
	{"-or"},
	{"-path", parse_path, false},
	{"-perm", parse_perm},
	{"-print", parse_print},
	{"-print0", parse_print0},
	{"-printf", parse_printf},
	{"-printx", parse_printx},
	{"-prune", parse_prune},
	{"-quit", parse_quit},
	{"-readable", parse_access, R_OK},
	{"-regex", parse_regex, 0},
	{"-regextype", parse_regextype},
	{"-rm", parse_delete},
	{"-samefile", parse_samefile},
	{"-size", parse_size},
	{"-sparse", parse_sparse},
	{"-true", parse_const, true},
	{"-type", parse_type, false},
	{"-uid", parse_user},
	{"-unique", parse_unique},
	{"-used", parse_used},
	{"-user", parse_user},
	{"-version", parse_version},
	{"-warn", parse_warn, true},
	{"-wholename", parse_path, false},
	{"-writable", parse_access, W_OK},
	{"-x", parse_mount},
	{"-xattr", parse_xattr},
	{"-xdev", parse_mount},
	{"-xtype", parse_type, true},
	{0},
};

/** Look up an argument in the parse table. */
static const struct table_entry *table_lookup(const char *arg) {
	for (const struct table_entry *entry = parse_table; entry->arg; ++entry) {
		bool match;
		if (entry->prefix) {
			match = strncmp(arg, entry->arg, strlen(entry->arg)) == 0;
		} else {
			match = strcmp(arg, entry->arg) == 0;
		}
		if (match) {
			return entry;
		}
	}

	return NULL;
}

/** Search for a fuzzy match in the parse table. */
static const struct table_entry *table_lookup_fuzzy(const char *arg) {
	const struct table_entry *best = NULL;
	int best_dist;

	for (const struct table_entry *entry = parse_table; entry->arg; ++entry) {
		int dist = typo_distance(arg, entry->arg);
		if (!best || dist < best_dist) {
			best = entry;
			best_dist = dist;
		}
	}

	return best;
}

/**
 * LITERAL : OPTION
 *         | TEST
 *         | ACTION
 */
static struct expr *parse_literal(struct parser_state *state) {
	// Paths are already skipped at this point
	const char *arg = state->argv[0];

	if (arg[0] != '-') {
		goto unexpected;
	}

	const struct table_entry *match = table_lookup(arg);
	if (match) {
		if (match->parse) {
			goto matched;
		} else {
			goto unexpected;
		}
	}

	match = table_lookup_fuzzy(arg);

	parse_error(state, "Unknown argument '%s'; did you mean '%s'?", arg, match->arg);

	if (!state->interactive || !match->parse) {
		fprintf(stderr, "\n");
		goto unmatched;
	}

	fprintf(stderr, " ");
	if (ynprompt() <= 0) {
		goto unmatched;
	}

	fprintf(stderr, "\n");
	state->argv[0] = match->arg;

matched:
	return match->parse(state, match->arg1, match->arg2);

unmatched:
	return NULL;

unexpected:
	parse_error(state, "Expected a predicate; found '%s'.\n", arg);
	return NULL;
}

/**
 * FACTOR : "(" EXPR ")"
 *        | "!" FACTOR | "-not" FACTOR
 *        | LITERAL
 */
static struct expr *parse_factor(struct parser_state *state) {
	if (skip_paths(state) != 0) {
		return NULL;
	}

	const char *arg = state->argv[0];
	if (!arg) {
		parse_error(state, "Expression terminated prematurely after '%s'.\n", state->last_arg);
		return NULL;
	}

	if (strcmp(arg, "(") == 0) {
		parser_advance(state, T_OPERATOR, 1);

		struct expr *expr = parse_expr(state);
		if (!expr) {
			return NULL;
		}

		if (skip_paths(state) != 0) {
			free_expr(expr);
			return NULL;
		}

		arg = state->argv[0];
		if (!arg || strcmp(arg, ")") != 0) {
			parse_error(state, "Expected a ')' after '%s'.\n", state->argv[-1]);
			free_expr(expr);
			return NULL;
		}
		parser_advance(state, T_OPERATOR, 1);

		return expr;
	} else if (strcmp(arg, "!") == 0 || strcmp(arg, "-not") == 0) {
		char **argv = parser_advance(state, T_OPERATOR, 1);

		struct expr *factor = parse_factor(state);
		if (!factor) {
			return NULL;
		}

		return new_unary_expr(eval_not, factor, argv);
	} else {
		return parse_literal(state);
	}
}

/**
 * TERM : FACTOR
 *      | TERM FACTOR
 *      | TERM "-a" FACTOR
 *      | TERM "-and" FACTOR
 */
static struct expr *parse_term(struct parser_state *state) {
	struct expr *term = parse_factor(state);

	while (term) {
		if (skip_paths(state) != 0) {
			free_expr(term);
			return NULL;
		}

		const char *arg = state->argv[0];
		if (!arg) {
			break;
		}

		if (strcmp(arg, "-o") == 0 || strcmp(arg, "-or") == 0
		    || strcmp(arg, ",") == 0
		    || strcmp(arg, ")") == 0) {
			break;
		}

		char **argv = &fake_and_arg;
		if (strcmp(arg, "-a") == 0 || strcmp(arg, "-and") == 0) {
			argv = parser_advance(state, T_OPERATOR, 1);
		}

		struct expr *lhs = term;
		struct expr *rhs = parse_factor(state);
		if (!rhs) {
			free_expr(lhs);
			return NULL;
		}

		term = new_binary_expr(eval_and, lhs, rhs, argv);
	}

	return term;
}

/**
 * CLAUSE : TERM
 *        | CLAUSE "-o" TERM
 *        | CLAUSE "-or" TERM
 */
static struct expr *parse_clause(struct parser_state *state) {
	struct expr *clause = parse_term(state);

	while (clause) {
		if (skip_paths(state) != 0) {
			free_expr(clause);
			return NULL;
		}

		const char *arg = state->argv[0];
		if (!arg) {
			break;
		}

		if (strcmp(arg, "-o") != 0 && strcmp(arg, "-or") != 0) {
			break;
		}

		char **argv = parser_advance(state, T_OPERATOR, 1);

		struct expr *lhs = clause;
		struct expr *rhs = parse_term(state);
		if (!rhs) {
			free_expr(lhs);
			return NULL;
		}

		clause = new_binary_expr(eval_or, lhs, rhs, argv);
	}

	return clause;
}

/**
 * EXPR : CLAUSE
 *      | EXPR "," CLAUSE
 */
static struct expr *parse_expr(struct parser_state *state) {
	struct expr *expr = parse_clause(state);

	while (expr) {
		if (skip_paths(state) != 0) {
			free_expr(expr);
			return NULL;
		}

		const char *arg = state->argv[0];
		if (!arg) {
			break;
		}

		if (strcmp(arg, ",") != 0) {
			break;
		}

		char **argv = parser_advance(state, T_OPERATOR, 1);

		struct expr *lhs = expr;
		struct expr *rhs = parse_clause(state);
		if (!rhs) {
			free_expr(lhs);
			return NULL;
		}

		expr = new_binary_expr(eval_comma, lhs, rhs, argv);
	}

	return expr;
}

/**
 * Parse the top-level expression.
 */
static struct expr *parse_whole_expr(struct parser_state *state) {
	if (skip_paths(state) != 0) {
		return NULL;
	}

	struct expr *expr = &expr_true;
	if (state->argv[0]) {
		expr = parse_expr(state);
		if (!expr) {
			return NULL;
		}
	}

	if (state->argv[0]) {
		parse_error(state, "Unexpected argument '%s'.\n", state->argv[0]);
		goto fail;
	}

	if (state->implicit_print) {
		struct expr *print = new_expr(eval_fprint, 1, &fake_print_arg);
		if (!print) {
			goto fail;
		}
		init_print_expr(state, print);

		expr = new_binary_expr(eval_and, expr, print, &fake_and_arg);
		if (!expr) {
			goto fail;
		}
	}

	if (state->warn && state->depth_arg && state->prune_arg) {
		parse_warning(state, "%s does not work in the presence of %s.\n", state->prune_arg, state->depth_arg);

		if (state->interactive) {
			fprintf(stderr, "Do you want to continue? ");
			if (ynprompt() == 0) {
				goto fail;
			}
		}

		fprintf(stderr, "\n");
	}

	return expr;

fail:
	free_expr(expr);
	return NULL;
}

/**
 * Dump the parsed form of the command line, for debugging.
 */
void dump_cmdline(const struct cmdline *cmdline, bool verbose) {
	CFILE *cerr = cmdline->cerr;

	cfprintf(cerr, "${ex}%s${rs} ", cmdline->argv[0]);

	const char *strategy = NULL;
	switch (cmdline->strategy) {
	case BFTW_BFS:
		strategy = "bfs";
		break;
	case BFTW_DFS:
		strategy = "dfs";
		break;
	case BFTW_IDS:
		strategy = "ids";
		break;
	}
	assert(strategy);
	cfprintf(cerr, "${cyn}-S${rs} ${bld}%s${rs} ", strategy);

	if (cmdline->flags & BFTW_LOGICAL) {
		cfprintf(cerr, "${cyn}-L${rs} ");
	} else if (cmdline->flags & BFTW_COMFOLLOW) {
		cfprintf(cerr, "${cyn}-H${rs} ");
	} else {
		cfprintf(cerr, "${cyn}-P${rs} ");
	}

	if (cmdline->optlevel != 3) {
		cfprintf(cerr, "${cyn}-O%d${rs} ", cmdline->optlevel);
	}

	enum debug_flags debug = cmdline->debug;
	if (debug) {
		cfprintf(cerr, "${cyn}-D${rs} ");
		for (int i = 0; debug; ++i) {
			enum debug_flags flag = debug_flags[i].flag;
			const char *name = debug_flags[i].name;
			if ((debug & flag) == flag) {
				cfprintf(cerr, "${bld}%s${rs}", name);
				debug ^= flag;
				if (debug) {
					cfprintf(cerr, ",");
				}
			}
		}
		cfprintf(cerr, " ");
	}

	for (size_t i = 0; i < cmdline->npaths; ++i) {
		const char *path = cmdline->paths[i];
		char c = path[0];
		if (c == '-' || c == '(' || c == ')' || c == '!' || c == ',') {
			cfprintf(cerr, "${cyn}-f${rs} ");
		}
		cfprintf(cerr, "${mag}%s${rs} ", path);
	}

	if (cmdline->cout->colors) {
		cfprintf(cerr, "${blu}-color${rs} ");
	} else {
		cfprintf(cerr, "${blu}-nocolor${rs} ");
	}
	if (cmdline->flags & BFTW_DEPTH) {
		cfprintf(cerr, "${blu}-depth${rs} ");
	}
	if (cmdline->ignore_races) {
		cfprintf(cerr, "${blu}-ignore_readdir_race${rs} ");
	}
	if (cmdline->flags & BFTW_XDEV) {
		cfprintf(cerr, "${blu}-mount${rs} ");
	}
	if (cmdline->mindepth != 0) {
		cfprintf(cerr, "${blu}-mindepth${rs} ${bld}%d${rs} ", cmdline->mindepth);
	}
	if (cmdline->maxdepth != INT_MAX) {
		cfprintf(cerr, "${blu}-maxdepth${rs} ${bld}%d${rs} ", cmdline->maxdepth);
	}
	if (cmdline->unique) {
		cfprintf(cerr, "${blu}-unique${rs} ");
	}

	dump_expr(cerr, cmdline->expr, verbose);

	fputs("\n", stderr);
}

/**
 * Dump the estimated costs.
 */
static void dump_costs(const struct cmdline *cmdline) {
	CFILE *cerr = cmdline->cerr;
	const struct expr *expr = cmdline->expr;
	cfprintf(cerr, "       Cost: ~${ylw}%g${rs}\n", expr->cost);
	cfprintf(cerr, "Probability: ~${ylw}%g%%${rs}\n", 100.0*expr->probability);
}

/**
 * Get the current time.
 */
static int parse_gettime(struct timespec *ts) {
#if _POSIX_TIMERS > 0
	int ret = clock_gettime(CLOCK_REALTIME, ts);
	if (ret != 0) {
		perror("clock_gettime()");
	}
	return ret;
#else
	struct timeval tv;
	int ret = gettimeofday(&tv, NULL);
	if (ret == 0) {
		ts->tv_sec = tv.tv_sec;
		ts->tv_nsec = tv.tv_usec * 1000L;
	} else {
		perror("gettimeofday()");
	}
	return ret;
#endif
}

/**
 * Parse the command line.
 */
struct cmdline *parse_cmdline(int argc, char *argv[]) {
	struct cmdline *cmdline = malloc(sizeof(struct cmdline));
	if (!cmdline) {
		perror("malloc()");
		goto fail;
	}

	cmdline->argv = NULL;
	cmdline->paths = NULL;
	cmdline->npaths = 0;
	cmdline->colors = NULL;
	cmdline->cout = NULL;
	cmdline->cerr = NULL;
	cmdline->mtab = NULL;
	cmdline->mtab_error = 0;
	cmdline->mindepth = 0;
	cmdline->maxdepth = INT_MAX;
	cmdline->flags = BFTW_RECOVER;
	cmdline->strategy = BFTW_BFS;
	cmdline->optlevel = 3;
	cmdline->debug = 0;
	cmdline->xargs_safe = false;
	cmdline->ignore_races = false;
	cmdline->unique = false;
	cmdline->expr = &expr_true;
	cmdline->nopen_files = 0;

	trie_init(&cmdline->open_files);

	static char* default_argv[] = {"bfs", NULL};
	if (argc < 1) {
		argc = 1;
		argv = default_argv;
	}

	cmdline->argv = malloc((argc + 1)*sizeof(*cmdline->argv));
	if (!cmdline->argv) {
		perror("malloc()");
		goto fail;
	}
	for (int i = 0; i <= argc; ++i) {
		cmdline->argv[i] = argv[i];
	}

	enum use_color use_color = COLOR_AUTO;
	if (getenv("NO_COLOR")) {
		// https://no-color.org/
		use_color = COLOR_NEVER;
	}

	cmdline->colors = parse_colors(getenv("LS_COLORS"));
	cmdline->cout = cfdup(stdout, use_color ? cmdline->colors : NULL);
	cmdline->cerr = cfdup(stderr, use_color ? cmdline->colors : NULL);
	if (!cmdline->cout || !cmdline->cerr) {
		perror("cfdup()");
		goto fail;
	}

	cmdline->mtab = parse_bfs_mtab();
	if (!cmdline->mtab) {
		cmdline->mtab_error = errno;
	}

	bool stdin_tty = isatty(STDIN_FILENO);
	bool stdout_tty = isatty(STDOUT_FILENO);
	bool stderr_tty = isatty(STDERR_FILENO);

	struct parser_state state = {
		.cmdline = cmdline,
		.argv = cmdline->argv + 1,
		.command = cmdline->argv[0],
		.regex_flags = 0,
		.stdout_tty = stdout_tty,
		.interactive = stdin_tty && stderr_tty,
		.use_color = use_color,
		.implicit_print = true,
		.warn = stdin_tty,
		.non_option_seen = false,
		.just_info = false,
		.last_arg = NULL,
		.depth_arg = NULL,
		.prune_arg = NULL,
	};

	if (strcmp(xbasename(state.command), "find") == 0) {
		// Operate depth-first when invoked as "find"
		cmdline->strategy = BFTW_DFS;
	}

	if (parse_gettime(&state.now) != 0) {
		goto fail;
	}

	cmdline->expr = parse_whole_expr(&state);
	if (!cmdline->expr) {
		if (state.just_info) {
			goto done;
		} else {
			goto fail;
		}
	}

	if (optimize_cmdline(cmdline) != 0) {
		goto fail;
	}

	if (cmdline->npaths == 0) {
		if (parse_root(&state, ".") != 0) {
			goto fail;
		}
	}

	if ((cmdline->flags & BFTW_LOGICAL) && !cmdline->unique) {
		// We need bftw() to detect cycles unless -unique does it for us
		cmdline->flags |= BFTW_DETECT_CYCLES;
	}

	if (cmdline->debug & DEBUG_TREE) {
		dump_cmdline(cmdline, false);
	}
	if (cmdline->debug & DEBUG_COST) {
		dump_costs(cmdline);
	}

done:
	return cmdline;

fail:
	free_cmdline(cmdline);
	return NULL;
}
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2017-2019 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

#include <assert.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef int bfs_printf_fn(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf);

struct bfs_printf {
	/** The printing function to invoke. */
	bfs_printf_fn *fn;
	/** String data associated with this directive. */
	char *str;
	/** The stat field to print. */
	enum bfs_stat_field stat_field;
	/** Character data associated with this directive. */
	char c;
	/** The current mount table. */
	const struct bfs_mtab *mtab;
	/** The next printf directive in the chain. */
	struct bfs_printf *next;
};

/** Print some text as-is. */
static int bfs_printf_literal(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	size_t len = dstrlen(directive->str);
	if (fwrite(directive->str, 1, len, file) == len) {
		return 0;
	} else {
		return -1;
	}
}

/** \c: flush */
static int bfs_printf_flush(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	return fflush(file);
}

/**
 * Print a value to a temporary buffer before formatting it.
 */
#define BFS_PRINTF_BUF(buf, format, ...)				\
	char buf[256];							\
	int ret = snprintf(buf, sizeof(buf), format, __VA_ARGS__);	\
	assert(ret >= 0 && ret < sizeof(buf));				\
	(void)ret

/** %a, %c, %t: ctime() */
static int bfs_printf_ctime(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	// Not using ctime() itself because GNU find adds nanoseconds
	static const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	static const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	const struct bfs_stat *statbuf = bftw_stat(ftwbuf, ftwbuf->stat_flags);
	if (!statbuf) {
		return -1;
	}

	const struct timespec *ts = bfs_stat_time(statbuf, directive->stat_field);
	if (!ts) {
		return -1;
	}

	struct tm tm;
	if (xlocaltime(&ts->tv_sec, &tm) != 0) {
		return -1;
	}

	BFS_PRINTF_BUF(buf, "%s %s %2d %.2d:%.2d:%.2d.%09ld0 %4d",
	               days[tm.tm_wday],
	               months[tm.tm_mon],
	               tm.tm_mday,
	               tm.tm_hour,
	               tm.tm_min,
	               tm.tm_sec,
	               (long)ts->tv_nsec,
	               1900 + tm.tm_year);

	return fprintf(file, directive->str, buf);
}

/** %A, %B/%W, %C, %T: strftime() */
static int bfs_printf_strftime(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	const struct bfs_stat *statbuf = bftw_stat(ftwbuf, ftwbuf->stat_flags);
	if (!statbuf) {
		return -1;
	}

	const struct timespec *ts = bfs_stat_time(statbuf, directive->stat_field);
	if (!ts) {
		return -1;
	}

	struct tm tm;
	if (xlocaltime(&ts->tv_sec, &tm) != 0) {
		return -1;
	}

	int ret;
	char buf[256];
	char format[] = "% ";
	switch (directive->c) {
	// Non-POSIX strftime() features
	case '@':
		ret = snprintf(buf, sizeof(buf), "%lld.%09ld0", (long long)ts->tv_sec, (long)ts->tv_nsec);
		break;
	case '+':
		ret = snprintf(buf, sizeof(buf), "%4d-%.2d-%.2d+%.2d:%.2d:%.2d.%09ld0",
		               1900 + tm.tm_year,
		               tm.tm_mon + 1,
		               tm.tm_mday,
		               tm.tm_hour,
		               tm.tm_min,
		               tm.tm_sec,
		               (long)ts->tv_nsec);
		break;
	case 'k':
		ret = snprintf(buf, sizeof(buf), "%2d", tm.tm_hour);
		break;
	case 'l':
		ret = snprintf(buf, sizeof(buf), "%2d", (tm.tm_hour + 11)%12 + 1);
		break;
	case 's':
		ret = snprintf(buf, sizeof(buf), "%lld", (long long)ts->tv_sec);
		break;
	case 'S':
		ret = snprintf(buf, sizeof(buf), "%.2d.%09ld0", tm.tm_sec, (long)ts->tv_nsec);
		break;
	case 'T':
		ret = snprintf(buf, sizeof(buf), "%.2d:%.2d:%.2d.%09ld0",
			       tm.tm_hour,
			       tm.tm_min,
			       tm.tm_sec,
			       (long)ts->tv_nsec);
		break;

	// POSIX strftime() features
	default:
		format[1] = directive->c;
		ret = strftime(buf, sizeof(buf), format, &tm);
		break;
	}

	assert(ret >= 0 && ret < sizeof(buf));
	(void)ret;

	return fprintf(file, directive->str, buf);
}

/** %b: blocks */
static int bfs_printf_b(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	const struct bfs_stat *statbuf = bftw_stat(ftwbuf, ftwbuf->stat_flags);
	if (!statbuf) {
		return -1;
	}

	uintmax_t blocks = ((uintmax_t)statbuf->blocks*BFS_STAT_BLKSIZE + 511)/512;
	BFS_PRINTF_BUF(buf, "%ju", blocks);
	return fprintf(file, directive->str, buf);
}

/** %d: depth */
static int bfs_printf_d(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	return fprintf(file, directive->str, (intmax_t)ftwbuf->depth);
}

/** %D: device */
static int bfs_printf_D(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	const struct bfs_stat *statbuf = bftw_stat(ftwbuf, ftwbuf->stat_flags);
	if (!statbuf) {
		return -1;
	}

	BFS_PRINTF_BUF(buf, "%ju", (uintmax_t)statbuf->dev);
	return fprintf(file, directive->str, buf);
}

/** %f: file name */
static int bfs_printf_f(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	return fprintf(file, directive->str, ftwbuf->path + ftwbuf->nameoff);
}

/** %F: file system type */
static int bfs_printf_F(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	const struct bfs_stat *statbuf = bftw_stat(ftwbuf, ftwbuf->stat_flags);
	if (!statbuf) {
		return -1;
	}

	const char *type = bfs_fstype(directive->mtab, statbuf);
	return fprintf(file, directive->str, type);
}

/** %G: gid */
static int bfs_printf_G(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	const struct bfs_stat *statbuf = bftw_stat(ftwbuf, ftwbuf->stat_flags);
	if (!statbuf) {
		return -1;
	}

	BFS_PRINTF_BUF(buf, "%ju", (uintmax_t)statbuf->gid);
	return fprintf(file, directive->str, buf);
}

/** %g: group name */
static int bfs_printf_g(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	const struct bfs_stat *statbuf = bftw_stat(ftwbuf, ftwbuf->stat_flags);
	if (!statbuf) {
		return -1;
	}

	struct group *grp = getgrgid(statbuf->gid);
	if (!grp) {
		return bfs_printf_G(file, directive, ftwbuf);
	}

	return fprintf(file, directive->str, grp->gr_name);
}

/** %h: leading directories */
static int bfs_printf_h(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	char *copy = NULL;
	const char *buf;

	if (ftwbuf->nameoff > 0) {
		size_t len = ftwbuf->nameoff;
		if (len > 1) {
			--len;
		}

		buf = copy = strndup(ftwbuf->path, len);
	} else if (ftwbuf->path[0] == '/') {
		buf = "/";
	} else {
		buf = ".";
	}

	if (!buf) {
		return -1;
	}

	int ret = fprintf(file, directive->str, buf);
	free(copy);
	return ret;
}

/** %H: current root */
static int bfs_printf_H(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	return fprintf(file, directive->str, ftwbuf->root);
}

/** %i: inode */
static int bfs_printf_i(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	const struct bfs_stat *statbuf = bftw_stat(ftwbuf, ftwbuf->stat_flags);
	if (!statbuf) {
		return -1;
	}

	BFS_PRINTF_BUF(buf, "%ju", (uintmax_t)statbuf->ino);
	return fprintf(file, directive->str, buf);
}

/** %k: 1K blocks */
static int bfs_printf_k(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	const struct bfs_stat *statbuf = bftw_stat(ftwbuf, ftwbuf->stat_flags);
	if (!statbuf) {
		return -1;
	}

	uintmax_t blocks = ((uintmax_t)statbuf->blocks*BFS_STAT_BLKSIZE + 1023)/1024;
	BFS_PRINTF_BUF(buf, "%ju", blocks);
	return fprintf(file, directive->str, buf);
}

/** %l: link target */
static int bfs_printf_l(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	if (ftwbuf->typeflag != BFTW_LNK) {
		return 0;
	}

	char *target = xreadlinkat(ftwbuf->at_fd, ftwbuf->at_path, 0);
	if (!target) {
		return -1;
	}

	int ret = fprintf(file, directive->str, target);
	free(target);
	return ret;
}

/** %m: mode */
static int bfs_printf_m(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	const struct bfs_stat *statbuf = bftw_stat(ftwbuf, ftwbuf->stat_flags);
	if (!statbuf) {
		return -1;
	}

	return fprintf(file, directive->str, (unsigned int)(statbuf->mode & 07777));
}

/** %M: symbolic mode */
static int bfs_printf_M(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	const struct bfs_stat *statbuf = bftw_stat(ftwbuf, ftwbuf->stat_flags);
	if (!statbuf) {
		return -1;
	}

	char buf[11];
	format_mode(statbuf->mode, buf);
	return fprintf(file, directive->str, buf);
}

/** %n: link count */
static int bfs_printf_n(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	const struct bfs_stat *statbuf = bftw_stat(ftwbuf, ftwbuf->stat_flags);
	if (!statbuf) {
		return -1;
	}

	BFS_PRINTF_BUF(buf, "%ju", (uintmax_t)statbuf->nlink);
	return fprintf(file, directive->str, buf);
}

/** %p: full path */
static int bfs_printf_p(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	return fprintf(file, directive->str, ftwbuf->path);
}

/** %P: path after root */
static int bfs_printf_P(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	const char *path = ftwbuf->path + strlen(ftwbuf->root);
	if (path[0] == '/') {
		++path;
	}
	return fprintf(file, directive->str, path);
}

/** %s: size */
static int bfs_printf_s(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	const struct bfs_stat *statbuf = bftw_stat(ftwbuf, ftwbuf->stat_flags);
	if (!statbuf) {
		return -1;
	}

	BFS_PRINTF_BUF(buf, "%ju", (uintmax_t)statbuf->size);
	return fprintf(file, directive->str, buf);
}

/** %S: sparseness */
static int bfs_printf_S(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	const struct bfs_stat *statbuf = bftw_stat(ftwbuf, ftwbuf->stat_flags);
	if (!statbuf) {
		return -1;
	}

	double sparsity;
	if (statbuf->size == 0 && statbuf->blocks == 0) {
		sparsity = 1.0;
	} else {
		sparsity = (double)BFS_STAT_BLKSIZE*statbuf->blocks/statbuf->size;
	}
	return fprintf(file, directive->str, sparsity);
}

/** %U: uid */
static int bfs_printf_U(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	const struct bfs_stat *statbuf = bftw_stat(ftwbuf, ftwbuf->stat_flags);
	if (!statbuf) {
		return -1;
	}

	BFS_PRINTF_BUF(buf, "%ju", (uintmax_t)statbuf->uid);
	return fprintf(file, directive->str, buf);
}

/** %u: user name */
static int bfs_printf_u(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	const struct bfs_stat *statbuf = bftw_stat(ftwbuf, ftwbuf->stat_flags);
	if (!statbuf) {
		return -1;
	}

	struct passwd *pwd = getpwuid(statbuf->uid);
	if (!pwd) {
		return bfs_printf_U(file, directive, ftwbuf);
	}

	return fprintf(file, directive->str, pwd->pw_name);
}

static const char *bfs_printf_type(enum bftw_typeflag typeflag) {
	switch (typeflag) {
	case BFTW_BLK:
		return "b";
	case BFTW_CHR:
		return "c";
	case BFTW_DIR:
		return "d";
	case BFTW_DOOR:
		return "D";
	case BFTW_FIFO:
		return "p";
	case BFTW_LNK:
		return "l";
	case BFTW_REG:
		return "f";
	case BFTW_SOCK:
		return "s";
	default:
		return "U";
	}
}

/** %y: type */
static int bfs_printf_y(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	const char *type = bfs_printf_type(ftwbuf->typeflag);
	return fprintf(file, directive->str, type);
}

/** %Y: target type */
static int bfs_printf_Y(FILE *file, const struct bfs_printf *directive, const struct BFTW *ftwbuf) {
	int error = 0;

	if (ftwbuf->typeflag != BFTW_LNK) {
		return bfs_printf_y(file, directive, ftwbuf);
	}

	const char *type = "U";

	const struct bfs_stat *statbuf = bftw_stat(ftwbuf, BFS_STAT_FOLLOW);
	if (statbuf) {
		type = bfs_printf_type(bftw_mode_typeflag(statbuf->mode));
	} else {
		switch (errno) {
		case ELOOP:
			type = "L";
			break;
		case ENOENT:
		case ENOTDIR:
			type = "N";
			break;
		default:
			type = "?";
			error = errno;
			break;
		}
	}

	int ret = fprintf(file, directive->str, type);
	if (error != 0) {
		ret = -1;
		errno = error;
	}
	return ret;
}

/**
 * Free a printf directive.
 */
static void free_directive(struct bfs_printf *directive) {
	if (directive) {
		dstrfree(directive->str);
		free(directive);
	}
}

/**
 * Create a new printf directive.
 */
static struct bfs_printf *new_directive(bfs_printf_fn *fn) {
	struct bfs_printf *directive = malloc(sizeof(*directive));
	if (!directive) {
		perror("malloc()");
		goto error;
	}

	directive->fn = fn;
	directive->str = dstralloc(2);
	if (!directive->str) {
		perror("dstralloc()");
		goto error;
	}
	directive->stat_field = 0;
	directive->c = 0;
	directive->mtab = NULL;
	directive->next = NULL;
	return directive;

error:
	free_directive(directive);
	return NULL;
}

/**
 * Append a printf directive to the chain.
 */
static struct bfs_printf **append_directive(struct bfs_printf **tail, struct bfs_printf *directive) {
	assert(directive);
	*tail = directive;
	return &directive->next;
}

/**
 * Append a literal string to the chain.
 */
static struct bfs_printf **append_literal(struct bfs_printf **tail, struct bfs_printf **literal) {
	struct bfs_printf *directive = *literal;
	if (directive && dstrlen(directive->str) > 0) {
		*literal = NULL;
		return append_directive(tail, directive);
	} else {
		return tail;
	}
}

struct bfs_printf *parse_bfs_printf(const char *format, struct cmdline *cmdline) {
	struct bfs_printf *head = NULL;
	struct bfs_printf **tail = &head;

	struct bfs_printf *literal = new_directive(bfs_printf_literal);
	if (!literal) {
		goto error;
	}

	for (const char *i = format; *i; ++i) {
		char c = *i;

		if (c == '\\') {
			c = *++i;

			if (c >= '0' && c < '8') {
				c = 0;
				for (int j = 0; j < 3 && *i >= '0' && *i < '8'; ++i, ++j) {
					c *= 8;
					c += *i - '0';
				}
				--i;
				goto one_char;
			}

			switch (c) {
			case 'a':  c = '\a'; break;
			case 'b':  c = '\b'; break;
			case 'f':  c = '\f'; break;
			case 'n':  c = '\n'; break;
			case 'r':  c = '\r'; break;
			case 't':  c = '\t'; break;
			case 'v':  c = '\v'; break;
			case '\\': c = '\\'; break;

			case 'c':
				tail = append_literal(tail, &literal);
				struct bfs_printf *directive = new_directive(bfs_printf_flush);
				if (!directive) {
					goto error;
				}
				tail = append_directive(tail, directive);
				goto done;

			case '\0':
				bfs_error(cmdline, "'%s': Incomplete escape sequence '\\'.\n", format);
				goto error;

			default:
				bfs_error(cmdline, "'%s': Unrecognized escape sequence '\\%c'.\n", format, c);
				goto error;
			}
		} else if (c == '%') {
			if (i[1] == '%') {
				c = *++i;
				goto one_char;
			}

			struct bfs_printf *directive = new_directive(NULL);
			if (!directive) {
				goto directive_error;
			}
			if (dstrapp(&directive->str, c) != 0) {
				perror("dstrapp()");
				goto directive_error;
			}

			const char *specifier = "s";

			// Parse any flags
			bool must_be_numeric = false;
			while (true) {
				c = *++i;

				switch (c) {
				case '#':
				case '0':
				case '+':
					must_be_numeric = true;
					// Fallthrough
				case ' ':
				case '-':
					if (strchr(directive->str, c)) {
						bfs_error(cmdline, "'%s': Duplicate flag '%c'.\n", format, c);
						goto directive_error;
					}
					if (dstrapp(&directive->str, c) != 0) {
						perror("dstrapp()");
						goto directive_error;
					}
					continue;
				}

				break;
			}

			// Parse the field width
			while (c >= '0' && c <= '9') {
				if (dstrapp(&directive->str, c) != 0) {
					perror("dstrapp()");
					goto directive_error;
				}
				c = *++i;
			}

			// Parse the precision
			if (c == '.') {
				do {
					if (dstrapp(&directive->str, c) != 0) {
						perror("dstrapp()");
						goto directive_error;
					}
					c = *++i;
				} while (c >= '0' && c <= '9');
			}

			switch (c) {
			case 'a':
				directive->fn = bfs_printf_ctime;
				directive->stat_field = BFS_STAT_ATIME;
				break;
			case 'b':
				directive->fn = bfs_printf_b;
				break;
			case 'c':
				directive->fn = bfs_printf_ctime;
				directive->stat_field = BFS_STAT_CTIME;
				break;
			case 'd':
				directive->fn = bfs_printf_d;
				specifier = "jd";
				break;
			case 'D':
				directive->fn = bfs_printf_D;
				break;
			case 'f':
				directive->fn = bfs_printf_f;
				break;
			case 'F':
				if (!cmdline->mtab) {
					bfs_error(cmdline, "Couldn't parse the mount table: %s.\n", strerror(cmdline->mtab_error));
					goto directive_error;
				}
				directive->fn = bfs_printf_F;
				directive->mtab = cmdline->mtab;
				break;
			case 'g':
				directive->fn = bfs_printf_g;
				break;
			case 'G':
				directive->fn = bfs_printf_G;
				break;
			case 'h':
				directive->fn = bfs_printf_h;
				break;
			case 'H':
				directive->fn = bfs_printf_H;
				break;
			case 'i':
				directive->fn = bfs_printf_i;
				break;
			case 'k':
				directive->fn = bfs_printf_k;
				break;
			case 'l':
				directive->fn = bfs_printf_l;
				break;
			case 'm':
				directive->fn = bfs_printf_m;
				specifier = "o";
				break;
			case 'M':
				directive->fn = bfs_printf_M;
				break;
			case 'n':
				directive->fn = bfs_printf_n;
				break;
			case 'p':
				directive->fn = bfs_printf_p;
				break;
			case 'P':
				directive->fn = bfs_printf_P;
				break;
			case 's':
				directive->fn = bfs_printf_s;
				break;
			case 'S':
				directive->fn = bfs_printf_S;
				specifier = "g";
				break;
			case 't':
				directive->fn = bfs_printf_ctime;
				directive->stat_field = BFS_STAT_MTIME;
				break;
			case 'u':
				directive->fn = bfs_printf_u;
				break;
			case 'U':
				directive->fn = bfs_printf_U;
				break;
			case 'w':
				directive->fn = bfs_printf_ctime;
				directive->stat_field = BFS_STAT_BTIME;
				break;
			case 'y':
				directive->fn = bfs_printf_y;
				break;
			case 'Y':
				directive->fn = bfs_printf_Y;
				break;

			case 'A':
				directive->stat_field = BFS_STAT_ATIME;
				goto directive_strftime;
			case 'B':
			case 'W':
				directive->stat_field = BFS_STAT_BTIME;
				goto directive_strftime;
			case 'C':
				directive->stat_field = BFS_STAT_CTIME;
				goto directive_strftime;
			case 'T':
				directive->stat_field = BFS_STAT_MTIME;
				goto directive_strftime;

			directive_strftime:
				directive->fn = bfs_printf_strftime;
				c = *++i;
				if (!c) {
					bfs_error(cmdline, "'%s': Incomplete time specifier '%s%c'.\n", format, directive->str, i[-1]);
					goto directive_error;
				} else if (strchr("%+@aAbBcCdDeFgGhHIjklmMnprRsStTuUVwWxXyYzZ", c)) {
					directive->c = c;
				} else {
					bfs_error(cmdline, "'%s': Unrecognized time specifier '%%%c%c'.\n", format, i[-1], c);
					goto directive_error;
				}
				break;

			case '\0':
				bfs_error(cmdline, "'%s': Incomplete format specifier '%s'.\n", format, directive->str);
				goto directive_error;

			default:
				bfs_error(cmdline, "'%s': Unrecognized format specifier '%%%c'.\n", format, c);
				goto directive_error;
			}

			if (must_be_numeric && strcmp(specifier, "s") == 0) {
				bfs_error(cmdline, "'%s': Invalid flags '%s' for string format '%%%c'.\n", format, directive->str + 1, c);
				goto directive_error;
			}

			if (dstrcat(&directive->str, specifier) != 0) {
				perror("dstrcat()");
				goto directive_error;
			}

			tail = append_literal(tail, &literal);
			tail = append_directive(tail, directive);

			if (!literal) {
				literal = new_directive(bfs_printf_literal);
				if (!literal) {
					goto error;
				}
			}

			continue;

		directive_error:
			free_directive(directive);
			goto error;
		}

	one_char:
		if (dstrapp(&literal->str, c) != 0) {
			perror("dstrapp()");
			goto error;
		}
	}

done:
	tail = append_literal(tail, &literal);
	if (head) {
		free_directive(literal);
		return head;
	} else {
		return literal;
	}

error:
	free_directive(literal);
	free_bfs_printf(head);
	return NULL;
}

int bfs_printf(FILE *file, const struct bfs_printf *command, const struct BFTW *ftwbuf) {
	int ret = 0, error = 0;

	for (const struct bfs_printf *directive = command; directive; directive = directive->next) {
		if (directive->fn(file, directive, ftwbuf) < 0) {
			ret = -1;
			error = errno;
		}
	}

	errno = error;
	return ret;
}

void free_bfs_printf(struct bfs_printf *command) {
	while (command) {
		struct bfs_printf *next = command->next;
		free_directive(command);
		command = next;
	}
}
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2018-2019 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * Types of spawn actions.
 */
enum bfs_spawn_op {
	BFS_SPAWN_CLOSE,
	BFS_SPAWN_DUP2,
	BFS_SPAWN_FCHDIR,
};

/**
 * A spawn action.
 */
struct bfs_spawn_action {
	struct bfs_spawn_action *next;

	enum bfs_spawn_op op;
	int in_fd;
	int out_fd;
};

int bfs_spawn_init(struct bfs_spawn *ctx) {
	ctx->flags = 0;
	ctx->actions = NULL;
	ctx->tail = &ctx->actions;
	return 0;
}

int bfs_spawn_destroy(struct bfs_spawn *ctx) {
	struct bfs_spawn_action *action = ctx->actions;
	while (action) {
		struct bfs_spawn_action *next = action->next;
		free(action);
		action = next;
	}
	return 0;
}

int bfs_spawn_setflags(struct bfs_spawn *ctx, enum bfs_spawn_flags flags) {
	ctx->flags = flags;
	return 0;
}

/** Add a spawn action to the chain. */
static struct bfs_spawn_action *bfs_spawn_add(struct bfs_spawn *ctx, enum bfs_spawn_op op) {
	struct bfs_spawn_action *action = malloc(sizeof(*action));
	if (action) {
		action->next = NULL;
		action->op = op;
		action->in_fd = -1;
		action->out_fd = -1;

		*ctx->tail = action;
		ctx->tail = &action->next;
	}
	return action;
}

int bfs_spawn_addclose(struct bfs_spawn *ctx, int fd) {
	if (fd < 0) {
		errno = EBADF;
		return -1;
	}

	struct bfs_spawn_action *action = bfs_spawn_add(ctx, BFS_SPAWN_CLOSE);
	if (action) {
		action->out_fd = fd;
		return 0;
	} else {
		return -1;
	}
}

int bfs_spawn_adddup2(struct bfs_spawn *ctx, int oldfd, int newfd) {
	if (oldfd < 0 || newfd < 0) {
		errno = EBADF;
		return -1;
	}

	struct bfs_spawn_action *action = bfs_spawn_add(ctx, BFS_SPAWN_DUP2);
	if (action) {
		action->in_fd = oldfd;
		action->out_fd = newfd;
		return 0;
	} else {
		return -1;
	}
}

int bfs_spawn_addfchdir(struct bfs_spawn *ctx, int fd) {
	if (fd < 0) {
		errno = EBADF;
		return -1;
	}

	struct bfs_spawn_action *action = bfs_spawn_add(ctx, BFS_SPAWN_FCHDIR);
	if (action) {
		action->in_fd = fd;
		return 0;
	} else {
		return -1;
	}
}

/** Facade for execvpe() which is non-standard. */
static int bfs_execvpe(const char *exe, char **argv, char **envp) {
#if __GLIBC__ || __linux__ || __NetBSD__ || __OpenBSD__
	return execvpe(exe, argv, envp);
#else
	extern char **environ;
	environ = envp;
	return execvp(exe, argv);
#endif
}

/** Actually exec() the new process. */
static void bfs_spawn_exec(const char *exe, const struct bfs_spawn *ctx, char **argv, char **envp, int pipefd[2]) {
	int error;
	enum bfs_spawn_flags flags = ctx ? ctx->flags : 0;
	const struct bfs_spawn_action *actions = ctx ? ctx->actions : NULL;

	close(pipefd[0]);

	for (const struct bfs_spawn_action *action = actions; action; action = action->next) {
		// Move the error-reporting pipe out of the way if necessary
		if (action->in_fd == pipefd[1] || action->out_fd == pipefd[1]) {
			int fd = dup(pipefd[1]);
			if (fd < 0) {
				goto fail;
			}
			close(pipefd[1]);
			pipefd[1] = fd;
		}

		switch (action->op) {
		case BFS_SPAWN_CLOSE:
			if (close(action->out_fd) != 0) {
				goto fail;
			}
			break;
		case BFS_SPAWN_DUP2:
			if (dup2(action->in_fd, action->out_fd) < 0) {
				goto fail;
			}
			break;
		case BFS_SPAWN_FCHDIR:
			if (fchdir(action->in_fd) != 0) {
				goto fail;
			}
			break;
		}
	}

	if (flags & BFS_SPAWN_USEPATH) {
		bfs_execvpe(exe, argv, envp);
	} else {
		execve(exe, argv, envp);
	}

fail:
	error = errno;
	while (write(pipefd[1], &error, sizeof(error)) < sizeof(error));
	close(pipefd[1]);
	_Exit(127);
}

pid_t bfs_spawn(const char *exe, const struct bfs_spawn *ctx, char **argv, char **envp) {
	// Use a pipe to report errors from the child
	int pipefd[2];
	if (pipe_cloexec(pipefd) != 0) {
		return -1;
	}

	int error;
	pid_t pid = fork();

	if (pid < 0) {
		error = errno;
		close(pipefd[1]);
		close(pipefd[0]);
		errno = error;
		return -1;
	} else if (pid == 0) {
		// Child
		bfs_spawn_exec(exe, ctx, argv, envp, pipefd);
	}

	// Parent
	close(pipefd[1]);

	ssize_t nbytes = read(pipefd[0], &error, sizeof(error));
	close(pipefd[0]);
	if (nbytes == sizeof(error)) {
		int wstatus;
		waitpid(pid, &wstatus, 0);
		errno = error;
		return -1;
	}

	return pid;
}
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2018-2019 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef STATX_BASIC_STATS
#	define HAVE_STATX true
#elif __linux__
#	include <linux/stat.h>
#	include <sys/syscall.h>
#endif

#if HAVE_STATX || defined(__NR_statx)
#	define HAVE_BFS_STATX true
#endif

#if __APPLE__
#	define st_atim st_atimespec
#	define st_ctim st_ctimespec
#	define st_mtim st_mtimespec
#	define st_birthtim st_birthtimespec
#endif

const char *bfs_stat_field_name(enum bfs_stat_field field) {
	switch (field) {
	case BFS_STAT_DEV:
		return "device number";
	case BFS_STAT_INO:
		return "inode nunmber";
	case BFS_STAT_TYPE:
		return "type";
	case BFS_STAT_MODE:
		return "mode";
	case BFS_STAT_NLINK:
		return "link count";
	case BFS_STAT_GID:
		return "group ID";
	case BFS_STAT_UID:
		return "user ID";
	case BFS_STAT_SIZE:
		return "size";
	case BFS_STAT_BLOCKS:
		return "block count";
	case BFS_STAT_RDEV:
		return "underlying device";
	case BFS_STAT_ATIME:
		return "access time";
	case BFS_STAT_BTIME:
		return "birth time";
	case BFS_STAT_CTIME:
		return "change time";
	case BFS_STAT_MTIME:
		return "modification time";
	}

	assert(false);
	return "???";
}

/**
 * Check if we should retry a failed stat() due to a potentially broken link.
 */
static bool bfs_stat_retry(int ret, enum bfs_stat_flag flags) {
	return ret != 0
		&& (flags & (BFS_STAT_NOFOLLOW | BFS_STAT_TRYFOLLOW)) == BFS_STAT_TRYFOLLOW
		&& is_nonexistence_error(errno);
}

/**
 * Convert a struct stat to a struct bfs_stat.
 */
static void bfs_stat_convert(const struct stat *statbuf, struct bfs_stat *buf) {
	buf->mask = 0;

	buf->dev = statbuf->st_dev;
	buf->mask |= BFS_STAT_DEV;

	buf->ino = statbuf->st_ino;
	buf->mask |= BFS_STAT_INO;

	buf->mode = statbuf->st_mode;
	buf->mask |= BFS_STAT_TYPE | BFS_STAT_MODE;

	buf->nlink = statbuf->st_nlink;
	buf->mask |= BFS_STAT_NLINK;

	buf->gid = statbuf->st_gid;
	buf->mask |= BFS_STAT_GID;

	buf->uid = statbuf->st_uid;
	buf->mask |= BFS_STAT_UID;

	buf->size = statbuf->st_size;
	buf->mask |= BFS_STAT_SIZE;

	buf->blocks = statbuf->st_blocks;
	buf->mask |= BFS_STAT_BLOCKS;

	buf->rdev = statbuf->st_rdev;
	buf->mask |= BFS_STAT_RDEV;

	buf->atime = statbuf->st_atim;
	buf->mask |= BFS_STAT_ATIME;

	buf->ctime = statbuf->st_ctim;
	buf->mask |= BFS_STAT_CTIME;

	buf->mtime = statbuf->st_mtim;
	buf->mask |= BFS_STAT_MTIME;

#if __APPLE__ || __FreeBSD__ || __NetBSD__
	buf->btime = statbuf->st_birthtim;
	buf->mask |= BFS_STAT_BTIME;
#endif
}

/**
 * bfs_stat() implementation backed by stat().
 */
static int bfs_stat_impl(int at_fd, const char *at_path, int at_flags, enum bfs_stat_flag flags, struct bfs_stat *buf) {
	struct stat statbuf;
	int ret = fstatat(at_fd, at_path, &statbuf, at_flags);

	if (bfs_stat_retry(ret, flags)) {
		at_flags |= AT_SYMLINK_NOFOLLOW;
		ret = fstatat(at_fd, at_path, &statbuf, at_flags);
	}

	if (ret == 0) {
		bfs_stat_convert(&statbuf, buf);
	}

	return ret;
}

#if HAVE_BFS_STATX

/**
 * Wrapper for the statx() system call, which had no glibc wrapper prior to 2.28.
 */
static int bfs_statx(int at_fd, const char *at_path, int at_flags, unsigned int mask, struct statx *buf) {
	// -fsanitize=memory doesn't know about statx(), so tell it the memory
	// got initialized
#if BFS_HAS_FEATURE(memory_sanitizer, false)
	memset(buf, 0, sizeof(*buf));
#endif

#if HAVE_STATX
	return statx(at_fd, at_path, at_flags, mask, buf);
#else
	return syscall(__NR_statx, at_fd, at_path, at_flags, mask, buf);
#endif
}

/**
 * bfs_stat() implementation backed by statx().
 */
static int bfs_statx_impl(int at_fd, const char *at_path, int at_flags, enum bfs_stat_flag flags, struct bfs_stat *buf) {
	unsigned int mask = STATX_BASIC_STATS | STATX_BTIME;
	struct statx xbuf;
	int ret = bfs_statx(at_fd, at_path, at_flags, mask, &xbuf);

	if (bfs_stat_retry(ret, flags)) {
		at_flags |= AT_SYMLINK_NOFOLLOW;
		ret = bfs_statx(at_fd, at_path, at_flags, mask, &xbuf);
	}

	if (ret != 0) {
		return ret;
	}

	// Callers shouldn't have to check anything except the times
	const unsigned int guaranteed = STATX_BASIC_STATS ^ (STATX_ATIME | STATX_CTIME | STATX_MTIME);
	if ((xbuf.stx_mask & guaranteed) != guaranteed) {
		errno = ENOTSUP;
		return -1;
	}

	buf->mask = 0;

	buf->dev = bfs_makedev(xbuf.stx_dev_major, xbuf.stx_dev_minor);
	buf->mask |= BFS_STAT_DEV;

	if (xbuf.stx_mask & STATX_INO) {
		buf->ino = xbuf.stx_ino;
		buf->mask |= BFS_STAT_INO;
	}

	buf->mode = xbuf.stx_mode;
	if (xbuf.stx_mask & STATX_TYPE) {
		buf->mask |= BFS_STAT_TYPE;
	}
	if (xbuf.stx_mask & STATX_MODE) {
		buf->mask |= BFS_STAT_MODE;
	}

	if (xbuf.stx_mask & STATX_NLINK) {
		buf->nlink = xbuf.stx_nlink;
		buf->mask |= BFS_STAT_NLINK;
	}

	if (xbuf.stx_mask & STATX_GID) {
		buf->gid = xbuf.stx_gid;
		buf->mask |= BFS_STAT_GID;
	}

	if (xbuf.stx_mask & STATX_UID) {
		buf->uid = xbuf.stx_uid;
		buf->mask |= BFS_STAT_UID;
	}

	if (xbuf.stx_mask & STATX_SIZE) {
		buf->size = xbuf.stx_size;
		buf->mask |= BFS_STAT_SIZE;
	}

	if (xbuf.stx_mask & STATX_BLOCKS) {
		buf->blocks = xbuf.stx_blocks;
		buf->mask |= BFS_STAT_BLOCKS;
	}

	buf->rdev = bfs_makedev(xbuf.stx_rdev_major, xbuf.stx_rdev_minor);
	buf->mask |= BFS_STAT_RDEV;

	if (xbuf.stx_mask & STATX_ATIME) {
		buf->atime.tv_sec = xbuf.stx_atime.tv_sec;
		buf->atime.tv_nsec = xbuf.stx_atime.tv_nsec;
		buf->mask |= BFS_STAT_ATIME;
	}

	if (xbuf.stx_mask & STATX_BTIME) {
		buf->btime.tv_sec = xbuf.stx_btime.tv_sec;
		buf->btime.tv_nsec = xbuf.stx_btime.tv_nsec;
		buf->mask |= BFS_STAT_BTIME;
	}

	if (xbuf.stx_mask & STATX_CTIME) {
		buf->ctime.tv_sec = xbuf.stx_ctime.tv_sec;
		buf->ctime.tv_nsec = xbuf.stx_ctime.tv_nsec;
		buf->mask |= BFS_STAT_CTIME;
	}

	if (xbuf.stx_mask & STATX_MTIME) {
		buf->mtime.tv_sec = xbuf.stx_mtime.tv_sec;
		buf->mtime.tv_nsec = xbuf.stx_mtime.tv_nsec;
		buf->mask |= BFS_STAT_MTIME;
	}

	return ret;
}

#endif // HAVE_BFS_STATX

/**
 * Allows calling stat with custom at_flags.
 */
static int bfs_stat_explicit(int at_fd, const char *at_path, int at_flags, enum bfs_stat_flag flags, struct bfs_stat *buf) {
#if HAVE_BFS_STATX
	static bool has_statx = true;

	if (has_statx) {
		int ret = bfs_statx_impl(at_fd, at_path, at_flags, flags, buf);
		if (ret != 0 && errno == ENOSYS) {
			has_statx = false;
		} else {
			return ret;
		}
	}
#endif

	return bfs_stat_impl(at_fd, at_path, at_flags, flags, buf);
}

int bfs_stat(int at_fd, const char *at_path, enum bfs_stat_flag flags, struct bfs_stat *buf) {
	int at_flags = 0;
	if (flags & BFS_STAT_NOFOLLOW) {
		at_flags |= AT_SYMLINK_NOFOLLOW;
	}

	if (at_path) {
		return bfs_stat_explicit(at_fd, at_path, at_flags, flags, buf);
	}

#ifdef AT_EMPTY_PATH
	static bool has_at_ep = true;
	if (has_at_ep) {
		at_flags |= AT_EMPTY_PATH;
		int ret = bfs_stat_explicit(at_fd, "", at_flags, flags, buf);
		if (ret != 0 && errno == EINVAL) {
			has_at_ep = false;
		} else {
			return ret;
		}
	}
#endif

	struct stat statbuf;
	if (fstat(at_fd, &statbuf) == 0) {
		bfs_stat_convert(&statbuf, buf);
		return 0;
	} else {
		return -1;
	}
}

const struct timespec *bfs_stat_time(const struct bfs_stat *buf, enum bfs_stat_field field) {
	if (!(buf->mask & field)) {
		errno = ENOTSUP;
		return NULL;
	}

	switch (field) {
	case BFS_STAT_ATIME:
		return &buf->atime;
	case BFS_STAT_BTIME:
		return &buf->btime;
	case BFS_STAT_CTIME:
		return &buf->ctime;
	case BFS_STAT_MTIME:
		return &buf->mtime;
	default:
		assert(false);
		errno = EINVAL;
		return NULL;
	}
}

void bfs_stat_id(const struct bfs_stat *buf, bfs_file_id *id) {
	memcpy(*id, &buf->dev, sizeof(buf->dev));
	memcpy(*id + sizeof(buf->dev), &buf->ino, sizeof(buf->ino));
}
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2019 Tavian Barnes <tavianator@tavianator.com>             *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

/**
 * This is an implementation of a "qp trie," as documented at
 * https://dotat.at/prog/qp/README.html
 *
 * An uncompressed trie over the dataset {AAAA, AADD, ABCD, DDAA, DDDD} would
 * look like
 *
 *       A    A    A    A
 *     *--->*--->*--->*--->$
 *     |    |    | D    D
 *     |    |    +--->*--->$
 *     |    | B    C    D
 *     |    +--->*--->*--->$
 *     | D    D    A    A
 *     +--->*--->*--->*--->$
 *               | D    D
 *               +--->*--->$
 *
 * A compressed (PATRICIA) trie collapses internal nodes that have only a single
 * child, like this:
 *
 *       A    A    AA
 *     *--->*--->*---->$
 *     |    |    | DD
 *     |    |    +---->$
 *     |    | BCD
 *     |    +----->$
 *     | DD    AA
 *     +---->*---->$
 *           | DD
 *           +---->$
 *
 * The nodes can be compressed further by dropping the actual compressed
 * sequences from the nodes, storing it only in the leaves.  This is the
 * technique applied in QP tries, and the crit-bit trees that inspired them
 * (https://cr.yp.to/critbit.html).  Only the index to test, and the values to
 * branch on, need to be stored in each node.
 *
 *       A    A    A
 *     0--->1--->2--->AAAA
 *     |    |    | D
 *     |    |    +--->AADD
 *     |    | B
 *     |    +--->ABCD
 *     | D    A
 *     +--->2--->DDAA
 *          | D
 *          +--->DDDD
 *
 * Nodes are represented very compactly.  Rather than a dense array of children,
 * a sparse array of only the non-NULL children directly follows the node in
 * memory.  A bitmap is used to track which children exist; the index of a child
 * i is found by counting the number of bits below bit i that are set.  A tag
 * bit is used to tell pointers to internal nodes apart from pointers to leaves.
 *
 * This implementation tests a whole nibble (half byte/hex digit) at every
 * branch, so the bitmap takes up 16 bits.  The remainder of a machine word is
 * used to hold the offset, which severely constrains its range on 32-bit
 * platforms.  As a workaround, we store relative instead of absolute offsets,
 * and insert intermediate singleton "jump" nodes when necessary.
 */

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if CHAR_BIT != 8
#	error "This trie implementation assumes 8-bit bytes."
#endif

/** Number of bits for the sparse array bitmap, aka the range of a nibble. */
#define BITMAP_BITS 16
/** The number of remaining bits in a word, to hold the offset. */
#define OFFSET_BITS (sizeof(size_t)*CHAR_BIT - BITMAP_BITS)
/** The highest representable offset (only 64k on a 32-bit architecture). */
#define OFFSET_MAX (((size_t)1 << OFFSET_BITS) - 1)

/**
 * An internal node of the trie.
 */
struct trie_node {
	/**
	 * A bitmap that hold which indices exist in the sparse children array.
	 * Bit i will be set if a child exists at logical index i, and its index
	 * into the array will be popcount(bitmap & ((1 << i) - 1)).
	 */
	size_t bitmap : BITMAP_BITS;

	/**
	 * The offset into the key in nibbles.  This is relative to the parent
	 * node, to support offsets larger than OFFSET_MAX.
	 */
	size_t offset : OFFSET_BITS;

	/**
	 * Flexible array of children.  Each pointer uses the lowest bit as a
	 * tag to distinguish internal nodes from leaves.  This is safe as long
	 * as all dynamic allocations are aligned to more than a single byte.
	 */
	uintptr_t children[];
};

/** Check if an encoded pointer is to a leaf. */
static bool trie_is_leaf(uintptr_t ptr) {
	return ptr & 1;
}

/** Decode a pointer to a leaf. */
static struct trie_leaf *trie_decode_leaf(uintptr_t ptr) {
	assert(trie_is_leaf(ptr));
	return (struct trie_leaf *)(ptr ^ 1);
}

/** Encode a pointer to a leaf. */
static uintptr_t trie_encode_leaf(const struct trie_leaf *leaf) {
	uintptr_t ptr = (uintptr_t)leaf ^ 1;
	assert(trie_is_leaf(ptr));
	return ptr;
}

/** Decode a pointer to an internal node. */
static struct trie_node *trie_decode_node(uintptr_t ptr) {
	assert(!trie_is_leaf(ptr));
	return (struct trie_node *)ptr;
}

/** Encode a pointer to an internal node. */
static uintptr_t trie_encode_node(const struct trie_node *node) {
	uintptr_t ptr = (uintptr_t)node;
	assert(!trie_is_leaf(ptr));
	return ptr;
}

void trie_init(struct trie *trie) {
	trie->root = 0;
}

/** Compute the popcount (Hamming weight) of a bitmap. */
static unsigned int trie_popcount(unsigned int n) {
#if __POPCNT__
	// Use the x86 instruction if we have it.  Otherwise, GCC generates a
	// library call, so use the below implementation instead.
	return __builtin_popcount(n);
#else
	// See https://en.wikipedia.org/wiki/Hamming_weight#Efficient_implementation
	n -= (n >> 1) & 0x5555;
	n = (n & 0x3333) + ((n >> 2) & 0x3333);
	n = (n + (n >> 4)) & 0x0F0F;
	n = (n + (n >> 8)) & 0xFF;
	return n;
#endif
}

/** Extract the nibble at a certain offset from a byte sequence. */
static unsigned char trie_key_nibble(const void *key, size_t offset) {
	const unsigned char *bytes = key;
	size_t byte = offset >> 1;

	// A branchless version of
	// if (offset & 1) {
	//         return bytes[byte] >> 4;
	// } else {
	//         return bytes[byte] & 0xF;
	// }
	unsigned int shift = (offset & 1) << 2;
	return (bytes[byte] >> shift) & 0xF;
}

/**
 * Finds a leaf in the trie that matches the key at every branch.  If the key
 * exists in the trie, the representative will match the searched key.  But
 * since only branch points are tested, it can be different from the key.  In
 * that case, the first mismatch between the key and the representative will be
 * the depth at which to make a new branch to insert the key.
 */
static struct trie_leaf *trie_representative(const struct trie *trie, const void *key, size_t length) {
	uintptr_t ptr = trie->root;
	if (!ptr) {
		return NULL;
	}

	size_t offset = 0;
	while (!trie_is_leaf(ptr)) {
		struct trie_node *node = trie_decode_node(ptr);
		offset += node->offset;

		unsigned int index = 0;
		if ((offset >> 1) < length) {
			unsigned char nibble = trie_key_nibble(key, offset);
			unsigned int bit = 1U << nibble;
			if (node->bitmap & bit) {
				index = trie_popcount(node->bitmap & (bit - 1));
			}
		}
		ptr = node->children[index];
	}

	return trie_decode_leaf(ptr);
}

struct trie_leaf *trie_first_leaf(const struct trie *trie) {
	return trie_representative(trie, NULL, 0);
}

struct trie_leaf *trie_find_str(const struct trie *trie, const char *key) {
	return trie_find_mem(trie, key, strlen(key) + 1);
}

struct trie_leaf *trie_find_mem(const struct trie *trie, const void *key, size_t length) {
	struct trie_leaf *rep = trie_representative(trie, key, length);
	if (rep && rep->length == length && memcmp(rep->key, key, length) == 0) {
		return rep;
	} else {
		return NULL;
	}
}

struct trie_leaf *trie_find_postfix(const struct trie *trie, const char *key) {
	size_t length = strlen(key);
	struct trie_leaf *rep = trie_representative(trie, key, length + 1);
	if (rep && rep->length >= length && memcmp(rep->key, key, length) == 0) {
		return rep;
	} else {
		return NULL;
	}
}

/**
 * Find a leaf that may end at the current node.
 */
static struct trie_leaf *trie_terminal_leaf(const struct trie_node *node) {
	// Finding a terminating NUL byte may take two nibbles
	for (int i = 0; i < 2; ++i) {
		if (!(node->bitmap & 1)) {
			break;
		}

		uintptr_t ptr = node->children[0];
		if (trie_is_leaf(ptr)) {
			return trie_decode_leaf(ptr);
		} else {
			node = trie_decode_node(ptr);
		}
	}

	return NULL;
}

/** Check if a leaf is a prefix of a search key. */
static bool trie_check_prefix(struct trie_leaf *leaf, size_t skip, const char *key, size_t length) {
	if (leaf && leaf->length <= length) {
		return memcmp(key + skip, leaf->key + skip, leaf->length - skip - 1) == 0;
	} else {
		return false;
	}
}

struct trie_leaf *trie_find_prefix(const struct trie *trie, const char *key) {
	uintptr_t ptr = trie->root;
	if (!ptr) {
		return NULL;
	}

	struct trie_leaf *best = NULL;
	size_t skip = 0;
	size_t length = strlen(key) + 1;

	size_t offset = 0;
	while (!trie_is_leaf(ptr)) {
		struct trie_node *node = trie_decode_node(ptr);
		offset += node->offset;
		if ((offset >> 1) >= length) {
			return best;
		}

		struct trie_leaf *leaf = trie_terminal_leaf(node);
		if (trie_check_prefix(leaf, skip, key, length)) {
			best = leaf;
			skip = offset >> 1;
		}

		unsigned char nibble = trie_key_nibble(key, offset);
		unsigned int bit = 1U << nibble;
		if (node->bitmap & bit) {
			unsigned int index = trie_popcount(node->bitmap & (bit - 1));
			ptr = node->children[index];
		} else {
			return best;
		}
	}

	struct trie_leaf *leaf = trie_decode_leaf(ptr);
	if (trie_check_prefix(leaf, skip, key, length)) {
		best = leaf;
	}

	return best;
}

/** Create a new leaf, holding a copy of the given key. */
static struct trie_leaf *new_trie_leaf(const void *key, size_t length) {
	struct trie_leaf *leaf = malloc(sizeof(*leaf) + length);
	if (leaf) {
		leaf->value = NULL;
		leaf->length = length;
		memcpy(leaf->key, key, length);
	}
	return leaf;
}

/** Compute the size of a trie node with a certain number of children. */
static size_t trie_node_size(unsigned int size) {
	// Empty nodes aren't supported
	assert(size > 0);
	// Node size must be a power of two
	assert((size & (size - 1)) == 0);

	return sizeof(struct trie_node) + size*sizeof(uintptr_t);
}

/** Find the offset of the first nibble that differs between two keys. */
static size_t trie_key_mismatch(const void *key1, const void *key2, size_t length) {
	const unsigned char *bytes1 = key1;
	const unsigned char *bytes2 = key2;
	size_t i = 0;
	size_t offset = 0;
	const size_t chunk = sizeof(size_t);

	for (; i + chunk <= length; i += chunk) {
		if (memcmp(bytes1 + i, bytes2 + i, chunk) != 0) {
			break;
		}
	}

	for (; i < length; ++i) {
		unsigned char b1 = bytes1[i], b2 = bytes2[i];
		if (b1 != b2) {
			offset = (b1 & 0xF) == (b2 & 0xF);
			break;
		}
	}

	offset |= i << 1;
	return offset;
}

/**
 * Insert a key into a node.  The node must not have a child in that position
 * already.  Effectively takes a subtrie like this:
 *
 *     ptr
 *      |
 *      v X
 *      *--->...
 *      | Z
 *      +--->...
 *
 * and transforms it to:
 *
 *     ptr
 *      |
 *      v X
 *      *--->...
 *      | Y
 *      +--->key
 *      | Z
 *      +--->...
 */
static struct trie_leaf *trie_node_insert(uintptr_t *ptr, const void *key, size_t length, size_t offset) {
	struct trie_node *node = trie_decode_node(*ptr);
	unsigned int size = trie_popcount(node->bitmap);

	// Double the capacity every power of two
	if ((size & (size - 1)) == 0) {
		node = realloc(node, trie_node_size(2*size));
		if (!node) {
			return NULL;
		}
		*ptr = trie_encode_node(node);
	}

	struct trie_leaf *leaf = new_trie_leaf(key, length);
	if (!leaf) {
		return NULL;
	}

	unsigned char nibble = trie_key_nibble(key, offset);
	unsigned int bit = 1U << nibble;

	// The child must not already be present
	assert(!(node->bitmap & bit));
	node->bitmap |= bit;

	unsigned int index = trie_popcount(node->bitmap & (bit - 1));
	uintptr_t *child = node->children + index;
	if (index < size) {
		memmove(child + 1, child, (size - index)*sizeof(*child));
	}
	*child = trie_encode_leaf(leaf);
	return leaf;
}

/**
 * When the current offset exceeds OFFSET_MAX, insert "jump" nodes that bridge
 * the gap.  This function takes a subtrie like this:
 *
 *     ptr
 *      |
 *      v
 *      *--->rep
 *
 * and changes it to:
 *
 *     ptr  ret
 *      |    |
 *      v    v
 *      *--->*--->rep
 *
 * so that a new key can be inserted like:
 *
 *     ptr  ret
 *      |    |
 *      v    v X
 *      *--->*--->rep
 *           | Y
 *           +--->key
 */
static uintptr_t *trie_jump(uintptr_t *ptr, const char *key, size_t *offset) {
	// We only ever need to jump to leaf nodes, since internal nodes are
	// guaranteed to be within OFFSET_MAX anyway
	assert(trie_is_leaf(*ptr));

	struct trie_node *node = malloc(trie_node_size(1));
	if (!node) {
		return NULL;
	}

	*offset += OFFSET_MAX;
	node->offset = OFFSET_MAX;

	unsigned char nibble = trie_key_nibble(key, *offset);
	node->bitmap = 1 << nibble;

	node->children[0] = *ptr;
	*ptr = trie_encode_node(node);
	return node->children;
}

/**
 * Split a node in the trie.  Changes a subtrie like this:
 *
 *     ptr
 *      |
 *      v
 *      *...>--->rep
 *
 * into this:
 *
 *     ptr
 *      |
 *      v X
 *      *--->*...>--->rep
 *      | Y
 *      +--->key
 */
static struct trie_leaf *trie_split(uintptr_t *ptr, const void *key, size_t length, struct trie_leaf *rep, size_t offset, size_t mismatch) {
	unsigned char key_nibble = trie_key_nibble(key, mismatch);
	unsigned char rep_nibble = trie_key_nibble(rep->key, mismatch);
	assert(key_nibble != rep_nibble);

	struct trie_node *node = malloc(trie_node_size(2));
	if (!node) {
		return NULL;
	}

	struct trie_leaf *leaf = new_trie_leaf(key, length);
	if (!leaf) {
		free(node);
		return NULL;
	}

	node->bitmap = (1 << key_nibble) | (1 << rep_nibble);

	size_t delta = mismatch - offset;
	if (!trie_is_leaf(*ptr)) {
		struct trie_node *child = trie_decode_node(*ptr);
		child->offset -= delta;
	}
	node->offset = delta;

	unsigned int key_index = key_nibble > rep_nibble;
	node->children[key_index] = trie_encode_leaf(leaf);
	node->children[key_index ^ 1] = *ptr;
	*ptr = trie_encode_node(node);
	return leaf;
}

struct trie_leaf *trie_insert_str(struct trie *trie, const char *key) {
	return trie_insert_mem(trie, key, strlen(key) + 1);
}

struct trie_leaf *trie_insert_mem(struct trie *trie, const void *key, size_t length) {
	struct trie_leaf *rep = trie_representative(trie, key, length);
	if (!rep) {
		struct trie_leaf *leaf = new_trie_leaf(key, length);
		if (leaf) {
			trie->root = trie_encode_leaf(leaf);
		}
		return leaf;
	}

	size_t limit = length < rep->length ? length : rep->length;
	size_t mismatch = trie_key_mismatch(key, rep->key, limit);
	if ((mismatch >> 1) >= length) {
		return rep;
	}

	size_t offset = 0;
	uintptr_t *ptr = &trie->root;
	while (!trie_is_leaf(*ptr)) {
		struct trie_node *node = trie_decode_node(*ptr);
		if (offset + node->offset > mismatch) {
			break;
		}
		offset += node->offset;

		unsigned char nibble = trie_key_nibble(key, offset);
		unsigned int bit = 1U << nibble;
		if (node->bitmap & bit) {
			assert(offset < mismatch);
			unsigned int index = trie_popcount(node->bitmap & (bit - 1));
			ptr = node->children + index;
		} else {
			assert(offset == mismatch);
			return trie_node_insert(ptr, key, length, offset);
		}
	}

	while (mismatch - offset > OFFSET_MAX) {
		ptr = trie_jump(ptr, key, &offset);
		if (!ptr) {
			return NULL;
		}
	}

	return trie_split(ptr, key, length, rep, offset, mismatch);
}

/** Free a chain of singleton nodes. */
static void trie_free_singletons(uintptr_t ptr) {
	while (!trie_is_leaf(ptr)) {
		struct trie_node *node = trie_decode_node(ptr);

		// Make sure the bitmap is a power of two, i.e. it has just one child
		assert((node->bitmap & (node->bitmap - 1)) == 0);

		ptr = node->children[0];
		free(node);
	}

	free(trie_decode_leaf(ptr));
}

/**
 * Try to collapse a two-child node like:
 *
 *     parent child
 *       |      |
 *       v      v
 *       *----->*----->*----->leaf
 *       |
 *       +----->other
 *
 * into
 *
 *     parent
 *       |
 *       v
 *     other
 */
static int trie_collapse_node(uintptr_t *parent, struct trie_node *parent_node, unsigned int child_index) {
	uintptr_t other = parent_node->children[child_index ^ 1];
	if (!trie_is_leaf(other)) {
		struct trie_node *other_node = trie_decode_node(other);
		if (other_node->offset + parent_node->offset <= OFFSET_MAX) {
			other_node->offset += parent_node->offset;
		} else {
			return -1;
		}
	}

	*parent = other;
	free(parent_node);
	return 0;
}

void trie_remove(struct trie *trie, struct trie_leaf *leaf) {
	uintptr_t *child = &trie->root;
	uintptr_t *parent = NULL;
	unsigned int child_bit = 0, child_index = 0;
	size_t offset = 0;
	while (!trie_is_leaf(*child)) {
		struct trie_node *node = trie_decode_node(*child);
		offset += node->offset;
		assert((offset >> 1) < leaf->length);

		unsigned char nibble = trie_key_nibble(leaf->key, offset);
		unsigned int bit = 1U << nibble;
		unsigned int bitmap = node->bitmap;
		assert(bitmap & bit);
		unsigned int index = trie_popcount(bitmap & (bit - 1));

		// Advance the parent pointer, unless this node had only one child
		if (bitmap & (bitmap - 1)) {
			parent = child;
			child_bit = bit;
			child_index = index;
		}

		child = node->children + index;
	}

	assert(trie_decode_leaf(*child) == leaf);

	if (!parent) {
		trie_free_singletons(trie->root);
		trie->root = 0;
		return;
	}

	struct trie_node *node = trie_decode_node(*parent);
	child = node->children + child_index;
	trie_free_singletons(*child);

	node->bitmap ^= child_bit;
	unsigned int parent_size = trie_popcount(node->bitmap);
	assert(parent_size > 0);
	if (parent_size == 1 && trie_collapse_node(parent, node, child_index) == 0) {
		return;
	}

	if (child_index < parent_size) {
		memmove(child, child + 1, (parent_size - child_index)*sizeof(*child));
	}

	if ((parent_size & (parent_size - 1)) == 0) {
		node = realloc(node, trie_node_size(parent_size));
		if (node) {
			*parent = trie_encode_node(node);
		}
	}
}

/** Free an encoded pointer to a node. */
static void free_trie_ptr(uintptr_t ptr) {
	if (trie_is_leaf(ptr)) {
		free(trie_decode_leaf(ptr));
	} else {
		struct trie_node *node = trie_decode_node(ptr);
		size_t size = trie_popcount(node->bitmap);
		for (size_t i = 0; i < size; ++i) {
			free_trie_ptr(node->children[i]);
		}
		free(node);
	}
}

void trie_destroy(struct trie *trie) {
	if (trie->root) {
		free_trie_ptr(trie->root);
	}
}
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2016 Tavian Barnes <tavianator@tavianator.com>             *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

#include <limits.h>
#include <stdlib.h>
#include <string.h>

// Assume QWERTY layout for now
static const int key_coords[UCHAR_MAX][3] = {
	['`']  = { 0,  0, 0},
	['~']  = { 0,  0, 1},
	['1']  = { 3,  0, 0},
	['!']  = { 3,  0, 1},
	['2']  = { 6,  0, 0},
	['@']  = { 6,  0, 1},
	['3']  = { 9,  0, 0},
	['#']  = { 9,  0, 1},
	['4']  = {12,  0, 0},
	['$']  = {12,  0, 1},
	['5']  = {15,  0, 0},
	['%']  = {15,  0, 1},
	['6']  = {18,  0, 0},
	['^']  = {18,  0, 1},
	['7']  = {21,  0, 0},
	['&']  = {21,  0, 1},
	['8']  = {24,  0, 0},
	['*']  = {24,  0, 1},
	['9']  = {27,  0, 0},
	['(']  = {27,  0, 1},
	['0']  = {30,  0, 0},
	[')']  = {30,  0, 1},
	['-']  = {33,  0, 0},
	['_']  = {33,  0, 1},
	['=']  = {36,  0, 0},
	['+']  = {36,  0, 1},

	['\t'] = { 1,  3, 0},
	['q']  = { 4,  3, 0},
	['Q']  = { 4,  3, 1},
	['w']  = { 7,  3, 0},
	['W']  = { 7,  3, 1},
	['e']  = {10,  3, 0},
	['E']  = {10,  3, 1},
	['r']  = {13,  3, 0},
	['R']  = {13,  3, 1},
	['t']  = {16,  3, 0},
	['T']  = {16,  3, 1},
	['y']  = {19,  3, 0},
	['Y']  = {19,  3, 1},
	['u']  = {22,  3, 0},
	['U']  = {22,  3, 1},
	['i']  = {25,  3, 0},
	['I']  = {25,  3, 1},
	['o']  = {28,  3, 0},
	['O']  = {28,  3, 1},
	['p']  = {31,  3, 0},
	['P']  = {31,  3, 1},
	['[']  = {34,  3, 0},
	['{']  = {34,  3, 1},
	[']']  = {37,  3, 0},
	['}']  = {37,  3, 1},
	['\\'] = {40,  3, 0},
	['|']  = {40,  3, 1},

	['a']  = { 5,  6, 0},
	['A']  = { 5,  6, 1},
	['s']  = { 8,  6, 0},
	['S']  = { 8,  6, 1},
	['d']  = {11,  6, 0},
	['D']  = {11,  6, 1},
	['f']  = {14,  6, 0},
	['F']  = {14,  6, 1},
	['g']  = {17,  6, 0},
	['G']  = {17,  6, 1},
	['h']  = {20,  6, 0},
	['H']  = {20,  6, 1},
	['j']  = {23,  6, 0},
	['J']  = {23,  6, 1},
	['k']  = {26,  6, 0},
	['K']  = {26,  6, 1},
	['l']  = {29,  6, 0},
	['L']  = {29,  6, 1},
	[';']  = {32,  6, 0},
	[':']  = {32,  6, 1},
	['\''] = {35,  6, 0},
	['"']  = {35,  6, 1},
	['\n'] = {38,  6, 0},

	['z']  = { 6,  9, 0},
	['Z']  = { 6,  9, 1},
	['x']  = { 9,  9, 0},
	['X']  = { 9,  9, 1},
	['c']  = {12,  9, 0},
	['C']  = {12,  9, 1},
	['v']  = {15,  9, 0},
	['V']  = {15,  9, 1},
	['b']  = {18,  9, 0},
	['B']  = {18,  9, 1},
	['n']  = {21,  9, 0},
	['N']  = {21,  9, 1},
	['m']  = {24,  9, 0},
	['M']  = {24,  9, 1},
	[',']  = {27,  9, 0},
	['<']  = {27,  9, 1},
	['.']  = {30,  9, 0},
	['>']  = {30,  9, 1},
	['/']  = {33,  9, 0},
	['?']  = {33,  9, 1},

	[' ']  = {18, 12, 0},
};

static int char_distance(char a, char b) {
	const int *ac = key_coords[(unsigned char)a], *bc = key_coords[(unsigned char)b];
	int ret = 0;
	for (int i = 0; i < 3; ++i) {
		ret += abs(ac[i] - bc[i]);
	}
	return ret;
}

int typo_distance(const char *actual, const char *expected) {
	// This is the Wagner-Fischer algorithm for Levenshtein distance, using
	// Manhattan distance on the keyboard for individual characters.

	const int insert_cost = 12;

	size_t rows = strlen(actual) + 1;
	size_t cols = strlen(expected) + 1;

	int arr0[cols], arr1[cols];
	int *row0 = arr0, *row1 = arr1;

	for (size_t j = 0; j < cols; ++j) {
		row0[j] = insert_cost * j;
	}

	for (size_t i = 1; i < rows; ++i) {
		row1[0] = row0[0] + insert_cost;

		char a = actual[i - 1];
		for (size_t j = 1; j < cols; ++j) {
			char b = expected[j - 1];
			int cost = row0[j - 1] + char_distance(a, b);
			int del_cost = row0[j] + insert_cost;
			if (del_cost < cost) {
				cost = del_cost;
			}
			int ins_cost = row1[j - 1] + insert_cost;
			if (ins_cost < cost) {
				cost = ins_cost;
			}
			row1[j] = cost;
		}

		int *tmp = row0;
		row0 = row1;
		row1 = tmp;
	}

	return row0[cols - 1];
}
/****************************************************************************
 * bfs                                                                      *
 * Copyright (C) 2016-2018 Tavian Barnes <tavianator@tavianator.com>        *
 *                                                                          *
 * Permission to use, copy, modify, and/or distribute this software for any *
 * purpose with or without fee is hereby granted.                           *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES *
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF         *
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  *
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   *
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN    *
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  *
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.           *
 ****************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <langinfo.h>
#include <regex.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#if BFS_HAS_SYS_PARAM
#	include <sys/param.h>
#endif

#if BFS_HAS_SYS_SYSMACROS
#	include <sys/sysmacros.h>
#elif BFS_HAS_SYS_MKDEV
#	include <sys/mkdev.h>
#endif

int xreaddir(DIR *dir, struct dirent **de) {
	while (true) {
		errno = 0;
		*de = readdir(dir);

		if (*de) {
			const char *name = (*de)->d_name;
			if (name[0] != '.' || (name[1] != '\0' && (name[1] != '.' || name[2] != '\0'))) {
				return 0;
			}
		} else if (errno != 0) {
			return -1;
		} else {
			return 0;
		}
	}
}

char *xreadlinkat(int fd, const char *path, size_t size) {
	++size; // NUL-terminator
	ssize_t len;
	char *name = NULL;

	while (true) {
		char *new_name = realloc(name, size);
		if (!new_name) {
			goto error;
		}
		name = new_name;

		len = readlinkat(fd, path, name, size);
		if (len < 0) {
			goto error;
		} else if (len >= size) {
			size *= 2;
		} else {
			break;
		}
	}

	name[len] = '\0';
	return name;

error:
	free(name);
	return NULL;
}

bool isopen(int fd) {
	return fcntl(fd, F_GETFD) >= 0 || errno != EBADF;
}

int redirect(int fd, const char *path, int flags, ...) {
	mode_t mode = 0;
	if (flags & O_CREAT) {
		va_list args;
		va_start(args, flags);

		// Use int rather than mode_t, because va_arg must receive a
		// fully-promoted type
		mode = va_arg(args, int);

		va_end(args);
	}

	int ret = open(path, flags, mode);

	if (ret >= 0 && ret != fd) {
		int orig = ret;
		ret = dup2(orig, fd);
		close(orig);
	}

	return ret;
}

int dup_cloexec(int fd) {
#ifdef F_DUPFD_CLOEXEC
	return fcntl(fd, F_DUPFD_CLOEXEC, 0);
#else
	int ret = dup(fd);
	if (ret < 0) {
		return -1;
	}

	if (fcntl(ret, F_SETFD, FD_CLOEXEC) == -1) {
		close(ret);
		return -1;
	}

	return ret;
#endif
}

int pipe_cloexec(int pipefd[2]) {
#if __linux__ || (BSD && !__APPLE__)
	return pipe2(pipefd, O_CLOEXEC);
#else
	if (pipe(pipefd) != 0) {
		return -1;
	}

	if (fcntl(pipefd[0], F_SETFD, FD_CLOEXEC) == -1 || fcntl(pipefd[1], F_SETFD, FD_CLOEXEC) == -1) {
		int error = errno;
		close(pipefd[1]);
		close(pipefd[0]);
		errno = error;
		return -1;
	}

	return 0;
#endif
}

char *xregerror(int err, const regex_t *regex) {
	size_t len = regerror(err, regex, NULL, 0);
	char *str = malloc(len);
	if (str) {
		regerror(err, regex, str, len);
	}
	return str;
}

int xlocaltime(const time_t *timep, struct tm *result) {
	// Should be called before localtime_r() according to POSIX.1-2004
	tzset();

	if (localtime_r(timep, result)) {
		return 0;
	} else {
		return -1;
	}
}

void format_mode(mode_t mode, char str[11]) {
	strcpy(str, "----------");

	switch (bftw_mode_typeflag(mode)) {
	case BFTW_BLK:
		str[0] = 'b';
		break;
	case BFTW_CHR:
		str[0] = 'c';
		break;
	case BFTW_DIR:
		str[0] = 'd';
		break;
	case BFTW_DOOR:
		str[0] = 'D';
		break;
	case BFTW_FIFO:
		str[0] = 'p';
		break;
	case BFTW_LNK:
		str[0] = 'l';
		break;
	case BFTW_SOCK:
		str[0] = 's';
		break;
	default:
		break;
	}

	if (mode & 00400) {
		str[1] = 'r';
	}
	if (mode & 00200) {
		str[2] = 'w';
	}
	if ((mode & 04100) == 04000) {
		str[3] = 'S';
	} else if (mode & 04000) {
		str[3] = 's';
	} else if (mode & 00100) {
		str[3] = 'x';
	}

	if (mode & 00040) {
		str[4] = 'r';
	}
	if (mode & 00020) {
		str[5] = 'w';
	}
	if ((mode & 02010) == 02000) {
		str[6] = 'S';
	} else if (mode & 02000) {
		str[6] = 's';
	} else if (mode & 00010) {
		str[6] = 'x';
	}

	if (mode & 00004) {
		str[7] = 'r';
	}
	if (mode & 00002) {
		str[8] = 'w';
	}
	if ((mode & 01001) == 01000) {
		str[9] = 'T';
	} else if (mode & 01000) {
		str[9] = 't';
	} else if (mode & 00001) {
		str[9] = 'x';
	}
}

const char *xbasename(const char *path) {
	const char *i;

	// Skip trailing slashes
	for (i = path + strlen(path); i > path && i[-1] == '/'; --i);

	// Find the beginning of the name
	for (; i > path && i[-1] != '/'; --i);

	// Skip leading slashes
	for (; i[0] == '/' && i[1]; ++i);

	return i;
}

int xfaccessat(int fd, const char *path, int amode) {
	int ret = faccessat(fd, path, amode, 0);

#ifdef AT_EACCESS
	// Some platforms, like Hurd, only support AT_EACCESS.  Other platforms,
	// like Android, don't support AT_EACCESS at all.
	if (ret != 0 && (errno == EINVAL || errno == ENOTSUP)) {
		ret = faccessat(fd, path, amode, AT_EACCESS);
	}
#endif

	return ret;
}

bool is_nonexistence_error(int error) {
	return error == ENOENT || errno == ENOTDIR;
}

/** Read a line from standard input. */
static char *xgetline(void) {
	char *line = dstralloc(0);
	if (!line) {
		return NULL;
	}

	while (true) {
		int c = getchar();
		if (c == '\n' || c == EOF) {
			break;
		}

		if (dstrapp(&line, c) != 0) {
			goto error;
		}
	}

	return line;

error:
	dstrfree(line);
	return NULL;
}

/** Compile and execute a regular expression for xrpmatch(). */
static int xrpregex(nl_item item, const char *response) {
	const char *pattern = nl_langinfo(item);
	if (!pattern) {
		return REG_BADPAT;
	}

	regex_t regex;
	int ret = regcomp(&regex, pattern, REG_EXTENDED);
	if (ret != 0) {
		return ret;
	}

	ret = regexec(&regex, response, 0, NULL, 0);
	regfree(&regex);
	return ret;
}

/** Check if a response is affirmative or negative. */
static int xrpmatch(const char *response) {
	int ret = xrpregex(NOEXPR, response);
	if (ret == 0) {
		return 0;
	} else if (ret != REG_NOMATCH) {
		return -1;
	}

	ret = xrpregex(YESEXPR, response);
	if (ret == 0) {
		return 1;
	} else if (ret != REG_NOMATCH) {
		return -1;
	}

	// Failsafe: always handle y/n
	char c = response[0];
	if (c == 'n' || c == 'N') {
		return 0;
	} else if (c == 'y' || c == 'Y') {
		return 1;
	} else {
		return -1;
	}
}

int ynprompt() {
	fflush(stderr);

	char *line = xgetline();
	int ret = line ? xrpmatch(line) : -1;
	dstrfree(line);
	return ret;
}

dev_t bfs_makedev(int ma, int mi) {
#ifdef makedev
	return makedev(ma, mi);
#else
	return (ma << 8) | mi;
#endif
}

int bfs_major(dev_t dev) {
#ifdef major
	return major(dev);
#else
	return dev >> 8;
#endif
}

int bfs_minor(dev_t dev) {
#ifdef minor
	return minor(dev);
#else
	return dev & 0xFF;
#endif
}
