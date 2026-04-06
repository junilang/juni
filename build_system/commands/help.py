import sys
from .make import show_help as help_make
from .lsp import show_help as help_lsp
from .help_indent import help_indent

def entry(argv: list[str]):
	if len(argv) != 0:
		match argv[0]:
			case "make":
				help_make(argv[1:], file=sys.stdout)
				return
			case "lsp":
				help_lsp(argv[1:], file=sys.stdout)
				return

	idt = help_indent
	print(
		f"usage: {sys.argv[0]} COMMAND [OPTIONS]\n" +
		f"       {sys.argv[0]} help [COMMAND]\n" +
		"available commands:\n" +
		f"{idt}make    - generate makefile\n" +
		f"{idt}lsp     - generate flags for lsp server\n" +
		f"{idt}help    - show help\n"
		, end="", file=sys.stdout
	)
