import distutils.core

distutils.core.setup(
	name = "pyfuzzilli",
	ext_modules = [
		distutils.core.Extension("_pyfuzzilli", sources=['pyfuzzilli.c', 'third_party/libreprl.c'], libraries=['rt'],)
	],
	description="pyfuzzilli",
)