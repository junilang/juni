import json
import os
from pathlib import Path
import sys
from typing import TextIO, override

from ..config import Config
from ..global_args import GlobalArgs
from ..args import Args
from .help_indent import help_indent
from ..shell_writer import ShellWriter

def entry(argv: list[str]):
	class MakeArgs(GlobalArgs):
		pass

	args = MakeArgs()
	try:
		args.parse(argv)
	except Args.InvalidException as e:
		print(e.args[0], file=sys.stderr)
		sys.exit(1)

	config = Config()

	config.load(profiles=args.profiles)

	compiler_command = (
		[ config.compiler ] +
		config.make_flags() +
		[ "-o", f"{config.out_dir}/{config.out_binary}", config.main_file ]
	)

	makefile_path = Path(config.out_dir + "/make")
	makefile_path.parent.mkdir(parents=True, exist_ok=True)

	with makefile_path.open("w") as makefile:
		w = ShellWriter(makefile)
		w.write_shebang()
		w.write_command(compiler_command)
		w.make_executable()

		print(f"-> {makefile_path} generated")


def show_help(argv: list[str], file: TextIO):
	idt = help_indent
	print(
		f"usage: {sys.argv[0]} make [OPTIONS]\n" +
		"options:\n"
		, end="", file=file
	)
