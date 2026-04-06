from dataclasses import dataclass

@dataclass
class Args:
	class InvalidException(Exception):
		pass

	class StopParseException(Exception):
		rest: list[str]
		def __init__(self, rest: list[str]):
			self.rest = rest


	no_flag: bool = False

	def __init__(self):
		pass

	def unknown_argument(self, full_arg: str):
		raise self.InvalidException(f"unknown argument \"{full_arg}\"")

	def unexpected_positional(self, arg: str):
		raise self.InvalidException(f"unexpected positional argument \"{arg}\"")

	def parse_long(self, arg: str, argv: list[str], full_arg: str) -> list[str]:
		self.unknown_argument(full_arg)

	def parse_short(self, arg: str, value: str, argv: list[str], full_arg: str) -> list[str]:
		self.unknown_argument(full_arg)

	def parse_positional(self, arg: str, argv: list[str]) -> list[str]:
		self.unexpected_positional(arg)

	def parse_dispatch(self, arg: str, argv: list[str]) -> list[str]:
		arglen = len(arg)

		if self.no_flag or arglen == 0 or arg[0] != "-":
			return self.parse_positional(arg, argv)
		elif arglen == 1:
			self.no_flag = True
		elif arg[1] != "-":
			return self.parse_short(arg[1], arg[2:], argv, arg)
		elif arglen == 2:
			self.no_flag = True
		else:
			return self.parse_long(arg[2:], argv, arg)

		return argv

	def parse(self, argv: list[str]) -> list[str]:
		try:
			while argv:
				argv = self.parse_dispatch(argv[0], argv[1:])

			return []

		except self.StopParseException as e:
			return e.rest
