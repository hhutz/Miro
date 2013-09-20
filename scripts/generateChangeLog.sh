#!/bin/bash
#
# automatically generates the ChangeLog-file from the
# CVS log entries
#
# be careful, this overrides your old ChangeLog-file without
# any comment
#

cd $MIRO_ROOT
rcs2log > ChangeLog

