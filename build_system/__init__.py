import sys
from . import commands

def main():
	argv = sys.argv[1:]

	if len(argv) != 0:
		match argv[0]:
			case "make":
				commands.cmd_make(argv[1:])
			case "help":
				commands.cmd_help(argv[1:])
			case "lsp":
				commands.cmd_lsp(argv[1:])
			case _:
				commands.cmd_make(argv)
	else:
		commands.cmd_make(argv)
