/* pwsafe2john processes input Password Safe files into a format suitable
 * for use with JtR.
 *
 * This software is Copyright © 2012, Dhiru Kholia <dhiru.kholia at gmail.com>,
 * and it is hereby released to the general public under the following terms:
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted.
 *
 * Password Safe file format:
 *
 * 1. http://keybox.rubyforge.org/password-safe-db-format.html
 *
 * 2. formatV3.txt at http://passwordsafe.svn.sourceforge.net/viewvc/passwordsafe/trunk/pwsafe/pwsafe/docs/
 *
 * Output Format: filename:$passwordsaf$*version*salt*iterations*hash */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

static char *magic = "PWS3";

/* helper functions for byte order conversions, header values are stored
 * in little-endian byte order */
static uint32_t fget32(FILE * fp)
{
	uint32_t v = fgetc(fp);
	v |= fgetc(fp) << 8;
	v |= fgetc(fp) << 16;
	v |= fgetc(fp) << 24;
	return v;
}


static void print_hex(unsigned char *str, int len)
{
	int i;
	for (i = 0; i < len; ++i)
		printf("%02x", str[i]);
}

static void process_file(const char *filename)
{
	FILE *fp;
	unsigned char buf[32];
	unsigned int iterations;

	if (!(fp = fopen(filename, "rb"))) {
		fprintf(stderr, "! %s: %s\n", filename, strerror(errno));
		return;
	}
	fread(buf, 4, 1, fp);
	if(memcmp(buf, magic, 4)) {
		fprintf(stderr, "%s : Couldn't find PWS3 magic string. Is this a Password Safe file?\n", filename);
		exit(1);
	}
	fread(buf, 32, 1, fp);
	iterations = fget32(fp);

	printf("%s:$passwordsafe$*3*", filename);
	print_hex(buf, 32);
	printf("*%d*", iterations);
	fread(buf, 32, 1, fp);
	print_hex(buf,32);
	printf("\n");

	fclose(fp);
}

int main(int argc, char **argv)
{
	int i;

	if (argc < 2) {
		puts("Usage: pwsafe2john [.psafe3 files]");
		return 0;
	}
	for (i = 1; i < argc; i++)
		process_file(argv[i]);

	return 0;
}