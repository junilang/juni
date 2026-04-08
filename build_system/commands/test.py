import sys
from typing import override
from ..global_args import GlobalArgs
from ..args import Args
from ..config import Config
from ..shell_writer import ShellWriter

from pathlib import Path

def entry(argv: list[str]):
	class TestArgs(GlobalArgs):
		default_dir: list[str]
		current_dir: list[str]
		dirs: dict[str, list[str]]

		def __init__(self):
			super().__init__()
			self.default_dir = []
			self.current_dir = self.default_dir
			self.dirs = dict()

		@override
		def parse_short(self, arg: str, value: str, argv: list[str], full_arg: str) -> list[str]:
			if arg == "i":
				if value == "":
					if len(argv) == 0:
						raise self.InvalidException(f"Expected value for \"{full_arg}\"")
					value = argv[0]
					argv = argv[1:]

				self.current_dir = []
				self.dirs[value] = self.current_dir
				return argv

			return super().parse_short(arg, value, argv, full_arg)

		@override
		def parse_positional(self, arg: str, argv: list[str]) -> list[str]:
			self.current_dir.append(arg)
			return argv

	args = TestArgs()
	try:
		args.parse(argv)
	except Args.InvalidException as e:
		print(e.args[0], file=sys.stderr)
		sys.exit(1)

	config = Config()
	config.load(profiles=args.profiles + ["testing"])

	test_files: list[str] = []

	def process_globs(path: str, globs: list[str]):
		if len(globs) == 0:
			globs = [""]

		root = Path(path)
		for file in root.rglob("*"):
			if not file.is_file():
				continue

			file_path = str(file).removeprefix(f"{path}/")

			if not file_path.endswith(config.tests_suffix):
				continue
			for glob in globs:
				if file_path.startswith(glob):
					break
			else:
				continue

			test_files.append(f"{path}/{file_path}")

	if len(args.default_dir) != 0 or len(args.dirs) == 0:
		process_globs(config.tests_dir, args.default_dir)

	for dir, globs in args.dirs.items():
		process_globs(dir, globs)

	def make_test_id(file: str) -> str:
		return file \
			.removesuffix(config.tests_suffix) \
			.replace(".", "_") \
			.replace("/", "_")

	test_ids = [make_test_id(test) for test in test_files]

	makefile_path = Path(config.out_dir + "/make")
	makefile_path.parent.mkdir(parents=True, exist_ok=True)

	gen_file = config.out_dir + "/tests_gen"

	compiler_command = (
		[config.compiler] +
		config.make_flags() +
		[
			f'-DBUILD_TESTING_INCLUDE=\"{Path(gen_file).absolute()}\"',
			"-o", f"{config.out_dir}/{config.out_binary}",
			config.main_file
		]
	)

	with makefile_path.open("w") as makefile:
		w = ShellWriter(makefile)
		w.write_shebang()

		w.write_block(
			"gen_test() {\n" +
			f'\t"{config.compiler}" -E "$1" \\\n' +
			f'\t\t| sed -e "s|{config.tests_sub}|{config.tests_sub}$2\\_|g"\n' +
			"}"
		)

		w.write_command(
			["echo", ">", f'"{gen_file}"'],
			oneline = True,
			quote = False,
			echo = False
		)

		for test_file, test_id in zip(test_files, test_ids):
			w.write_block(
				f"echo generating test \"{test_file}\"\n" +
				f"gen_test \"{test_file}\" \"{test_id}\" >> \"{gen_file}\"",
			)

		w.write_block(f"echo TEST_SUITE_BEGIN >> \"{gen_file}\"")

		for test_id in test_ids:
			w.write_block(f"echo \"TEST_RUN({config.tests_sub}{test_id})\" >> \"{gen_file}\"")

		w.write_block(f"echo TEST_SUITE_END >> \"{gen_file}\"")

		w.write_command(compiler_command)

		w.make_executable()
		print(f"-> {makefile_path} generated")
