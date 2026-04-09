from enum import Enum
import json
from pathlib import Path
import sys
from typing import TextIO, override
from .help_indent import help_indent
from ..args import Args
from ..global_args import GlobalArgs
from ..config import Config

def entry(argv: list[str]):
	class Mode(Enum):
		CLANGD_CFLAGS = 0 # compile_flags.txt for clangd
		CCLS=1 # .ccls

	class LspArgs(GlobalArgs):
		mode = Mode.CLANGD_CFLAGS
		out_file: str | None = None

		@override
		def parse_long(self, arg: str, argv: list[str], full_arg: str) -> list[str]:
			if arg == "clangd":
				self.mode = Mode.CLANGD_CFLAGS
				return argv
			elif arg == "ccls":
				self.mode = Mode.CCLS
				return argv
			else:
				return super().parse_long(arg, argv, full_arg)

		@override
		def parse_short(self, arg: str, value: str, argv: list[str], full_arg: str) -> list[str]:
			if arg == "o":
				if value == "":
					if len(argv) == 0:
						raise self.InvalidException(f"Expected value for argument {full_arg}")
					value = argv[0]
					argv = argv[1:]

				self.out_file = value

				return argv
			else:
				return super().parse_short(arg, value, argv, full_arg)

	args = LspArgs()
	try:
		args.parse(argv)
	except Args.InvalidException as e:
		print(e.args[0], file=sys.stderr)
		sys.exit(1)

	config = Config()
	config.load(profiles=(args.profiles + ["lsp"]))
	args.set_config(config)


	compiler_flags = config.make_flags() + [
		"-include", config.main_file
	]

	out_file = args.out_file
	match args.mode:
		case Mode.CLANGD_CFLAGS:
			out_file = out_file or "compile_flags.txt"
		case Mode.CCLS:
			compiler_flags = [config.compiler] + compiler_flags
			out_file = out_file or ".ccls"

	if out_file is None:
		raise RuntimeError()

	path = Path(out_file)
	path.parent.mkdir(parents=True, exist_ok=True)

	with path.open("w") as file:
		def write(s: str):
			file.write(s)

		write("\n".join(compiler_flags))

	print(f"-> {path} generated")


def show_help(argv: list[str], file: TextIO):
	idt = help_indent
	file.write(
		f"usage: {sys.argv[0]} lsp [OPTIONS]\n" +
		"options:\n"
	)
