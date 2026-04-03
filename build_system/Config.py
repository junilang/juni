from dataclasses import dataclass

@dataclass
class Config:
	compiler_flags: list[str]
	compiler_macros: list[str]
