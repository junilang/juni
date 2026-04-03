import sys
from typing import TextIO
from ..args import Args
from .help_indent import *

def entry(argv: list[str]):
	class MakeArgs(Args):
		pass

	args = MakeArgs()
	try:
		args.parse(argv)
	except Args.InvalidException as e:
		print(e.args[0], file=sys.stderr)
		sys.exit(1)


def show_help(argv: list[str], file: TextIO):
	idt = help_indent
	print(
		f"usage: {sys.argv[0]} make [OPTIONS]\n" +
		"options:\n"
		, end="", file=file
	)
