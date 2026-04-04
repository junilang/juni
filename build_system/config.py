from dataclasses import dataclass, field
from typing import Any

@dataclass
class Config:
	project_name: str = "project"
	out_dir: str = "out"
	out_binary: str = "project"
	main_file: str = "src/main.cc"
	compiler_binary: str = "cc"
	compiler_flags: list[str] = field(default_factory=list)
	compiler_macros: dict[str, str] = field(default_factory=dict)

	def merge_compiler(self, data: dict):
		self.compiler_binary = data.get("binary", self.compiler_binary)

		flags = data.get("flags")
		if isinstance(flags, list):
			self.compiler_flags += flags

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
