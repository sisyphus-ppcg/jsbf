import pyfuzzilli

pyfuzzilli.initialize(
	"../v8/out/fuzzbuild/d8",
	[
		"--allow-natives-syntax",
		"--no-fuzzilli-enable-builtins-coverage",
		"--fuzzing",
		"--no-verify-simplified-lowering"
	]
)

status, code = pyfuzzilli.execute(b"""
if (1 + 2 + 3 == 5) {
	fuzzilli("FUZZILLI_CRASH", 0);
}
""")
if status == pyfuzzilli.ExitType.TIMEOUT:
	print('timeout')
elif status == pyfuzzilli.ExitType.SIGNAL:
	print('signal')
elif status == pyfuzzilli.ExitType.CODE:
	print('code', code)