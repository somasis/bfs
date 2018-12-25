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

#include "color.h"
#include "bftw.h"
#include "stat.h"
#include "util.h"
#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

struct ext_color {
	const char *ext;
	size_t len;

	const char *color;

	struct ext_color *next;
};

struct colors {
	const char *reset;
	const char *bold;
	const char *gray;
	const char *red;
	const char *green;
	const char *yellow;
	const char *blue;
	const char *magenta;
	const char *cyan;

	const char *normal;
	const char *file;
	const char *dir;
	const char *link;
	const char *multi_hard;
	const char *pipe;
	const char *door;
	const char *block;
	const char *chardev;
	const char *orphan;
	const char *missing;
	const char *socket;
	const char *setuid;
	const char *setgid;
	const char *capable;
	const char *sticky_ow;
	const char *ow;
	const char *sticky;
	const char *exec;

	const char *warning;
	const char *error;

	struct ext_color *ext_list;

	char *data;
};

struct color_name {
	const char *name;
	size_t offset;
};

#define COLOR_NAME(name, field) {name, offsetof(struct colors, field)}

static const struct color_name color_names[] = {
	COLOR_NAME("bd", block),
	COLOR_NAME("bld", bold),
	COLOR_NAME("blu", blue),
	COLOR_NAME("ca", capable),
	COLOR_NAME("cd", chardev),
	COLOR_NAME("cyn", cyan),
	COLOR_NAME("di", dir),
	COLOR_NAME("do", door),
	COLOR_NAME("er", error),
	COLOR_NAME("ex", exec),
	COLOR_NAME("fi", file),
	COLOR_NAME("grn", green),
	COLOR_NAME("gry", gray),
	COLOR_NAME("ln", link),
	COLOR_NAME("mag", magenta),
	COLOR_NAME("mh", multi_hard),
	COLOR_NAME("mi", missing),
	COLOR_NAME("no", normal),
	COLOR_NAME("or", orphan),
	COLOR_NAME("ow", ow),
	COLOR_NAME("pi", pipe),
	COLOR_NAME("red", red),
	COLOR_NAME("rs", reset),
	COLOR_NAME("sg", setgid),
	COLOR_NAME("so", socket),
	COLOR_NAME("st", sticky),
	COLOR_NAME("su", setuid),
	COLOR_NAME("tw", sticky_ow),
	COLOR_NAME("wr", warning),
	COLOR_NAME("ylw", yellow),
	{0},
};

static const char **get_color(const struct colors *colors, const char *name) {
	for (const struct color_name *entry = color_names; entry->name; ++entry) {
		if (strcmp(name, entry->name) == 0) {
			return (const char **)((char *)colors + entry->offset);
		}
	}

	return NULL;
}

static void set_color(struct colors *colors, const char *name, const char *value) {
	const char **color = get_color(colors, name);
	if (color) {
		*color = value;
	}
}

struct colors *parse_colors(const char *ls_colors) {
	struct colors *colors = malloc(sizeof(struct colors));
	if (!colors) {
		goto done;
	}

	// From man console_codes
	colors->reset   = "0";
	colors->bold    = "01";
	colors->gray    = "01;30";
	colors->red     = "01;31";
	colors->green   = "01;32";
	colors->yellow  = "01;33";
	colors->blue    = "01;34";
	colors->magenta = "01;35";
	colors->cyan    = "01;36";

	// Defaults generated by dircolors --print-database
	colors->normal     = NULL;
	colors->file       = NULL;
	colors->dir        = "01;34";
	colors->link       = "01;36";
	colors->multi_hard = NULL;
	colors->pipe       = "40;33";
	colors->socket     = "01;35";
	colors->door       = "01;35";
	colors->block      = "40;33;01";
	colors->chardev    = "40;33;01";
	colors->orphan     = "40;31;01";
	colors->setuid     = "37;41";
	colors->setgid     = "30;43";
	colors->capable    = "30;41";
	colors->sticky_ow  = "30;42";
	colors->ow         = "34;42";
	colors->sticky     = "37;44";
	colors->exec       = "01;32";
	colors->warning    = "40;33;01";
	colors->error      = "40;31;01";
	colors->ext_list   = NULL;
	colors->data       = NULL;

