/*-------------------------------------------------------------------------
 *
 * controlfile.c: operations for control file
 *
 * Copyright (c) 2009-2015, NIPPON TELEGRAPH AND TELEPHONE CORPORATION
 *
 *-------------------------------------------------------------------------
 */

#include "postgres_fe.h"
#include "catalog/pg_control.h"
#include "common/fe_memutils.h"

#include <unistd.h>

#include "pg_arman.h"

/*
 * Reads control file into a pg_malloc()'d buffer and returns a pointer to it.
 * To be used by more special-purpose routines such as get_current_timeline()
 * and get_data_checksum_version()
 *
 * NOTE: The special-purpose routines need to pg_free()/free() the block of
 * memory allocated here once they are done using the control file info
 *
 */
char *
read_control_file()
{
	char 		*buffer;
	int			fd;
	char		ControlFilePath[MAXPGPATH];
	pg_crc32c	crc;

	snprintf(ControlFilePath, MAXPGPATH, "%s/global/pg_control", pgdata);

	if ((fd = open(ControlFilePath, O_RDONLY | PG_BINARY, 0)) == -1)
	{
		elog(WARNING, _("could not open pg_controldata file \"%s\": %s"),
		ControlFilePath, strerror(errno));
		return NULL;
	}

	buffer = (char *) pg_malloc(PG_CONTROL_SIZE);

	if (read(fd, buffer, PG_CONTROL_SIZE) != PG_CONTROL_SIZE)
	{
		elog(WARNING, _("could not read pg_controldata file \"%s\": %s"),
		ControlFilePath, strerror(errno));
		return NULL;
	}

	close(fd);

	/* Check the CRC. */
	INIT_CRC32C(crc);
	COMP_CRC32C(crc,
			buffer,
			offsetof(ControlFileData, crc));
	FIN_CRC32C(crc);

	if (!EQ_CRC32C(crc, ((ControlFileData *) buffer)->crc))
	{
		elog(WARNING, "Calculated CRC checksum does not match value stored in file.");
	}

	if (((ControlFileData *) buffer)->pg_control_version != PG_CONTROL_VERSION)
	{
		elog(WARNING, "The byte ordering used to store the pg_control file might not match the one "
				"used by this program.");
	}

	return buffer;
}
