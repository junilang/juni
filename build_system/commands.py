import sys
from .args import Args

help_indent = "    "

def cmd_make(argv: list[str]):
	class MakeArgs(Args):
		pass

	args = MakeArgs()
	try:
		args.parse(argv)
	except Args.InvalidException as e:
		print(e.args[0], file=sys.stderr)
		sys.exit(1)


def cmd_help_make(argv: list[str]):
	idt = help_indent
	print(
		f"usage: {sys.argv[0]} make [OPTIONS]\n" +
		"options:\n"
		, end=""
	)

def cmd_help(argv: list[str]):
	if len(argv) != 0:
		match argv[0]:
			case "make":
				cmd_help_make(argv[1:])
				return

	idt = help_indent
	print(
		f"usage: {sys.argv[0]} COMMAND [OPTIONS]\n" +
		f"       {sys.argv[0]} help [COMMAND]\n" +
		"available commands:\n" +
		f"{idt}make    - generate makefile\n" +
		f"{idt}help    - show help\n"
		, end=""
	)
