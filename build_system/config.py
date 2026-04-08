from dataclasses import dataclass, field
import json
import sys
from typing import Any

@dataclass
class Config:
	project_name: str = "project"
	out_dir: str = "out"
	tests_dir: str = "tests"
	tests_suffix: str = ".c"
	tests_sub: str = "ZZ"
	out_binary: str = "project"
	main_file: str = "src/main.c"
	compiler: str = "cc"
	flags: list[str | tuple[str]] = field(default_factory=list)
	macros: dict[str, str] = field(default_factory=dict)
	std_include: str | None = None

	def merge(self, data: Any):
		if not isinstance(data, dict):
			raise RuntimeError("data is not dict")

		self.project_name = data.get("project_name", self.project_name)
		self.out_dir = data.get("out_dir", self.out_dir)
		self.main_file = data.get("main_file", self.main_file)
		self.out_binary = data.get("out_binary", self.out_binary)
		self.compiler = data.get("compiler", self.compiler)
		self.std_include = data.get("std_include", self.std_include)
		self.tests_dir = data.get("tests_dir", self.tests_dir)
		self.tests_suffix = data.get("tests_suffix", self.tests_suffix)
		self.tests_sub = data.get("tests_sub", self.tests_sub)

		flags = data.get("flags")
		if isinstance(flags, list):
			for flag in flags:
				if isinstance(flag, list):
					self.flags.append(tuple(flag))
				else:
					self.flags.append(str(flag))

		macros = data.get("macros")
		if isinstance(macros, dict):
			for k, v in macros.items():
				self.macros[k] = v

	def load(
		self,
		profiles: list[str] = [],
		configs: list[str] = ["project.json", "project_local.json"]
	):
		found_profiles: set[str] = set()

		for config in configs:
			try:
				with open(config, "r") as file:
					data = json.load(file)
					self.merge(data)
					for profile in profiles:
						profile_data = data.get(f"profile_{profile}")
						if profile_data is not None:
							found_profiles.add(profile)
							self.merge(profile_data)

			except FileNotFoundError:
				print(f"warning: cannot open config \"{config}\"", file=sys.stderr)
			except json.JSONDecodeError as e:
				print(f"failed to decode config file \"{config}\":\n   {e}", file=sys.stderr)
				sys.exit(1)

		for profile in (set(profiles) - found_profiles):
			print(f"warning: profile \"{profile}\" not found in config files", file=sys.stderr)


	def make_flags(self) -> list[str]:
		command = []

		for flag in self.flags:
			if isinstance(flag, tuple):
				command += list(flag)
			else:
				command.append(flag)

		for macro, value in self.macros.items():
			if value == "":
				command.append(f"-D{macro}")
			else:
				command.append(f"-D{macro}={value}")

		if self.std_include is not None:
			command += ["-I", self.std_include]

		return command
