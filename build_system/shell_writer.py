import os
import stat

from typing import TextIO

class ShellWriter:
	file: TextIO
	echo_commands: bool

	def __init__(self, file: TextIO, echo_commands = True):
		self.file = file
		self.echo_commands = echo_commands

	def write_shebang(self, command="/bin/sh"):
		self.file.write(f"#!{command}\n\n")

	def write_block(self, block: str):
		self.file.write(block + "\n\n")

	def write_command(self, command: list[str], echo: bool | None = None, quote=True, oneline=False):
		if echo is None:
			echo = self.echo_commands

		if echo:
			def echo_quote(s):
				return s.replace('"', '\\"')

			self.file.write(
				f'echo + "{echo_quote(" ".join(command))}"\n\n'
			)

		if quote:
			def do_quote(s):
				s = s.replace('"', '\\"')
				return '"' + s + '"'

			command = [do_quote(s) for s in command]

		command_str = None

		if oneline:
			command_str = " ".join(command)
		else:
			command_str = " \\\n\t".join(command)

		self.file.write(command_str + "\n\n")

	def make_executable(self):
		fd = self.file.fileno()
		if not hasattr(os, "fchmod"):
			return

		mode = os.fstat(fd).st_mode
		os.fchmod(fd, mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)
