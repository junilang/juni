import json
import os
import sys
from typing import TextIO, override

from ..config import Config
from ..args import Args
from .help_indent import help_indent
from ..util.chmod import make_executable

def entry(argv: list[str]):
	class MakeArgs(Args):
		pass

	args = MakeArgs()
	try:
		args.parse(argv)
	except Args.InvalidException as e:
		print(e.args[0], file=sys.stderr)
		sys.exit(1)

	config = Config()

	with open("project.json", "r") as file:
		config.merge(json.load(file))

	makefile_path = config.out_dir + "/make"
	compiler_command = config.make_command()

	try:
		os.mkdir(config.out_dir)
	except Exception:
		pass

	with open(makefile_path, "w") as makefile:
		def write(s: str):
			makefile.write(s)

		write("#!/bin/sh\n\n")

		write(f"echo + {" ".join(compiler_command)}\n\n")
		write(" \\\n".join(compiler_command))

		make_executable(makefile.fileno())

	print(f"-> {makefile_path} generated")


def show_help(argv: list[str], file: TextIO):
	idt = help_indent
	print(
		f"usage: {sys.argv[0]} make [OPTIONS]\n" +
		"options:\n"
		, end="", file=file
	)
