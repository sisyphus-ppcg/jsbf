from . import _pyfuzzilli
import enum

__all__ = ['initialize', 'execute', 'ExitType']

class ExitType(enum.Enum):
	CODE = 0
	SIGNAL = 1
	TIMEOUT = 2

def initialize(path, flags=[]):
	return _pyfuzzilli.initialize(path, flags)

def execute(s):
	code = _pyfuzzilli.execute(s)
	if _code_timeout(code):
		return (ExitType.TIMEOUT, None)
	elif _code_signalled(code):
		return (ExitType.SIGNAL, _code_get_signal(code))
	else:
		return (ExitType.CODE, _code_get_exit_code(code))

def _code_signalled(code):
	return code & 0xff != 0

def _code_get_signal(code):
	return code & 0xFF

def _code_get_exit_code(code):
	return (code >> 8) & 0xFF

def _code_timeout(code):
	return code & 0xff0000 != 0