from typing import override
from .args import Args


class GlobalArgs(Args):
	profiles: list[str]

	def __init__(self):
		super().__init__()
		self.profiles = []

	@override
	def parse_long(self, arg: str, argv: list[str], full_arg: str) -> list[str]:
		match arg:
			case "profile":
				if len(argv) == 0:
					raise self.InvalidException(f"Expected value for {full_arg}")

				self.profiles.append(argv[0])
				return argv[1:]

		return super().parse_long(arg, argv, full_arg)

	@override
	def parse_short(self, arg: str, value: str, argv: list[str], full_arg: str) -> list[str]:
		match arg:
			case "p":
				if value == "":
					if len(argv) == 0:
						raise self.InvalidException(f"Expected value for {full_arg}")
					value = argv[0]
					argv = argv[1:]

				self.profiles.append(value)

				return argv

		return super().parse_short(arg, value, argv, full_arg)