	if (ls_colors) {
		colors->data = strdup(ls_colors);
	}

	if (!colors->data) {
		goto done;
	}

	char *start = colors->data;
	char *end;
	struct ext_color *ext;
	for (end = strchr(start, ':'); *start && end; start = end + 1, end = strchr(start, ':')) {
		char *equals = strchr(start, '=');
		if (!equals) {
			continue;
		}

		*equals = '\0';
		*end = '\0';

		const char *key = start;
		const char *value = equals + 1;

		// Ignore all-zero values
		if (strspn(value, "0") == strlen(value)) {
			continue;
		}

		if (key[0] == '*') {
			ext = malloc(sizeof(struct ext_color));
			if (ext) {
				ext->ext = key + 1;
				ext->len = strlen(ext->ext);
				ext->color = value;
				ext->next = colors->ext_list;
				colors->ext_list = ext;
			}
		} else {
			set_color(colors, key, value);
		}
	}

done:
	return colors;
}

void free_colors(struct colors *colors) {
	if (colors) {
		struct ext_color *ext = colors->ext_list;
		while (ext) {
			struct ext_color *saved = ext;
			ext = ext->next;
			free(saved);
		}

		free(colors->data);
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

static const char *ext_color(const struct colors *colors, const char *filename) {
	size_t namelen = strlen(filename);

	for (const struct ext_color *ext = colors->ext_list; ext; ext = ext->next) {
		if (namelen < ext->len) {
			continue;
		}

		const char *suffix = filename + namelen - ext->len;
		if (strcasecmp(suffix, ext->ext) == 0) {
			return ext->color;
		}
	}

	return NULL;
}

static const char *file_color(const struct colors *colors, const char *filename, const struct BFTW *ftwbuf) {
	const struct bfs_stat *sb = ftwbuf->statbuf;
	if (!sb) {
		return colors->orphan;
	}

	const char *color = NULL;

	switch (sb->mode & S_IFMT) {
	case S_IFREG:
		if (sb->mode & S_ISUID) {
			color = colors->setuid;
		} else if (sb->mode & S_ISGID) {
			color = colors->setgid;
		} else if (bfs_check_capabilities(ftwbuf)) {
			color = colors->capable;
		} else if (sb->mode & 0111) {
			color = colors->exec;
		}

		if (!color && sb->nlink > 1) {
			color = colors->multi_hard;
		}

		if (!color) {
			color = ext_color(colors, filename);
		}

		if (!color) {
			color = colors->file;
		}

		break;

	case S_IFDIR:
		if (sb->mode & S_ISVTX) {
			if (sb->mode & S_IWOTH) {
				color = colors->sticky_ow;
			} else {
				color = colors->sticky;
			}
		} else if (sb->mode & S_IWOTH) {
			color = colors->ow;
		}

		if (!color) {
			color = colors->dir;
		}

		break;

	case S_IFLNK:
		if (xfaccessat(ftwbuf->at_fd, ftwbuf->at_path, F_OK) == 0) {
			color = colors->link;
		} else {
			color = colors->orphan;
		}
		break;

	case S_IFBLK:
		color = colors->block;
		break;
	case S_IFCHR:
		color = colors->chardev;
		break;
	case S_IFIFO:
		color = colors->pipe;
		break;
	case S_IFSOCK:
		color = colors->socket;
		break;

#ifdef S_IFDOOR
	case S_IFDOOR:
		color = colors->door;
		break;
#endif
	}

	if (!color) {
		color = colors->normal;
	}

	return color;
}

static int print_esc(const char *esc, FILE *file) {
	if (fputs("\033[", file) == EOF) {
		return -1;
	}
	if (fputs(esc, file) == EOF) {
		return -1;
	}
	if (fputs("m", file) == EOF) {
		return -1;
	}

	return 0;
}

static int print_colored(const struct colors *colors, const char *esc, const char *str, size_t len, FILE *file) {
	if (esc) {
		if (print_esc(esc, file) != 0) {
			return -1;
		}
	}
	if (fwrite(str, 1, len, file) != len) {
		return -1;
	}
	if (esc) {
		if (print_esc(colors->reset, file) != 0) {
			return -1;
		}
	}

	return 0;
}

static int print_path(CFILE *cfile, const struct BFTW *ftwbuf) {
	const struct colors *colors = cfile->colors;
	FILE *file = cfile->file;
	const char *path = ftwbuf->path;

	if (!colors) {
		return fputs(path, file) == EOF ? -1 : 0;
	}

	if (ftwbuf->nameoff > 0) {
		if (print_colored(colors, colors->dir, path, ftwbuf->nameoff, file) != 0) {
			return -1;
		}
	}

	const char *filename = path + ftwbuf->nameoff;
	const char *color = file_color(colors, filename, ftwbuf);
	if (print_colored(colors, color, filename, strlen(filename), file) != 0) {
		return -1;
	}

	return 0;
}

static int print_link(CFILE *cfile, const struct BFTW *ftwbuf) {
	int ret = -1;

	char *target = xreadlinkat(ftwbuf->at_fd, ftwbuf->at_path, 0);
	if (!target) {
		goto done;
	}

	struct BFTW altbuf = *ftwbuf;
	altbuf.path = target;
	altbuf.nameoff = xbasename(target) - target;

	struct bfs_stat statbuf;
	if (bfs_stat(ftwbuf->at_fd, ftwbuf->at_path, 0, 0, &statbuf) == 0) {
		altbuf.statbuf = &statbuf;
	} else {
		altbuf.statbuf = NULL;
	}

	ret = print_path(cfile, &altbuf);

done:
	free(target);
	return ret;
}

int cfprintf(CFILE *cfile, const char *format, ...) {
	const struct colors *colors = cfile->colors;
	FILE *file = cfile->file;

	int ret = -1;
	int error = errno;

	va_list args;
	va_start(args, format);

	for (const char *i = format; *i; ++i) {
		const char *percent = strchr(i, '%');
		if (!percent) {
			if (fputs(i, file) == EOF) {
				goto done;
			}
			break;
		}

		size_t len = percent - i;
		if (fwrite(i, 1, len, file) != len) {
			goto done;
		}

		i = percent + 1;
		switch (*i) {
		case '%':
			if (fputc('%', file) == EOF) {
				goto done;
			}
			break;

		case 'c':
			if (fputc(va_arg(args, int), file) == EOF) {
				goto done;
			}
			break;

		case 'd':
			if (fprintf(file, "%d", va_arg(args, int)) < 0) {
				goto done;
			}
			break;

		case 'g':
			if (fprintf(file, "%g", va_arg(args, double)) < 0) {
				goto done;
			}
			break;

		case 's':
			if (fputs(va_arg(args, const char *), file) == EOF) {
				goto done;
			}
			break;

		case 'z':
			++i;
			if (*i != 'u') {
				goto invalid;
			}
			if (fprintf(file, "%zu", va_arg(args, size_t)) < 0) {
				goto done;
			}
			break;

		case 'm':
			if (fputs(strerror(error), file) == EOF) {
				goto done;
			}
			break;

		case 'P':
			if (print_path(cfile, va_arg(args, const struct BFTW *)) != 0) {
				goto done;
			}
			break;

		case 'L':
			if (print_link(cfile, va_arg(args, const struct BFTW *)) != 0) {
				goto done;
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

			const char **esc = get_color(colors, name);
			if (!esc) {
				goto invalid;
			}
			if (*esc) {
				if (print_esc(*esc, file) != 0) {
					goto done;
				}
			}

			i = end;
			break;
		}

		default:
		invalid:
			assert(false);
			errno = EINVAL;
			goto done;
		}
	}

	ret = 0;

done:
	va_end(args);
	return ret;
}
