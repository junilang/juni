from dataclasses import dataclass, field
from typing import Any

@dataclass
class Config:
	project_name: str = "project"
	out_dir: str = "out"
	out_binary: str = "project"
	main_file: str = "src/main.cc"
	compiler_binary: str = "cc"
	compiler_flags: list[str | tuple[str]] = field(default_factory=list)
	compiler_macros: dict[str, str] = field(default_factory=dict)
	compiler_std_include: str | None = None

	def merge_compiler(self, data: dict):
		self.compiler_binary = data.get("binary", self.compiler_binary)
		self.compiler_std_include = data.get("std_include", self.compiler_std_include)

		flags = data.get("flags")
		if isinstance(flags, list):
			for flag in flags:
				if isinstance(flag, list):
					self.compiler_flags.append(tuple(flag))
				else:
					self.compiler_flags.append(str(flag))

		macros = data.get("macros")
		if isinstance(macros, dict):
			for k, v in macros.items():
				self.compiler_macros[k] = v

	def merge(self, data: Any):
		if not isinstance(data, dict):
			raise RuntimeError("data is not dict")

		self.project_name = data.get("project_name", self.project_name)
		self.out_dir = data.get("out_dir", self.out_dir)
		self.main_file = data.get("main_file", self.main_file)
		self.out_binary = data.get("out_binary", self.out_binary)

		compiler = data.get("compiler")
		if isinstance(compiler, dict):
			self.merge_compiler(compiler)

	def make_command_flags(self) -> list[str]:
		command = []

		for flag in self.compiler_flags:
			if isinstance(flag, tuple):
				command += list(flag)
			else:
				command.append(flag)

		for macro in self.compiler_macros:
			command.append(f"-D{macro}={self.compiler_macros[macro]}")

		if self.compiler_std_include is not None:
			command += ["-I", self.compiler_std_include]

		return command

	def make_command(self) -> list[str]:
		command = [self.compiler_binary]
		command += self.make_command_flags()
		command += [self.main_file, "-o", f"{self.out_dir}/{self.out_binary}"]

		return command
