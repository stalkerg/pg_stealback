#ifndef PARSEXLOG_H
#define PARSEXLOG_H

#include "access/rmgr.h"
#include "access/xlog_internal.h"
#include "access/xlogreader.h"
#include "access/timeline.h"
#include "catalog/pg_control.h"
#include "catalog/storage_xlog.h"
#include "commands/dbcommands_xlog.h"

extern TimeLineHistoryEntry *timeline_history;
extern int timeline_num_entries;

extern void extractPageMap(const char *datadir, XLogRecPtr startpoint,
			   int tliIndex, XLogRecPtr endpoint);
extern void findLastCheckpoint(const char *datadir, XLogRecPtr searchptr,
				   int tliIndex,
				   XLogRecPtr *lastchkptrec, TimeLineID *lastchkpttli,
				   XLogRecPtr *lastchkptredo);
extern XLogRecPtr readOneRecord(const char *datadir, XLogRecPtr ptr,
			  int tliIndex);

#endif