import json
import sys
from typing import TextIO
from .help_indent import help_indent
from ..args import Args
from ..config import Config

def entry(argv: list[str]):
	class LspArgs(Args):
		pass

	args = LspArgs()
	try:
		args.parse(argv)
	except Args.InvalidException as e:
		print(e.args[0], file=sys.stderr)
		sys.exit(1)

	config = Config()

	with open("project.json", "r") as file:
		config.merge(json.load(file))

	file_path = "compile_flags.txt"
	compiler_flags = config.make_command_flags() + [
		"-xc-header",
		"-include",
		config.main_file
	]

	with open(file_path, "w") as file:
		def write(s: str):
			file.write(s)

		write("\n".join(compiler_flags))

	print(f"-> {file_path} generated")


def show_help(argv: list[str], file: TextIO):
	idt = help_indent
	file.write(
		f"usage: {sys.argv[0]} lsp [OPTIONS]\n" +
		"options:\n"
	)
