import os
import stat

def make_executable(fd: int):
	if not hasattr(os, "fchmod"):
		return

	mode = os.fstat(fd).st_mode
	os.fchmod(fd, mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)
